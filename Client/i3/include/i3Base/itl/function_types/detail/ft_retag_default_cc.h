#pragma once


namespace i3
{
	namespace ft
	{

		namespace detail
		{
			template<class Tag, class RefTag>
			struct selector_bits 
			{
				static const bits32 value =	( bits<Tag>::value & pp_ft_bit_default_cc) | ( bits<RefTag>::value & (pp_ft_mask_member|pp_ft_mask_variadic));
			};	
			
			template<bits32 bit> struct cc_tag_select;

			template<class Tag, class RefTag> 
			struct retag_default_cc : bits_tag_combine<Tag, typename cc_tag_select< selector_bits<Tag, RefTag>::value >::type> {};
			
			class test_class;

			template<bits32 bit> 
			struct cc_tag_select 
			{
				typedef null_tag	type;
			};
			
			template<>
			struct cc_tag_select< pp_ft_bit_default_cc|pp_ft_bit_non_member|pp_ft_bit_non_variadic > 
			{
				typedef void (*tester)();
				typedef bits_tag< components<tester>::bits::value & pp_ft_mask_cc , pp_ft_mask_cc >	type;
			};
			
			template<>
			struct cc_tag_select<pp_ft_bit_default_cc|pp_ft_bit_non_member|pp_ft_bit_variadic>
			{
				typedef void (*tester)(...);
				typedef bits_tag<components<tester>::bits::value & pp_ft_mask_cc, pp_ft_mask_cc >	type;
			};
			
			template<>
			struct cc_tag_select<pp_ft_bit_default_cc|pp_ft_bit_member| pp_ft_bit_non_variadic>
			{
				typedef void (test_class::*tester)();
				typedef bits_tag<components<tester>::bits::value & pp_ft_mask_cc, pp_ft_mask_cc >	type;
			};			
			
			template<>
			struct cc_tag_select<pp_ft_bit_default_cc|pp_ft_bit_member|pp_ft_bit_variadic>
			{
				typedef void (test_class::*tester)(...);
				typedef bits_tag<components<tester>::bits::value & pp_ft_mask_cc, pp_ft_mask_cc >	type;
			};	


		}
		

	}
}