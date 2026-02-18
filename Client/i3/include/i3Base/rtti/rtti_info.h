#pragma once

#include "get_rtti_tag.h"

//#include "../pool/class_common_pool.h"

namespace i3
{
	// class_info는 타입별 스태틱한 변수. 내부 데이터자체보다는 스태틱한 this포인터에 무게중심이 있음..

			
	class rtti_info : private noncopyable //: public i3::class_common_pool<rtti_info, noncopyable>
	{
	public:
		template<class B> rtti_info( i3::identity<B> );
		bool		kind_of(const rtti_info* static_info) const 
		{ 
			const std::size_t rhs_layer = static_info->m_layer_list.size() - 1;
			return ( rhs_layer + 1 > m_layer_list.size() ) ? false : (m_layer_list[rhs_layer] == static_info); 
		}
		bool		same_of(const rtti_info* static_info) const { return this == static_info; }
	private:

		template<class B> void		_create(i3::identity<B> , rtti_available_tag);
		template<class B> void		_create(i3::identity<B> , class_meta_available_tag);
		template<class B> void		_create(i3::identity<B> , rtti_na_tag);

		i3::vector<rtti_info*>	m_layer_list;	// kind_of 타입비교 기능 증진을 위해 배열로 둔다..마지막에 자기 자신을 넣을것이므로 size-1가 곧 layer_index가 됨..
	};


	template< class B> 
	rtti_info::rtti_info( i3::identity<B> t)
	{
		typedef typename get_rtti_tag<B>::type		rtti_tag;
		_create(t, rtti_tag() );
	}

	template<class B> void		rtti_info::_create(i3::identity<B> , rtti_available_tag)
	{
		m_layer_list = B::static_rtti()->m_layer_list;	
		m_layer_list.push_back(this);  
		if (m_layer_list.size() != m_layer_list.capacity())
			i3::vector<rtti_info*>(m_layer_list).swap(m_layer_list); 
	}

	template<class B> void		rtti_info::_create(i3::identity<B> , class_meta_available_tag)
	{
		m_layer_list = B::static_meta()->m_rtti_info.m_layer_list;	
		m_layer_list.push_back(this);  
		if (m_layer_list.size() != m_layer_list.capacity())
			i3::vector<rtti_info*>(m_layer_list).swap(m_layer_list); 
	}

	template<class B> void		rtti_info::_create(i3::identity<B> , rtti_na_tag)
	{
		m_layer_list.push_back(this);
	}
	
}