#pragma once

//
//  생성동기 : 어떠한 경우에도 유효한 value를 갖도록 조정..( 없다면 -1을 사용)
//
#include "mpl_index_if_x.h"

namespace i3
{
	namespace mpl
	{
		template<class Seq = na, class T = na>
		struct index_of_x : index_if_x< Seq, same_as<T> > {};
	}
}


#define na_spec (2, index_of_x)
#include "aux_/mpl_na_spec.h"
