#if !defined( _I3LV_RES_CAMERA_H__)
#define _I3LV_RES_CAMERA_H__

#include "i3levelres.h"

class I3_EXPORT_TDK i3LevelResCamera :	public i3LevelRes
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResCamera, i3LevelRes);

protected:
	i3CameraObject * m_pCamera;			// [initialize at constructor]

	i3Node	*	m_pScene = nullptr;

public:
	i3LevelResCamera(void);
	virtual ~i3LevelResCamera(void);

	i3Node	*	getScene()	{	return m_pScene;}

	i3CameraObject *	getCamObject(void)						{	return m_pCamera;}
	void				setCamObject( i3CameraObject * pCam)	{	I3_REF_CHANGE( m_pCamera, pCam);}

public:

	virtual	bool	OnValidateBrief( bool bForce) override;
	virtual bool	OnValidate( i3LevelScene * pScene, bool bForce) override;

	i3GameRes *					CreateGameRes(void);
	virtual i3LevelElement3D *	CreateInstance( i3LevelScene * pScene) override;
	virtual i3Node *			GetElementAttachNode( i3LevelScene * pScene) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif