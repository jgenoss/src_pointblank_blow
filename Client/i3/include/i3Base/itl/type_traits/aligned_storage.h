#pragma once

#include "alignment_of.h"
#include "type_with_alignment.h"
#include "is_pod.h"
#include "../mpl/mpl_if.h"
#include "../mpl/mpl_identity.h"

namespace i3
{

	static const size_t alignment_of_max_align = alignment_of<max_align>::value;
	
	namespace detail
	{
		template<std::size_t s, std::size_t a>
		struct aligned_storage_impl
		{
			union union_data
			{
				char buf[s];
				typename mpl::eval_if_c< a == std::size_t(-1),	mpl::identity<max_align>, 
															type_with_alignment<a> >::type aligned_data;
			} data;
			__forceinline void* address() const { return const_cast<aligned_storage_impl*>(this); }
		};

		template<size_t a>
		struct aligned_storage_impl<0u, a> {	__forceinline void* address() const { return 0; } };
	}
	
	template<std::size_t s, size_t a = std::size_t(-1) >
	class aligned_storage : private detail::aligned_storage_impl<s, a> 
	{
	public:
		typedef detail::aligned_storage_impl<s, a>	base_type;
		typedef detail::aligned_storage_impl<s, a>	type;

		static const std::size_t size	  = s;
		static const std::size_t alignment = ( a == std::size_t(-1)) ? alignment_of_max_align : a ;
		
		aligned_storage(){}
		~aligned_storage(){}

		__forceinline void*			address() { return static_cast<base_type*>(this)->address(); }
		__forceinline const void*	address() const { return static_cast<const base_type*>(this)->address(); }

	private:
		aligned_storage(const aligned_storage&);
		aligned_storage& operator=(const aligned_storage&);
	};

	template<size_t s, size_t a>
	struct is_pod< aligned_storage<s, a> > : true_type {};
	
}