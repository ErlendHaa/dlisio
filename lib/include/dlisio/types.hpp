#ifndef DLISIO_TYPES_HPP
#define DLISIO_TYPES_HPP

#include <cstdint>
#include <string>
#include <complex>

#include "strong-typedef.hpp"

namespace dl {
/*
 * A family of primitive functions to parse the data types specified by RP66.
 * They all return a pointer to the first character NOT consumed by calling
 * this function.
 *
 * If the output argument is NULL it is not written, but distance is still
 * calculated. This is necessary for variable-length strings to ensure a large
 * enough buffer. Strings will NOT be null terminated
 */

const char* sshort_frombytes(const char*, int8_t*);
const char* snorm_frombytes( const char*, int16_t*);
const char* slong_frombytes( const char*, int32_t*);

const char* ushort_frombytes(const char*, uint8_t*);
const char* unorm_frombytes( const char*, uint16_t*);
const char* ulong_frombytes( const char*, uint32_t*);

const char* fshort_frombytes(const char*, float*);
const char* fsingl_frombytes(const char*, float*);
const char* fdoubl_frombytes(const char*, double*);

/* IBM and VAX floats */
const char* isingl_frombytes(const char*, float*);
const char* vsingl_frombytes(const char*, float*);

/* complex or validated floats */
const char* fsing1_frombytes(const char*, float* V, float* A);
const char* fsing2_frombytes(const char*, float* V, float* A, float* B);
const char* csingl_frombytes(const char*, float* R, float* I);

const char* fdoub1_frombytes(const char*, double* V, double* A);
const char* fdoub2_frombytes(const char*, double* V, double* A, double* B);
const char* cdoubl_frombytes(const char*, double* R, double* I);

const char* uvari_frombytes(const char*, int32_t* out);

const char* ident_frombytes(const char*, int32_t* len, char* out);
const char* ascii_frombytes(const char*, int32_t* len, char* out);

constexpr int TZ_LST = 0; // local standard
constexpr int TZ_DST = 1; // local daylight savings
constexpr int TZ_GMT = 2; // greenwich mean time

constexpr int YEAR_ZERO = 1900;
const char* dtime_frombytes(const char*, int* Y,
                                         int* TZ,
                                         int* M,
                                         int* D,
                                         int* H,
                                         int* MN,
                                         int* S,
                                         int* MS);

const char* origin_frombytes( const char*, int32_t* out );

/* obname = { origin, ushort, ident } */
const char* obname_frombytes( const char*, int32_t* origin,
                                           uint8_t* copy_number,
                                           int32_t* idlen,
                                           char* identifier );

/* objref = { ident, obname } */
const char* objref_frombytes( const char*, int32_t* ident_len,
                                           char* ident,
                                           int32_t* origin,
                                           uint8_t* copy_number,
                                           int32_t* objname_len,
                                           char* identifier );

/* attref = { ident, obname, ident } */
const char* attref_frombytes( const char*, int32_t* ident1_len,
                                           char* ident1,
                                           int32_t* origin,
                                           uint8_t* copy_number,
                                           int32_t* objname_len,
                                           char* identifier,
                                           int32_t* ident2_len,
                                           char* ident2 );

/* status is a boolean */
const char* status_frombytes(const char*, uint8_t*);
const char* units_frombytes(const char*, int32_t*, char*);

/*
 * A family of the reverse operation, i.e. transform a native data type to an
 * RP66 compatible one.
 *
 * They share the same name as the parsing functions, except for a trailing o.
 *
 * Unlike the input functions, they work on fixed-size unsigned numbers as
 * buffers, to make return-values more ergonomic
 */
void* sshort_tobytes(void*, int8_t);
void* snorm_tobytes( void*, int16_t);
void* slong_tobytes( void*, int32_t);

void* ushort_tobytes(void*, uint8_t);
void* unorm_tobytes( void*, uint16_t);
void* ulong_tobytes( void*, uint32_t);

void* fsingl_tobytes(void*, float);
void* fdoubl_tobytes(void*, double);

/* IBM and VAX floats */
void* isingl_tobytes(void*, float);
void* vsingl_tobytes(void*, float);

/* complex or validated floats */
void* fsing1_tobytes(void*, float, float);
void* fsing2_tobytes(void*, float, float, float);
void* csingl_tobytes(void*, float, float);

void* fdoub1_tobytes(void*, double, double);
void* fdoub2_tobytes(void*, double, double, double);
void* cdoubl_tobytes(void*, double, double);

void* uvari_tobytes( void*, int32_t, int width );

void* ident_tobytes(void*, uint8_t len, const char* in);
void* ascii_tobytes(void*, int32_t len, const char* in, std::uint8_t);

void* origin_tobytes(void*, int32_t);
void* status_tobytes(void*, uint8_t);

void* dtime_tobytes( void*, int Y,
                             int TZ,
                             int M,
                             int D,
                             int H,
                             int MN,
                             int S,
                             int MS );

/* obname = { origin, ushort, ident } */
void* obname_tobytes( void*, int32_t origin,
                              uint8_t copy_number,
                              uint8_t idlen,
                              const char* identifier );

/* objref = { ident, obname } */
void* objref_tobytes( void*, uint8_t ident_len,
                              const char* ident,
                              int32_t origin,
                              uint8_t copy_number,
                              uint8_t objname_len,
                              const char* identifier );

/* attref = { ident, obname, ident } */
void* attref_tobytes( void*, uint8_t ident1_len,
                              const char* ident1,
                              int32_t origin,
                              uint8_t copy_number,
                              uint8_t objname_len,
                              const char* identifier,
                              uint8_t ident2_len,
                              const char* ident2 );

void* units_tobytes( void*, uint8_t len, const char* in );

enum class representation_code : std::uint8_t {
    fshort = 1,  // Low precision floating point
    fsingl = 2,  // IEEE single precision floating point
    fsing1 = 3,  // Validated single precision floating point
    fsing2 = 4,  // Two-way validated single precision floating point
    isingl = 5,  // IBM single precision floating point
    vsingl = 6,  // VAX single precision floating point
    fdoubl = 7,  // IEEE double precision floating point
    fdoub1 = 8,  // Validated double precision floating point
    fdoub2 = 9,  // Two-way validated double precision floating point
    csingl = 10, // Single precision complex
    cdoubl = 11, // Double precision complex
    sshort = 12, // Short signed integer
    snorm  = 13, // Normal signed integer
    slong  = 14, // Long signed integer
    ushort = 15, // Short unsigned integer
    unorm  = 16, // Normal unsigned integer
    ulong  = 17, // Long unsigned integer
    uvari  = 18, // Variable-length unsigned integer
    ident  = 19, // Variable-length identifier
    ascii  = 20, // Variable-length ASCII character string
    dtime  = 21, // Date and time
    origin = 22, // Origin reference
    obname = 23, // Object name
    objref = 24, // Object reference
    attref = 25, // Attribute reference
    status = 26, // Boolean status
    units  = 27, // Units expression
    undef  = 66, // Undefined value
};

/*
 * get the size (in bytes) of a particular data type. Expects a UNORM or
 * similar type code.
 *
 * Returns a negative value passed an invalid type code, ie.
 * representation_code::undef
 */

int sizeof_type(representation_code);

constexpr int VARIABLE_LENGTH = 0;

// values below represent size on disk (in bytes)
constexpr int SIZEOF_FSHORT = 2;
constexpr int SIZEOF_FSINGL = 4;
constexpr int SIZEOF_FSING1 = 8;
constexpr int SIZEOF_FSING2 = 12;
constexpr int SIZEOF_ISINGL = 4;
constexpr int SIZEOF_VSINGL = 4;
constexpr int SIZEOF_FDOUBL = 8;
constexpr int SIZEOF_FDOUB1 = 16;
constexpr int SIZEOF_FDOUB2 = 24;
constexpr int SIZEOF_CSINGL = 8;
constexpr int SIZEOF_CDOUBL = 16;
constexpr int SIZEOF_SSHORT = 1;
constexpr int SIZEOF_SNORM  = 2;
constexpr int SIZEOF_SLONG  = 4;
constexpr int SIZEOF_USHORT = 1;
constexpr int SIZEOF_UNORM  = 2;
constexpr int SIZEOF_ULONG  = 4;
constexpr int SIZEOF_UVARI  = VARIABLE_LENGTH;
constexpr int SIZEOF_IDENT  = VARIABLE_LENGTH;
constexpr int SIZEOF_ASCII  = VARIABLE_LENGTH;
constexpr int SIZEOF_DTIME  = 8;
constexpr int SIZEOF_ORIGIN = VARIABLE_LENGTH;
constexpr int SIZEOF_OBNAME = VARIABLE_LENGTH;
constexpr int SIZEOF_OBJREF = VARIABLE_LENGTH;
constexpr int SIZEOF_ATTREF = VARIABLE_LENGTH;
constexpr int SIZEOF_STATUS = 1;
constexpr int SIZEOF_UNITS  = VARIABLE_LENGTH;

/*
 * It's _very_ often necessary to access the raw underlying type of the strong
 * type aliases for comparisons, literals, or conversions. dl::decay inspects
 * the argument type and essentially static casts it, regardless of which dl
 * type comes in - it's an automation of static_cast<const value_type&>(x)
 * which otherwise would be repeated a million times
 *
 * The stong typedef's using from detail::strong_typedef has a more specific
 * overload available - the templated version is for completeness.
 */
template < typename T >
T& decay( T& x ) noexcept (true) {
    return x;
}

#define DLIS_REGISTER_TYPEALIAS(name, type) \
    struct name : detail::strong_typedef< name, type > { \
        name() = default; \
        name( const name& ) = default; \
        name( name&& )      = default; \
        name& operator = ( const name& ) = default; \
        name& operator = ( name&& )      = default; \
        using detail::strong_typedef< name, type >::strong_typedef; \
        using detail::strong_typedef< name, type >::operator =; \
        using detail::strong_typedef< name, type >::operator ==; \
        using detail::strong_typedef< name, type >::operator !=; \
    }; \
    inline const type& decay(const name& x) noexcept (true) { \
        return static_cast< const type& >(x); \
    } \
    inline type& decay(name& x) noexcept (true) { \
        return static_cast< type& >(x); \
    }

DLIS_REGISTER_TYPEALIAS(fshort, float)
DLIS_REGISTER_TYPEALIAS(isingl, float)
DLIS_REGISTER_TYPEALIAS(vsingl, float)
DLIS_REGISTER_TYPEALIAS(uvari,  std::int32_t)
DLIS_REGISTER_TYPEALIAS(origin, std::int32_t)
DLIS_REGISTER_TYPEALIAS(ident,  std::string)
DLIS_REGISTER_TYPEALIAS(ascii,  std::string)
DLIS_REGISTER_TYPEALIAS(units,  std::string)
DLIS_REGISTER_TYPEALIAS(status, std::uint8_t)

#undef DLIS_REGISTER_TYPEALIAS

template< typename T, int > struct validated;
template< typename T >
struct validated< T, 2 > {
    T V, A;

    bool operator == (const validated& o) const noexcept (true) {
        return this->V == o.V && this->A == o.A;
    }

    bool operator != (const validated& o) const noexcept (true) {
        return !(*this == o);
    }
};
template< typename T > struct validated< T, 3 > {
    T V, A, B;

    bool operator == (const validated& o) const noexcept (true) {
        return this->V == o.V && this->A == o.A && this->B == o.B;
    }

    bool operator != (const validated& o) const noexcept (true) {
        return !(*this == o);
    }
};

using fsing1 = validated< float, 2 >;
using fsing2 = validated< float, 3 >;
using fdoub1 = validated< double, 2 >;
using fdoub2 = validated< double, 3 >;

using ushort = std::uint8_t;
using unorm  = std::uint16_t;
using ulong  = std::uint32_t;

using sshort = std::int8_t;
using snorm  = std::int16_t;
using slong  = std::int32_t;

using fsingl = float;
using fdoubl = double;

using csingl = std::complex< fsingl >;
using cdoubl = std::complex< fdoubl >;

struct dtime {
    int Y, TZ, M, D, H, MN, S, MS;

    bool operator == (const dtime& o) const noexcept (true);
    bool operator != (const dtime& o) const noexcept (true);
};

struct obname {
    dl::origin origin;
    dl::ushort copy;
    dl::ident  id;

    bool operator == ( const obname& rhs ) const noexcept (true);
    bool operator != (const obname& o) const noexcept (true);
    dl::ident fingerprint(const std::string& type) const noexcept (false);
};

struct objref {
    dl::ident  type;
    dl::obname name;

    bool operator == ( const objref& rhs ) const noexcept( true );
    bool operator != (const objref& o) const noexcept (true);
    dl::ident fingerprint() const noexcept (false);
};

struct attref {
    dl::ident  type;
    dl::obname name;
    dl::ident  label;

    bool operator == ( const attref& rhs ) const noexcept( true );
    bool operator != (const attref& o) const noexcept (true);
};

/*
 * Register useful compile time information on the types for other template
 * functions to hook into
 */

template < typename T > struct typeinfo;
template <> struct typeinfo< dl::fshort > {
    static const representation_code reprc = dl::representation_code::fshort;
    constexpr static const char name[] = "fshort";
};
template <> struct typeinfo< dl::fsingl > {
    static const representation_code reprc = dl::representation_code::fsingl;
    constexpr static const char name[] = "fsingl";
};
template <> struct typeinfo< dl::fsing1 > {
    static const representation_code reprc = dl::representation_code::fsing1;
    constexpr static const char name[] = "fsing1";
};
template <> struct typeinfo< dl::fsing2 > {
    static const representation_code reprc = dl::representation_code::fsing2;
    constexpr static const char name[] = "fsing2";
};
template <> struct typeinfo< dl::isingl > {
    static const representation_code reprc = dl::representation_code::isingl;
    constexpr static const char name[] = "isingl";
};
template <> struct typeinfo< dl::vsingl > {
    static const representation_code reprc = dl::representation_code::vsingl;
    constexpr static const char name[] = "vsingl";
};
template <> struct typeinfo< dl::fdoubl > {
    static const representation_code reprc = dl::representation_code::fdoubl;
    constexpr static const char name[] = "fdoubl";
};
template <> struct typeinfo< dl::fdoub1 > {
    static const representation_code reprc = dl::representation_code::fdoub1;
    constexpr static const char name[] = "fdoub1";
};
template <> struct typeinfo< dl::fdoub2 > {
    static const representation_code reprc = dl::representation_code::fdoub2;
    constexpr static const char name[] = "fdoub2";
};
template <> struct typeinfo< dl::csingl > {
    static const representation_code reprc = dl::representation_code::csingl;
    constexpr static const char name[] = "csingl";
};
template <> struct typeinfo< dl::cdoubl > {
    static const representation_code reprc = dl::representation_code::cdoubl;
    constexpr static const char name[] = "cdoubl";
};
template <> struct typeinfo< dl::sshort > {
    static const representation_code reprc = dl::representation_code::sshort;
    constexpr static const char name[] = "sshort";
};
template <> struct typeinfo< dl::snorm > {
    static const representation_code reprc = dl::representation_code::snorm;
    constexpr static const char name[] = "snorm";
};
template <> struct typeinfo< dl::slong > {
    static const representation_code reprc = dl::representation_code::slong;
    constexpr static const char name[] = "slong";
};
template <> struct typeinfo< dl::ushort > {
    static const representation_code reprc = dl::representation_code::ushort;
    constexpr static const char name[] = "ushort";
};
template <> struct typeinfo< dl::unorm > {
    static const representation_code reprc = dl::representation_code::unorm;
    constexpr static const char name[] = "unorm";
};
template <> struct typeinfo< dl::ulong > {
    static const representation_code reprc = dl::representation_code::ulong;
    constexpr static const char name[] = "ulong";
};
template <> struct typeinfo< dl::uvari > {
    static const representation_code reprc = dl::representation_code::uvari;
    constexpr static const char name[] = "uvari";
};
template <> struct typeinfo< dl::ident > {
    static const representation_code reprc = dl::representation_code::ident;
    constexpr static const char name[] = "ident";
};
template <> struct typeinfo< dl::ascii > {
    static const representation_code reprc = dl::representation_code::ascii;
    constexpr static const char name[] = "ascii";
};
template <> struct typeinfo< dl::dtime > {
    static const representation_code reprc = dl::representation_code::dtime;
    constexpr static const char name[] = "dtime";
};
template <> struct typeinfo< dl::origin > {
    static const representation_code reprc = dl::representation_code::origin;
    constexpr static const char name[] = "origin";
};
template <> struct typeinfo< dl::obname > {
    static const representation_code reprc = dl::representation_code::obname;
    constexpr static const char name[] = "obname";
};
template <> struct typeinfo< dl::objref > {
    static const representation_code reprc = dl::representation_code::objref;
    constexpr static const char name[] = "objref";
};
template <> struct typeinfo< dl::attref > {
    static const representation_code reprc = dl::representation_code::attref;
    constexpr static const char name[] = "attref";
};
template <> struct typeinfo< dl::status > {
    static const representation_code reprc = dl::representation_code::status;
    constexpr static const char name[] = "status";
};
template <> struct typeinfo< dl::units > {
    static const representation_code reprc = dl::representation_code::units;
    constexpr static const char name[] = "units";
};

const char* cast( const char* xs, sshort& ) noexcept (true);
const char* cast( const char* xs, snorm& )  noexcept (true);
const char* cast( const char* xs, slong& )  noexcept (true);
const char* cast( const char* xs, ushort& ) noexcept (true);
const char* cast( const char* xs, unorm& )  noexcept (true);
const char* cast( const char* xs, ulong& )  noexcept (true);
const char* cast( const char* xs, uvari& )  noexcept (true);
const char* cast( const char* xs, fshort& ) noexcept (true);
const char* cast( const char* xs, fsingl& ) noexcept (true);
const char* cast( const char* xs, fdoubl& ) noexcept (true);
const char* cast( const char* xs, fsing1& ) noexcept (true);
const char* cast( const char* xs, fsing2& ) noexcept (true);
const char* cast( const char* xs, fdoub1& ) noexcept (true);
const char* cast( const char* xs, fdoub2& ) noexcept (true);
const char* cast( const char* xs, csingl& ) noexcept (true);
const char* cast( const char* xs, cdoubl& ) noexcept (true);
const char* cast( const char* xs, isingl& ) noexcept (true);
const char* cast( const char* xs, vsingl& ) noexcept (true);
const char* cast( const char* xs, status& ) noexcept (true);
const char* cast( const char* xs, ident& )  noexcept (false);
const char* cast( const char* xs, units& )  noexcept (false);
const char* cast( const char* xs, ascii& )  noexcept (false);
const char* cast( const char* xs, origin& ) noexcept (true);
const char* cast( const char* xs, obname& ) noexcept (false);
const char* cast( const char* xs, objref& ) noexcept (false);
const char* cast( const char* xs, attref& ) noexcept (false);
const char* cast( const char* xs, dtime& )  noexcept (true);
const char* cast( const char* xs, representation_code& reprc )
noexcept (false);

} // namespace dl

#endif //DLISIO_TYPES_HPP
