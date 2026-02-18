#pragma once

#include "sprint_io.h"

namespace i3
{
	namespace detail_sprintf
	{
		template<class Formatter>
		class formatter_io 
		{
		public:
			typedef typename Formatter::value_type				value_type;
			typedef value_type									Ch;
			typedef typename Formatter::sprint_object_tag		sprint_object_tag;
			typedef sprint_object_tag							Tag;
			formatter_io( const Formatter& fmter) : m_obj(fmter.m_obj)											// 이 부분 변경해야한다..이렇게 처리하면..
			{																								// sprint_object가 복사가 된다..
			}																								//

			template<class T> inline typename i3::disable_if<is_scalar<T>,
			sprint_io<Ch, Tag>&>::type	operator%(const T& val) 
			{ 
				return m_obj % val;
			}

			template<class T> inline typename i3::enable_if< is_scalar<T>,
			sprint_io<Ch, Tag>&>::type	operator%( T val )
			{
				return m_obj % val;
			}
			
			template<class T> inline
			sprint_io<Ch, Tag>&	operator%(const T* val )
			{
				return m_obj % val;
			}

			template<class Out> typename i3::disable_if_iterator<Out,size_t>::type result(Out& res) 
			{  
				return m_obj.result(res); 
			}
			template<class Out> typename i3::enable_if_iterator<Out,size_t>::type  result(Out res) 
			{ 
				return m_obj.result(res); 
			}

		private:
			sprint_io<Ch, Tag>		m_obj;			
		};
	}
}