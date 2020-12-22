#ifndef DLISIO_TYPES_HPP
#define DLISIO_TYPES_HPP

#include <cstdint>

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

} // namespace dl

#endif //DLISIO_TYPES_HPP
