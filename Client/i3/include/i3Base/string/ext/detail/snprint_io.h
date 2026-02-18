#pragma once

#include "sprint_object.h"
#include "stack_string_storage.h"

namespace i3
{
	namespace detail_sprintf
	{
		
		template<class Ch, class Tag = sprint_global_tag>
		class snprint_io : private i3::noncopyable
		{
		public:
			typedef snprint_io<Ch, Tag>						this_type;
			typedef i3::stack_string_storage<Ch, mpl::size_t<256> >	string_storage_type;
			typedef sprint_object<Ch,Tag>							sprint_object_type;
			typedef sprint_item<Ch>									item_type;

			snprint_io( const sprint_object<Ch,Tag>& obj, size_t out_count) : m_obj(const_cast<sprint_object_type&>(obj) ), m_out_count(out_count), 
				m_num_item(obj.calculate_num_item_by_ncount(out_count)), m_curr_shift(0) 
			{
					
			}

			template<class T> inline typename i3::disable_if<is_scalar<T>,
			snprint_io&>::type	operator%(const T& val) 
			{ 
				distribute<this_type, const T&>(*this, val);	
				return *this;
			}

			template<class T> inline typename i3::enable_if< is_scalar<T>,
			snprint_io&>::type	operator%( T val )
			{
				distribute<this_type, T>(*this, val);
				return *this;
			}
			
			template<class T> inline
			snprint_io&	operator%(const T* val )
			{
				distribute<this_type, const T*>(*this, val);
				return *this;
			}
			
			template<class Out> typename i3::disable_if_iterator<Out,size_t>::type result(Out& res) 
			{  
				if ( m_curr_shift < get_num_arg() ) m_obj.reset_contents_unfilled_items(m_curr_shift, m_num_item);
				return m_obj.result(res, m_num_item, m_out_count); 
			}
			template<class Out> typename i3::enable_if_iterator<Out,size_t>::type  result(Out res) 
			{ 
				if ( m_curr_shift < get_num_arg() ) m_obj.reset_contents_unfilled_items(m_curr_shift, m_num_item);
				return m_obj.result(res, m_num_item, m_out_count); 
			}
			
			string_storage_type&		get_temp_storage()  { return m_temp_storage; }
			sprint_object<Ch,Tag>&	get_sprint_object()  { return m_obj; }
			std::size_t				get_curr_num_item() const { return m_num_item; }		// 이 쪽 구현이 다름..
			int						get_num_arg() const { return m_obj.get_num_arg(); }
			item_type*				get_item(std::size_t idx) const { return m_obj.get_item(idx); }

		private:
			sprint_object<Ch,Tag>&	m_obj;			// 반드시 참조여야한다.
			string_storage_type		m_temp_storage;	// 변환될만한것들은 여기서 보관하고
			int						m_curr_shift;						// %연산자마다 증가..
			size_t					m_out_count;			// 변수위치 중요..
			size_t					m_num_item;

			template<class SprintIO, class T> friend
			void	distribute(SprintIO& self, T x);
		};
		
		
		
	}
}
