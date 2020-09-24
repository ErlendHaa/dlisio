#include <algorithm>
#include <bitset>
#include <cstdlib>
#include <cstring>
#include <string>
#include <ciso646>

#include <fmt/core.h>

#include <dlisio/dlisio.h>
#include <dlisio/ext/types.hpp>

namespace {

struct set_descriptor {
    int role;
    bool type;
    bool name;
};

set_descriptor parse_set_descriptor( const char* cur ) noexcept (false) {
    std::uint8_t attr;
    std::memcpy( &attr, cur, DLIS_DESCRIPTOR_SIZE );

    set_descriptor flags;
    dlis_component( attr, &flags.role );

    int type, name;
    const auto err = dlis_component_set( attr, flags.role, &type, &name );

    switch (err) {
        case DLIS_OK:
            break;
        case DLIS_UNEXPECTED_VALUE: {
                const auto bits = std::bitset< 8 >{ attr }.to_string();
                const auto role = dlis_component_str(flags.role);
                const auto msg  = "error parsing object set descriptor: "
                                "expected SET, RSET or RDSET, was {} ({})"
                                ;
                throw std::invalid_argument(fmt::format(msg, role, bits));
            }
        default:
            throw std::runtime_error("unhandled error in dlis_component_set");
    }

    flags.type = type;
    flags.name = name;

    return flags;
}

struct attribute_descriptor {
    /* label, count, reprc, units, value are valid only when 'object' and
    'absent' are false. See parse_attribute_descriptor for parsing routine*/
    bool label;
    bool count;
    bool reprc;
    bool units;
    bool value;
    bool object = false;
    bool absent = false;
    bool invariant = false;
};

attribute_descriptor parse_attribute_descriptor( const char* cur ) {
    std::uint8_t attr;
    std::memcpy( &attr, cur, DLIS_DESCRIPTOR_SIZE );

    int role;
    dlis_component( attr, &role );

    attribute_descriptor flags;
    switch (role) {
        case DLIS_ROLE_ABSATR:
            flags.absent = true;
            break;

        case DLIS_ROLE_OBJECT:
            flags.object = true;
            break;

        case DLIS_ROLE_INVATR:
            flags.invariant = true;
            break;

        default:
            break;
    }

    if (flags.object || flags.absent) return flags;

    int label, count, reprc, units, value;
    const auto err = dlis_component_attrib( attr, role, &label,
                                                        &count,
                                                        &reprc,
                                                        &units,
                                                        &value );

    switch (err) {
        case DLIS_OK:
            break;
        case DLIS_UNEXPECTED_VALUE: {
            const auto bits = std::bitset< 8 >(role).to_string();
            const auto was  = dlis_component_str(role);
            const auto msg  = "error parsing attribute descriptor: "
                              "expected ATTRIB, INVATR, ABSATR or OBJECT, "
                              "was {} ({})";
            throw std::invalid_argument(fmt::format(msg, was, bits));
            }
        default:
            throw std::runtime_error( "unhandled error in "
                                      "dlis_component_attrib" );
    }

    flags.label = label;
    flags.count = count;
    flags.reprc = reprc;
    flags.units = units;
    flags.value = value;

    return flags;
}

struct object_descriptor {
    bool name;
};

object_descriptor parse_object_descriptor( const char* cur ) {
    std::uint8_t attr;
    std::memcpy( &attr, cur, DLIS_DESCRIPTOR_SIZE );

    int role;
    dlis_component( attr, &role );

    int name;
    const auto err = dlis_component_object( attr, role, &name );

    switch (err) {
        case DLIS_OK:
            break;
        case DLIS_UNEXPECTED_VALUE: {
            const auto bits = std::bitset< 8 >{ attr }.to_string();
            const auto was  = dlis_component_str(role);
            const auto msg  = "error parsing object descriptor: "
                            "expected OBJECT, was {} ({})"
                            ;
            throw std::invalid_argument(fmt::format(msg, was, bits));
            }
        default:
            throw std::runtime_error("unhandled error in "
                                     "dlis_component_object");
    }

    object_descriptor flags;
    flags.name = name;

    return flags;
}

using std::swap;
const char* cast( const char* xs, dl::sshort& i ) noexcept (true) {
    std::int8_t x;
    xs = dlis_sshort( xs, &x );

    dl::sshort tmp{ x };
    swap( i, tmp );

    return xs;
}

const char* cast( const char* xs, dl::snorm& i ) noexcept (true) {
    std::int16_t x;
    xs = dlis_snorm( xs, &x );

    dl::snorm tmp{ x };
    swap( i, tmp );

    return xs;
}

const char* cast( const char* xs, dl::slong& i ) noexcept (true) {
    std::int32_t x;
    xs = dlis_slong( xs, &x );

    dl::slong tmp{ x };
    swap( i, tmp );
    return xs;
}

const char* cast( const char* xs, dl::ushort& i ) noexcept (true) {
    std::uint8_t x;
    xs = dlis_ushort( xs, &x );

    dl::ushort tmp{ x };
    swap( tmp, i );
    return xs;
}


const char* cast( const char* xs, dl::unorm& i ) noexcept (true) {
    std::uint16_t x;
    xs = dlis_unorm( xs, &x );

    dl::unorm tmp{ x };
    swap( tmp, i );
    return xs;
}

const char* cast( const char* xs, dl::ulong& i ) noexcept (true) {
    std::uint32_t x;
    xs = dlis_ulong( xs, &x );
    i = dl::ulong{ x };
    return xs;
}

const char* cast( const char* xs, dl::uvari& i ) noexcept (true) {
    std::int32_t x;
    xs = dlis_uvari( xs, &x );
    i = dl::uvari{ x };
    return xs;
}

const char* cast( const char* xs, dl::fshort& f ) noexcept (true) {
    float x;
    xs = dlis_fshort( xs, &x );
    f = dl::fshort{ x };
    return xs;
}

const char* cast( const char* xs, dl::fsingl& f ) noexcept (true) {
    float x;
    xs = dlis_fsingl( xs, &x );
    f = dl::fsingl{ x };
    return xs;
}

const char* cast( const char* xs, dl::fdoubl& f ) noexcept (true) {
    double x;
    xs = dlis_fdoubl( xs, &x );
    f = dl::fdoubl{ x };
    return xs;
}

const char* cast( const char* xs, dl::fsing1& f ) noexcept (true) {
    float v, a;
    xs = dlis_fsing1( xs, &v, &a );
    f = dl::fsing1{ v, a };
    return xs;
}

const char* cast( const char* xs, dl::fsing2& f ) noexcept (true) {
    float v, a, b;
    xs = dlis_fsing2( xs, &v, &a, &b );
    f = dl::fsing2{ v, a, b };
    return xs;
}

const char* cast( const char* xs, dl::fdoub1& f ) noexcept (true) {
    double v, a;
    xs = dlis_fdoub1( xs, &v, &a );
    f = dl::fdoub1{ v, a };
    return xs;
}

const char* cast( const char* xs, dl::fdoub2& f ) noexcept (true) {
    double v, a, b;
    xs = dlis_fdoub2( xs, &v, &a, &b );
    f = dl::fdoub2{ v, a, b };
    return xs;
}

const char* cast( const char* xs, dl::csingl& f ) noexcept (true) {
    float re, im;
    xs = dlis_csingl( xs, &re, &im );
    f = dl::csingl{ std::complex< float >{ re, im } };
    return xs;
}

const char* cast( const char* xs, dl::cdoubl& f ) noexcept (true) {
    double re, im;
    xs = dlis_cdoubl( xs, &re, &im );
    f = dl::cdoubl{ std::complex< double >{ re, im } };
    return xs;
}

const char* cast( const char* xs, dl::isingl& f ) noexcept (true) {
    float x;
    xs = dlis_isingl( xs, &x );
    f = dl::isingl{ x };
    return xs;
}

const char* cast( const char* xs, dl::vsingl& f ) noexcept (true) {
    float x;
    xs = dlis_vsingl( xs, &x );
    f = dl::vsingl{ x };
    return xs;
}

const char* cast( const char* xs, dl::status& s ) noexcept (true) {
    dl::status::value_type x;
    xs = dlis_status( xs, &x );
    s = dl::status{ x };
    return xs;
}

template <typename T>
const char* parse_ident( const char* xs, T& out ) noexcept (false) {
    char str[ 256 ];
    std::int32_t len;

    xs = dlis_ident( xs, &len, str );

    T tmp{ std::string{ str, str + len } };
    swap( out, tmp );
    return xs;
}

const char* cast( const char* xs, dl::ident& id ) {
    return parse_ident( xs, id );
}

const char* cast( const char* xs, dl::units& id ) {
    return parse_ident( xs, id );
}

const char* cast( const char* xs, dl::ascii& ascii ) noexcept (false) {
    std::vector< char > str;
    std::int32_t len;

    dlis_ascii( xs, &len, nullptr );
    str.resize( len );
    xs = dlis_ascii( xs, &len, str.data() );

    dl::ascii tmp{ std::string{ str.begin(), str.end() } };
    swap( ascii, tmp );
    return xs;
}

const char* cast( const char* xs, dl::origin& origin ) noexcept (true) {
    dl::origin::value_type x;
    xs = dlis_origin( xs, &x );
    origin = dl::origin{ x };
    return xs;
}

const char* cast( const char* xs, dl::obname& obname ) noexcept (false) {
    char str[ 256 ];
    std::int32_t len;

    std::int32_t orig;
    std::uint8_t copy;

    xs = dlis_obname( xs, &orig, &copy, &len, str );

    dl::obname tmp{ dl::origin{ orig },
                    dl::ushort{ copy },
                    dl::ident{ std::string{ str, str + len } } };
    swap( obname, tmp );
    return xs;
}

const char* cast( const char* xs, dl::objref& objref ) noexcept (false) {
    char iden[ 256 ];
    char name[ 256 ];
    std::int32_t ident_len;
    std::int32_t origin;
    std::uint8_t copy_number;
    std::int32_t obname_len;

    xs = dlis_objref( xs, &ident_len,
                          iden,
                          &origin,
                          &copy_number,
                          &obname_len,
                          name );

    dl::objref tmp{ dl::ident{ std::string{ iden, iden + ident_len } },
                    dl::obname{
                        dl::origin{ origin },
                        dl::ushort{ copy_number },
                        dl::ident{ std::string{ name, name + obname_len } }
                    }
    };

    swap( objref, tmp );
    return xs;
}

const char* cast( const char* xs, dl::attref& attref ) noexcept (false) {
    char id1[ 256 ];
    char obj[ 256 ];
    char id2[ 256 ];
    std::int32_t ident1_len;
    std::int32_t origin;
    std::uint8_t copy_number;
    std::int32_t obname_len;
    std::int32_t ident2_len;

    xs = dlis_attref( xs, &ident1_len,
                          id1,
                          &origin,
                          &copy_number,
                          &obname_len,
                          obj,
                          &ident2_len,
                          id2 );

    dl::attref tmp{ dl::ident{ std::string{ id1, id1 + ident1_len } },
                    dl::obname{
                        dl::origin{ origin },
                        dl::ushort{ copy_number },
                        dl::ident{ std::string{ obj, obj + obname_len } }
                    },
                    dl::ident{ std::string{ id2, id2 + ident2_len } }
    };

    swap( attref, tmp );
    return xs;
}


const char* cast( const char* xs, dl::dtime& dtime ) noexcept (true) {
    dl::dtime dt;
    xs = dlis_dtime( xs, &dt.Y,
                         &dt.TZ,
                         &dt.M,
                         &dt.D,
                         &dt.H,
                         &dt.MN,
                         &dt.S,
                         &dt.MS );
    dt.Y = dlis_year( dt.Y );
    swap( dtime, dt );
    return xs;
}

const char* cast( const char* xs,
                  dl::object_attribute& attr ) noexcept (false) {

    dl::representation_code& reprc = attr.reprc;

    dl::ushort x{ 0 };
    xs = cast( xs, x );

    if (x < DLIS_FSHORT || x > DLIS_UNITS) {
        const auto msg = "Invalid representation code {}";
        const auto code = static_cast< int >(x);
        dl::dlis_error err {
            dl::error_severity::INFO,
            fmt::format(msg, code),
            "Appendix B: Representation Codes",
            "Continue. Postpone dealing with this until later"
        };
        attr.info.push_back(err);

        reprc = dl::representation_code::undef;
    } else {
        reprc = static_cast< dl::representation_code >( x );
    }
    return xs;
}

template < typename T >
const char* extract( std::vector< T >& vec,
                     std::int32_t count,
                     const char* xs ) noexcept (false) {

    T elem;
    std::vector< T > tmp;
    for( std::int32_t i = 0; i < count; ++i ) {
        xs = cast( xs, elem );
        tmp.push_back( std::move( elem ) );
    }

    vec.swap( tmp );
    return xs;
}

template < typename T >
std::vector< T >& reset( dl::value_vector& value ) noexcept (false) {
    return value.emplace< std::vector< T > >();
}

const char* elements( const char* xs, dl::object_attribute& attr ) {
    const auto reprc = attr.reprc;
    dl::value_vector& vec = attr.value;
    const auto n = dl::decay( attr.count );

    if (n == 0) {
        vec = mpark::monostate{};
        return xs;
    }

    using rpc = dl::representation_code;
    switch (reprc) {
        case rpc::fshort: return extract( reset< dl::fshort >( vec ), n, xs );
        case rpc::fsingl: return extract( reset< dl::fsingl >( vec ), n, xs );
        case rpc::fsing1: return extract( reset< dl::fsing1 >( vec ), n, xs );
        case rpc::fsing2: return extract( reset< dl::fsing2 >( vec ), n, xs );
        case rpc::isingl: return extract( reset< dl::isingl >( vec ), n, xs );
        case rpc::vsingl: return extract( reset< dl::vsingl >( vec ), n, xs );
        case rpc::fdoubl: return extract( reset< dl::fdoubl >( vec ), n, xs );
        case rpc::fdoub1: return extract( reset< dl::fdoub1 >( vec ), n, xs );
        case rpc::fdoub2: return extract( reset< dl::fdoub2 >( vec ), n, xs );
        case rpc::csingl: return extract( reset< dl::csingl >( vec ), n, xs );
        case rpc::cdoubl: return extract( reset< dl::cdoubl >( vec ), n, xs );
        case rpc::sshort: return extract( reset< dl::sshort >( vec ), n, xs );
        case rpc::snorm : return extract( reset< dl::snorm  >( vec ), n, xs );
        case rpc::slong : return extract( reset< dl::slong  >( vec ), n, xs );
        case rpc::ushort: return extract( reset< dl::ushort >( vec ), n, xs );
        case rpc::unorm : return extract( reset< dl::unorm  >( vec ), n, xs );
        case rpc::ulong : return extract( reset< dl::ulong  >( vec ), n, xs );
        case rpc::uvari : return extract( reset< dl::uvari  >( vec ), n, xs );
        case rpc::ident:  return extract( reset< dl::ident  >( vec ), n, xs );
        case rpc::ascii : return extract( reset< dl::ascii  >( vec ), n, xs );
        case rpc::dtime : return extract( reset< dl::dtime  >( vec ), n, xs );
        case rpc::origin: return extract( reset< dl::origin >( vec ), n, xs );
        case rpc::obname: return extract( reset< dl::obname >( vec ), n, xs );
        case rpc::objref: return extract( reset< dl::objref >( vec ), n, xs );
        case rpc::attref: return extract( reset< dl::attref >( vec ), n, xs );
        case rpc::status: return extract( reset< dl::status >( vec ), n, xs );
        case rpc::units : return extract( reset< dl::units  >( vec ), n, xs );
        default: {
            const auto msg = "unable to interpret attribute: "
                             "unknown representation code {}";
            const auto code = static_cast< int >(reprc);
            throw std::runtime_error(fmt::format(msg, code));
        }
    }

    return xs;
}

struct variant_equal {
    template < typename T, typename U >
    bool operator () (T&&, U&&) const noexcept (true) {
        return false;
    }

    bool operator () (mpark::monostate,
                      mpark::monostate)
    const noexcept (true) {
        return true;
    }

    template < typename T >
    bool operator () (const std::vector< T >& lhs,
                      const std::vector< T >& rhs)
    const noexcept (true) {
        return (lhs.size() == rhs.size())
            && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }
};

bool value_variant_eq(const dl::value_vector& lhs,
                      const dl::value_vector& rhs)
noexcept (true) {
    return mpark::visit(variant_equal{}, lhs, rhs);
}

}

namespace dl {

error_severity decrease(error_severity s)  noexcept (true)
{
    /*
     * DEBUG is not even a true error, no reason to decrease it
     * INFO is the lowest level of error possible, can't be decreased
     * WARNING is severe enough that information that it happened should stay
     * ERROR has to be decreased if other items in hierarchy were processed
     */
    switch(s)
    {
        case error_severity::DEBUG:   return error_severity::DEBUG;
        case error_severity::INFO:    return error_severity::INFO;
        case error_severity::WARNING: return error_severity::WARNING;
        case error_severity::ERROR:   return error_severity::WARNING;
        default:
            throw std::runtime_error("decrease: unknown severity");
    }
}

std::string dlis_error::message() const noexcept (true) {
    /*
     * TODO: format. Without formatting nested errors look bad
     * note: regex::replace version should work from gcc 4.9
     * const std::string formatted_problem = std::regex_replace(
     * this->problem, std::regex("\n"), "\n  ");
     */
    const std::string problem = "Problem: " + this->problem + "." ;
    std::string spec = "";
    if (!this->specification.empty()) {
        spec = "\nSpecification reference: " + this->specification + ".";
    }
    std::string action = "";
    if (!this->action.empty()) {
        action = "\nTaken action: " + this->action + ".";
    }
    return problem + spec + action;
}

bool object_attribute::operator == (const object_attribute& o)
const noexcept (true) {
    return this->label == o.label
        && this->count == o.count
        && this->reprc == o.reprc
        && this->units == o.units
        // invariant doesn't matter for attribute equality,
        // so ignore it
        && value_variant_eq(this->value, o.value);
}

dl::ident obname::fingerprint(const std::string& type)
const noexcept (false) {
    const auto& origin = dl::decay(this->origin);
    const auto& copy = dl::decay(this->copy);
    const auto& id = dl::decay(this->id);

    int size;
    auto err = dlis_object_fingerprint_size(type.size(),
                                            type.data(),
                                            id.size(),
                                            id.data(),
                                            origin,
                                            copy,
                                            &size);

    if (err)
        throw std::invalid_argument("invalid argument");

    auto str = std::vector< char >(size);
    err = dlis_object_fingerprint(type.size(),
                                  type.data(),
                                  id.size(),
                                  id.data(),
                                  origin,
                                  copy,
                                  str.data());

    if (err)
        throw std::runtime_error("fingerprint: something went wrong");

    return dl::ident( std::string(str.begin(), str.end()) );
}

dl::ident objref::fingerprint() const noexcept (false) {
    return this->name.fingerprint(dl::decay(this->type));
}

void basic_object::set( const object_attribute& attr ) noexcept (false) {
    /*
     * This is essentially map::insert-or-update
     */
    const auto eq = [&]( const object_attribute& x ) {
        return attr.label == x.label;
    };

    auto itr = std::find_if( this->attributes.begin(),
                             this->attributes.end(),
                             eq );

    if (itr == this->attributes.end())
        this->attributes.push_back(attr);
    else
        *itr = attr;
}

void basic_object::remove( const object_attribute& attr ) noexcept (false) {
    /*
     * This is essentially map::remove
     */
    const auto eq = [&]( const object_attribute& x ) {
        return attr.label == x.label;
    };

    auto itr = std::remove_if( this->attributes.begin(),
                               this->attributes.end(),
                               eq );

    this->attributes.erase( itr, this->attributes.end() );
}

std::size_t basic_object::len() const noexcept (true) {
    return this->attributes.size();
}

const object_attribute&
basic_object::at( const std::string& key )
const noexcept (false)
{
    auto eq = [&key]( const dl::object_attribute& attr ) {
        return dl::decay( attr.label ) == key;
    };

    auto itr = std::find_if( this->attributes.begin(),
                             this->attributes.end(),
                             eq );

    if (itr == this->attributes.end())
        throw std::out_of_range( key );

    return *itr;
}

bool basic_object::operator == (const basic_object& o) const noexcept (true) {
    return this->object_name       == o.object_name
        && this->attributes.size() == o.attributes.size()
        && std::equal(this->attributes.begin(),
                      this->attributes.end(),
                      o.attributes.begin());
}

bool basic_object::operator != (const basic_object& o) const noexcept (true) {
    return !(*this == o);
}


const char* object_set::parse_template( const char* cur) noexcept (false) {
    const char* end = this->record.data.data() + this->record.data.size();

    while (true) {
        if (cur >= end)
            throw std::out_of_range( "unexpected end-of-record in template" );

        const auto flags = parse_attribute_descriptor( cur );
        if (flags.object) {
            return cur;
        }

        /* decriptor read, so advance the cursor */
        cur += DLIS_DESCRIPTOR_SIZE;

        if (flags.absent) {
            dlis_error err {
                dl::error_severity::WARNING,
                "Absent Attribute in object template",
                "3.2.2.2 Component Usage: A Template consists of a collection "
                    "of Attribute Components and/or Invariant Attribute "
                    "Components, mixed in any fashion.",
                "Skipped"
            };
            this->info.push_back(err);
            continue;
        }

        object_attribute attr;

        if (!flags.label) {
            dlis_error err {
                dl::error_severity::WARNING,
                "Label not set in template",
                "3.2.2.2 Component Usage: All Components in the Template must "
                    "have distinct, non-null Labels.",
                "Assumed descriptor corrupted, attempt to read label anyway"
            };
            this->info.push_back(err);
        }

                         cur = cast( cur, attr.label );
        if (flags.count) cur = cast( cur, attr.count );
        if (flags.reprc) cur = cast( cur, attr );
        if (flags.units) cur = cast( cur, attr.units );
        if (flags.value) cur = elements( cur, attr );
        attr.invariant = flags.invariant;

        this->tmpl.push_back( std::move( attr ) );

        if (cur == end){
            dlis_error err {
                dl::error_severity::DEBUG,
                "Set contains no objects",
                "3.2.2.2 Component Usage: A Set consists of one or more Objects",
                ""
            };
            this->info.push_back(err);
            return cur;
        }
    }
}

namespace {

basic_object defaulted_object( const object_template& tmpl ) noexcept (false) {
    basic_object def;
    for (const auto& attr : tmpl)
        def.set( attr );

    return def;

}

struct len {
    template < typename Vec >
    std::size_t operator () ( const Vec& vec ) const {
        return vec.size();
    }

    std::size_t operator () ( const mpark::monostate& ) const {
        throw std::invalid_argument( "patch: len() called on monostate" );
    }
};

struct shrink {
    std::size_t size;
    explicit shrink( std::size_t sz ) : size( sz ) {}

    template < typename T >
    void operator () ( std::vector< T >& vec ) const {
        vec.resize(this->size);
    }

    void operator () ( const mpark::monostate& ) const  {
        throw std::invalid_argument( "patch: shrink() called on monostate" );
    }
};

void patch_missing_value( dl::object_attribute& attr )
noexcept (false)
{
    const dl::representation_code reprc = attr.reprc;
    const std::size_t count             = dl::decay( attr.count );
    dl::value_vector& value             = attr.value;

    /*
     * value is *NOT* monostate, i.e. there is a default value.  if count !=
     * values.size(), resize it.
     */
    if (!mpark::holds_alternative< mpark::monostate >(value)) {
        const auto size = mpark::visit( len(), value );
        /* same size, so return */
        if (size == count) return;

        /* smaller, shrink and all is fine */
        if (size > count) {

            const auto msg = "Default value is not overridden, but new count "
                             "is. count (which is {}) < original count (which "
                             "is {})";

            mpark::visit( shrink( count ), value );
            dlis_error err {
                dl::error_severity::WARNING,
                fmt::format(msg, count, size),
                "3.2.2.1 Component Descriptor: The number of Elements that "
                    "make up the Value is specified by the Count "
                    "Characteristic.",
                "shrank default value to new count"
            };
            attr.info.push_back(err);
            return;
        }

        /*
         * count is larger, which makes little sense. Likely file is already
         * spoiled, but mark attribute as errored and attempt to continue
         */
        const auto msg = "Default value is not overridden, but new count "
                         "is. count (which is {}) > original count (which "
                         "is {})";

        dlis_error err {
            dl::error_severity::ERROR,
            fmt::format(msg, count, size),
            "3.2.2.1 Component Descriptor: The number of Elements that "
                "make up the Value is specified by the Count "
                "Characteristic.",
            "values is left as default. Continue processing"
        };
        attr.info.push_back(err);
    }

    /*
     * value *is* monstate, so initialize a default value that corresponds to
     * whatever type is there.
     *
     * 3.2.2 EFLR: Component Structure declares ident with the empty string as
     * a default type, but this is already stored in the defaulted reprc,
     * making this switch work in the general case
     */

    using rpc = dl::representation_code;
    switch (reprc) {
        case rpc::fshort: reset< dl::fshort >(value).resize(count); return;
        case rpc::fsingl: reset< dl::fsingl >(value).resize(count); return;
        case rpc::fsing1: reset< dl::fsing1 >(value).resize(count); return;
        case rpc::fsing2: reset< dl::fsing2 >(value).resize(count); return;
        case rpc::isingl: reset< dl::isingl >(value).resize(count); return;
        case rpc::vsingl: reset< dl::vsingl >(value).resize(count); return;
        case rpc::fdoubl: reset< dl::fdoubl >(value).resize(count); return;
        case rpc::fdoub1: reset< dl::fdoub1 >(value).resize(count); return;
        case rpc::fdoub2: reset< dl::fdoub2 >(value).resize(count); return;
        case rpc::csingl: reset< dl::csingl >(value).resize(count); return;
        case rpc::cdoubl: reset< dl::cdoubl >(value).resize(count); return;
        case rpc::sshort: reset< dl::sshort >(value).resize(count); return;
        case rpc::snorm:  reset< dl::snorm  >(value).resize(count); return;
        case rpc::slong:  reset< dl::slong  >(value).resize(count); return;
        case rpc::ushort: reset< dl::ushort >(value).resize(count); return;
        case rpc::unorm:  reset< dl::unorm  >(value).resize(count); return;
        case rpc::ulong:  reset< dl::ulong  >(value).resize(count); return;
        case rpc::uvari:  reset< dl::uvari  >(value).resize(count); return;
        case rpc::ident:  reset< dl::ident  >(value).resize(count); return;
        case rpc::ascii:  reset< dl::ascii  >(value).resize(count); return;
        case rpc::dtime:  reset< dl::dtime  >(value).resize(count); return;
        case rpc::origin: reset< dl::origin >(value).resize(count); return;
        case rpc::obname: reset< dl::obname >(value).resize(count); return;
        case rpc::objref: reset< dl::objref >(value).resize(count); return;
        case rpc::attref: reset< dl::attref >(value).resize(count); return;
        case rpc::status: reset< dl::status >(value).resize(count); return;
        case rpc::units:  reset< dl::units  >(value).resize(count); return;
        default: {
            // repcode is incorrect, but value is missing
            // hence we can just log error but can continue processing
            const auto msg = "value is declared, but representation code is "
                              "unknown {}, hence unable to interpret";
            const auto code = static_cast< int >(reprc);
            dl::dlis_error err {
                dl::error_severity::ERROR,
                fmt::format(msg, code),
                "Appendix B: Representation Codes",
                "attribute value is left as default. Continue processing"
            };
            attr.info.push_back(err);

        }
    }
}

/* Define parent error_severity by severity of children.
 * Find worst error among the children and decrease it by level to
 * report it on parent.
 *
 * Note: methods are basically the same, but unifying them requires sacrifies.
 * Nicest solution seems to be to add common inheritance hierarchy for all the
 * classes that can contain errors. But the way dlisio is designed means that
 * it would be the only inherited structure out there.
 * So in order to avoid introducing new concepts to the code, duplication is
 * introduced instead...
 */

void max_severity( dl::error_severity& severity,
                   const std::vector< dlis_error > infos)
noexcept (false)
{
    if (infos.size()) {
        for (const auto& info : infos) {
            severity = std::max(severity, info.severity);
        }
    }
}

dl::error_severity define_parental_severity(
    const std::vector< object_attribute > attrs) noexcept (false)
{
    dl::error_severity severity = dl::error_severity::DEBUG;
    for (const auto& attr: attrs) {
        max_severity(severity, attr.info);
    }
    return decrease(severity);
}

dl::error_severity define_parental_severity(
    const std::vector< basic_object > objs) noexcept (false)
{
    dl::error_severity severity = dl::error_severity::DEBUG;
    for (const auto& obj: objs) {
        max_severity(severity, obj.info);
    }
    return decrease(severity);
}

}

const char* object_set::parse_objects(const char* cur) noexcept (false) {

    const char* end = this->record.data.data() + this->record.data.size();
    const auto default_object = defaulted_object( tmpl );

    while (cur != end) {
        if (std::distance( cur, end ) < 0)
            throw std::out_of_range( "unexpected end-of-record" );

        auto object_flags = parse_object_descriptor( cur );
        cur += DLIS_DESCRIPTOR_SIZE;

        auto current = default_object;
        current.type = type;

        if (!object_flags.name) {
            dlis_error err {
                dl::error_severity::WARNING,
                "OBJECT:name was not set",
                "3.2.2.1 Component Descriptor: That is, every Object has "
                    "a non-null Name",
                "Assumed descriptor corrupted, attempt to read name anyway"
            };
            current.info.push_back(err);
        }

        cur = cast( cur, current.object_name );

        for (const auto& template_attr : tmpl) {
            if (template_attr.invariant) continue;
            if (cur == end) break;

            const auto flags = parse_attribute_descriptor( cur );
            if (flags.object) break;


            /*
             * only advance after this is surely not a new object, because if
             * it's the next object we want to read it again
             */
            cur += DLIS_DESCRIPTOR_SIZE;

            auto attr = template_attr;
            // absent means no meaning, so *unset* whatever is there
            if (flags.absent) {
                current.remove( attr );
                continue;
            }

            if (flags.invariant) {
                dlis_error err {
                    dl::error_severity::WARNING,
                    "Invariant attribute in object attributes",
                    "3.2.2.2 Component Usage: Invariant Attribute Components, "
                        "which may only appear in the Template [...]",
                    "ignored invariant bit, assumed that attribute followed"
                };
                attr.info.push_back(err);
            }

            if (flags.label) {
                dlis_error err {
                    dl::error_severity::WARNING,
                    "Label bit set in object attribute",
                    "3.2.2.2 Component Usage: Attribute Components that follow "
                        "Object Components must not have Attribute Labels",
                    "ignored label bit, assumed that label never followed"
                };
                attr.info.push_back(err);
            }

            if (flags.count) cur = cast( cur, attr.count );
            if (flags.reprc) cur = cast( cur, attr );
            if (flags.units) cur = cast( cur, attr.units );
            if (flags.value) cur = elements( cur, attr );

            const auto count = dl::decay( attr.count );

            /*
             * 3.2.2.1 Component Descriptor
             * When an object attribute count is zero, the value is explicitly
             * undefined, even if a default exists.
             *
             * This is functionally equivalent to the value being marked absent
             */
            if (count == 0) {
                attr.value = mpark::monostate{};
            } else if (!flags.value) {
                /*
                 * Count is non-zero, but there's no value for this attribute.
                 * Expand what's already defaulted, and if it is monostate, set
                 * the default of that value
                 *
                 * For non-zero count we should check default values only when
                 * representation code is not changed.
                 *
                 * TODO: in the future it's possible to allow promotion between
                 * certain codes (ident -> ascii), but is no need for now
                 */

                if (flags.reprc && attr.reprc != template_attr.reprc) {
                    const auto msg = "count ({}) isn't 0 and representation "
                        "code ({}) changed, but value is not explicitly set";
                    const auto code = static_cast< int >(attr.reprc);
                    dlis_error err {
                        dl::error_severity::WARNING,
                        fmt::format(msg, count, code),
                        "-",
                        "setting default value for new representation code"
                    };
                    attr.info.push_back(err);
                    attr.value = mpark::monostate{};
                }

                patch_missing_value( attr );
            }

            current.set(attr);
        }

        const auto severity = define_parental_severity(current.attributes);

        if (severity >= dl::error_severity::INFO){
            const auto msg = "Problems occurred on processing object. "
                             "Be careful when trusting retrieved data";
            dlis_error err { severity, msg, "", "" };
            current.info.push_back(err);
        }

        this->objs.push_back( std::move( current ) );
    }

    const auto severity = define_parental_severity(this->objs);
    if (severity >= dl::error_severity::INFO){
        const auto msg = "Problems occurred on processing object set. "
                         "Be careful when trusting retrieved data";
        dlis_error err { severity, msg, "", "" };
        this->info.push_back(err);
    }

    return cur;
}

const char* object_set::parse_set_component(const char* cur) noexcept (false) {

    const char* end = this->record.data.data() + this->record.data.size();
    if (std::distance( cur, end ) <= 0)
        throw std::out_of_range( "eflr must be non-empty" );

    const auto flags = parse_set_descriptor( cur );
    cur += DLIS_DESCRIPTOR_SIZE;

    if (std::distance( cur, end ) <= 0) {
        const auto msg = "unexpected end-of-record after SET descriptor";
        throw std::out_of_range( msg );
    }

    switch (flags.role) {
        case DLIS_ROLE_RDSET: {
            dlis_error err {
                dl::error_severity::INFO,
                "Redundant sets are not supported by dlisio",
                "3.2.2.2 Component Usage: A Redundant Set is an identical copy "
                    "of some Set written previously in the same Logical File",
                "Set will be processed as a usual one, which might lead to "
                    "issues with duplicated objects"
            };
            this->info.push_back(err);
            break;
        }
        case DLIS_ROLE_RSET: {
            dlis_error err {
                dl::error_severity::WARNING,
                "Replacement sets are not supported by dlisio",
                "3.2.2.2 Component Usage: Attributes of the Replacement Set "
                    "reflect all updates that may have been applied since the "
                    "original Set was written",
                "Set will be processed as a usual one, which might lead to "
                    "issues with duplicated objects and invalid information"
            };
            this->info.push_back(err);
            break;
        }
        default:
            break;
    }

    /*
     * TODO: check for every read that inside [begin,end)?
     */
    auto tmp_role = flags.role;

    dl::ident tmp_type;
    dl::ident tmp_name;

    if (!flags.type) {
        dlis_error err {
            dl::error_severity::WARNING,
            "SET:type not set",
            "3.2.2.1 Component Descriptor: A Set’s Type Characteristic must "
                "be non-null and must always be explicitly present in "
                "the Set Component",
            "Assumed descriptor corrupted, attempt to read type anyway"
        };
        this->info.push_back(err);
    }

                    cur = cast( cur, tmp_type );
    if (flags.name) cur = cast( cur, tmp_name );

    this->type = tmp_type;
    this->name = tmp_name;
    this->role = tmp_role;
    return cur;
}

object_set::object_set(dl::record rec) noexcept (false)  {
    this->record = std::move(rec);
    try {
        parse_set_component(this->record.data.data());
    } catch(const std::exception& e) {
        dlis_error err {
            dl::error_severity::ERROR,
            e.what(),
            "",
            "parsing set components interrupted"
        };
        report({err}, "object set creation: error on parsing types");
    }
}

void object_set::parse() noexcept (false) {
    if (this->parsed) return;

    const char* cur = this->record.data.data();

    try {
        /* As cursor value is not stored, read data again to get the position */
        cur = parse_set_component(cur);
        cur = parse_template(cur);
        cur = parse_objects(cur);
    } catch (const std::exception& e) {
        dlis_error err {
            dl::error_severity::ERROR,
            e.what(),
            "",
            "parse interrupted"
        };
        this->info.push_back(err);
        const auto object_set_id = "object set " + dl::decay(this->name) +
                                   " of type " + dl::decay(this->type);
        report({err}, object_set_id + " parse: error on parsing");
    }
    /* If set is parsed in default mode, exception will be thrown and set won't
     * be considered parsed. If set is parsed after that in error-escape mode,
     * parsing will be locked, but error will get stored on object set anyway.
     */
    this->parsed = true;
}

dl::object_vector& object_set::objects() noexcept (false) {
    this->parse();
    if (this->info.size()) {
        const auto msg = "Message from object set " + dl::decay(this->name)
                         + " of type " + dl::decay(this->type);
        report(this->info, msg);
    }
    return this->objs;
}

std::vector< dl::ident > pool::types() const noexcept (true) {
    std::vector< dl::ident > types;
    for (const auto& eflr : this->eflrs) {
        types.push_back( eflr.type );
    }
    return types;
}

object_vector pool::get(const std::string& type,
                        const std::string& name,
                        const dl::matcher& m)
noexcept (false) {
    object_vector objs;

    for (auto& eflr : this->eflrs) {
        if (not m.match(dl::ident{type}, eflr.type)) continue;

        for (const auto& obj : eflr.objects()) {
            if (not m.match(dl::ident{name}, obj.object_name.id)) continue;

            if (obj.info.size()) {
                const auto msg = "Message from object " +
                                  dl::decay(obj.object_name
                                     .fingerprint(dl::decay( obj.type )));
                report(obj.info, msg);
            }

            objs.push_back(obj);
        }
    }
    return objs;
}

object_vector pool::get(const std::string& type,
                        const dl::matcher& m)
noexcept (false) {
    object_vector objs;

    for (auto& eflr : this->eflrs) {
        if (not m.match(dl::ident{type}, eflr.type)) continue;

        auto tmp = eflr.objects();
        // do not report object errors from this method as method intention
        // is to return object sets rather than singular objects
        objs.insert(objs.end(), tmp.begin(), tmp.end());
    }
    return objs;
}

void report( const std::vector< dl::dlis_error >& codes,
             const std::string& context ) noexcept (false) {

    for (const auto& code : codes) {
        const std::string msg = "\nAt: " + context + "\n" + code.message();
        std::string level;

        if (dl::get_escape_level() < code.severity)
            throw std::runtime_error(msg);

        switch (code.severity) {
            case dl::error_severity::DEBUG:
                level = "debug";
                break;
            case dl::error_severity::INFO:
                level = "info";
                break;
            case dl::error_severity::ERROR:
                level = "error";
                break;
            case dl::error_severity::WARNING:
                level = "warning";
                break;
            default:
                throw std::runtime_error("Unknown severity ");
        }
        // translate msg to ident to deal with possible encoding issues
        get_logger().log(level, dl::ident(msg));
    }
}

}
