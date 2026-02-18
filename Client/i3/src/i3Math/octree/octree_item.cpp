#include "i3MathType.h"

//#if 0

#include "octree/octree_item.h"
#include "structure/frustum_intersection.h"
#include "octree/octree_test_option.h"
#include "octree/octree_callback.h"
#include "octree/octree.h"

namespace i3
{

	void	octree_item_dynamic::cull_frustum(const frustum_plane& fr, unsigned int flag_fr)
	{
		// 각 item에서 좀더 세밀한 충돌체크를 추가할수도 있기 때문에 음수판정함수만으로 할수 없고...
		// 양,음,교차 브랜치 판정 함수로 변경한다...

		intersect_side result_side = i3::intersect::frustum_aabb_branch(fr, get_aabb(),	flag_fr, get_frusutm_coherency_ref() );
			
		switch (result_side)
		{ 
		case intersect_side_positive:
			this->on_fully_visible_frustum_aabb();
		break;

		case intersect_side_negative:
			this->on_hidden_frustum_aabb();
		break;

		case intersect_side_both:
			this->on_partially_visible_frustum_aabb(fr, flag_fr);
		break;
		}
		
		/*
		if (i3::intersect::is_frustum_aabb_leaf_negative_side(fr, *m_aabb, flag_fr, m_frustumplane_coherency))
		{
			this->on_hidden_frustum_aabb();
		}
		else
		{
			this->on_visible_frustum_aabb();
		}
		*/
	}

	void octree_item_static::cull_frustum(const frustum_plane& fr, unsigned int flag_fr)
	{
		const size_t num_owner = m_vec_owner.size();
		i3::uint32_t curr_sum = m_node_fully_visible_count + m_node_hidden_count +
							  m_object_check_count;

		if (curr_sum == num_owner)		// 이 경우엔 리셋한다..
		{
			m_node_fully_visible_count = 0;
			m_node_hidden_count = 0;
			m_object_check_count = 0;
		}
		
		++m_object_check_count;

		if (m_object_check_count == 1)		// 교차시엔 한번만 검사하면 그 이후엔 카운팅만 세면 된다..
		{
			intersect_side result_side = i3::intersect::frustum_aabb_branch(fr, get_aabb(), flag_fr, get_frusutm_coherency_ref());

			// 이 쪽은 오브젝트 검사이므로 여기 도달하면 최종결과임...
			switch (result_side)
			{
			case intersect_side_positive:
				this->on_fully_visible_frustum_aabb();
				break;

			case intersect_side_negative:
				this->on_hidden_frustum_aabb();		
				break;

			case intersect_side_both:
				this->on_partially_visible_frustum_aabb(fr, flag_fr);
				break;
			}
		}

	}

	octree_item_static::octree_item_static() :  m_node_fully_visible_count(0),
												m_node_hidden_count(0),
												m_object_check_count(0)
	{
	}

	void octree_item_static::add_node(octree_node* n)
	{
		m_vec_owner.push_back(n);				// 부모 목록...지울때 다 지워야할 것..
	}
	
	void octree_item_static::clear_nodes()
	{
		m_vec_owner.clear();
	}
	
	void	octree_item_static::add_node_fully_visible_count()	// owner 중 하나가 full visible이라도, 다른 owner는 hidden, partially일수 있다..
	{
		const size_t num_owner = m_vec_owner.size();
		i3::uint32_t curr_sum = m_node_fully_visible_count + m_node_hidden_count + m_object_check_count;

		if (curr_sum == num_owner)		// 리셋시점..첫 노드 업데이트..
		{
			m_node_fully_visible_count = 0;	m_node_hidden_count = 0;	m_object_check_count = 0;		
		}

		++m_node_fully_visible_count;
	
		if (m_node_fully_visible_count == num_owner)	// 마지막 업데이트 시점..
		{
			this->on_fully_visible_frustum_aabb();
		}
	}

	void	octree_item_static::add_node_hidden_count()
	{
		const size_t num_owner = m_vec_owner.size();
		i3::uint32_t curr_sum = m_node_fully_visible_count + m_node_hidden_count + m_object_check_count;

		if (curr_sum == num_owner)	// 리셋시점..첫 노드 업데이트..
		{
			m_node_fully_visible_count = 0;	m_node_hidden_count = 0;	m_object_check_count = 0;
		}

		++m_node_hidden_count;
	
		if (m_node_hidden_count == num_owner)					// 마지막 업데이트 시점...
		{
			this->on_hidden_frustum_aabb();		// 모든 노드상에서 히든 떨어져야 히든이다... 그렇지 않으면 무시된다..
		}
	}
	
	void	octree_item_static::_add_item_to_octree_(i3::octree* o)
	{
		o->add_item_static(this);
	}

	void	octree_item_static::_remove_item_to_octree_(i3::octree* o)
	{
		o->remove_item_static(this);
	}

	void	octree_item_static::_update_item_to_octree_(i3::octree* o)
	{
		// none_impl..
	}

	void	octree_item_dynamic::_add_item_to_octree_(i3::octree* o)
	{
		o->add_item_dynamic(this);
	}

	void	octree_item_dynamic::_remove_item_to_octree_(i3::octree* o)
	{
		o->remove_item_dynamic(this);
	}

	void	octree_item_dynamic::_update_item_to_octree_(i3::octree* o)
	{
		o->update_item_dynamic(this);
	}
	
	octree_item_dynamic::octree_item_dynamic() : m_owner(nullptr)
	{

	}
}

//#endif
