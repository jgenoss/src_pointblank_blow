
namespace i3
{

	inline vec3& vec3::operator=(const vec3& v) { x = v.x; y = v.y; z = v.z; return *this; }

	inline void  vec3::set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }

	inline vec3 vec3::operator -() { return vec3(-x,-y,-z); }
	inline vec3 vec3::operator +() { return vec3(*this); }


	inline vec3& vec3::operator+=(const vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
	inline vec3& vec3::operator-=(const vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	inline vec3& vec3::operator*=(const float f) { x *= f; y *= f; z *= f; return *this; }
	inline vec3& vec3::operator/=(const float f) { x /= f; y /= f; z /= f; return *this; }


	inline vec3 operator + ( const vec3& lhs, const vec3& rhs)
	{
		return vec3(lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z);
	}

	inline vec3 operator - ( const vec3& lhs, const vec3& rhs)
	{
		return vec3(lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z);
	}

	inline vec3 operator * ( const vec3& lhs, float rhs)
	{
		return vec3(lhs.x*rhs, lhs.y*rhs, lhs.z*rhs);
	}

	inline vec3 operator / ( const vec3& lhs, float rhs)
	{
		return vec3(lhs.x/rhs, lhs.y/rhs, lhs.z/rhs);
	}

	inline vec3 operator * ( float lhs, const vec3& rhs)
	{
		return vec3(rhs.x*lhs, rhs.y*lhs, rhs.z*lhs);
	}




}