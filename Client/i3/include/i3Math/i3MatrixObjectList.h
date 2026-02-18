#if !defined( __I3_MATRIX_OBJECT_LIST_H)
#define __I3_MATRIX_OBJECT_LIST_H

#include "i3Base.h"
#include "i3Matrix.h"
#include "i3MatrixObject.h"

class I3_EXPORT_MATH i3MatrixObjectList : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3MatrixObjectList, i3PersistantElement);

protected:
	i3::vector<i3PersistantElement*>	m_List;

public:
	virtual ~i3MatrixObjectList(void);

	i3MatrixObject *		Get( INT32 idx)							{	return (i3MatrixObject *) m_List[idx]; }
	void					Set( INT32 idx, i3MatrixObject * p);

	void					SetCount( INT32 count)					{	m_List.resize( count); }
	INT32					GetCount(void)							{ return INT32(m_List.size()); }

	void					Add( i3MatrixObject * p);

	virtual void			OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;
};

#endif
