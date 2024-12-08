#ifndef LU_MM_INTERNAL_H
#define LU_MM_INTERNAL_H

#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif

#define mm_malloc(sz) 			lu_event_mm_malloc_(sz)
#define mm_calloc(count, size) 	lu_event_mm_calloc_((count), (size))
#define mm_strdup(s) 			lu_event_mm_strdup_(s)
#define mm_realloc(p, sz) 		lu_event_mm_realloc_((p), (sz))
#define mm_free(p) 				lu_event_mm_free_(p)

#ifdef LU_EVENT__DISABLE_MM_REPLACEMENT 
#define mm_malloc(sz) malloc(sz)
#define mm_calloc(n, sz) calloc((n), (sz))
#define mm_strdup(s) strdup(s)
#define mm_realloc(p, sz) realloc((p), (sz))
#define mm_free(p) free(p)
#endif

#ifdef __cplusplus
}
#endif


#endif //LU_MM_INTERNAL_H