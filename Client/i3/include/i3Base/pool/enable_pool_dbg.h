#pragma once

//
// 오브젝트 풀과 static_pool_type_malloc 등에서 적용가능함...
//

#include "pool_dbg_user_config.h"
#include "../itl/type_traits/integral_constant.h"

#define	i3_disable_pool_dbg(TYPE)	namespace i3 { template<> struct enable_pool_dbg<TYPE> : false_type {}; }

namespace i3
{

#if defined(_DEBUG) && defined(I3_POOL_DBG)
	struct enable_pool_dbg_default : true_type {};
#else
	struct enable_pool_dbg_default : false_type{}; 
#endif
	template<class T>
	struct enable_pool_dbg : enable_pool_dbg_default {};

}
