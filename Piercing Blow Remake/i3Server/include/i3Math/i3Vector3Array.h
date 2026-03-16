#if !defined( __I3_VECTOR3D_ARRAY_H)
#define __I3_VECTOR3D_ARRAY_H

#include "i3Base.h"
#include "i3MathType.h"
#include "i3Vector.h"

class I3_EXPORT_MATH i3Vector3Array : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3Vector3Array);

protected:
	VEC3D *			m_pVector;
	UINT32			m_VectorCount;

public:
	i3Vector3Array(void);
	virtual ~i3Vector3Array(void);
	
	VEC3D *			GetVectorArray(void)				{ return m_pVector; }
	VEC3D *			GetVector( INT32 idx)				{ return m_pVector + idx; }
	UINT32			GetVectorCount(void)				{ return m_VectorCount; }

	void			SetVector( INT32 idx, VEC3D * pPos)	{ i3Vector::Copy( &m_pVector[idx], pPos); }
	void			SetVector( INT32 idx, REAL32 x, REAL32 y, REAL32 z)
	{
		i3Vector::Set( &m_pVector[idx], x, y, z);
	}
	void			SetVectorCount( UINT32 count);

	void			_forceSetVectorCount( UINT32 count)		{ m_VectorCount = count; }

	INT32			Find( VEC3D * pVec, INT32 limitCount = -1, REAL32 err = I3_EPS);

#if defined( I3_CHECK_FLOAT_VALUE_FOR_PHYSICX)
	void			CheckValid( UINT32 cnt = 0);
#endif

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
