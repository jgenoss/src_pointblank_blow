#pragma once

#include "../iterator/iterator_facade.h"
#include "../optional/optional.h"

namespace i3
{
	namespace detail
	{
		struct void_type {};	

		template<class R> __forceinline
		const R&		operator,(const R& res, const void_type&) { return res;  }
		template<class R> __forceinline
		R&				operator,( R& res, const void_type&) {  return res;  }

	}

	template<class CallBound, class ListType, class VectorType>
	struct signal_call_iterator_cache
	{
		typedef typename mpl::if_<	i3::is_same<typename CallBound::result_type, void>, 
							detail::void_type, typename CallBound::result_type>::type	result_type;
		typedef ListType										list_type;
		typedef VectorType										weak_vector_type;
		
		signal_call_iterator_cache(const CallBound& bound, const list_type& list, weak_vector_type& vec) : 
			f(bound), holder_list(list), weak_list_cache(vec) {}
		~signal_call_iterator_cache() { weak_list_cache.clear(); }			// 위크포인터 리스트는 외부변수이므로 명시적으로 제거해야함..

		const CallBound&				f;
		const list_type&				holder_list;
		weak_vector_type&				weak_list_cache;
		i3::optional<result_type>		result_cache;	
	};

	template<class CallBound, class Cache>
	class signal_call_iterator : 
		public i3::iterator_facade< signal_call_iterator<CallBound, Cache>,
									typename Cache::result_type,				// ==> 이게 가능하려면 i3::function이거나, 함수객체여야한다.
									i3::forward_iterator_tag,
									typename const Cache::result_type&>
	{
		typedef i3::iterator_facade< signal_call_iterator<CallBound, Cache>,
									typename Cache::result_type,				// ==> 이게 가능하려면 i3::function이거나, 함수객체여야한다.
									i3::forward_iterator_tag,
									typename const Cache::result_type&> facade_base_type;

		typedef typename CallBound::result_type														result_type;						//
		typedef Cache																				cache_type;
		typedef typename cache_type::list_type														list_type;
		typedef typename list_type::const_iterator													connect_iterator;
		typedef typename list_type::value_type														shared_ptr_type;
		typedef typename cache_type::weak_vector_type												cache_weak_vector_type;
		typedef typename cache_weak_vector_type::value_type											weak_ptr_type;
		
		friend class i3::iterator_core_access;

	public:
		
		signal_call_iterator( connect_iterator it, cache_type& c) : m_connect_iter(it), m_next_callable_iter(c.holder_list.end()), m_cache(c)
		{
		}
		
		typedef typename facade_base_type::reference		reference;

	private:
		reference	dereference() const					// 함수호출이 들어가므로 주 함수임..
		{
			if (!m_cache.result_cache)						// 이 경우에만, 함수호출되도록 처리한다.증가반복자에서 리셋된다.
			{
				const shared_ptr_type&		holder = *m_connect_iter;
				weak_ptr_type			weak_for_check = holder;		// 

				m_cache.result_cache.reset( (m_cache.f( holder->m_fun ), detail::void_type()) );		// iterator 유효성이 반드시 보장되어야함..
				
				if (weak_for_check.expired())							// 삭제된듯 하다..// 리스트를 뒤져서 유효한 놈으로 갱신해줘야한다.
				{	//이전 노드들에 대한 기록 필요가 부담이 됨..// 캐시컨테이너에 일단 쌓아 놓은 기 순회노드들 조사로 처리해본다..
					//이전 노드들이 유효한게 없다면???
					
					m_next_callable_iter = m_cache.holder_list.begin();			// 일단 홀더리스트 시작점으로 먼저 놓아보고...
					m_connect_iter = m_cache.holder_list.end();					

					while ( !m_cache.weak_list_cache.empty() )
					{
						const weak_ptr_type& ptr = m_cache.weak_list_cache.back();
						
						if (!ptr.expired())
						{
							shared_ptr_type sp = ptr.lock();
							m_next_callable_iter = sp->m_iter;
							++m_next_callable_iter;
							break;
						}
						m_cache.weak_list_cache.pop_back();
					}
				
				}
				
			}																	
			
			return m_cache.result_cache.get();
		}
		
		void		increment()
		{	
			// 증가 전에 반복자 유효성 확인이 필요함..
			if (m_next_callable_iter != m_cache.holder_list.end() )
			{
				m_connect_iter = m_next_callable_iter;
				m_next_callable_iter = m_cache.holder_list.end();
			}
			else
			if ( m_connect_iter != m_next_callable_iter )		//  m_connect_iter != end() 여야한다..
			{
				const shared_ptr_type&		holder = *m_connect_iter;
				weak_ptr_type			weak_for_check = holder;		// 
				m_cache.weak_list_cache.push_back(weak_for_check);		
				++m_connect_iter;
			}
			m_cache.result_cache.reset();			
		}
		
		bool		equal(const signal_call_iterator& rhs) const { return m_connect_iter == rhs.m_connect_iter; }


	private:
		mutable connect_iterator		m_connect_iter;
		mutable connect_iterator		m_next_callable_iter;
		cache_type&						m_cache;					
	};

	
	
	
	
	
}