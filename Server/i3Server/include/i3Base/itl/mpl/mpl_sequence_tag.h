#pragma once


#include "mpl_sequence_tag_fwd.h"
#include "../type_traits/has_tag.h"
#include "../type_traits/has_begin.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
				
		namespace aux
		{
			template<bool Has_tag, bool Has_begin>
			struct sequence_tag_impl {	template<class Seq> struct result { typedef typename Seq::tag	type; };	};

			template<bool Has_begin>
			struct sequence_tag_impl<false, Has_begin> { template<class Seq> struct result { typedef nested_begin_end_tag	type; }; };
			
			template<>
			struct sequence_tag_impl<false, false> { template<class Seq> struct result { typedef non_sequence_tag	type; };  };

		}

		template<class Seq = na> 
		struct sequence_tag : aux::sequence_tag_impl< has_tag<Seq>::value, has_begin<Seq>::value>::template result<Seq>		// 지연원칙에따라, type붙지 않음.
		{
		};


	}
}


#define na_spec (1, sequence_tag)
#include "aux_/mpl_na_spec.h"

