#pragma once

//#if 0
#include "i3Base/itl/intrusive/smart_list.h"
#include "word_aabb.h"

namespace i3
{
	class aabb;
	class octree_node;
	class frustum_plane;
	class octree_frustum_callback;
	class occ_quad;
	class octree;

	// octree_item은 단독생성되지 않도록 신경쓸 것..
	class I3_EXPORT_MATH octree_item
	{
	public:
		octree_item() : m_aabb(nullptr), m_frustumplane_coherency(0), m_occ_quad_coherency(nullptr) {}

		void				bind_aabb(const aabb* a) { m_aabb = a; }
		bool				has_aabb() const { return m_aabb != nullptr;  }

		virtual bool		is_in_octree() const = 0;

		virtual void		on_hidden_frustum_aabb() = 0;
		virtual void		on_fully_visible_frustum_aabb() = 0;
		virtual void		on_partially_visible_frustum_aabb(const frustum_plane& fr, unsigned int flag_frustum) = 0;

		virtual void		on_hidden_occluder_aabb() = 0;
		virtual void		on_visible_occluder_aabb() = 0;

		const aabb&			get_aabb() const { return *m_aabb; }
		const word_aabb&	get_word_aabb() const { return m_word_aabb; }
	private:

		void				set_word_aabb(const word_aabb& a) { m_word_aabb = a; }
		void				reset_word_aabb() { m_word_aabb = 0; }

		unsigned int&		get_frusutm_coherency_ref() { return m_frustumplane_coherency; }

		const occ_quad*		get_occ_quad_coherency() const { return m_occ_quad_coherency;  }
		void				set_occ_quad_coherency(const occ_quad* oq) { m_occ_quad_coherency = oq;  }

		virtual void		_add_item_to_octree_(i3::octree* o) = 0;
		virtual void		_remove_item_to_octree_(i3::octree* o) = 0;
		virtual void		_update_item_to_octree_(i3::octree* o) = 0;

		word_aabb			m_word_aabb;					// 16바이트..(초기 월드 크기 내부 유효범위에서만 작동된다)
		const aabb*			m_aabb;							// i3BoundBox의 첫두멤버와 메모리공간이 일치하므로, reinterpret_cast로 넣어둔다..
		unsigned int		m_frustumplane_coherency;		// 프러스텀 면 캐시 인덱스..(0~5)
		const occ_quad*		m_occ_quad_coherency;			// 가장 마지막에 가리게 한 occ_quad를 캐싱..(occ_quad가 삭제없이 유지되어야함)

		friend class octree_node;
		friend class octree;
		friend class occ_quad;
		friend class occ_culler;
		friend class octree_item_dynamic;
		friend class octree_item_static;
	};

	// 
	class I3_EXPORT_MATH octree_item_static : public octree_item
	{
	public:
		octree_item_static();
		const i3::vector<octree_node*>& get_nodes() const { return m_vec_owner; }

		virtual bool	is_in_octree() const { return !m_vec_owner.empty(); }

	private:
		void add_node(octree_node* n);
		void clear_nodes();
		void cull_frustum(const frustum_plane& frustum, unsigned int flag_fr);

		void						add_node_fully_visible_count();	// owner 중 하나가 full visible이라도, 다른 owner는 hidden, partially일수 있다..
		void						add_node_hidden_count();
	
		virtual void		_add_item_to_octree_(i3::octree* o);// final;
		virtual void		_remove_item_to_octree_(i3::octree* o);// final;
		virtual void		_update_item_to_octree_(i3::octree* o);// final;

		i3::vector<octree_node*>	m_vec_owner;		// 오브젝트가 속한 부모노드들 집합(부모가 복수)
		i3::uint32_t					m_node_fully_visible_count;	// 비지블 2 + 히든 카운트 = m_vec_owner.size()가 되도록 조절..(이렇게하면 리셋과정을 없앨수 있음)
		i3::uint32_t					m_node_hidden_count;
		i3::uint32_t					m_object_check_count;
	
		friend class octree_node;
		friend class octree;
	};

	class I3_EXPORT_MATH octree_item_dynamic : public octree_item
	{
	public:
		octree_item_dynamic();

		i3::intrusive::raw_list_member_hook<octree_item_dynamic>	m_intrusive_list_hook;

		virtual bool	is_in_octree() const { return m_owner != nullptr;  }

		octree_node* get_node() const { return m_owner; }
	private:

		void set_node(octree_node* n) {  m_owner = n; }		
		void cull_frustum(const frustum_plane& frustum,	unsigned int flag_fr);

		virtual void		_add_item_to_octree_(i3::octree* o);// final;
		virtual void		_remove_item_to_octree_(i3::octree* o);// final;
		virtual void		_update_item_to_octree_(i3::octree* o);// final;

		octree_node*		m_owner;				// 단일 노드 부모...

		friend class octree;
		friend class octree_node;
	};

	
	
}

//#endif
