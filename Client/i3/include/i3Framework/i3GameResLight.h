#if !defined( __I3_GAMERES_LIGHT_H)
#define __I3_GAMERES_LIGHT_H

#include "i3GameRes.h"
#include "i3LightObject.h"

class I3_EXPORT_FRAMEWORK i3GameResLight : public i3GameRes
{
	I3_EXPORT_CLASS_DEFINE( i3GameResLight, i3GameRes);
protected:
	i3LightObject *		m_pLight = nullptr;

public:
	i3GameResLight(void);
	virtual ~i3GameResLight(void);

	i3LightAttr *		getLightAttr(void)						{ return m_pLight->getLightAttr(); }

	i3LightObject *		getLightObject(void)					{ return m_pLight; }
	void				setLightObject( i3LightObject * pObj)	{ I3_REF_CHANGE( m_pLight, pObj); }

	virtual void		Destroy( void) override;
	virtual void		OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual void		CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif
