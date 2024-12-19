#ifndef LU_HASH_TABLE_INTERNAL_H
#define LU_HASH_TABLE_INTERNAL_H

#include <cerrno>
#include <string.h>   
#include <stddef.h>   
#include <stdlib.h>  

// 默认哈希表大小与最大负载因子
#define LU_HASH_TABLE_DEFAULT_SIZE 16
#define LU_HASH_TABLE_MAX_LOAD_FACTOR 0.75

#define LU_BUCKET_LIST_THRESHOLD 10  // 链表转红黑树的阈值

// 内存分配宏，若分配失败则打印错误并退出
#define LU_MM_MALLOC(size) \
    ({ void* ptr = malloc(size); if (ptr == NULL) { printf("Memory allocation failed!\n"); exit(1); } ptr; })
#define LU_MM_CALLOC(nmemb, size) \
    ({ void* ptr = calloc(nmemb, size); if (ptr == NULL) { printf("Memory callocation failed!\n"); exit(1); } ptr; })

// 内存释放宏
#define LU_MM_FREE(ptr) \
    do { if (ptr) { free(ptr); ptr = NULL; } } while (0)

 
 
typedef enum {
    LU_BUCKET_LIST,   // 使用链表
    LU_BUCKET_RBTREE , // 使用红黑树
} lu_hash_bucket_type_t;



// 哈希表节点结构体
typedef struct lu_hash_bucket_node_s {
    struct lu_hash_bucket_node_s *next;  // 指向下一个节点
    int                            key;    // 键值
    void*                          value;  // 存储的值
} lu_hash_bucket_node_t;

//  链表节点指针类型（桶内链表节点）
typedef lu_hash_bucket_node_t *lu_hash_bucket_node_ptr_t;
// 哈希表元素指针类型（哈希桶中的元素）
typedef lu_hash_bucket_node_t *lu_hash_bucket_element_t;
//typedef struct lu_rb_tree_s lu_rb_tree_t;

typedef struct lu_rb_tree_node_s lu_rb_tree_node_t;

// 红黑树结构体
typedef struct lu_rb_tree_s {
    lu_rb_tree_node_t* root;  // 红黑树的根节点
    lu_rb_tree_node_t* nil;   // 哨兵节点，空树时使用
} lu_rb_tree_t;

typedef enum lu_node_color_u{ RED, BLACK } lu_node_color_t,lu_rb_tree_color_t;

// 红黑树节点结构体
typedef struct lu_rb_tree_node_s {
    int key;
    void* value;
    struct lu_rb_tree_node_s* left;
    struct lu_rb_tree_node_s* right;
    struct lu_rb_tree_node_s* parent;
    lu_node_color_t color;  // 红黑树的颜色
} lu_rb_tree_node_t;


typedef struct lu_hash_bucket_s {
    lu_hash_bucket_type_t type;          // 桶的类型（链表或红黑树）
    union {
        lu_hash_bucket_node_ptr_t list_head;  // 链表头
        lu_rb_tree_t* rb_tree;               // 红黑树指针
    } data;
} lu_hash_bucket_t;


// 哈希表结构体
typedef struct lu_hash_table_s {
    int                                table_size;  //  // 哈希表大小
    lu_hash_bucket_t*                   buckets;     // 哈希桶数组
} lu_hash_table_t;


// 哈希表句柄类型（对外提供的句柄）
typedef struct lu_hash_table_handle_s {
    lu_hash_table_t* table;  // 指向实际哈希表
} lu_hash_table_handle_t;






// // 函数声明
// int lu_hash_function(int key, int table_size);
// lu_hash_table_t* lu_hash_table_init(int table_size);
// void lu_hash_table_insert(lu_hash_table_t* table, int key, void* value);
// /** 
//  * 从哈希表中根据键值查找元素，返回元素指针，如果没有找到则返回NULL
//  */
// lu_hash_bucket_element_t lu_hash_table_find(lu_hash_table_t* table, int key);
// void lu_hash_table_delete(lu_hash_table_t* table, int key);  // 删除元素
// void* lu_hash_table_retrieve(lu_hash_bucket_element_t element);  // 提取哈希表元素中的值
// void lu_hash_table_destroy(lu_hash_table_t* table);  // 销毁哈希表，释放内存


// 函数声明
int lu_hash_function(int key, int table_size);
lu_hash_table_t* lu_hash_table_init(int table_size);
void lu_hash_table_insert(lu_hash_table_t* table, int key, void* value);
void* lu_hash_table_find(lu_hash_table_t* table, int key);
void lu_hash_table_delete(lu_hash_table_t* table, int key);
void lu_hash_table_destroy(lu_hash_table_t* table);
 

//rb_tree
//lu_hash_table_t* lu_hash_table_init(int table_size);


#endif /* LU_HASH_TABLE_INTERNAL_H */
