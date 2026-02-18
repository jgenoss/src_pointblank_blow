#include "i3MathType.h"

#include "mesh_function/mesh_delete_t_junction.h"
#include "structure/vec3.h"
#include "structure/vec3_function.h"

namespace i3
{
	namespace
	{
		float point_line_distance(const i3::vec3& pt, const i3::vec3& line_pos0, const i3::vec3& line_pos1)
		{
			vec3 line01 = line_pos1;	line01 -= line_pos0;
			vec3 pt_line = line_pos0;	pt_line -= pt;
			vec3 cross_line_pt;
			i3::vec3_cross(cross_line_pt, line01, pt_line);

			float fLenCross = i3::vec3_length(cross_line_pt);
			float fLenLine01 = i3::vec3_length(line01);
			return fLenCross / fLenLine01;
		}

		// gpg3 등 Lengyel의 T-junction 판별 함수 코드 참고

		bool is_point_on_line(const i3::vec3& pt, const i3::vec3& line_pos0, const i3::vec3& line_pos1, float epsilon)
		{
			vec3 line01 = line_pos1;	line01 -= line_pos0;
			vec3 line0pt = pt;			line0pt -= line_pos0;

			float lensq_line0pt = i3::vec3_length_sq(line0pt);
			float lensq_line01 = i3::vec3_length_sq(line01);
			float dot_line01_line0pt = i3::vec3_dot(line0pt, line01);

			float dist_sq = lensq_line0pt - (dot_line01_line0pt*dot_line01_line0pt) / lensq_line01;
//			const float eps_sq = epsilon * epsilon;

			if (dist_sq > I3_EPS)		// 포인트 2개를 거치는 무한직선과 한점간의 거리....떨어져있으면 일단 아웃...
				return false;

			float len_line01 = sqrtf(lensq_line01);

			float t = dot_line01_line0pt / len_line01;		// 포인트 2개 사이에 존재하는지 여부 체크..

			return  t >= epsilon &&	(t <= len_line01 - epsilon);
		}


		bool delete_t_junction_imp(i3::vector<i3::uint32_t>& inout_vec_index,
			const vec3* vert_arr, size_t num_vert)
		{
			bool t_junction_result = false;

			for (size_t i = 0; i < inout_vec_index.size()/3 ; )		// 3항 비움..
			{
				bool is_t_junction_found = false;

				if (inout_vec_index[i * 3] >= num_vert) { ++i; continue; }
				if (inout_vec_index[i * 3 + 1] >= num_vert) { ++i; continue; }
				if (inout_vec_index[i * 3 + 2] >= num_vert) { ++i; continue; }
								
				for (size_t j = 0; j < 3; ++j )
				{
					i3::uint32_t indexa0 = inout_vec_index[i * 3 + j];
					i3::uint32_t indexa1 = inout_vec_index[i * 3 + (j + 1) % 3];
					i3::uint32_t indexa2 = inout_vec_index[i * 3 + (j + 2) % 3];
				
					const i3::vec3& a0 = vert_arr[indexa0];
					const i3::vec3& a1 = vert_arr[indexa1];
									
					for (size_t k = 0; k < inout_vec_index.size() / 3; ++k )
					{
						if (k == i) continue;			// 자기것 이외의 모든 것과 조사하도록 한다..

						for (size_t l = 0; l < 3; ++l)
						{
							i3::uint32_t indexb = inout_vec_index[k * 3 + l];
							if (indexb >= num_vert) continue;
							if (indexb == indexa0 || indexb == indexa1 || indexb == indexa2) 
								continue;

							const vec3& b = vert_arr[indexb];

							bool is_on_line_a01_b = is_point_on_line(b, a0, a1, 1.0f);
							if (is_on_line_a01_b)
							{
								inout_vec_index[i * 3 + (j + 1) % 3] = indexb;

								size_t inserted_idx = (i + 1) * 3;
								i3::vector<i3::uint32_t>::iterator it = inout_vec_index.begin() + inserted_idx;
								inout_vec_index.insert(it, 3, 0);

								inout_vec_index[inserted_idx + j] = indexb;
								inout_vec_index[inserted_idx + (j + 1) % 3 ] = indexa1;
								inout_vec_index[inserted_idx + (j + 2) % 3 ] = indexa2;
								is_t_junction_found = true;
								break;
							}
						}

						if (is_t_junction_found)
							break;
					}

					if (is_t_junction_found)
						break;
				}

				if (is_t_junction_found)		// T Junction발생시 폴리건이 분할되었기 때문에 해당 i부터 다시 해야함..
					t_junction_result = true;
				else
					++i;
			}
			
			return t_junction_result;
		}
	}

	bool delete_t_junction(const i3::uint32_t* index_arr, size_t num_face,
		const vec3* vert_arr, size_t num_vert,
		i3::vector<i3::uint32_t>& out_vec_index)
	{
		out_vec_index.assign(index_arr + 0, index_arr + num_face * 3);
		return delete_t_junction_imp(out_vec_index, vert_arr, num_vert);
	}
	

}