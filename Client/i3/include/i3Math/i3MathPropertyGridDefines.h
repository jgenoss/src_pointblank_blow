#ifndef __I3_MATH_PROPERTY_GRID_DEFINES_H__
#define __I3_MATH_PROPERTY_GRID_DEFINES_H__

#if defined( I3_WINDOWS)

#include "i3Vector.h"
#include "i3Color.h"
#include "i3Matrix.h"

I3_EXPORT_MATH void	AddMFCProperty_Vec3D(	i3::vector<PROPERTYINFO*>& List, 
											VEC3D* pVec, const char* pszName, const char* pszDesc = nullptr,
											REAL32 fMin = 0.0f, REAL32 fMax = 0.0f);

I3_EXPORT_MATH void	AddMFCProperty_Vec4D(	i3::vector<PROPERTYINFO*>& List, 
											VEC4D* pVec, const char* pszName, const char* pszDesc = nullptr,
											REAL32 fMin = 0.0f, REAL32 fMax = 0.0f);

I3_EXPORT_MATH void	AddMFCProperty_Matrix(	i3::vector<PROPERTYINFO*>& List, 
											MATRIX* pMat, const char* pszName, const char* pszDesc = nullptr,
											REAL32 fMin = 0.0f, REAL32 fMax = 0.0f);

I3_EXPORT_MATH void	AddMFCProperty_Color(	i3::vector<PROPERTYINFO*>& List, 
											COLOR* pCol, const char* pszName, const char* pszDesc = nullptr);

I3_EXPORT_MATH void	AddMFCProperty_Font(i3::vector<PROPERTYINFO*>& List,
											LPLOGFONTA lf, COLOR* pCol, const char* pszName, const char* pszDesc = nullptr);

#endif

#endif