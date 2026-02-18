#if !defined( __I3_VECTOR4D_ARRAY_H)
#define __I3_VECTOR4D_ARRAY_H

#include "i3Base.h"
#include "i3MathType.h"
#include "i3Vector.h"

class I3_EXPORT_MATH i3Vector4Array : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3Vector4Array, i3PersistantElement);

protected:
	VEC4D *			m_pVector = nullptr;
	UINT32			m_VectorCount = 0;

public:
	virtual ~i3Vector4Array(void);
	
	VEC4D *			GetVectorArray(void)				{ return m_pVector; }
	VEC4D *			GetVector( INT32 idx)				{ return m_pVector + idx; }
	UINT32			GetVectorCount(void)				{ return m_VectorCount; }

	void			SetVector( INT32 idx, VEC4D * pPos)	{ i3Vector::Copy( &m_pVector[idx], pPos); }
	void			SetVector( INT32 idx, REAL32 x, REAL32 y, REAL32 z, REAL32 w)
	{
		i3Vector::Set( &m_pVector[idx], x, y, z, w);
	}
	void			SetVectorCount( UINT32 count);

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
