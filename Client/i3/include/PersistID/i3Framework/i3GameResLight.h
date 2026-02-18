#if !defined( __I3_GAMERES_LIGHT_H)
#define __I3_GAMERES_LIGHT_H

#include "i3GameRes.h"
#include "i3LightObject.h"

class I3_EXPORT_FRAMEWORK i3GameResLight : public i3GameRes
{
	I3_CLASS_DEFINE( i3GameResLight);
protected:
	i3LightObject *		m_pLight;

public:
	i3GameResLight(void);
	virtual ~i3GameResLight(void);

	i3LightAttr *		getLightAttr(void)						{ return m_pLight->getLightAttr(); }

	i3LightObject *		getLightObject(void)					{ return m_pLight; }
	void				setLightObject( i3LightObject * pObj)	{ I3_REF_CHANGE( m_pLight, pObj); }

	virtual void		OnBuildObjectList( i3List * pList);
	virtual void		CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
