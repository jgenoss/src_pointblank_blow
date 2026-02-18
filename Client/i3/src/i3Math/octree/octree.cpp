#include "i3MathType.h"

//#if 0
#include "octree/octree_constant.h"
#include "octree/octree.h"

#include "octree/word_aabb.h"
#include "octree/octree_item.h"
#include "octree/octree_node.h"
#include "structure/intersect_side.h"
#include "structure/frustum_intersection.h"

#pragma push_macro("min")
#undef min

#pragma push_macro("max")
#undef max

namespace i3
{
	// 배틀진입시 배경의 전체 월드 사이즈를 생성자 인수로..(따라서, 동적생성이 좋을 것임)
	octree::octree(const aabb& a) : m_aabb_orig(a), m_aabb_ext(a), m_root(new octree_node), m_ext(new octree_node),
		m_tot_static_item_count(0), m_tot_dynamic_item_count(0)
	{
		i3::calc_world_aabb_scale_offset(m_aabb_orig, m_scale, m_offset);
	}

	octree::~octree()
	{
		delete m_ext;
		delete m_root;
	}


	// 스태틱은 dynamic보다 더 깊숙이 넣는다..( 먼저 들어갈수 있는 곳부터 구하고 다시 중복해서 넣기)
	void octree::add_item_static(octree_item_static* item)
	{
		if (!item->has_aabb())
			return;

		if (item->get_nodes().empty() == false)		// 기존에 존재하면 삭제부터 처리한다.
		{
			octree::remove_item_static(item);
		}

		word_aabb a;

		if (i3::convert_aabb_to_word_aabb(item->get_aabb(), m_scale, m_offset, a) )
		{
			item->set_word_aabb(a);
			i3::octree_lv_info lvInfo = i3::calc_lv_info(a);
			octree_node* node = octree::_obtain_node(lvInfo);		// 들어갈수 있는 말단을 구한 이후 더 깊이 넣는다..
			octree::_insert_item_deep(item, node);
		}
		else
		{
			item->reset_word_aabb();
			m_ext->add_item(item);	// static의 경우 owner가 여러개 들어갈수 있다.
			m_aabb_ext.merge(item->get_aabb());
		}

		++m_tot_static_item_count;
	}

	void octree::add_item_dynamic(octree_item_dynamic* item)
	{
		if (!item->has_aabb())
			return;

		if (item->get_node())	// 노드 존재시엔 업데이트로 대신 처리하도록 해줌...
		{
			octree::update_item_dynamic(item);	
			return;
		}
		
		word_aabb		a;		// word_aabb안에 lv_info 내장..
		
		if ( i3::convert_aabb_to_word_aabb(item->get_aabb(), m_scale, m_offset, a) )
		{
			item->set_word_aabb(a);
			i3::octree_lv_info lvInfo = i3::calc_lv_info(a);
			octree_node* node = octree::_obtain_node(lvInfo);
			node->add_item(item);
		}
		else	// false인 경우 월드바운드보다도 외각에 존재..
		{
			item->reset_word_aabb();
			m_ext->add_item(item);		// 외각노드는 자식이 없음..
			m_aabb_ext.merge(item->get_aabb());
		}

		++m_tot_dynamic_item_count;
	}

	bool octree::remove_item_dynamic( octree_item_dynamic* item)
	{
		octree_node* node = item->get_node();
		if (!node)
			return false;
		node->_remove_item(item);
		item->set_node(nullptr);
		--m_tot_dynamic_item_count;
		return true;
	}

	bool octree::remove_item_static(octree_item_static* item)		// 이쪽은 삭제가 dynamic보다 느리게 처리된다..
	{
		const i3::vector<octree_node*>& vec_node = item->get_nodes();
		if (vec_node.empty())
			return false;
		const size_t num_node = vec_node.size();
		for (size_t i = 0 ; i < num_node; ++i)
			vec_node[i]->_remove_item(item);
		
		item->clear_nodes();

		++m_tot_static_item_count;
		return true;
	}


	octree_node*	octree::_obtain_node(const octree_lv_info& info)
	{
		DWORD dwLevel = (DWORD)info.lv;	DWORD dwX = (DWORD)info.x;	
		DWORD dwY = (DWORD)info.y;	DWORD dwZ = (DWORD)info.z;			// dwX,Y,Z는 0 아니면 1이다.
		
		octree_node* node = m_root;
		octree_node* child_node;

		DWORD branch_bit;
		DWORD child_index;

		while ( dwLevel != 0 )
		{
			--dwLevel;				// 만약 16레벨(맥스치 - 제일 작은 오브젝트 크기  )라면 x,y,z 성분별로 16개의 비트 불값을 모두 가져온다..
			branch_bit = (1 << dwLevel);
			child_index =	( (dwX & branch_bit) >> dwLevel) + 
							( (dwY & branch_bit) >> dwLevel) * 2 + 
							( (dwZ & branch_bit) >> dwLevel) * 4;		// 루트부터 (0~7)의 인덱스로 변경됨..
			
			if ( !node->has_child() )
				node->_create_children();			// 옥트리 특성으로 일단 8개를 모두 생성하게 된다...
			
			child_node = node->get_child(child_index);

			if ( child_node->is_empty_leaf_node() )	// 해당 자식노드가 아직 활성화안된 것이면 부모노드에 활성화카운트를 올림..
				node->_inc_active_child_count();		// 

			node = child_node;
		}
		
		
		return node;		// 마지막 노드도 active카운팅되었으므로 이후 아이템이 add되길 기대해야함..
	}

	void octree::update_item_dynamic(octree_item_dynamic* item)
	{
		word_aabb a;

		if (i3::convert_aabb_to_word_aabb(item->get_aabb(), m_scale, m_offset, a) )
		{
			if (a == item->get_word_aabb())		// 65536해상도 기준 이동상 변경 사항이 없음.
				return;
			
			item->set_word_aabb(a);
			
			i3::octree_lv_info lvInfo = i3::calc_lv_info(a);

			octree_node* old_node = item->get_node();
			
			if ( old_node )
			{
			
				if (lvInfo == old_node->get_lv_info() )		// 같으면 아직 같은 노드 수준임...
					return;
			
				old_node->_remove_item_lazy(item);
				item->set_node(nullptr);

				octree_node* new_node = octree::_obtain_node(lvInfo);
				new_node->add_item(item);

				old_node->_commit_remove_item_lazy();
			}
			else
			{
				octree_node* new_node = octree::_obtain_node(lvInfo);
				new_node->add_item(item);
				++m_tot_dynamic_item_count;
			}
		}
		else
		{
			octree_node* old_node = item->get_node();
			
			m_aabb_ext.merge(item->get_aabb());		// 이건 값 변동파악이 어려워서 실시간 갱신이 요구될수 있다...

			if (old_node)
			{
				if ( old_node == m_ext )		// 이미 외각노드라면...스킵..
					return;

				item->reset_word_aabb();
				old_node->_remove_item(item);
				item->set_node(nullptr);
				
				m_ext->add_item(item);
			}
			else
			{
				item->reset_word_aabb();
				m_ext->add_item(item);
				++m_tot_dynamic_item_count;
			}
		}
	}



	void octree::_obtain_node_deep_rec(octree_item_static* item, const word_aabb& partial_obj_aabb,
		octree_node* node, const octree::word_pos& node_pos)
	{
		if (node->m_lv_info.lv == MAX_OCTREE_DEPTH )		// 더 깊이 넣을수가 없을 것이다..최하단 리프..여기에 그냥 넣는다.
		{
			node->add_item(item);
			return;
		}

		DWORD currNodeLen = (65536 >> node->m_lv_info.lv);
		DWORD currNodeHalfLen = currNodeLen >> 1;

		DWORD dwLenX = partial_obj_aabb.x1 - partial_obj_aabb.x0 + 1;
		DWORD dwLenY = partial_obj_aabb.y1 - partial_obj_aabb.y0 + 1;
		DWORD dwLenZ = partial_obj_aabb.z1 - partial_obj_aabb.z0 + 1;

		if (dwLenX > currNodeHalfLen || dwLenY > currNodeHalfLen || dwLenZ > currNodeHalfLen)	// 각축별로 모두 반길이를 넘어서면 
		{																						// 매우 큰것으로 간주하고 포기한다..
			node->add_item(item);
			return;
		}

		// 그렇지 않다면, item_aabb를 8등분해봅니다... (item_aabb)...
		DWORD xPatt = DWORD(partial_obj_aabb.x0 ^ partial_obj_aabb.x1);
		DWORD yPatt = DWORD(partial_obj_aabb.y0 ^ partial_obj_aabb.y1);
		DWORD zPatt = DWORD(partial_obj_aabb.z0 ^ partial_obj_aabb.z1);

		struct DwordRange { DwordRange() : pos0(0), pos1(0) {} DWORD pos0, pos1; };

		DwordRange		aadwRange[3][2];	// 축별로 left, right 하나씩...

		if (xPatt & currNodeHalfLen)		// 존재하면 중간경계에 걸리는 것임.. (쪼갠다..)
		{
			aadwRange[0][0].pos0 = partial_obj_aabb.x0;
			aadwRange[0][0].pos1 = node_pos.x + currNodeHalfLen;
			aadwRange[0][1].pos0 = node_pos.x + currNodeHalfLen;
			aadwRange[0][1].pos1 = partial_obj_aabb.x1 + 1;
		}
		else
			if (partial_obj_aabb.x1 < node_pos.x + currNodeHalfLen)		// 왼쪽에만 존재..
			{
				aadwRange[0][0].pos0 = partial_obj_aabb.x0;
				aadwRange[0][0].pos1 = partial_obj_aabb.x1 + 1;
			}
			else												// 그렇지 않으면 오른쪽에만 존재..
			{
				aadwRange[0][1].pos0 = partial_obj_aabb.x0;
				aadwRange[0][1].pos1 = partial_obj_aabb.x1 + 1;
			}

		if (yPatt & currNodeHalfLen)		// 존재하면 중간경계에 걸리는 것임.. (쪼갠다..)
		{
			aadwRange[1][0].pos0 = partial_obj_aabb.y0;
			aadwRange[1][0].pos1 = node_pos.y + currNodeHalfLen;
			aadwRange[1][1].pos0 = node_pos.y + currNodeHalfLen;
			aadwRange[1][1].pos1 = partial_obj_aabb.y1 + 1;
		}
		else
			if (partial_obj_aabb.y1 < node_pos.y + currNodeHalfLen)		// 아랫쪽에만 존재..
			{
				aadwRange[1][0].pos0 = partial_obj_aabb.y0;
				aadwRange[1][0].pos1 = partial_obj_aabb.y1 + 1;
			}
			else												// 그렇지 않으면 윗쪽에만 존재..
			{
				aadwRange[1][1].pos0 = partial_obj_aabb.y0;
				aadwRange[1][1].pos1 = partial_obj_aabb.y1 + 1;
			}

		if (zPatt & currNodeHalfLen)		// 존재하면 중간경계에 걸리는 것임.. (쪼갠다..)
		{
			aadwRange[2][0].pos0 = partial_obj_aabb.z0;
			aadwRange[2][0].pos1 = node_pos.z + currNodeHalfLen;
			aadwRange[2][1].pos0 = node_pos.z + currNodeHalfLen;
			aadwRange[2][1].pos1 = partial_obj_aabb.z1 + 1;
		}
		else
			if (partial_obj_aabb.z1 < node_pos.z + currNodeHalfLen)		// 앞쪽에만 존재..
			{
				aadwRange[2][0].pos0 = partial_obj_aabb.z0;
				aadwRange[2][0].pos1 = partial_obj_aabb.z1 + 1;
			}
			else												// 그렇지 않으면 뒷 쪽에만 존재..
			{
				aadwRange[2][1].pos0 = partial_obj_aabb.z0;
				aadwRange[2][1].pos1 = partial_obj_aabb.z1 + 1;
			}

		if (!node->has_child())
			node->_create_children();			// 옥트리 특성으로 일단 8개를 모두 생성하게 된다...

												// z,y,x순으로 3중 루프 처리.. 

		word_pos child_pos;

		for (int i = 0; i < 2; ++i)
		{
			const DwordRange* zRange = &aadwRange[2][i];

			if (zRange->pos0 == 0 && zRange->pos1 == 0)			// 이 경우 오브젝트가 들어있지 않은 공간임..
				continue;

			child_pos.z = node_pos.z + WORD(i * currNodeHalfLen);

			for (int j = 0; j < 2; ++j)
			{
				const DwordRange* yRange = &aadwRange[1][j];

				if (yRange->pos0 == 0 && yRange->pos1 == 0)		// 이 경우 오브젝트가 들어있지 않은 공간임..
					continue;

				child_pos.y = node_pos.y + WORD(j * currNodeHalfLen);

				for (int k = 0; k < 2; ++k)
				{
					const DwordRange* xRange = &aadwRange[0][k];

					if (xRange->pos0 == 0 && xRange->pos1 == 0)		// 이 경우 오브젝트가 들어있지 않은 공간임..
						continue;

					child_pos.x = node_pos.x + WORD(k * currNodeHalfLen);

					word_aabb partial_child_aabb;
					partial_child_aabb.x0 = WORD(xRange->pos0);
					partial_child_aabb.y0 = WORD(yRange->pos0);
					partial_child_aabb.z0 = WORD(zRange->pos0);
					partial_child_aabb.x1 = WORD(xRange->pos1 - 1);
					partial_child_aabb.y1 = WORD(yRange->pos1 - 1);
					partial_child_aabb.z1 = WORD(zRange->pos1 - 1);

					octree_node* child_node = node->get_child(i * 4 + j * 2 + k);

					if (child_node->is_empty_leaf_node())	// 해당 자식노드가 아직 활성화안된 것이면 부모노드에 활성화카운트를 올림..
						node->_inc_active_child_count();	// 틀림없이 포함됨..

					_obtain_node_deep_rec(item, partial_child_aabb, child_node, child_pos);
				}
			}
		}
		
	}
	
	void octree::_find_word_pos(const octree_lv_info& info, octree::word_pos& out_pos)
	{
		DWORD dwLevel = (DWORD)info.lv;	DWORD dwX = (DWORD)info.x;	
		DWORD dwY = (DWORD)info.y;	DWORD dwZ = (DWORD)info.z;			// dwX,Y,Z는 0 아니면 1이다.

		DWORD branch_bit;

		out_pos.x = 0;	out_pos.y = 0;	out_pos.z = 0;

		WORD half_length = WORD(65536>>1);

		while ( dwLevel != 0 )
		{
			--dwLevel;				// 만약 16레벨(맥스치 - 제일 작은 오브젝트 크기  )라면 x,y,z 성분별로 16개의 비트 불값을 모두 가져온다..
			
			branch_bit = (1 << dwLevel);

			if (dwX & branch_bit)
				out_pos.x += half_length;
			if (dwY & branch_bit)
				out_pos.y += half_length;
			if (dwZ & branch_bit)
				out_pos.z += half_length;

			half_length >>= 1;
		}
	}

	void		octree::_insert_item_deep(octree_item_static* item, octree_node* start_node)
	{
		word_pos wPos;
		_find_word_pos(start_node->get_lv_info(), wPos);
		_obtain_node_deep_rec( item, item->get_word_aabb(), start_node, wPos);
	}

	void octree::cull_frustum(const frustum_plane& fr) // = octree_test_visible
	{
	
		// 프러스텀이 ext노드와 겹치면 외각노드를 처리해주어야한다.
		unsigned int flag_frustum = 0x3F;			// 프러스텀 플래그는 모두 켜놓은(6면이 모두 테스트대상)상태로 진행..

		if (m_ext->has_item())
		{
			if (octree::_cull_frustum_external_node(fr, flag_frustum) == false)
				return;
		}

		if (m_root->is_empty_leaf_node() == false)
		{
			m_root->cull_frustum(fr, m_aabb_orig, flag_frustum);
		}
	}
		
	bool octree::_cull_frustum_external_node(const frustum_plane& fr, unsigned int& inout_flag)
	{
		unsigned int coherency = m_ext->get_plane_coherency();

		intersect_side result = i3::intersect::frustum_aabb_branch(fr, m_aabb_ext, inout_flag, coherency);

		if (result == intersect_side_negative)
		{
			m_ext->set_plane_coherency(coherency);


			m_ext->_invoke_on_hidden_callback();

			if (m_root->is_empty_leaf_node() == false)
				m_root->_invoke_on_hidden_callback_rec();

			return false;							// 아무것도 하지 않음...
		}

		if (result == intersect_side_positive)		// 모든 노드를 순회해 모든 아이템을 얻으면 끝남...
		{
			m_ext->_invoke_on_visible_callback();

			if (m_root->is_empty_leaf_node() == false)
				m_root->_invoke_on_visible_callback_rec();

			return false;
		}

		// 인터섹트한 상황이라면....(아마 대부분...) m_ext노드를 전부 조사하는 함수..(콜백으로 넘어가야함)
		m_ext->_check_invoke_callback(fr, inout_flag);
		return true;		// m_root는 이 함수 아닌 이후에서 처리된다...
	}

	void octree::add_item(octree_item* item)
	{
		item->_add_item_to_octree_(this);			// 디스패칭..
	}

	void octree::remove_item(octree_item* item)
	{
		item->_remove_item_to_octree_(this);			// 디스패칭..
	}

	void octree::update_item(octree_item* item)
	{
		item->_update_item_to_octree_(this);			// 디스패칭..
	}
	
}

#pragma pop_macro("max")
#pragma pop_macro("min")

//#endif