#pragma once

namespace i3
{
	namespace bi
	{
		template<class V, class T>
		void visit_each( V& v, const value<T>& t, int )
		{
			using i3::visit_each;
			visit_each(v, t.get(), 0);
		}

		template<class V, class R, class F, class L>
		void visit_each(V& v, const bind_t<R,F,L>& t, int)
		{
			t.accept(v);
		}
	}
}