#pragma once


#include "tls_mgr.h"


namespace i3
{


	I3_EXPORT_BASE DWORD			get_curr_tls_thread_id();
	I3_EXPORT_BASE DWORD			get_num_tls_thread_id();
	I3_EXPORT_BASE unsigned long	get_num_tls_ptr();

	I3_EXPORT_BASE void*			tls_malloc(size_t n);
	I3_EXPORT_BASE void				tls_free(void* ptr);
	I3_EXPORT_BASE void*			tls_malloc(DWORD hthread, size_t n);
	I3_EXPORT_BASE void				tls_free(DWORD hthread, void* ptr);

	I3_EXPORT_BASE void*			tls_aligned_malloc(size_t n, size_t align);
	I3_EXPORT_BASE void				tls_aligned_free(void* ptr);
	I3_EXPORT_BASE void*			tls_aligned_malloc(DWORD hThread, size_t n, size_t align);
	I3_EXPORT_BASE void				tls_aligned_free(DWORD hThread, void* ptr);

	I3_EXPORT_BASE void*			tls_safe_malloc(size_t n);
	I3_EXPORT_BASE void				tls_safe_free(void* ptr);
	I3_EXPORT_BASE void*			tls_safe_malloc(DWORD hthread, size_t n);
	I3_EXPORT_BASE void				tls_safe_free(DWORD hthread, void* ptr);

	I3_EXPORT_BASE void*			tls_safe_aligned_malloc(size_t n, size_t align);
	I3_EXPORT_BASE void				tls_safe_aligned_free(void* ptr);
	I3_EXPORT_BASE void*			tls_safe_aligned_malloc(DWORD hThread, size_t n, size_t align);
	I3_EXPORT_BASE void				tls_safe_aligned_free(DWORD hThread, void* ptr);





}