#ifndef DLISIO_STREAM_HPP
#define DLISIO_STREAM_HPP

#include <lfp/lfp.h>

namespace dl {

/* Stream - wrapper for lfp_protocol
 *
 * The main purpose of stream is to handle lfp return codes in a manner that
 * suits dlisio and make a more ergonomic interface for caller functions.
 */
class stream {
public:
    explicit stream( lfp_protocol* p ) : f(p) {};

    lfp_protocol* protocol() const noexcept (true);

    /* Seek within the logical domain of the current file. I.e. seeks are
     * performed with the outer-most lfp_protocol. */
    void lseek( std::int64_t offset ) noexcept (false);

    /* The tell reported by the current (outer-most) lfp_protocol */
    std::int64_t ltell() const noexcept(true);

    /* The tell reported by the inner-most lfp_protocol */
    std::int64_t ptell() const noexcept(false);

    std::int64_t read( char* dst, int n ) noexcept (false);

    void close() noexcept (true);
    int eof() const noexcept (true);
private:
    lfp_protocol* f;
};

}

#endif // DLISIO_STREAM_HPP
