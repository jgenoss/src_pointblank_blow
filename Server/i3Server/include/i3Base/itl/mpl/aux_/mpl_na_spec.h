
// no include guard and pragma once
#ifndef mpl_na_spec_h
#define mpl_na_spec_h
#include "../mpl_lambda_fwd.h"
#include "../mpl_limit.h"
#include "../mpl_bool.h"
#include "../mpl_na.h"
#include "../../preprocessor/ext/pp_enum_datas.h"
#include "../../preprocessor/ext/pp_enum_params_1_with_a_partial_default.h"

#endif // mpl_na_spec_h			// 헤더만 가드를 침..

#define		ns_i			pp_tuple_elem(2, 0, na_spec)
#define		name			pp_tuple_elem(2, 1, na_spec)

namespace i3
{
	namespace mpl
	{
		template<>
		struct name< pp_enum_datas(ns_i, na) >
		{
			template< pp_enum_params_1_with_a_partial_default(mpl_arity_limit, ns_i, class T, na) >
			struct apply : name< pp_enum_params_1(ns_i, T) > {};
		};

		template<class Tag>
		struct lambda< name< pp_enum_datas(ns_i, na) >, Tag >
		{
			typedef false_		is_le;
			typedef name< pp_enum_datas(ns_i, na) > result_;
			typedef name< pp_enum_datas(ns_i, na) > type;
		};
	}
}

#undef		name
#undef		ns_i
#undef		na_spec

