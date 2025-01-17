
#ifndef _LU_MM_PALLOC_H_INCLUDED_
#define _LU_MM_PALLOC_H_INCLUDED_


#include "lu_mm_core.h"


/*
 * NGX_MAX_ALLOC_FROM_POOL should be (lu_mm_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */

#define LU_MM_MAX_ALLOC_FROM_POOL  (lu_mm_pagesize -1)


#define LU_MM_DEFAULT_POOL_SIZE       (16 * 1024)


#define LU_MM_POOL_ALIGNMENT       16


#define LU_MM_MIN_POOL_SIZE                                                         \
        lu_align((sizeof(lu_mm_pool_t) + 2 * sizeof(lu_mm_pool_large_t)),              \
            LU_MM_POOL_ALIGNMENT))                      


typedef struct lu_mm_pool_large_s  lu_mm_pool_large_t;



struct lu_mm_pool_large_s {
    lu_mm_pool_large_t     *next;     // 指向下一块大内存块的指针
    void                 *alloc;    // 大内存块的起始地址
};
 
typedef struct lu_mm_pool_data_s {
    lu_uchar_t               *last;     // 保存当前数据块中内存分配指针的当前位置。每次Nginx程序从内存池中申请内存时，
                                        //从该指针保存的位置开始划分出请求的内存大小，并更新该指针到新的位置。
    lu_uchar_t               *end;      // 保存内存块的结束位置
    lu_mm_pool_t           *next;     // 内存池由多块内存块组成，指向下一个数据块的位置。
    lu_uintptr_t            failed;   // 当前数据块内存不足引起分配失败的次数
} lu_mm_pool_data_t;
 
struct lu_mm_pool_s {
    lu_mm_pool_data_t       d;        // 内存池当前的数据区指针的结构体
    size_t                max;      // 当前数据块最大可分配的内存大小（Bytes）
    lu_mm_pool_t           *current;  // 当前正在使用的数据块的指针
    lu_mm_pool_large_t     *large;    // pool 中指向大数据块的指针（大数据快是指 size > max 的数据块）

};



void *lu_mm_alloc(size_t size);
void *lu_mm_calloc(size_t size);

lu_mm_pool_t *lu_mm_create_pool(size_t size);
void lu_mm_destroy_pool(lu_mm_pool_t *pool);
void lu_mm_reset_pool(lu_mm_pool_t *pool);

void *lu_mm_palloc(lu_mm_pool_t *pool, size_t size);
void *lu_mm_pnalloc(lu_mm_pool_t *pool, size_t size);
void *lu_mm_pcalloc(lu_mm_pool_t *pool, size_t size);
void *lu_mm_pmemalign(lu_mm_pool_t *pool, size_t size, size_t alignment);
lu_intptr_t lu_mm_pfree(lu_mm_pool_t *pool, void *p);



#endif /* _LU_MM_PALLOC_H_INCLUDED_ */
