#include "muduo/net/SocketsOps.h"
#include "muduo/base/Logging.h"
#include "muduo/base/Types.h"
#include "muduo/net/Endian.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>  // snprintf
#include <sys/socket.h>
#include <sys/uio.h>  // readv
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

// /* Structure describing a generic socket address.  */
// struct sockaddr
// {
//  uint16 sa_family;           /* Common data: address family and length.  */
//  char sa_data[14];           /* Address data.  */
// };

// /* Structure describing an Internet socket address.  */
// struct sockaddr_in
// {
//  uint16 sin_family;          /* Address family AF_INET */
//  uint16 sin_port;            /* Port number.  */
//  uint32 sin_addr.s_addr;     /* Internet address.  */
//  unsigned char sin_zero[8];  /* Pad to size of `struct sockaddr'.  */
// };

// /* Ditto, for IPv6.  */
// struct sockaddr_in6
// {
//  uint16 sin6_family;         /* Address family AF_INET6 */
//  uint16 sin6_port;           /* Transport layer port # */
//  uint32 sin6_flowinfo;       /* IPv6 flow information */
//  uint8  sin6_addr[16];       /* IPv6 address */
//  uint32 sin6_scope_id;       /* IPv6 scope-id */
// };

namespace {
#if VALGRIND || defined(NO_ACCEPT4)
void setNonBlockAndCloseOnExec(int sockfd) {
  // non-block
  int flags = ::fcntl(sockfd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  int ret = ::fcntl(sockfd, F_SETFL, flags);

  // close-on-exec
  flags = ::fcntl(sockfd, F_GETFL, 0);
  flags |= FD_CLOEXEC;
  ret = ::fcntl(sockfd, F_SETFL, flags);
}
#endif
}  // namespace

const sockaddr *sockets::sockaddr_cast(const sockaddr_in *addr) {
  return static_cast<const struct sockaddr *>(
      implicit_cast<const void *>(addr));
}

const sockaddr *sockets::sockaddr_cast(const sockaddr_in6 *addr) {
  return static_cast<const struct sockaddr *>(
      implicit_cast<const void *>(addr));
}

sockaddr *sockets::sockaddr_cast(sockaddr_in6 *addr) {
  return static_cast<struct sockaddr *>(implicit_cast<void *>(addr));
}

const sockaddr_in *sockets::sockaddr_in_cast(const sockaddr *addr) {
  return static_cast<const struct sockaddr_in *>(
      implicit_cast<const void *>(addr));
}

const sockaddr_in6 *sockets::sockaddr_in6_cast(const sockaddr *addr) {
  return static_cast<const struct sockaddr_in6 *>(
      implicit_cast<const void *>(addr));
}

//#include <sys/socket.h>
// int socket(int domain , int type , int protocol );
// Returns file descriptor on success, or –1 on error

int sockets::createNonblockingOrDie(sa_family_t family) {
  int fd = ::socket(family, SOCK_STREAM | O_NONBLOCK | SOCK_CLOEXEC, 0);
  if (fd < 0) LOG_SYSFATAL << "sockets::createNonblockingOrDie";
  return fd;
}

//#include <sys/socket.h>
// int bind(int sockfd , const struct sockaddr * addr , socklen_t addrlen );
// Returns 0 on success, or –1 on error

/*
struct sockaddr {
sa_family_t sa_family; // Address family (AF_* constant)
char sa_data[14]; // Socket address (size varies according to socket domain)
};
*/

void sockets::bindOrDie(int socket, const sockaddr *addr) {
  int ret =
      ::bind(socket, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
  if (ret < 0) LOG_SYSFATAL << "sockets::bindOrDie";
}

//#include <sys/socket.h>
// int listen(int sockfd , int backlog );
// Returns 0 on success, or –1 on error

void sockets::listenOrDie(int sockfd) {
  int ret = ::listen(sockfd, SOMAXCONN);
  if (ret < 0) LOG_SYSFATAL << "sockets::listenOrDie";
}

//#include <sys/socket.h>
// int accept(int sockfd , struct sockaddr * addr , socklen_t * addrlen );
// Returns file descriptor on success, or –1 on error

int sockets::accept(int sockfd, sockaddr_in6 *addr) {
  socklen_t addrlen = static_cast<socklen_t>(sizeof(*addr));
  int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen,
                         SOCK_CLOEXEC | SOCK_NONBLOCK);
  if (connfd < 0) {
    int savedErrno = errno;
    LOG_ERROR << "sockets::accept";
    switch (savedErrno) {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO:  // ???
      case EPERM:
      case EMFILE:  // per-process lmit of open file desctiptor ???
        // expected errors
        errno = savedErrno;
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        // unexpected errors
        LOG_FATAL << "unexpected error of ::accept " << savedErrno;
        break;
      default:
        LOG_FATAL << "unknown error of ::accept " << savedErrno;
        break;
    }
  }
  return connfd;
}

//#include <sys/socket.h>
// int connect(int sockfd , const struct sockaddr * addr , socklen_t addrlen );
// Returns 0 on success, or –1 on error

// If connect() fails and we wish to reattempt the connection, then SUSv3
// specifies that the portable method of doing so is to close the socket, create
// a new socket, and reattempt the connection with the new socket.

int sockets::connect(int socket, const sockaddr *addr) {
  int ret = ::connect(socket, addr,
                      static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
  if (ret < 0) LOG_FATAL << "sockets::connect";
  return ret;
}

ssize_t sockets::read(int sockfd, void *buf, size_t count) {
  return ::read(sockfd, buf, count);
}

ssize_t sockets::readv(int sockfd, const iovec *iov, int iovcnt) {
  return ::readv(sockfd, iov, iovcnt);
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count) {
  return ::write(sockfd, buf, count);
}

//#include <unistd.h>

//       int close(int fd);
void sockets::close(int sockfd) {
  if (::close(sockfd) < 0) LOG_SYSERR << "sockets::close";
}

//#include <sys/socket.h>

// int shutdown(int socket, int how);
// SHUT_RD
//        Disables further receive operations.
// SHUT_WR
//        Disables further send operations.
// SHUT_RDWR
//        Disables further send and receive operations.

void sockets::shutdownWrite(int sockfd) {
  if (::shutdown(sockfd, SHUT_WR) < 0) LOG_SYSERR << "sockets::shutdownWrite";
}

void sockets::toIpPort(char *buf, size_t size, const sockaddr *addr) {
  const sockaddr_in *addr_in4 = sockaddr_in_cast(addr);
  uint16_t port = networkToHost16(addr_in4->sin_port);
  sockets::toIp(buf, size, addr);
  size_t off = strlen(buf);
  assert(size > off);
  snprintf(buf + off, size - off, ":%u", port);
}

//#include <arpa/inet.h>
// const char *inet_ntop(int af, const void *src,                      char
//*dst, socklen_t size);
void sockets::toIp(char *buf, size_t size, const sockaddr *addr) {
  if (addr->sa_family == AF_INET) {
    const sockaddr_in *addr_in4 = sockaddr_in_cast(addr);
    assert(size >= INET_ADDRSTRLEN);
    ::inet_ntop(AF_INET, &(addr_in4->sin_addr), buf, static_cast<socklen_t>(size));
  } else if (addr->sa_family == AF_INET6) {
    const sockaddr_in6 *addr_in6 = sockaddr_in6_cast(addr);
    assert(size > INET6_ADDRSTRLEN);
    ::inet_ntop(AF_INET6, &(addr_in6->sin6_addr), buf, static_cast<socklen_t>(size));
  }
}
//#include <arpa/inet.h>

// int inet_pton(int af, const char *src, void *dst);
void sockets::fromIpPort(const char *ip, uint16_t port, sockaddr_in *addr) {
  addr->sin_family = AF_INET;
  addr->sin_port = networkToHost16(port);
  if (::inet_pton(AF_INET, ip, &(addr->sin_addr)) <= 0)
    LOG_SYSERR << "sockets::fromIpPort";
}

void sockets::fromIpPort(const char *ip, uint16_t port,
                         struct sockaddr_in6 *addr) {
  addr->sin6_family = AF_INET6;
  addr->sin6_port = networkToHost16(port);
  if (::inet_pton(AF_INET6, ip, &(addr->sin6_addr)) <= 0)
    LOG_SYSERR << "sockets::fromIpPort";
}

//int sockets::getSocketError(int sockfd) {}

//sockaddr_in6 sockets::getLocalAddr(int sockfd) {}

//sockaddr_in6 sockets::getPeerAddr(int sockfd) {}

//bool sockets::isSelfConnect(int sockfd) {}
