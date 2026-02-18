#ifndef __I3_MATH_PROPERTY_GRID_DEFINES_H__
#define __I3_MATH_PROPERTY_GRID_DEFINES_H__

#if defined( I3_WINDOWS)

I3_EXPORT_MATH void	AddMFCProperty_Vec3D(	i3List* pList, 
											VEC3D* pVec, const char* pszName, const char* pszDesc = NULL, 
											REAL32 fMin = 0.0f, REAL32 fMax = 0.0f);

I3_EXPORT_MATH void	AddMFCProperty_Vec4D(	i3List* pList, 
											VEC4D* pVec, const char* pszName, const char* pszDesc = NULL, 
											REAL32 fMin = 0.0f, REAL32 fMax = 0.0f);

I3_EXPORT_MATH void	AddMFCProperty_Matrix(	i3List* pList, 
											MATRIX* pMat, const char* pszName, const char* pszDesc = NULL, 
											REAL32 fMin = 0.0f, REAL32 fMax = 0.0f);

I3_EXPORT_MATH void	AddMFCProperty_Color(	i3List* pList, 
											COLOR* pCol, const char* pszName, const char* pszDesc = NULL);

I3_EXPORT_MATH void	AddMFCProperty_Font(	i3List* pList,
											LPLOGFONT lf, COLOR* pCol, const char* pszName, const char* pszDesc = NULL);						

#endif

#endif