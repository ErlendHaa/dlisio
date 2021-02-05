#include <cstdint>
#include <cstring>
#include <algorithm>
#include <cstring>

#include <fmt/core.h>

#include <dlisio/lis/protocol.hpp>
#include <dlisio/lis/types.h>
#include <dlisio/lis/types.hpp>

namespace dlisio { namespace lis79 {
namespace lis = dlisio::lis79;

constexpr const int lis::spec_block1::size;
constexpr const int lis::spec_block0::size;

bool is_padbytes(const char* xs, std::uint16_t size) {
    constexpr int PADBYTE_NULL  = 0x00;
    constexpr int PADBYTE_SPACE = 0x20;

    /* Calling this function with size=0 is nonsensical. We regard this special
     * case as: "In this buffer of zero bytes, there are no padbytes". So the
     * function returns false.
     */
    if (size == 0) return false;

    auto* cur = xs;
    char padfmt = *cur;
    if ( (padfmt != PADBYTE_NULL) and (padfmt != PADBYTE_SPACE) ) {
        return false;
    }

    while ( ++cur < xs + size ) {
        if (*cur != padfmt) return false;
    }

    return true;
}

lis::prheader read_prh(char* xs) noexcept (false) {
    char buffer[lis::prheader::size];
    std::memcpy(buffer, xs, prheader::size);

    #ifdef HOST_LITTLE_ENDIAN
        std::reverse(buffer + 0, buffer + 2);
        std::reverse(buffer + 2, buffer + 4);
    #endif

    lis::prheader head;
    std::memcpy(&head.length,      buffer + 0, sizeof(std::uint16_t));
    std::memcpy(&head.attributes,  buffer + 2, sizeof(std::uint16_t));
    return head;
}

lis::lrheader read_lrh(const char* xs) noexcept (true) {
    lis::lrheader head;

    std::memcpy(&head.type,       xs + 0, sizeof( std::uint8_t ));
    std::memcpy(&head.attributes, xs + 1, sizeof( std::uint8_t ));

    return head;
}

bool valid_rectype(lis::byte type) {
    const auto rectype = static_cast< lis::record_type >(lis::decay( type ));
    using rt = lis::record_type;

    switch (rectype) {
        case rt::normal_data:
        case rt::alt_data:
        case rt::job_id:
        case rt::wellsite:
        case rt::toolstring:
        case rt::encrp_table:
        case rt::table_dump:
        case rt::format_spec:
        case rt::descriptor:
        case rt::sw_boot:
        case rt::bootstrap:
        case rt::cp_kernel:
        case rt::program_fh:
        case rt::program_oh:
        case rt::program_ol:
        case rt::fileheader:
        case rt::filetrailer:
        case rt::tapeheader:
        case rt::tapetrailer:
        case rt::reelheader:
        case rt::reeltrailer:
        case rt::logical_eof:
        case rt::logical_bot:
        case rt::logical_eot:
        case rt::logical_eom:
        case rt::op_command:
        case rt::op_response:
        case rt::sys_output:
        case rt::flic_comm:
        case rt::blank_rec:
        case rt::picture:
        case rt::image:
            return true;
        default:
            return false;
    }
}

/* record_info */
record_type record_info::type() const noexcept (false) {
    return static_cast< record_type >( lis::decay(this->lrh.type));
}

namespace {

using std::swap;
const char* cast( const char* xs, lis::i8& i ) noexcept (true) {
    std::int8_t x;
    xs = lis_i8( xs, &x );

    lis::i8 tmp{ x };
    swap( i, tmp );

    return xs;
}

const char* cast( const char* xs, lis::i16& i ) noexcept (true) {
    std::int16_t x;
    xs = lis_i16( xs, &x );

    lis::i16 tmp{ x };
    swap( i, tmp );

    return xs;
}

const char* cast( const char* xs, lis::i32& i )
noexcept (true) {
    std::int32_t x;
    xs = lis_i32( xs, &x );

    lis::i32 tmp{ x };
    swap( i, tmp );

    return xs;
}

const char* cast( const char* xs, lis::f16& f )
noexcept (true) {
    float x;
    xs = lis_f16( xs, &x );
    f = lis::f16{ x };
    return xs;
}

const char* cast( const char* xs, lis::f32& f )
noexcept (true) {
    float x;
    xs = lis_f32( xs, &x );
    f = lis::f32{ x };
    return xs;
}

const char* cast( const char* xs, lis::f32low& f )
noexcept (true) {
    float x;
    xs = lis_f32low( xs, &x );
    f = lis::f32low{ x };
    return xs;
}

const char* cast( const char* xs, lis::f32fix& f )
noexcept (true) {
    float x;
    xs = lis_f32fix( xs, &x );
    f = lis::f32fix{ x };
    return xs;
}

/* string- or alphanumeric (reprc 65) is a bit special in that it doesn't
 * contain its own length. I don't see a nice way around this, other than to
 * break the cast's interface by adding length as a parameter.
 */
const char* cast(const char* xs, lis::string& s, std::int32_t len)
noexcept (true) {
    std::vector< char > str;
    str.resize( len );
    xs = lis_string( xs, len, str.data() );

    lis::string tmp{ std::string{ str.begin(), str.end() } };
    swap( s, tmp );
    return xs;
}

const char* cast( const char* xs, lis::byte& i )
noexcept (true) {
    std::uint8_t x;
    xs = lis_byte( xs, &x );

    lis::byte tmp{ x };
    swap( tmp, i );
    return xs;
}

const char* cast(const char* xs, lis::mask& s, std::int32_t len)
noexcept (true) {
    std::vector< char > str;
    str.resize( len );
    xs = lis_mask( xs, len, str.data() );

    lis::mask tmp{ std::string{ str.begin(), str.end() } };
    swap( s, tmp );
    return xs;
}

const char* cast( const char* xs, lis::representation_code& reprc )
noexcept (false) {

    std::uint8_t x;
    xs = lis_byte( xs, &x );

    // TODO: validate representation code
    reprc = static_cast< lis::representation_code >( x );
    return xs;
}

template < typename T >
const char* extract( const char* xs, T& val )
noexcept (false) {
    xs = cast( xs, val );
    return xs;
}

template < typename T, typename U>
const char* extract( const char* xs, T& val, U size )
noexcept (false) {
    const auto sz = lis::decay( size );
    xs = cast( xs, val, sz );
    return xs;
}

template < typename T >
T& reset( lis::value_type& value ) noexcept (false) {
    return value.emplace< T >();
}

template < typename T >
const char* element( const char* xs,
                     T size,
                     lis::representation_code reprc,
                     lis::value_type& val )
noexcept (false) {

    using rpc = lis::representation_code;
    switch (reprc) {
        case rpc::i8 :    return extract( xs, reset< lis::i8     >(val) );
        case rpc::i16:    return extract( xs, reset< lis::i16    >(val) );
        case rpc::i32:    return extract( xs, reset< lis::i32    >(val) );
        case rpc::f16:    return extract( xs, reset< lis::f16    >(val) );
        case rpc::f32:    return extract( xs, reset< lis::f32    >(val) );
        case rpc::f32low: return extract( xs, reset< lis::f32low >(val) );
        case rpc::f32fix: return extract( xs, reset< lis::f32fix >(val) );
        case rpc::string: return extract( xs, reset< lis::string >(val), size );
        case rpc::byte:   return extract( xs, reset< lis::byte   >(val) );
        case rpc::mask:   return extract( xs, reset< lis::mask   >(val), size );
        default: {
            const auto msg = "unable to interpret attribute: "
                             "unknown representation code {}";
            const auto code = static_cast< int >(reprc);
            throw std::runtime_error(fmt::format(msg, code));
        }
    }

    return xs;
}

} // namespace

lis::entry_block read_entry_block( const lis::record& rec, std::size_t* offset )
noexcept (false) {
    constexpr int SB_MINIMUM_SIZE = 3;

    const auto* cur = rec.data.data();
    const auto* end = cur + rec.data.size();

    if (offset) cur += *offset;

    if ( std::distance(cur, end) < SB_MINIMUM_SIZE ) {
        const auto msg = "lis::entry_block: "
                         "{} bytes left in record, expected at least {} more";
        const auto left = std::distance(cur, end);
        throw std::runtime_error(fmt::format(msg, left, SB_MINIMUM_SIZE));
    }

    lis::entry_block entry;

    cur = cast( cur, entry.type  ); // TODO verify type
    cur = cast( cur, entry.size  );
    cur = cast( cur, entry.reprc ); // TODO verify reprc

    if ( std::distance(cur, end) < lis::decay( entry.size ) ) {
        const auto msg = "lis::entry_block: "
                         "{} bytes left in record, expected at least {} more";
        const auto left = std::distance(cur, end);
        throw std::runtime_error(fmt::format(msg, left, lis::decay(entry.size)));
    }

    auto repr = static_cast< lis::representation_code>( lis::decay(entry.reprc) );
    cur = element(cur, entry.size, repr, entry.value);

    if (offset) *offset += (SB_MINIMUM_SIZE + lis::decay( entry.size ));

    return entry;
}

namespace {

template < typename T >
void read_spec_block(const lis::record& rec, std::size_t* offset, T& spec )
noexcept (false) {
    const auto* cur = rec.data.data();
    const auto* end = cur + rec.data.size();

    if (offset) cur += *offset;

    if ( std::distance(cur, end) < T::size ) {
        const auto msg = "lis::spec_block: "
                         "{} bytes left in record, expected at least {} more";
        const auto left = std::distance(cur, end);
        throw std::runtime_error(fmt::format(msg, left, T::size));
    }

    cur = cast( cur, spec.mnemonic,         4 );
    cur = cast( cur, spec.service_id,       6 );
    cur = cast( cur, spec.service_order_nr, 8 );
    cur = cast( cur, spec.units,            4 );
    cur += 4;                       // Skip API codes
    cur = cast( cur, spec.filenr );
    cur = cast( cur, spec.ssize );
    cur += 3;                       // Skip padding (and process level)
    cur = cast( cur, spec.samples );
    cur = cast( cur, spec.reprc );
    cur += 5;                       // Skip padding / Process indicators

    if (offset) *offset += T::size;
}

} // namespace

spec_block0 read_spec_block0(const record& rec, std::size_t* offset) noexcept (false) {
    lis::spec_block0 spec;
    read_spec_block(rec, offset, spec);
    return spec;
}

spec_block1 read_spec_block1(const record& rec, std::size_t* offset) noexcept (false) {
    lis::spec_block1 spec;
    read_spec_block(rec, offset, spec);
    return spec;
}

lis::dfsr parse_dfsr( const lis::record& rec ) noexcept (false) {
    lis::dfsr formatspec;
    formatspec.info = rec.info; //carry over the header information of the record

    std::uint8_t subtype = 0;
    std::size_t offset = 0;

    while (true) {
        const auto entry = read_entry_block(rec, &offset);
        const auto type  = static_cast< lis::entry_type >( lis::decay(entry.type) );

        formatspec.entries.push_back( std::move(entry) );
        // TODO swich on subtype based on entry block

        if ( type == lis::entry_type::terminator )
            break;
    }

    while ( offset < rec.data.size() ) {
        if (subtype == 0) {
            formatspec.specs.emplace_back( read_spec_block0(rec, &offset) );
        } else {
            formatspec.specs.emplace_back( read_spec_block1(rec, &offset) );
        }
    }

    return formatspec;
}

std::string dfs_fmtstr( const dfsr& dfs ) noexcept (false) {
    std::string fmt{};

    for (const auto& spec : dfs.specs) {
        std::uint8_t s; // size of one entry
        char f;
        using rpc = lis::representation_code;
        switch (spec.reprc) {
            case rpc::i8:     { f=LIS_FMT_I8;     s=LIS_SIZEOF_I8;     break; }
            case rpc::i16:    { f=LIS_FMT_I16;    s=LIS_SIZEOF_I16;    break; }
            case rpc::i32:    { f=LIS_FMT_I32;    s=LIS_SIZEOF_I32;    break; }
            case rpc::f16:    { f=LIS_FMT_F16;    s=LIS_SIZEOF_F16;    break; }
            case rpc::f32:    { f=LIS_FMT_F32;    s=LIS_SIZEOF_F32;    break; }
            case rpc::f32low: { f=LIS_FMT_F32LOW; s=LIS_SIZEOF_F32LOW; break; }
            case rpc::f32fix: { f=LIS_FMT_F32FIX; s=LIS_SIZEOF_F32FIX; break; }
            case rpc::byte:   { f=LIS_FMT_BYTE;   s=LIS_SIZEOF_BYTE;   break; }

            /* Assume for now that repcode 65 (lis::string) and repcode 77
             * (lis::mask) cannot be used as types in the frame.
             *
             * repcode 65 and 77 are variable length, but the length is not
             * encoded in the type itself. Rather, it must come from an
             * external source. It doesn't seem like DFSR and IFLR have a
             * mechanism of specifying such sizes.
             */
            default: {
                std::string msg = "lis::dfs_fmtstr: Cannot create formatstring"
                                  ". Invalid repcode ({}) in channel ({})";
                const auto code = static_cast< int >(spec.reprc);
                const auto mnem = lis::decay(spec.mnemonic);
                throw std::runtime_error(fmt::format(msg, code, mnem));
            }
        }

        const auto size = lis::decay(spec.ssize);
        if( size % s ) {
            std::string msg  = "lis::dfs_fmtstr: Cannot compute an integral "
                "number of entries from size ({}) / repcode({}) for channel {}";
            const auto code = static_cast< int >(spec.reprc);
            const auto mnem = lis::decay(spec.mnemonic);
            throw std::runtime_error(fmt::format(msg, size, code, mnem));
        }
        std::int16_t entries = size / s;
        fmt.append( entries, f );
    }

    return fmt;
}

} // namespace lis79

} // namespace dlisio