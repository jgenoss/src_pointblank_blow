#if !defined( __I3_NODE_LIGHTSET_H)
#define __I3_NODE_LIGHTSET_H

#include "i3Base.h"
#include "i3Node.h"
#include "i3LightAttr.h"

class I3_EXPORT_SCENE i3LightSet : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3LightSet);
protected:
	i3List			m_LightList;

public:
	i3LightSet(void);
	virtual ~i3LightSet(void);

	void			AddLightAttr( i3LightAttr * pAttr);

	void			RemoveLightAttr( i3LightAttr * pAttr);

	void			RemoveAllLightAttrs(void);

	i3LightAttr *	GetLightAttr( INT32 idx)
	{
		return (i3LightAttr *) m_LightList.Items[ idx];
	}

	INT32			GetLightAttrCount(void)
	{
		return	m_LightList.GetCount();
	}

	i3List *		GetLightAttrs(void)
	{
		return &m_LightList;
	}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
