#pragma once

//#if 0
// WIN32의 min, max 매크로 방해 차단..

#include "vec3.h"

#pragma push_macro("min")
#undef min

#pragma push_macro("max")
#undef max

namespace i3
{
	struct mat4;

	class aabb
	{
	public:
		aabb();
		aabb(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z);
		aabb(const vec3& p_min, const vec3& p_max);
		aabb(const aabb& a); 
		aabb(const vec3& v);

		void				identity();
		void				reset();
		void				reset(const vec3& v);
		bool				is_valid() const;

		vec3&			min();							vec3&			max();
		const vec3&		min() const;					const vec3&		max() const;
		void			min(const vec3& v);				void			max(const vec3& v);

		void				merge(const vec3& v);
		void				merge(const aabb& a);

		void				set_center_extent(const vec3& center, const vec3& extent);

		void				calc_center(vec3& outCenter) const;
		void				calc_extent(vec3& outExtent) const;
		void				calc_length(vec3& outLength) const;

		vec3			calc_center() const;
		vec3			calc_extent() const;
		vec3			calc_length() const;

		vec3			calc_vertex(int is_max_x, int is_max_y, int is_max_z ) const; 
		void			calc_vertex(vec3& out, int is_max_x, int is_max_y, int is_max_z ) const; 

		float			calc_vertex_x(int is_max) const; 
		float			calc_vertex_y(int is_max) const; 
		float			calc_vertex_z(int is_max) const; 


		aabb&	operator=(const aabb& a);
		aabb&	operator+=( const vec3& rhs);
		aabb&	operator*=( const vec3& rhs); 

		I3_EXPORT_MATH void			transform(i3::aabb& out, const i3::mat4& m) const;
	private:
		// 순서 바뀌면 안됨..(순서의존적 구현이 calc_vertex_x,y,z함수에 존재)
		vec3					m_min;	
		vec3					m_max;	
	};
	
//	void transform_aabb


	I3_EXPORT_MATH bool aabb_triangle(const i3::aabb& a, const i3::vec3& v0, const i3::vec3& v1, const i3::vec3& v2);
	I3_EXPORT_MATH bool aabb_triangle(const i3::vec3& aabb_center, const i3::vec3& aabb_extent, const i3::vec3& v0, const i3::vec3& v1, const i3::vec3& v2);
	I3_EXPORT_MATH bool aabb_triangle2(const i3::vec3& aabb_center, const i3::vec3& aabb_extent, const i3::vec3& v0, const i3::vec3& v1, const i3::vec3& v2);
}




#include "aabb.inl"

#pragma pop_macro("max")
#pragma pop_macro("min")

//#endif