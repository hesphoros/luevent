
#ifndef _LU_MM_POLL_CORE_H_INCLUDED_
#define _LU_MM_POLL_CORE_H_INCLUDED_

#include <lu_util.h>

#define NGX_HAVE_POSIX_MEMALIGN  1
#define LU_MM_POOL_HAVE_POSIX_MEMALIGN  1

typedef struct lu_mm_pool_s      lu_mm_pool_t;



// #define  NGX_OK          0
// #define  NGX_ERROR      -1
// #define  NGX_AGAIN      -2
// #define  NGX_BUSY       -3
// #define  NGX_DONE       -4
// #define  NGX_DECLINED   -5
// #define  NGX_ABORT      -6



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>


#include "lu_mm_alloc.h"
#include "lu_mm_palloc.h"

// typedef intptr_t        ngx_int_t;//long int  lu_intptr_t
// lu_uintptr_t uintptr_t       ngx_uint_t;//unsigned long int lu_uintptr_t
// typedef unsigned char u_char;


#define LU_ALIGNMENT   sizeof(unsigned long)    /* platform word 平台字节长*/
#define lu_align(d, a)     (((d) + (a - 1)) & ~(a - 1)) /* b对齐为a的倍数*/
#define lu_align_ptr(p, a)                                                   \
    (lu_uchar_t*) (((lu_uintptr_t) (p) + ((lu_uintptr_t) a - 1)) & ~((lu_uintptr_t) a - 1))/*用于将指针 p 对齐到指定的对齐字节数 a*/

#define lu_memzero(buf, n)       (void) memset(buf, 0, n)




#endif /* _LU_MM_POLL_CORE_H_INCLUDED_ */
