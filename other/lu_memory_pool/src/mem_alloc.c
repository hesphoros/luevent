
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include "mem_core.h"

static int debug = 0;

typedef uintptr_t lu_uintptr_t;//ngx_uint_t
typedef intptr_t  lu_intptr_t ;//ngx_int_t





ngx_uint_t  ngx_pagesize;
ngx_uint_t  ngx_pagesize_shift;
ngx_uint_t  ngx_cacheline_size;



void * ngx_alloc(size_t size)
{
    void  *p;

    p = malloc(size);
    if (p == NULL) {
        fprintf(stderr,"malloc(%zu) failed", size);
    }

    if(debug) fprintf(stderr, "malloc: %p:%zu", p, size);

    return p;
}


void * ngx_calloc(size_t size)
{
    void  *p;

    p = ngx_alloc(size);

    if (p) {
        ngx_memzero(p, size);
    }

    return p;
}

// /*
#if (NGX_HAVE_POSIX_MEMALIGN)

void * ngx_memalign(size_t alignment, size_t size)
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

#elif (NGX_HAVE_MEMALIGN)

void * ngx_memalign(size_t alignment, size_t size)
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
