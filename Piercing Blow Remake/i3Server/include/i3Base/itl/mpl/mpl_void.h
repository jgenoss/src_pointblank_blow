#pragma once

#include "mpl_void_fwd.h"
#include "mpl_bool.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		struct void_ {	typedef void_		type;	};			// 실제로 void타입을 갖고 있지 않다..
				
		template< class T >	struct is_void_	: false_{};
		template<> struct is_void_<void_> : true_ {};
		template< class T >	struct is_not_void_	: true_	{};
		template<> struct is_not_void_<void_> : false_{};
	}
}


#define na_spec (1, is_void_)
#include "aux_/mpl_na_spec.h"


#define na_spec (1, is_not_void_)
#include "aux_/mpl_na_spec.h"
