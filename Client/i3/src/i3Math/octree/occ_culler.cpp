#include "i3MathType.h"

#include "octree/occ_culler.h"
#include "octree/occ_item.h"
#include "octree/occ_quad_set.h"

#include "mesh_function/mesh_adjacency.h"
#include "mesh_function/mesh_weld.h"
#include "mesh_function/mesh_convert_quad.h"
#include "mesh_function/quad_adj_collection.h"

#include "octree/octree.h"
#include "octree/octree_item.h"

#include "structure/vec3_function.h"
#include "structure/plane_function.h"
#include "structure/plane_intersection.h"

#include "structure/frustum_plane.h"

#include "octree/occ_plane.h"
#include "structure/intersect_side.h"

#include "i3Base/itl/vector_util.h"
#include "i3Base/itl/algorithm/find.h"

namespace i3
{
	struct occ_float_rect
	{
		float minX, minY;
		float maxX, maxY;
	};

	//
//	struct quad_edge_pair
//	{
//		i3::uint32_t	quad_idx;
//	};
//	bool operator==(const quad_edge_pair& l, i3::uint32_t r)
//	{
//		return l.quad_idx == r;
//	}

	struct occ_object : i3::class_object_pool<occ_object>
	{
		~occ_object();

		i3::vector<i3::uint32_t> m_already_checked_quad_list; // 인접면에 면 존재시에 겹쳐진 놈은 다 등록...(중복검사 제거용이다..)
		i3::vector<occ_object*>	m_intersected_quad_list;	 // 순 오클루디와 오클루더가 교차된 것들만 모아놓는다..도중 삭제가 없음..

		occ_float_rect		m_occ_float_rect;					//
		float				m_projected_area;					// area_angle,solid_angle은 버리고, 투영된 확장aabb면적으로 바꿈...
		octree_item*		m_octree_item;
		occ_quad*			m_occluder_quad;
		occ_plane*			m_cached_occ_plane;					// 동적 생성될수있음.. (소멸자에서 소멸처리해야함..)

		enum object_state { visible, backface_cull, occluded, invalid_aabb };
		object_state		m_object_state;
		int					m_occludee_intersect_count;		// 오클루더인 경우, 오클루디와의 교차횟수를 기록..(증감)
	};

	namespace
	{
		// 삼각형과 사각형 넓이를 구하는 함수 작성
		// http://geomalgorithms.com/a01-_area.html 참고..

		// 0.5 * length(  (v2 - v0 ) ^ (v3 - v1 ) ) 임..

		float calc_quad_area(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3)
		{
			vec3 v02 = v2;		v02 -= v0;
			vec3 v13 = v3;		v13 -= v1;
			vec3 cross_v02_v13;
			i3::vec3_cross(cross_v02_v13, v02, v13);

			return i3::vec3_length(cross_v02_v13) * .5f;
		}

		// 0.5 * length( (v1 - v0 ) ^ ( v2 - v0) ) 임..

		float calc_triangle_area(const vec3& v0, const vec3& v1, const vec3& v2)
		{
			vec3 v01 = v1;		v01 -= v0;
			vec3 v02 = v2;		v02 -= v0;
			vec3 cross_v01_v02;
			i3::vec3_cross(cross_v01_v02, v01, v02);
			return i3::vec3_length(cross_v01_v02) * .5f;
		}

	}

	occ_culler::occ_culler(octree* arg_octree) : m_octree(arg_octree), m_query_callback(nullptr), // 일단은 옥트리 의존적으로 처리..
		m_statistics_callback(nullptr)
	{
		::memset(&m_statistics_info, 0, sizeof(m_statistics_info));
	}


	bool occ_culler::register_occ_quads(const occ_mesh_set* mesh_set)
	{
		

		const i3::vector<vec3>&	vec_vert = mesh_set->m_vec_vert;
		const i3::vector<i3::uint32_t>& vec_vert_index = mesh_set->m_vec_vert_index;

		const size_t num_vert_index = vec_vert_index.size();
		const size_t num_face = num_vert_index / 3;
		const size_t num_vert = vec_vert.size();

		i3::svector<i3::uint32_t> vecPointReps(num_vert);
		i3::svector<i3::uint32_t> vecAdj(num_vert_index);
		// 1. 인접버퍼와 대표정점목록 부터 먼저 구함.

		bool result = i3::generate_adjacency_and_pointreps(&vec_vert_index[0], num_face, &vec_vert[0], num_vert, 0.1f/*1e-5f*/, &vecAdj[0],&vecPointReps[0]);

		if (result == false)
			return false;

		i3::svector<i3::uint32_t> vecVertexRemap(num_vert);
		size_t num_new_vert;

		i3::svector<i3::uint32_t> vecNewVertIdxList(num_vert_index);

		// 2. Weld / RemapVertice를 통해 중복정점 제거된 인덱스 목록과 정점목록을 얻는다..
		result = i3::weld_index_buffer(&vec_vert_index[0], num_face,
			&vec_vert[0], num_vert, &vecNewVertIdxList[0], &vecVertexRemap[0], num_new_vert,
			&vecPointReps[0]);

		if (result == false)
			return false;

		i3::vector<vec3> vec_new_vert(num_new_vert);
		i3::remap_vertices(&vec_vert[0], num_new_vert, &vecVertexRemap[0], &vec_new_vert[0]);
			

		// 3. build_quad_index_list 함수를 통해서 쿼드정점목록과 쿼드인접버퍼를 얻어냄...

		i3::vector<i3::uint32_t>  quad_idx_list;
		
		i3::build_quad_index_list(&vecNewVertIdxList[0], vecNewVertIdxList.size()/3, &vec_new_vert[0], vec_new_vert.size(),
			quad_idx_list, &vecAdj[0]);

		i3::quad_adj_collection* collection = i3::create_quad_adj_collection(&quad_idx_list[0], quad_idx_list.size() / 4,
			&vec_new_vert[0], vec_new_vert.size(), 0.1f, 0.1f);
		

		// 4. 각 쿼드들에 대한 AABB를 구하여, 쿼드 당 목록으로 넣어둠...
		const size_t num_quad = quad_idx_list.size() / 4;

		i3::vector<i3::aabb>	vec_quad_aabb(num_quad);

		for (size_t i = 0; i < num_quad; ++i)
		{
			i3::aabb& this_aabb = vec_quad_aabb[i];

			const i3::uint32_t* quad_arr = &quad_idx_list[i * 4];
			const size_t num_quad_vert = (quad_arr[3] != UNUSED32) ? 4 : 3;	// 4개..

			for (size_t j = 0; j < num_quad_vert; ++j)
			{
				this_aabb.merge(vec_new_vert[quad_arr[j]]);
			}
		}

		// 5. 쿼드 평면식 계산..

		i3::vector<i3::plane>	vec_quad_plane(num_quad);

		for (size_t i = 0; i < num_quad; ++i)
		{
			i3::plane& this_plane = vec_quad_plane[i];

			const i3::uint32_t* quad_arr = &quad_idx_list[i * 4];	// 최소 3개이상은 확보..

			i3::make_plane_from_points(this_plane,
				vec_new_vert[quad_arr[0]],
				vec_new_vert[quad_arr[1]],
				vec_new_vert[quad_arr[2]]);
		}

	
		occ_quad_set* pNewSet = new occ_quad_set(this, vec_new_vert,
			quad_idx_list, collection, vec_quad_aabb, vec_quad_plane);

		m_vec_occ_quad_set.push_back(pNewSet);

		// 다음은 일단 임시적 대응이고, 오클루더와 오클루디의 static/dynamic 여부는 차후 변경될수도 있음..


		m_statistics_info.tot_occluder_count = m_octree->get_curr_static_item_count();
		m_statistics_info.tot_occludee_count = m_octree->get_curr_dynamic_item_count();

		if (m_statistics_callback)
			m_statistics_callback->on_change(m_statistics_info);
		
		return true;
	}

	occ_culler::~occ_culler()
	{
		i3::cu::for_each_delete(m_vec_occ_quad_set);
	}
	
	void occ_culler::_add_octree_visible_quad(occ_quad* quad)
	{
		occ_object* new_occ = new occ_object;

		new_occ->m_projected_area = 0.f;
		new_occ->m_octree_item = quad->get_octree_item();
		new_occ->m_occluder_quad = quad;
		new_occ->m_cached_occ_plane = nullptr;
		new_occ->m_object_state = occ_object::visible;
		new_occ->m_occludee_intersect_count = 0;

		m_vec_occ_object.push_back(new_occ);					// 별로 하는게 없네...등록만...갯수는 꽤나 많을 것이다..
		quad->bind_occ_object(new_occ);

		++m_statistics_info.octree_visible_occluder_count;
	
	}
	
	void occ_culler::_add_octree_visible_item(octree_item* item)
	{
		occ_object* new_occ = new occ_object;
		new_occ->m_projected_area = 0.f;		// 나중에 계산함..
		new_occ->m_octree_item = item;
		new_occ->m_occluder_quad = nullptr;
		new_occ->m_cached_occ_plane = nullptr;
		new_occ->m_object_state = occ_object::visible;
		new_occ->m_occludee_intersect_count = 0;

		m_vec_occ_object.push_back(new_occ);

		++m_statistics_info.octree_visible_occludee_count;
	
	}
		
	occ_object::~occ_object()
	{
		if (m_occluder_quad)
		{
			m_occluder_quad->bind_occ_object(nullptr);
			if (m_cached_occ_plane)						// occ_quad가 아니라면 캐싱될 occ_plane도 없음..
				delete m_cached_occ_plane;
		}
	}
	
	namespace
	{
		void	calcluate_projected_rect_with_aabb_and_view_projection(occ_float_rect& out, const aabb& obj_aabb, 
			const mat4& matView, const mat4& matProj)
		{
			// 월드상의 aabb를 viewMat과 projectMat으로 사상시켜, (-1~ 1) 사이의 값을 지닌 2디 랙트를 만든다....
						
			vec3 center_obj;		obj_aabb.calc_center(center_obj);
			vec3 extent_obj;		obj_aabb.calc_extent(extent_obj);

			// 먼저 center를 matView에 넣어 카메라 좌표계로 변경... Vec3TransformCoord함수를 호출해도 되지만, 일단 직접 곱하기로 했다..
			vec3 viewed_center_obj;
			viewed_center_obj.x = (matView.m[0][0] * center_obj.x + matView.m[1][0] * center_obj.y + matView.m[2][0] * center_obj.z + matView.m[3][0]);
			viewed_center_obj.y = (matView.m[0][1] * center_obj.x + matView.m[1][1] * center_obj.y + matView.m[2][1] * center_obj.z + matView.m[3][1]);
			viewed_center_obj.z = (matView.m[0][2] * center_obj.x + matView.m[1][2] * center_obj.y + matView.m[2][2] * center_obj.z + matView.m[3][2]);

			// extent의 경우 matView의 회전축과 연동되는데, 이때 각 axis축의 해당x,y,z성분이 음이라도 양값이 되도록 fabs처리를 한다...
			// Vec3TransformNormal과 유사하지만, 축성분이 음이면 모두 양값처리한다..
			vec3 semiLength;
			semiLength.x = fabs(matView.m[0][0]) * extent_obj.x + fabs(matView.m[1][0]) * extent_obj.y + fabs(matView.m[2][0]) * extent_obj.z;
			semiLength.y = fabs(matView.m[0][1]) * extent_obj.x + fabs(matView.m[1][1]) * extent_obj.y + fabs(matView.m[2][1]) * extent_obj.z;
			semiLength.z = fabs(matView.m[0][2]) * extent_obj.x + fabs(matView.m[1][2]) * extent_obj.y + fabs(matView.m[2][2]) * extent_obj.z;

			// 이제 뷰기준에 대해 min, max값을 구할수 있는데, 프로젝션행렬의 _11, _22 곱하기(x,y scaling) 및 w값 (_34) 나누기 처리를 해야한다...
			vec3 vMin = viewed_center_obj;	vMin -= semiLength;
			vec3 vMax = viewed_center_obj;	vMax += semiLength;
					
			// 여기까지의 min,max는 카메라 좌표계에서의 신규aabb에 해당된다... (좀더 커졌을 것임)
			// projection matrix의 _33,_43, _34 3개 요소로 NearZ를 알수 있음..

			float fSign = matProj._34;

			if (fSign < 0.f)		// 헷갈리므로, DirectX 표준 Z좌표로 변경하여 계산..
			{
				float fMinZ = -vMax.z;	float fMaxZ = -vMin.z;
				vMin.z = fMinZ;		vMax.z = fMaxZ;
			}

			bool bMinZOver = false;
			if (vMin.z < 1e-06f)
			{
				bMinZOver = true;
			}

//			float fNearZ = (matProj._43 / matProj._33) * (fSign * -1.f);

			float xScalingPerW = matProj._11;		// 왼손좌표계의 경우 _34 = 1, 오른손이면 _34 = -1이 된다..
			float yScalingPerW = matProj._22;		//
									
			// 미리 x성분과 y성분은 투영행렬 스케일 수치를 곱해둔다..z성분은 나중에 나눌때 쓰이게됨..
			vMin.x *= xScalingPerW;		vMin.y *= yScalingPerW;
			vMax.x *= xScalingPerW;		vMax.y *= yScalingPerW;
			
			if (bMinZOver)		//// invMinZ가 무한대...
			{
				float invMaxZ = 1.f / vMax.z;

				if (vMin.x < 0.f)
					out.minX = -1.f;
				else
					out.minX = vMin.x * invMaxZ;
				
				if (vMax.x > 0.f)
					out.maxX = 1.f;  
				else
					out.maxX = vMax.x * invMaxZ;

				//
				if (vMin.y < 0.f)
					out.minY = -1.f;
				else
					out.minY = vMin.y * invMaxZ;

				if (vMax.y > 0.f)
					out.maxY = 1.f;
				else
					out.maxY = vMax.y * invMaxZ;
			}
			else
			{
				float invMinZ = 1.f / vMin.z;			
				float invMaxZ = 1.f / vMax.z;
				{
					float fMin1 = vMin.x * invMinZ;
					float fMin2 = vMin.x * invMaxZ;
					out.minX = (fMin1 < fMin2) ? fMin1 : fMin2;
				}
				{
					float fMax1 = vMax.x * invMinZ;
					float fMax2 = vMax.x * invMaxZ;
					out.maxX = (fMax1 < fMax2) ? fMax2 : fMax1;
				}
				{
					float fMin1 = vMin.y * invMinZ;
					float fMin2 = vMin.y * invMaxZ;
					out.minY = (fMin1 < fMin2) ? fMin1 : fMin2;
				}
				{
					float fMax1 = vMax.y * invMinZ;
					float fMax2 = vMax.y * invMaxZ;
					out.maxY = (fMax1 < fMax2) ? fMax2 : fMax1;
				}
			}
					
			if (out.minX < -1.f)
				out.minX = -1.f;

			if (out.maxX > 1.f)
				out.maxX = 1.f;
			
			if (out.minY < -1.f)
				out.minY = -1.f;

			if (out.maxY > 1.f)
				out.maxY = 1.f;

			I3ASSERT(out.minX <= out.maxX && out.minY <= out.maxY);

		// 아래코드는 3디공간상 투영시 최소,최대 위치가 바뀌기 때문에 틀린 코드이므로 반영되어서는 안되지만, 가독성때문에 남겨둠...
		//	out.minX = vMin.x / vMin.z * xScalingPerW;
		//	out.minY = vMin.y / vMin.z * yScalingPerW;
		//	out.maxX = vMax.x / vMax.z * xScalingPerW;
		//	out.maxY = vMax.y / vMax.z * yScalingPerW;
	
		}
		
		bool is_not_intersected_float_rect(const occ_object* occluder_obj, occ_object* occludee_obj)
		{
			const occ_float_rect& occluder_2d_rect = occluder_obj->m_occ_float_rect;
			const occ_float_rect& occludee_2d_rect = occludee_obj->m_occ_float_rect;

			return (occluder_2d_rect.maxX < occludee_2d_rect.minX ||
					 occluder_2d_rect.minX > occludee_2d_rect.maxX ||
					 occluder_2d_rect.maxY < occludee_2d_rect.minY ||
					 occluder_2d_rect.minY > occludee_2d_rect.maxY);		// 이 경우는 무조건 보이게 됨...
		}

			
		intersect_side check_occ_impl(const vec3& cam_pos, occ_object* occluder_obj, occ_object* occludee_arg, const occ_plane& occluder_plane)
		{
			intersect_side res_side;
				
			if (is_not_intersected_float_rect(occluder_obj, occludee_arg))
			{
				return intersect_side_positive;
			}

			if (occludee_arg->m_occluder_quad)		//  이 경우엔 내부 오클루더가 오클루디임..
			{
				const occ_quad* occludee_quad = occludee_arg->m_occluder_quad;
				const vec3& v0 = occludee_quad->get_quad_vertex_pos(0);
				const vec3& v1 = occludee_quad->get_quad_vertex_pos(1);
				const vec3& v2 = occludee_quad->get_quad_vertex_pos(2);

				if (occludee_quad->is_quad_triangle())
				{
					res_side = i3::intersect::occ_triangle(occluder_plane, v0, v1, v2);
				}
				else
				{
					const vec3& v3 = occludee_quad->get_quad_vertex_pos(3);

					res_side = i3::intersect::occ_quad(occluder_plane, v0, v1, v2, v3);
				}
			}
			else
			{
				const i3::aabb& occludee_aabb = occludee_arg->m_octree_item->get_aabb();

				res_side = i3::intersect::occ_aabb(occluder_plane, occludee_aabb);
			}

			return res_side;
		}
						
	}


	void occ_culler::cull_frustum(const frustum_plane& fr, const mat4& matView,
		const mat4& matProj, const vec3& cam_pos)			// 내부에서 옥트리 업데이트를 병행..
	{
	//	m_vecRenderableObjects_GeomOCC.clear();

		int	prev_octree_visible_occluder_count = m_statistics_info.octree_visible_occluder_count;
		int prev_octree_visible_occludee_count = m_statistics_info.octree_visible_occludee_count;
		
		m_statistics_info.octree_visible_occluder_count = 0;
		m_statistics_info.octree_visible_occludee_count = 0;

		m_octree->cull_frustum(fr);
		
		// 이후부터 오클루전 컬링 루틴임..

		typedef i3::vector<occ_object*>::iterator occ_iterator;
	
		// 사전 처리...무효aabb인 오클루디와 backface 오클루더를 일단 다른 컨테이너로 걸러냄..
		{
			for (occ_iterator it = m_vec_occ_object.begin(); it != m_vec_occ_object.end(); )		// 3항 비움..반복자 처리에 유의해야한다..
			{
				occ_object* occludee_obj = *it;
				octree_item* oct_item = occludee_obj->m_octree_item;

				if (oct_item->get_aabb().is_valid() == false)
				{
					it = m_vec_occ_object.erase(it);		// AABB 무효화된 경우 안보이는 것으로 간주하자..
					occludee_obj->m_object_state = occ_object::invalid_aabb;
					m_vec_invisible_object.push_back(occludee_obj);
					continue;
				}
				
				if (occludee_obj->m_occluder_quad)		// 대상오브젝트가 오클루더인 경우.. 은면검사를 해주는게 좋을것 같음..옥트리에서 처리하기가 좀 그래서 이쪽에서 처리해둔다..
				{
					occ_quad* curr_occludee_quad = occludee_obj->m_occluder_quad;

					const i3::plane& qp = curr_occludee_quad->get_quad_plane();
					const i3::plane& near_plane = fr.get_plane(i3::frustum_plane::PLANE_NEAR);

					i3::vec3 cam_to_obj_dir;
					const size_t num_vert = curr_occludee_quad->is_quad_triangle() ? 3 : 4;

					bool is_front_face = false;

					for (size_t i = 0; i < num_vert; ++i)
					{
						const i3::vec3& vert = curr_occludee_quad->get_quad_vertex_pos(i);

						if (i3::calc_plane_distance(near_plane, vert) > 0.f)	// 정점이 시야 뒤에 있으면 계산 무시..
						{
							cam_to_obj_dir = vert;
							cam_to_obj_dir -= cam_pos;

							float fDot = i3::plane_normal_dot_vec3(qp, cam_to_obj_dir);
							is_front_face = (fDot < 0.f);			// 단 한번만 측정되면 된다..
							break;
						}
					}

					if (is_front_face == false)
					{
						it = m_vec_occ_object.erase(it);		// AABB 무효화된 경우 안보이는 것으로 간주하자..
						occludee_obj->m_object_state = occ_object::backface_cull;
						m_vec_invisible_object.push_back(occludee_obj);
						continue;
					}
				}

				++it;
			}

			///////////////

			// 옥트리에서 걸려온 오클루더들에 대해 면적을 구한다..
			{
				const size_t num_occ_object = m_vec_occ_object.size();

				for (size_t i = 0; i < num_occ_object; ++i)
				{
					occ_object* occ = m_vec_occ_object[i];
					const aabb& occ_aabb = occ->m_octree_item->get_aabb();

					calcluate_projected_rect_with_aabb_and_view_projection(occ->m_occ_float_rect, occ_aabb, matView, matProj);

					const occ_float_rect& fRect = occ->m_occ_float_rect;
					occ->m_projected_area = (fRect.maxX - fRect.minX) * (fRect.maxY - fRect.minY);
				}
			}
			
			///////////////
			// temporal coherency 처리.. octree_item에서 hidden 처리된 occ_quad pointer가 있다면...여기서 선계산해본다...

			for (occ_iterator it = m_vec_occ_object.begin() ;  it != m_vec_occ_object.end(); )		// 3항 비움..반복자 처리에 유의해야한다..
			{
				occ_object* occludee_obj = *it;
				octree_item* oct_item = occludee_obj->m_octree_item;
								
				const occ_quad* occluder_quad = oct_item->get_occ_quad_coherency();

				if (occluder_quad == nullptr)		// TC가 없다면 그냥 다음 것으로 넘김..
				{
					++it;	continue;
				}
				
				occ_object* occluder_obj = occluder_quad->get_bound_occ_object();

				if (occluder_obj == nullptr)	// 이것이 널이면 프러스텀컬링에서 배제된 오클루더임...담번엔 가릴수도 있다고 가정하고, 일단 놔두고 continue;
				{
					++it;	continue;
				}

				if (occluder_obj->m_cached_occ_plane == nullptr)
				{
					occluder_obj->m_cached_occ_plane = new occ_plane;			// 캐싱용으로 occ_plane을 동적생성한다..// 다음번 계산에서 재활용될수 있기때문 
					occluder_obj->m_cached_occ_plane->apply(cam_pos, occluder_quad);
				}

				intersect_side occludee_result_side = check_occ_impl(cam_pos, occluder_obj, occludee_obj, *occluder_obj->m_cached_occ_plane);
					
				if (occludee_result_side == intersect_side_negative)
				{
					it = m_vec_occ_object.erase(it);
					occludee_obj->m_object_state = occ_object::occluded;
					m_vec_invisible_object.push_back(occludee_obj);

					const size_t num_intersected_occluder = occludee_obj->m_intersected_quad_list.size();

					for (size_t i = 0; i < num_intersected_occluder; ++i)
					{
						occ_object* intersected_occluder = occludee_obj->m_intersected_quad_list[i];
						--intersected_occluder->m_occludee_intersect_count;
					}

				}
				else
				{
					
					i3::uint32_t quad_idx = occluder_quad->get_face_index();

					if (occludee_obj->m_occluder_quad == nullptr && occludee_result_side == intersect_side_both)
					{
						++occluder_obj->m_occludee_intersect_count;
						occludee_obj->m_intersected_quad_list.push_back(occluder_obj);
					}

					occludee_obj->m_already_checked_quad_list.push_back(quad_idx);	// 히든이 아닐때 경우 중복체크가 안되도록...

					if (occludee_result_side == intersect_side_positive)		// 오클루더와 완전이 분리된 경우 coherency제거..
						oct_item->set_occ_quad_coherency(nullptr);

					++it;				// 보일때는 다음번 오클루디를 검사하도록 한다..보일때도 응집성문제로 coherency는 여기서는 일단 유지하도록 한다...
				}
				
			}
			
		}
			

		// 면적이 작은 것에서 큰 것 순으로 정렬...(큰놈이면 화면상에서 많은 시야각을 가진 (큰) 놈이다...)
		struct comp_less_occ
		{
			bool operator()(const occ_object* l, const occ_object* r) const
			{
				return l->m_projected_area < r->m_projected_area;
			}
		};

		i3::sort(m_vec_occ_object.begin(), m_vec_occ_object.end(), comp_less_occ());

		// 가장 큰 오클루더를 하나 꺼내고, 오클루전 쿼드 컬링 작업 수행...
		// 일단 오브젝트부터 추려본다...

	
		occ_plane ocpl_in_stack;
		occ_plane* curr_occ_plane = nullptr;

		while (m_vec_occ_object.empty() == false)			// 오클루더를 하나씩 꺼낸다..큰놈부터(뒷쪽에서부터)
		{
			occ_object* cur_occluder = m_vec_occ_object.back();
			m_vec_occ_object.pop_back();

			occ_quad* cur_occ_quad = cur_occluder->m_occluder_quad;

			if (cur_occ_quad == nullptr)		// 면적순 정렬된 상태에서, 오클루더가 아닌데 면적이 더 큰 경우, 무조건 보일 것으로 기대...
			{
				occ_object* cur_occludee = cur_occluder;
				m_vecRenderableObjects_GeomOCC.push_back(cur_occludee);
			//	delete cur_occludee;			// 실제로는 오클루디이다..
				continue;
			}

			if (cur_occluder->m_cached_occ_plane != nullptr)		// occ_plane 재활용..
			{
				curr_occ_plane = cur_occluder->m_cached_occ_plane;
			}
			else
			{
				curr_occ_plane = &ocpl_in_stack;
				ocpl_in_stack.apply(cam_pos, cur_occ_quad);
			}

			i3::uint32_t cur_occ_face = cur_occ_quad->get_face_index();		// 현재 쿼드 인덱스..
			
			// 일단 오클루디 목록과의 가시성 검사를 수행한다..
	
			for (occ_iterator it = m_vec_occ_object.begin(); it != m_vec_occ_object.end(); )		// 세번째 항을 비워둠..
			{
				occ_object*	cur_occludee = *it;
				
				if (!cur_occludee->m_already_checked_quad_list.empty())
				{
					// 이미 체크한 놈은 더 이상 검사하지 않는다.. 이번 루프에서는 스킵..
					i3::vector<i3::uint32_t>& vec_already_checked_quad_list = cur_occludee->m_already_checked_quad_list;

					i3::vector<i3::uint32_t>::iterator uint_iter = i3::find(vec_already_checked_quad_list.begin(),
						vec_already_checked_quad_list.end(), cur_occ_face);

					if (uint_iter != vec_already_checked_quad_list.end() )
					{
						vec_already_checked_quad_list.erase(uint_iter);		++it;
						continue;
					}
				}
								
			//	const aabb& cur_occludee_aabb = cur_occludee->m_octree_item->get_aabb();
					
				intersect_side occludee_result_side = check_occ_impl(cam_pos, cur_occluder, cur_occludee, *curr_occ_plane);

				if (occludee_result_side == intersect_side_negative)
				{
					octree_item* oct_item = cur_occludee->m_octree_item;
					oct_item->set_occ_quad_coherency(cur_occ_quad);		// 오클루더를 coherency로 오클루디 아이템에 셋팅..
					it = m_vec_occ_object.erase(it);

					cur_occludee->m_object_state = occ_object::occluded;
					m_vec_invisible_object.push_back(cur_occludee);

					const size_t num_intersected_occluder = cur_occludee->m_intersected_quad_list.size();

					for (size_t i = 0; i < num_intersected_occluder; ++i)
					{
						occ_object* intersected_occluder = cur_occludee->m_intersected_quad_list[i];
						--intersected_occluder->m_occludee_intersect_count;
					}

				}
				else
				{
					if (cur_occludee->m_occluder_quad == nullptr && occludee_result_side == intersect_side_both)
					{
						cur_occludee->m_intersected_quad_list.push_back(cur_occluder);
						++cur_occluder->m_occludee_intersect_count;
					}

					++it;							// 교차나 보일때는 다음번 검사로 넘긴다...
				}
			}
			
			m_vec_processed_object.push_back(cur_occluder);
		//	m_vecRemainQuads.push_back(cur_occluder->m_occluder_quad);
		//	delete cur_occluder;		// 소멸자에서 쿼드에 바인딩된 오클루더 객체는 삭제..
		}

		const size_t remainOccCount = m_vec_occ_object.size();
		for (size_t i = 0; i < remainOccCount; ++i)
		{
			occ_object*	cur_occludee = m_vec_occ_object[i];
			assert(cur_occludee->m_occluder_quad == nullptr);

			m_vecRenderableObjects_GeomOCC.push_back(cur_occludee);
		//	delete cur_occludee;
		}
		// 처리가 모두 끝났으면... 사용중인 목록은 비워둔다..
		m_vec_occ_object.clear();

		const size_t num_processed = m_vec_processed_object.size();

		if (m_query_callback == nullptr)
		{
			for (size_t i = 0; i < num_processed; ++i)
			{
				occ_object* processed_occluder = m_vec_processed_object[i];
				delete processed_occluder;
			}
		}
		else
		{
			for (size_t i = 0; i < num_processed; ++i)
			{
				occ_object* processed_occluder = m_vec_processed_object[i];
				if (processed_occluder->m_occludee_intersect_count > 0)
				{
					m_vecRemainQuads.push_back(processed_occluder->m_occluder_quad);
				}
				delete processed_occluder;
			}
		}
		m_vec_processed_object.clear();

		const size_t num_culled = m_vec_invisible_object.size();		// 백페이스컬링과 오클루전 컬링된 것들..
		int       curr_geom_occ_visible_occluder_count = m_statistics_info.octree_visible_occluder_count;
		
		for (size_t i = 0; i < num_culled; ++i)
		{
			occ_object* cur_occludee = m_vec_invisible_object[i];

			octree_item* oct_item = cur_occludee->m_octree_item;
			occ_quad*    curr_quad = cur_occludee->m_occluder_quad;

			delete cur_occludee;

			if (curr_quad == nullptr)
			{
				oct_item->on_hidden_occluder_aabb();
			}
			else
			{
				--curr_geom_occ_visible_occluder_count;
			}
		}

		m_vec_invisible_object.clear();
		
		// m_vecRenderableItems를 순회하여 item의 가상함수를 호출하도록 한다...
	
		int curr_hw_query_calc_occluder_count = m_vecRemainQuads.size();
		int curr_geom_occ_visible_occludee_count = m_vecRenderableObjects_GeomOCC.size();
		int curr_surely_renderable_occludee_count = curr_geom_occ_visible_occludee_count;
		int currNumOccludeeToRender = curr_geom_occ_visible_occludee_count;

		if (m_query_callback == nullptr)
		{
			const size_t num_visible = m_vecRenderableObjects_GeomOCC.size();
			for (size_t i = 0; i < num_visible; ++i)
			{
				occ_object* obj = m_vecRenderableObjects_GeomOCC[i];
				octree_item* item = obj->m_octree_item;
				item->on_visible_occluder_aabb();			// 쿼드는 제외시킨다...

				delete obj;
			}

			m_vecRenderableObjects_GeomOCC.clear();
		}
		else
		{
			curr_surely_renderable_occludee_count = 0;

			m_vecUnsurelyRenderableItem_OnGeomOCC.clear();

			const size_t num_visible = m_vecRenderableObjects_GeomOCC.size();

			for (size_t i = 0; i < num_visible; ++i)
			{
				occ_object* obj = m_vecRenderableObjects_GeomOCC[i];

				if (obj->m_intersected_quad_list.empty())
				{
					++curr_surely_renderable_occludee_count;
					octree_item* item = obj->m_octree_item;
					item->on_visible_occluder_aabb();			// 쿼드는 제외시킨다...
				}
				else
				{
					m_vecUnsurelyRenderableItem_OnGeomOCC.push_back(obj->m_octree_item);
				}
				delete obj;
			}
			m_vecRenderableObjects_GeomOCC.clear();

			i3::mat4 matViewProj;
			i3::matrix_multiply(matView, matProj, matViewProj);
			currNumOccludeeToRender = m_query_callback->on_additional_process(m_vecRemainQuads, m_vecUnsurelyRenderableItem_OnGeomOCC, matViewProj);
		}
		
		int curr_static_item_count = m_octree->get_curr_static_item_count();
		int curr_dynamic_item_count = m_octree->get_curr_dynamic_item_count();

		if (m_statistics_callback)
		{
		
			if (prev_octree_visible_occluder_count != m_statistics_info.octree_visible_occluder_count ||
				prev_octree_visible_occludee_count != m_statistics_info.octree_visible_occludee_count ||
				curr_geom_occ_visible_occluder_count != m_statistics_info.geom_occ_visible_occluder_count ||
				curr_geom_occ_visible_occludee_count != m_statistics_info.geom_occ_visible_occludee_count ||
				curr_surely_renderable_occludee_count != m_statistics_info.geom_occ_surely_visible_occludee_count ||
				curr_hw_query_calc_occluder_count != m_statistics_info.hw_query_calc_occluder_count ||
				currNumOccludeeToRender != m_statistics_info.hw_query_visible_occludee_count ||
				curr_static_item_count != m_statistics_info.tot_occluder_count ||
				curr_dynamic_item_count != m_statistics_info.tot_occludee_count
				)
			{
				// 상위 2개는 이미 최신화되어있음...(octree_visible_occluder_count, hw_query_visible_occludee_count)
				m_statistics_info.tot_occluder_count = curr_static_item_count;
				m_statistics_info.tot_occludee_count = curr_dynamic_item_count;

				m_statistics_info.geom_occ_visible_occluder_count = curr_geom_occ_visible_occluder_count;
				m_statistics_info.geom_occ_visible_occludee_count = curr_geom_occ_visible_occludee_count;

				m_statistics_info.geom_occ_surely_visible_occludee_count = curr_surely_renderable_occludee_count;

				m_statistics_info.hw_query_calc_occluder_count = curr_hw_query_calc_occluder_count;
				m_statistics_info.hw_query_visible_occludee_count = currNumOccludeeToRender;
				m_statistics_callback->on_change(m_statistics_info);
			}
		}
		else
		{
			m_statistics_info.tot_occluder_count = curr_static_item_count;
			m_statistics_info.tot_occludee_count = curr_dynamic_item_count;

			m_statistics_info.geom_occ_visible_occluder_count = curr_geom_occ_visible_occluder_count;
			m_statistics_info.geom_occ_visible_occludee_count = curr_geom_occ_visible_occludee_count;

			m_statistics_info.geom_occ_surely_visible_occludee_count = curr_surely_renderable_occludee_count;

			m_statistics_info.hw_query_calc_occluder_count = curr_hw_query_calc_occluder_count;
			m_statistics_info.hw_query_visible_occludee_count = currNumOccludeeToRender;
		}

		m_vecRemainQuads.clear();

	}
	

	void occ_culler::update_occludee_item(octree_item* item)
	{
		m_octree->update_item(item);
	}

	void occ_culler::add_occludee_item(octree_item* item)
	{
		m_octree->add_item(item);
	}

	void occ_culler::remove_occludee_item(octree_item* item)
	{
		m_octree->remove_item(item);
	}


}
