#pragma once

#include <typeinfo>
#include "../pool/class_common_pool.h"
#include "type_traits/remove_reference.h"
#include "move/move.h"

namespace i3
{
	// boost any의 원본과 거의 같게...

	class any
	{
		I3_COPYABLE_MOVABLE(any)
	public:
		any() : m_content(void_holder::i()) {}
		template<class value_type>
		any(const value_type& val) : m_content( new holder<value_type>(val)) {}


		any(const any& rhs) : m_content( rhs.m_content->clone() ) {}		// 조건문 제거..
		any(rv_ref(any) rval) : m_content(rval.m_content)		
		{
			rval.m_content = void_holder::i();
		}

		~any() { m_content->release(); }
		
		any& swap(any& rhs) {  i3::swap(m_content, rhs.m_content); return *this; }
		template<class value_type>
		any&	operator=(const value_type& rhs)	{  any(rhs).swap(*this);	return *this; }

		any&	operator=(const_assign_ref(any) rhs) { any(static_cast<const any&>(rhs)).swap(*this); return *this; }

		any&	operator=(rv_ref(any) rval) 
		{ 
			m_content->release();
			m_content = rval.m_content; rval.m_content = void_holder::i(); 
			return *this;
		}

	//	any&	operator=(const any& rhs)			{ any(rhs).swap(*this); return *this; }
		
		bool	empty() const { return m_content == void_holder::i(); }
		const std::type_info& type() const { return m_content->type(); }

		void	clear() { m_content->release();		m_content = void_holder::i(); }	// 인터페이스 추가..
		
		void	move(any& rhs) 
		{
			m_content->release();
			m_content = rhs.m_content;	rhs.m_content = void_holder::i(); 
		}

	private:
		struct placeholder 
		{
			virtual ~placeholder() {}
			virtual const std::type_info&	type() const = 0;
			virtual placeholder*			clone() const = 0;
			virtual void					release() const = 0;
		};
		template<class value_type>
		struct holder : public i3::class_common_pool< holder<value_type>, placeholder>
		{
			holder(const value_type& val) : m_value(val) {}
			virtual const std::type_info&	type() const { return typeid(value_type); }			
			virtual placeholder*			clone() const
			{
#if defined( I3_DEBUG)
				placeholder * p = new holder(m_value);
				NetworkDump_Malloc( p, sizeof(holder), __FILE__, __LINE__);
				return p;
#else
				return new holder(m_value);
#endif
			}
			virtual void					release() const
			{
				NetworkDump_Free( const_cast<holder*>(this), __FILE__, __LINE__);
				delete this;
			}
			value_type	m_value;
		};
		
		template<class ignored>
		struct template_void_holder : public placeholder
		{
			template_void_holder(){}
			virtual const std::type_info&	type() const { return typeid(void); }
			virtual placeholder*			clone() const {  return &ms_void_holder;  }
			virtual void					release() const {}		// do nothing..
			
			static template_void_holder*	i() { return &ms_void_holder; }	
			static template_void_holder		ms_void_holder;
		};
		typedef template_void_holder<std::size_t> void_holder;				// std::size_t는 ignored_type으로 cpp의 스태틱선언강제를 막는 용도..

		placeholder*		m_content;
		
		template<class value_type> friend value_type*	any_cast(any*);
		template<class value_type> friend value_type*	unsafe_any_cast(any*);
	};

	template<class ignored>
	any::template_void_holder<ignored> any::template_void_holder<ignored>::ms_void_holder;

	inline void swap(any& a, any& b) {	a.swap(b);	}

	class bad_any_cast : public std::bad_cast
	{
	public:	virtual const char*	what() const throw() { return "i3::bad_any_cast : failed conversion using i3::any_cast"; }
	};
	
	template<class value_type> inline
	value_type* any_cast(any* a) 
	{
		return ( a && a->type() == typeid(value_type) )	? &static_cast<any::holder<value_type>*>(a->m_content)->m_value	: 0;
	}

	template<class value_type> inline const value_type* any_cast(const any* a) {	return any_cast<value_type>(const_cast<any*>(a)); }

	template<class value_type> inline value_type any_cast(any& a)
	{
		typedef typename remove_reference<value_type>::type nonref;	nonref* result = any_cast<nonref>(&a);
		if(!result)	throw bad_any_cast();
		return *result;
	}

	template<class value_type> inline 
	value_type any_cast(const any& a)
	{
		typedef typename remove_reference<value_type>::type nonref;	return any_cast<const nonref &>(const_cast<any&>(a));
	}

	template<class value_type> inline 
	value_type* unsafe_any_cast(any* a) {	return &static_cast<any::holder<value_type>*>(a->m_content)->m_value;	}

	template<class value_type> inline 
	const value_type * unsafe_any_cast(const any* a) {	return unsafe_any_cast<value_type>(const_cast<any*>(a));  }



}