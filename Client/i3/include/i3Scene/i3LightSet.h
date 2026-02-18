#if !defined( __I3_NODE_LIGHTSET_H)
#define __I3_NODE_LIGHTSET_H

#include "i3Base.h"
#include "i3Node.h"
#include "i3LightAttr.h"

class I3_EXPORT_SCENE i3LightSet : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3LightSet, i3Node);
protected:
	i3::vector<i3LightAttr*>			m_LightList;

public:
	i3LightSet(void);
	virtual ~i3LightSet(void);

	void			AddLightAttr( i3LightAttr * pAttr);

	void			RemoveLightAttr( i3LightAttr * pAttr);

	void			RemoveAllLightAttrs(void);

	i3LightAttr *	GetLightAttr( INT32 idx)
	{
		return m_LightList[ idx];
	}

	INT32			GetLightAttrCount(void)
	{
		return	(INT32)m_LightList.size();
	}

	const i3::vector<i3LightAttr*>&		GetLightAttrs(void)
	{
		return m_LightList;
	}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
