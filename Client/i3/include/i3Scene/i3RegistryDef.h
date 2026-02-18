#if !defined( _I3REGISTRY_DEF_H__)
#define _I3REGISTRY_DEF_H__

#include "i3Registry.h"

#define	IR_INT32		i3RegINT32
#define IR_REAL32		i3RegREAL32
#define IR_STRING		i3RegString
#define IR_VEC2D		i3RegVEC2D
#define IR_VEC3D		i3RegVEC3D
#define IR_VEC4D		i3RegVEC4D
#define IR_COLOR		i3RegCOLOR
#define IR_MATRIX		i3RegMATRIX

#define IRTOTYPE(t,n)				(((t*)n)->getValue())

#define IRTO_INT32(pirINT32)		(((IR_INT32*)pirINT32)->getValue())
#define IRTO_REAL32(pirREAL32)		(((IR_REAL32*)pirREAL32)->getValue())
#define IRTO_STRING(pirString)		(((IR_STRING*)pirString)->getValue())
#define IRTO_VEC2D(pirVec2d)		(((IR_VEC2D*)pirVec2d)->getValue())
#define IRTO_VEC3D(pirVec3d)		(((IR_VEC3D*)pirVec3d)->getValue())
#define IRTO_VEC4D(pirVec4d)		(((IR_VEC4D*)pirVec4d)->getValue())
#define IRTO_COLOR(pirCol)			(((IR_COLOR*)pirCol)->getValue())
#define IRTO_MATRIX(pirMatrix)		(((IR_MATRIX*)pirMatrix)->getValue())

//#define FIND_REG_KEY(s)			i3RegKey * pKey = i3Registry::FindKey(s);\
//									if( pKey != nullptr)

#define FIND_REG_KEY(s)				i3Registry::FindKey(s)

//#define FIND_REG_KEY_FROM(s,r)	i3RegKey * pKey = i3Registry::FindKey(s,r);\
//									if( pKey != nullptr)

#define FIND_REG_KEY_FROM(s,r)		i3Registry::FindKey(s, r)

#define IS_REG_DATA(k,s)					i3Registry::FindData(k,s)


//읽어 들인 Pef 파일에 내용을 얻기 방식
//1. 분활된 Pef 파일(단일 값)
//2. 통합된 Pef 파일(다중 값)
//3. 통합된 Pef 파일에 단일 값(다중-단일값)

//단일 i3RegData에 값을 얻어 온다.
I3_EXPORT_SCENE inline void GET_REG_SINGLE_DATA(i3RegData * pData, INT32* pValue)
{
	if( pData == nullptr )		return;
	pData->GetValue( pValue );
}
I3_EXPORT_SCENE inline void GET_REG_SINGLE_DATA(i3RegData * pData, REAL32* pValue)
{
	if( pData == nullptr )		return;
	pData->GetValue( pValue );
}
I3_EXPORT_SCENE inline void GET_REG_SINGLE_DATA(i3RegData * pData, i3::rc_wstring& outValue)
{
	if( pData == nullptr )		return;
	pData->GetValue( outValue );
}
I3_EXPORT_SCENE inline void GET_REG_SINGLE_DATA(i3RegData * pData, VEC2D* pValue)
{
	if( pData == nullptr )		return;
	pData->GetValue( pValue );
}
I3_EXPORT_SCENE inline void GET_REG_SINGLE_DATA(i3RegData * pData, VEC3D* pValue)
{
	if( pData == nullptr )		return;
	pData->GetValue( pValue );
}
I3_EXPORT_SCENE inline void GET_REG_SINGLE_DATA(i3RegData * pData, VEC4D* pValue)
{
	if( pData == nullptr )		return;
	pData->GetValue( pValue );
}
I3_EXPORT_SCENE inline void GET_REG_SINGLE_DATA(i3RegData * pData, I3COLOR* pValue)
{
	if( pData == nullptr )		return;
	pData->GetValue( pValue );
}
I3_EXPORT_SCENE inline void GET_REG_SINGLE_DATA(i3RegData * pData, MATRIX* pValue)
{
	if( pData == nullptr )		return;
	pData->GetValue( pValue );
}

//다중 i3RegData에 값을 얻어 온다.
I3_EXPORT_SCENE inline void GET_REG_ARRAY_DATA(i3RegArray * pArrayData, INT32 * pValue)
{
	if( pArrayData == nullptr )		return;
	if( pArrayData->getSize() == 1 ) 
		(*pArrayData)[ 0 ]->GetValue( pValue );
	else 
		(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( pValue );
}
I3_EXPORT_SCENE inline void GET_REG_ARRAY_DATA(i3RegArray * pArrayData, REAL32 * pValue)
{
	if( pArrayData == nullptr )		return;
	if( pArrayData->getSize() == 1 ) 
		(*pArrayData)[ 0 ]->GetValue( pValue );
	else 
		(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( pValue );
}
I3_EXPORT_SCENE inline void GET_REG_ARRAY_DATA(i3RegArray * pArrayData, i3::rc_wstring& outValue)
{
	if( pArrayData == nullptr )		return;
	if( pArrayData->getSize() == 1 ) 
		(*pArrayData)[ 0 ]->GetValue( outValue );
	else 
		(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( outValue );
}
I3_EXPORT_SCENE inline void GET_REG_ARRAY_DATA(i3RegArray * pArrayData, VEC2D * pValue)
{
	if( pArrayData == nullptr )		return;
	if( pArrayData->getSize() == 1 ) 
		(*pArrayData)[ 0 ]->GetValue( pValue );
	else 
		(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( pValue );
}
I3_EXPORT_SCENE inline void GET_REG_ARRAY_DATA(i3RegArray * pArrayData, VEC3D * pValue)
{
	if( pArrayData == nullptr )		return;
	if( pArrayData->getSize() == 1 ) 
		(*pArrayData)[ 0 ]->GetValue( pValue );
	else 
		(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( pValue );
}
I3_EXPORT_SCENE inline void GET_REG_ARRAY_DATA(i3RegArray * pArrayData, VEC4D * pValue)
{
	if( pArrayData == nullptr )		return;
	if( pArrayData->getSize() == 1 ) 
		(*pArrayData)[ 0 ]->GetValue( pValue );
	else 
		(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( pValue );
}
I3_EXPORT_SCENE inline void GET_REG_ARRAY_DATA(i3RegArray * pArrayData, I3COLOR * pValue)
{
	if( pArrayData == nullptr )		return;
	if( pArrayData->getSize() == 1 ) 
		(*pArrayData)[ 0 ]->GetValue( pValue );
	else 
		(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( pValue );
}
I3_EXPORT_SCENE inline void GET_REG_ARRAY_DATA(i3RegArray * pArrayData, MATRIX * pValue)
{
	if( pArrayData == nullptr )		return;
	if( pArrayData->getSize() == 1 ) 
		(*pArrayData)[ 0 ]->GetValue( pValue );
	else 
		(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( pValue );
}

//단일값 i3RegKey 또는 다중 i3RegKey에 해당되는 데이터를 읽어 들인다.
I3_EXPORT_SCENE inline void GET_REG_DATA(i3RegData * pData, INT32 * pValue)
{
	if( pData == nullptr )		return;
	if( pData->getDataType() == I3REG_TYPE_ARRAY )	
	{
		GET_REG_ARRAY_DATA((i3RegArray*)pData, pValue);
	}
	else
	{
		GET_REG_SINGLE_DATA(pData, pValue);
	}
}
I3_EXPORT_SCENE inline void GET_REG_DATA(i3RegData* pData, REAL32 * pValue)
{
	if( pData == nullptr )		return;
	if( pData->getDataType() == I3REG_TYPE_ARRAY )	
	{
		GET_REG_ARRAY_DATA((i3RegArray*)pData, pValue);
	}
	else
	{
		GET_REG_SINGLE_DATA(pData, pValue);
	}
}
I3_EXPORT_SCENE inline void GET_REG_DATA(i3RegData* pData, i3::rc_wstring& outValue )
{
	if( pData == nullptr )		return;
	if( pData->getDataType() == I3REG_TYPE_ARRAY )	
	{
		GET_REG_ARRAY_DATA((i3RegArray*)pData, outValue);
	}
	else
	{
		GET_REG_SINGLE_DATA(pData, outValue);
	}
}
I3_EXPORT_SCENE inline void GET_REG_DATA(i3RegData* pData, VEC2D * pValue)
{
	if( pData == nullptr )		return;
	if( pData->getDataType() == I3REG_TYPE_ARRAY )	
	{
		GET_REG_ARRAY_DATA((i3RegArray*)pData, pValue);
	}
	else
	{
		GET_REG_SINGLE_DATA(pData, pValue);
	}
}
I3_EXPORT_SCENE inline void GET_REG_DATA(i3RegData* pData, VEC3D * pValue)
{
	if( pData == nullptr )		return;
	if( pData->getDataType() == I3REG_TYPE_ARRAY )	
	{
		GET_REG_ARRAY_DATA((i3RegArray*)pData, pValue);
	}
	else
	{
		GET_REG_SINGLE_DATA(pData, pValue);
	}
}
I3_EXPORT_SCENE inline void GET_REG_DATA(i3RegData* pData, VEC4D * pValue)
{
	if( pData == nullptr )		return;
	if( pData->getDataType() == I3REG_TYPE_ARRAY )	
	{
		GET_REG_ARRAY_DATA((i3RegArray*)pData, pValue);
	}
	else
	{
		GET_REG_SINGLE_DATA(pData, pValue);
	}
}
I3_EXPORT_SCENE inline void GET_REG_DATA(i3RegData* pData, I3COLOR * pValue)
{
	if( pData == nullptr )		return;
	if( pData->getDataType() == I3REG_TYPE_ARRAY )	
	{
		GET_REG_ARRAY_DATA((i3RegArray*)pData, pValue);
	}
	else
	{
		GET_REG_SINGLE_DATA(pData, pValue);
	}
}
I3_EXPORT_SCENE inline void GET_REG_DATA(i3RegData* pData, MATRIX * pValue)
{
	if( pData == nullptr )		return;
	if( pData->getDataType() == I3REG_TYPE_ARRAY )	
	{
		GET_REG_ARRAY_DATA((i3RegArray*)pData, pValue);
	}
	else
	{
		GET_REG_SINGLE_DATA(pData, pValue);
	}
}

//단일값 i3RegKey 또는 다중 i3RegKey에 해당되는 DataName에 데이터를 읽어 들인다.
I3_EXPORT_SCENE inline void FIND_REG_DATA(i3RegKey* pKey, const char * pszDataName, bool * pValue)
{
	i3RegData * pData = (i3RegData*)i3Registry::FindData(pKey, pszDataName);
	if (pData != nullptr)
	{
		INT32 val;
		GET_REG_DATA(pData, &val);
		*pValue = val != 0;
	}
}

I3_EXPORT_SCENE inline void FIND_REG_DATA(i3RegKey* pKey, const char * pszDataName, INT32 * pValue)
{
	i3RegData * pData = (i3RegData*)i3Registry::FindData( pKey, pszDataName );
	if( pData != nullptr )
		GET_REG_DATA(pData, pValue);
}
I3_EXPORT_SCENE inline void FIND_REG_DATA(i3RegKey* pKey, const char * pszDataName, REAL32 * pValue)
{
	i3RegData * pData = (i3RegData*)i3Registry::FindData( pKey, pszDataName );
	if( pData != nullptr )
		GET_REG_DATA(pData, pValue);
}
I3_EXPORT_SCENE inline void FIND_REG_DATA(i3RegKey* pKey, const char * pszDataName, i3::rc_wstring& outValue)
{
	i3RegData * pData = (i3RegData*)i3Registry::FindData( pKey, pszDataName );
	if( pData != nullptr )
		GET_REG_DATA(pData, outValue);
}
I3_EXPORT_SCENE inline void FIND_REG_DATA(i3RegKey* pKey, const char * pszDataName, VEC2D * pValue)
{
	i3RegData * pData = (i3RegData*)i3Registry::FindData( pKey, pszDataName );
	if( pData != nullptr )
		GET_REG_DATA(pData, pValue);
}
I3_EXPORT_SCENE inline void FIND_REG_DATA(i3RegKey* pKey, const char * pszDataName, VEC3D * pValue)
{
	i3RegData * pData = (i3RegData*)i3Registry::FindData( pKey, pszDataName );
	if( pData != nullptr )
		GET_REG_DATA(pData, pValue);
}
I3_EXPORT_SCENE inline void FIND_REG_DATA(i3RegKey* pKey, const char * pszDataName, VEC4D * pValue)
{
	i3RegData * pData = (i3RegData*)i3Registry::FindData( pKey, pszDataName );
	if( pData != nullptr )
		GET_REG_DATA(pData, pValue);
}
I3_EXPORT_SCENE inline void FIND_REG_DATA(i3RegKey* pKey, const char * pszDataName, I3COLOR * pValue)
{
	i3RegData * pData = (i3RegData*)i3Registry::FindData( pKey, pszDataName );
	if( pData != nullptr )
		GET_REG_DATA(pData, pValue);
}
I3_EXPORT_SCENE inline void FIND_REG_DATA(i3RegKey* pKey, const char * pszDataName, MATRIX * pValue)
{
	i3RegData * pData = (i3RegData*)i3Registry::FindData( pKey, pszDataName );
	if( pData != nullptr )
		GET_REG_DATA(pData, pValue);
}


//단일 i3RegData에 대한 형식을 검사한다.
I3_EXPORT_SCENE inline void ASSERT_REG_SINGLE_DATA_TYPE(i3RegData* & pData, i3ClassMeta * pMeta)
{
	I3ASSERT( pData->same_of( pMeta ) );
}

//다중 i3RegData에 대한 형식을 검사한다.
//매개변수 pData는 다중 i3RegData에 있는 인덱스 i3RegData* 로 변경된다.
I3_EXPORT_SCENE inline void ASSERT_REG_ARRAY_DATA_TYPE(i3RegData* & pData, i3ClassMeta * pMeta)
{
	i3RegArray * pArrayData = (i3RegArray*)pData;
	if( pArrayData->getSize() == 1 )	
	{
		pData = (*pArrayData)[0];
	}	
	else	
	{	
		pData = (*pArrayData)[ i3Registry::Get_LocationIndex() ];	
	}

	I3ASSERT( pData->same_of( pMeta ) );
}

//단일값 i3RegData 또는 다중 i3RegData에 대한 형식을 검사한다.
//매개변수 pData는 다중 형식 일 경우 i3RegData에 있는 인덱스 i3RegData* 로 변경된다.
I3_EXPORT_SCENE inline void ASSERT_REG_TYPE(i3RegData* & pData, i3ClassMeta * pMeta)
{
	if( pData->getDataType() == I3REG_TYPE_ARRAY )	
	{
		ASSERT_REG_ARRAY_DATA_TYPE(pData, pMeta);
	}
	else
	{
		ASSERT_REG_SINGLE_DATA_TYPE(pData, pMeta);
	}
}
#endif
