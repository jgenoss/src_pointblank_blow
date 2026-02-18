#pragma once

namespace i3
{
	inline
	bool	same_of(const i3::element* p, const class_meta* compare_meta)
	{
		return (p) ? p->same_of(compare_meta)  : false;
	}

	inline
	bool	kind_of(const i3::element* p, const class_meta* compare_meta)
	{
		return (p) ? p->kind_of(compare_meta) : false;
	}

	//
	template<class Ptr> __forceinline
	bool	same_of( const class_meta* p ) 
	{
		compile_assert_pred( (i3::is_pointer<Ptr>) );
		typedef typename i3::remove_pointer<Ptr>::type	U;
		compile_assert_pred( (i3::has_static_meta<U>) );
		return (p) ? p->same_of(U::static_meta()) : false;
	}

	template<class Ptr> __forceinline
	bool	kind_of(const class_meta* p ) 
	{
		compile_assert_pred( (i3::is_pointer<Ptr>) );
		typedef typename i3::remove_pointer<Ptr>::type	U;
		compile_assert_pred( (i3::has_static_meta<U>) );
		return (p) ? p->kind_of(U::static_meta()) : false;
	}
	
}
