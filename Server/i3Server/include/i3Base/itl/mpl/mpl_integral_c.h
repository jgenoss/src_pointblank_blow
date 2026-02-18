#pragma once


//
//  포워딩만으로 충분하다면, 포워드만 모은 헤더를 따로 둘것..
//
#include "mpl_integral_c_fwd.h"
#include "mpl_integral_c_tag.h"

namespace i3
{
	namespace mpl
	{

		template<class T, T N>
		struct integral_c 
		{
			static const	T				value = N;
			typedef			integral_c		type;
			typedef			T				value_type;
			typedef			integral_c_tag	tag;
			typedef			integral_c<T, static_cast<T>(value+1)>	next;
			typedef			integral_c<T, static_cast<T>(value-1)>  prior;
			operator		T() const		{ return this->value; }
		};

		template< bool C >
		struct integral_c<bool, C>
		{
			static const	bool				value = C;
			typedef			integral_c_tag		tag;
			typedef			integral_c			type;
			typedef			bool				value_type;
			operator		bool() const		{ return this->value; }
		};

	}

}
