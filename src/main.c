#include "lu_memory_manager.h"
#include <stdio.h>
#include "lu_erron.h"
#include <memory.h>
#include "lu_hash_table-internal.h"


//#define LU_EVENT__ENABLE_DEFAULT_MEMORY_LOGGING


// 定义包含姓名、ID号、性别等信息的结构体
typedef struct {
    char name[50];       // 姓名
    int id;              // ID号
    char gender;         // 性别 ('M' = 男, 'F' = 女)
} Person;

// 定义一个包含 `lu_hash_table_handle_t` 的结构体
typedef struct {
    lu_hash_table_handle_t* hash_table_handle;  // 哈希表句柄，用于存储 Person 数据
} PersonDatabase;

// 创建并初始化 Person 实例并插入哈希表
void add_person_to_db(PersonDatabase* db, Person* person) {
    // 在哈希表中插入 Person 数据，使用 id 作为 key
    lu_hash_table_insert(db->hash_table_handle, person->id, person);
}

// 查找指定 ID 的 Person 数据
Person* find_person_by_id(PersonDatabase* db, int id) {
    return (Person*)lu_hash_table_find(db->hash_table_handle, id);
}

// 销毁数据库并释放内存
void destroy_person_db(PersonDatabase* db) {
    lu_hash_table_destroy(db->hash_table_handle);
}

void test_hash(){
     // 初始化哈希表数据库
    PersonDatabase db;
    db.hash_table_handle = lu_hash_table_init(4);

  
    Person people[36] = {
        {"张三", 1001, 'M'}, {"李四", 1002, 'F'}, {"王五", 1003, 'M'},
        {"赵六", 1004, 'F'}, {"孙七", 1005, 'M'}, {"周八", 1006, 'F'},
        {"吴九", 1007, 'M'}, {"郑十", 1008, 'F'}, {"钱十一", 1009, 'M'},
        {"刘十二", 1010, 'F'}, {"陈十三", 1011, 'M'}, {"杨十四", 1012, 'F'},
        {"黄十五", 1013, 'M'}, {"吴十六", 1014, 'F'}, {"沈十七", 1015, 'M'},
        {"蒋十八", 1016, 'F'}, {"魏十九", 1017, 'M'}, {"宋二十", 1018, 'F'},
        {"张二十一", 1019, 'M'}, {"王二十二", 1020, 'F'}, {"李二十三", 1021, 'M'},
        {"周二十四", 1022, 'F'}, {"郑二十五", 1023, 'M'}, {"赵二十六", 1024, 'F'},
        {"钱二十七", 1025, 'M'}, {"孙二十八", 1026, 'F'}, {"陈二十九", 1027, 'M'},
        {"刘三十", 1028, 'F'}, {"蒋三十一", 1029, 'M'}, {"魏三十二", 1030, 'F'},
        {"杨三十三", 1031, 'M'}, {"黄三十四", 1032, 'F'}, {"沈三十五", 1033, 'M'},
        {"宋三十六", 1034, 'F'}
    };

    // 将 Person 数据添加到哈希表中
    for (int i = 0; i < 36; ++i) {
        add_person_to_db(&db, &people[i]);
    }

    // // 查找指定 ID 的 Person 数据
    // for (int i = 1001; i <= 1034; ++i) {
    //     Person* found_person = find_person_by_id(&db, i);
    //     if (found_person) {
    //         printf("Found person: Name: %s, ID: %d, Gender: %c\n", found_person->name, found_person->id, found_person->gender);
    //     } else {
    //         printf("Person with ID %d not found\n", i);
    //     }
    // }

    // 销毁哈希表数据库，释放内存
    destroy_person_db(&db);

}

void test_error_to_string(){
    lu_enable_default_memory_logging(1); 
    printf("%s\n",lu_get_error_string(LU_ERROR_BAD_ADDRESS));
    printf("%s\n",lu_get_error_string(LU_ERROR_OUT_OF_MEMORY));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_BAD_FILE_DESCRIPTOR));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_BROKEN_PIPE));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_NO_SUCH_FILE_OR_DIRECTORY));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_NO_SUCH_PROCESS));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_NO_SUCH_DEVICE));
    printf("%s\n",lu_get_error_string_hash(LU_ERROR_NO_SUCH_DEVICE_OR_ADDRESS));    
    void *p = mm_memalign(100,100);
}

void test_mm_memory(){
    
    void *p = mm_malloc(100);
    printf("%p size: %d\n",p,sizeof(*p));
   
    p = mm_realloc(p,200);
    printf("%p size: %d\n",p,sizeof(p));

    p = mm_calloc(1,200);
    printf("%p %s size: %d\n",p,*(int*)p,sizeof(p));

    p = mm_strdup("hello");
    printf("%p %s\n",p,(char*)p);
    
    mm_free(p);

}


int main(){
    test_hash();
    return 0;
}