#include "lu_hash_table-internal.h"


/**
 * @file lu_hash.c
 * @brief Function prototypes for hash table operations including linked list and red-black tree management.
 *
 * This file contains function prototypes for operations on hash tables with linked list and red-black tree
 * buckets. It includes functions for insertion, deletion, search, tree balancing (rotation, fixup), and cleanup.
 * These operations ensure efficient handling of hash table buckets, whether using a linked list or a red-black tree.
 *
 * @author [hesphoros]
 * @contact [hesphoros@gmail.com]
 * @date 2024-12-25
 * @version 1.0
 */





static int			 lu_convert_bucket_to_rbtree(lu_hash_bucket_t* bucket);
static lu_rb_tree_t* lu_rb_tree_init(void);
static void			 lu_rb_tree_insert(lu_rb_tree_t* tree, int key, void* value);
static void			 lu_hash_rb_tree_delete(lu_hash_bucket_t* bucket, int key);

static void lu_hash_list_delete(lu_hash_bucket_t* bucket, int key);
static lu_hash_bucket_node_t* lu_hash_list_find(lu_hash_bucket_t* bucket, int key);
static lu_rb_tree_node_t* lu_hash_rb_tree_find(lu_rb_tree_t* tree, int key);

static void lu_rb_tree_insert_fixup(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
static void lu_rb_tree_right_rotate(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
static void lu_rb_tree_left_rotate(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);

static void lu_rb_tree_left_rotate_delete(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
static void lu_rb_tree_right_rotate_delete(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
static void lu_rb_tree_transplant(lu_rb_tree_t* tree, lu_rb_tree_node_t* u, lu_rb_tree_node_t* v);
static void lu_rb_tree_delete_fixup(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
static lu_rb_tree_node_t* lu_rb_tree_minimum(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
static lu_rb_tree_node_t* lu_rb_tree_maximum(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);

static void lu_rb_tree_destroy_node(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);
static void lu_hash_list_destory(lu_hash_bucket_t* bucket);
static void lu_hash_rb_tree_destory(lu_hash_bucket_t* bucket);

static lu_rb_tree_node_t* lu_rb_tree_successor(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);


static void lu_hash_table_resize(lu_hash_table_t* table);

static void lu_rb_tree_rehash(lu_rb_tree_t* tree, lu_rb_tree_node_t* node, lu_hash_bucket_t* new_buckets, int new_table_size, lu_rb_tree_node_t* nil);

/**
 * @brief Computes a hash value for a given key using the multiplication method.
 *
 * This function implements a hash function based on the multiplication method,
 * using the fractional part of the product between the key and the constant
 * A (the reciprocal of the golden ratio). If the table size is a power of two,
 * the modulo operation is optimized using bitwise operations. Otherwise, a
 * standard modulo operation is applied.
 *
 * @param key The integer key to be hashed.
 * @param table_size The size of the hash table (number of buckets).
 * @return The computed hash value, ranging from 0 to table_size - 1.
 */
int lu_hash_function(int key, size_t table_size)
{
	static const double golden_rate_reciprocal = 0.6180339887; // Reciprocal of the golden ratio

	double temp = key * golden_rate_reciprocal;
	double fractional_part = temp - (int)temp; // Extract fractional part
	int hash = (int)(table_size * fractional_part);

	// Optimize modulo operation if table_size is a power of two
	if ((table_size & (table_size - 1)) == 0) {
		return hash & (table_size - 1); // Use bitwise AND for power-of-two table sizes
	}

	// Fallback to standard modulo operation
	return hash % table_size;
}

/**
 * @brief Finds the successor of a given node in a red-black tree.
 *
 * The successor of a node is the node with the smallest key greater than the given node's key.
 * If the node has a right child, the successor is the minimum node in the right subtree.
 * Otherwise, the successor is one of the node's ancestors.
 *
 * @param tree The red-black tree.
 * @param node The node whose successor is to be found.
 * @return The successor node, or the tree's nil node if no successor exists.
 */
static lu_rb_tree_node_t* lu_rb_tree_successor(lu_rb_tree_t* tree, lu_rb_tree_node_t* node)
{
	if (node->right != tree->nil) {
		return lu_rb_tree_minimum(tree, node->right);
	}

	lu_rb_tree_node_t* successor = node->parent;
	while (successor != tree->nil && node == successor->right) {
		node = successor;
		successor = successor->parent;
	}
	return successor;
}

/**
 * Initializes a hash table with the specified number of buckets.
 * If the specified `table_size` is invalid (less than or equal to 0),
 * a default size is used instead (`LU_HASH_TABLE_DEFAULT_SIZE`).
 *
 * Memory is allocated for the hash table structure and the buckets.
 * Each bucket is initialized as a linked list by default.
 *
 * @param table_size The number of buckets in the hash table. If <= 0, default size is used.
 * @return A pointer to the newly initialized hash table, or exits the program if memory allocation fails.
 *
 * Usage example:
 *     lu_hash_table_t* hash_table = lu_hash_table_init(16); // Creates a hash table with 16 buckets.
 */
lu_hash_table_t* lu_hash_table_init(size_t table_size)
{
	if (table_size <= 0) {
		table_size = LU_HASH_TABLE_DEFAULT_SIZE;
	}
	lu_hash_table_t* table = (lu_hash_table_t*)LU_MM_MALLOC(sizeof(lu_hash_table_t));
	table->element_count = 0;
	table->buckets = (lu_hash_bucket_t*)LU_MM_CALLOC(table_size, sizeof(lu_hash_bucket_t));
	table->table_size = table_size;

	for (size_t i = 0; i < table_size; i++) {
		table->buckets[i].type = LU_HASH_BUCKET_LIST;
		table->buckets[i].data.list_head = NULL;
		table->buckets[i].esize_bucket = 0;
	}

	return table;
}

/**
 * Inserts a key-value pair into the hash table.
 *
 * This function calculates the bucket index using the provided key and inserts
 * the key-value pair into the corresponding bucket. If the bucket is implemented
 * as a linked list, it checks for existing keys and updates their values if found;
 * otherwise, it creates a new node and inserts it at the head of the list.
 *
 * @param table A pointer to the hash table.
 * @param key   The key to be inserted or updated in the hash table.
 * @param value A pointer to the value associated with the key.
 *
 * Usage:
 *     lu_hash_table_insert(hash_table, 42, value_ptr);
 */
void lu_hash_table_insert(lu_hash_table_t* table, int key, void* value)
{
	// Check if we need to resize the hash table
	// if ((double)table->element_count / table->table_size > LU_HASH_TABLE_MAX_LOAD_FACTOR) {
	// 	lu_hash_table_resize(table);
	// }
	int index = lu_hash_function(key, table->table_size);

	lu_hash_bucket_t* bucket = &table->buckets[index];
	if (LU_HASH_BUCKET_LIST == bucket->type) {
		lu_hash_bucket_node_ptr_t new_node = (lu_hash_bucket_node_ptr_t)LU_MM_MALLOC(sizeof(lu_hash_bucket_node_t));
		if (!new_node) {
#ifdef LU_HASH_DEBUG
			printf("Memory allocation  failed for new code\n");
#endif // LU_HASH_DEBUG
			return;
		}

		// Check if the key already exists and update the value
		lu_hash_bucket_node_t* current = bucket->data.list_head;
		while (current) {
			if (current->key == key) {
				current->value = value; // Update value if key exists
				return;
			}
			current = current->next;
		}

		// Assign the value to the new node
		new_node->value = value;

		// Assign the key to the new nod
		new_node->key = key;

		// Link the new node to the existing linked list
		new_node->next = bucket->data.list_head;

		// Update the head of the linked list to the new node
		bucket->data.list_head = new_node;

		// Increment the global element count and local bucket size
		//table->element_count++;
		//bucket->esize_bucket++;
		//increase teh element_count
		table->element_count++;
		bucket->esize_bucket++;

		// Check if the bucket's linked list length exceeds the threshold
		if (bucket->esize_bucket > LU_HASH_BUCKET_LIST_THRESHOLD) {
#ifdef LU_HASH_DEBUG
			printf("Bucket[%d] size exceeded threshold. Converting to red-black tree...\n", index);
#endif // LU_HASH_DEBUG
			//Convert the internal structure of bucket from list to rb_tree
			if (lu_convert_bucket_to_rbtree(bucket) != 1) {
#ifdef LU_HASH_DEBUG
				printf("Error: Bucket[%d] failed to convert bucket to red-black tree.\n", index);
#endif // LU_HASH_DEBUG
			}
		}
	}
	else if (LU_HASH_BUCKET_RBTREE == bucket->type) {
		/**Insert into the red-black tree*/

		//Check the rb_tree and nil
		if (NULL == bucket->data.rb_tree || NULL == bucket->data.rb_tree->nil) {
#ifdef LU_HASH_DEBUG
			printf("Inserting key %d into red-black tree \n", key);
			printf("Error: RB-tree or tree->nil is not initialized\n");
#endif // LU_HASH_DEBUG
			lu_hash_erron_global_ = LU_ERROR_TREE_OR_NIL_NOT_INIT;
			return;
		}

		lu_rb_tree_insert(bucket->data.rb_tree, key, value);
		bucket->esize_bucket++;
		table->element_count++;
	}
}

/**
 * @brief Searches for a key in a hash table and returns the corresponding value or node.
 *
 * This function finds the appropriate bucket in the hash table by using the hash function
 * on the given key. It then determines the type of the bucket (linked list or red-black tree)
 * and delegates the search to the corresponding bucket-specific find function. If the key
 * is found, the associated value or node is returned; otherwise, the function returns NULL.
 *
 * @param table A pointer to the hash table where the key will be searched.
 * @param key The key to search for in the hash table.
 * @return A pointer to the value or node associated with the key if found, or NULL if the key does not exist.
 */
void* lu_hash_table_find(lu_hash_table_t* table, int key)
{

	if (table == NULL) {
		#ifdef LU_HASH_DEBUG
		fprintf(stderr, "Error: Hash table is not initialized.\n");
		#endif // LU_HASH_DEBUG
		return NULL;
	}

	// Calculate the index of the bucket in the hash table using the hash function
	int index = lu_hash_function(key, table->table_size);

	// Retrieve the hash bucket at the calculated index
	lu_hash_bucket_t* bucket = &table->buckets[index];

	// Check the bucket type and call the corresponding find function
	if (bucket->type == LU_HASH_BUCKET_LIST) {
		// Use linked list search if the bucket stores data as a list
		lu_hash_bucket_node_ptr_t node = lu_hash_list_find(bucket, key);
		if (NULL != node) {
			return	node->value;
		}
	}
	else if (bucket->type == LU_HASH_BUCKET_RBTREE) {
		// Use red-black tree search if the bucket stores data as a tree
		lu_rb_tree_node_t* rb_node = lu_hash_rb_tree_find(bucket->data.rb_tree, key);
		if (NULL != rb_node) {
			return rb_node->value;
		}
	}
#ifdef LU_HASH_DEBUG
	printf("Key not found in hash table\n");
#endif // LU_HASH_DEBUG

	// Return NULL if no matching key is found
	return NULL;
}

/**
 * @brief Deletes a key from the hash table.
 *
 * This function locates the appropriate bucket in the hash table using the hash function,
 * identifies the type of the bucket (linked list or red-black tree), and delegates the
 * deletion operation to the corresponding bucket-specific delete function. After the deletion,
 * the element count in the hash table is decremented.
 *
 * @param table A pointer to the hash table from which the key will be deleted.
 * @param key The key to delete from the hash table.
 * @return void
 */
void lu_hash_table_delete(lu_hash_table_t* table, int key)
{
	// Calculate the index of the bucket in the hash table using the hash function
	int index = lu_hash_function(key, table->table_size);

	// Retrieve the hash bucket at the calculated index
	lu_hash_bucket_t* bucket = &table->buckets[index];

	// Check the bucket type and call the corresponding delete function
	if (LU_HASH_BUCKET_LIST == bucket->type) {
		// Delete the key from the linked list bucket
		lu_hash_list_delete(bucket, key);
	}
	else if (LU_HASH_BUCKET_RBTREE == bucket->type) {
		// Delete the key from the red-black tree bucket
		lu_hash_rb_tree_delete(bucket, key);
	}

	// Decrement the total element count in the hash table
	table->element_count--;

#ifdef LU_HASH_DEBUG
	// Debug output to confirm deletion
	printf("Delete %d in bucket[%p]", key, &bucket);
#endif // LU_HASH_DEBUG
}

/**
 * @brief Destroys a hash table and frees all allocated memory.
 *
 * This function deallocates all resources associated with the given hash table, including its buckets
 * and their underlying data structures. Depending on the type of each bucket (linked list or red-black tree),
 * the corresponding destruction function is called. Finally, the memory allocated for the buckets and
 * the hash table itself is freed.
 *
 * @param table A pointer to the hash table to be destroyed. If the pointer is NULL, the function does nothing.
 * @return void
 */
void lu_hash_table_destroy(lu_hash_table_t* table)
{
	// Check if the table is NULL to avoid unnecessary operations
	if (table == NULL) {
		return;
	}

	// Iterate through each bucket in the hash table
	for (size_t i = 0; i < table->table_size; i++) {
		lu_hash_bucket_t* bucket = &table->buckets[i];

		// Destroy the bucket if it uses a linked list for storage
		if (bucket->type == LU_HASH_BUCKET_LIST) {
			lu_hash_list_destory(bucket);
		}
		// Destroy the bucket if it uses a red-black tree for storage
		else if (bucket->type == LU_HASH_BUCKET_RBTREE) {
			if (bucket->data.rb_tree != NULL) {
				lu_hash_rb_tree_destory(bucket);
			}
		}
	}

	// Free the memory allocated for the buckets array
	LU_MM_FREE(table->buckets);

	// Free the memory allocated for the hash table structure itself
	LU_MM_FREE(table);
}

/**
 * Converts a hash bucket's linked list to a red-black tree.
 *
 * This function takes a hash bucket that is implemented as a linked list,
 * initializes a new red-black tree, and transfers all elements from the linked
 * list to the red-black tree. Once the transfer is complete, it updates the bucket
 * to use the red-black tree as its underlying data structure.
 *
 * @param bucket Pointer to the hash bucket to be converted.
 * @return 1 on success, -1 on failure (e.g., memory allocation error or invalid bucket).
 */
static int lu_convert_bucket_to_rbtree(lu_hash_bucket_t* bucket)
{
	// Check if the bucket is valid and of the correct type
	if (!bucket || bucket->type != LU_HASH_BUCKET_LIST) {
#ifdef LU_HASH_DEBUG
		printf("Error: Invalid bucket or bucket is not a linked list.\n");
#endif //LU_HASH_DEBUG
		return -1; // Return error if the bucket is invalid or not a linked list
	}

	// Initialize the new red-black tree
	lu_rb_tree_t* new_tree = lu_rb_tree_init();
	if (!new_tree) {
#ifdef LU_HASH_DEBUG
		printf("Error: Memory allocation failed for red-black tree.\n");
#endif //LU_HASH_DEBUG
		return -1;// Return error if memory allocation for the red-black tree fails
	}

	// Transfer elements from the linked list to the red-black tree
	lu_hash_bucket_node_ptr_t node = bucket->data.list_head;// Start with the head of the list

	// Transfer all elements from the linked list to the red-black tree
	while (node)
	{
		lu_rb_tree_insert(new_tree, node->key, node->value); // Insert key-value pair into the red-black tree
		lu_hash_bucket_node_ptr_t temp = node; // Save current node pointer
		node = node->next; // Move to the next node
		LU_MM_FREE(temp); // Free the memory of the linked list node
	}

	// Update the bucket to use the red-black tree
	bucket->data.list_head = NULL;			// Clear the linked list head
	bucket->type = LU_HASH_BUCKET_RBTREE;	// Update the bucket type
	bucket->data.rb_tree = new_tree;		// Point to the new red-black tree
#ifdef LU_HASH_DEBUG
	printf("Bucket[%p] successfully converted to red-black tree.\n", &bucket);
#endif
	return 1; // Indicate successful conversion
}

/**
 * Initializes a red-black tree.
 * Allocates memory for the tree and its sentinel node (`nil`), and sets
 * up the tree structure with `nil` as its root and children.
 *
 * @return Pointer to the initialized red-black tree, or exits the program if memory allocation fails.
 */
static lu_rb_tree_t* lu_rb_tree_init(void)
{
	lu_rb_tree_t* rb_tree = (lu_rb_tree_t*)LU_MM_MALLOC(sizeof(lu_rb_tree_t));
	if (NULL == rb_tree) {
#ifdef LU_HASH_DEBUG
		printf("Error ops! rb_tree is NULL in lu_rb_tree_init function\n");
#endif // LU_HASH_DEBUG
		lu_hash_erron_global_ = LU_ERROR_OUT_OF_MEMORY;
		exit(lu_hash_erron_global_);
	}

	// Allocate memory for the sentinel node (`nil`)
	rb_tree->nil = (lu_rb_tree_node_t*)LU_MM_MALLOC(sizeof(lu_rb_tree_node_t));
	if (NULL == rb_tree->nil) {
#ifdef LU_HASH_DEBUG
		printf("Error ops! rb_tree->nil is NULL in lu_rb_tree_init function\n");
#endif // LU_HASH_DEBUG
		LU_MM_FREE(rb_tree);
		lu_hash_erron_global_ = LU_ERROR_OUT_OF_MEMORY;
		exit(lu_hash_erron_global_);
	}

	/**Initialize the `nil` sentinel node*/
	// Sentinel node is always black
	rb_tree->nil->color = BLACK;
	rb_tree->nil->left = rb_tree->nil;
	rb_tree->nil->right = rb_tree->nil;
	rb_tree->nil->parent = rb_tree->nil;
	// Set the root to point to `nil`
	rb_tree->root = rb_tree->nil;

#ifdef LU_HASH_DEBUG
	//printf("Red-black tree initialized successfully. Root: %p, Nil: %p\n", rb_tree->root, rb_tree->nil);
#endif // LU_HASH_DEBUG

	return rb_tree;
}

/**
 * @brief Inserts a new node into the red-black tree with the specified key and value.
 *
 * This function inserts a new node into the red-black tree, maintaining the properties of the
 * red-black tree. If the tree or its nil sentinel node is uninitialized, or if memory allocation
 * for the new node fails, the function will exit early with an error message (if debugging is enabled).
 *
 * @param tree Pointer to the red-black tree.
 * @param key The key for the new node.
 * @param value The value associated with the key in the new node.
 */
static void lu_rb_tree_insert(lu_rb_tree_t* tree, int key, void* value)
{
	if (NULL == tree || NULL == tree->nil) {
#ifdef LU_HASH_DEBUG
		printf("Error: RB-tree or tree->nil is not initialized\n");
#endif // LU_HASH_DEBUG
		lu_hash_erron_global_ = LU_ERROR_TREE_OR_NIL_NOT_INIT;
		return;
	}
	lu_rb_tree_node_t* new_node = (lu_rb_tree_node_t*)LU_MM_MALLOC(sizeof(lu_rb_tree_node_t));
	if (NULL == new_node) {
#ifdef LU_HASH_DEBUG
		printf("Error: Memory allocation failed in not initialized!(lu_rb_tree_node_t)\n");
#endif // LU_HASH_DEBUG
		return;
	}

	// Initialize the new node with the given key and value.
	new_node->key = key;
	new_node->value = value;
	new_node->color = RED;
	new_node->left = new_node->right = new_node->parent = tree->nil;

	if (tree->root == tree->nil) {
		// Case 1:The tree is empty, so the new node becomes the root.
		tree->root = new_node;
		new_node->color = BLACK; // Root is always black.
	}
	else {
		// Case 2: Find the correct position for the new node.
		lu_rb_tree_node_t* parent = tree->root;// Pointer to track the parent of the new node.
		lu_rb_tree_node_t* current = tree->root;// Pointer to traverse the tree.

		// Traverse the tree to find the insertion point.
		while (current != tree->nil) {
			parent = current;
			if (key < current->key) {
				current = current->left;
			}
			else {
				current = current->right;
			}
		}

		// Set the parent of the new node and attach it as a child of the parent.
		new_node->parent = parent;
		if (key < parent->key) {
			parent->left = new_node;
		}
		else {
			parent->right = new_node;
		}
		// Sanity check: Ensure new node and tree are valid before fixing violations.
		if (new_node == NULL || tree == NULL) {
#ifdef LU_HASH_DEBUG
			printf("Error: Invalid node or tree during fixup\n");
#endif
			return;
		}

		// Fix any violations of the red-black tree properties.
		lu_rb_tree_insert_fixup(tree, new_node);
	}
}

/**
 * @brief Searches for a value in a hash bucket's linked list using a given key.
 *
 * This function iterates through a hash bucket's linked list to find the node
 * that matches the specified key. If the key is found, the corresponding value
 * is returned; otherwise, the function returns NULL.
 *
 * @param bucket A pointer to the hash bucket containing the linked list.
 * @param key A pointer to the key to search for in the linked list.
 * @return A point to the node
 */
static lu_hash_bucket_node_t* lu_hash_list_find(lu_hash_bucket_t* bucket, int key)
{
	//When data internal type == LU_HASH_BUCKET_LIST

	lu_hash_bucket_node_ptr_t node = bucket->data.list_head;
	while (node != NULL)
	{
		if (node->key == key) {
			return node;
		}
		node = node->next;
	}

#ifdef LU_HASH_DEBUG
	printf("Not find the element in the list\n");
#endif // LU_HASH_DEBUG

	return NULL;
}

/**
 * @brief Searches for a key in a red-black tree within the given hash bucket.
 *
 * This function performs a search operation in a red-black tree structure,
 * starting from the root node. It compares the given key with the keys in
 * the tree nodes and traverses the tree accordingly (left or right) until the
 * key is found or the search reaches a `nil` node. If the key is found, a
 * pointer to the corresponding node is returned; otherwise, the function returns NULL.
 *
 * @param bucket A pointer to the hash bucket containing the red-black tree.
 * @param key A pointer to the key being searched for in the red-black tree.
 * @return A pointer to the tree node if the key is found, or NULL if the key does not exist in the tree.
 */
static lu_rb_tree_node_t* lu_hash_rb_tree_find(lu_rb_tree_t* tree, int key)
{
	lu_rb_tree_node_t* current = tree->root;
	while (current != tree->nil) {
		if (key == current->key) {
			return current;
		}
		else if (key < current->key) {
			current = current->left;
		}
		else {
			current = current->right;
		}
	}
#ifdef LU_HASH_DEBUG
	printf("Not find the element in the rb-tree\n");
#endif // LU_HASH_DEBUG

	return NULL;
}

/**
 * @brief Deletes a node with the specified key from a linked list in a hash bucket.
 *
 * This function searches for a node with the given key in the linked list within the specified hash bucket.
 * If the node is found, it is removed from the linked list, its memory is deallocated, and the bucket's
 * element count (`esize_bucket`) is decremented. If the key is not found, no action is taken.
 *
 * @param bucket A pointer to the hash bucket containing the linked list.
 * @param key A pointer to the key of the node to delete from the linked list.
 * @return void
 */
static void lu_hash_list_delete(lu_hash_bucket_t* bucket, int key)
{
	// Pointers to track the current node and its previous node
	lu_hash_bucket_node_ptr_t prev = NULL;
	lu_hash_bucket_node_ptr_t node = bucket->data.list_head;

	// Iterate through the linked list to find the node with the matching key
	while (node != NULL) {
		// If the key matches the current node's key
		if (node->key == (key)) {
			// If the node to delete is the head of the list
			if (prev == NULL) {
				bucket->data.list_head = node->next;
			}
			else {
				// Link the previous node to the next node, bypassing the current node
				prev->next = node->next;
			}
			// Free the memory allocated for the node
			LU_MM_FREE(node);
			return;
		}
		// Move to the next node in the list, updating the previous node pointer
		prev = node;
		node = node->next;
	}

	// Decrement the bucket's element count after deletion
	bucket->esize_bucket--;
}

/**
 * @brief Deletes a node with the specified key from a red-black tree in a hash bucket.
 *
 * This function deletes a node with the given key from the red-black tree stored in a hash bucket.
 * If the key is found, the node is removed from the tree while maintaining the red-black tree
 * properties (balance and color rules). If the key does not exist in the tree, the function does nothing.
 * The function also decrements the bucket's element count after a successful deletion.
 *
 * @param bucket A pointer to the hash bucket containing the red-black tree.
 * @param key A pointer to the key of the node to be deleted from the red-black tree.
 * @return void
 */
static void lu_hash_rb_tree_delete(lu_hash_bucket_t* bucket, int key)
{
	// Find the node with the given key in the red-black tree
	lu_rb_tree_node_t* node = lu_hash_rb_tree_find(bucket->data.rb_tree, key);
	if (node == NULL) {
		return; // Key not found, no action needed
	}

	// Temporary variables for node manipulation
	lu_rb_tree_node_t* y = node;
	lu_rb_tree_node_t* x;
	lu_rb_tree_color_t original_color = y->color;

	// Case 1: Node has no left child
	if (node->left == bucket->data.rb_tree->nil) {
		x = node->right;
		lu_rb_tree_transplant(bucket->data.rb_tree, node, node->right);
	}
	// Case 2: Node has no right child
	else if (node->right == bucket->data.rb_tree->nil) {
		x = node->left;
		lu_rb_tree_transplant(bucket->data.rb_tree, node, node->left);
	}
	// Case 3: Node has two children
	else {
		y = lu_rb_tree_minimum(bucket->data.rb_tree, node->right); // Find the successor
		original_color = y->color;
		x = y->right;

		// If successor is not the direct child of the node
		if (y->parent != node) {
			lu_rb_tree_transplant(bucket->data.rb_tree, y, y->right);
			y->right = node->right;
			y->right->parent = y;
		}

		// Replace the node with its successor
		lu_rb_tree_transplant(bucket->data.rb_tree, node, y);
		y->left = node->left;
		y->left->parent = y;
		y->color = node->color;
	}

	// Fix red-black tree properties if a black node was removed
	if (original_color == BLACK) {
		lu_rb_tree_delete_fixup(bucket->data.rb_tree, x);
	}

	// Free the memory allocated for the deleted node
	LU_MM_FREE(node);

	// Decrement the bucket's element count
	bucket->esize_bucket--;
}

/**
 * @brief Fixes violations of red-black tree properties after an insertion.
 *
 * This function ensures that the red-black tree properties are restored after
 * a node is inserted. It resolves cases where the tree may temporarily violate
 * properties such as the double-red rule (a red node cannot have a red parent).
 *
 * @param tree Pointer to the red-black tree.
 * @param node Pointer to the newly inserted node.
 */
static void lu_rb_tree_insert_fixup(lu_rb_tree_t* tree, lu_rb_tree_node_t* node)
{
	// While the current node is not the root and its parent is red
	while (node != tree->root && node->parent != tree->nil && node->parent->color == RED) {
		// Ensure node->parent is not nil and has a parent
		lu_rb_tree_node_t* parent = node->parent; // Parent of the current node
		lu_rb_tree_node_t* grandparent = parent->parent; // Grandparent of the current node

		// Case 1: Parent is the left child of the grandparent
		if (parent == grandparent->left) {
			// Uncle is the right child of the grandparent
			lu_rb_tree_node_t* uncle = grandparent->right;

			// Case 1.1: Uncle is RED
			if (uncle != tree->nil && uncle != NULL && uncle->color == RED) {
				// Case 1: Uncle is RED, recoloring required
				parent->color = BLACK;
				uncle->color = BLACK;
				grandparent->color = RED;
				node = grandparent; // Move up to the grandparent for further checks
			}
			else {
				// Uncle is BLACK or NULL, perform rotations
				if (node == parent->right) {
					node = parent;// Move node up to parent
					lu_rb_tree_left_rotate(tree, node); // Ensure node is valid
				}
				parent->color = BLACK;
				grandparent->color = RED;
				lu_rb_tree_right_rotate(tree, grandparent);
			}
		}
		else {
			// Case 2: Parent is the right child of the grandparent

			//printf("Parent: %p, Grandparent: %p, Uncle: %p\n", parent, grandparent, grandparent->left);

			// Symmetric case: node->parent is the right child of the grandparent
			lu_rb_tree_node_t* uncle = grandparent->left; // Uncle is the left child of the grandparent

			// Check if uncle is valid and not nil       // Case 2.1: Uncle is RED
			if (uncle != tree->nil && uncle != NULL && uncle->color == RED) {
				// Case 1: Uncle is RED, recoloring required
				parent->color = BLACK;
				uncle->color = BLACK;
				grandparent->color = RED;
				node = grandparent; // Move the node up
			}
			else {
				// Uncle is BLACK or NULL, perform rotations
				if (node == parent->left) {
					node = parent;
					lu_rb_tree_right_rotate(tree, node); // Ensure node is valid
				}
				parent->color = BLACK;
				grandparent->color = RED;
				lu_rb_tree_left_rotate(tree, grandparent); // Perform right rotation on parent
			}
		}
	}
	// Ensure the root is always black, as required by red-black tree properties
	tree->root->color = BLACK; // Ensure root is always black
}

/**
 * @brief Performs a right rotation on the given node in the red-black tree.
 *
 * A right rotation is a tree balancing operation where the left child of a node
 * becomes its parent, and the node becomes the right child of its previous left child.
 * This operation maintains the binary search tree structure and is used in red-black
 * tree balancing algorithms.
 *
 * Before Rotation:
 *         node
 *        /    \
 *     left     c
 *    /    \
 *   a      b
 *
 * After Rotation:
 *        left
 *       /    \
 *      a     node
 *           /    \
 *          b      c
 *
 * @param tree Pointer to the red-black tree structure.
 * @param node Pointer to the node on which the right rotation is performed.
 */
static void lu_rb_tree_right_rotate(lu_rb_tree_t* tree, lu_rb_tree_node_t* node)
{
	// Ensure the node has a left child to rotate with
	if (node->left == tree->nil) {
		return;   // Cannot perform right rotation if the left child is nil
	}
	// Step 1: Extract the left child of the node
	lu_rb_tree_node_t* left = node->left;

	// Step 2: Move the right subtree of the left child to the left subtree of the node
	node->left = left->right;

	if (left->right != tree->nil) {
		left->right->parent = node;// Update parent of the moved subtree
	}

	// Step 3: Update the parent of the left child
	left->parent = node->parent;

	// Step 4: Update the parent's child pointer to point to the left child
	if (node->parent == tree->nil) {
		tree->root = left;// If the node is the root, update the root pointer
	}
	else if (node == node->parent->left) { // If the node is a left child, update parent's left pointer
		node->parent->left = left;// If the node is a right child, update parent's right pointer
	}
	else {
		node->parent->right = left;
	}
	// Step 5: Update pointers to complete the rotation
	left->right = node;// Make the original node the right child of its left child
	node->parent = left;// Update the parent of the original node
}

/**
 * @brief Performs a left rotation on the given node in the red-black tree.
 *
 * A left rotation is a tree balancing operation where the right child of a node
 * becomes its parent, and the node becomes the left child of its previous right child.
 * This operation maintains the binary search tree structure and is used in red-black
 * tree balancing algorithms.
 *
 * Before Rotation:
 *         node
 *        /    \
 *       a     right
 *            /     \
 *           b       c
 *
 * After Rotation:
 *         right
 *        /    \
 *      node     c
 *     /    \
 *    a      b
 *
 * @param tree Pointer to the red-black tree structure.
 * @param node Pointer to the node on which the left rotation is performed.
 */
static void lu_rb_tree_left_rotate(lu_rb_tree_t* tree, lu_rb_tree_node_t* node)
{
	// Ensure the node has a right child to rotate with
	if (node->right == tree->nil) {
		return; // Cannot perform left rotation if the right child is nil
	}

	// Step 1: Extract the right child of the node
	lu_rb_tree_node_t* right = node->right;

	// Step 2: Move the left subtree of the right child to the right subtree of the node
	node->right = right->left;
	if (right->left != tree->nil) {
		right->left->parent = node; // Update parent of the moved subtree
	}

	// Step 3: Update the parent of the right child
	right->parent = node->parent;

	// Step 4: Update the parent's child pointer to point to the right child
	if (node->parent == tree->nil) {
		tree->root = right; // If the node is the root, update the root pointer
	}
	else if (node == node->parent->left) {
		node->parent->left = right; // If the node is a left child, update parent's left pointer
	}
	else {
		node->parent->right = right; // If the node is a right child, update parent's right pointer
	}

	// Step 5: Update pointers to complete the rotation
	right->left = node;        // Make the original node the left child of its right child
	node->parent = right;      // Update the parent of the original node
}

/**
 * @brief Performs a left rotation on a node in a red-black tree during a delete operation.
 *
 * This function performs a left rotation around the specified node in the red-black tree.
 * A left rotation moves the node's right child into its position and adjusts the relationships
 * between the parent, left, and right child nodes. The red-black tree's structure and properties
 * are preserved during this operation.
 *
 * @param tree A pointer to the red-black tree where the rotation will occur.
 * @param node A pointer to the node to be rotated to the left.
 * @return void
 */
static void lu_rb_tree_left_rotate_delete(lu_rb_tree_t* tree, lu_rb_tree_node_t* node)
{
	// Get the right child of the node
	lu_rb_tree_node_t* right = node->right;

	// Move the left child of the right node to the right of the current node
	node->right = right->left;
	if (right->left != tree->nil) {
		right->left->parent = node; // Update the parent of the left child
	}

	// Update the parent of the right node
	right->parent = node->parent;

	// If the current node is the root, update the root pointer
	if (node->parent == tree->nil) {
		tree->root = right;
	}
	// Otherwise, update the parent's left or right child to point to the right node
	else if (node == node->parent->left) {
		node->parent->left = right;
	}
	else {
		node->parent->right = right;
	}

	// Complete the rotation by making the current node the left child of the right node
	right->left = node;
	node->parent = right;
}

/**
 * @brief Performs a right rotation on a node in a red-black tree during a delete operation.
 *
 * This function performs a right rotation around the specified node in the red-black tree.
 * A right rotation moves the node's left child into its position and adjusts the relationships
 * between the parent, left, and right child nodes. The red-black tree's structure and properties
 * are preserved during this operation.
 *
 * @param tree A pointer to the red-black tree where the rotation will occur.
 * @param node A pointer to the node to be rotated to the right.
 * @return void
 */
static void lu_rb_tree_right_rotate_delete(lu_rb_tree_t* tree, lu_rb_tree_node_t* node)
{
	// Get the left child of the node
	lu_rb_tree_node_t* left = node->left;

	// Move the right child of the left node to the left of the current node
	node->left = left->right;
	if (left->right != tree->nil) {
		left->right->parent = node; // Update the parent of the right child
	}

	// Update the parent of the left node
	left->parent = node->parent;

	// If the current node is the root, update the root pointer
	if (node->parent == tree->nil) {
		tree->root = left;
	}
	// Otherwise, update the parent's right or left child to point to the left node
	else if (node == node->parent->right) {
		node->parent->right = left;
	}
	else {
		node->parent->left = left;
	}

	// Complete the rotation by making the current node the right child of the left node
	left->right = node;
	node->parent = left;
}

/**
 * @brief Replaces one subtree with another in a red-black tree.
 *
 * This function replaces the subtree rooted at node `u` with the subtree rooted at node `v` in the
 * red-black tree. It updates the parent pointers and adjusts the tree structure accordingly. This
 * operation is typically used during deletion operations in the red-black tree.
 *
 * @param tree A pointer to the red-black tree.
 * @param u A pointer to the node being replaced.
 * @param v A pointer to the node replacing `u`.
 * @return void
 */
static void lu_rb_tree_transplant(lu_rb_tree_t* tree, lu_rb_tree_node_t* u, lu_rb_tree_node_t* v)
{
	// If `u` is the root of the tree, update the root to point to `v`
	if (u->parent == tree->nil) {
		tree->root = v;
	}
	// If `u` is the left child of its parent, update the parent's left child to `v`
	else if (u == u->parent->left) {
		u->parent->left = v;
	}
	// Otherwise, update the parent's right child to `v`
	else {
		u->parent->right = v;
	}

	// If `v` is not a sentinel (nil) node, update its parent to `u`'s parent
	if (v != tree->nil) {
		v->parent = u->parent;
	}
}

/**
 * @brief Restores the red-black tree properties after a node deletion.
 *
 * This function fixes violations of red-black tree properties that may occur
 * during the deletion of a node. It ensures that the tree remains balanced and
 * adheres to the red-black tree properties (e.g., every path from a node to its
 * descendant leaves has the same number of black nodes). It uses rotations and
 * color adjustments to restore these properties.
 *
 * @param tree A pointer to the red-black tree where the fixup is applied.
 * @param node A pointer to the node that may cause violations after deletion.
 * @return void
 */
static void lu_rb_tree_delete_fixup(lu_rb_tree_t* tree, lu_rb_tree_node_t* node)
{
	// Continue until the node is the root or is no longer violating the black-depth property
	while (node != tree->root && node->color == BLACK) {
		// If the node is the left child of its parent
		if (node == node->parent->left) {
			lu_rb_tree_node_t* sibling = node->parent->right;

			// Case 1: Sibling is red
			if (sibling->color == RED) {
				sibling->color = BLACK;
				node->parent->color = RED;
				lu_rb_tree_left_rotate_delete(tree, node->parent);
				sibling = node->parent->right;
			}

			// Case 2: Sibling and its children are black
			if (sibling->left->color == BLACK && sibling->right->color == BLACK) {
				sibling->color = RED;
				node = node->parent;
			}
			// Case 3: Sibling's left child is red, right child is black
			else {
				if (sibling->right->color == BLACK) {
					sibling->left->color = BLACK;
					sibling->color = RED;
					lu_rb_tree_right_rotate_delete(tree, sibling);
					sibling = node->parent->right;
				}
				// Case 4: Sibling's right child is red
				sibling->color = node->parent->color;
				node->parent->color = BLACK;
				sibling->right->color = BLACK;
				lu_rb_tree_left_rotate_delete(tree, node->parent);
				node = tree->root;
			}
		}
		// Symmetric case: node is the right child of its parent
		else {
			lu_rb_tree_node_t* sibling = node->parent->left;

			// Case 1: Sibling is red
			if (sibling->color == RED) {
				sibling->color = BLACK;
				node->parent->color = RED;
				lu_rb_tree_right_rotate_delete(tree, node->parent);
				sibling = node->parent->left;
			}

			// Case 2: Sibling and its children are black
			if (sibling->right->color == BLACK && sibling->left->color == BLACK) {
				sibling->color = RED;
				node = node->parent;
			}
			// Case 3: Sibling's right child is red, left child is black
			else {
				if (sibling->left->color == BLACK) {
					sibling->right->color = BLACK;
					sibling->color = RED;
					lu_rb_tree_left_rotate_delete(tree, sibling);
					sibling = node->parent->left;
				}
				// Case 4: Sibling's left child is red
				sibling->color = node->parent->color;
				node->parent->color = BLACK;
				sibling->left->color = BLACK;
				lu_rb_tree_right_rotate_delete(tree, node->parent);
				node = tree->root;
			}
		}
	}

	// Ensure the final node is black
	node->color = BLACK;
}

/**
 * @brief Finds the node with the minimum key in a red-black tree.
 *
 * This function traverses the leftmost path of the red-black tree starting from the
 * given node and returns the node with the minimum key. The minimum key node is
 * always located at the leftmost leaf of the subtree.
 *
 * @param tree A pointer to the red-black tree.
 * @param node A pointer to the node from which to start the search for the minimum.
 * @return A pointer to the node with the minimum key.
 */
static lu_rb_tree_node_t* lu_rb_tree_minimum(lu_rb_tree_t* tree, lu_rb_tree_node_t* node)
{
	// Traverse the leftmost path of the tree to find the minimum key node
	while (node->left != tree->nil) {
		node = node->left;
	}

	// Return the node with the minimum key
	return node;
}

/**
 * @brief Finds the node with the maximum key in a red-black tree.
 *
 * This function traverses the rightmost path of the red-black tree starting from the
 * given node and returns the node with the maximum key. The maximum key node is
 * always located at the rightmost leaf of the subtree.
 *
 * @param tree A pointer to the red-black tree.
 * @param node A pointer to the node from which to start the search for the maximum.
 * @return A pointer to the node with the maximum key.
 */
static lu_rb_tree_node_t* lu_rb_tree_maximum(lu_rb_tree_t* tree, lu_rb_tree_node_t* node)
{
	// Traverse the rightmost path of the tree to find the maximum key node
	while (node->right != tree->nil) {
		node = node->right;
	}

	// Return the node with the maximum key
	return node;
}

static void lu_rb_tree_destroy_node(lu_rb_tree_t* tree, lu_rb_tree_node_t* node)
{
	if (node == tree->nil) {
		return;
	}

	lu_rb_tree_destroy_node(tree, node->left);
	lu_rb_tree_destroy_node(tree, node->right);

	LU_MM_FREE(node);
}

/**
 * @brief Destroys a linked list stored in a hash bucket and frees all its memory.
 *
 * This function traverses a linked list stored in the given hash bucket, deallocates
 * the memory for each node, and sets the list head to `NULL` after the destruction
 * process is complete.
 *
 * @param bucket A pointer to the hash bucket containing the linked list to be destroyed.
 * @return void
 */
static void lu_hash_list_destory(lu_hash_bucket_t* bucket)
{
	// Get the head of the linked list
	lu_hash_bucket_node_ptr_t node = bucket->data.list_head;

	// Traverse the list and free each node
	while (node != NULL) {
		lu_hash_bucket_node_ptr_t temp = node; // Store the current node
		node = node->next;                    // Move to the next node
		LU_MM_FREE(temp);                     // Free the current node
	}

	// Set the list head to NULL to indicate the list is empty
	bucket->data.list_head = NULL;
}

/**
 * @brief Destroys a red-black tree stored in a hash bucket and frees all its memory.
 *
 * This function deallocates all memory associated with a red-black tree stored in the given
 * hash bucket. It first recursively frees all nodes in the tree, then deallocates the sentinel
 * `nil` node, and finally frees the tree structure itself.
 *
 * @param bucket A pointer to the hash bucket containing the red-black tree to be destroyed.
 * @return void
 */
static void lu_hash_rb_tree_destory(lu_hash_bucket_t* bucket)
{
	// If the red-black tree is NULL, nothing to destroy
	if (bucket->data.rb_tree == NULL) {
		return;
	}

	// Recursively destroy all nodes in the red-black tree starting from the root
	lu_rb_tree_destroy_node(bucket->data.rb_tree, bucket->data.rb_tree->root);

	// Free the sentinel nil node if it is not NULL
	if (bucket->data.rb_tree->nil != NULL) {
		LU_MM_FREE(bucket->data.rb_tree->nil);
	}

	// Free the memory allocated for the red-black tree structure
	LU_MM_FREE(bucket->data.rb_tree);
}

static void lu_rb_tree_rehash(lu_rb_tree_t* tree, lu_rb_tree_node_t* node, lu_hash_bucket_t* new_buckets, int new_table_size, lu_rb_tree_node_t* nil)
{
	if (node != nil) {
		lu_rb_tree_rehash(tree, node->left, new_buckets, new_table_size, nil);
		lu_rb_tree_rehash(tree, node->right, new_buckets, new_table_size, nil);

		int new_index = lu_hash_function(node->key, new_table_size);
		lu_hash_bucket_t* new_bucket = &new_buckets[new_index];

		if (new_bucket->type == LU_HASH_BUCKET_LIST) {
			lu_hash_bucket_node_ptr_t new_node = (lu_hash_bucket_node_ptr_t)LU_MM_MALLOC(sizeof(lu_hash_bucket_node_t));
			new_node->key = node->key;
			new_node->value = node->value;
			new_node->next = new_bucket->data.list_head;
			new_bucket->data.list_head = new_node;
			new_bucket->esize_bucket++;
		}
		else if (new_bucket->type == LU_HASH_BUCKET_RBTREE) {
			lu_rb_tree_insert(new_bucket->data.rb_tree, node->key, node->value);
			new_bucket->esize_bucket++;
		}
	}
}

static void lu_hash_table_resize(lu_hash_table_t* table)
{
	int new_table_size = table->table_size * 2;
	lu_hash_bucket_t* new_buckets = (lu_hash_bucket_t*)LU_MM_CALLOC(new_table_size, sizeof(lu_hash_bucket_t));

	for (int i = 0; i < new_table_size; i++) {
		new_buckets[i].type = LU_HASH_BUCKET_LIST;
		new_buckets[i].data.list_head = NULL;
		new_buckets[i].esize_bucket = 0;
	}

	for (size_t i = 0; i < table->table_size; i++) {
		lu_hash_bucket_t* old_bucket = &table->buckets[i];
		if (old_bucket->type == LU_HASH_BUCKET_LIST) {
			lu_hash_bucket_node_t* node = old_bucket->data.list_head;
			while (node) {
				int new_index = lu_hash_function(node->key, new_table_size);
				lu_hash_bucket_t* new_bucket = &new_buckets[new_index];

				lu_hash_bucket_node_ptr_t new_node = (lu_hash_bucket_node_ptr_t)LU_MM_MALLOC(sizeof(lu_hash_bucket_node_t));
				new_node->key = node->key;
				new_node->value = node->value;
				new_node->next = new_bucket->data.list_head;
				new_bucket->data.list_head = new_node;
				new_bucket->esize_bucket++;

				node = node->next;
			}
		}
		else if (old_bucket->type == LU_HASH_BUCKET_RBTREE) {
			// Handle red-black tree bucket rehashing
			// This part can be implemented similarly by traversing the tree and rehashing each node
			lu_rb_tree_rehash(old_bucket->data.rb_tree, old_bucket->data.rb_tree->root, new_buckets, new_table_size, old_bucket->data.rb_tree->nil);
		}
	}

	LU_MM_FREE(table->buckets);
	table->buckets = new_buckets;
	table->table_size = new_table_size;
}