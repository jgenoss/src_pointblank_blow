#pragma once

#include "../../../itl/range/algorithm/copy.h"
#include "../../../itl/utility/copy_or_swap.h"
#include "../generic_string_ncopy.h"

namespace i3
{
	namespace detail_sprintf
	{

#pragma warning(push)	
#pragma warning(disable : 4018)
		//
		// Out타입의 경우 내부값이 같은 문자원형을 유지해야한다..
		//
		struct get_result_mutable_iterator
		{
			template<class Ch, class Tag, class Out>
			static size_t	call(const sprint_object<Ch, Tag>& obj, Out& res)
			{
				std::size_t num_res = obj.m_prefix.size();
				
				const size_t num_item = obj.m_num_item;
				for (size_t i = 0 ; i < num_item ; ++i)
				{
					const sprint_item<Ch>*	itm = obj.m_items[i];
					num_res += itm->m_conv_range1.size();
					num_res += itm->m_conv_range2.size();
					num_res += itm->m_conv_range3.size();
					num_res += itm->m_src_range.size();
				}
				
				res.resize(num_res);						// 이 함수가 된다고 우선 가정하고 나중에 수정할것..

				typedef typename i3::range_iterator<Out>::type	out_iterator;
				out_iterator	tmp_it = i3::begin(res);

				tmp_it = i3::range::copy(obj.m_prefix, tmp_it);

				for (size_t i = 0 ; i < num_item ; ++i)
				{
					const sprint_item<Ch>*	itm = obj.m_items[i];

					if (!itm->m_conv_range1.empty())
						tmp_it = i3::range::copy(itm->m_conv_range1, tmp_it);
					if (!itm->m_conv_range2.empty())
						tmp_it = i3::range::copy(itm->m_conv_range2, tmp_it);
					if (!itm->m_conv_range3.empty())
						tmp_it = i3::range::copy(itm->m_conv_range3, tmp_it);
					if (!itm->m_src_range.empty())
						tmp_it = i3::range::copy(itm->m_src_range, tmp_it);
				}

				return num_res;
			}

			template<class Ch, class Tag, class Out>
			static size_t	call(const sprint_object<Ch,Tag>& obj, Out& res, const std::size_t num_item, std::size_t ncount)
			{
				std::size_t ncount_tmp = ncount;
				std::size_t num_res;
				size_t stop_idx = 0;

				if ( obj.m_prefix.size() < ncount_tmp )
				{
					num_res = obj.m_prefix.size();		ncount_tmp = ncount - num_res;
					
					for ( ; stop_idx < num_item ; ++stop_idx)
					{
						const sprint_item<Ch>*	itm = obj.m_items[stop_idx];

						if ( itm->m_conv_range1.size() < ncount_tmp )
						{
							num_res += itm->m_conv_range1.size();		ncount_tmp = ncount - num_res;
						}
						else
						{
							num_res += ncount_tmp;		break;
						}

						if ( itm->m_conv_range2.size() < ncount_tmp )
						{
							num_res += itm->m_conv_range2.size();		ncount_tmp = ncount - num_res;
						}
						else
						{
							num_res += ncount_tmp;		break;
						}	

						if ( itm->m_conv_range3.size() < ncount_tmp )
						{
							num_res += itm->m_conv_range3.size();		ncount_tmp = ncount - num_res;
						}
						else
						{
							num_res += ncount_tmp;		break;
						}	
						
						if ( itm->m_src_range.size() < ncount_tmp )
						{
							num_res += itm->m_src_range.size();		ncount_tmp = ncount - num_res;
						}
						else
						{
							num_res += ncount_tmp;		break;
						}
					}

					res.resize(num_res);		
					
					typedef typename i3::range_iterator<Out>::type	out_iterator;
					out_iterator	tmp_it = i3::begin(res);
					tmp_it = i3::range::copy(obj.m_prefix, tmp_it);

					for (size_t i = 0 ; i < stop_idx ; ++i )
					{
						const sprint_item<Ch>*	itm = obj.m_items[i];

						if (!itm->m_conv_range1.empty())
							tmp_it = i3::range::copy(itm->m_conv_range1, tmp_it);
						if (!itm->m_conv_range2.empty())
							tmp_it = i3::range::copy(itm->m_conv_range2, tmp_it);
						if (!itm->m_conv_range3.empty())
							tmp_it = i3::range::copy(itm->m_conv_range3, tmp_it);
						if (!itm->m_src_range.empty())
							tmp_it = i3::range::copy(itm->m_src_range, tmp_it);
					}

					if ( stop_idx < num_item )
					{
						const sprint_item<Ch>*	itm = obj.m_items[stop_idx];
						ncount_tmp = num_res - i3::distance( i3::begin(res), tmp_it);
						
						if (!itm->m_conv_range1.empty())
						{
							if ( itm->m_conv_range1.size() < ncount_tmp )
							{
								tmp_it = i3::range::copy(itm->m_conv_range1, tmp_it);	ncount_tmp -= itm->m_conv_range1.size();
							}
							else
							{
								i3::copy(itm->m_conv_range1.begin(), itm->m_conv_range1.begin() + ncount_tmp, tmp_it);	return num_res;
							}
						}

						if (!itm->m_conv_range2.empty())
						{
							if ( itm->m_conv_range2.size() < ncount_tmp )
							{
								tmp_it = i3::range::copy(itm->m_conv_range2, tmp_it);	ncount_tmp -= itm->m_conv_range2.size();
							}
							else
							{
								i3::copy(itm->m_conv_range2.begin(), itm->m_conv_range2.begin() + ncount_tmp, tmp_it);	return num_res;
							}
						}
						
						if (!itm->m_conv_range3.empty())
						{
							if ( itm->m_conv_range3.size() < ncount_tmp )
							{
								tmp_it = i3::range::copy(itm->m_conv_range3, tmp_it);	ncount_tmp -= itm->m_conv_range3.size();
							}
							else
							{
								i3::copy(itm->m_conv_range3.begin(), itm->m_conv_range3.begin() + ncount_tmp, tmp_it);	return num_res;
							}
						}

						if (!itm->m_src_range.empty())
						{
							if ( itm->m_src_range.size() < ncount_tmp )
							{
								tmp_it = i3::range::copy(itm->m_src_range, tmp_it);		// ncount_tmp -= itm->m_src_range.size();
							}
							else
							{
								i3::copy(itm->m_src_range.begin(), itm->m_src_range.begin() + ncount_tmp, tmp_it);	return num_res;
							}
						}
					
					}
				}
				else
				{
					num_res = ncount_tmp;
					res.resize(num_res);		
					i3::copy( obj.m_prefix.begin(), obj.m_prefix.begin() + num_res, i3::begin(res) );
				}
					
				return num_res;
			}

		};
		
		struct get_result_const_iterator
		{
			template<class Ch, class Tag, class Out>
			static size_t	call(const sprint_object<Ch,Tag>& obj, Out& res)
			{
			//	i3_prof_func();
				// 총 사이즈를 구하는 것이 가능하므로 사이즈부터 구하고 메모리 확보한뒤에 한꺼번에 카피하는 방식으로 처리할것..
				std::size_t num_res = obj.m_prefix.size();
				const size_t num_item = obj.m_num_item;
				for (size_t i = 0 ; i < num_item ; ++i)
				{
					const sprint_item<Ch>*	itm = obj.m_items[i];
					num_res += itm->m_conv_range1.size();
					num_res += itm->m_conv_range2.size();
					num_res += itm->m_conv_range3.size();
					num_res += itm->m_src_range.size();
				}

				// iterator쓰기 접근이 안되므로, 이 경우엔 다른 컨테이너를 동원한다...(여기서는 stack_basic_string을 쓴다...)
				typedef i3::stack_basic_string<mpl::size_t<512>, Ch>	tmp_container_type;
				tmp_container_type	tmp( num_res, Ch() );
				
				typedef typename i3::range_iterator<tmp_container_type>::type	tmp_iterator;
				tmp_iterator	tmp_it = i3::begin(tmp);

				tmp_it = i3::range::copy(obj.m_prefix, tmp_it);

				for (size_t i = 0 ; i < num_item ; ++i)
				{
					const sprint_item<Ch>*	itm = obj.m_items[i];

					if (!itm->m_conv_range1.empty())
						tmp_it = i3::range::copy(itm->m_conv_range1, tmp_it);
					if (!itm->m_conv_range2.empty())
						tmp_it = i3::range::copy(itm->m_conv_range2, tmp_it);
					if (!itm->m_conv_range3.empty())
						tmp_it = i3::range::copy(itm->m_conv_range3, tmp_it);
					if (!itm->m_src_range.empty())
						tmp_it = i3::range::copy(itm->m_src_range, tmp_it);
				}

				Out	tmp2(tmp.begin(), tmp.end());
				i3::copy_or_swap(res, tmp2);
				return num_res;
			}

			template<class Ch, class Tag, class Out>
			static size_t	call(const sprint_object<Ch, Tag>& obj, Out& res, const std::size_t num_item, std::size_t ncount)
			{
				std::size_t ncount_tmp = ncount;
				std::size_t num_res;
				size_t stop_idx = 0;

				if ( obj.m_prefix.size() < ncount_tmp )
				{
					num_res = obj.m_prefix.size();		ncount_tmp = ncount - num_res;
					
					for ( ; stop_idx < num_item ; ++stop_idx)
					{
						const sprint_item<Ch>*	itm = obj.m_items[stop_idx];

						if ( itm->m_conv_range1.size() < ncount_tmp )
						{
							num_res += itm->m_conv_range1.size();		ncount_tmp = ncount - num_res;
						}
						else
						{
							num_res += ncount_tmp;		break;
						}

						if ( itm->m_conv_range2.size() < ncount_tmp )
						{
							num_res += itm->m_conv_range2.size();		ncount_tmp = ncount - num_res;
						}
						else
						{
							num_res += ncount_tmp;		break;
						}	

						if ( itm->m_conv_range3.size() < ncount_tmp )
						{
							num_res += itm->m_conv_range3.size();		ncount_tmp = ncount - num_res;
						}
						else
						{
							num_res += ncount_tmp;		break;
						}	
						
						if ( itm->m_src_range.size() < ncount_tmp )
						{
							num_res += itm->m_src_range.size();		ncount_tmp = ncount - num_res;
						}
						else
						{
							num_res += ncount_tmp;		break;
						}
					}

					// iterator쓰기 접근이 안되므로, 이 경우엔 다른 컨테이너를 동원한다...(여기서는 stack_basic_string을 쓴다...)
					typedef i3::stack_basic_string<mpl::size_t<512>, Ch>	tmp_container_type;
					tmp_container_type	tmp( num_res, Ch() );
					
					typedef typename i3::range_iterator<tmp_container_type>::type	out_iterator;
					out_iterator	tmp_it = i3::begin(tmp);
					tmp_it = i3::range::copy(obj.m_prefix, tmp_it);

					for (size_t i = 0 ; i < stop_idx ; ++i )
					{
						const sprint_item<Ch>*	itm = obj.m_items[i];

						if (!itm->m_conv_range1.empty())
							tmp_it = i3::range::copy(itm->m_conv_range1, tmp_it);
						if (!itm->m_conv_range2.empty())
							tmp_it = i3::range::copy(itm->m_conv_range2, tmp_it);
						if (!itm->m_conv_range3.empty())
							tmp_it = i3::range::copy(itm->m_conv_range3, tmp_it);
						if (!itm->m_src_range.empty())
							tmp_it = i3::range::copy(itm->m_src_range, tmp_it);
					}

					if ( stop_idx < num_item )
					{
						const sprint_item<Ch>*	itm = obj.m_items[stop_idx];
						ncount_tmp = num_res - i3::distance( i3::begin(tmp), tmp_it);
						
						if (!itm->m_conv_range1.empty())
						{
							if ( itm->m_conv_range1.size() < ncount_tmp )
							{
								tmp_it = i3::range::copy(itm->m_conv_range1, tmp_it);	ncount_tmp -= itm->m_conv_range1.size();
							}
							else
							{
								i3::copy(itm->m_conv_range1.begin(), itm->m_conv_range1.begin() + ncount_tmp, tmp_it);	
								Out	tmp2(tmp.begin(), tmp.end());
								i3::copy_or_swap(res, tmp2);
								return num_res;
							}
						}

						if (!itm->m_conv_range2.empty())
						{
							if ( itm->m_conv_range2.size() < ncount_tmp )
							{
								tmp_it = i3::range::copy(itm->m_conv_range2, tmp_it);	ncount_tmp -= itm->m_conv_range2.size();
							}
							else
							{
								i3::copy(itm->m_conv_range2.begin(), itm->m_conv_range2.begin() + ncount_tmp, tmp_it);	
								Out	tmp2(tmp.begin(), tmp.end());
								i3::copy_or_swap(res, tmp2);
								return num_res;
							}
						}
						
						if (!itm->m_conv_range3.empty())
						{
							if ( itm->m_conv_range3.size() < ncount_tmp )
							{
								tmp_it = i3::range::copy(itm->m_conv_range3, tmp_it);	ncount_tmp -= itm->m_conv_range3.size();
							}
							else
							{
								i3::copy(itm->m_conv_range3.begin(), itm->m_conv_range3.begin() + ncount_tmp, tmp_it);	
								Out	tmp2(tmp.begin(), tmp.end());
								i3::copy_or_swap(res, tmp2);				
								return num_res;
							}
						}

						if (!itm->m_src_range.empty())
						{
							if ( itm->m_src_range.size() < ncount_tmp )
							{
								tmp_it = i3::range::copy(itm->m_src_range, tmp_it);		// ncount_tmp -= itm->m_src_range.size();
							}
							else
							{
								i3::copy(itm->m_src_range.begin(), itm->m_src_range.begin() + ncount_tmp, tmp_it);	
								Out	tmp2(tmp.begin(), tmp.end());
								i3::copy_or_swap(res, tmp2);	
								return num_res;
							}
						}
					
					}

					Out	tmp2(tmp.begin(), tmp.end());
					i3::copy_or_swap(res, tmp2);
				}
				else
				{
					num_res = ncount_tmp;
					Out	tmp2(obj.m_prefix.begin(), obj.m_prefix.begin() + num_res);
					i3::copy_or_swap(res, tmp2);
				}

				return num_res;
			}

		};


		template<class Ch, class Tag> 
		template<class Out> inline typename i3::disable_if_iterator<Out, size_t>::type
		sprint_object<Ch, Tag>::result(Out& res)
		{
			typedef typename i3::range_value<Out>::type result_char_type;
			compile_assert_pred( (is_same<Ch, result_char_type>) );

			typedef typename mpl::if_< mpl::or_< i3::is_template_same<i3::basic_string<mpl_,mpl_,mpl_>, Out>, 
												 i3::is_template_same<i3::stack_basic_string<mpl_,mpl_,mpl_,mpl_>, Out> >,
												 get_result_mutable_iterator, get_result_const_iterator>::type get_result_struct_type;
			
			return get_result_struct_type::call(*this, res);
		}
		
		template<class Ch, class Tag> 
		template<class Out> inline typename i3::disable_if_iterator<Out, size_t>::type
		sprint_object<Ch, Tag>::result(Out& res, const std::size_t num_item, std::size_t ncount)
		{
			typedef typename i3::range_value<Out>::type result_char_type;
			compile_assert_pred( (is_same<Ch, result_char_type>) );

			typedef typename mpl::if_< mpl::or_< i3::is_template_same<i3::basic_string<mpl_,mpl_,mpl_>, Out>, 
												 i3::is_template_same<i3::stack_basic_string<mpl_,mpl_,mpl_,mpl_>, Out> >,
												 get_result_mutable_iterator, get_result_const_iterator>::type get_result_struct_type;
			return get_result_struct_type::call(*this, res, num_item, ncount);
		}		

		//
		// 리터럴의 경우, 제네릭 함수도 좋지만, 되도록 좀더 저수준에 가깝게 작성해본다..
		// 

		template<class Ch, class Tag> 
		template<class Out> typename i3::enable_if_iterator<Out, size_t>::type
		sprint_object<Ch, Tag>::result(Out it)
		{
		//	i3_prof_func();
			typedef typename i3::iterator_value<Out>::type result_char_type;
			compile_assert_pred( (is_same<Ch, result_char_type>) );
		
			Out res = it;
			res = i3::range::copy(m_prefix, res);

			const size_t num_item = m_num_item;		//m_items.size();

			for (size_t i = 0 ; i < num_item ; ++i)
			{
				item_type*	itm = m_items[i];

				if (!itm->m_conv_range1.empty())
					res = i3::range::copy(itm->m_conv_range1, res);
				if (!itm->m_conv_range2.empty())
					res = i3::range::copy(itm->m_conv_range2, res);
				if (!itm->m_conv_range3.empty())
					res = i3::range::copy(itm->m_conv_range3, res);
				if (!itm->m_src_range.empty())
					res = i3::range::copy(itm->m_src_range, res);
			}

			*res = typename i3::iterator_value<Out>::type();
			return size_t( i3::distance( it, res) );
		}

		
		//
		// 널값 처리 문제는 generic_string_ncopy에서 어느정도 처리된다..
		// 
		
		template<class Ch, class Tag> 
		template<class Out> typename i3::enable_if_iterator<Out, size_t>::type
		sprint_object<Ch, Tag>::result(Out it, const std::size_t num_item, std::size_t ncount)
		{
		//	i3_prof_func();
			typedef typename i3::iterator_value<Out>::type result_char_type;
			compile_assert_pred( (is_same<Ch, result_char_type>) );
		
			typedef typename str_ptr_range::iterator		src_iterator;
			Out				res = it;
			std::size_t		ncount_old = ncount;
			std::size_t		len = i3::generic_string_ncopy(res, m_prefix, ncount);
			ncount -= len;
			i3::advance(res, len); 

			if (ncount > 0 )
			{
				for (size_t i = 0 ; i < num_item ; ++i)
				{
					item_type*	itm = m_items[i];
				
					if (!itm->m_conv_range1.empty())
					{
						len = i3::generic_string_ncopy(res, itm->m_conv_range1, ncount);
						ncount -= len;			i3::advance(res, len); 
						if ( ncount == 0 ) break;
					}

					if (!itm->m_conv_range2.empty())
					{
						len = i3::generic_string_ncopy(res, itm->m_conv_range2, ncount);
						ncount -= len;			i3::advance(res, len); 
						if ( ncount == 0 ) break;
					}
					
					if (!itm->m_conv_range3.empty())
					{
						len = i3::generic_string_ncopy(res, itm->m_conv_range3, ncount);
						ncount -= len;			i3::advance(res, len); 
						if ( ncount == 0 ) break;
					}

					if (!itm->m_src_range.empty())
					{
						len = i3::generic_string_ncopy(res, itm->m_src_range, ncount);
						ncount -= len;			i3::advance(res, len); 
						if ( ncount == 0 ) break;
					}
				}
			}
			
			std::size_t curr_size =	std::size_t(i3::distance( it, res));
			if ( curr_size >= ncount_old )
			{
				*res = result_char_type();
				--curr_size;
			}
			return curr_size;
		}
		//

#pragma warning(pop)	

	}

}