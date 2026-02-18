#if !defined( __I3_LightOBJECT_H)
#define __I3_LightOBJECT_H

#include "i3GameObj.h"

class I3_EXPORT_FRAMEWORK i3LightObject : public i3GameObj
{
	I3_CLASS_DEFINE( i3LightObject);
protected:
	i3LightAttr *	m_pLightAttr;
	i3LightSet *	m_pLightSet;

public:
	i3LightObject(void);
	virtual ~i3LightObject(void);

	i3LightAttr *		getLightAttr(void)			{ return m_pLightAttr; }

	virtual BOOL		Create( i3Node *pParentNode = NULL, BOOL bEnable = TRUE );
	virtual void		Create( i3GameResSceneGraph * pRes);

	virtual void		OnBindResource();

	virtual void		OnBuildObjectList( i3List * pList);
	virtual void		CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
