#pragma once

#include "v_pool.h"
#include "v_common_pool.h"
#include "v_none_pool.h"
#include "v_invalid_pool.h"
#include "v_storage_pool_mt.h"
#include "v_compact_pool.h"
 
namespace i3
{
	namespace detail
	{
		template<class T>
		struct default_pool_select
		{
			typedef typename mpl::if_<i3::is_abstract<T>, v_invalid_pool, 
				v_none_pool >::type type;
		};
		// 디폴트풀을 common_pool --> none_pool로 변경..(현재 1.1버전 PointBlank의 메모리사용량이 아직 너무 많아서 나중에 큰블록할당이 잘 안된다.)
		// 2012.10.18.수빈

		struct safe_alignment_if_abstract { static const size_t value = 0; };
		
		template<class T>
		struct safe_alignment_of
		{
			typedef typename mpl::if_< is_abstract<T>, 
				detail::safe_alignment_if_abstract, 
				i3::alignment_of<T> >::type type;
			static const size_t value = type::value;
		};
	}

	class v_pool_info : private noncopyable //: public i3::class_common_pool<v_pool_info, noncopyable>
	{
	public:
		template<class T>
		v_pool_info(i3::identity<T>) : 
			m_pool( create_v_pool_ptr< typename detail::default_pool_select<T>::type >() ), 
			m_class_size(sizeof(T)) {}
		v_pool_info(const v_pool_ptr& p) : m_pool(p) {}
		void			set_pool(const v_pool_ptr& p) { m_pool = p; }
		std::size_t		get_class_size() const { return m_class_size; }
		const char*		get_pool_name() const { return m_pool->type_name(); }
#if !defined(_DEBUG)
		void*			allocate() { return m_pool->allocate(m_class_size); }
#else
		void*			allocate(const char* file, int line) { return m_pool->allocate(m_class_size, file, line); }
#endif
		void			deallocate(void* data) { m_pool->deallocate(data); }
	private:
		v_pool_ptr		m_pool;
		std::size_t		m_class_size;
	};
}
