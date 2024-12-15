void lu_rb_tree_insert(lu_rb_tree_t* tree, int key, void* value) {
    // 创建新节点
    lu_rb_tree_node_t* new_node = (lu_rb_tree_node_t*)malloc(sizeof(lu_rb_tree_node_t));
    if (new_node == NULL) {
        printf("Memory allocation failed for new node\n");
        return;
    }

    // 初始化节点字段
    new_node->key = key;
    new_node->value = value;
    new_node->color = RED;  // 新插入的节点是红色
    new_node->left = tree->nil;  // 左子节点指向树的nil节点
    new_node->right = tree->nil; // 右子节点指向树的nil节点
    new_node->parent = tree->nil; // 父节点初始化为nil

    // 如果树为空，初始化root为nil
    if (tree->root == tree->nil) {
        tree->root = new_node;
    } else {
        lu_rb_tree_node_t* parent = tree->root;
        lu_rb_tree_node_t* current = tree->root;
        while (current != tree->nil) {
            parent = current;
            if (key < current->key) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        // 插入新节点
        new_node->parent = parent;
        if (key < parent->key) {
            parent->left = new_node;
        } else {
            parent->right = new_node;
        }

        // 修复红黑树的性质
        lu_rb_tree_insert_fixup(tree, new_node);
    }
}
