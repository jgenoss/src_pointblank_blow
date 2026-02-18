
#include "plane.h"
#include "vec3_function.h"
#include "mat4.h"
#include <math.h>

namespace i3
{
	inline const vec3&		get_plane_normal(const plane& pl)
	{
		return reinterpret_cast<const vec3&>(pl);
	}

	inline vec3&			get_plane_normal(plane& pl)
	{
		return reinterpret_cast<vec3&>(pl);
	}

	inline void	make_plane_from_points(plane& pl, const vec3& v0, const vec3& v1, const vec3& v2)
	{
		vec3 v01 = v1;	v01 -= v0; 	vec3 v02 = v2;	v02 -= v0;
		vec3& plane_normal = get_plane_normal(pl);
		i3::vec3_cross(plane_normal, v01, v02);
		i3::vec3_normalize_self(plane_normal);
		pl.d = -i3::vec3_dot(v0, plane_normal);
	}

	inline plane make_plane_from_points(const vec3& v0, const vec3& v1, const vec3& v2)
	{
		vec3 v01 = v1;	v01 -= v0; 	vec3 v02 = v2;	v02 -= v0;
		vec3 n;		i3::vec3_cross(n, v01, v02);
		i3::vec3_normalize_self(n);
		return plane(n.x, n.y, n.z, -i3::vec3_dot(v0, n));
	}
	
	inline void		make_plane_from_point_normal(plane& pl, const vec3& p, const vec3& n)
	{
		pl.a = n.x;		pl.b = n.y;		pl.c = n.z;
		pl.d = -vec3_dot(p, n);
	}

	inline plane	make_plane_from_point_normal(const vec3& p, const vec3& n)
	{
		return i3::plane(n.x, n.y, n.z, -vec3_dot(p, n));
	}

	inline float	plane_normal_abs_dot_vec3(const plane& pl, const vec3& v)
	{
		return fabs(pl.a * v.x) + fabs(pl.b * v.y) + fabs(pl.c * v.z);
	}

	inline float plane_normal_dot_vec3(const plane& pl, const vec3& v)
	{
		return pl.a * v.x + pl.b * v.y + pl.c * v.z;
	}

	inline float plane_dot_vec3(const plane& pl, const vec3& v)
	{
		return pl.a * v.x + pl.b * v.y + pl.c * v.z + pl.d;
	}

	inline float calc_plane_distance(const plane& pl, const vec3& v) // 위 함수와 같은 함수이다. (가독성을 위한 네이밍만 구분됨)
	{
		return pl.a * v.x + pl.b * v.y + pl.c * v.z + pl.d;
	}

	namespace detail
	{
		const float plane_normalize_epsilon = 1e-06f;
	}

	inline plane plane_normal_normalize(const plane& pl)
	{
		float len = plane_normal_length(pl);

		if (len <= detail::plane_normalize_epsilon)
			return plane(0.f, 0.f, 0.f, 0.f);

		return plane(pl.a / len, pl.b / len, pl.c / len, pl.d / len);
	}

	inline void plane_normal_normalize(plane& dest, const plane& src)
	{
		float len = plane_normal_length(src);
		if (len <= detail::plane_normalize_epsilon)
		{
			dest.a = 0.f; dest.b = 0.f; dest.c = 0.f; dest.d = 0.f;
		}
		else
		{
			dest = src;	dest /= len;
		}
	}

	inline void plane_normal_normalize_self(plane& pl)
	{
		float len = plane_normal_length(pl);
		if (len <= detail::plane_normalize_epsilon)
		{
			pl.a = 0.f; pl.b = 0.f; pl.c = 0.f; pl.d = 0.f;
		}
		else
		{
			pl /= len;
		}
	}

	inline float plane_normal_length_sq(const plane& pl)
	{
		return pl.a * pl.a + pl.b * pl.b + pl.c * pl.c;
	}

	inline float plane_normal_length(const plane& pl)
	{
		return sqrtf(plane_normal_length_sq(pl) );
	}

	inline void plane_transform_inv_transpose_mat(plane& out, const plane& pl, const mat4& m)
	{
		plane tmp = pl;
		out.a = m._11*tmp.a + m._21*tmp.b + m._31*tmp.c + m._41*tmp.d;
		out.b = m._12*tmp.a + m._22*tmp.b + m._32*tmp.c + m._42*tmp.d;
		out.c = m._13*tmp.a + m._23*tmp.b + m._33*tmp.c + m._43*tmp.d;
		out.d = m._14*tmp.a + m._24*tmp.b + m._34*tmp.c + m._44*tmp.d;
	}

	inline plane plane_transform_inv_transpose_mat(const plane& pl, const mat4& m)
	{
		return i3::plane
				(
				m._11*pl.a + m._21*pl.b + m._31*pl.c + m._41*pl.d,
				m._12*pl.a + m._22*pl.b + m._32*pl.c + m._42*pl.d,
				m._13*pl.a + m._23*pl.b + m._33*pl.c + m._43*pl.d,
				m._14*pl.a + m._24*pl.b + m._34*pl.c + m._44*pl.d
				);
	}

	inline void	plane_transform_inv_transpose_mat_self(plane& inout, const mat4& m)
	{
		plane tmp = inout;
		inout.a = m._11*tmp.a + m._21*tmp.b + m._31*tmp.c + m._41*tmp.d;
		inout.b = m._12*tmp.a + m._22*tmp.b + m._32*tmp.c + m._42*tmp.d;
		inout.c = m._13*tmp.a + m._23*tmp.b + m._33*tmp.c + m._43*tmp.d;
		inout.d = m._14*tmp.a + m._24*tmp.b + m._34*tmp.c + m._44*tmp.d;
	}

}
