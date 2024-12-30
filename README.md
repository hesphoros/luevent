## Development Log

## Monday 30 Dec 2024
- [X] In the lu_error.c file,replace the internal-struct error_map to lu_hash_table.
- [X] In the lu_event.c lu_event-internal.h file,imple the structure "lu_event_t" "lu_event_base_t" "lu_event_change_t" and so on.
- [X] In the lu_event.c file,imple the function "lu_event_new_with_config()" (but not the implementation)

### Friday 29 Dec 2024
- [X] Finished implementing 'lu_hash_resize' function.


### Saturday 28 Dec 2024
- [X] Open-sourced the `lu_hash_table` library.
- [X] Improved the log.c log output interface.

### Sunday 22 Dec 2024
- [X] Completed `lu_hash_table` insert encapsulation.
- [X] Added `element_count` counter inside `lu_hash_table_t` to track the number of elements.
- [X] Added `esize_bucket` counter inside `lu_hash_bucket_t` to track the number of elements in each bucket.

### Friday 20 Dec 2024
- [X] Refined `lu_rb_tree` encapsulation.
- [X] Improved `lu_utils` `vsnprintf` and socket error handling implementation.

### Thursday 19 Dec 2024
- [X] Completed `lu_rb_tree` encapsulation.
- [X] Fixed `lu_hash_insert` function to check for duplicates and update existing data rather than directly inserting.

### Wednesday 18 Dec 2024
- [X] Completed test cases for `lu_hash_table`.
- [X] Tested insert, find, delete operations, and transformations between internal linked list and red-black tree.
- [X] Improved documentation.

### Tuesday 17 Dec 2024
- [X] Finished implementing internal `hash_table` with red-black tree support.
- [X] Optimized hash function (multiplicative hash + bitwise operations).

### Monday 16 Dec 2024
- [X] Fixed memory leak in `lu_hash_table_insert` function.
- [X] Fixed list pointer clearing issue when converting from linked list to red-black tree.

### Sunday 15 Dec 2024
- [X] Started implementing internal hash table based on `uthash` library.

### Saturday 14 Dec 2024
- [X] Refined `lu_log.c` and `lu_log-internal.h` for logging functionality.
- [X] Fixed socket error handling and added string conversion for socket error codes using hash table.
- [X] Introduced inline function wrapping for `vsnprintf` and `snprintf` for cross-platform compatibility.

### Friday 13 Dec 2024
- [X] Implemented default logging mechanisms and custom logging callbacks for error and fatal logging.
- [X] Introduced macros for conditional debug and memory logging.

### Wednesday 11 Dec 2024
- [X] Enhanced `lu_error.h` and `lu_error.c` with error code definitions and lazy-loaded string mapping.
- [X] Completed interface for error string retrieval: `lu_get_error_string` and `lu_get_error_string_hash`.

### Tuesday 10 Dec 2024
- [X] Completed `mm-internal.h` header encapsulation and memory management functions (`mm_malloc`, `mm_calloc`, `mm_strdup`, `mm_realloc`, `mm_free`).
- [X] Finished implementation of conditional memory logging and default log to `memory_management.log` file.

### Monday 9 Dec 2024
- [X] Encapsulated `lu_mm-internal.h` header file.
- [X] Added macro `LU_EVENT__DISABLE_CUSTOM_MM_REPLACEMENT` for disabling custom memory management.
