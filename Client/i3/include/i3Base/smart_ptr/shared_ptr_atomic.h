#pragma once

#include "detail/spinlock_pool.h"

namespace i3
{
	template<class T> 
	shared_ptr<T> atomic_load(const shared_ptr<T>* p) { i3::detail::spinlock_pool<2>::scoped_lock lock(p); return *p; }	
	
	template<class T>
	void atomic_store(shared_ptr<T>* p, shared_ptr<T> r)
	{
		i3::detail::spinlock_pool<2>::scoped_lock lock(p); p->swap(r); 
	}
	
	template<class T> 
	shared_ptr<T> atomic_exchange( shared_ptr<T> * p, shared_ptr<T> r )
	{
		i3::detail::spinlock& sp = i3::detail::spinlock_pool<2>::spinlock_for( p );

		sp.lock();
		p->swap( r );
		sp.unlock();

		return r; // return std::move( r )
	}

	template<class T> 
	bool atomic_compare_exchange( shared_ptr<T>* p, shared_ptr<T>* v, shared_ptr<T> w )
	{
		i3::detail::spinlock& sp = i3::detail::spinlock_pool<2>::spinlock_for( p );
		sp.lock();

		if( p->internal_equiv(*v) )
		{
			p->swap( w );
			sp.unlock();
			return true;
		}
		else
		{
			shared_ptr<T> tmp( *p );
			sp.unlock();
			tmp.swap( *v );
			return false;
		}
	}

}
