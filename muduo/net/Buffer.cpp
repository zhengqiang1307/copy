#include "muduo/net/Buffer.h"
#include "muduo/net/SocketsOps.h"

#include <errno.h>
#include <sys/uio.h>

using namespace muduo;
using namespace muduo::net;

const char Buffer::kCRLF[]="\r\n";

const size_t Buffer::kCheapPrepend;
const size_t Buffer::kInitialSize;

ssize_t Buffer::readFd(int fd, int* savedErrno)
{
    char extrabuff[65536];
    struct iovec vec[2];
    const size_t writable=writableBytes();
    vec[0].iov_base=begin()+writeIndex_;
    vec[0].iov_len=writable;
    vec[1].iov_base=extrabuff;
    vec[1].iov_len=sizeof (extrabuff);
    const int iovcnt=(writable<sizeof (extrabuff)?2:1);
    const ssize_t n=sockets::readv(fd,vec,iovcnt);
    if(n<0)*savedErrno=errno;
    else if (implicit_cast<size_t>(n) <=writable) {
        writeIndex_+=implicit_cast<unsigned long>(n);
    }
    else {
        writeIndex_=buffer_.size();
        append(extrabuff,implicit_cast<unsigned long>(n)-writable);
    }
    return n;
}
