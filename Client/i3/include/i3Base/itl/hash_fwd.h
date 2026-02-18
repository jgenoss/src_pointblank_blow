#pragma once

#include <cstddef>

namespace i3
{
	template<class T> struct hash;

	template<class T> void hash_combine(std::size_t& seed, const T& val);

	template<class It> std::size_t	hash_range(It f, It l);
	template<class It> void			hash_range(std::size_t& out, It f, It l);
}
