#include "lu_mm_core.h"
 

static inline void *lu_mm_palloc_small(lu_mm_pool_t *pool, size_t size,
    lu_uintptr_t align);
static void *lu_mm_palloc_block(lu_mm_pool_t *pool, size_t size);
static void *lu_mm_palloc_large(lu_mm_pool_t *pool, size_t size);

 


lu_mm_pool_t * lu_mm_create_pool(size_t size)
{
    lu_mm_pool_t  *p;

    p = lu_mm_memalign(LU_MM_POOL_ALIGNMENT, size);
    if (p == NULL) {
        return NULL;
    }
    //它将 p 指针向后偏移 sizeof(lu_mm_pool_t) 字节，这样 p->d.last 就指向了内存块中除去 ngx_pool_t 结构体大小后的位置，即可用于分配的起始位置。
    //将 p 指向的内存块的 last 成员设置为指向当前内存块后面的一段内存的起始地址。
    //这段内存的大小为 sizeof(lu_mm_pool_t)，即 ngx_pool_t 结构体的大小。
    //这样做是为了在这块内存中分配其他数据时，可以从这段内存的末尾开始分配，确保内存块的内存布局是连续的。
    p->d.last = (lu_uchar_t *) p + sizeof(lu_mm_pool_t);//指向这块内存已使用区域的最后部分。
    
    //指向未使用区域的尾部
    p->d.end = (lu_uchar_t *) p + size;
    //下一块内存块的地址
    p->d.next = NULL;

    //内存分配的失败次数
    p->d.failed = 0;

    //这行代码确定了内存池中可用于分配的最大内存大小。为了确保内存分配不会超出预先设定的阈值， 
    size = size - sizeof(lu_mm_pool_t);
    p->max = (size < LU_MM_MAX_ALLOC_FROM_POOL) ? size : LU_MM_MAX_ALLOC_FROM_POOL;

    // 当前正在使用的数据块的指针
    p->current = p;
    //p->chain = NULL;
    p->large = NULL;
    //p->cleanup = NULL;
    //p->log = log;

    return p;
}


void lu_mm_destroy_pool(lu_mm_pool_t *pool)
{
    lu_mm_pool_t          *p, *n;
    lu_mm_pool_large_t    *l;


#if (NGX_DEBUG)

    /*
     * we could allocate the pool->log from this pool
     * so we cannot use this log while free()ing the pool
     */

    for (l = pool->large; l; l = l->next) {
        fprintf(stderr,"free: %p", l->alloc);
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        fprintf(stderr,"free: %p, unused: %zu", p, p->d.end - p->d.last);

        if (n == NULL) {
            break;
        }
    }

#endif

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            lu_mm_free(l->alloc);
        }
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        lu_mm_free(p);

        if (n == NULL) {
            break;
        }
    }
}


void lu_mm_reset_pool(lu_mm_pool_t *pool)
{
    lu_mm_pool_t      *p;
    
    lu_mm_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            lu_mm_free(l->alloc);
        }
    }

    for (p = pool; p; p = p->d.next) {
        p->d.last = (lu_uchar_t *) p + sizeof(lu_mm_pool_t);
        p->d.failed = 0;
    }

    pool->current = pool;
    //pool->chain = NULL;
    pool->large = NULL;
}


void *lu_mm_pool_alloc(lu_mm_pool_t *pool, size_t size)
{
//#if !(NGX_DEBUG_PALLOC)
    if (size <= pool->max) {
        return lu_mm_palloc_small(pool, size, 1);
    }
//#endif

    return lu_mm_palloc_large(pool, size);
}


void *lu_mm_pool_nalloc(lu_mm_pool_t *pool, size_t size)
{
#if !(NGX_DEBUG_PALLOC)
    if (size <= pool->max) {
        return lu_mm_palloc_small(pool, size, 0);
    }
#endif

    return lu_mm_palloc_large(pool, size);
}


static inline void *lu_mm_palloc_small(lu_mm_pool_t *pool, size_t size, lu_uintptr_t align)
{
    lu_uchar_t      *m;
    lu_mm_pool_t  *p;

    p = pool->current;

    do {
        m = p->d.last;

        if (align) {
            m = lu_mm_align_ptr(m, LU_MM_POOL_ALIGNMENT);
        }

        if ((size_t) (p->d.end - m) >= size) {
            p->d.last = m + size;

            return m;
        }

        p = p->d.next;

    } while (p);

    return lu_mm_palloc_block(pool, size);
}


static void *lu_mm_palloc_block(lu_mm_pool_t *pool, size_t size)
{
    lu_uchar_t      *m;
    size_t       psize;
    lu_mm_pool_t  *p, *new;

    psize = (size_t) (pool->d.end - (lu_uchar_t *) pool);

    m = lu_mm_memalign(LU_MM_POOL_ALIGNMENT, psize);
    if (m == NULL) {
        return NULL;
    }

    new = (lu_mm_pool_t *) m;

    new->d.end = m + psize;
    new->d.next = NULL;
    new->d.failed = 0;

    m += sizeof(lu_mm_pool_data_t);
    m = lu_mm_align_ptr(m, LU_MM_POOL_ALIGNMENT);
    new->d.last = m + size;

    for (p = pool->current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            pool->current = p->d.next;
        }
    }

    p->d.next = new;

    return m;
}


static void *lu_mm_palloc_large(lu_mm_pool_t *pool, size_t size)
{
    void              *p;
    lu_uintptr_t         n;
    lu_mm_pool_large_t  *large;

    p = lu_mm_alloc(size);
    if (p == NULL) {
        return NULL;
    }

    n = 0;

    for (large = pool->large; large; large = large->next) {
        if (large->alloc == NULL) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {
            break;
        }
    }

    large = lu_mm_palloc_small(pool, sizeof(lu_mm_pool_large_t), 1);
    if (large == NULL) {
        lu_mm_free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


void *ngx_pmemalign(lu_mm_pool_t *pool, size_t size, size_t alignment)
{
    void              *p;
    lu_mm_pool_large_t  *large;

    p = lu_mm_memalign(alignment, size);
    if (p == NULL) {
        return NULL;
    }

    large = lu_mm_palloc_small(pool, sizeof(lu_mm_pool_large_t), 1);
    if (large == NULL) {
        lu_mm_free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


lu_intptr_t ngx_pfree(lu_mm_pool_t *pool, void *p)
{
    lu_mm_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            fprintf(stderr,"free: %p", l->alloc);
            lu_mm_free(l->alloc);
            l->alloc = NULL;

            return LU_ERROR_LUEVENT_MEMORY_POOL_OK;
        }
    }

    return LU_ERROR_LUEVENT_MEMORY_POOL_DECLINED;
}


void * ngx_pcalloc(lu_mm_pool_t *pool, size_t size)
{
    void *p;

    p = lu_mm_pool_alloc(pool, size);
    if (p) {
        lu_mm_memzero(p, size);
    }

    return p;
}


