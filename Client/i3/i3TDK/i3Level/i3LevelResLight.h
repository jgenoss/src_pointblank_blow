#if !defined( __I3_LEVELRES_LIGHT_H)
#define __I3_LEVELRES_LIGHT_H

#include "i3LevelRes.h"

class I3_EXPORT_TDK i3LevelResLight : public i3LevelRes
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResLight, i3LevelRes);
protected:
	i3LightObject *		m_pLight = nullptr;

	i3SceneGraphInfo	*	m_pScene = nullptr;

public:
	i3LevelResLight(void);
	virtual ~i3LevelResLight(void);

	i3SceneGraphInfo		*	getScene(void)	{	return m_pScene;}

	i3LightObject	*			getLightObject(void)	{	return m_pLight;}
	void						setLightObject( i3LightObject * pObj){	I3_REF_CHANGE( m_pLight, pObj);}

	bool						OnExport( i3LevelScene * pScene, const char * pszPath);

	virtual i3LevelElement3D *	CreateInstance( i3LevelScene * pScene) override;
	virtual bool				OnValidateBrief( bool bForce) override;
	virtual bool				OnValidate( i3LevelScene * pScene, bool bForce) override;
	virtual i3GameRes *			CreateGameRes(void) override;
	virtual bool				OnSerialize(void) override;

};

#endif
