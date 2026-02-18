#include "i3MathType.h"
#include "i3MathPropertyGridDefines.h"

//////////////////////////////////////////////////////////////////////
void	AddMFCProperty_Vec3D(	i3::vector<PROPERTYINFO*>& List, 
								VEC3D* pVec, const char* pszName, const char* pszDesc, 
								REAL32 fMin, REAL32 fMax)
{

	UINT32 nFlag = PROPERTYFLAG_NONE;
	REAL32*	pUserData = nullptr;
	
	if( fMin < fMax)
	{
		nFlag |= PROPERTYFLAG_HASRANGE;
		pUserData = new REAL32[2];
		NetworkDump_Malloc( pUserData, sizeof( REAL32) * 2, __FILE__, __LINE__);
		pUserData[0] = fMin;
		pUserData[1] = fMax;
	}

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_VEC3D, pVec, pszName, pszDesc, nFlag, pUserData);

	List.push_back( pProp);
}

void	AddMFCProperty_Vec4D(	i3::vector<PROPERTYINFO*>& List, 
								VEC4D* pVec, const char* pszName, const char* pszDesc, 
								REAL32 fMin, REAL32 fMax)
{

	UINT32 nFlag = PROPERTYFLAG_NONE;
	REAL32*	pUserData = nullptr;

	if( fMin < fMax)
	{
		nFlag |= PROPERTYFLAG_HASRANGE;
		pUserData = new REAL32[2];
		NetworkDump_Malloc( pUserData, sizeof( REAL32) * 2, __FILE__, __LINE__);
		pUserData[0] = fMin;
		pUserData[1] = fMax;
	}

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_VEC4D, pVec, pszName, pszDesc, nFlag, pUserData);

	List.push_back( pProp);
}

void	AddMFCProperty_Matrix(	i3::vector<PROPERTYINFO*>& List, 
								MATRIX* pMat, const char* pszName, const char* pszDesc, 
								REAL32 fMin, REAL32 fMax)
{

	UINT32 nFlag = PROPERTYFLAG_NONE;
	REAL32*	pUserData = nullptr;

	if( fMin < fMax)
	{
		nFlag |= PROPERTYFLAG_HASRANGE;
		pUserData = new REAL32[2];
		NetworkDump_Malloc( pUserData, sizeof( REAL32) * 2, __FILE__, __LINE__);
		pUserData[0] = fMin;
		pUserData[1] = fMax;
	}

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	
	pProp->Set( EPT_MATRIX, pMat, pszName, pszDesc, nFlag, pUserData);

	List.push_back( pProp);
}

void	AddMFCProperty_Color(	i3::vector<PROPERTYINFO*>& List, 
								COLOR* pCol, const char* pszName, const char* pszDesc)
{
	UINT32 nFlag = PROPERTYFLAG_NONE;
	REAL32*	pUserData = nullptr;

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_COLOR, pCol, pszName, pszDesc, nFlag, pUserData);

	List.push_back( pProp);
}

void	AddMFCProperty_Font(	i3::vector<PROPERTYINFO*>& List,
								LPLOGFONTA lf, COLOR* pCol, const char* pszName, const char* pszDesc)
{

	PROPERTYINFO* pProp = (PROPERTYINFO*) i3MemAlloc( sizeof( PROPERTYINFO));
	pProp->Set( EPT_FONT, lf, pszName, pszDesc, 0, pCol);
	List.push_back( pProp);
}