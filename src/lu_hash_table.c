#include "lu_hash_table-internal.h"
#include <cstddef>
#include <stdio.h>

int luHash(int key,int table_size)
{    
    return key % table_size;
}


lu_hash_table_t* luHashInit(int table_size) {
    if (table_size <= 0) {
        table_size = LU_HASH_TABLE_DEFAULT_SIZE;
    }

    lu_hash_table_t* hash_table = (lu_hash_table_t*)malloc(sizeof(lu_hash_table_t));
    if (hash_table == NULL) {
        printf("lu_hash_table_init: malloc failed\n");
        return NULL;
    }
    hash_table->table_size = table_size;

    // 分配桶数组（哈希表的“桶”）
    hash_table->the_lists = (lu_hash_list_ptr_t*)malloc(table_size * sizeof(lu_hash_list_ptr_t));
    if (hash_table->the_lists == NULL) {
        printf("lu_hash_table_init: malloc the_lists failed\n");
        free(hash_table);  // 释放已分配的hash_table
        return NULL;
    }

    // 初始化每个桶的链表
    for (int i = 0; i < table_size; i++) {
        hash_table->the_lists[i] = (lu_hash_list_ptr_t)malloc(sizeof(lu_hash_list_node_t));
        if (hash_table->the_lists[i] == NULL) {
            printf("lu_hash_table_init: malloc the_lists[i] failed\n");
            // 释放已分配的内存
            for (int j = 0; j < i; j++) {
                free(hash_table->the_lists[j]);
            }
            free(hash_table->the_lists);
            free(hash_table);
            return NULL;
        }
        memset(hash_table->the_lists[i], 0, sizeof(lu_hash_list_node_t));
        hash_table->the_lists[i]->next = NULL;
    }

    return hash_table;
}


void luHashInsert(lu_hash_table_t* table, int key, void* value){
    lu_hash_element_t e = NULL,tmp = NULL;
    lu_hash_list_ptr_t list = NULL;
    e = luFind(table, key);
    if(NULL == e){
        tmp = (lu_hash_element_t)malloc(sizeof(lu_hash_list_node_t));
        if(NULL == tmp){
            printf("lu_hash_table_insert: malloc failed\n");
            return;
        }

        list = table->the_lists[luHash(key, table->table_size)];
        tmp->key = key;
        tmp->value = value;
        tmp->next = list->next;
        list->next = tmp;
    }else{
        printf("lu_hash_table_insert: key already exists\n");
        return;
    }
}

lu_hash_element_t luFind(lu_hash_table_t* table, int key){
    int i = 0;
    lu_hash_list_ptr_t list = NULL;
    lu_hash_element_t e      = NULL;
    i = luHash(key, table->table_size);
    list = table->the_lists[i];
    //遍历链表 
    //时间复杂度 O(n)
    while(e!= NULL && e->key != key)
        e = e->next;
    return e;

}


void luDelele(lu_hash_table_t* table, int key){
    lu_hash_element_t e = NULL,last = NULL;
    lu_hash_list_ptr_t list = NULL;
    int i = luHash(key,table->table_size);
    list = table->the_lists[i];
    last = list;
    e = list->next;
    while(e!= NULL && e->key != key){
        last = e;
        e = e->next;
    }
    if(e != NULL){
        last->next = e->next;
        free(e);
    }else{
        printf("lu_hash_table_delete: key not found\n");
    }
}

void* luRetrieve(lu_hash_element_t e){
    return (e != NULL) ? e->value : NULL;
}


void luDestroy(lu_hash_table_t* table){
    int i = 0;
    lu_hash_list_ptr_t list = NULL;
    lu_hash_element_t cur = NULL,next = NULL;
    for(i = 0; i < table->table_size; i++)
    {
        list = table->the_lists[i];
        cur = list->next;
        while(cur != NULL)
        {
            next = cur->next;
            free(cur);
            cur = next;
        }
        free(list);
    }
    free(table->the_lists);
    free(table);

}