#ifndef DLISIO_IO_HPP
#define DLISIO_IO_HPP

#include <array>
#include <string>
#include <tuple>
#include <vector>
#include <map>

#include <lfp/lfp.h>

#include <dlisio/stream.hpp>
#include <dlisio/types.hpp>
#include <dlisio/records.hpp>

namespace dl {

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