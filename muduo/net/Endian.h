#ifndef ENDIAN_H
#define ENDIAN_H

#include <endian.h>
#include <stdint.h>

namespace muduo {
namespace net {
namespace sockets {
// disable warnings for a while
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
inline uint64_t hostToNetwork64(uint64_t host64) { return htobe64(host64); }

inline uint32_t hostToNetwork32(uint32_t host) { return htobe32(host); }

inline uint16_t hostToNetwork16(uint16_t host) { return htobe16(host); }

inline uint64_t networkToHost64(uint64_t network) { return be64toh(network); }

inline uint32_t networkToHost32(uint32_t network) { return be32toh(network); }

inline uint16_t networkToHost16(uint16_t network) { return be16toh(network); }
#pragma GCC diagnostic pop
}  // namespace sockets
}  // namespace net
}  // namespace muduo

#endif  // ENDIAN_H
