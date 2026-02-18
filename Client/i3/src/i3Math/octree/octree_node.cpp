#include "i3MathType.h"

//#if 0

#include "octree/octree_node.h"
#include "i3Base/itl/range/algorithm/find.h"
#include "structure/aabb.h"
#include "octree/octree_callback.h"
#include "octree/octree_test_option.h"
#include "octree/octree_item.h"
#include "structure/intersect_side.h"
#include "structure/frustum_intersection.h"

#pragma push_macro("min")
#undef min

#pragma push_macro("max")
#undef max

namespace i3
{
	octree_children::octree_children(octree_node* parent)
	{
		octree_lv_info info = parent->get_lv_info();

		++info.lv;										// 차일드 깊이당 레벨이 하나씩 증가된다.
		info.x <<= 1;	info.y <<=1;	info.z <<=1;	// 노드가 작을수록 기존 비트를 왼쪽으로 이동하고 자식비트를 넣게 된다.
		
		m_child_nodes[0].m_parent = parent;	m_child_nodes[0].m_lv_info = info;
		++info.x;
		m_child_nodes[1].m_parent = parent;	m_child_nodes[1].m_lv_info = info;

		info = m_child_nodes[0].m_lv_info;
		++info.y;
		m_child_nodes[2].m_parent = parent;	m_child_nodes[2].m_lv_info = info;
		++info.x;
		m_child_nodes[3].m_parent = parent;	m_child_nodes[3].m_lv_info = info;

		info = m_child_nodes[0].m_lv_info;
		++info.z;
		m_child_nodes[4].m_parent = parent;	m_child_nodes[4].m_lv_info = info;
		++info.x;
		m_child_nodes[5].m_parent = parent;	m_child_nodes[5].m_lv_info = info;

		info = m_child_nodes[4].m_lv_info;
		++info.y;
		m_child_nodes[6].m_parent = parent;	m_child_nodes[6].m_lv_info = info;
		++info.x;
		m_child_nodes[7].m_parent = parent;	m_child_nodes[7].m_lv_info = info;

	}

	octree_node::octree_node() : m_parent(nullptr), m_children(nullptr), m_active_child_count(0),
		m_frustumplane_coherency(0)
	{
		m_lv_info.info64 = 0;
	}	// 임시로 구현 비움..

	octree_node::~octree_node()
	{
		delete m_children;
	}

	void octree_node::_dec_active_child_count()
	{
		if (--m_active_child_count == 0)		// 칠드런클래스 제거한다..
		{
			delete m_children;	m_children = nullptr;
			if (m_itemlist_static.empty() && m_itemlist_dynamic.empty() && m_parent)
			{
				m_parent->_dec_active_child_count();		// 아이템이 없다면 상위로 재귀 제거처리..
			}
		}
	}

	void octree_node::add_item(octree_item_static* item)
	{
		item->add_node(this);
		m_itemlist_static.push_back(item);		// 
	}

	void octree_node::add_item(octree_item_dynamic* item)
	{
		item->set_node(this);
		m_itemlist_dynamic.push_front(item);	// 링크드리스트의 경우는 그냥 앞에 놓음..(딱히 큰 이유는 없음..최신을 먼저 계산..)
	}
	
	void	octree_node::_remove_item(octree_item_dynamic* item)
	{
		m_itemlist_dynamic.erase( m_itemlist_dynamic.value_to_iterator(item) );

		if ( this->is_empty_leaf_node() )
		{
			octree_node* parent = this->get_parent();
			if (parent)
				parent->_dec_active_child_count();
		}
	}

	void	octree_node::_remove_item_lazy(octree_item_dynamic* item)
	{
		m_itemlist_dynamic.erase(m_itemlist_dynamic.value_to_iterator(item));		// 아이템만 삭제해두고 나중에 체크.
	}

	void	octree_node::_commit_remove_item_lazy()
	{
		if (this->is_empty_leaf_node())
		{
			octree_node* parent = this->get_parent();
			if (parent)
				parent->_dec_active_child_count();
		}
	}


	void	octree_node::_remove_item(octree_item_static* item)
	{
		i3::vector<octree_item_static*>::iterator it = i3::range::find(m_itemlist_static, item);
		if ( it != m_itemlist_static.end() )
			m_itemlist_static.erase(it);
		
		if ( this->is_empty_leaf_node() )
		{
			octree_node* parent = this->get_parent();
			if (parent)
				parent->_dec_active_child_count();
		}
	}


	
	void	octree_node::_invoke_on_hidden_callback()
	{
		const size_t num_static_item = m_itemlist_static.size();

		for (size_t i = 0; i < num_static_item; ++i)
			m_itemlist_static[i]->add_node_hidden_count();

		dynamic_item_list_t::iterator end = m_itemlist_dynamic.end();

		for (dynamic_item_list_t::iterator it = m_itemlist_dynamic.begin(); it != end; ++it)
		{
			octree_item_dynamic* item = *it;
			item->on_hidden_frustum_aabb();
		}
	}

	void	octree_node::_invoke_on_hidden_callback_rec()
	{
		if (has_item())
			octree_node::_invoke_on_hidden_callback();

		if (has_child())
		{
			for (size_t i = 0; i < 8; ++i)		// 옥트리자식들..
			{
				octree_node* child_node = octree_node::get_child(i);
				if (child_node->is_empty_leaf_node() == false)
					child_node->_invoke_on_hidden_callback_rec();
			}
		}
	}

	void	octree_node::_invoke_on_visible_callback()
	{
		const size_t num_static_item = m_itemlist_static.size();

		for (size_t i = 0; i < num_static_item; ++i)
			m_itemlist_static[i]->add_node_fully_visible_count();
		
		dynamic_item_list_t::iterator end = m_itemlist_dynamic.end();

		for (dynamic_item_list_t::iterator it = m_itemlist_dynamic.begin(); it != end; ++it)
		{
			octree_item_dynamic* item = *it;
			item->on_fully_visible_frustum_aabb();
		}
	}

	void	octree_node::_invoke_on_visible_callback_rec()
	{
		if (has_item())
			octree_node::_invoke_on_visible_callback();

		if (has_child())
		{
			for (size_t i = 0; i < 8; ++i)		// 옥트리자식들..
			{
				octree_node* child_node = octree_node::get_child(i);
				if (child_node->is_empty_leaf_node() == false)
					child_node->_invoke_on_visible_callback_rec();
			}
		}
	}

	void	octree_node::_check_invoke_callback(const frustum_plane& fr, unsigned int flag_frustum)
	{
		// 각 아이템 별로 멤버함수를 따로 호출하도록 한다...
		const size_t num_static_item = m_itemlist_static.size();

		for (size_t i = 0; i < num_static_item; ++i)
			m_itemlist_static[i]->cull_frustum(fr, flag_frustum);


		dynamic_item_list_t::iterator end = m_itemlist_dynamic.end();

		for (dynamic_item_list_t::iterator it = m_itemlist_dynamic.begin(); it != end; ++it)
			(*it)->cull_frustum(fr, flag_frustum);

	}

	void	octree_node::cull_frustum(const frustum_plane& frustum, const aabb& aabb_mine, unsigned int flag_frustum)
	{
		intersect_side state = i3::intersect::frustum_aabb_branch(frustum, aabb_mine, flag_frustum, m_frustumplane_coherency);

		if (state == intersect_side_negative)
		{
			octree_node::_invoke_on_hidden_callback_rec();			// 리커시브한 전개임..
			return;
		}

		if (state == intersect_side_positive)
		{
			octree_node::_invoke_on_visible_callback_rec();
			return;
		}

		octree_node::_check_invoke_callback_rec(frustum, aabb_mine, flag_frustum);

	}

	void	octree_node::_check_invoke_callback_rec(const frustum_plane& fr, const aabb& aabb_mine, unsigned int flag_frustum)
	{
		if (has_item())
			octree_node::_check_invoke_callback(fr, flag_frustum);

		if (has_child())
		{
			aabb aabb_child;
			vec3 extent_mine = aabb_mine.calc_extent();

			for (size_t i = 0; i < 8; ++i)		// 옥트리자식들..
			{
				octree_node* child_node = octree_node::get_child(i);		// 이경우 항상 존재..

				if (child_node->is_empty_leaf_node() == false)	//액티브한경우만 찾으면 됨...
				{
					child_node->calc_node_aabb(aabb_child, aabb_mine, extent_mine);
					child_node->cull_frustum(fr, aabb_child, flag_frustum);
				}
			}
		}
	}

	void	octree_node::calc_node_aabb(aabb& out, const aabb& aabb_parent, const vec3& extent_parent) const
	{
		out.min(aabb_parent.min());		// 미니멈값을 우선 취하고..
		out.min().x += float(m_lv_info.x & 1) * extent_parent.x;
		out.min().y += float(m_lv_info.y & 1) * extent_parent.y;
		out.min().z += float(m_lv_info.z & 1) * extent_parent.z;
		out.max(out.min());
		out.max() += extent_parent;
	}


	// item목록에 따라 aabb를 확장한다...줄어들지는 않는다..
	void	octree_node::calc_item_list_merged_aabb(aabb& inout_aabb)
	{
		const size_t num_static_item = m_itemlist_static.size();

		for (size_t i = 0; i < num_static_item; ++i)
		{
			const aabb& a = m_itemlist_static[i]->get_aabb();
			inout_aabb.merge(a);
		}

		dynamic_item_list_t::iterator end = m_itemlist_dynamic.end();

		for (dynamic_item_list_t::iterator it = m_itemlist_dynamic.begin(); it != end; ++it)
		{
			const aabb& a = (*it)->get_aabb();
			inout_aabb.merge(a);
		}
	}



}

#pragma pop_macro("max")
#pragma pop_macro("min")

//#endif
