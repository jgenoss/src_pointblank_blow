#pragma once

#include "sprint_item.h"

#include "../../../itl/range/detail/enable_if_iterator.h"

namespace i3
{
	namespace detail_sprintf
	{
		//
		// 되도록이면 iterator_range에 최대한 의존하는게 좋다..
		// 객체를 전역 저장하는게 좋지 않아, sprintf 동일 동작 처리시에는 스택객체로 처리한다. .
		// 생성자에서는 item_class를 쪼개는 parse처리..
		// 모든 printf류는 positional과 non_positional의 mixing을 허용하지 않음.
	
		struct sprint_local_tag; 
		struct sprint_global_tag; 

		template<class Ch, class Tag>
		class sprint_object;

		template<class Ch, class Tag>
		class sprint_object_holder;

		template<class Ch>
		class sprint_object_holder<Ch, sprint_local_tag>
		{
			sprint_object_holder() {}
			~sprint_object_holder()
			{
				i3::cu::for_each_delete(m_items);	
			}

			typedef sprint_item<Ch>						item_type;			
								
			i3::vector<item_type*>						m_items;

			friend struct get_result_mutable_iterator;
			friend struct get_result_const_iterator;
			friend class sprint_object<Ch, sprint_local_tag>;
		};


		template<class Ch>
		class sprint_object_holder<Ch, sprint_global_tag> : private i3::noncopyable
		{
			sprint_object_holder() :  m_items( get_item_list<Ch>() ) {}
			~sprint_object_holder() {}
			typedef sprint_item<Ch>						item_type;	
		
			i3::vector<item_type*>&						m_items;
		
			friend struct get_result_mutable_iterator;
			friend struct get_result_const_iterator;
			friend class sprint_object<Ch, sprint_global_tag>;
		};


		template<class Ch, class Tag = sprint_global_tag>
		class sprint_object : public sprint_object_holder<Ch, Tag>
		{
		public:
			typedef sprint_object<Ch, Tag>							this_type;
			typedef Ch												value_type;
			typedef i3::iterator_range<const Ch*>					format_range_type;		
			typedef format_range_type								str_ptr_range;
			typedef sprint_item<Ch>									item_type;		

			sprint_object() : m_num_arg(0), m_num_item(0), m_prefix(str_ptr_range(nullptr, nullptr))
			{
			}

			//
			// 생성자를 템플릿 생성자로 바꾸고, as_literal구현을 바깥에서 안으로 옮길것..
			//
			sprint_object(const	format_range_type& fmt) : m_num_arg(0), m_num_item(0)
			{
//				i3_prof_func();
				parse(fmt);			// 나머지 멤버변수는 함수로 셋팅..
			}
		
			void	reset_format(const	format_range_type& fmt) 
			{
				m_num_arg = 0; m_num_item = 0;	
				parse(fmt);
			}
			
			std::size_t		get_num_item() const { return m_num_item; }
			int				get_num_arg() const  { return m_num_arg; }
			item_type*		get_item(std::size_t idx) const { return m_items[idx]; }
			
			void			reset_contents_unfilled_items(int start_arg, std::size_t curr_num_item);

			~sprint_object()
			{
			}
		
			template<class Out> typename i3::disable_if_iterator<Out,size_t>::type result(Out& res);
			template<class Out> typename i3::enable_if_iterator<Out,size_t>::type  result(Out res);
			template<class Out> typename i3::disable_if_iterator<Out,size_t>::type result(Out& res, const size_t num_item, size_t ncount);
			template<class Out> typename i3::enable_if_iterator<Out,size_t>::type  result(Out res, const size_t num_item, size_t ncount);

		private:
			std::size_t		calculate_num_item_by_ncount(std::size_t ncount) const;
			void			parse(const format_range_type& fmt);
			
			template<class It> friend
			bool parse_printf_style(It& f, It l, sprint_item<Ch>* itm );

			friend struct get_result_mutable_iterator;
			friend struct get_result_const_iterator;
			template<class , class > friend class snprint_io;

			std::size_t												m_num_item;
			str_ptr_range											m_prefix;			// 예외적인 포맷전 첫문자열..
			int														m_num_arg;			// 최대예상자릿수..(넘기면 무시하게)
		};
			
		template<class Ch, class Tag>
		std::size_t		sprint_object<Ch, Tag>::calculate_num_item_by_ncount(std::size_t ncount) const
		{
			std::size_t num_res = m_prefix.size();
			if ( ncount <= num_res) 
				return 0;
			const size_t num_item = m_num_item;
			for (size_t i = 0 ; i < num_item ; ++i)
			{
				const sprint_item<Ch>*	itm = m_items[i];
				num_res += itm->m_src_range.size();
				if (ncount <= num_res)
					return i + 1;
			}

			return num_item;
		}
		
		// 인수가 채워지지 않았다면 기존 내용물은 리셋해야한다...(정상적이라면 이 함수에 일처리가 없어야 함..)
		template<class Ch, class Tag>		
		void			sprint_object<Ch, Tag>::reset_contents_unfilled_items(int start_arg, std::size_t curr_num_item)
		{
			typedef sprint_item<Ch>::str_ptr_range		str_ptr_range;

			for (int i = start_arg ; i < m_num_arg; ++i )
			{
				for (size_t j = 0 ; j < curr_num_item ; ++j )
				{
					sprint_item<Ch>*	itm = m_items[j];

					if ( itm->m_arg_idx == i )		// 일일이 찾을수밖에 없음..(단점은 아닌것으로 보인다)
					{
						itm->m_conv_range1 = str_ptr_range(nullptr, nullptr);
						itm->m_conv_range2 = str_ptr_range(nullptr, nullptr);
						itm->m_conv_range3 = str_ptr_range(nullptr, nullptr);
					}
				}
			}
		}

		template<class SprintIO, class T>
		void	distribute(SprintIO& self, T x)		// T에 참조가 박히지 않은 상태이므로, 매개변수지정이 필요함.
		{
//			i3_prof_func();

			if (self.m_curr_shift >= self.get_num_arg() ) return;		// 지정인수를 넘기면 다 무시..
			
			typedef typename SprintIO::item_type				item_type;
			typedef typename SprintIO::string_storage_type		string_storage_type;
			
			const std::size_t curr_num_item = self.get_curr_num_item();

			for (size_t i = 0 ; i < curr_num_item ; ++i)
			{
				item_type* itm = self.get_item(i);

				if ( itm->m_arg_idx == self.m_curr_shift )		// 일일이 찾을수밖에 없음..(단점은 아닌것으로 보인다)
				{
					put<T, item_type, string_storage_type>(x, itm, self.get_temp_storage() ); 
				}
			}
			++self.m_curr_shift;
		}

	}

}

#include "sprint_parse.h"
#include "sprint_put.h"
#include "sprint_result.h"
