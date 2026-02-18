#pragma once

namespace i3
{
	template<class Visitor, class T> inline
	void visit_each(Visitor& v, const T& t, long) { v(t); }

	template<class Visitor, class T> inline
	void visit_each(Visitor& v, const T& t) { visit_each(v, t, 0); }
}
