#pragma once

#include "mpl_begin_end_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class Seq> struct begin_type { typedef typename Seq::begin	type; };
			template<class Seq> struct end_type   { typedef typename Seq::end   type; };
		}

		
		template< class Tag >
		struct begin_impl
		{
			template< class Seq > struct apply
			{
				typedef typename eval_if<has_begin<Seq>, aux::begin_type<Seq>, void_>::type type;
			};
		};

		template< class Tag >
		struct end_impl
		{
			template< class Seq > struct apply
			{
				typedef typename eval_if<has_begin<Seq>, aux::end_type<Seq>, void_>::type type;
			};
		};

////////// 몇가지 예외적인 태그 처리..
		template<> struct begin_impl<nested_begin_end_tag> { template<class Seq> struct apply { typedef typename Seq::begin		type; }; };
		template<> struct end_impl<nested_begin_end_tag> { template<class Seq> struct apply { typedef typename Seq::end		type; }; };
		
		template<> struct begin_impl<non_sequence_tag> { template<class Seq> struct apply { typedef void_		type; }; };
		template<> struct end_impl<non_sequence_tag> { template<class Seq> struct apply { typedef void_		type; }; };
		template<> struct begin_impl<na> { template<class Seq> struct apply { typedef void_		type; }; };
		template<> struct end_impl<na> { template<class Seq> struct apply { typedef void_		type; }; };

//////////


		template<class Seq = na>
		struct begin
		{
			typedef typename sequence_tag<Seq>::type	tag_;
			typedef typename begin_impl<tag_>::template apply<Seq>::type	type;
		};

		template<class Seq = na>
		struct end
		{
			typedef typename sequence_tag<Seq>::type	tag_;
			typedef typename end_impl<tag_>::template apply<Seq>::type	type;
		};
		
	}
}

#define na_spec (1, begin)
#include "aux_/mpl_na_spec.h"
#define na_spec (1, end)
#include "aux_/mpl_na_spec.h"

