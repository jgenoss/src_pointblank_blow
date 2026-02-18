#pragma once

namespace CaseRun
{
	struct Vec3
	{
		float x, y, z;

		Vec3() {};
		Vec3(float * pArray);
		Vec3(float x, float y, float z);
		Vec3(float scalar);

		Vec3 & operator += (const Vec3 &);
		Vec3 & operator -= (const Vec3 &);
		Vec3 & operator *= (const Vec3 &);
		Vec3 & operator /= (const Vec3 &);
		Vec3 & operator *= (float);
		Vec3 & operator /= (float);

		// binary operators
		Vec3 operator + (const Vec3 &) const;
		Vec3 operator - (const Vec3 &) const;
		Vec3 operator * (const Vec3 &) const;
		Vec3 operator / (const Vec3 &) const;
		Vec3 operator * (float) const;
		Vec3 operator / (float) const;

		Vec3 & operator = (float);
		Vec3 & operator = (float *);
		Vec3 & operator = (const Vec3);

		bool operator == (const Vec3 &) const;
		bool operator != (const Vec3 &) const;

		float	operator [](const INT32 idx) const;
		float & operator [](const INT32 idx);

		bool	IsAlike(const Vec3 & V2, float eps = 0.00002f) const;
		void	Set(float x, float y, float z);
		void	Zero(void);
		float	Normalize(void);

		float	LengthSq(void) const;
		float	Length(void) const;
		
		static float	Dot(const Vec3 & v1, const Vec3 & v2);
		static Vec3		Cross(const Vec3 & v1, const Vec3 & v2);
		static	Vec3	Minimize(const Vec3 & v1, const Vec3 & v2);
		static	Vec3	Maximize(const Vec3 & v1, const Vec3 & v2);
		static  Vec3	LERP(const Vec3 & v1, const Vec3 & v2, float s);
		static Vec3		Normal(const Vec3 & v1, const Vec3 & v2, const Vec3 & v3);

		static const Vec3 &	ZeroVector(void);
		static const Vec3 &	XAxis(void);
		static const Vec3 &	YAxis(void);
		static const Vec3 &	ZAxis(void);
	};
}