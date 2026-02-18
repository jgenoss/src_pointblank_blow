#pragma once


#include "function_equal.h"

namespace i3
{
	template<class F> inline bool operator==(const function<F>& f, detail::function::useless_clear_type*) {	return f.empty();	}
	template<class F> inline bool operator!=(const function<F>& f, detail::function::useless_clear_type*) {	return !f.empty();	}
	template<class F> inline bool operator==(detail::function::useless_clear_type*, const function<F>& f) {	return f.empty();	}
	template<class F> inline bool operator!=(detail::function::useless_clear_type*, const function<F>& f) {	return !f.empty();	}
	
	template<class F, class Functor> inline typename disable_if<is_integral<Functor>, 
	bool>::type operator==(const function<F>& f, Functor g)
	{	if (const Functor* fp = f.template target<Functor>()) return function_equal(*fp, g);	return false; }
	
	template<class F, class Functor> inline typename disable_if<is_integral<Functor>, 
	bool>::type operator==(Functor g, const function<F>& f)
	{	if (const Functor* fp = f.template target<Functor>()) return function_equal(g, *fp);	return false;	}
	
	template<class F, class Functor> inline typename disable_if<is_integral<Functor>, 
	bool>::type operator!=(const function<F>& f, Functor g)
	{	if (const Functor* fp = f.template target<Functor>()) return !function_equal(*fp, g);	return true; }
	
	template<class F, class Functor> inline typename disable_if<is_integral<Functor>, 
	bool>::type operator!=(Functor g, const function<F>& f)
	{	if (const Functor* fp = f.template target<Functor>()) return !function_equal(g, *fp);	return true; }
	

	template<class F, class Functor> inline typename disable_if<is_integral<Functor>, 
	bool>::type operator==(const function<F>& f, reference_wrapper<Functor> g)
	{	if (const Functor* fp = f.template target<Functor>()) return fp == g.get_pointer() ;	return false; }
	
	template<class F, class Functor> inline typename disable_if<is_integral<Functor>, 
	bool>::type operator==(reference_wrapper<Functor> g, const function<F>& f)
	{	if (const Functor* fp = f.template target<Functor>()) return g.get_pointer() == fp;	return false;	}
	
	template<class F, class Functor> inline typename disable_if<is_integral<Functor>, 
	bool>::type operator!=(const function<F>& f, reference_wrapper<Functor> g)
	{	if (const Functor* fp = f.template target<Functor>()) return fp != g.get_pointer() ;	return true; }
	
	template<class F, class Functor> inline typename disable_if<is_integral<Functor>, 
	bool>::type operator!=(reference_wrapper<Functor> g, const function<F>& f)
	{	if (const Functor* fp = f.template target<Functor>()) return g.get_pointer() != fp;		return true;	}


}