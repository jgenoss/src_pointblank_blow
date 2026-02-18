#if !defined( __I3_LIGHT_ATTR_H)
#define __I3_LIGHT_ATTR_H

#include "i3RenderAttr.h"
#include "i3Gfx.h"

class I3_EXPORT_SCENE i3LightAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3LightAttr, i3RenderAttr);
protected:
	i3Light *			m_pLight;

public:
	i3LightAttr(void);
	virtual ~i3LightAttr(void);

	i3Light *			GetLightObject(void)					{ return m_pLight; }
	void				SetLightObject( i3Light * pLight);

	void				setLightType( I3G_LIGHT_TYPE type)		{ m_pLight->SetLightType( type); }
	I3G_LIGHT_TYPE		getLightType(void)						{ return m_pLight->GetLightType(); }

	void				setDiffuse( COLORREAL * pColor)			{ m_pLight->SetDiffuse( pColor); }
	COLORREAL *			getDiffuse(void)						{ return m_pLight->GetDiffuse(); }

	void				setSpecular( COLORREAL * pColor)		{ m_pLight->SetSpecular( pColor); }
	COLORREAL *			getSpecular(void)						{ return m_pLight->GetSpecular(); }

	void				setInnerAngle( REAL32 val)				{ m_pLight->setInnerAngle( val); }
	REAL32				getInnerAngle(void)						{ return m_pLight->getInnerAngle(); }

	void				setOuterAngle( REAL32 val)				{ m_pLight->setOuterAngle( val); }
	REAL32				getOuterAngle(void)						{ return m_pLight->getOuterAngle(); }

	void				setRange( REAL32 val)					{ m_pLight->setRange( val); }
	REAL32				getRange(void)							{ return m_pLight->getRange(); }

	i3Texture *			getPatternMap(void)						{ return m_pLight->getPatternMap(); }
	void				SetPatternMap( i3Texture * pTex)		{ m_pLight->setPatternMap( pTex); }

	virtual void		Apply( i3RenderContext * pContext) override;
	virtual void		CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	//virtual BOOL		IsSame( i3RenderAttr * pAttr);

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	#if defined( I3_DEBUG)
	virtual void Dump(void) override;
	#endif
};

#endif
