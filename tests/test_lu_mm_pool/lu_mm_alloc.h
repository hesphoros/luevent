
#ifndef _LU_MM_ALLOC_H_INCLUDED_
#define _LU_MM_ALLOC_H_INCLUDED_



#include "lu_mm_core.h"


#ifdef __cplusplus
extern "C" {
#endif



void *lu_mm_alloc(size_t size);
void *lu_mm_calloc(size_t size);

#define lu_mm_free         free


/*
 * Linux has memalign() or posix_memalign()
 * Solaris has memalign()
 * FreeBSD 7.0 has posix_memalign(), besides, early version's malloc()
 * aligns allocations bigger than page size at the page boundary
 */

#if (LU_MM_HAVE_POSIX_MEMALIGN || LU_MM_HAVE_MEMALIGN)

void *lu_mm_memalign(size_t alignment, size_t size);

#else

#define lu_mm_memalign(alignment, size)  lu_mm_alloc(size)

#endif



extern lu_uintptr_t  lu_mm_pagesize;
extern lu_uintptr_t  lu_mm_pagesize_shift;
extern lu_uintptr_t  lu_mm_cacheline_size;

#ifdef __cplusplus
}
#endif


#endif /* _LU_MM_ALLOC_H_INCLUDED_ */
