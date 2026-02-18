#if !defined( __I3_NODE_ATTRSET_H)
#define __I3_NODE_ATTRSET_H

#include "i3Base.h"
#include "i3Node.h"
#include "i3RenderAttr.h"

#define		I3_DEF_ATTR_PRIORITY			10

class I3_EXPORT_SCENE i3AttrSet : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3AttrSet);

protected:
	i3List			m_AttrList;

	UINT32			m_Priority;

public:
	i3AttrSet(void);
	virtual ~i3AttrSet(void);

	void			AddAttr( i3RenderAttr * pAttr);

	void			RemoveAttr( i3RenderAttr * pAttr);

	void			RemoveAllAttrs(void);

	i3RenderAttr *	GetAttr( INT32 idx)
	{
		return (i3RenderAttr *) m_AttrList.Items[ idx];
	}

	i3RenderAttr *	FindAttr( i3ClassMeta * pMeta);

	INT32			GetAttrCount(void)
	{
		return	m_AttrList.GetCount();
	}

	i3List *		GetAttrs(void)
	{
		return &m_AttrList;
	}


	UINT32			GetPriority(void)					{ return m_Priority; }
	void			SetPriority( UINT32 prio = I3_DEF_ATTR_PRIORITY)			{ m_Priority = prio; }

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
