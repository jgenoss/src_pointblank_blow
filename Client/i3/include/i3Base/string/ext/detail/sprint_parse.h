#pragma once

#include "sprint_flags.h"
#include "../../../itl/range/value_type.h"
#include "../../../itl/algorithm/find.h"

namespace i3
{
	namespace detail_sprintf
	{
		enum { no_posit = -1, ignored = -2, no_input = -3, };		


		template<class Ch> inline
		bool	is_digit_fast(Ch c) { return  Ch('0') <= c && c <= Ch('9') ; }
		
		template<class It>
		int		str2int(It f, It l, It& end)
		{
			int res = 0;
			typedef typename i3::iterator_value<It>::type	Ch;

			for ( ; f != l && is_digit_fast(*f) ; ++f)
			{
				res *= 10;
				res += Ch(*f) - Ch('0');
			}

			end = f;

			return res;
		}

		template<class Iter> inline
		Iter wrap_scan_notdigit(Iter beg, Iter end)
		{
			for ( ; beg != end && is_digit_fast(*beg); ++beg) {}
			return beg;
		}
		
		// 아래는 함수이름과 달리 '*'이후의 관련 포맷을 제낀다..(*에 붙는 숫자와 $를 제거)
		template<class Iter> inline
		Iter skip_asterisk(Iter f, Iter l)		
		{
			typedef typename i3::iterator_value<Iter>::type		Ch;
			++f;
			f = wrap_scan_notdigit(f, l);
			if ( f != l && *f == Ch('$') )
				++f;
			return f;
		}

		template<class FmtRange>
		size_t calc_num_item_reserve( const FmtRange& fmt, typename i3::range_value<FmtRange>::type arg_mark)
		{
		
			size_t num_item = 0;
			typedef typename i3::range_iterator<FmtRange>::type		iterator;
		
			iterator beg = fmt.begin();
			iterator end = fmt.end();
			iterator next;

			while ( (beg = i3::find(beg, end, arg_mark)) != end  )
			{
				next = beg;		++next;

				if ( next == end )	// "%"로 끝나는경우..(버그성)
				{	
					break;	//	++num_item;	break;
				}
				if ( *beg == *next )
				{
					beg = next; ++beg;	++num_item; continue;		// 수정..%%는 끊고, 무항아이템으로 처리(2014.06.13.수빈)
				}

				beg = next;
				
				beg = wrap_scan_notdigit(beg, end);
				if ( beg != end && *beg == arg_mark )		// %로 감싸진 경우만..
					++beg;
				++num_item;
			}
			return num_item;
		}
		
		// 아래 함수가 초반 파싱에서 중요함..(첫인수가 밖으로 리턴됨)
		template<class It, class Ch>
		bool parse_printf_style( It& f, It l, sprint_item<Ch>* itm )
		{
				
			itm->m_arg_idx = no_posit;
			itm->m_width   = 0;
			itm->m_precision = -1;
			itm->m_flags = 0;
			
			if ( *f == Ch('%'))			// '%%'의 경우일 것이다.
			{
				typedef typename sprint_item<Ch>::str_ptr_range	str_ptr_range;
				itm->m_arg_idx = no_input;
				itm->m_conv_range1 = str_ptr_range(nullptr, nullptr);
				itm->m_conv_range2 = str_ptr_range(nullptr, nullptr);
				itm->m_conv_range3 = str_ptr_range(nullptr, nullptr);
				++f;
				return true;
			}

			bool in_brackets = false;
			
			if ( *f == Ch('|') )
			{
				in_brackets = true;	++f;
				if (f == l)		
					return false;
			}
			if ( *f == Ch('0') )			// 0은 자릿수로 따지지 않기 때문에 우선 공백으로 처리..
				goto parse_flags;		
			
			if ( is_digit_fast(*f) )				// 그외의 모든 정수 숫자.
			{
				int val = str2int(f, l, f); 
				
				if (f == l )		// 추가정보가 없다..
					return false;
				
				if ( *f == Ch('%') )		// 단순 자리표..(나중에 올 실 인수타입에만 의존)
				{
					itm->m_arg_idx = val - 1;
					++f;
					if (in_brackets == false)		// |이 안들어간경우 올바르게 종료..
						return true;			
				}

				if ( *f == Ch('$') )				// |가 들어간경우 때문에 else가 들어가선 안된다..
				{
					itm->m_arg_idx = val - 1;
					++f;
				}
				else
				{
					itm->m_width  = val;			// %나 $가 더 붙지 않은 모든 숫자는 width이다..
					itm->m_arg_idx = no_posit;
					goto parse_precision;
				}
			}

parse_flags:				// 모든 sprint의 format flag처리해야함..
			while( f != l )
			{
				switch(*f)
				{
				case Ch('\''):	case Ch('l'):	case Ch('h'):		break;
				case Ch('-'):	itm->m_flags |= SPRINTF_FL_LEFT;	break;
				case Ch('+'):	itm->m_flags |= SPRINTF_FL_SIGN;	break;
				case Ch(' '):	itm->m_flags |= SPRINTF_FL_SIGNSP;	break;
				case Ch('0'):	itm->m_flags |= SPRINTF_FL_LEADZERO;	break;
				case Ch('#'):	itm->m_flags |= SPRINTF_FL_ALTERNATE;	break;
				default:		goto parse_width;			// 대부분의 경우 f가 증가되지 않고 다음조사로 가야한다..
				}
				++f;
			}

			if ( f == l ) return true;
parse_width:			
			if ( *f == Ch('*') )		// 별표는 제낀다.
				f = skip_asterisk(f, l);
			if ( f != l && is_digit_fast(*f) )
				itm->m_width = str2int(f, l, f);
parse_precision:
			if ( f == l) return true;
				
			if (*f == Ch('.') )			// 유효숫자표시..
			{
				itm->m_precision = 0;
				++f;
				if ( f == l) return true;
				if ( *f == Ch('*') ) 
				{
					f = skip_asterisk(f, l);
					if ( f == l ) return true;
				}
				if ( is_digit_fast(*f) )
				{
					itm->m_precision = str2int(f, l, f);
				}
			}
			
			while ( f != l && ( *f == Ch('l') || *f == Ch('L') || *f == Ch('h') ) ) 
			{ 
				++f; 
			}

			if ( f == l) return true;
			
			if ( in_brackets && *f == Ch('|') )			// 두번째 |는 포맷종료 의미..
			{
				++f;	return true;
			}
			
			// 메인 포맷..
			switch(*f)
			{
			case Ch('X'):	
				itm->m_flags |= SPRINTF_FL_INTEGRAL|SPRINTF_FL_HEX|SPRINTF_FL_UPPER|SPRINTF_FL_FORCEUNSIGNED; 
			break;
			case Ch('x'):	
				itm->m_flags |= SPRINTF_FL_INTEGRAL|SPRINTF_FL_HEX|SPRINTF_FL_LOWER|SPRINTF_FL_FORCEUNSIGNED; 
			break;
			case Ch('p'):	
				itm->m_flags |= SPRINTF_FL_INTEGRAL|SPRINTF_FL_HEX|SPRINTF_FL_UPPER|SPRINTF_FL_FORCEUNSIGNED; 
				itm->m_precision = sizeof(void*) * 2; 
			break;
			case Ch('o'):	
				itm->m_flags |= SPRINTF_FL_INTEGRAL|SPRINTF_FL_OCT|SPRINTF_FL_FORCEUNSIGNED;	 
				if (itm->m_flags & SPRINTF_FL_ALTERNATE) itm->m_flags |= SPRINTF_FL_FORCEOCTAL;	
			break;

			case Ch('E'):	
				itm->m_flags |= SPRINTF_FL_SCI|SPRINTF_FL_UPPER; 
				if (itm->m_precision < 0) itm->m_precision = 6;
				else if (itm->m_precision > _CVTBUFSIZE) itm->m_precision = _CVTBUFSIZE;
			break;

			case Ch('e'):	
				itm->m_flags |= SPRINTF_FL_SCI|SPRINTF_FL_LOWER; 
				if (itm->m_precision < 0) itm->m_precision = 6;
				else if (itm->m_precision > _CVTBUFSIZE) itm->m_precision = _CVTBUFSIZE;
			break;

			case Ch('f'):	
				itm->m_flags |= SPRINTF_FL_FIXEDFLOAT; 
				if (itm->m_precision < 0) itm->m_precision = 6;
				else if (itm->m_precision > _CVTBUFSIZE) itm->m_precision = _CVTBUFSIZE;
			break;
			case Ch('G'):	
				itm->m_flags |= SPRINTF_FL_GFLOAT|SPRINTF_FL_UPPER; 
				if (itm->m_precision < 0) itm->m_precision = 6;
				else if (itm->m_precision == 0 ) itm->m_precision = 1;
				else if (itm->m_precision > _CVTBUFSIZE) itm->m_precision = _CVTBUFSIZE;
			break;

			case Ch('g'):	
				itm->m_flags |= SPRINTF_FL_GFLOAT|SPRINTF_FL_LOWER; 
				if (itm->m_precision < 0) itm->m_precision = 6;
				else if (itm->m_precision == 0 ) itm->m_precision = 1;
				else if (itm->m_precision > _CVTBUFSIZE) itm->m_precision = _CVTBUFSIZE;
			break;

			case Ch('u'):	
				itm->m_flags |= SPRINTF_FL_INTEGRAL|SPRINTF_FL_FORCEUNSIGNED; 
			break;

			case Ch('d'):	case Ch('i'):	
				itm->m_flags |= SPRINTF_FL_INTEGRAL; 
			break;

			case Ch('C'):	
			case Ch('c'):	
				itm->m_flags |= SPRINTF_FL_SINGLECHAR;	// 정수형에서 따진다..
				itm->m_precision = 1;	
			break;

			case Ch('S'):	case Ch('s'):		// 이 포맷은 큰 의미가 없으며, 바깥 인수에 더 의존적이다..					
			break;

			case Ch('n'):	
			itm->m_arg_idx = ignored;				
			break;		//무시한다.

			default: 
			break;
			}

			++f;
			if ( f == l) return true;

			if (in_brackets &&  *f == Ch('|') )
			{
				++f;	return true;
			}

			return true;
		}
		

		template<class Ch, class Tag>
		void	sprint_object<Ch, Tag>::parse(const format_range_type& fmt)
		{
//			i3_prof_func();

//			i3_prof_start("parse #1 block");

			const size_t num_item_reserve = calc_num_item_reserve(fmt, Ch('%') );
			
			if ( num_item_reserve > m_items.size() )	
			{
				m_items.reserve(num_item_reserve);
				for (size_t i = m_items.size() ; i < num_item_reserve ; ++i)
					m_items.push_back(new sprint_item<Ch>);
			}

//			i3_prof_end();

//			i3_prof_start("parse #2 block");

			bool	ordered_args = true;
			int		max_argN = -1;	
			size_t	num_item = 0;
			
			typedef typename i3::range_iterator<format_range_type>::type		iterator;
		
			iterator start = fmt.begin();
			iterator beg = fmt.begin();
			iterator end = fmt.end();
			iterator next;		

			while ( ( beg = i3::find(beg, end, Ch('%') ) ) != end  )
			{
				str_ptr_range& src_rng = (num_item == 0) ? m_prefix : m_items[num_item - 1]->m_src_range;
			
				next = beg;		++next;
				if (next == end)
				{
				//	src_rng = str_ptr_range(start, next);	
					--end;		// 잘못된 마지막 '%'는 그냥 표시하지 않는다..
					break;
				}

				if (*beg == *next)
				{
					src_rng = str_ptr_range(start, next);	// '%%'의 경우 첫 %는 기록 되도록 함.
				}
				else
				{
					src_rng = str_ptr_range(start, beg);		// src_rng가 빠짐없이 들어가야됨..
				}
			
				start = beg;	beg = next;
				
				bool parse_ok = parse_printf_style( beg, end, m_items[num_item] ); 		// %다음부터..		
				
				if (!parse_ok) continue;			// 파싱이 제대로 안되면 아이템으로 따지지 않는다..
				
				start = beg;
				
				int argN = m_items[num_item]->m_arg_idx;

				if ( argN == ignored ) continue;
				if ( argN == no_posit) ordered_args = false;
				else if (argN > max_argN) max_argN = argN;
				++num_item;
			}
//			i3_prof_end();

			str_ptr_range& src_rng = (num_item == 0) ? m_prefix : m_items[num_item - 1]->m_src_range;

			src_rng = str_ptr_range(start, end);

			if (!ordered_args)		// 위치포맷이 아닌것이 하나라도 있는경우..
			{	
				int non_ordered_items = 0;
				for (size_t i = 0 ; i < num_item ; ++i)
				{
					if ( m_items[i]->m_arg_idx == no_posit)
					{
						m_items[i]->m_arg_idx = non_ordered_items;			// 위치포맷으로 강제 변경..
						++non_ordered_items;
					}
				}
				if ( non_ordered_items - 1 > max_argN )
					max_argN = non_ordered_items - 1;
			}
		
			m_num_item = num_item;

			m_num_arg = max_argN + 1;

		}


	}
}