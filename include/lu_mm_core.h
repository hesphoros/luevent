#ifndef _LU_MM_CORE_H_INCLUDED_
#define _LU_MM_CORE_H_INCLUDED_

#define LU_MM_HAVE_POSIX_MEMALIGN  1

typedef struct lu_mm_pool_s            lu_mm_pool_t;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>

#include <lu_erron.h>
// #include <sys/mman.h>
#include "lu_util.h"

#ifdef __cplusplus
extern "C" {
#endif



#define LU_MM_ALIGNMENT   sizeof(unsigned long)    /* platform word 平台字节长*/

#define lu_mm_align(d,a)     (((d) + (a - 1)) & ~(a - 1))  /* b对齐为a的倍数*/
 
#define lu_mm_align_ptr(p,a)                                            \
    (lu_uchar_t *) (((uintptr_t) (p) + ((uintptr_t) a - 1))             \
            & ~((uintptr_t) a - 1)) /*用于将指针 p 对齐到指定的对齐字节数 a*/


#define lu_mm_memzero(buf,n)        (void) memset(buf, 0, n)


#ifdef __cplusplus
}
#endif

#include "lu_mm_alloc.h"
#include "lu_mm_palloc.h"


#endif /* _LU_MM_CORE_H_INCLUDED_ */
