#if !defined( __I3_LightOBJECT_H)
#define __I3_LightOBJECT_H

#include "i3GameObj.h"

class I3_EXPORT_FRAMEWORK i3LightObject : public i3GameObj
{
	I3_EXPORT_CLASS_DEFINE( i3LightObject, i3GameObj);
protected:
	i3LightAttr *	m_pLightAttr = nullptr;
	i3LightSet *	m_pLightSet = i3LightSet::new_object_ref(); // m_pLightSet은 i3GameObj에서 자동으로 삭제...

public:
	i3LightObject(void);

	i3LightAttr *		getLightAttr(void)			{ return m_pLightAttr; }

	virtual bool		Create( i3Node *pParentNode = nullptr, bool bEnable = true ) override;
	virtual void		Create( i3GameResSceneGraph * pRes);

	virtual void		OnBindResource() override {}

	virtual void		OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual void		CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif
