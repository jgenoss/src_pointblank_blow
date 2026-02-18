#if !defined( _I3GAME_RES_CAMERA_H__)
#define _I3GAME_RES_CAMERA_H__

#include "i3GameRes.h"
#include "i3CameraObject.h"

class I3_EXPORT_FRAMEWORK i3GameResCamera : public i3GameRes
{
	I3_CLASS_DEFINE( i3GameResCamera);

protected:
	i3CameraObject * m_pCamObj;

public:
	i3GameResCamera(void);
	virtual ~i3GameResCamera(void);

	i3CameraObject * getCamObject( void)	{	return m_pCamObj;}
	void			SetCamObject( i3CameraObject * pObj)	{	I3_REF_CHANGE( m_pCamObj, pObj);}

	virtual void	OnBuildObjectList( i3List * pList);
	virtual	UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif