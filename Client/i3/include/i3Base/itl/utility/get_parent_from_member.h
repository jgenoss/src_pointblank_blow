#pragma once

// boost intrusive/detail/parent_from_member Âü°í..
#include "../../atomic/cstdint.h"

namespace i3
{
	
	namespace detail
	{
		template<class Parent, class Member> inline
		std::ptrdiff_t offset_from_pointer_to_member(const Member Parent::* ptr_to_member)
		{
			union caster_union
			{
				const Member Parent::*	ptr_to_member;
				int32_t				offset;
			} caster;

			compile_assert( sizeof(caster) == sizeof(int32_t) );

			caster.ptr_to_member = ptr_to_member;
			return std::ptrdiff_t(caster.offset);
		}
		
	}
	
	template<class Parent, class Member> inline
	Parent* get_parent_from_member(Member* member, const Member Parent::* ptr_to_member)
	{
		return static_cast<Parent*>( static_cast<void*>( 
			static_cast<char*>(static_cast<void*>(member)) - detail::offset_from_pointer_to_member(ptr_to_member)   
			)  );
	}
	
	template<class Parent, class Member> inline
	const Parent* get_parent_from_member(const Member* member, const Member Parent::* ptr_to_member)
	{
		return static_cast<const Parent*>( static_cast<const void*>( 
			static_cast<const char*>(static_cast<const void*>(member)) - detail::offset_from_pointer_to_member(ptr_to_member)   
			)  );
	}

}