#ifndef LU_ERRNO_H_
#define LU_ERRNO_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifndef EPERM
#define EPERM 1
#endif

#ifndef ENOENT
#define ENOENT 2
#endif

#ifndef ENOFILE
#define ENOFILE ENOENT
#endif

#ifndef ESRCH
#define ESRCH 3
#endif

#ifndef EINTR
#define EINTR 4
#endif

#ifndef EIO
#define EIO 5
#endif

#ifndef ENXIO
#define ENXIO 6
#endif

#ifndef E2BIG
#define E2BIG 7
#endif

#ifndef ENOEXEC
#define ENOEXEC 8
#endif

#ifndef EBADF
#define EBADF 9
#endif

#ifndef ECHILD
#define ECHILD 10
#endif

#ifndef EAGAIN
#define EAGAIN 11
#endif

#ifndef ENOMEM
#define ENOMEM 12  /* Out of memory */
#endif

#ifndef EACCES
#define EACCES 13
#endif

#ifndef EFAULT
#define EFAULT 14
#endif

#ifndef EBUSY
#define EBUSY 16
#endif

#ifndef EEXIST
#define EEXIST 17
#endif

#ifndef EXDEV
#define EXDEV 18
#endif

#ifndef ENODEV
#define ENODEV 19
#endif

#ifndef ENOTDIR
#define ENOTDIR 20
#endif

#ifndef EISDIR
#define EISDIR 21
#endif

#ifndef ENFILE
#define ENFILE 23
#endif

#ifndef EMFILE
#define EMFILE 24
#endif

#ifndef ENOTTY
#define ENOTTY 25
#endif

#ifndef EFBIG
#define EFBIG 27
#endif

#ifndef ENOSPC
#define ENOSPC 28
#endif

#ifndef ESPIPE
#define ESPIPE 29
#endif

#ifndef EROFS
#define EROFS 30
#endif

#ifndef EMLINK
#define EMLINK 31
#endif

#ifndef EPIPE
#define EPIPE 32
#endif

#ifndef EDOM
#define EDOM 33
#endif

#ifndef EDEADLK
#define EDEADLK 36
#endif

#ifndef ENAMETOOLONG
#define ENAMETOOLONG 38
#endif

#ifndef ENOLCK
#define ENOLCK 39
#endif

#ifndef ENOSYS
#define ENOSYS 40
#endif

#ifndef ENOTEMPTY
#define ENOTEMPTY 41
#endif

#ifndef EINVAL
#define EINVAL 22
#endif

#ifndef ERANGE
#define ERANGE 34
#endif

#ifndef EILSEQ
#define EILSEQ 42
#endif

#ifndef STRUNCATE
#define STRUNCATE 80
#endif

// Posix thread extensions
#ifndef ENOTSUP
#define ENOTSUP 129
#endif

// Extensions for network-related errors
#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT 102
#endif

#ifndef EADDRINUSE
#define EADDRINUSE 100
#endif

#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL 101
#endif

#ifndef EISCONN
#define EISCONN 113
#endif

#ifndef ENOBUFS
#define ENOBUFS 119
#endif

#ifndef ECONNABORTED
#define ECONNABORTED 106
#endif

#ifndef EALREADY
#define EALREADY 103
#endif

#ifndef ECONNREFUSED
#define ECONNREFUSED 107
#endif

#ifndef ECONNRESET
#define ECONNRESET 108
#endif

#ifndef EDESTADDRREQ
#define EDESTADDRREQ 109
#endif

#ifndef EHOSTUNREACH
#define EHOSTUNREACH 110
#endif

#ifndef EMSGSIZE
#define EMSGSIZE 115
#endif

#ifndef ENETDOWN
#define ENETDOWN 116
#endif

#ifndef ENETRESET
#define ENETRESET 117
#endif

#ifndef ENETUNREACH
#define ENETUNREACH 118
#endif

#ifndef ENOPROTOOPT
#define ENOPROTOOPT 123
#endif

#ifndef ENOTSOCK
#define ENOTSOCK 128
#endif

#ifndef ENOTCONN
#define ENOTCONN 126
#endif

#ifndef ECANCELED
#define ECANCELED 105
#endif

#ifndef EINPROGRESS
#define EINPROGRESS 112
#endif

#ifndef EOPNOTSUPP
#define EOPNOTSUPP 130
#endif

#ifndef EWOULDBLOCK
#define EWOULDBLOCK 140
#endif

#ifndef EOWNERDEAD
#define EOWNERDEAD 133
#endif

#ifndef EPROTO
#define EPROTO 134
#endif

#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT 135
#endif

#ifndef EBADMSG
#define EBADMSG 104
#endif

#ifndef EIDRM
#define EIDRM 111
#endif

#ifndef ENODATA
#define ENODATA 120
#endif

#ifndef ENOLINK
#define ENOLINK 121
#endif

#ifndef ENOMSG
#define ENOMSG 122
#endif

#ifndef ENOSR
#define ENOSR 124
#endif

#ifndef ENOSTR
#define ENOSTR 125
#endif

#ifndef ENOTRECOVERABLE
#define ENOTRECOVERABLE 127
#endif

#ifndef ETIME
#define ETIME 137
#endif

#ifndef ETXTBSY
#define ETXTBSY 139
#endif

#ifndef ETIMEDOUT
#define ETIMEDOUT 138
#endif

#ifndef ELOOP
#define ELOOP 114
#endif

#ifndef EPROTOTYPE
#define EPROTOTYPE 136
#endif

#ifndef EOVERFLOW
#define EOVERFLOW 132
#endif

#ifdef __cplusplus
}
#endif

#endif // LU_ERRNO_H_
