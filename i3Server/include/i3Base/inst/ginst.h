#pragma once

#include "inst_memory.h"
#include "../itl/null_type.h"
#include "../memory/delayed_atexit.h"

namespace i3
{
	// 
	// 널값으로 초기화되고, 나중에 대입되는 객체도 필요할 것이다..
	// 


	template<class I> struct use_delayed_atexit_ginst : false_type {};
	
	namespace detail_ginst { template<class, bool> struct atexit_ginst; }

	template<class I>
	class ginst
	{
	public:
		typedef use_delayed_atexit_ginst<I>				use_delayed_atexit_ginst;	

		operator		I* () { return ms_i; }
		operator const	I* () const { return ms_i; }
		I*				operator->() const { return ms_i; }
		I&				operator*() const { return *ms_i; }
		ginst<I>&		operator=(I* rhs);
		static void		destroy_explicit() { _destroy(); }		// optional function..

	private:
		template<class, bool> friend struct detail_ginst::atexit_ginst;	

		static I*		_create();
		static void		_destroy();
		static I*		ms_i;
	};

	template<class I> I*			ginst<I>::ms_i = ginst<I>::_create();
	
	template<class I, class B = null_type>
	class shared_ginst : public B 
	{
	public:
		static I*			i() { return ginst<I>(); }
	};
			
}

#include "ginst.inl"
