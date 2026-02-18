
namespace i3
{
	
	inline plane::plane() { }

	inline plane::plane(const float *pf) { ::memcpy(this, pf, sizeof(plane));	}

	inline plane::plane(float a, float b, float c, float d) : a(a), b(b), c(c), d(d) {}

	inline plane::operator float* () { return (float*)&a; }

	inline plane::operator const float* () const { return (const float*)&a; }

	inline plane plane::operator + () const { return *this; }

	inline plane plane::operator - () const {	return plane(-a, -b, -c, -d); }

	// 아래 2개 비교함수는 근사값 비교로 나중에 교체하는게 좋겠다..
	inline bool plane::operator == (const plane& rhs) const { return a == rhs.a && b == rhs.b && c == rhs.c && d == rhs.d; }
	inline bool plane::operator != (const plane& rhs) const { return a != rhs.a || b != rhs.b || c != rhs.c || d != rhs.d; }
	

	inline plane& plane::operator *= (float f)
	{
		a *= f;	b *= f;	c *= f;	d *= f;
		return *this;
	}

	inline plane& plane::operator /= (float f)
	{
		float inv = 1.f / f;
		a *= inv;	b *= inv;	c *= inv;	d *= inv;
		return *this;
	}

	inline plane plane::operator * (float f) const
	{
		return plane(a * f, b * f, c * f, d * f);
	}

	inline plane plane::operator / (float f) const
	{
		float inv = 1.f / f;
		return plane(a * inv, b * inv, c * inv, d * inv);
	}

	inline plane operator * (float f, const plane& pl)
	{
		return plane(pl.a * f, pl.b * f, pl.c * f, pl.d * f);
	}

	inline plane_a::plane_a(const float* f) : plane(f) { }

	inline plane_a::plane_a(const plane& rhs) : plane(rhs) { }
	inline plane_a::plane_a(float a, float b, float c, float d) : plane(a, b, c, d) { }

	inline void* plane_a::operator new (size_t s)
	{
		return i3::static_pool_malloc(s);
	}

	inline void* plane_a::operator new[](size_t s)
	{
		return i3::static_pool_malloc(s);
	}

	inline void  plane_a::operator delete(void* p)
	{
		i3::static_pool_free(p);
	}

	inline void  plane_a::operator delete[](void* p)
	{
		i3::static_pool_free(p);
	}

	inline plane_a& plane_a::operator=(const plane& rhs)
	{
		::memcpy(this, &rhs, sizeof(plane));
		return *this;
	}
	
}


