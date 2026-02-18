#include "i3FrameworkPCH.h"
#include "i3CameraObject.h"
#include "i3GameSceneInfo.h"

I3_CLASS_INSTANCE( i3CameraObject);

i3CameraObject::i3CameraObject(void)
{
	m_pCamera->addStyle( I3_CAM_STYLE_SUB);
}

i3CameraObject::~i3CameraObject(void)
{
	I3_SAFE_RELEASE( m_pCamera);
}

void i3CameraObject::SetMatrix( MATRIX * pMtx)
{
	i3MatrixObject * pmatObj = m_pCamera->GetMatrixObject();

	pmatObj->SetMatrix( pMtx);
}

void i3CameraObject::setCamera( i3Camera * pCam)
{
	I3_REF_CHANGE( m_pCamera, pCam);

	m_pCamera->addStyle( I3_CAM_STYLE_SUB);
}

void i3CameraObject::Create( void)
{
	if( m_pCamera != nullptr)
	{
		REAL32	width, height;
		I3G_IMAGE_FORMAT	rtcolor, rtdepth;

		width = m_RTWidth;
		if( width <= 0)	width = 32;

		height = m_RTHeight;
		if( height <= 0) height = 32;

		rtcolor = m_RTColorFormat;
		if( rtcolor == I3G_IMAGE_FORMAT_NONE)	rtcolor = I3G_IMAGE_FORMAT_BGRX_8888;

		rtdepth = m_RTDepthFormat;
		if( rtdepth == I3G_IMAGE_FORMAT_NONE)	rtdepth = I3G_IMAGE_FORMAT_D24X8;

		m_pCamera->CreateRenderTarget(width, height, rtcolor, rtdepth);
	}
}

void i3CameraObject::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3GameObj::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct ALIGN1 CAMERA_OBJ_INFO
	{
		REAL32	m_FOV = 0.0f;
		REAL32	m_Near = 0.0f;
		REAL32	m_Far = 0.0f;
		REAL32	m_Width = 0.0f;
		REAL32	m_Height = 0.0f;
		UINT8	m_Mode = 0;
		UINT8	m_bEnable = 0;
	};

	struct ALIGN1 CAMERA_OBJ_INFO2
	{
		REAL32	m_FOV = 0.0f;
		REAL32	m_Near = 0.0f;
		REAL32	m_Far = 0.0f;
		REAL32	m_Width = 0.0f;
		REAL32	m_Height = 0.0f;
		UINT8	m_Mode = 0;
		UINT8	m_bEnable = 0;
		REAL32	m_UpdateInterval = 0.0f;
		REAL32	m_RTWidth = 0.0f;
		REAL32	m_RTHeight = 0.0f;
		UINT32	m_RTColorFormat = 0;
		UINT32	m_RTDepthFormat = 0;
	};

	struct ALIGN1 CAMERA_RT_INFO
	{
		REAL32	m_RTWidth = 0.0f;
		REAL32	m_RTHeight = 0.0f;
		UINT32	m_RTColorFormat = 0;
		UINT32	m_RTDepthFormat = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32 i3CameraObject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::CAMERA_RT_INFO info;
	
	Result = i3GameObj::OnSave( pResFile);
	I3_CHKIO( Result);

	Rc = m_pCamera->OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	info.m_RTWidth			= m_RTWidth;
	info.m_RTHeight			= m_RTHeight;
	info.m_RTColorFormat	= (UINT32)m_RTColorFormat;
	info.m_RTDepthFormat	= (UINT32)m_RTDepthFormat;

	Rc = pResFile->Write( &info, sizeof( info));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3CameraObject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;

	Result = i3GameObj::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = m_pCamera->OnLoad( pResFile);
	I3_CHKIO( Result);
	Result += Rc;

	if( pResFile->GetVersionMinor() >= 7)
	{
		pack::CAMERA_RT_INFO	info;
		Rc = pResFile->Read( &info, sizeof( info));
		I3_CHKIO( Rc);
		Result += Rc;

		m_RTWidth	= info.m_RTWidth;
		m_RTHeight	= info.m_RTHeight;
		m_RTColorFormat	= (I3G_IMAGE_FORMAT)info.m_RTColorFormat;
		m_RTDepthFormat	= (I3G_IMAGE_FORMAT)info.m_RTDepthFormat;
	}
	else
	{
		m_RTWidth	= 32.0f;
		m_RTHeight	= 32.0f;
		m_RTColorFormat	= I3G_IMAGE_FORMAT_BGRX_8888;
		m_RTDepthFormat	= I3G_IMAGE_FORMAT_D24X8;
	}

	return Result;
}

UINT32 i3CameraObject::OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData)
{
	if( m_pCamera == nullptr)
		return 0;

	pack::CAMERA_OBJ_INFO2 * pData;

	INT32	sz = sizeof(pack::CAMERA_OBJ_INFO2);
	*ppData = pData = (pack::CAMERA_OBJ_INFO2 *)i3MemAlloc( sz);

	pData->m_bEnable = (UINT8)m_pCamera->GetEnable();

	pData->m_FOV			= m_pCamera->GetFOV();
	pData->m_Near			= m_pCamera->GetNear();
	pData->m_Far			= m_pCamera->GetFar();
	pData->m_Width			= m_pCamera->GetWindowWidth();
	pData->m_Height			= m_pCamera->GetWindowHeight();
	pData->m_Mode			= (UINT8)m_pCamera->GetMode();
	pData->m_UpdateInterval = m_pCamera->getUpdateInterval();

	pData->m_RTWidth		= m_RTWidth;
	pData->m_RTHeight		= m_RTHeight;
	pData->m_RTColorFormat	= (UINT32)m_RTColorFormat;
	pData->m_RTDepthFormat	= (UINT32)m_RTDepthFormat;

	I3TRACE("ONQUERYGAMECAMERA: wid: %f, height: %f", pData->m_RTWidth, pData->m_RTHeight);
	return sz;
}

UINT32 i3CameraObject::OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	i3Stream * pStream = pInfo->getStream();
	UINT32 result = 0;

	if( sz == sizeof(pack::CAMERA_OBJ_INFO2))
	{
		pack::CAMERA_OBJ_INFO2 data;
		result = pStream->Read( &data, sizeof(data));
		I3_CHKIO( result);

		m_pCamera->SetFOV( data.m_FOV);
		m_pCamera->SetNearFar( data.m_Near, data.m_Far);
		m_pCamera->SetWindowSize( data.m_Width, data.m_Height);
		m_pCamera->SetMode( (i3Camera::I3_CAMERA_MODE)data.m_Mode);
		m_pCamera->SetEnable( data.m_bEnable != 0);
		m_pCamera->setUpdateInterval( data.m_UpdateInterval);

		m_RTWidth	= data.m_RTWidth;
		m_RTHeight	= data.m_RTHeight;
		m_RTColorFormat	= (I3G_IMAGE_FORMAT)data.m_RTColorFormat;
		m_RTDepthFormat	= (I3G_IMAGE_FORMAT)data.m_RTDepthFormat;

		I3TRACE("ONQUERYGAMECAMERA: wid: %f, height: %f", data.m_RTWidth, data.m_RTHeight);
	}
	else
	{
		pack::CAMERA_OBJ_INFO data;
		result = pStream->Read( &data, sizeof(data));
		I3_CHKIO( result);

		m_pCamera->SetFOV( data.m_FOV);
		m_pCamera->SetNearFar( data.m_Near, data.m_Far);
		m_pCamera->SetWindowSize( data.m_Width, data.m_Height);
		m_pCamera->SetMode( (i3Camera::I3_CAMERA_MODE)data.m_Mode);
		m_pCamera->SetEnable( data.m_bEnable != 0);
	}

	I3ASSERT( result == sz);

	return result;
}

bool i3CameraObject::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3GameObj::OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	rv = m_pCamera->OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	pXML->addAttr( "RTWidth", m_RTWidth);
	pXML->addAttr( "RTHeight", m_RTHeight);
	pXML->addAttr( "RTColorFormat", m_RTColorFormat);
	pXML->addAttr( "RTDepthFormat", m_RTDepthFormat);

	return true;
}

bool i3CameraObject::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3GameObj::OnLoadXML( pFile, pXML);
	if( rv == false)
		return false;

	rv = m_pCamera->OnLoadXML( pFile, pXML);
	if( rv == false)
		return false;

	if( pXML->getAttr( "RTWidth", &m_RTWidth) == nullptr)
		m_RTWidth = 256;

	if( pXML->getAttr( "RTHeight", &m_RTHeight) == nullptr)
		m_RTHeight = 256;

	if( pXML->getAttr( "RTColorFormat", (INT32 *) &m_RTColorFormat) == nullptr)
		m_RTColorFormat = I3G_IMAGE_FORMAT_BGRX_8888;

	if( pXML->getAttr( "RTDepthFormat", (INT32 *)&m_RTDepthFormat) == nullptr)
		m_RTDepthFormat = I3G_IMAGE_FORMAT_D24X8;

	return true;
}
