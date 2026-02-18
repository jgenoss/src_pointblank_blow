#include "i3MathType.h"

#include "mesh_function/mesh_convert_quad.h"
#include "mesh_function/mesh_adjacency.h"

#include "structure/vec3_function.h"
#include "i3Base/smart_ptr/scoped_array.h"

namespace i3
{
	// 첫 세 정점은 인덱스리스트의 삼각형으로 가정하고 나머지 한점은 인접면 반대쪽 정점이라고 가정...


	float		calc_quad_quality_score(const i3::uint32_t* index_arr,
		const vec3* vert_arr, i3::uint32_t face0, i3::uint32_t face1, i3::uint32_t face0_adj_edge_idx, i3::uint32_t face1_adj_edge_idx)
	{
		// 1. 각 삼각형의 평면노멀값을 비교...정규화시키지 않더라도, 외적이 0에 근사하면 평면노멀이 같거나 반대방향인것은 알수 있고....
		//    부호가 반대인 경우는 내적으로 확인하거나 성분 하나만 놓고 확인해도 될거 같다...

		if ( face0 == UNUSED32 || face1 == UNUSED32)
			return 0.f;

		vec3 pl_normal_0, pl_normal_1;

		{
			const vec3& v0 = vert_arr[index_arr[face0 * 3]];
			const vec3& v1 = vert_arr[index_arr[face0 * 3 + 1]];
			const vec3& v2 = vert_arr[index_arr[face0 * 3 + 2]];

			vec3 v0_1 = v1;	v0_1 -= v0;		vec3 v0_2 = v2;	v0_2 -= v0;
			i3::vec3_cross(pl_normal_0, v0_1, v0_2);
		}

		{
			const vec3& v0 = vert_arr[index_arr[face1 * 3]];
			const vec3& v1 = vert_arr[index_arr[face1 * 3 + 1]];
			const vec3& v2 = vert_arr[index_arr[face1 * 3 + 2]];

			vec3 v0_1 = v1;	v0_1 -= v0;		vec3 v0_2 = v2;	v0_2 -= v0;
			i3::vec3_cross(pl_normal_1, v0_1, v0_2);
		}

		float dot = i3::vec3_dot(pl_normal_0, pl_normal_1);

		if (dot <= I3_EPS)		// 평면 노멀이 서로 다른 방향을 가르킨다면 .. 점수를 0으로 놓음...
			return 0.f;

		vec3 cross_normal01;	i3::vec3_cross(cross_normal01, pl_normal_0, pl_normal_1);
		float len_sq_cross = i3::vec3_length_sq(cross_normal01);

		if (len_sq_cross >= I3_EPS)		// 평면노멀이 거의 같지를 않다면 점수를 0으로 놓음..
			return 0.f;

		// 2. 각 면별 공유엣지를 토대로 4점 쿼드를 얻음...

		const vec3* pv0 = nullptr;	const vec3* pv1 = nullptr;	const vec3* pv2 = nullptr;	const vec3* pv3 = nullptr;
	
		switch (face0_adj_edge_idx)
		{
		case 0:		// 0-1 모서리 공유
			pv0 = &vert_arr[index_arr[face0 * 3 + 2]];
			pv1 = &vert_arr[index_arr[face0 * 3 + 0]];
			pv3 = &vert_arr[index_arr[face0 * 3 + 1]];
		break;
		case 1:		// 1-2 모서리 공유
			pv0 = &vert_arr[index_arr[face0 * 3 + 0]];
			pv1 = &vert_arr[index_arr[face0 * 3 + 1]];
			pv3 = &vert_arr[index_arr[face0 * 3 + 2]];
		break;
		case 2:		// 2-0 모서리 공유
			pv0 = &vert_arr[index_arr[face0 * 3 + 1]];
			pv1 = &vert_arr[index_arr[face0 * 3 + 2]];
			pv3 = &vert_arr[index_arr[face0 * 3 + 0]];
		break;
		default:
			return 0.f;
		}

		const vec3& v0 = *pv0;
		const vec3& v1 = *pv1; 
		const vec3& v3 = *pv3; 
	
		switch (face1_adj_edge_idx)
		{
		case 0: pv2 = &vert_arr[index_arr[face1 * 3 + 2]];	break;	// 0-1변 공유하므로 2번째 정점이 비인접정점..
		case 1: pv2 = &vert_arr[index_arr[face1 * 3]];	break;		// 1-2변 공유하므로 0번째 정점이 비인접정점..
		case 2: pv2 = &vert_arr[index_arr[face1 * 3 + 1]]; break;	// 2-0변 공유하므로 1번째 정점이 비인접 정점.
		default:	return 0.f;										// 잘못된 인접변인덱스를 가지면 그냥 0.f로 처리
		}
		const vec3& v2 = *pv2;

		// v0~v4까지 시계반대방향으로 회전하여 배치됨..첫점과 마지막점은 공유변을 갖지 않는 쪽임..
		// 각 정점별 사잇각이 90도에 얼마나 가까운지 찾아보고, 평균을 낸다...  90도에 가까우면 0, 멀어질수록 1이 되도록 하면 된다..
		// 각도함수를 쓰지 않는다면 cos로 대신하여, dot / (len0*len1) 으로 처리해도 될것 같긴 하다...  a dot b = |a| |b| cos
		// 0번 
		float fdot_avg = 0.f;

		
		vec3 v01 = v1;	v01 -= v0;		i3::vec3_normalize_self(v01);
		vec3 v03 = v3;	v03 -= v0;		i3::vec3_normalize_self(v03);

		vec3 v0103_cross; i3::vec3_cross(v0103_cross, v01, v03);

		vec3 v10 = v0;	v10 -= v1;		i3::vec3_normalize_self(v10);
		vec3 v12 = v2;	v12 -= v1;		i3::vec3_normalize_self(v12);

		vec3 v1012_cross; i3::vec3_cross(v1012_cross, v10, v12);
		
		vec3 v21 = v1;	v21 -= v2;		i3::vec3_normalize_self(v21);
		vec3 v23 = v3;	v23 -= v2;		i3::vec3_normalize_self(v23);

		vec3 v2123_cross; i3::vec3_cross(v2123_cross, v21, v23);
		
		vec3 v32 = v2;	v32 -= v3;		i3::vec3_normalize_self(v32);
		vec3 v30 = v0;	v30 -= v3;		i3::vec3_normalize_self(v30);

		vec3 v3230_cross; i3::vec3_cross(v3230_cross, v32, v30);

		{
			float first_dot = i3::vec3_dot(v0103_cross, v1012_cross);
			float second_dot = i3::vec3_dot(v0103_cross, v2123_cross);
			float third_dot = i3::vec3_dot(v0103_cross, v3230_cross);
			// 위 3개의 내적들이 모두 양이거나 모두 음이 아니라면 concave라고 하여 
			// 움푹 파인 쿼드가 생성될수 있는데...계산오차가 생기므로 배제시키도록 한다..
			
			bool is_convex = (first_dot > I3_EPS && second_dot > I3_EPS && third_dot > I3_EPS) ||
				             (first_dot < -I3_EPS && second_dot < -I3_EPS && third_dot < -I3_EPS);
			
			if (is_convex == false)
				return 0.f;
		}
		
		{
			float fCos = i3::vec3_dot(v01, v03);
			fdot_avg += fabs(fCos);
		}

		{
			float fCos = i3::vec3_dot(v10, v12);
			fdot_avg += fabs(fCos);
		}

		{
			float fCos = i3::vec3_dot(v21, v23);
			fdot_avg += fabs(fCos);
		}

		{
			float fCos = i3::vec3_dot(v32, v30);
			fdot_avg += fabs(fCos);
		}

		fdot_avg /= 4.f;			

		return (1.f - fdot_avg);				// 0~1사이값으로 코사인평균값이 0일수록 좋으므로 1로 바꿔줌...
	}


	void		build_quad_index_list(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert,
		i3::vector<i3::uint32_t>& out_quad_vert_idx_list,
		i3::vector<i3::uint32_t>& out_quad_adj_list, const i3::uint32_t* in_adj)
	{
		// 전체를 모두 훑어서 만들어야함...

		const i3::uint32_t* adj_arr = in_adj;
		i3::scoped_array<i3::uint32_t>	adj_allocated;
		
		if (adj_arr == nullptr)
		{
			adj_allocated.reset(new i3::uint32_t[num_face * 3]);
			adj_arr = adj_allocated.get();
			i3::generate_adjacency(index_arr, num_face, vert_arr, num_vert, 0.f, adj_allocated.get());		// 인접 폴리건 인덱스목록부터 구함..
		}

		i3::vector<i3::uint32_t>	vecAdjQuadToTri;
		vecAdjQuadToTri.reserve(num_face * 3);

		i3::vector<i3::uint32_t>	vec2TriToQuad(num_face);	// 각 삼각형인덱스-->쿼드 인덱스로 매핑된 자료구조.. 삼각형 갯수만큼 존재할것으로 보인다.
															// reserve 대신 크기만큼 일단 만듬..

		out_quad_vert_idx_list.reserve(num_face * 3);		// 넉넉잡힌 케이스...삼각형 인덱스 목록과 비교시 쿼드구성실패한 삼각형의 경우 33%증가, 쿼드구성이 잘되면 33%감소임..
		out_quad_adj_list.reserve(num_face * 3);			//

		i3::vector<bool> vec_already_processed(num_face, false);

		for (size_t i = 0; i < num_face; ++i)
		{
			if (vec_already_processed[i] == true)		// 이미 쌍으로 처리된 삼각형인덱스는 건너뛴다..
				continue;

			float quality_max = 0.f;
			i3::uint32_t adj_for_quad = UNUSED32;
			size_t edge0_for_quad = UNUSED32;
			size_t edge1_for_quad = UNUSED32;

			for (size_t j = 0; j < 3; ++j)		// 이쪽은 인접면을 하나씩 까보는 루프이다.. 0-1,1-2,2-0
			{
				i3::uint32_t adj = adj_arr[i * 3 + j];

				if (adj == UNUSED32)		// 인접면이 없는 경우엔 continue를 수행..
				{
					continue;
				}
				
				if (vec_already_processed[adj] == true)		// 이미 다른 쿼드로 배정된 인접삼각형도 배제할수 있다..
					continue;
				
				size_t found_edge1 = UNUSED32;

				for (size_t k = 0; k < 3; ++k)
				{
					i3::uint32_t adjadj = adj_arr[adj * 3 + k];		// 인접면에서의 인접면 점검..
					
					if (adjadj == i)		// 이 조건을 만족하면 두개의 인접삼각형의 어떤 모서리가 공유되는지 알수 있음..
					{
						found_edge1 = k;
						break;
					}
				}
				
				if (found_edge1 == UNUSED32)		// 에러상황 .. 일단 그냥 continue처리..
				{
					continue;
				}

				float quality = calc_quad_quality_score(index_arr, vert_arr, i, adj, j, found_edge1);
					
				if (quality > quality_max)
				{
					quality_max = quality;
					adj_for_quad = adj;
					edge0_for_quad = j;
					edge1_for_quad = found_edge1;
				}
			}

			vec_already_processed[i] = true;
		
			i3::uint32_t curr_quad_idx = i3::uint32_t(out_quad_vert_idx_list.size()) / 4;

			vec2TriToQuad[i] = curr_quad_idx;		// 일단 기본루프상의 삼각형은 무조건 배정처리 된다.

			if (adj_for_quad == UNUSED32)			// 인접면과의 쿼드구성이 전혀 안되는 경우, 그냥 삼각형으로 쿼드를 만든다...
			{
				out_quad_vert_idx_list.push_back(index_arr[i * 3 + 0]);
				out_quad_vert_idx_list.push_back(index_arr[i * 3 + 1]);
				out_quad_vert_idx_list.push_back(index_arr[i * 3 + 2]);
				out_quad_vert_idx_list.push_back(UNUSED32);			// 마지막에 UNUSED32를 삽입..

				vecAdjQuadToTri.push_back(adj_arr[i * 3 + 0]);	// 0-1, 1-2, 2-3, 3-0 순으로 시계반대방향...
				vecAdjQuadToTri.push_back(adj_arr[i * 3 + 1]);
				vecAdjQuadToTri.push_back(adj_arr[i * 3 + 2]);
				vecAdjQuadToTri.push_back(UNUSED32);
			}
			else
			{
				// 4점이 확정된 정상적 경우, 쿼드목록에 등록..
				i3::uint32_t iv0 = UNUSED32;
				i3::uint32_t iv1 = UNUSED32;
				i3::uint32_t iv2 = UNUSED32;
				i3::uint32_t iv3 = UNUSED32;

				// 쿼드를 이루는 삼각형 인접변 아닌 다른 인접변을 쿼드 인접변으로 만드는 요령.
				// 첫번째 삼각형에서 쿼드인접변 이전 인접변과 이후 인접변을 구분..
				// 두번째 삼각형에서는 쿼드인접변 다음번 인접변 2개로 구분...
				// 첫삼각형의 이전 인접변 -> 둘째 삼각형의 담번 인접변 -> 둘째 삼각형의 다다음번 인접변 -> 첫삼각형의 이후 인접변..

				i3::uint32_t adjQuadToTri0 = UNUSED32;
				i3::uint32_t adjQuadToTri1 = UNUSED32;
				i3::uint32_t adjQuadToTri2 = UNUSED32;
				i3::uint32_t adjQuadToTri3 = UNUSED32;

				switch (edge0_for_quad)
				{
				case 0:		// 0-1 모서리 공유
					iv0 = index_arr[i * 3 + 2];		// 인접 아닌 정점을 시작으로 한다.
					iv1 = index_arr[i * 3 + 0];
					iv3 = index_arr[i * 3 + 1];
					// 0-1 이전 인접변은 2-0,  0-1 이후 인접변은 1-2
					adjQuadToTri0 = adj_arr[i * 3 + 2];
					adjQuadToTri3 = adj_arr[i * 3 + 1];
					break;
				case 1:		// 1-2 모서리 공유
					iv0 = index_arr[i * 3 + 0];
					iv1 = index_arr[i * 3 + 1];
					iv3 = index_arr[i * 3 + 2];
					// 1-2 이전 인접변은 0-1,  1-2 이후 인접변은 2-0
					adjQuadToTri0 = adj_arr[i * 3 + 0];
					adjQuadToTri3 = adj_arr[i * 3 + 2];
					break;

				case 2:		// 2-0 모서리 공유
					iv0 = index_arr[i * 3 + 1];
					iv1 = index_arr[i * 3 + 2];
					iv3 = index_arr[i * 3 + 0];
					// 2-0 이전 인접변은 1-2,  2-0 이후 인접변은 0-1
					adjQuadToTri0 = adj_arr[i * 3 + 1];
					adjQuadToTri3 = adj_arr[i * 3 + 0];
					break;

				default:
					break;		// 도달될리가 없음..
				}

				switch (edge1_for_quad)
				{
				case 0: // 0-1변 공유하므로 2번째 정점이 비인접정점..
					iv2 = index_arr[adj_for_quad * 3 + 2];
					adjQuadToTri1 = adj_arr[adj_for_quad * 3 + 1];		// 1-2
					adjQuadToTri2 = adj_arr[adj_for_quad * 3 + 2];		// 2-0
					break;
				case 1: // 1-2변 공유하므로 0번째 정점이 비인접정점..
					iv2 = index_arr[adj_for_quad * 3];
					adjQuadToTri1 = adj_arr[adj_for_quad * 3 + 2];		// 2-0
					adjQuadToTri2 = adj_arr[adj_for_quad * 3 + 0];		// 0-1
					break;
				case 2: // 2-0변 공유하므로 1번째 정점이 비인접 정점.
					iv2 = index_arr[adj_for_quad * 3 + 1];
					adjQuadToTri1 = adj_arr[adj_for_quad * 3 + 0];		// 0-1
					adjQuadToTri2 = adj_arr[adj_for_quad * 3 + 1];		// 1-2
					break;
				default:	break;	// 도달될리가 없음..
				}

				out_quad_vert_idx_list.push_back(iv0);
				out_quad_vert_idx_list.push_back(iv1);
				out_quad_vert_idx_list.push_back(iv2);
				out_quad_vert_idx_list.push_back(iv3);

				vecAdjQuadToTri.push_back(adjQuadToTri0);			// 0-1, 1-2, 2-3, 3-0 순으로 시계반대방향...
				vecAdjQuadToTri.push_back(adjQuadToTri1);
				vecAdjQuadToTri.push_back(adjQuadToTri2);
				vecAdjQuadToTri.push_back(adjQuadToTri3);

				vec_already_processed[adj_for_quad] = true;

				vec2TriToQuad[adj_for_quad] = curr_quad_idx;		// 쿼드에 참여된 두번째 삼각형에 쿼드인덱스를 추가로 넣어둔다..
			}
		}
		
		// 이번에는 만들어진 쿼드목록을 다시 순회하여, 인접된 삼각형인덱스를 쿼드 인덱스로 변경매핑처리한다...
		const size_t num_quad_vert_idx_list = out_quad_vert_idx_list.size();
		const size_t num_quad = num_quad_vert_idx_list / 4;

		for (size_t i = 0; i < num_quad; ++i)
		{
			i3::uint32_t adj_tri0 = vecAdjQuadToTri[i * 4 + 0];		// 모든 성분이 UNUSED32일수 있으므로 신경써야함..
			i3::uint32_t adj_tri1 = vecAdjQuadToTri[i * 4 + 1];
			i3::uint32_t adj_tri2 = vecAdjQuadToTri[i * 4 + 2];
			i3::uint32_t adj_tri3 = vecAdjQuadToTri[i * 4 + 3];

			i3::uint32_t adj_quad0 = (adj_tri0 != UNUSED32) ? vec2TriToQuad[adj_tri0] : UNUSED32;
			i3::uint32_t adj_quad1 = (adj_tri1 != UNUSED32) ? vec2TriToQuad[adj_tri1] : UNUSED32;
			i3::uint32_t adj_quad2 = (adj_tri2 != UNUSED32) ? vec2TriToQuad[adj_tri2] : UNUSED32;
			i3::uint32_t adj_quad3 = (adj_tri3 != UNUSED32) ? vec2TriToQuad[adj_tri3] : UNUSED32;

			out_quad_adj_list.push_back(adj_quad0);
			out_quad_adj_list.push_back(adj_quad1);
			out_quad_adj_list.push_back(adj_quad2);
			out_quad_adj_list.push_back(adj_quad3);
		}
		
	}

	// 인접 버퍼 없는 버전...(인접 버퍼를 통한 인접쿼드 구하는 방식(정확한 정점 일치 없이 여러개의 쿼드가 동시에 인접된 경우)에 
	// 결함이 있다고 판단하여, 다른 방식을 고안해야함...

	void		build_quad_index_list(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert,
		i3::vector<i3::uint32_t>& out_quad_vert_idx_list, const i3::uint32_t* in_adj)
	{
		// 전체를 모두 훑어서 만들어야함...
		const i3::uint32_t* adj_arr = in_adj;
		i3::scoped_array<i3::uint32_t>	adj_allocated;

		if (adj_arr == nullptr)
		{
			adj_allocated.reset(new i3::uint32_t[num_face * 3]);
			adj_arr = adj_allocated.get();
			i3::generate_adjacency(index_arr, num_face, vert_arr, num_vert, 0.f, adj_allocated.get());		// 인접 폴리건 인덱스목록부터 구함..
		}
			
		out_quad_vert_idx_list.reserve(num_face * 3);		// 넉넉잡힌 케이스...삼각형 인덱스 목록과 비교시 쿼드구성실패한 삼각형의 경우 33%증가, 쿼드구성이 잘되면 33%감소임..
	
		i3::vector<bool> vec_already_processed(num_face, false);

		for (size_t i = 0; i < num_face; ++i)
		{
			if (vec_already_processed[i] == true)		// 이미 쌍으로 처리된 삼각형인덱스는 건너뛴다..
				continue;

			float quality_max = 0.f;
			i3::uint32_t adj_for_quad = UNUSED32;
			size_t edge0_for_quad = UNUSED32;
			size_t edge1_for_quad = UNUSED32;

			for (size_t j = 0; j < 3; ++j)		// 이쪽은 인접면을 하나씩 까보는 루프이다.. 0-1,1-2,2-0
			{
				i3::uint32_t adj = adj_arr[i * 3 + j];

				if (adj == UNUSED32)		// 인접면이 없는 경우엔 continue를 수행..
				{
					continue;
				}

				if (vec_already_processed[adj] == true)		// 이미 다른 쿼드로 배정된 인접삼각형도 배제할수 있다..
					continue;

				size_t found_edge1 = UNUSED32;

				for (size_t k = 0; k < 3; ++k)
				{
					i3::uint32_t adjadj = adj_arr[adj * 3 + k];		// 인접면에서의 인접면 점검..

					if (adjadj == i)		// 이 조건을 만족하면 두개의 인접삼각형의 어떤 모서리가 공유되는지 알수 있음..
					{
						found_edge1 = k;
						break;
					}
				}

				if (found_edge1 == UNUSED32)		// 에러상황 .. 일단 그냥 continue처리..
				{
					continue;
				}

				float quality = calc_quad_quality_score(index_arr, vert_arr, i, adj, j, found_edge1);

				if (quality > quality_max)
				{
					quality_max = quality;
					adj_for_quad = adj;
					edge0_for_quad = j;
					edge1_for_quad = found_edge1;
				}
			}

			vec_already_processed[i] = true;
					
			if (adj_for_quad == UNUSED32)			// 인접면과의 쿼드구성이 전혀 안되는 경우, 그냥 삼각형으로 쿼드를 만든다...
			{
				out_quad_vert_idx_list.push_back(index_arr[i * 3 + 0]);
				out_quad_vert_idx_list.push_back(index_arr[i * 3 + 1]);
				out_quad_vert_idx_list.push_back(index_arr[i * 3 + 2]);
				out_quad_vert_idx_list.push_back(UNUSED32);			// 마지막에 UNUSED32를 삽입..
			}
			else
			{
				// 4점이 확정된 정상적 경우, 쿼드목록에 등록..
				i3::uint32_t iv0 = UNUSED32;
				i3::uint32_t iv1 = UNUSED32;
				i3::uint32_t iv2 = UNUSED32;
				i3::uint32_t iv3 = UNUSED32;

				// 쿼드를 이루는 삼각형 인접변 아닌 다른 인접변을 쿼드 인접변으로 만드는 요령.
				// 첫번째 삼각형에서 쿼드인접변 이전 인접변과 이후 인접변을 구분..
				// 두번째 삼각형에서는 쿼드인접변 다음번 인접변 2개로 구분...
				// 첫삼각형의 이전 인접변 -> 둘째 삼각형의 담번 인접변 -> 둘째 삼각형의 다다음번 인접변 -> 첫삼각형의 이후 인접변..

				switch (edge0_for_quad)
				{
				case 0:		// 0-1 모서리 공유
					iv0 = index_arr[i * 3 + 2];		// 인접 아닌 정점을 시작으로 한다.
					iv1 = index_arr[i * 3 + 0];
					iv3 = index_arr[i * 3 + 1];
					break;
				case 1:		// 1-2 모서리 공유
					iv0 = index_arr[i * 3 + 0];
					iv1 = index_arr[i * 3 + 1];
					iv3 = index_arr[i * 3 + 2];
					break;

				case 2:		// 2-0 모서리 공유
					iv0 = index_arr[i * 3 + 1];
					iv1 = index_arr[i * 3 + 2];
					iv3 = index_arr[i * 3 + 0];
					break;

				default:
					break;		// 도달될리가 없음..
				}

				switch (edge1_for_quad)
				{
				case 0: // 0-1변 공유하므로 2번째 정점이 비인접정점..
					iv2 = index_arr[adj_for_quad * 3 + 2];
				break;
				case 1: // 1-2변 공유하므로 0번째 정점이 비인접정점..
					iv2 = index_arr[adj_for_quad * 3];
				break;
				case 2: // 2-0변 공유하므로 1번째 정점이 비인접 정점.
					iv2 = index_arr[adj_for_quad * 3 + 1];
				break;
				default:	break;	// 도달될리가 없음..
				}

				out_quad_vert_idx_list.push_back(iv0);
				out_quad_vert_idx_list.push_back(iv1);
				out_quad_vert_idx_list.push_back(iv2);
				out_quad_vert_idx_list.push_back(iv3);
							
				vec_already_processed[adj_for_quad] = true;
			
			}
		}
		
	}

}