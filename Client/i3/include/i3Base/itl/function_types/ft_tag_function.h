#pragma once



namespace i3
{
	//
	// 크게 2부분으로 나눔... ( calling convention / 타입 )
	//

	namespace ft 
	{
		template<bits32 Bits>
		struct decode_flags {	static const bits32 value = Bits & pp_ft_mask_flags;	};		// ccid부분 제외..
		
		template<bits32 Bits>
		struct decode_cc_id {	static const bits32 value =  ( ( Bits & pp_ft_mask_full) / pp_ft_bit_default_cc ) >> 1;	};

		template<bits32 Bits>
		struct decode_tag_bits { static const bits32 value = Bits & pp_ft_mask_full;  };			// flag + ccid
		
		template<bits32 Bits>
		struct decode_arity		{ static const std::size_t value = Bits >> pp_ft_shift_arity; };
				

		template<bits32 Flags, bits32 CCID>
		struct encode_bits
		{
			static const bits32 value = Flags | ( ( pp_ft_bit_default_cc * CCID )<< 1 );
		};
			
		template<class Tag1, class Tag2 = null_bits_tag, class Tag3 = null_bits_tag, class Tag4 = null_bits_tag>
		struct	tag : bits_tag_combine<Tag1, Tag2, Tag3, Tag4> {};

		namespace detail
		{
			template<class Tag, class RefTag = Tag> struct retag_default_cc; 
		}

		template<class Tag, class QueryTag>
		struct represents : is_bits_tag_match< Tag, detail::retag_default_cc<QueryTag,Tag> > {};

	}
}
