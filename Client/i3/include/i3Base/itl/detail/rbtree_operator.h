#pragma once

#include "../algorithm/equal.h"
#include "../algorithm/lexicographical_compare.h"

namespace i3
{
	
	template<class V, class P, class A> inline
	bool operator==(const rbtree_base<V, P, A>& lhs, const rbtree_base<V, P, A>& rhs)
	{	
		return lhs.size() == rhs.size() && i3::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template<class V, class P, class A> inline
	bool operator!=(const rbtree_base<V, P, A>& lhs, const rbtree_base<V, P, A>& rhs){	return !(lhs == rhs);	}

	template<class V, class P, class A> inline
	bool operator<(const rbtree_base<V, P, A>& lhs, const rbtree_base<V, P, A>& rhs)
	{	
		return i3::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template<class V, class P, class A> inline
	bool operator>(const rbtree_base<V, P, A>& lhs, const rbtree_base<V, P, A>& rhs){	return rhs < lhs;	}

	template<class V, class P, class A> inline
	bool operator<=(const rbtree_base<V, P, A>& lhs, const rbtree_base<V, P, A>& rhs){	return !(rhs < lhs); }

	template<class V, class P, class A> inline
	bool operator>=(const rbtree_base<V, P, A>& lhs, const rbtree_base<V, P, A>& rhs) {	return !(lhs < rhs); }
	
	
	
}