#if !defined( _I3CAMERA_OBJECT_H__)
#define _I3CAMERA_OBJECT_H__

#include "i3GameObj.h"

class I3_EXPORT_FRAMEWORK i3CameraObject : public i3GameObj
{
	I3_EXPORT_CLASS_DEFINE( i3CameraObject, i3GameObj);

public:
	i3CameraObject(void);
	virtual ~i3CameraObject(void);

protected:
	i3Camera	*				m_pCamera = i3Camera::new_object();

	REAL32					m_RTWidth = 256.0f;
	REAL32					m_RTHeight = 256.0f;
	I3G_IMAGE_FORMAT			m_RTColorFormat = I3G_IMAGE_FORMAT_BGRX_8888;
	I3G_IMAGE_FORMAT			m_RTDepthFormat = I3G_IMAGE_FORMAT_D24X8;

public:
	virtual void	Create(void);

	i3Camera	*	getCamera(void)		{	return m_pCamera;}
	void			setCamera(i3Camera * pCam);

	REAL32		getRTWidth(void)					{	return m_RTWidth;}
	REAL32		getRTHeight(void)					{	return m_RTHeight;}
	void			setRTWidth(REAL32 width)			{	m_RTWidth = width;}
	void			setRTHeight(REAL32	height)		{	m_RTHeight = height;}

	I3G_IMAGE_FORMAT getColorRTFormat(void)				{	return m_RTColorFormat;}
	I3G_IMAGE_FORMAT getDepthRTFormat(void)				{	return m_RTDepthFormat;}
	void			setColorRTFormat( I3G_IMAGE_FORMAT fmt)	{	m_RTColorFormat = fmt;}
	void			setDepthRTFormat( I3G_IMAGE_FORMAT fmt)	{	m_RTDepthFormat = fmt;}

	void			SetMatrix( MATRIX * pMtx);

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual UINT32		OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData) override;
	virtual UINT32		OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz) override;
};

#endif