#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <mpark/variant.hpp>

#include <dlisio/lis/pack.h>
#include <dlisio/lis/types.h>
#include <dlisio/lis/types.hpp>
#include <dlisio/lis/io.hpp>
#include <dlisio/lis/protocol.hpp>
#include <dlisio/exception.hpp>

#include "common.hpp"

namespace lis = dlisio::lis79;
namespace py = pybind11;
using namespace py::literals;

namespace pybind11 { namespace detail {

/*
 * Register boost::optional and mpark::variant type casters, since C++17 is not
 * a requirement yet, and auto-conversion from optional to None/object and
 * auto-variant-extraction is desired.
 *
 * https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html
 */

template < typename... T >
struct type_caster< mpark::variant< T... > > :
    variant_caster< mpark::variant< T... > > {};

/*
 * Automate the conversion of strong typedefs to python type that corresponds
 * to the underlying data type (as returned by dl::decay).
 */
template < typename T >
struct lis_caster {
    PYBIND11_TYPE_CASTER(T, _("lisio.core.type.")+_(lis::typeinfo< T >::name));

    static handle cast( const T& src, return_value_policy, handle ) {
        return py::cast( lis::decay( src ) ).release();
    }

    /*
     * For now, do not succeed ever when trying to convert from a python value
     * to the corresponding C++ value, because it's not used and probably
     * requires some template specialisation
     */
    bool load( handle, bool ) { return false; }
};


template <>
struct type_caster< mpark::monostate > {
    PYBIND11_TYPE_CASTER(mpark::monostate, _("monostate"));

    static handle cast( const mpark::monostate&, return_value_policy, handle ) {
        return py::none();
    }

    bool load( handle, bool ) { return false; }
};

template <>
handle lis_caster< lis::string >::cast(const lis::string& src,
                                       return_value_policy,
                                       handle) {
    //TODO possibly implement a trailing whitespace stripper?
    return dlisio::detail::decode_str(lis::decay(src));
}

template <>
handle lis_caster< lis::mask >::cast(const lis::mask& src,
                                     return_value_policy,
                                     handle) {
    return dlisio::detail::decode_str(lis::decay(src));
}
/*
 * Now *register* the strong-typedef type casters with pybind, so that py::cast
 * and the pybind implicit conversion works.
 *
 * Notice that types that just alias native types (std::int32_t/i32 etc.)
 * SHOULD NOT be registered this way, as the conversion already exists, and
 * would cause an infinite loop in the conversion logic.
 */
template <> struct type_caster< lis::i8     > : lis_caster< lis::i8     > {};
template <> struct type_caster< lis::i16    > : lis_caster< lis::i16    > {};
template <> struct type_caster< lis::i32    > : lis_caster< lis::i32    > {};
template <> struct type_caster< lis::f16    > : lis_caster< lis::f16    > {};
template <> struct type_caster< lis::f32    > : lis_caster< lis::f32    > {};
template <> struct type_caster< lis::f32low > : lis_caster< lis::f32low > {};
template <> struct type_caster< lis::f32fix > : lis_caster< lis::f32fix > {};
template <> struct type_caster< lis::string > : lis_caster< lis::string > {};
template <> struct type_caster< lis::byte   > : lis_caster< lis::byte   > {};
template <> struct type_caster< lis::mask   > : lis_caster< lis::mask   > {};

}} // namespace pybind11::detail

namespace {

void assert_overflow(const char* ptr, const char* end, int skip) {
    if (ptr + skip > end) {
        const auto msg = "corrupted record: fmtstr would read past end";
        throw std::runtime_error(msg);
    }
}

void read_fdata_frame( const std::string& fmt,
                       const char*& ptr,
                       const char* end,
                       unsigned char*& dst )
noexcept (false) {

    int src_skip, dst_skip;

    lis_packflen(fmt.c_str(), ptr, &src_skip, &dst_skip);
    assert_overflow(ptr, end, src_skip);
    lis_packf(fmt.c_str(), ptr, dst);
    dst += dst_skip;
    ptr += src_skip;
}

void read_fdata_record( const std::string& fmt,
                        const lis::record& src,
                        unsigned char*&    dst,
                        int& frames,
                        const std::size_t& itemsize,
                        std::size_t allocated_rows,
                        std::function<void (std::size_t)> resize )
noexcept (false) {

    const auto* ptr = src.data.data();
    const auto* end = ptr + src.data.size();

    /* get frame number and slots */
    while (ptr < end) {
        if (frames == allocated_rows) {
            resize(frames * 2);
            dst += (frames * itemsize);
        }

        read_fdata_frame(fmt, ptr, end, dst);

        ++frames;
    }
}


py::object read_fdata(const std::string& fmt,
                      lis::iodevice& file,
                      const lis::record_index& index,
                      const lis::record_info& recinfo,
                      std::size_t itemsize,
                      py::object alloc)
noexcept (false) {
    /*
     * TODO: veriy that format string is valid
     */
    /*
     * This function goes through a lot of ceremony to use numpy arrays
     * directly, and to write output data in-place in the return value. The
     * main reason is *exception safety*.
     *
     * Virtually every operation that goes into this can throw an exception,
     * and when that happens it's important that not-yet-complete data is
     * cleaned up.
     *
     * Of course, for all non-object types this is not a problem, as they're
     * just bytes in an array, and std::vector would've been plenty. It's made
     * more complicated by the presence of PyObject* pointers embedded in the
     * data stream - there are no C++ destructors that can elegantly reach
     * them, and doing that would pretty much be replicating numpy
     * functionality anyway.
     *
     * By writing directly into the numpy array as we go, PyObjects are either
     * default-constructed (set to None) by numpy, or properly created (and
     * replaced) here.
     */
    auto implicits = index.implicits_of( recinfo.ltell );
    auto allocated_rows = implicits.size();
    auto dstobj = alloc(allocated_rows);
    auto dstb = py::buffer(dstobj);
    auto info = dstb.request(true);
    auto* dst = static_cast< unsigned char* >(info.ptr);

    /*
     * Resizing is clumsy, because in-place resize (through the method)
     * requires there to be no references to the underlying data. That means
     * the buffer-info and buffer must be wiped before resizing takes place,
     * and then carefully restored to the new memory.
     */
    auto resize = [&](std::size_t n) {
        info = py::buffer_info {};
        dstb = py::buffer {};
        dstobj.attr("resize")(n);
        allocated_rows = n;
        dstb = py::buffer(dstobj);
        info = dstb.request(true);
        dst = static_cast< unsigned char* >(info.ptr);
    };

    int frames = 0;

    for ( const auto& head : implicits ) {
        /* get record */
        auto record = file.read_record( head );
        read_fdata_record( fmt,
                           record,
                           dst,
                           frames,
                           itemsize,
                           allocated_rows,
                           resize );
    }

    assert(allocated_rows >= frames);
    if (allocated_rows > frames)
        resize(frames);

    return dstobj;
}

} // namespace


void init_lis_extension(py::module_ &m) {
    py::enum_< lis::record_type >( m, "lis_rectype")
        .value( "normal_data" , lis::record_type::normal_data)
        .value( "alt_data"    , lis::record_type::alt_data   )
        .value( "job_id"      , lis::record_type::job_id     )
        .value( "wellsite"    , lis::record_type::wellsite   )
        .value( "toolstring"  , lis::record_type::toolstring )
        .value( "encrp_table" , lis::record_type::encrp_table)
        .value( "table_dump"  , lis::record_type::table_dump )
        .value( "format_spec" , lis::record_type::format_spec)
        .value( "descriptor"  , lis::record_type::descriptor )
        .value( "sw_boot"     , lis::record_type::sw_boot    )
        .value( "bootstrap"   , lis::record_type::bootstrap  )
        .value( "cp_kernel"   , lis::record_type::cp_kernel  )
        .value( "program_fh"  , lis::record_type::program_fh )
        .value( "program_oh"  , lis::record_type::program_oh )
        .value( "program_ol"  , lis::record_type::program_ol )
        .value( "fileheader"  , lis::record_type::fileheader )
        .value( "filetrailer" , lis::record_type::filetrailer)
        .value( "tapeheader"  , lis::record_type::tapeheader )
        .value( "tapetrailer" , lis::record_type::tapetrailer)
        .value( "reelheader"  , lis::record_type::reelheader )
        .value( "reeltrailer" , lis::record_type::reeltrailer)
        .value( "logical_eof" , lis::record_type::logical_eof)
        .value( "logical_bot" , lis::record_type::logical_bot)
        .value( "logical_eot" , lis::record_type::logical_eot)
        .value( "logical_eom" , lis::record_type::logical_eom)
        .value( "op_command"  , lis::record_type::op_command )
        .value( "op_response" , lis::record_type::op_response)
        .value( "sys_output"  , lis::record_type::sys_output )
        .value( "flic_comm"   , lis::record_type::flic_comm  )
        .value( "blank_rec"   , lis::record_type::blank_rec  )
        .value( "picture"     , lis::record_type::picture    )
        .value( "image"       , lis::record_type::image      )
    ;

    py::enum_< lis::representation_code >( m, "lis_reprc")
        .value( "i8"     , lis::representation_code::i8     )
        .value( "i16"    , lis::representation_code::i16    )
        .value( "i32"    , lis::representation_code::i32    )
        .value( "f16"    , lis::representation_code::f16    )
        .value( "f32"    , lis::representation_code::f32    )
        .value( "f32low" , lis::representation_code::f32low )
        .value( "f32fix" , lis::representation_code::f32fix )
        .value( "string" , lis::representation_code::string )
        .value( "byte"   , lis::representation_code::byte   )
        .value( "mask"   , lis::representation_code::mask   )
    ;

    /* start - io.hpp */
    m.def("openlis", &lis::open,
            py::arg("filepath"),
            py::arg("offset")    = 0,
            py::arg("tapeimage") = true
    );

    py::class_< lis::iodevice >( m, "lis_stream" )
        .def( "__repr__", [](const lis::iodevice& x) {
            return "lis::iodevice(poffset={})"_s.format( x.poffset() );
        })
        .def( "read_record",   &lis::iodevice::read_record )
        .def( "index_records", &lis::iodevice::index_records )
        .def( "index_record",  &lis::iodevice::index_record )
        .def( "poffset",       &lis::iodevice::poffset )
        .def( "psize",         &lis::iodevice::psize )
        .def( "ptell",         &lis::iodevice::ptell )
        .def( "istruncated",   &lis::iodevice::truncated )
        .def( "close",         &lis::iodevice::close )
        .def( "seek",          &lis::iodevice::seek )
        .def( "read", []( lis::iodevice& s, py::buffer b, long long off, int n ) {
            auto info = b.request();
            if (info.size < n) {
                std::string msg =
                      "buffer to small: buffer.size (which is "
                    + std::to_string( info.size ) + ") < "
                    + "n (which is " + std::to_string( n ) + ")"
                ;
                throw std::invalid_argument( msg );
            }
            s.seek( off );
            s.read( static_cast< char* >( info.ptr ), n);
            return b;
        })
        .def( "read_physical_header",  &lis::iodevice::read_physical_header )
        .def( "read_logical_header",   &lis::iodevice::read_logical_header )
    ;

    py::class_< lis::lrheader >( m, "lrheader" )
        .def_readonly( "type", &lis::lrheader::type )
        .def( "__repr__", []( const lis::lrheader& x ) {
            return "lis::lrheader(type={})"_s.format( x.type );
        })
    ;

    py::class_< lis::prheader >( m, "prheader" )
        .def_property_readonly( "length", []( const lis::prheader& x ) {
            return x.length;
        })
        .def( "__repr__", [](const lis::prheader& x) {
            bool prev = x.attributes & lis::prheader::predces;
            bool succ = x.attributes & lis::prheader::succses;
            return "lis::prheader(length={}, pred={}, succ={})"_s.format(
                x.length,
                prev,
                succ
            );
        })
    ;

    py::class_< lis::record_info >( m, "lis_record_info" )
        .def( "__repr__", [](const lis::record_info& x) {
            return "lis::record_info(type={}, ltell={})"_s.format(
                x.lrh.type, x.ltell
            );
        })
        .def_readonly(          "ltell", &lis::record_info::ltell )
        .def_readonly(          "prh",   &lis::record_info::prh   )
        .def_property_readonly( "type",  &lis::record_info::type  )
    ;

    py::class_< lis::record >( m, "lis_record", py::buffer_protocol() )
        .def( "__repr__", [](const lis::record& x) {
            return "lis::record(type={}, ltell={}, size={})"_s.format(
                x.info.type(), x.info.ltell, x.data.size()
            );
        })
        .def_buffer( []( lis::record& rec ) -> py::buffer_info {
            const auto fmt = py::format_descriptor< char >::format();
            return py::buffer_info(
                rec.data.data(),    /* Pointer to buffer */
                sizeof(char),       /* Size of one scalar */
                fmt,                /* Python struct-style format descriptor */
                1,                  /* Number of dimensions */
                { rec.data.size() },/* Buffer dimensions */
                { 1 }               /* Strides (in bytes) for each index */
            );
        })
    ;

    py::class_< lis::record_index >( m, "lis_record_index" )
        .def( "explicits", &lis::record_index::explicits )
        .def( "implicits", &lis::record_index::implicits )
        .def( "size",      &lis::record_index::size )
        .def( "__repr__", [](const lis::record_index& x) {
            return "lis::record_info(size={})"_s.format(
                x.size()
            );
        })
    ;
    /* end - io.hpp */

    /* start - parse.hpp */
    py::class_< lis::entry_block >( m, "entry_block" )
        .def_readonly( "type",  &lis::entry_block::type  )
        .def_readonly( "size",  &lis::entry_block::size  )
        .def_readonly( "reprc", &lis::entry_block::reprc )
        .def_readonly( "value", &lis::entry_block::value )
    ;

    py::class_< lis::spec_block >( m, "spec_block" )
        .def_readonly( "mnemonic",         &lis::spec_block::mnemonic         )
        .def_readonly( "service_id",       &lis::spec_block::service_id       )
        .def_readonly( "service_order_nr", &lis::spec_block::service_order_nr )
        .def_readonly( "units",            &lis::spec_block::units            )
        .def_readonly( "filenr",           &lis::spec_block::filenr           )
        .def_readonly( "size",             &lis::spec_block::ssize            )
        .def_readonly( "samples",          &lis::spec_block::samples          )
        .def_readonly( "reprc",            &lis::spec_block::reprc            )
    ;

    py::class_< lis::dfsr >( m, "dfsr" )
        .def_readonly( "info",    &lis::dfsr::info    )
        .def_readonly( "entries", &lis::dfsr::entries )
        .def_readonly( "specs",   &lis::dfsr::specs   )
    ;

    m.def( "parse_dfsr", &lis::parse_dfsr );
    m.def("dfs_formatstring", &lis::dfs_fmtstr);
    /* end - parse.hpp */

    m.def("read_fdata", read_fdata);
}