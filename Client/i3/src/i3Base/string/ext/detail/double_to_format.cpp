#include "i3CommonType.h"

namespace i3 
{
	namespace detail_sprintf
	{
		//
		//  fformat은 소수점 처리에만 관심이 있는 계산임..
		//

		I3_EXPORT_BASE size_t	double_to_fformat(double val, int precision, bool force_point, char* out, int* sign)
		{
			char* out_last = out;
			char fcvt_buff[_CVTBUFSIZE];	
			char* in_buff  = fcvt_buff;
			int dec_pos;

			::_fcvt_s(fcvt_buff, _CVTBUFSIZE, val, precision, &dec_pos, sign);		// 	
			
			const char* in_buff_end = in_buff + strlen(fcvt_buff);

			if ( dec_pos > 0 )
			{
				::memcpy(out_last, in_buff, dec_pos);		// 소수점 위치까지는 카피..
				out_last += dec_pos;
				in_buff  += dec_pos;
				
				if ( in_buff < in_buff_end )
				{
					*out_last++ = '.';		// 소수점 표시..
					ptrdiff_t num_copy =  in_buff_end - in_buff; 
					::memcpy(out_last, in_buff, num_copy);	// 소수점 이하 카피..(buff처리는 종료)
					out_last += num_copy;
				}
				else if (force_point)
				{
					*out_last++ = '.';		// 소수점 표시..
				}
			}
			else
			{
				*out_last++ = '0';		// 1보다 작은 경우 0을 우선찍음..
				
				if (in_buff < in_buff_end)
				{
					*out_last++ = '.';		// 소수점 표시..
					::memset(out_last, '0', -dec_pos );
					out_last += -dec_pos;
					ptrdiff_t num_copy =  in_buff_end - in_buff; 
					::memcpy(out_last, in_buff, num_copy);
					out_last += num_copy;
				}
				else if (precision > 0)			// fcvt_len == 0인 경우, 찍어야할 유효숫자가 더 없음을 의미..
				{								// 자릿수를 강제로 찍으려면 precision_num수를 그대로 적용..	
					*out_last++ = '.';		// 소수점 표시..
					::memset(out_last, '0', precision);
					out_last += precision;
				}
				else if (force_point)
				{
					*out_last++ = '.';		// 소수점 표시..
				}
			}

			*out_last = '\0';
			return size_t(out_last - out);
		}

		//
		// e-format은 _ecvt_s를 쓴다.. ecvt를 쓸때 결과값의 소수점 이하가 유효자리이므로 +1을 처리해서 유효처리한다..
		//
		I3_EXPORT_BASE size_t	double_to_eformat(double val, int precision, bool force_point, bool cap, char* out, int* sign)
		{
			char* out_last = out;

			char fcvt_buff[_CVTBUFSIZE];	
			char* in_buff  = fcvt_buff;
			int dec_pos;

			::_ecvt_s(fcvt_buff, _CVTBUFSIZE, val, precision + 1, &dec_pos, sign);		
			 	
			// dec의 위치가 중요하다. 어쨋거나 강제로 첫숫자를 제외한 나머지를 소수점 이하로 처리한다..

			const char* in_buff_end = in_buff + strlen(fcvt_buff);
			
			int e = dec_pos-1;
			
			if (*in_buff == '0')		// 처음부터 0이면 그냥 e는 -1이 아니고 0이기 때문에, e값을 바꾼다..
				e = 0;

			*out_last++ = *in_buff++;	// 첫숫자는 강제로 찍는다..
			
			if ( in_buff < in_buff_end)
			{
				*out_last++ = '.';		// 소수점 표시..
				ptrdiff_t num_copy =  in_buff_end - in_buff; 
				::memcpy(out_last, in_buff, num_copy );		// 소수점 이하 카피..(buff처리는 종료)
				out_last += num_copy;
			}
			else if ( force_point )
			{
				*out_last++ = '.';		// 소수점 표시..
			}

			// e값찍기..
			*out_last++ = ( cap ) ? 'E' : 'e';

			// e 부호 찍기..
			if ( e < 0 )
			{
				*out_last++ = '-';	e = -e;
			}
			else
			{
				*out_last++ = '+';
			}
			
			// e 3자리를 찍어야한다..(가수의 절대 갯수는 3자리이다...double max수치상 넘길 일 없을것이다.)
			int c[3];
			c[2] = (e % 10) + int('0');	e/=10;	c[1] = e % 10 + int('0');	e/=10;	c[0] = e % 10 + int('0');	
			
			*out_last++ = char(c[0]);	*out_last++ = char(c[1]);	*out_last++ = char(c[2]);
			*out_last = '\0';

			return size_t(out_last - out);
		}


		//
		//   gformat은 _ecvt_s를 쓴다... (유효자릿수 위주로 계산된다..)
		//   g에서 e표현으로 넘어가는 조건은 e값이 -4보다 작거나, 혹은 유효자리인수보다 같거나 큰 경우이고, 그 외에는
		//   일반 소수점 자리로 표현한다..
		//   
		// del_zero옵션은 소숫점 이하 끝부분이 0이면 모두 제거하는 경우이다..

		I3_EXPORT_BASE size_t	double_to_gformat(double val, int precision, bool force_point, bool cap, bool del_zero, char* out, int* sign)
		{
			char* out_last = out;

			char fcvt_buff[_CVTBUFSIZE];	
			char* in_buff  = fcvt_buff;
			int dec_pos;
			
			if (precision == 0)
				precision = 1;		// 1로 강제변경한다..	// sprintf내부 특성이다..

			::_ecvt_s(fcvt_buff, _CVTBUFSIZE, val, precision, &dec_pos, sign);		
			
			char* in_buff_end = in_buff + strlen(fcvt_buff);
			int e = dec_pos-1;
			
			if ( e < -4 || e >= precision )
			{
				// e 포맷으로 찍는다..
				*out_last++ = *in_buff++;	// 첫숫자는 강제로 찍는다..(존재할것이다..)
				
				if (del_zero)		// 끝 0을 제거하는 루틴옵션..
				{
					for ( ; in_buff < in_buff_end ; --in_buff_end )
					{
						if (  *(in_buff_end-1) != '0' )
							break;
					}
				}

				if ( in_buff < in_buff_end)
				{
					*out_last++ = '.';		// 소수점 표시..
					ptrdiff_t num_copy =  in_buff_end - in_buff; 
					::memcpy(out_last, in_buff, num_copy );		// 소수점 이하 카피..(buff처리는 종료)
					out_last += num_copy;
				}
				else if ( force_point )
				{
					*out_last++ = '.';		// 소수점 표시..
				}

				// e값찍기..
				*out_last++ = ( cap ) ? 'E' : 'e';

				// e 부호 찍기..
				if ( e < 0 )
				{
					*out_last++ = '-';	e = -e;
				}
				else
				{
					*out_last++ = '+';
				}
				
				// e 3자리를 찍어야한다..(가수의 절대 갯수는 3자리이다...double max수치상 넘길 일 없을것이다.)
				int c[3];
				c[2] = (e % 10) + int('0');	e/=10;	c[1] = e % 10 + int('0');	e/=10;	c[0] = e % 10 + int('0');	
				
				*out_last++ = char(c[0]);	*out_last++ = char(c[1]);	*out_last++ = char(c[2]);
			}
			else
			{
				if ( dec_pos > 0 )
				{
					::memcpy(out_last, in_buff, dec_pos);		// 소수점 위치까지는 카피..
					out_last += dec_pos;
					in_buff  += dec_pos;
					
					if (del_zero)		// 끝 0을 제거하는 루틴옵션..
					{
						for ( ; in_buff < in_buff_end ; --in_buff_end )
						{
							if (  *(in_buff_end-1) != '0' )
								break;
						}
					}

					if ( in_buff < in_buff_end )
					{
						*out_last++ = '.';		// 소수점 표시..
						ptrdiff_t num_copy =  in_buff_end - in_buff; 
						::memcpy(out_last, in_buff, num_copy);	// 소수점 이하 카피..(buff처리는 종료)
						out_last += num_copy;
					}
					else if (force_point)
					{
						*out_last++ = '.';		// 소수점 표시..
					}
				}
				else
				{
					*out_last++ = '0';		// 1보다 작은 경우 0을 우선찍음..
					
					if (del_zero)		// 끝 0을 제거하는 루틴옵션..
					{
						for ( ; in_buff < in_buff_end ; --in_buff_end )
						{
							if (  *(in_buff_end-1) != '0' )
								break;
						}
					}

					if (in_buff < in_buff_end)
					{
						*out_last++ = '.';		// 소수점 표시..
						::memset(out_last, '0', -dec_pos );
						out_last += -dec_pos;
						ptrdiff_t num_copy =  in_buff_end - in_buff; 
						::memcpy(out_last, in_buff, num_copy);
						out_last += num_copy;
					}
					else if (force_point)
					{
						*out_last++ = '.';		// 소수점 표시..
					}
				}
			}

			*out_last = '\0';		
			return size_t(out_last - out);
		}

		
		
		
	}
}
