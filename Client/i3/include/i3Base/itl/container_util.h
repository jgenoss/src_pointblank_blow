#pragma once

#include "functor_util.h"
#include "algorithm_util.h"
#include "type_traits/is_template_same.h"
#include "mpl/mpl_placeholders.h"
#include "algorithm/for_each.h"

namespace i3
{

namespace container_util
{
	

	namespace detail 
	{
		template<class Ctn, class F> __forceinline
			void for_each_if_pair( Ctn& ctn, F f, mpl::false_)
		{
			i3::for_each(ctn.begin(), ctn.end(), f );
		}
		template<class Ctn, class F> __forceinline
			void for_each_if_pair( Ctn& ctn, F f, mpl::true_)
		{
			i3::au::for_each_2nd(ctn.begin(), ctn.end(), f);
		}
	}

	template<class Ctn, class Fn> __forceinline
	void	for_each_container(Ctn& ctn, Fn f) 
	{	
		typedef typename Ctn::value_type		value_type;
		detail::for_each_if_pair(ctn, f, mpl::bool_< i3::is_template_same< i3::pair<mpl::_,mpl::_>, value_type>::value>() );   
	}

	template<class Ctn>
	void for_each_delete( Ctn& ctn ) {  for_each_container(ctn, i3::fu::delete_fun() );  }
	
	template<class Ctn>
	void for_each_safe_delete( Ctn& ctn ) {  for_each_container( ctn, i3::fu::safe_delete_fun() );  }

	template<class Ctn>
	void for_each_delete_clear( Ctn& ctn ) {  for_each_container( ctn, i3::fu::delete_fun() ); ctn.clear();   }
	
	template<class Ctn, class R, class C>
	void for_each_safe_release( Ctn& ctn, R (C::*f)() ) 
	{
		typedef typename Ctn::value_type		value_type;
		detail::for_each_if_pair(ctn, i3::fu::mem_fun_safe(f), mpl::bool_< i3::is_template_same< i3::pair<mpl::_,mpl::_>, value_type>::value>());	
	}

	template<class Ctn, class R, class C>
	void for_each_safe_release( const Ctn& ctn, R (C::*f)() const ) 
	{
		typedef typename Ctn::value_type		value_type;
		detail::for_each_if_pair(ctn, i3::fu::mem_fun_safe(f), mpl::bool_< i3::is_template_same< i3::pair<mpl::_,mpl::_>, value_type>::value>());	
	}

	template<class Ctn, class R, class C>
	void for_each_release( Ctn& ctn, R (C::*f)() ) 
	{
		typedef typename Ctn::value_type		value_type;
		detail::for_each_if_pair(ctn, i3::mem_fun(f), mpl::bool_< i3::is_template_same< i3::pair<mpl::_,mpl::_>, value_type>::value>());	
	}
	template<class Ctn, class R, class C>
	void for_each_release( const Ctn& ctn, R (C::*f)() const ) 
	{
		typedef typename Ctn::value_type		value_type;
		detail::for_each_if_pair(ctn, i3::mem_fun(f), mpl::bool_< i3::is_template_same< i3::pair<mpl::_,mpl::_>, value_type>::value>());	
	}	

	template<class Ctn, class R, class C>
	void for_each_safe_release_clear( Ctn& ctn, R (C::*f)() ) { for_each_safe_release(ctn, f);	ctn.clear(); }
	template<class Ctn, class R, class C>
	void for_each_safe_release_clear( Ctn& ctn, R (C::*f)() const ) { for_each_safe_release(ctn, f);	ctn.clear(); }

	template<class Ctn, class R, class C>
	void for_each_release_clear( Ctn& ctn, R (C::*f)() ) { for_each_release(ctn, f);	ctn.clear(); }
	template<class Ctn, class R, class C>
	void for_each_release_clear( Ctn& ctn, R (C::*f)() const ) { for_each_release(ctn, f);	ctn.clear(); }


}

namespace cu = container_util;

}