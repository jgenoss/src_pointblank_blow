#pragma once

#include "bits32.h"
#include "../mpl/mpl_bitxor.h"

namespace i3
{
	
	template<bits32 Bits, bits32 Mask> 
	struct bits_tag  {	typedef mpl::bits32<Bits>	bits;	typedef mpl::bits32<Mask>	mask;	};

	template<class T> struct bits : T::bits {};
	template<class T> struct mask : T::mask {};
	
	namespace detail
	{
		template<bits32 Lbit, bits32 Rbit, bits32 Rmask>
		struct is_bits_tag_match_impl
		{
			static const bool value = (Rbit == (Lbit & Rmask) );
		};
	}

	template<class Tag, class QueryTag>
	struct is_bits_tag_match : mpl::bool_< detail::is_bits_tag_match_impl< bits<Tag>::value , bits<QueryTag>::value, mask<QueryTag>::value>::value > {};
	
	namespace detail
	{
		template<bits32 Lbit, bits32 Lmask, bits32 Rbit, bits32 Rmask>
		struct bits_tag_combine_impl
		{
			typedef bits_tag< (Lbit&~Rmask)|Rbit , Lmask|Rmask > type;
		};

		template<class LTag, class RTag>
		struct bits_tag_combine : bits_tag_combine_impl< bits<LTag>::value, mask<LTag>::value, bits<RTag>::value, mask<RTag>::value>::type {};
	}

	typedef bits_tag<0,0> null_bits_tag;

	template<class Tag1, class Tag2 = null_bits_tag, class Tag3 = null_bits_tag, class Tag4 = null_bits_tag>
	struct bits_tag_combine : detail::bits_tag_combine< detail::bits_tag_combine<Tag1, Tag2>, detail::bits_tag_combine<Tag3, Tag4> > {};

	template<class Tag1, class Tag2, class Tag3>
	struct bits_tag_combine<Tag1,Tag2,Tag3,null_bits_tag> : detail::bits_tag_combine< detail::bits_tag_combine<Tag1, Tag2>, Tag3> {};

	template<class Tag1, class Tag2>
	struct bits_tag_combine<Tag1,Tag2,null_bits_tag,null_bits_tag> : detail::bits_tag_combine<Tag1, Tag2> {};

	template<class Tag1>
	struct bits_tag_combine<Tag1, null_bits_tag, null_bits_tag, null_bits_tag> : Tag1 {};
	
	
	template<class Tag, class QueryTag>
	struct extract_bits_tag : bits_tag< (bits<Tag>::value & mask<QueryTag>::value), mask<QueryTag>::value > {};
	
	template<class Base, class OldProp, class NewProp>
	struct changed_bits_tag : Base 
	{
		typedef mpl::bitxor_<typename Base::bits, typename OldProp::bits, typename NewProp::bits> bits;
	};
	
	
}
