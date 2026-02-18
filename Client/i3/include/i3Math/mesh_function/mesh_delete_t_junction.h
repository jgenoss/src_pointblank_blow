#pragma once

// http://www.jkgs.or.kr/journal/article.php?code=6866 의 의사코드 참조
// 너무 단순한 의사코드라서 계산시간은 꽤 걸릴 것으로 보임..

namespace i3
{
	// T 접합부가 없다면 out_vec_index에 아무것도 넣지 않도록 한다..없으면 false리턴..
	struct vec3;

	bool delete_t_junction( const i3::uint32_t* index_arr, size_t num_face,
							const vec3* vert_arr, size_t num_vert,
							i3::vector<i3::uint32_t>& out_vec_index);
	
}
