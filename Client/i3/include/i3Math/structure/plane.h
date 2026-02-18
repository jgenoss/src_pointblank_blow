#pragma once

namespace i3
{
	
	struct plane
	{
		plane();
		plane(const float *pf);
		plane(float fa, float fb, float fc, float fd);

		operator float* ();
		operator const float* () const;

		plane& operator *= (float f);
		plane& operator /= (float f);
		
		plane operator + () const;
		plane operator - () const;

		plane operator * (float f) const;
		plane operator / (float f) const;

		bool operator == (const plane& rhs) const;
		bool operator != (const plane& rhs) const;

		float a, b, c, d;
	};
	
	plane operator * (float f, const plane& pl);

	struct __declspec(align(16)) plane_a : plane
	{
		plane_a() {};
		plane_a(const float* f);
		plane_a(const plane& m);
		plane_a(float a, float b, float c, float d);

		void* operator new   (size_t);
		void* operator new[](size_t);
		void operator delete   (void*);   
		void operator delete[](void*);   

		plane_a& operator=(const plane&);
	};
	
}

#include "plane.inl"
