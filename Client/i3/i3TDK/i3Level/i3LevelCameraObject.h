#if !defined( _I3LV_CAMERA_OBJECT_H__)
#define _I3LV_CAMERA_OBJECT_H__

#include "i3LevelCamera.h"

class I3_EXPORT_TDK i3LevelCameraObject : public i3LevelCamera
{
	I3_EXPORT_CLASS_DEFINE( i3LevelCameraObject, i3LevelCamera);

protected:
	i3CameraObject	*	m_pCameraObject;			// [initialize at constructor]

	i3Transform		*	m_pTrans;					// [initialize at constructor]
	i3AttrSet		*	m_pFrustumAttrSet;			// [initialize at constructor]
	i3VertexArray	*	m_pVA;						// [initialize at constructor]

public:
	i3LevelCameraObject(void);
	virtual ~i3LevelCameraObject(void);

public:
	i3CameraObject	*	getCameraObject( void)	{	return m_pCameraObject;}

	virtual void	BindRes( i3LevelRes * pRes) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual	i3GameObj	*	CreateGameInstance(void) override;
	virtual void		Select(void) override;
	virtual void		Unselect(void) override;
	void				_UpdateFrustum();
};

#endif	//#define _I3LV_CAMERA_OBJECT_H__