#include "lu_hash_table-internal.h"
 
#include <stdio.h>  // 为了打印错误信息
#include <stdlib.h> // 为了使用malloc, free
#include <string.h> // 为了使用memset


void lu_rb_tree_right_rotate(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
void lu_rb_tree_left_rotate(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
void lu_rb_tree_insert_fixup(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
void lu_rb_tree_left_rotate_delete(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
void lu_rb_tree_delete(lu_rb_tree_t* tree, int key);
void lu_rb_tree_right_rotate_delete(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
lu_rb_tree_node_t* lu_rb_tree_find(lu_rb_tree_t* tree, int key);
void lu_rb_tree_delete_fixup(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
void lu_rb_tree_transplant(lu_rb_tree_t* tree, lu_rb_tree_node_t* u, lu_rb_tree_node_t* v);
lu_rb_tree_node_t* lu_rb_tree_minimum(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
lu_rb_tree_node_t* lu_rb_tree_maximum(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);

// 哈希函数
int lu_hash_function(int key, int table_size) {
    return key % table_size;
}

// 初始化哈希表
lu_hash_table_t* lu_hash_table_init(int table_size) {
    int i = 0;
    lu_hash_table_t* hash_table = NULL;

    // 如果传入的大小无效，使用默认大小
    if (table_size <= 0) {
        table_size = LU_HASH_TABLE_DEFAULT_SIZE;
    }

    // 分配哈希表内存
    hash_table = (lu_hash_table_t*)LU_MM_MALLOC(sizeof(lu_hash_table_t));
    if (hash_table == NULL) {
        printf("lu_hash_table_init: malloc failed for hash table\n");
        return NULL;
    }
    hash_table->table_size = table_size;

    // 为哈希桶数组分配内存
    hash_table->buckets = (lu_hash_bucket_t*)LU_MM_MALLOC(table_size * sizeof(lu_hash_bucket_t));
    if (hash_table->buckets == NULL) {
        printf("lu_hash_table_init: malloc failed for buckets\n");
        free(hash_table);
        return NULL;
    }

    for(i = 0; i < table_size; i++){
        hash_table->buckets[i].type = LU_BUCKET_LIST;
        hash_table->buckets[i].data.list_head = NULL;
    }
    // // 初始化哈希桶中的每个链表头
    // for (i = 0; i < table_size; i++) {
    //     hash_table->buckets[i] = (lu_hash_bucket_node_t*)LU_MM_MALLOC(sizeof(lu_hash_bucket_node_t));
    //     if (hash_table->buckets[i] == NULL) {
    //         printf("lu_hash_table_init: malloc failed for bucket[%d]\n", i);
    //         for (int j = 0; j < i; j++) {
    //             free(hash_table->buckets[j]);
    //         }
    //         free(hash_table->buckets);
    //         free(hash_table);
    //         return NULL;
    //     }
    //     memset(hash_table->buckets[i], 0, sizeof(lu_hash_bucket_node_t));
    // }

    return hash_table;
}
//TODO
// 红黑树插入（简化版，这里假设已经实现）
void lu_rb_tree_insert(lu_rb_tree_t* tree, int key, void* value) {
   lu_rb_tree_node_t* new_node = (lu_rb_tree_node_t*)malloc(sizeof(lu_rb_tree_node_t));
    if (new_node == NULL) {
        printf("Memory allocation failed for new node\n");
        return;
    }
    new_node->key = key;
    new_node->value = value;
    new_node->left = tree->nil;
    new_node->right = tree->nil;
    new_node->parent = tree->nil;
    new_node->color = RED;

    lu_rb_tree_node_t* y = tree->nil;
    lu_rb_tree_node_t* x = tree->root;

    // 查找插入位置
    while (x != tree->nil) {
        y = x;
        if (new_node->key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    new_node->parent = y;
    if (y == tree->nil) {
        tree->root = new_node;
    } else if (new_node->key < y->key) {
        y->left = new_node;
    } else {
        y->right = new_node;
    }

    // 修复红黑树性质
    lu_rb_tree_insert_fixup(tree, new_node);
}

 

// 插入元素
void lu_hash_table_insert(lu_hash_table_t* table, int key, void* value) {
    int index = lu_hash_function(key, table->table_size);
    lu_hash_bucket_t* bucket = &table->buckets[index];

    if (bucket->type == LU_BUCKET_LIST) {
        // 在链表中插入
        lu_hash_bucket_node_ptr_t new_node = (lu_hash_bucket_node_ptr_t)malloc(sizeof(lu_hash_bucket_node_t));
        if (new_node == NULL) {
            printf("Memory allocation failed for new node\n");
            return;
        }
        new_node->key = key;
        new_node->value = value;
        new_node->next = bucket->data.list_head;
        bucket->data.list_head = new_node;

        // 检查桶内元素数量是否超过阈值，若超过则转换为红黑树
        int count = 0;
        lu_hash_bucket_node_ptr_t node = bucket->data.list_head;
        while (node != NULL) {
            count++;
            node = node->next;
        }

        if (count > LU_BUCKET_LIST_THRESHOLD) {
            // 转换为红黑树
            lu_rb_tree_t* new_tree = (lu_rb_tree_t*)malloc(sizeof(lu_rb_tree_t));
            if (new_tree == NULL) {
                printf("Memory allocation failed for new red-black tree\n");
                return;
            }

            new_tree->nil = (lu_rb_tree_node_t*)malloc(sizeof(lu_rb_tree_node_t));
            if (new_tree->nil == NULL) {
                printf("Memory allocation failed for nil node\n");
                return;
            }

            new_tree->root = new_tree->nil; // 红黑树初始化为空树
            // 插入所有链表元素到红黑树中
            node = bucket->data.list_head;
            while (node != NULL) {
                lu_rb_tree_insert(new_tree, node->key, node->value);
                node = node->next;
            }

            // 将桶的类型改为红黑树，并清空链表
            bucket->type = LU_BUCKET_RBTREE;
            bucket->data.rb_tree = new_tree;

            // 清空链表
            node = bucket->data.list_head;
            while (node != NULL) {
                lu_hash_bucket_node_ptr_t temp = node;
                node = node->next;
                free(temp);
            }
            bucket->data.list_head = NULL;
        }
    } else if (bucket->type == LU_BUCKET_RBTREE) {
        // 在红黑树中插入
        lu_rb_tree_insert(bucket->data.rb_tree, key, value);
    }
}



 
// 查找元素
void* lu_hash_table_find(lu_hash_table_t* table, int key) {
    int index = lu_hash_function(key, table->table_size);
    lu_hash_bucket_t* bucket = &table->buckets[index];

    if (bucket->type == LU_BUCKET_LIST) {
        // 在链表中查找
        lu_hash_bucket_node_ptr_t node = bucket->data.list_head;
        while (node != NULL) {
            if (node->key == key) {
                return node->value;
            }
            node = node->next;
        }
    } else if (bucket->type == LU_BUCKET_RBTREE) {
         
        lu_rb_tree_find(bucket->data.rb_tree, key);
    }

    return NULL;  // 未找到
}

 
// 删除元素
void lu_hash_table_delete(lu_hash_table_t* table, int key) {
    int index = lu_hash_function(key, table->table_size);
    lu_hash_bucket_t* bucket = &table->buckets[index];

    if (bucket->type == LU_BUCKET_LIST) {
        // 在链表中删除
        lu_hash_bucket_node_ptr_t prev = NULL;
        lu_hash_bucket_node_ptr_t node = bucket->data.list_head;
        while (node != NULL) {
            if (node->key == key) {
                if (prev == NULL) {
                    bucket->data.list_head = node->next;
                } else {
                    prev->next = node->next;
                }
                free(node);
                return;
            }
            prev = node;
            node = node->next;
        }
    } else if (bucket->type == LU_BUCKET_RBTREE) {
        // 在红黑树中删除（简化版，这里假设已经实现删除函数）
        // lu_rb_tree_delete(bucket->data.rb_tree, key);
    }
}

// 提取元素的值
void* lu_hash_table_retrieve(lu_hash_bucket_element_t element) {
    return element ? element->value : NULL;
}

// 销毁哈希表
void lu_hash_table_destroy(lu_hash_table_t* table) {
    int i;
    for (i = 0; i < table->table_size; i++) {
        lu_hash_bucket_t* bucket = &table->buckets[i];
        if (bucket->type == LU_BUCKET_LIST) {
            lu_hash_bucket_node_ptr_t node = bucket->data.list_head;
            while (node != NULL) {
                lu_hash_bucket_node_ptr_t temp = node;
                node = node->next;
                free(temp);
            }
        } else if (bucket->type == LU_BUCKET_RBTREE) {
            // 销毁红黑树（这里简化版假设已经有相应的销毁函数）
            // lu_rb_tree_destroy(bucket->data.rb_tree);
        }
    }
    free(table->buckets);
    free(table);
}

 

 void lu_rb_tree_right_rotate(lu_rb_tree_t* tree, lu_rb_tree_node_t* node) {
    lu_rb_tree_node_t* left = node->left;
    node->left = left->right;
    if (left->right != tree->nil) {
        left->right->parent = node;
    }
    left->parent = node->parent;
    if (node->parent == tree->nil) {
        tree->root = left;
    } else if (node == node->parent->right) {
        node->parent->right = left;
    } else {
        node->parent->left = left;
    }
    left->right = node;
    node->parent = left;
}


void lu_rb_tree_left_rotate(lu_rb_tree_t* tree, lu_rb_tree_node_t* node) {
    lu_rb_tree_node_t* right = node->right;
    node->right = right->left;
    if (right->left != tree->nil) {
        right->left->parent = node;
    }
    right->parent = node->parent;
    if (node->parent == tree->nil) {
        tree->root = right;
    } else if (node == node->parent->left) {
        node->parent->left = right;
    } else {
        node->parent->right = right;
    }
    right->left = node;
    node->parent = right;
}


void lu_rb_tree_insert_fixup(lu_rb_tree_t* tree, lu_rb_tree_node_t* node) {
    while (node->parent->color == RED) {
        if (node->parent == node->parent->parent->left) {
            lu_rb_tree_node_t* uncle = node->parent->parent->right;
            if (uncle->color == RED) {
                // Case 1: Uncle is red
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if (node == node->parent->right) {
                    // Case 2: Node is right child
                    node = node->parent;
                    lu_rb_tree_left_rotate(tree, node);
                }
                // Case 3: Node is left child
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                lu_rb_tree_right_rotate(tree, node->parent->parent);
            }
        } else {
            lu_rb_tree_node_t* uncle = node->parent->parent->left;
            if (uncle->color == RED) {
                // Case 1: Uncle is red
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    // Case 2: Node is left child
                    node = node->parent;
                    lu_rb_tree_right_rotate(tree, node);
                }
                // Case 3: Node is right child
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                lu_rb_tree_left_rotate(tree, node->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

// 查找红黑树中的节点
lu_rb_tree_node_t* lu_rb_tree_find(lu_rb_tree_t* tree, int key) {
    lu_rb_tree_node_t* current = tree->root;
    while (current != tree->nil) {
        if (key == current->key) {
            return current;
        } else if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return NULL;  // 找不到
}


// 左旋操作
void lu_rb_tree_left_rotate_delete(lu_rb_tree_t* tree, lu_rb_tree_node_t* node) {
    lu_rb_tree_node_t* right = node->right;
    node->right = right->left;
    if (right->left != tree->nil) {
        right->left->parent = node;
    }
    right->parent = node->parent;
    if (node->parent == tree->nil) {
        tree->root = right;
    } else if (node == node->parent->left) {
        node->parent->left = right;
    } else {
        node->parent->right = right;
    }
    right->left = node;
    node->parent = right;
}

// 右旋操作
void lu_rb_tree_right_rotate_delete(lu_rb_tree_t* tree, lu_rb_tree_node_t* node) {
    lu_rb_tree_node_t* left = node->left;
    node->left = left->right;
    if (left->right != tree->nil) {
        left->right->parent = node;
    }
    left->parent = node->parent;
    if (node->parent == tree->nil) {
        tree->root = left;
    } else if (node == node->parent->right) {
        node->parent->right = left;
    } else {
        node->parent->left = left;
    }
    left->right = node;
    node->parent = left;
}

// 修复删除后的红黑树性质
void lu_rb_tree_delete_fixup(lu_rb_tree_t* tree, lu_rb_tree_node_t* node) {
    while (node != tree->root && node->color == BLACK) {
        if (node == node->parent->left) {
            lu_rb_tree_node_t* sibling = node->parent->right;
            if (sibling->color == RED) {
                sibling->color = BLACK;
                node->parent->color = RED;
                lu_rb_tree_left_rotate_delete(tree, node->parent);
                sibling = node->parent->right;
            }

            if (sibling->left->color == BLACK && sibling->right->color == BLACK) {
                sibling->color = RED;
                node = node->parent;
            } else {
                if (sibling->right->color == BLACK) {
                    sibling->left->color = BLACK;
                    sibling->color = RED;
                    lu_rb_tree_right_rotate_delete(tree, sibling);
                    sibling = node->parent->right;
                }
                sibling->color = node->parent->color;
                node->parent->color = BLACK;
                sibling->right->color = BLACK;
                lu_rb_tree_left_rotate_delete(tree, node->parent);
                node = tree->root;
            }
        } else {
            lu_rb_tree_node_t* sibling = node->parent->left;
            if (sibling->color == RED) {
                sibling->color = BLACK;
                node->parent->color = RED;
                lu_rb_tree_right_rotate_delete(tree, node->parent);
                sibling = node->parent->left;
            }

            if (sibling->right->color == BLACK && sibling->left->color == BLACK) {
                sibling->color = RED;
                node = node->parent;
            } else {
                if (sibling->left->color == BLACK) {
                    sibling->right->color = BLACK;
                    sibling->color = RED;
                    lu_rb_tree_left_rotate_delete(tree, sibling);
                    sibling = node->parent->left;
                }
                sibling->color = node->parent->color;
                node->parent->color = BLACK;
                sibling->left->color = BLACK;
                lu_rb_tree_right_rotate_delete(tree, node->parent);
                node = tree->root;
            }
        }
    }
    node->color = BLACK;
}

void lu_rb_tree_transplant(lu_rb_tree_t* tree, lu_rb_tree_node_t* u, lu_rb_tree_node_t* v) {
    // 如果 u 是根节点，则更新根节点
    if (u->parent == tree->nil) {
        tree->root = v;
    } else if (u == u->parent->left) {  // 如果 u 是父节点的左子节点
        u->parent->left = v;
    } else {  // 如果 u 是父节点的右子节点
        u->parent->right = v;
    }
    
    // 更新 v 的父节点为 u 的父节点
    if (v != tree->nil) {
        v->parent = u->parent;
    }
}



// 删除红黑树中的节点
void lu_rb_tree_delete(lu_rb_tree_t* tree, int key) {
    lu_rb_tree_node_t* node = lu_rb_tree_find(tree, key);
    if (node == NULL) {
        return;  // 节点不存在
    }

    lu_rb_tree_node_t* y = node;
    lu_rb_tree_node_t* x;
    lu_rb_tree_color_t original_color = y->color;

    if (node->left == tree->nil) {
        x = node->right;
        lu_rb_tree_transplant(tree, node, node->right);
    } else if (node->right == tree->nil) {
        x = node->left;
        lu_rb_tree_transplant(tree, node, node->left);
    } else {
        y = lu_rb_tree_minimum(tree, node->right);
        original_color = y->color;
        x = y->right;
        if (y->parent == node) {
            x->parent = y;
        } else {
            lu_rb_tree_transplant(tree, y, y->right);
            y->right = node->right;
            y->right->parent = y;
        }
        lu_rb_tree_transplant(tree, node, y);
        y->left = node->left;
        y->left->parent = y;
        y->color = node->color;
    }

    if (original_color == BLACK) {
        lu_rb_tree_delete_fixup(tree, x);
    }
    free(node);
}


// 查找以 node 为根的子树中的最小节点
lu_rb_tree_node_t* lu_rb_tree_minimum(lu_rb_tree_t* tree, lu_rb_tree_node_t* node) {
    while (node->left != tree->nil) {
        node = node->left;  // 一直向左走，直到没有左子节点
    }
    return node;  // 返回最左边的节点
}


// 查找以 node 为根的子树中的最大节点
lu_rb_tree_node_t* lu_rb_tree_maximum(lu_rb_tree_t* tree, lu_rb_tree_node_t* node) {
    while (node->right != tree->nil) {
        node = node->right;  // 一直向右走，直到没有右子节点
    }
    return node;  // 返回最右边的节点
}

