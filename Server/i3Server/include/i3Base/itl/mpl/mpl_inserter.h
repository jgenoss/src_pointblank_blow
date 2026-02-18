#pragma once

namespace i3
{
	namespace mpl
	{
		template<class Seq, class Op> struct inserter {	typedef Seq state;	typedef Op operation; };
	}
}
