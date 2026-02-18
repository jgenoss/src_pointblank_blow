

namespace i3
{
	inline
	void*		large_object_cache_base::private_malloc(size_t size) { return i3::heap_malloc(m_private_heap, size);  }

	inline
	void		large_object_cache_base::private_free(void* data) { i3::heap_free(m_private_heap, data);  }
}
