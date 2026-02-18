#pragma once

#include "itl/range/as_literal.h"
#include "itl/range/value_type.h"
#include "itl/iterator/iterator_traits.h"
#include "itl/range/detail/enable_if_iterator.h"
#include "string/ext/make_generic_literal.h"
#include "string/ext/safe_string_copy.h"
#include "itl/type_traits/is_arithmetic.h"

template<class Lit, class InputRng> inline typename i3::enable_if<mpl::and_< i3::is_iterator<Lit>, mpl::not_< i3::is_arithmetic<InputRng> > >,
void>::type ColorString(Lit out, size_t buff_size, const InputRng& rng, UINT32 r, UINT32 g, UINT32 b, UINT32 a = 255)
{
	typedef typename i3::iterator_value<Lit>::type Ch;
	i3::snprintf(out, buff_size, i3::make_generic_literal(Ch, "{col:%d,%d,%d,%d}%s{/col}"), r,g,b,a, rng);
}

template<class Seq, class InputRng> inline typename i3::disable_if< mpl::or_< i3::is_iterator<Seq>, i3::is_arithmetic<InputRng> >,
void>::type ColorString(Seq& out, const InputRng& rng, UINT32 r, UINT32 g, UINT32 b, UINT32 a  = 255)
{
	typedef typename i3::range_value<Seq>::type  Ch;
	i3::sprintf(out, i3::make_generic_literal(Ch, "{col:%d,%d,%d,%d}%s{/col}"), r,g,b,a, rng);
}

template<class Lit> inline typename i3::enable_if_iterator<Lit,
void>::type ColorString(Lit inout, size_t buff_size,  UINT32 r, UINT32 g, UINT32 b, UINT32 a  = 255)
{
	typedef typename i3::iterator_value<Lit>::type Ch;
	i3::stack_basic_string<mpl::size_t<128>, Ch>  str_temp;
	i3::snprintf(str_temp, buff_size, i3::make_generic_literal(Ch, "{col:%d,%d,%d,%d}%s{/col}"), r,g,b,a, inout);
	i3::safe_string_copy(inout, str_temp, buff_size);
}

template<class Seq> inline typename i3::disable_if_iterator<Seq,
void>::type ColorString(Seq& inout, UINT32 r, UINT32 g, UINT32 b, UINT32 a  = 255)
{
	typedef typename i3::range_value<Seq>::type Ch;
	i3::stack_basic_string<mpl::size_t<128>, Ch>  str_temp;
	i3::sprintf(str_temp, i3::make_generic_literal(Ch, "{col:%d,%d,%d,%d}%s{/col}"), r,g,b,a, inout);
	i3::generic_string_copy(inout, str_temp);
}

template<class Lit, class InputRng> inline typename i3::enable_if_iterator<Lit,
void>::type ColorString(Lit out, size_t buff_size, const InputRng& rng, I3COLOR color)
{
	ColorString(out, buff_size, rng, color.r, color.g, color.b, color.a);		
}

template<class Seq, class InputRng> inline typename i3::disable_if_iterator<Seq,
void>::type ColorString(Seq& out, const InputRng& rng, I3COLOR color)
{
	ColorString(out, rng, color.r, color.g, color.b, color.a);		
}

template<class Lit> inline typename i3::enable_if_iterator<Lit,
void>::type ColorString(Lit inout, size_t buff_size, I3COLOR color)
{
	ColorString(inout, buff_size, color.r, color.g, color.b, color.a);		
}

template<class Seq> inline typename i3::disable_if_iterator<Seq,
void>::type ColorString(Seq& inout, I3COLOR color)
{
	ColorString(inout, color.r, color.g, color.b, color.a);		
}

