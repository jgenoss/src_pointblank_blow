#pragma once

namespace i3
{
	namespace mpl
	{
		template< class Tag > struct begin_impl;
		template< class Tag > struct end_impl;

		template< class Seq > struct begin;
		template< class Seq > struct end;
	}
}
