#pragma once

namespace i3
{
	namespace mpl
	{
		template<class Tag> struct has_key_impl;
		template<class AssociativeSeq, class Key> struct has_key;
	}
}
