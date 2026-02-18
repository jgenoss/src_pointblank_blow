#pragma once

#include "variant_fwd.h"
#include "../hash.h"
#include "static_visitor.h"

namespace i3
{

	namespace detail
	{
		namespace variant 
		{
			struct variant_hasher: public i3::static_visitor<std::size_t> 
			{
				template <class T>	std::size_t operator()(const T& val) const 
				{
					i3::hash<T> hasher;
					return hasher(val);
				}
			};
		}
	}

	template<pp_enum_params(variant_limit, class T)> inline
	std::size_t hash_value( const variant<pp_enum_params(variant_limit, T)>& val) 
	{
		std::size_t seed = i3::apply_visitor(detail::variant::variant_hasher(), val);
		i3::hash_combine(seed, val.which());
		return seed;
	}
}
