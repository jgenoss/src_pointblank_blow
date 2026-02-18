#pragma once 


namespace i3
{

	

	
	template <class C, class T, class A> inline
	bool	operator==(const fixed_basic_string<C, T, A>& left, const fixed_basic_string<C, T, A>& right) {	return left.equal_to(right);	}
	
	template <class C, class T, class A> inline
	bool	operator==(const C* left, const fixed_basic_string<C, T, A>& right) {	return right.equal_to(left); }

	template <class C, class T, class A> inline
	bool	operator==(const fixed_basic_string<C, T, A>& left, const C* right) {	return left.equal_to(right);	}
	
	template <class C, class T, class A> inline
	bool	operator==(const fixed_basic_string<C, T, A>& left, const rc_basic_string<C, T, A>& right) {	return left.equal_to(right);	}
	
	template <class C, class T, class A> inline
	bool	operator==(const rc_basic_string<C, T, A>& left, const fixed_basic_string<C, T, A>& right) {	return right.equal_to(left);	}
	

	template <class C, class T, class A> inline
	bool	operator!=(const fixed_basic_string<C, T, A>& left, const fixed_basic_string<C, T, A>& right) {	return !(left == right); }

	template <class C, class T, class A> inline
	bool	operator!=(const C* left, const fixed_basic_string<C, T, A>& right) {	return !(left == right); }

	template <class C, class T, class A> inline
	bool	operator!=(const fixed_basic_string<C, T, A>& left, const C* right) {	return !(left == right); }

	template <class C, class T, class A> inline
	bool	operator!=(const fixed_basic_string<C, T, A>& left, const rc_basic_string<C, T, A>& right) {	return !(left == right); }

	template <class C, class T, class A> inline
	bool	operator!=(const rc_basic_string<C, T, A>& left, const fixed_basic_string<C, T, A>& right) {	return !(left == right); }

///////////////////////////////////////////////////////////////////////
	template <class C, class T, class A> inline
	bool	operator<(const fixed_basic_string<C, T, A>& left, const fixed_basic_string<C, T, A>& right) {	return left.compare(right) < 0; }

	template <class C, class T, class A> inline
	bool	operator<(const C* left, const fixed_basic_string<C, T, A>& right) {	return right.compare(left) > 0; }

	template <class C, class T, class A> inline
	bool	operator<(const fixed_basic_string<C, T, A>& left, const C* right) {	return left.compare(right) < 0; }
	
	template <class C, class T, class A> inline
	bool	operator<(const fixed_basic_string<C, T, A>& left, const rc_basic_string<C, T, A>& right) {	return left.compare(right) < 0; }
	
	template <class C, class T, class A> inline
	bool	operator<(const rc_basic_string<C, T, A>& left, const fixed_basic_string<C, T, A>& right) {	return right.compare(left) > 0; }

	
	template <class C, class T, class A> inline
	bool	operator>(const fixed_basic_string<C, T, A>& left, const fixed_basic_string<C, T, A>& right) {	return right < left; }

	template <class C, class T, class A> inline
	bool	operator>(const C* left, const fixed_basic_string<C, T, A>& right) {	return right < left; }
	
	template <class C, class T, class A> inline
	bool	operator>(const fixed_basic_string<C, T, A>& left, const C* right) {	return right < left; }

	template <class C, class T, class A> inline
	bool	operator>(const fixed_basic_string<C, T, A>& left, const rc_basic_string<C, T, A>& right) {	return right < left; }

		template <class C, class T, class A> inline
	bool	operator>(const rc_basic_string<C, T, A>& left, const fixed_basic_string<C, T, A>& right) {	return right < left; }


////////////////////////////////////////////////////////////////////////

	template <class C, class T, class A> inline
	bool	operator<=(const fixed_basic_string<C, T, A>& left, const fixed_basic_string<C, T, A>& right) {	return !(right < left); }

	template <class C, class T, class A> inline
	bool	operator<=(const C* left, const fixed_basic_string<C, T, A>& right) {	return !(right < left); }
	
	template <class C, class T, class A> inline
	bool	operator<=(const fixed_basic_string<C, T, A>& left, const C* right) {	return !(right < left); }

	template <class C, class T, class A> inline
	bool	operator<=(const fixed_basic_string<C, T, A>& left, const rc_basic_string<C, T, A>& right) {	return !(right < left); }

	template <class C, class T, class A> inline
	bool	operator<=(const rc_basic_string<C, T, A>& left, const fixed_basic_string<C, T, A>& right) {	return !(right < left); }

////////////////////////////////////////////////////////////////////////

	template <class C, class T, class A> inline
	bool	operator>=(const fixed_basic_string<C, T, A>& left, const fixed_basic_string<C, T, A>& right) {	return !(left < right); }

	template <class C, class T, class A> inline
	bool	operator>=(const C* left, const fixed_basic_string<C, T, A>& right) {	return !(left < right); }
	
	template <class C, class T, class A> inline
	bool	operator>=(const fixed_basic_string<C, T, A>& left, const C* right){	return !(left < right); }

	template <class C, class T, class A> inline
	bool	operator>=(const fixed_basic_string<C, T, A>& left, const rc_basic_string<C, T, A>& right) {	return !(left < right); }

	template <class C, class T, class A> inline
	bool	operator>=(const rc_basic_string<C, T, A>& left, const fixed_basic_string<C, T, A>& right) {	return !(left < right); }


////////////////////////////////////////////////////////////////////////
	
	
}

