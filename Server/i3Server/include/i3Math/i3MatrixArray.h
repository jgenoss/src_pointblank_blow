#if !defined( __I3_MATRIX_ARRAY_H)
#define __I3_MATRIX_ARRAY_H

#include "i3Base.h"
#include "i3Matrix.h"

class I3_EXPORT_MATH i3MatrixArray : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3MatrixArray);

protected:
	MATRIX *		m_pArray;
	INT32			m_Count;

public:
	i3MatrixArray(void);
	virtual ~i3MatrixArray(void);

	BOOL		Create( INT32 Count);

	void		SetMatrix( INT32 Idx, MATRIX * pMatrix)		{ i3Matrix::Copy( &m_pArray[Idx], pMatrix); }
	MATRIX *	GetMatrix( INT32 Idx)						{ return &m_pArray[ Idx]; }

	INT32		GetCount(void)								{ return m_Count; }

	virtual UINT32 OnSave( i3ResourceFile * pResFile);
	virtual UINT32 OnLoad( i3ResourceFile * pResFile);
};

#endif
