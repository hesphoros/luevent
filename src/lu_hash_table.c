#include "lu_hash_table-internal.h"
#include <cstddef>
#include <stdio.h>

int luHash(int key,int table_size)
{    
    return key % table_size;
}

lu_hash_table_t* luHashInit(int table_size){
    int i = 0;
    lu_hash_table_t* hash_table = NULL;
    if(hash_table <= 0)
        table_size = LU_HASH_TABLE_DEFAULT_SIZE;
    hash_table = (lu_hash_table_t*)malloc(sizeof(lu_hash_table_t));
    if(hash_table == NULL)
    {
        printf("lu_hash_table_init: malloc failed\n");
        return NULL;
    }
    hash_table->table_size = table_size;

    //为hash桶分配空间 其结构为一个数组指针
    hash_table->the_lists = (lu_hash_list_ptr_t*)malloc(table_size * sizeof(lu_hash_list_ptr_t));
    if(NULL == hash_table->the_lists){
        printf("lu_hash_table_init: malloc the_lists failed\n");
        free(hash_table);
        return NULL;
    }

    //为 Hash 桶对应的指针数组初始化链表节点
    for(i = 0; i < table_size; i++){
        hash_table->the_lists[i] = (lu_hash_list_node_t *) malloc(sizeof(lu_hash_list_node_t));
        if(NULL == hash_table->the_lists[i]){
            printf("lu_hash_table_init: malloc the_lists[i] failed\n");
            free(hash_table->the_lists);
            free(hash_table);
            return NULL;
        }else{
            memset(hash_table->the_lists[i], 0, sizeof(lu_hash_list_node_t));
        }

    }
        
}

void luHashInsert(lu_hash_table_t* table, int key, void* value){
    element_t e = NULL,tmp = NULL;
    lu_hash_list_ptr_t list = NULL;
    e = luFind(table, key);
    if(NULL == e){
        tmp = (element_t)malloc(sizeof(lu_hash_list_node_t));
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

element_t luFind(lu_hash_table_t* table, int key){
    int i = 0;
    lu_hash_list_ptr_t list = NULL;
    element_t e      = NULL;
    i = luHash(key, table->table_size);
    list = table->the_lists[i];
    //遍历链表 
    //时间复杂度 O(n)
    while(e!= NULL && e->key != key)
        e = e->next;
    return e;

}


void luDelele(lu_hash_table_t* table, int key){
    element_t e = NULL,last = NULL;
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

void* luRetrieve(element_t e){
    return e? e->value : NULL;
}