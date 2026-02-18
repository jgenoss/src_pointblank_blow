#pragma once 

#include <windows.h>
#include "../itl/noncopyable.h"
#include "../itl/swap.h"

namespace i3
{

	struct dtor_virtual_ref_count_tag {};
	struct all_virtual_ref_count_tag : public dtor_virtual_ref_count_tag {};
	struct dispose_virtual_ref_count_tag : public dtor_virtual_ref_count_tag {};		// dispose가상함수를 추가..

	struct atomic_ref_count_tag{};
	struct non_atomic_ref_count_tag{};
	
	template<	
				class atomic_ref_count_category, 
				class virtual_ref_count_category
			>
	class ref_count_template;
	
	template< 
				class mine,
				class atomic_ref_count_category = non_atomic_ref_count_tag
			>
	class static_ref_count;

/////////////////
	typedef ref_count_template< non_atomic_ref_count_tag, dtor_virtual_ref_count_tag>			ref_count;
	typedef ref_count_template< non_atomic_ref_count_tag, dispose_virtual_ref_count_tag>		ref_count_d;
	typedef ref_count_template< non_atomic_ref_count_tag, all_virtual_ref_count_tag>			ref_count_v;
	
	typedef ref_count_template< atomic_ref_count_tag,  dtor_virtual_ref_count_tag>				ref_count_a;
	typedef ref_count_template< atomic_ref_count_tag, dispose_virtual_ref_count_tag>			ref_count_ad;
	typedef ref_count_template< atomic_ref_count_tag,  all_virtual_ref_count_tag>				ref_count_av;
/////////////////	
	
	class ref_count_base : private noncopyable
	{
	protected:
		typedef ref_count_base				my_type;

		ref_count_base() : m_count(0) {}
		__forceinline long	get_ref_count_impl() const { return m_count; }
		
		void	swap_impl(my_type& rhs) { i3::swap(m_count, rhs.m_count); }
	private:
		mutable long		m_count;

		template<class> friend class ref_count_is_atomic;
	};
	
	template <class atomic_ref_count_category>
	class ref_count_is_atomic;

	template <>
	class ref_count_is_atomic<atomic_ref_count_tag > : public ref_count_base
	{
	protected:	
		__forceinline void	add_ref_impl() const {  InterlockedIncrement(&m_count); }
		__forceinline bool	release_impl() const { 	return ( 0 == InterlockedDecrement( &m_count) ); 	}
	};

	template <>
	class ref_count_is_atomic<non_atomic_ref_count_tag > : public ref_count_base
	{
	protected:	
		__forceinline void	add_ref_impl() const  {  ++m_count; }
		__forceinline bool	release_impl() const {	return ( --m_count == 0 );	}
	};
		
	template< class atomic_ref_count_category, class virtual_ref_count_category>
	class ref_count_template;
	
	template< class atomic_ref_count_category>
	class ref_count_template<atomic_ref_count_category, all_virtual_ref_count_tag>
		: public ref_count_is_atomic<atomic_ref_count_category>
	{
	public:
		typedef ref_count_template<atomic_ref_count_category, all_virtual_ref_count_tag>	my_type;

		virtual		 ~ref_count_template() {}
		virtual void add_ref() const {   add_ref_impl(); }
		virtual void release() const 
		{	 
			if ( release_impl() )
			{
				delete this;
			}
		}
		virtual long release_get_ref_count() const
		{
			if ( release_impl() )
			{
				delete this;	return 0;
			}
			return get_ref_count_impl();
		}

		long		get_ref_count() const { return get_ref_count_impl(); }
		void		swap(my_type& rhs) { swap_impl(rhs); }
	};

	template< class atomic_ref_count_category>
	class ref_count_template< atomic_ref_count_category, dtor_virtual_ref_count_tag> 
		: public ref_count_is_atomic<atomic_ref_count_category>
	{
	public:
		typedef ref_count_template< atomic_ref_count_category, dtor_virtual_ref_count_tag> 	my_type;

		virtual			~ref_count_template() {}
		void			add_ref() const {   add_ref_impl(); }
		void			release() const 
		{	 
			if (release_impl()) 
			{
				delete this; 
			}
		}
		
		long			release_get_ref_count() const
		{
			if ( release_impl() )
			{
				delete this;	return 0;
			}
			return get_ref_count_impl();
		}

		long			get_ref_count() const { return get_ref_count_impl(); }
		void			swap(my_type& rhs) { swap_impl(rhs); }
	};
	
//	
	template< class atomic_ref_count_category>
	class ref_count_template< atomic_ref_count_category, dispose_virtual_ref_count_tag> 
		: public ref_count_is_atomic<atomic_ref_count_category>
	{
	public:
		typedef ref_count_template< atomic_ref_count_category, dispose_virtual_ref_count_tag> 	my_type;
		
		virtual		void	dispose() = 0;				// 가상함수 추가..

		virtual			~ref_count_template() {}
		void			add_ref() const {   add_ref_impl(); }
		void			release()  
		{	 
			if (release_impl()) 
			{
				dispose();
				delete this; 
			}
		}
		
		long			release_get_ref_count() 
		{
			if ( release_impl() )
			{
				dispose();
				delete this;	return 0;
			}
			return get_ref_count_impl();
		}

		long			get_ref_count() const { return get_ref_count_impl(); }
		void			swap(my_type& rhs) { swap_impl(rhs); }
	};
	
//	
	template< class mine, class atomic_ref_count_category >
	class static_ref_count
		: public ref_count_is_atomic<atomic_ref_count_category>
	{
	public:
		typedef static_ref_count< mine, atomic_ref_count_category> 	my_type;

		void			add_ref() const {  add_ref_impl(); }
		void			release() const 
		{  
			if (release_impl() )
			{
				delete static_cast<const mine* const>(this);
			}
		}
		long			release_get_ref_count() const
		{
			if (release_impl() )
			{
				delete static_cast<const mine* const>(this);	return 0;
			}
			return get_ref_count_impl();
		}

		long			get_ref_count() const { return get_ref_count_impl(); }	
		void			swap(my_type& rhs) { swap_impl(rhs); }
	};

	
}