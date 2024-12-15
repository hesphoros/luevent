#ifndef LU_HASH_TABLE_INTERNAL_H
#define LU_HASH_TABLE_INTERNAL_H


#include <string.h>   
#include <stddef.h>   
#include <stdlib.h>  


#define LU_HASH_TABLE_DEFAULT_SIZE 16
#define LU_HASH_TABLE_MAX_LOAD_FACTOR 0.75

typedef struct lu_hash_list_node_s {
    struct lu_hash_list_node_s *next;
    int                         key;
    void*                       value;
} lu_hash_list_node_t;

typedef lu_hash_list_node_t *lu_hash_list_ptr_t;
typedef lu_hash_list_node_t *element_t;

//typedef ListNode *List;
//typedef ListNode *Elemen


typedef struct lu_hash_table_s {
    int                               table_size;
    lu_hash_list_ptr_t*               the_lists; 

}lu_hash_table_t;


int lu_Hash(int key,int table_size);
lu_hash_table_t* luHashInit(int table_size);
void luHashInsert(lu_hash_table_t* table, int key, void* value);
/**从hash表中根据键zhi查找元素，返回元素指针，如果没有找到则返回NULL */
element_t luFind(lu_hash_table_t* table, int key);


#endif /* LU_HASH_TABLE_INTERNAL_H */