#include "i3MathType.h"
#include "octree/occ_quad_set.h"


#include "octree/occ_culler.h"
#include "octree/octree.h"
#include "octree/occ_item.h"

#include "mesh_function/quad_adj_collection.h"

namespace i3
{
	occ_mesh_set::occ_mesh_set()
	{

	}

	occ_mesh_set::~occ_mesh_set()
	{

	}

	void occ_mesh_set::add_meshes(const i3::uint32_t* index_arr, size_t num_face,
		const vec3* vert_arr, size_t num_vert, bool twoSideEnabled)
	{
		size_t vert_idx_start_offset = m_vec_vert.size();

		m_vec_vert.insert(m_vec_vert.end(), vert_arr, vert_arr + num_vert);

		if (twoSideEnabled == false)
		{
			const size_t num_vert_index = num_face * 3;

			const size_t idx_offset = m_vec_vert_index.size();

			m_vec_vert_index.resize(idx_offset + num_vert_index);

			for (size_t i = 0; i < num_vert_index; ++i)
			{
				if (index_arr[i] == UNUSED32)
					m_vec_vert_index[idx_offset + i] = UNUSED32;
				else
					m_vec_vert_index[idx_offset + i] = index_arr[i] + vert_idx_start_offset;
			}
		}
		else
		{
			const size_t num_vert_index = num_face * 2 * 3;
			const size_t idx_offset = m_vec_vert_index.size();

			m_vec_vert_index.resize(idx_offset + num_vert_index);

			const size_t half_face = num_face / 2;
			const size_t half_vert_idx = num_vert_index / 2;

			for (size_t i = 0; i < half_vert_idx; ++i)
			{
				if (index_arr[i] == UNUSED32)
					m_vec_vert_index[idx_offset + i] = UNUSED32;
				else
					m_vec_vert_index[idx_offset + i] = index_arr[i] + vert_idx_start_offset;
			}

			size_t vert_counter = idx_offset + half_vert_idx;

			for (size_t i = 0; i < half_face; ++i, vert_counter += 3)
			{
				size_t offset_vert_idx = idx_offset + i * 3;

				m_vec_vert_index[vert_counter + 0] = m_vec_vert_index[offset_vert_idx + 2];
				m_vec_vert_index[vert_counter + 1] = m_vec_vert_index[offset_vert_idx + 1];
				m_vec_vert_index[vert_counter + 2] = m_vec_vert_index[offset_vert_idx + 0];
			}

		}
	}

	void occ_mesh_set::add_meshes(const vec3* vert_arr, size_t num_vert, bool twoSideEnabled)
	{
		//
		size_t vert_idx_start_offset = m_vec_vert.size();
		//
		m_vec_vert.insert(m_vec_vert.end(), vert_arr, vert_arr + num_vert);

		if (twoSideEnabled == false)
		{
			const size_t num_face = num_vert / 3;
			const size_t num_vert_index = num_face * 3;
					
			const size_t idx_offset = m_vec_vert_index.size();

			m_vec_vert_index.resize(idx_offset + num_vert_index);

			for (size_t i = 0; i < num_vert_index; ++i)
			{
				m_vec_vert_index[idx_offset + i] = i + vert_idx_start_offset;
			}
		}
		else
		{
			const size_t num_face = num_vert * 2 / 3;
			const size_t num_vert_index = num_face * 3;

			const size_t idx_offset = m_vec_vert_index.size();

			m_vec_vert_index.resize(idx_offset + num_vert_index);
			
			const size_t half_face = num_face / 2;
			const size_t half_vert_idx = num_vert_index / 2;

			for (size_t i = 0; i < half_vert_idx; ++i)
			{
				m_vec_vert_index[idx_offset + i] = i + vert_idx_start_offset;
			}

			size_t vert_counter = idx_offset + half_vert_idx;

			for (size_t i = 0; i < half_face; ++i, vert_counter += 3)
			{
				size_t offset_vert = i * 3;
				
				m_vec_vert_index[vert_counter + 0] = offset_vert + 2 + vert_idx_start_offset;
				m_vec_vert_index[vert_counter + 1] = offset_vert + 1 + vert_idx_start_offset;
				m_vec_vert_index[vert_counter + 2] = offset_vert + 0 + vert_idx_start_offset;
			}
		}
	}
	
	occ_quad_set::occ_quad_set(occ_culler* culler, i3::vector<vec3>& vec_vert_to_move, i3::vector<i3::uint32_t>& quad_idx_list_to_move,
		quad_adj_collection* collection, i3::vector<i3::aabb>& quad_aabb_list_to_move,
		i3::vector<i3::plane>& quad_plane_list_to_move)
	{
		m_vec_vert.swap(vec_vert_to_move);
		m_quad_idx_list.swap(quad_idx_list_to_move);
		m_quad_adj_collection = collection;
		m_quad_aabb_list.swap(quad_aabb_list_to_move);
		m_quad_plane_list.swap(quad_plane_list_to_move);
	
		octree* ot = const_cast<octree*>(culler->get_octree());
		const size_t num_quad = m_quad_idx_list.size() / 4;

		for (size_t i = 0; i < num_quad; ++i)
		{
			occ_quad*	new_quad = new occ_quad(culler, this, i);
			m_vec_occ_quad.push_back(new_quad);
			ot->add_item_static(new_quad->get_octree_item());
		}

	}
	
	occ_quad_set::~occ_quad_set()
	{
		i3::safe_delete_quad_adj_collection(m_quad_adj_collection);
		i3::cu::for_each_delete(m_vec_occ_quad);
	}

	const i3::uint32_t*	occ_quad_set::get_quad_indices(size_t faceIdx) const
	{
		return &m_quad_idx_list[faceIdx * 4];
	}

	i3::uint32_t			occ_quad_set::get_quad_index(size_t faceIdx, size_t idx) const		// idx -> 0,1,2,3
	{
		return m_quad_idx_list[faceIdx * 4 + idx];
	}

	bool			occ_quad_set::is_quad_triangle(size_t faceIdx) const
	{
		return m_quad_idx_list[faceIdx * 4 + 3] == UNUSED32;
	}

	const vec3&			occ_quad_set::get_quad_vertex(size_t faceIdx, size_t idx) const		// idx -> 0,1,2,3
	{
		return m_vec_vert[ m_quad_idx_list[faceIdx * 4 + idx] ];			// 삼각형인 경우에 대해서는 외부에서 예외처리할 것..
	}
	
	const i3::aabb&		occ_quad_set::get_quad_aabb(size_t faceIdx) const
	{
		return m_quad_aabb_list[faceIdx];
	}
	
	const i3::plane&	occ_quad_set::get_quad_plane(size_t faceIdx) const
	{
		return m_quad_plane_list[faceIdx];
	}

	occ_quad*			occ_quad_set::get_occ_quad(size_t faceIdx) const
	{
		return m_vec_occ_quad[faceIdx];
	}
	
	occ_quad*			occ_quad_set::get_adj_quad(size_t faceIdx, size_t edge_idx, size_t idx) const
	{
		i3::uint32_t adj_idx = occ_quad_set::get_adj_index(faceIdx, edge_idx, idx);

		return (adj_idx != UNUSED32) ? m_vec_occ_quad[adj_idx] : nullptr;
	}

	size_t			occ_quad_set::get_adj_quad_count(size_t faceIdx, size_t edge_idx) const
	{
		return m_quad_adj_collection->get_adjacent_quad_count(faceIdx, edge_idx);
	}
	
	i3::uint32_t			occ_quad_set::get_adj_index(size_t faceIdx, size_t edge_idx, size_t idx) const
	{
		return m_quad_adj_collection->get_adjacent_quad_face(faceIdx, edge_idx, idx);
	}

	i3::uint32_t		occ_quad_set::find_adj_shared_edge_idx(size_t faceIdx, size_t edge_idx, size_t idx) const		// 이웃면에서의 인접선 인덱스를 구한다..
	{
		return m_quad_adj_collection->get_adjacent_quad_edge(faceIdx, edge_idx, idx);
	}

	/*
	{
		::uint32_t adj_idx = m_quad_adj_list[faceIdx * 4 + edge_idx];
		
		if (adj_idx == UNUSED32) return UNUSED32;

		const ::uint32_t* adj_adj = &m_quad_adj_list[adj_idx * 4];

		for ( ::uint32_t i = 0; i < 4; ++i )
		{
			if (adj_adj[i] == faceIdx)
				return i;
		}
		return UNUSED32;
	}
	*/

	/*
	bool	occ_quad_set::find_adj_idx_and_shared_edge_idx(size_t faceIdx, size_t edge_idx, ::uint32_t& out_adj_idx, ::uint32_t& out_shared_edge_idx) const
	{
		::uint32_t adj_idx = m_quad_adj_list[faceIdx * 4 + edge_idx];
		
		if (adj_idx == UNUSED32)
		{
			out_adj_idx = UNUSED32;
			out_shared_edge_idx = UNUSED32;
			return false;
		}

		const ::uint32_t* adj_adj = &m_quad_adj_list[adj_idx * 4];

		for (::uint32_t i = 0; i < 4; ++i)
		{
			if (adj_adj[i] == faceIdx)
			{
				out_adj_idx = adj_idx;
				out_shared_edge_idx = i;
				return true;
			}
		}

		out_adj_idx = UNUSED32;					// 버그 상태..일단 모두 무효화처리함..
		out_shared_edge_idx = UNUSED32;
		return false;
	}
	*/

	occ_quad::occ_quad(occ_culler* culler, const occ_quad_set* quad_set, size_t quad_face) : m_culler(culler), 
		m_quad_set(quad_set), m_quad_face(quad_face), m_bound_occ_object(nullptr)
	{
		m_octree_item = new occluder_item_static(this);
		m_octree_item->bind_aabb(&this->get_quad_aabb());
	}
		
	occ_quad::~occ_quad()
	{
		delete m_octree_item;
	}

	bool			occ_quad::is_quad_triangle() const
	{
		return m_quad_set->is_quad_triangle(m_quad_face);
	}

	const i3::uint32_t*	occ_quad::get_quad_vertex_indices() const
	{
		return m_quad_set->get_quad_indices(m_quad_face);
	}

	i3::uint32_t		occ_quad::get_quad_vertex_index(size_t i) const
	{
		return m_quad_set->get_quad_index(m_quad_face, i);
	}

	const vec3&		occ_quad::get_quad_vertex_pos(size_t i) const
	{
		return m_quad_set->get_quad_vertex(m_quad_face, i);
	}

	const i3::aabb&		occ_quad::get_quad_aabb() const
	{
		return m_quad_set->get_quad_aabb(m_quad_face);
	}

	const i3::plane&	occ_quad::get_quad_plane() const
	{
		return m_quad_set->get_quad_plane(m_quad_face);
	}

	
	const vec3*		occ_quad::get_quad_set_vertex_arr() const
	{
		return m_quad_set->get_vertex_arr();
	}

	size_t			occ_quad::get_quad_set_num_vertex() const
	{
		return m_quad_set->get_num_vertex();
	}

	size_t			occ_quad::get_adj_quad_count(size_t edge_idx) const
	{
		return m_quad_set->get_adj_quad_count(m_quad_face, edge_idx);
	}

	occ_quad*		occ_quad::get_adjacent_quad(size_t edge_idx, size_t idx) const	// 0-1,1-2,2-3,3-0 or 0-1,1-2,2-0,unused32..
	{
		return m_quad_set->get_adj_quad(m_quad_face, edge_idx, idx);
	}
	
	i3::uint32_t		occ_quad::get_adjacent_index(size_t edge_idx, size_t idx) const
	{
		return m_quad_set->get_adj_index(m_quad_face, edge_idx, idx);
	}
	
	i3::uint32_t		occ_quad::find_adj_shared_edge_idx(size_t edge_idx, size_t idx) const
	{
		return m_quad_set->find_adj_shared_edge_idx(m_quad_face, edge_idx, idx);
	}

	/*
	bool occ_quad::find_adj_idx_and_shared_edge_idx(size_t edge_idx, ::uint32_t& out_adj_idx, ::uint32_t& out_shared_edge_idx) const
	{
		return m_quad_set->find_adj_idx_and_shared_edge_idx(m_quad_face, edge_idx, out_adj_idx, out_shared_edge_idx);
	}
	*/


}

