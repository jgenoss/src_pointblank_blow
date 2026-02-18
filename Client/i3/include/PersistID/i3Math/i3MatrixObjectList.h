#if !defined( __I3_MATRIX_OBJECT_LIST_H)
#define __I3_MATRIX_OBJECT_LIST_H

#include "i3Base.h"
#include "i3Matrix.h"
#include "i3MatrixObject.h"

class I3_EXPORT_MATH i3MatrixObjectList : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3MatrixObjectList);

protected:
	i3List		m_List;

public:
	i3MatrixObjectList(void);
	virtual ~i3MatrixObjectList(void);

	i3MatrixObject *		Get( INT32 idx)							{	return (i3MatrixObject *) m_List.Items[idx]; }
	void					Set( INT32 idx, i3MatrixObject * p);

	void					SetCount( INT32 count)					{	m_List.SetCount( count); }
	INT32					GetCount(void)							{ return m_List.GetCount(); }

	void					Add( i3MatrixObject * p);

	virtual void			OnBuildObjectList( i3List * pList);
	virtual UINT32			OnSave( i3ResourceFile * pResFile);
	virtual UINT32			OnLoad( i3ResourceFile * pResFile);
};

#endif
