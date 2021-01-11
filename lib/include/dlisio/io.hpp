#ifndef DLISIO_IO_HPP
#define DLISIO_IO_HPP

#include <array>
#include <string>
#include <tuple>
#include <vector>
#include <map>

#include <lfp/lfp.h>

#include <dlisio/types.hpp>
#include <dlisio/records.hpp>

namespace dl {

/* Stream - wrapper for lfp_protocol
 *
 * The main purpose of stream is to handle lfp return codes in a manner that
 * suits dlisio and make a more ergonomic interface for caller functions.
 */
class stream {
public:
    explicit stream( lfp_protocol* p ) : f(p) {};

    void close();
    int eof() const noexcept (true);
    lfp_protocol* protocol() const noexcept (true);

    /* Seek within the logical domain of the current file. I.e. seeks are
     * performed with the outer-most lfp_protocol. */
    void lseek( std::int64_t offset ) noexcept (false);

    /* The tell reported by the current (outer-most) lfp_protocol */
    std::int64_t ltell() const noexcept(true);

    /* The tell reported by the inner-most lfp_protocol */
    std::int64_t ptell() const noexcept(false);

    std::int64_t read( char* dst, int n ) noexcept (false);

private:
    lfp_protocol* f;
};


struct stream_offsets {
    std::vector< long long > explicits;
    std::vector< long long > implicits;
    std::vector< long long > broken;
};

stream open(const std::string&, std::int64_t) noexcept (false);
stream open_rp66(const stream&) noexcept (false);
stream open_tapeimage(const stream&) noexcept (false);

long long findsul(stream&) noexcept (false);
long long findvrl(stream&, long long) noexcept (false);
bool hastapemark(stream&) noexcept (false);

dl::record extract(stream&, long long, dl::error_handler&) noexcept (false);
dl::record& extract(stream&, long long, long long, dl::record&,
    dl::error_handler&) noexcept (false);

stream_offsets findoffsets(dl::stream&, dl::error_handler&) noexcept (false);

std::map< dl::ident, std::vector< long long > >
findfdata(dl::stream&, const std::vector< long long >&, dl::error_handler&)
noexcept (false);

}

#endif // DLISIO_IO_HPP
