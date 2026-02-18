#if !defined( __I3_Level_LIGHT_H)
#define __I3_Level_LIGHT_H

#include "i3LevelElement3D.h"
#include "i3LevelIcon.h"

class I3_EXPORT_TDK i3LevelLight : public i3LevelElement3D
{
	I3_EXPORT_CLASS_DEFINE( i3LevelLight, i3LevelElement3D);

protected:
	REAL32				m_rLightDist = 0.0f;
	VEC3D				m_vLightRot;
	VEC3D				m_vLightTargetPos;

	i3LightObject *		m_pLight;			// [initialize at constructor]
	i3Transform		*	m_pTrans;			// [initialize at constructor]

public:
	i3LevelLight(void);
	virtual ~i3LevelLight(void);

	i3Transform	*	getLightTrans()						{ return m_pLight->getSceneObject();}

	void			setLightDist( REAL32 Dist)			{ m_rLightDist = Dist;	UpdateTrans();}
	REAL32			getLightDist()						{ return m_rLightDist;}

	void			setLightRotation( VEC3D *vRot)		{ i3Vector::Copy( &m_vLightRot, vRot); UpdateTrans();}
	VEC3D		*	getLightRotation()					{ return &m_vLightRot;}

	void			setLightTargetPos( VEC3D * vPos)	{ i3Vector::Copy( &m_vLightTargetPos, vPos); UpdateTrans();}
	VEC3D		*	getLightTargetPos()					{ return &m_vLightTargetPos;}

	void			UpdateTrans();

	i3LightObject*	getLightObject()					{ return m_pLight;}
	i3LightAttr *	getLightAttr()						{ return m_pLight->getLightAttr();}
	
	void		SetDiffuse( COLORREAL * pCol)			{ m_pLight->getLightAttr()->setDiffuse( pCol); }
	COLORREAL *	GetDiffuse(void)						{ return m_pLight->getLightAttr()->getDiffuse(); }

	void		SetSpecular( COLORREAL * pCol)			{ m_pLight->getLightAttr()->setSpecular( pCol); }
	COLORREAL *	GetSpecular(void)						{ return m_pLight->getLightAttr()->getSpecular(); }

	virtual void		BindRes( i3LevelRes * pRes) override;
	
	virtual i3GameObj *	CreateGameInstance(void) override;
	virtual void		Select(void) override;
	virtual void		Unselect(void) override;

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void		OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif