#pragma once

#include "octree_item.h"

namespace i3
{
	class occ_culler;
	
	class I3_EXPORT_MATH occludee_item_dynamic : public i3::octree_item_dynamic
	{
	public:
		occludee_item_dynamic(occ_culler* culler) : m_culler(culler) {}

		virtual void		on_hidden_frustum_aabb() = 0;
		virtual void		on_hidden_occluder_aabb() = 0;
		virtual void		on_visible_occluder_aabb() = 0;
	private:
		virtual void		on_fully_visible_frustum_aabb();// final;
		virtual void		on_partially_visible_frustum_aabb(const frustum_plane& fr, unsigned int flag_frustum);// final;
		occ_culler*			m_culler;
	};

	class occ_quad;

	class I3_EXPORT_MATH occluder_item_static : public i3::octree_item_static
	{
	private:
		occluder_item_static(occ_quad* q) : m_occ_quad(q) {}

		virtual void		on_hidden_frustum_aabb();
		virtual void		on_fully_visible_frustum_aabb();
		virtual void		on_partially_visible_frustum_aabb(const frustum_plane& fr, unsigned int flag_frustum);

		virtual void		on_hidden_occluder_aabb() {}		// 호출되지 않을 것임..
		virtual void		on_visible_occluder_aabb() {}		// 호출되지 않을 것임..

		occ_quad*	m_occ_quad;
		friend class occ_quad;
	};

}