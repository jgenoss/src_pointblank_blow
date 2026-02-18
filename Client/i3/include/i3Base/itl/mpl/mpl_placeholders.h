#if !defined(pp_is_iterating)

#ifndef mpl_placeholders_h
#define mpl_placeholders_h

#include "mpl_arg.h"
#include "mpl_limit.h"

namespace i3
{
	namespace mpl
	{
		typedef arg<-1>		_;

		namespace placeholders
		{
			using ::i3::mpl::_;
		}
	}
}

// 자리표 네임스페이스가 가독성에 조금 무리를 주는것 같아 전역 typedef을 좀더 둔다.. mpl_ , mpl_1, mpl_2, mpl_3 

typedef ::i3::mpl::_		mpl_;


#define pp_iteration_params_1  (3, (1, pp_inc(mpl_arity_limit), "mpl_placeholders.h"))
#include pp_iterate()

#endif

#else

#define i_	pp_frame_iteration(1)

namespace i3
{
	namespace mpl
	{
		typedef arg<i_> pp_cat(_,i_);
		
		namespace placeholders
		{
			using pp_cat(::i3::mpl::_,i_);
		}
	}
}


typedef pp_cat(::i3::mpl::_,i_)	pp_cat(mpl_, i_);


#undef i_

#endif	// pp_is_iterating