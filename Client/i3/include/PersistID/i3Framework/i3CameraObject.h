#if !defined( _I3CAMERA_OBJECT_H__)
#define _I3CAMERA_OBJECT_H__

#include "i3GameObj.h"

class I3_EXPORT_FRAMEWORK i3CameraObject : public i3GameObj
{
	I3_CLASS_DEFINE( i3CameraObject);

public:
	i3CameraObject(void);
	virtual ~i3CameraObject(void);

protected:
	i3Camera	*				m_pCamera;

	RT_REAL32					m_RTWidth;
	RT_REAL32					m_RTHeight;
	I3G_IMAGE_FORMAT			m_RTColorFormat;
	I3G_IMAGE_FORMAT			m_RTDepthFormat;

	i3List		*	m_pTargetList;

public:
	virtual void	Create(void);

	i3Camera	*	getCamera(void)		{	return m_pCamera;}
	void			setCamera(i3Camera * pCam);

	RT_REAL32		getRTWidth(void)					{	return m_RTWidth;}
	RT_REAL32		getRTHeight(void)					{	return m_RTHeight;}
	void			setRTWidth(RT_REAL32 width)			{	m_RTWidth = width;}
	void			setRTHeight(RT_REAL32	height)		{	m_RTHeight = height;}

	I3G_IMAGE_FORMAT getColorRTFormat(void)				{	return m_RTColorFormat;}
	I3G_IMAGE_FORMAT getDepthRTFormat(void)				{	return m_RTDepthFormat;}
	void			setColorRTFormat( I3G_IMAGE_FORMAT fmt)	{	m_RTColorFormat = fmt;}
	void			setDepthRTFormat( I3G_IMAGE_FORMAT fmt)	{	m_RTDepthFormat = fmt;}

	void			SetMatrix( RT_MATRIX * pMtx);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	virtual UINT32		OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData);
	virtual UINT32		OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz);
};

#endif