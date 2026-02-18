#include "stdafx.h"
#include "i3LevelResCamera.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelCameraObject.h"

I3_CLASS_INSTANCE( i3LevelResCamera);

i3LevelResCamera::i3LevelResCamera(void)
{
	m_pCamera		= i3CameraObject::new_object();
	m_ResType		= I3LV_TYPE_CAMERA;
}

i3LevelResCamera::~i3LevelResCamera(void)
{
	I3_SAFE_RELEASE( m_pCamera);
	I3_SAFE_RELEASE( m_pScene);
}

bool i3LevelResCamera::OnValidateBrief( bool bForce)
{
	if( bForce == false)
	{
		if( isBriefLoaded())
			return true;
	}
	i3Node * pNode = g_pScene->GetCommonCameraResource()->GetSceneRoot();
	I3ASSERT( pNode != nullptr);

	I3_REF_CHANGE( m_pScene, pNode);

	SetName("Camera");
	setPath("Camera");

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return true;
}

bool i3LevelResCamera::OnValidate( i3LevelScene * pScene,bool bForce)
{
	if( bForce == false)
	{
		if( isLoaded())
			return true;
	}

	OnValidateBrief( false);

	addResState( I3LV_RES_STATE_LOADED);

	return true;
}

i3GameRes *			i3LevelResCamera::CreateGameRes(void)
{
	i3GameResCamera * pRes;

	pRes = i3GameResCamera::new_object();

	pRes->SetCamObject( m_pCamera);

	return pRes;
}

i3LevelElement3D *	i3LevelResCamera::CreateInstance( i3LevelScene * pScene)
{
	OnValidate( pScene, false);

	i3LevelCameraObject * pObj = i3LevelCameraObject::new_object_ref();

	pObj->BindRes( this);

	pScene->AddElement( pObj, "Camera");

	i3CameraObject * pCam = pObj->getCameraObject();
	if( pCam != nullptr)
	{
		pCam->SetName( pObj->GetName());
	}

	return pObj;
}

i3Node *	i3LevelResCamera::GetElementAttachNode( i3LevelScene * pScene)
{
	return nullptr;
}
/*
void	i3LevelResCamera::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pCamera != nullptr)
	{
		m_pCamera->OnBuildObjectList( pList);
	}

	i3LevelRes::OnBuildObjectList( pList);
}*/

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct ALIGN1 CAMERA
	{
		UINT8		m_ID[4] = { 'I', 'P', 'C', '0' };
		OBJREF64	m_pCam = 0;
		UINT32		pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32 i3LevelResCamera::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;

	Result = i3LevelRes::OnSave( pResFile);
	I3_CHKIO( Result);

	Rc = m_pCamera->OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;

	/*
	I3LVRES_PERSIST_CAMERA data;

	i3mem::FillZero( &data, sizeof( data));
	i3mem::Copy( data.m_ID, "IPC0", sizeof( UINT8) * 4);

	data.m_pCam = (OBJREF64)pResFile->GetObjectPersistID( m_pCamera);

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
	*/
}

UINT32 i3LevelResCamera::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;

	Result = i3LevelRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = m_pCamera->OnLoad( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	/*
	I3LVRES_PERSIST_CAMERA data;

	Rc = pResFile->Read( &data ,sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_pCam > 0)
	{
		i3CameraObject * pObj = (i3CameraObject*)pResFile->FindObjectByID( (OBJREF)data.m_pCam);
		I3ASSERT( pObj != nullptr);

		setCamObject( pObj);
	}*/

	return Result;
}
