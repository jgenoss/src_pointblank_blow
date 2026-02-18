#pragma once

//#if 0
#include "../structure/aabb.h"
#include "octree_lv_info.h"
#include "octree_test_option.h"

namespace i3
{
	class octree_item;
	class octree_item_static;
	class octree_item_dynamic;
	class octree_node;
	class frustum_plane;
	struct word_aabb;

	class I3_EXPORT_MATH octree
	{
	public:
		octree(const aabb& a );			// 배틀진입시 배경의 전체 월드 사이즈를 생성자 인수로..(따라서, 동적생성이 좋을 것임)
		~octree();

		const aabb& get_octree_aabb() const { return m_aabb_orig; }
		const aabb& get_octree_aabb_ext() const { return m_aabb_ext; }

		void add_item_static(octree_item_static* item);			
		void add_item_dynamic(octree_item_dynamic* item);		
		
		bool remove_item_dynamic( octree_item_dynamic* item);
		bool remove_item_static(octree_item_static* item);				// 이쪽은 삭제가 dynamic보다 느리게 처리된다..

		void update_item_dynamic(octree_item_dynamic* item);			// 특정아이템(캐릭?) 이동시 노드갱신 반영계산..
																		// static은 dynamic이동 처리를 하지 말것..
		void cull_frustum(const frustum_plane& fr);

		void add_item(octree_item* item);
		void remove_item(octree_item* item);
		void update_item(octree_item* item);

		int	get_curr_static_item_count() const { return m_tot_static_item_count; }
		int get_curr_dynamic_item_count() const { return m_tot_dynamic_item_count; }
			
	private:
		
		octree_node* _obtain_node(const octree_lv_info& info);
		void _insert_item_deep(octree_item_static* item, octree_node* start_node);

		bool _cull_frustum_external_node(const frustum_plane& fr, unsigned int& inout_flag);

		struct word_pos { WORD x, y, z; };

		static void _obtain_node_deep_rec(octree_item_static* item, const word_aabb& partial_obj_aabb,
										octree_node* node, const octree::word_pos& node_pos);
		static void _find_word_pos(const octree_lv_info& info, octree::word_pos& out_pos);

		octree_node*		m_root;		
		octree_node*		m_ext;				// 외각노드..

		aabb				m_aabb_orig;		// 
		aabb				m_aabb_ext;			// 외각노드에 대한 AABB..

		vec3				m_scale;			// 
		vec3				m_offset;			// 

		int					m_tot_static_item_count;		// 통계용..
		int					m_tot_dynamic_item_count;		// 통계용..
	};

}

//#endif
