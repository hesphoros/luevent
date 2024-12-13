#ifndef LU_EVENT_VISIBILITY_H_INCLUDED_
#define LU_EVENT_VISIBILITY_H_INCLUDED_

/**
 * @file lu_visibility.h 
 * @brief 定义符号导出
 * @author <Lucifer gj3372819612@gmail>
 * @version 1.0
 * @date 2024-12-13
 * @copyright Copyright (c) 2024, <Lucifer gj3372819612@gmail>. All rights reserved.
 * @details 定义符号导出，用于控制符号的可见性。
*/

#if defined(event_shared_EXPORTS) || \
    defined(event_extra_shared_EXPORTS) || \
    defined(event_core_shared_EXPORTS) || \
    defined(event_pthreads_shared_EXPORTS) || \
    defined(event_openssl_shared_EXPORTS) || \
    defined(event_mbedtls_shared_EXPORTS)
 

#ifdef  __GNUC__
#define LU_EVENT_EXPORT_SYMBOL __attribute__ ((visibility("default")))
#else
#define LU_EVENT_EXPORT_SYMBOL  /* unknown compiler */
#endif

#else /* event_*_EXPORTS */

#define LU_EVENT_EXPORT_SYMBOL

#endif /* event_*_EXPORTS */



/** 默认符号导出，如果没有指定特定的导出符号 */
#if !defined(LU_EVENT_CORE_EXPORT_SYMBOL)
# define LU_EVENT_CORE_EXPORT_SYMBOL LU_EVENT_EXPORT_SYMBOL
#endif


#endif /* LU_EVENT_VISIBILITY_H_INCLUDED_ */

