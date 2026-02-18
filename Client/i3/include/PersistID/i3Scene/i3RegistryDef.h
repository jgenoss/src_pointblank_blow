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
//									if( pKey != NULL)

#define FIND_REG_KEY(s)				i3Registry::FindKey(s)

//#define FIND_REG_KEY_FROM(s,r)	i3RegKey * pKey = i3Registry::FindKey(s,r);\
//									if( pKey != NULL)

#define FIND_REG_KEY_FROM(s,r)		i3Registry::FindKey(s, r)

#define IS_REG_DATA(k,s)					i3Registry::FindData(k,s)
#define FIND_REG_DATA(k,s,v)			\
{\
	i3RegData * pData = i3Registry::FindData( (k), (s));\
	if( pData != NULL)		pData->GetValue( v);\
}
							
#endif
