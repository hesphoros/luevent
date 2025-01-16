
#ifndef _NGX_ALLOC_H_INCLUDED_
#define _NGX_ALLOC_H_INCLUDED_



#include "lu_mm_core.h"


void *lu_mm_pool_alloc(size_t size);
void *lu_mm_pool_calloc(size_t size);

#define ngx_free         free




#if (NGX_HAVE_POSIX_MEMALIGN || NGX_HAVE_MEMALIGN)

void *lu_mm_pool_memalign(size_t alignment, size_t size);

#else

#define lu_memalign(alignment, size)  lu_mm_pool_alloc(size)

#endif



extern lu_uintptr_t  lu_mm_pool_pagesize;
extern lu_uintptr_t  lu_mm_pool__pagesize_shift;
extern lu_uintptr_t  lu_mm_pool__cacheline_size;


#endif /* _NGX_ALLOC_H_INCLUDED_ */
