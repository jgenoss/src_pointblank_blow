#pragma once

#include "../../mpl/mpl_begin_end_fwd.h"

namespace i3
{
	namespace mpl
	{
		
		template<> 
		struct size_impl< ft::components_mpl_sequence_tag >
		{
		  template< typename S > struct apply : mpl::size<typename S::types>  {};
		};

		template<> 
		struct empty_impl< ft::components_mpl_sequence_tag >
		{
		  template< typename S > struct apply : mpl::empty <typename S::types> {};
		};

		template<> 
		struct front_impl< ft::components_mpl_sequence_tag>
		{
		  template< typename S > struct apply : mpl::front <typename S::types> {};
		};

		template<> 
		struct back_impl< ft::components_mpl_sequence_tag >
		{
		  template< typename S > struct apply : mpl::back <typename S::types> {};
		};

		template<> 
		struct at_impl< ft::components_mpl_sequence_tag >
		{
		  template< typename S, typename N > struct apply : mpl::at <typename S::types, N >{};
		};

		template<> 
		struct begin_impl< ft::components_mpl_sequence_tag >
		{
		  template< typename S > struct apply : mpl::begin <typename S::types> {};
		};

		template<> 
		struct end_impl< ft::components_mpl_sequence_tag >
		{
		  template< typename S > struct apply : mpl::end <typename S::types> {};
		};

		template<> 
		struct clear_impl< ft::components_mpl_sequence_tag >
		{
		  template< typename S > struct apply : S
		  {
			typedef apply type;		typedef typename mpl::clear< typename S::types >::type types;
		  };
		};

		template<>
		struct push_front_impl< ft::components_mpl_sequence_tag >
		{
		  template< typename S, typename T > struct apply : S
		  { 
			typedef apply type;		typedef typename mpl::push_front< typename S::types, T >::type types;
		  };
		};

		template<>
		struct pop_front_impl< ft::components_mpl_sequence_tag >
		{
		  template< typename S > struct apply : S
		  {
			typedef apply type;		typedef typename mpl::pop_front< typename S::types >::type types; 
		  };
		};

		template<>
		struct push_back_impl< ft::components_mpl_sequence_tag >
		{
		  template< typename S, typename T > struct apply : S
		  {
			typedef apply type;		typedef typename mpl::push_back< typename S::types, T >::type types; 
		  };
		};

		template<>
		struct pop_back_impl< ft::components_mpl_sequence_tag >
		{
		  template< typename S > struct apply : S
		  {
			typedef apply type;		typedef typename mpl::pop_back< typename S::types >::type types; 
		  };
		};

	}
}

