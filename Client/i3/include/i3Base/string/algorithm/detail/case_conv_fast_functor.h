#pragma once

namespace i3
{
	
	template<class C>
	struct to_lower_functor_fast : i3::unary_function<C, C>
	{
		enum {ALPHABET_INTERVAL = 'a' - 'A', };

		inline C operator()(C ch) const 
		{ 
			return ( ch < C('A') || ch > C('Z') ) ? ch : C( ch + ALPHABET_INTERVAL);
		}
	};

	template<class C>
	struct to_upper_functor_fast : i3::unary_function<C, C>
	{
		enum {ALPHABET_INTERVAL = 'a' - 'A', };

		inline C operator()(C ch) const 
		{ 
			return ( ch < C('a') || ch > C('z') ) ? ch : C(ch - ALPHABET_INTERVAL);
		}
	};


}