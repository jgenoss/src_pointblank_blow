#include "i3MathType.h"

#include "structure/box_plane_np_lookup.h"

namespace i3
{
	namespace detail
	{
		//	z -> y -> x 순임
		//     참고자료는 Efficient View Frustum Culling //  http://www.cescg.org/CESCG-2002/DSykoraJJelinek/
		//     참고자료와 달리 프러스텀 노멀을 안쪽으로 향하게 했으므로, 비교대상인 n_vert와 p_vert는 평면 교차함수에서 그 역할이 바뀐다..
		//     p_vert와 평면간 비교시 음이면 네거티브, p_vert, n_vert와 평면간 비교시 모두 양이면 포지티브로 정한다..

		const int3	box_plane_np_lookup::n_vert[8] =
		{
			int3(1, 1, 1),
			int3(1, 1, 0),
			int3(1, 0, 1),
			int3(1, 0, 0),
			int3(0, 1, 1),
			int3(0, 1, 0),
			int3(0, 0, 1),
			int3(0, 0, 0)
		};

		const int3	box_plane_np_lookup::p_vert[8] =
		{
			int3(0, 0, 0),
			int3(0, 0, 1),
			int3(0, 1, 0),
			int3(0, 1, 1),
			int3(1, 0, 0),
			int3(1, 0, 1),
			int3(1, 1, 0),
			int3(1, 1, 1)
		};
		
	}

}
