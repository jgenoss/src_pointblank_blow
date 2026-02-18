#include "i3MathType.h"
#include "structure/aabb.h"
#include "structure/mat4.h"
#include "structure/vec3_function.h"

#pragma push_macro("min")
#undef min

#pragma push_macro("max")
#undef max

namespace i3
{
	void	aabb::transform(i3::aabb& out, const i3::mat4& m) const
	{
		// aabb의 트랜스폼은 단순 min,max 변환만으로 처리될수 없다...
		vec3 center_obj;		this->calc_center(center_obj);
		vec3 extent_obj;		this->calc_extent(extent_obj);

		// 먼저 center를 mat에 넣어 좌표 변경... Vec3TransformCoord함수를 호출해도 되지만, 일단 직접 곱하기로 했다..
		vec3 result_center;
		result_center.x = (m.m[0][0] * center_obj.x + m.m[1][0] * center_obj.y + m.m[2][0] * center_obj.z + m.m[3][0]);
		result_center.y = (m.m[0][1] * center_obj.x + m.m[1][1] * center_obj.y + m.m[2][1] * center_obj.z + m.m[3][1]);
		result_center.z = (m.m[0][2] * center_obj.x + m.m[1][2] * center_obj.y + m.m[2][2] * center_obj.z + m.m[3][2]);

		// extent의 경우 mat의 회전축과 연동되는데, 이때 각 axis축의 해당x,y,z성분이 음이라도 양값이 되도록 fabs처리를 한다...
		// Vec3TransformNormal과 유사하지만, 축성분이 음이면 모두 양값처리한다..
		vec3 semiLength;
		semiLength.x = fabs(m.m[0][0]) * extent_obj.x + fabs(m.m[1][0]) * extent_obj.y + fabs(m.m[2][0]) * extent_obj.z;
		semiLength.y = fabs(m.m[0][1]) * extent_obj.x + fabs(m.m[1][1]) * extent_obj.y + fabs(m.m[2][1]) * extent_obj.z;
		semiLength.z = fabs(m.m[0][2]) * extent_obj.x + fabs(m.m[1][2]) * extent_obj.y + fabs(m.m[2][2]) * extent_obj.z;

		// 이제 min, max값을 구할수 있는데, 변환 중심점에 구해진 반길이 벡터를 빼거나 더하면 된다.

		out.min() = result_center;		out.min() -= semiLength;
		out.max() = result_center;		out.max() += semiLength;
	}

	// RealTime Collision Dectection과 Akeinine-Moller 의사코드 참고했습니다..

	bool aabb_triangle(const i3::aabb& a, const i3::vec3& v0, const i3::vec3& v1, const i3::vec3& v2)
	{
		return aabb_triangle(a.calc_center(), a.calc_extent(), v0, v1, v2);
	}

	bool aabb_triangle(const i3::vec3& aabb_center, const i3::vec3& aabb_extent, const i3::vec3& v0, const i3::vec3& v1, const i3::vec3& v2)
	{
		// 1. 삼각형을 aabb 중심으로 이동하여 로컬계로 변경..
		i3::vec3 lv0(v0);		lv0 -= aabb_center;
		i3::vec3 lv1(v1);		lv1 -= aabb_center;
		i3::vec3 lv2(v2);		lv2 -= aabb_center;

		// 2.  삼각형의 모서리 벡터 구함...
		i3::vec3 edge01 = lv1;	edge01 -= lv0;
		i3::vec3 edge12 = lv2;	edge12 -= lv1;
		i3::vec3 edge20 = lv0;	edge20 -= lv2;

		// 분리축 테스트.. (9개의 축으로 테스트한다고 함)
		// aabb 절대축 3개 대비 삼각형 3모서리벡터의 외적으로 축 9개 생성...


		float fex, fey, fez;
		float p0, p1, p2;
		float min, max;
		float rad;

		fex = fabs(edge01.x);	fey = fabs(edge01.y);	fez = fabs(edge01.z);

		// 	AXISTEST_X01(edge01.z, edge01.y, fez, fey);

		p0 = edge01.z*lv0.y - edge01.y*lv0.z;	
		p2 = edge01.z*lv2.y - edge01.y*lv2.z;	
		if (p0 < p2) { min = p0; max = p2; }
		else { min = p2; max = p0; } 
		rad = fez * aabb_extent.y + fey * aabb_extent.z;  
		if (min > rad || max < -rad) return false;

		// AXISTEST_Y02(edge01.z, edge01.x, fez, fex);

		p0 = -edge01.z*lv0.x + edge01.x*lv0.z;
		p2 = -edge01.z*lv2.x + edge01.x*lv2.z;
		if (p0 < p2) { min = p0; max = p2; }
		else { min = p2; max = p0; }
		rad = fez * aabb_extent.x + fex * aabb_extent.z;  
		if (min > rad || max < -rad) return false;

		// AXISTEST_Z12(edge01.y, edge01.x, fey, fex);

		p1 = edge01.y*lv1.x - edge01.x*lv1.y;			
		p2 = edge01.y*lv2.x - edge01.x*lv2.y;			   
		if (p2 < p1) { min = p2; max = p1; }
		else { min = p1; max = p2; } 
		rad = fey * aabb_extent.x + fex * aabb_extent.y; 
		if (min > rad || max < -rad) return false;

		fex = fabs(edge12.x);	fey = fabs(edge12.y);	fez = fabs(edge12.z);

//		AXISTEST_X01(edge12.z, edge12.y, fez, fey);

		p0 = edge12.z*lv0.y - edge12.y*lv0.z;	
		p2 = edge12.z*lv2.y - edge12.y*lv2.z;	
		if (p0 < p2) { min = p0; max = p2; }
		else { min = p2; max = p0; }
		rad = fez * aabb_extent.y + fey * aabb_extent.z; 
		if (min > rad || max < -rad) return false;
		
//		AXISTEST_Y02(edge12.z, edge12.x, fez, fex);

		p0 = -edge12.z*lv0.x + edge12.x*lv0.z;	
		p2 = -edge12.z*lv2.x + edge12.x*lv2.z;	 
		if (p0 < p2) { min = p0; max = p2; }
		else { min = p2; max = p0; } 
		rad = fez * aabb_extent.x + fex * aabb_extent.z;
		if (min > rad || max < -rad) return false;

//		AXISTEST_Z0(edge12.y, edge12.x, fey, fex);

		p0 = edge12.y*lv0.x - edge12.x*lv0.y;		
		p1 = edge12.y*lv1.x - edge12.x*lv1.y;		
		if (p0 < p1) { min = p0; max = p1; }
		else { min = p1; max = p0; } 
		rad = fey * aabb_extent.x + fex * aabb_extent.y; 
		if (min > rad || max < -rad) return false;

		fex = fabs(edge20.x);	fey = fabs(edge20.y);	fez = fabs(edge20.z);

//		AXISTEST_X2(edge20.z, edge20.y, fez, fey);

		p0 = edge20.z*lv0.y - edge20.y*lv0.z;	
		p1 = edge20.z*lv1.y - edge20.y*lv1.z;
		if (p0 < p1) { min = p0; max = p1; }
		else { min = p1; max = p0; } 
		rad = fez * aabb_extent.y + fey * aabb_extent.z; 
		if (min > rad || max < -rad) return false;
		
//		AXISTEST_Y1(edge20.z, edge20.x, fez, fex);

		p0 = -edge20.z*lv0.x + edge20.x*lv0.z;	
		p1 = -edge20.z*lv1.x + edge20.x*lv1.z;	 
		if (p0 < p1) { min = p0; max = p1; }
		else { min = p1; max = p0; } 
		rad = fez * aabb_extent.x + fex * aabb_extent.z;
		if (min > rad || max < -rad) return false;
		
//		AXISTEST_Z12(edge20.y, edge20.x, fey, fex);

		p1 = edge20.y*lv1.x - edge20.x*lv1.y;	
		p2 = edge20.y*lv2.x - edge20.x*lv2.y;	
		if (p2 < p1) { min = p2; max = p1; }
		else { min = p1; max = p2; } 
		rad = fey * aabb_extent.x + fex * aabb_extent.y;
		if (min > rad || max < -rad) return false;

		// 2. aabb 각face normal 축에 대해 삼각형의 최소,최대점과의 교차 여부를 따지는 부분..

//		FINDMINMAX(lv0.x, lv1.x, lv2.x, min, max);

		min = max = lv0.x;   
		if (lv1.x<min) min = lv1.x; 
		if (lv1.x>max) max = lv1.x; 
		if (lv2.x<min) min = lv2.x; 
		if (lv2.x>max) max = lv2.x;
		
		if (min > aabb_extent.x || max < -aabb_extent.x) return false;

//		FINDMINMAX(lv0.y, lv1.y, lv2.y, min, max);

		min = max = lv0.y; 
		if (lv1.y<min) min = lv1.y;
		if (lv1.y>max) max = lv1.y;
		if (lv2.y<min) min = lv2.y; 
		if (lv2.y>max) max = lv2.y;
		if (min > aabb_extent.y || max < -aabb_extent.y) return false;

//		FINDMINMAX(lv0.z, lv1.z, lv2.z, min, max);

		min = max = lv0.z; 
		if (lv1.z<min) min = lv1.z;
		if (lv1.z>max) max = lv1.z; 
		if (lv2.z<min) min = lv2.z;
		if (lv2.z>max) max = lv2.z;
		if (min > aabb_extent.z || max < -aabb_extent.z) return false;

		// 3. 마지막으로 삼각형의 평면 노멀과 박스간 교차여부... (이 경우도 로컬계로 간주하여 처리..)

		i3::vec3 triNormal;		i3::vec3_cross(triNormal, edge01, edge12);
		float    triDist = fabs( i3::vec3_dot(triNormal, lv0) );

		float fMaxDist = fabs(triNormal.x*aabb_extent.x) + fabs(triNormal.y*aabb_extent.y) + fabs(triNormal.z * aabb_extent.z);

		return fMaxDist >= triDist;				// 교차하는 것 이외에는 모두 false 처리...
	}

	bool aabb_triangle2(const i3::vec3& aabb_center, const i3::vec3& aabb_extent, const i3::vec3& v0, const i3::vec3& v1, const i3::vec3& v2)
	{
		// 1. 삼각형을 aabb 중심으로 이동하여 로컬계로 변경..
		i3::vec3 lv0(v0);		lv0 -= aabb_center;
		i3::vec3 lv1(v1);		lv1 -= aabb_center;
		i3::vec3 lv2(v2);		lv2 -= aabb_center;

		// 2.  삼각형의 모서리 벡터 구함...
		i3::vec3 edge01 = lv1;	edge01 -= lv0;
		i3::vec3 edge12 = lv2;	edge12 -= lv1;
		i3::vec3 edge20 = lv0;	edge20 -= lv2;

		// 분리축 테스트.. (9개의 축으로 테스트한다고 함)
		// aabb 절대축 3개 대비 삼각형 3모서리벡터의 외적으로 축 9개 생성...
		
		float fex, fey, fez;
		float p0, p1, p2;
		float min, max;
		float rad;

		fex = fabs(edge01.x);	fey = fabs(edge01.y);	fez = fabs(edge01.z);

		// 	AXISTEST_X01(edge01.z, edge01.y, fez, fey);

		p0 = edge01.z*lv0.y - edge01.y*lv0.z;
		p2 = edge01.z*lv2.y - edge01.y*lv2.z;
		if (p0 < p2) { min = p0; max = p2; }
		else { min = p2; max = p0; }
		rad = fez * aabb_extent.y + fey * aabb_extent.z;
		if (min > rad || max < -rad) return false;

		// AXISTEST_Y02(edge01.z, edge01.x, fez, fex);

		p0 = -edge01.z*lv0.x + edge01.x*lv0.z;
		p2 = -edge01.z*lv2.x + edge01.x*lv2.z;
		if (p0 < p2) { min = p0; max = p2; }
		else { min = p2; max = p0; }
		rad = fez * aabb_extent.x + fex * aabb_extent.z;
		if (min > rad || max < -rad) return false;

		// AXISTEST_Z12(edge01.y, edge01.x, fey, fex);

		p1 = edge01.y*lv1.x - edge01.x*lv1.y;
		p2 = edge01.y*lv2.x - edge01.x*lv2.y;
		if (p2 < p1) { min = p2; max = p1; }
		else { min = p1; max = p2; }
		rad = fey * aabb_extent.x + fex * aabb_extent.y;
		if (min > rad || max < -rad) return false;

		fex = fabs(edge20.x);	fey = fabs(edge20.y);	fez = fabs(edge20.z);

		//		AXISTEST_X2(edge20.z, edge20.y, fez, fey);

		p0 = edge20.z*lv0.y - edge20.y*lv0.z;
		p1 = edge20.z*lv1.y - edge20.y*lv1.z;
		if (p0 < p1) { min = p0; max = p1; }
		else { min = p1; max = p0; }
		rad = fez * aabb_extent.y + fey * aabb_extent.z;
		if (min > rad || max < -rad) return false;

		//		AXISTEST_Y1(edge20.z, edge20.x, fez, fex);

		p0 = -edge20.z*lv0.x + edge20.x*lv0.z;
		p1 = -edge20.z*lv1.x + edge20.x*lv1.z;
		if (p0 < p1) { min = p0; max = p1; }
		else { min = p1; max = p0; }
		rad = fez * aabb_extent.x + fex * aabb_extent.z;
		if (min > rad || max < -rad) return false;

		//		AXISTEST_Z12(edge20.y, edge20.x, fey, fex);

		p1 = edge20.y*lv1.x - edge20.x*lv1.y;
		p2 = edge20.y*lv2.x - edge20.x*lv2.y;
		if (p2 < p1) { min = p2; max = p1; }
		else { min = p1; max = p2; }
		rad = fey * aabb_extent.x + fex * aabb_extent.y;
		if (min > rad || max < -rad) return false;
			
		return true;				// 교차하는 것 이외에는 모두 false 처리...
	}
	/*
	bool aabb_triangle2(const i3::vec3& aabb_center, const i3::vec3& aabb_extent, const i3::vec3& v0, const i3::vec3& v1, const i3::vec3& v2)
	{
		bool bRes1 = aabb_triangle(aabb_center, aabb_extent, v0, v1, v2);
		bool bRes2 = aabb_triangle3(aabb_center, aabb_extent, v0, v1, v2);

		I3ASSERT(bRes1 == bRes2);

		return bRes2;
	}
	*/

	/*
	bool aabb_triangle2(const i3::vec3& aabb_center, const i3::vec3& aabb_extent, const i3::vec3& v0, const i3::vec3& v1, const i3::vec3& v2)
	{
		// 1. 삼각형을 aabb 중심으로 이동하여 로컬계로 변경..
		i3::vec3 lv0(v0);		lv0 -= aabb_center;
		i3::vec3 lv1(v1);		lv1 -= aabb_center;
		i3::vec3 lv2(v2);		lv2 -= aabb_center;

		// 2.  삼각형의 모서리 벡터 구함...
		i3::vec3 edge01 = lv1;	edge01 -= lv0;
		i3::vec3 edge12 = lv2;	edge12 -= lv1;
		i3::vec3 edge20 = lv0;	edge20 -= lv2;

		// 분리축 테스트.. (9개의 축으로 테스트한다고 함)
		// aabb 절대축 3개 대비 삼각형 3모서리벡터의 외적으로 축 9개 생성...


		float fex, fey, fez;
		float p0; // , p1, p2;
		float min, max;
		float rad;

		fex = fabs(edge01.x);	fey = fabs(edge01.y);	fez = fabs(edge01.z);

		// 	AXISTEST_X01(edge01.z, edge01.y, fez, fey);

		p0 = edge01.z*lv0.y - edge01.y*lv0.z;
	
		if (p0 < 0.f) { min = p0; max = 0.f; }
		else { min = 0.f; max = p0; }
		rad = fez * aabb_extent.y + fey * aabb_extent.z;
		if (min > rad || max < -rad) return false;

		// AXISTEST_Y02(edge01.z, edge01.x, fez, fex);

		p0 = -edge01.z*lv0.x + edge01.x*lv0.z;
	
		if (p0 < 0.f) { min = p0; max = 0.f; }
		else { min = 0.f; max = p0; }
		rad = fez * aabb_extent.x + fex * aabb_extent.z;
		if (min > rad || max < -rad) return false;

		fex = fabs(edge12.x);	fey = fabs(edge12.y);	fez = fabs(edge12.z);

		//		AXISTEST_X01(edge12.z, edge12.y, fez, fey);

		p0 = edge12.z*lv0.y - edge12.y*lv0.z;
	
		if (p0 < 0.f) { min = p0; max = 0.f; }
		else { min = 0.f; max = p0; }
		rad = fez * aabb_extent.y + fey * aabb_extent.z;
		if (min > rad || max < -rad) return false;

		//		AXISTEST_Y02(edge12.z, edge12.x, fez, fex);

		p0 = -edge12.z*lv0.x + edge12.x*lv0.z;
	
		if (p0 < 0.f) { min = p0; max = 0.f; }
		else { min = 0.f; max = p0; }
		rad = fez * aabb_extent.x + fex * aabb_extent.z;
		if (min > rad || max < -rad) return false;

		//		AXISTEST_Z0(edge12.y, edge12.x, fey, fex);

		p0 = edge12.y*lv0.x - edge12.x*lv0.y;
		if (p0 < 0.f) { min = p0; max = 0.f; }
		else { min = 0.f; max = p0; }
		rad = fey * aabb_extent.x + fex * aabb_extent.y;
		if (min > rad || max < -rad) return false;

		fex = fabs(edge20.x);	fey = fabs(edge20.y);	fez = fabs(edge20.z);

		//		AXISTEST_X2(edge20.z, edge20.y, fez, fey);

		p0 = edge20.z*lv0.y - edge20.y*lv0.z;
	
		if (p0 < 0.f) { min = p0; max = 0.f; }
		else { min = 0.f; max = p0; }
		rad = fez * aabb_extent.y + fey * aabb_extent.z;
		if (min > rad || max < -rad) return false;

		//		AXISTEST_Y1(edge20.z, edge20.x, fez, fex);

		p0 = -edge20.z*lv0.x + edge20.x*lv0.z;
	
		if (p0 < 0.f) { min = p0; max = 0.f; }
		else { min = 0.f; max = p0; }
		rad = fez * aabb_extent.x + fex * aabb_extent.z;
		if (min > rad || max < -rad) return false;

		return true;
		
		// 3. 마지막으로 삼각형의 평면 노멀과 박스간 교차여부... (이 경우도 로컬계로 간주하여 처리..)

//		i3::vec3 triNormal;		i3::vec3_cross(triNormal, edge01, edge12);
//		float    triDist = fabs(i3::vec3_dot(triNormal, lv0));

//		float fMaxDist = fabs(triNormal.x*aabb_extent.x) + fabs(triNormal.y*aabb_extent.y) + fabs(triNormal.z * aabb_extent.z);

//		return fMaxDist >= triDist;				// 교차하는 것 이외에는 모두 false 처리...
		
//	}
*/

}


#pragma pop_macro("max")
#pragma pop_macro("min")
