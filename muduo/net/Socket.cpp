#include "muduo/net/Socket.h"
#include <netinet/tcp.h>
#include "muduo/base/Logging.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/SocketsOps.h"

namespace muduo {
namespace net {
Socket::~Socket() { sockets::close(sockfd_); }

bool Socket::getTcpInfo(tcp_info *tcpi) const {
  socklen_t len = sizeof(tcp_info);
  memZero(tcpi, len);
  return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
}

bool Socket::getTcpInfoString(char *buf, int len) const {
  tcp_info tcpi;
  bool ok = getTcpInfo(&tcpi);
  if (ok) {
    snprintf(
        buf, static_cast<size_t>(len),
        "unrecovered=%u "
        "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
        "lost=%u retrans=%u rtt=%u rttvar=%u "
        "sshthresh=%u cwnd=%u total_retrans=%u",
        tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
        tcpi.tcpi_rto,          // Retransmit timeout in usec
        tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
        tcpi.tcpi_snd_mss, tcpi.tcpi_rcv_mss,
        tcpi.tcpi_lost,     // Lost packets
        tcpi.tcpi_retrans,  // Retransmitted packets out
        tcpi.tcpi_rtt,      // Smoothed round trip time in usec
        tcpi.tcpi_rttvar,   // Medium deviation
        tcpi.tcpi_snd_ssthresh, tcpi.tcpi_snd_cwnd,
        tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
  }
  return ok;
}

void Socket::bindAddress(const muduo::net::InetAddress &localAddr) {
  sockets::bindOrDie(sockfd_, localAddr.getSockAddr());
}

void Socket::listen() { sockets::listenOrDie(sockfd_); }

int Socket::accept(InetAddress *peeraddr) {
  sockaddr_in6 addr;
  memZero(&addr, sizeof(addr));
  int conn = sockets::accept(sockfd_, &addr);
  if (conn >= 0) {
    peeraddr->setSockAddrInet6(addr);
  }
  return conn;
}

void Socket::shutdownWrite() { sockets::shutdownWrite(sockfd_); }

void Socket::setTcpNoDelay(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}

void Socket::setReuseAddr(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}

void Socket::setReusePort(bool on)
{
#ifdef SO_REUSEPORT
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                         &optval, static_cast<socklen_t>(sizeof optval));
  if (ret < 0 && on)
  {
    LOG_SYSERR << "SO_REUSEPORT failed.";
  }
#else
  if (on)
  {
    LOG_ERROR << "SO_REUSEPORT is not supported.";
  }
#endif
}

void Socket::setKeepAlive(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}

}  // namespace net

}  // namespace muduo
