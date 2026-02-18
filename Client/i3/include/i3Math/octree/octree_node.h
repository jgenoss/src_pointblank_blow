#pragma once

//#if 0
#include "i3Base/itl/intrusive/smart_list.h"
#include "i3Base/pool/class_object_pool.h"

#include "octree_item.h"
#include "octree_lv_info.h"

namespace i3
{
	class octree_childs;
	class octree_item_static;		// 고정된 것..(현재는 fix된 오클루전 쿼드만 고려)
	class octree_item_dynamic;		// 움직이는 것.. (현재는 움직이는 캐릭터만 고려)
	class octree_item;
	class octree_children;
	class frustum_plane;
	
	class octree_node
	{
	public:
		octree_node();	// 임시로 구현 비움..
		~octree_node();	// 임시로 구현 비움..
			
		void add_item(octree_item_static* item);
		void add_item(octree_item_dynamic* item);

		void calc_item_list_merged_aabb(aabb& inout_aabb);

		bool is_empty_leaf_node() const 
		{
			return m_children == nullptr && m_itemlist_static.empty() && m_itemlist_dynamic.empty();
		} 	// 아이템(정적,동적)과 자식노드 모두 없는 경우만 empty...

		bool has_child() const { return m_children != nullptr; }
		bool has_static_item() const { return !m_itemlist_static.empty();  }
		bool has_dynamic_item() const { return !m_itemlist_dynamic.empty();  }
		bool has_item() const { return has_static_item() || has_dynamic_item(); }

		octree_node*	get_child(size_t idx) const;
		octree_node*	get_parent() const { return m_parent; }
		const octree_lv_info& get_lv_info() const { return m_lv_info; }

		void			set_plane_coherency(unsigned int coherency) { m_frustumplane_coherency = coherency; }
		unsigned int	get_plane_coherency() const { return m_frustumplane_coherency; }

		void			cull_frustum(const frustum_plane& frustum, const aabb& aabb_mine, unsigned int flag_frustum);

		void			calc_node_aabb(aabb& out, const aabb& aabb_parent, const vec3& extent_parent) const;
	

	
	private:
		void			_remove_item(octree_item_dynamic* item);
		void			_remove_item(octree_item_static* item);

		void			_remove_item_lazy(octree_item_dynamic* item);
		void			_commit_remove_item_lazy();
	
		void			_create_children();											
		void			_inc_active_child_count() { ++m_active_child_count; }		
		void			_dec_active_child_count();

		void			_invoke_on_hidden_callback();
		void			_invoke_on_hidden_callback_rec();

		void			_invoke_on_visible_callback();
		void			_invoke_on_visible_callback_rec();

		void			_check_invoke_callback(const frustum_plane& fr, unsigned int flag_frustum);
		void			_check_invoke_callback_rec(const frustum_plane& fr, const aabb& aabb_mine, unsigned int flag_frustum);

		
		octree_node*	m_parent;
		octree_children* m_children;
		
		octree_lv_info	m_lv_info;
		DWORD			m_active_child_count;
		unsigned int	m_frustumplane_coherency;
		// 스태틱의 경우, add만 존재하고 개별 remove가 없으므로 vector로 충분할 것임..
		// 대신에 스태틱의 경우 하나의 아이템이 여러개의 노드들에 포함될수 있어야한다...
		i3::vector<octree_item_static*>						m_itemlist_static;		
		// dynamic의 경우, remove가 많을 것이며, 반복자를 통해 빠르게 삭제가 가능해야함..

		typedef i3::intrusive::raw_list<octree_item_dynamic, i3::intrusive::smart_member_hook<octree_item_dynamic,
			i3::intrusive::raw_list_member_hook<octree_item_dynamic>,
			&octree_item_dynamic::m_intrusive_list_hook> > dynamic_item_list_t;
		
		dynamic_item_list_t m_itemlist_dynamic;
			
		friend class octree_children;
		friend class octree;
	};

	
	class octree_children : public i3::class_object_pool<octree_children>
	{
	public:
		octree_children(octree_node* parent);
		octree_node* get_child(size_t idx) { return m_child_nodes + idx; }
	private:
		octree_node  m_child_nodes[8];		// 옥트리의 8.
	};

	inline
	void octree_node::_create_children() 
	{ 
		if (m_children == nullptr)
			m_children = new octree_children(this);
	}
	
	inline
	octree_node*	octree_node::get_child(size_t idx) const { return m_children->get_child(idx); }


}

//#endif