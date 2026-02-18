#pragma once

//#include <stdint.h>
#include "../structure/vec3.h"
#include "../structure/aabb.h"
#include "../structure/plane.h"

#include "i3Base/pool/class_object_pool.h"

namespace i3
{
	// 오클루전 쿼드셋 모음...  아마도 단일하게 하나만 존재하면 될것 같다..

	// 맥스 익스포터의 익스포트과정 혹은 익스포트파일을 임포팅하는 과정수준에서, 
	// 모든 쿼드에 해당할만한 메시들을 모아서, 미리 단일한 정점배열과 인덱스 배열을 쌓아둔다.. (이부분은 기존 i3Engine종속)

	// 단일 정점배열/인덱스배열을 쌓아둘때 어느 정도의 최적화 루틴이 수행되어 정점배열과 인덱스배열의 크기를 줄일수 있으면 줄여야할 것이다..
	
	// 

	// 0. 옥트리로부터 모든 보이는 오브젝트/오클루더를 끄집어옴.. --> 이후부터는 옥트리는 더 쓰지 않음...
	// 1. 오클루더를 끄집어올때 따로 보이는 오클루더 목록에 넣음..
	// 2. 오브젝트를 끄집어올때 보이는 오브젝트 목록에 넣음..

	// 3. 오클루더 목록은 area angle 공식 : f = -A(N*V) / (V*V) 를 토대로 큰값->작은값으로 소팅...
	// 4. 오클루디 목록은 solid_angle 계산식으로 작은값에서 큰값으로 소팅.. (크기가 작게 보이는 놈이 더 잘 컬링됨. 오클루더보다 크다면 그 이후것부터는
	//     모두 렌더링되는 것으로 간주하여 더 이상 계산 안해도 될듯??)

	//     오클루딩 계산시 오클루더의 area angle계산식과 오클루디 solid angle계산식을 비교하여 오클루디쪽이 더 큰값이면..
	//     가리는 것보다 가려지는것이 더 크다고 간주...보일가능성이 높은 것으로 컬링에서 일찍 배제시킨다...

	// 5. 가장 큰 오클루더로 나머지 오클루더 및 보이는 오브젝트들을 컬링 계산한다..
	//   -> 오클루딩된 오클루더와 오브젝트는 목록에서 배제시킨다.. // 처리가 끝났으면 오클루더 목록에서 자기자신을 제외..
		
	// 6. 다음번 오클루더를 가지고 4번을 반복...

	// 현재 i3Engine은 보이는 것들에서 안보이는 것만 따로 예외를 거는 방식이므로, 안보이는 것으로 처리된 것들은 모조리
	// 오브젝트에 마킹을 걸어야 한다...(옥트리프러스텀 컬링 및 오클루전 컬링 양쪽에서 hide된 것들에 대해 마킹 처리)
	// 마킹된 것들은 보일때는 복원되어야 한다...
	// ( 렌더링 직후에 일괄 복원하는 것은 좋지 않다....보이는 시점에서 마킹을 지우는쪽으로 할것)
	// 

	// 오클루전 컬링에 의해 가려진 오브젝트의 경우엔, 오브젝트에 해당 오클루더 포인터나 오클루더 인덱스를 넣어서,
	// 다시 오클루전 컬링 계산 진입시에, 미리 선처리 최적화를 행해본다...  

	// 선처리 컬링 및 인접쿼드컬링 방식 때문에 해당 오클루더 순서가 아니더라도, 미리 오클루더 계산이 이뤄진 케이스가 
	// 있을수 있으므로, 자기 순서가 아닌 오클루더와의 계산이 visible 처리로 끝난 경우 오클루더 인덱스를 따로 넣어..
	// 이미 계산 처리했다(컬링실패임..)는 마킹을 넣어둘 필요가 있다... 이 마킹은 모든 오클루더 계산이 끝나면 리셋되어야 할것임...
	// 인접쿼드나 선처리시 쓰이는 기처리 마킹은 1개이상이 될수 있으므로, vector처리하는게 좋다...(아주 많지는 않을것임)
	// 인접쿼드에 의한 컬링성공시에는 인접쿼드인덱스가 아닌 원래의 쿼드인덱스를 선처리용으로 보관해둔다...

	// 보이는 것과 안보이는 것이 각각 확정된 순간에 그 즉시 오브젝트의 안보임 플래그를 바꾼다. 
	//
	class occ_culler;
	class occ_quad;
	struct occ_object;
	class occ_plane;
	class octree_item;
	class occluder_item_static;
	class quad_adj_collection;

	class I3_EXPORT_MATH occ_mesh_set
	{
	public:
		occ_mesh_set();
		~occ_mesh_set();

		void add_meshes( const i3::uint32_t* index_arr, size_t num_face,
			const vec3* vert_arr, size_t num_vert, bool twoSideEnabled);
		void add_meshes(const vec3* vert_arr, size_t num_vert, bool twoSideEnabled);

	private:
		i3::vector<vec3>		m_vec_vert;
		i3::vector<i3::uint32_t>	m_vec_vert_index;

		friend class occ_culler;
	};

	class I3_EXPORT_MATH occ_quad_set
	{
	public:
		size_t				get_num_vertex() const { return m_vec_vert.size();  }
		const vec3*			get_vertex_arr() const { return &m_vec_vert[0]; }
		size_t				get_num_face() const { return m_quad_idx_list.size() / 4;  }
		const i3::uint32_t*	get_index_arr() const { return &m_quad_idx_list[0]; }


		const i3::uint32_t*	get_quad_indices(size_t faceIdx) const;
		i3::uint32_t			get_quad_index(size_t faceIdx, size_t idx) const;		// idx -> 0,1,2,3 --> 3 == unused32일수 있다.

		bool				is_quad_triangle(size_t faceIdx) const;
		const vec3&			get_quad_vertex(size_t faceIdx, size_t idx) const;		// idx -> 0,1,2,(3)
		
		const i3::aabb&		get_quad_aabb(size_t faceIdx) const;
		const i3::plane&	get_quad_plane(size_t faceIdx) const;
	
		occ_quad*			get_occ_quad(size_t faceIdx) const;

		size_t				get_adj_quad_count(size_t faceIdx, size_t edge_idx) const;
		i3::uint32_t			get_adj_index(size_t faceIdx, size_t edge_idx, size_t idx) const;

		occ_quad*			get_adj_quad(size_t faceIdx, size_t edge_idx, size_t idx) const;
	
		i3::uint32_t			find_adj_shared_edge_idx(size_t faceIdx, size_t edge_idx, size_t idx) const;		// 이웃면에서의 인접선 인덱스를 구한다..
//		bool				find_adj_idx_and_shared_edge_idx(size_t faceIdx, size_t edge_idx, 
//			::uint32_t& out_adj_idx, ::uint32_t& out_shared_edge_idx) const;

		~occ_quad_set();
	private:
		occ_quad_set(occ_culler* culler, i3::vector<vec3>& vec_vert_to_move, i3::vector<i3::uint32_t>& quad_idx_list_to_move,
			quad_adj_collection* collection, i3::vector<i3::aabb>& quad_aabb_list_to_move,
			i3::vector<i3::plane>& quad_plane_list_to_move);
	
		i3::vector<vec3>		m_vec_vert;
		i3::vector<i3::uint32_t>	m_quad_idx_list;
		
		quad_adj_collection*	m_quad_adj_collection;

		i3::vector<i3::aabb>	m_quad_aabb_list;
		i3::vector<i3::plane>	m_quad_plane_list;		// 쿼드 평면...(평면노멀은 단위벡터)
	
		i3::vector<occ_quad*>	m_vec_occ_quad;			// 쿼드모음..동적할당및 소거가 이루어져야함..

	//	friend class occ_mesh_set;
		friend class occ_culler;
	};

	class I3_EXPORT_MATH occ_quad : public i3::class_object_pool<occ_quad>
	{
	public:
		~occ_quad();
		bool				is_quad_triangle() const;
		const i3::uint32_t*	get_quad_vertex_indices() const;
		i3::uint32_t		get_quad_vertex_index(size_t idx) const;		// 0,1,2,3
		const vec3&			get_quad_vertex_pos(size_t idx) const;			// 0,1,2,(3)   네번째것이 존재안할수 있으므로 주의..
		
		const vec3*			get_quad_set_vertex_arr() const;
		size_t				get_quad_set_num_vertex() const;

		const i3::aabb&		get_quad_aabb() const;
		const i3::plane&	get_quad_plane() const;
	
		size_t				get_adj_quad_count(size_t edge_idx) const;
		occ_quad*			get_adjacent_quad(size_t edge_idx, size_t idx) const;	// 0-1,1-2,2-3,3-0 or 0-1,1-2,2-0,unused32..
		i3::uint32_t			get_adjacent_index(size_t edge_idx, size_t idx) const;

		i3::uint32_t			find_adj_shared_edge_idx(size_t edge_idx, size_t idx) const;		// 이웃면에서의 인접선 인덱스를 구한다..

		i3::uint32_t			get_face_index() const { return i3::uint32_t(m_quad_face); }

//		bool find_adj_idx_and_shared_edge_idx(size_t edge_idx, ::uint32_t& out_adj_idx, ::uint32_t& out_shared_edge_idx) const;

		occluder_item_static*	get_octree_item() const { return m_octree_item;  }

		void				bind_occ_object(occ_object* obj) { m_bound_occ_object = obj;  }
		occ_object*			get_bound_occ_object() const { return m_bound_occ_object; }

		occ_culler*			get_culler() const { return m_culler;  }
	private:
		occ_quad(occ_culler* culler, const occ_quad_set* quad_set, size_t quad_face);

		occ_culler*				m_culler;				// 일종의 매니저같은 것임..
		const occ_quad_set*		m_quad_set;
		size_t					m_quad_face;

		occluder_item_static*	m_octree_item;
		
		occ_object*				m_bound_occ_object;

		friend class occ_quad_set;
	};

	
}
