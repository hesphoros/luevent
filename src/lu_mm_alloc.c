
#include "lu_mm_core.h"

static int debug = 0;




lu_uintptr_t  lu_mm_pagesize;
lu_uintptr_t  lu_mm_pagesize_shift;
lu_uintptr_t  lu_mm_cacheline_size;



void * lu_mm_alloc(size_t size)
{
    void  *p;

    p = malloc(size);
    if (p == NULL) {
        fprintf(stderr,"malloc(%zu) failed", size);
    }

    if(debug) fprintf(stderr, "malloc: %p:%zu", p, size);

    return p;
}


void * lu_mm_calloc(size_t size)
{
    void  *p;

    p = lu_mm_alloc(size);

    if (p) {
        lu_mm_memzero(p, size);
    }

    return p;
}

// /*
#if (LU_MM_HAVE_POSIX_MEMALIGN)

void * lu_mm_memalign(size_t alignment, size_t size)
{
    void  *p;
    int    err;

    err = posix_memalign(&p, alignment, size);//按照指定的对齐要求和大小分配内存块。

    if (err) {
        fprintf(stderr,"posix_memalign(%zu, %zu) failed", alignment, size);
        p = NULL;
    }

    if(debug) fprintf(stderr,"posix_memalign: %p:%zu @%zu", p, size, alignment);

    return p;
}

#elif (LU_MM_HAVE_MEMALIGN)

void * lu_mm_memalign(size_t alignment, size_t size)
{
    void  *p;

    p = memalign(alignment, size);
    if (p == NULL) {
        fprintf(stderr,"memalign(%zu, %zu) failed", alignment, size);
    }

    if(debug) fprintf(stderr,"memalign: %p:%zu @%zu", p, size, alignment);

    return p;
}

#endif
// */
