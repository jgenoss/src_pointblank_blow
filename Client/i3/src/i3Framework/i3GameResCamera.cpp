#include "i3FrameworkPCH.h"
#include "i3GameResCamera.h"


I3_CLASS_INSTANCE( i3GameResCamera);

i3GameResCamera::~i3GameResCamera(void)
{
	I3_SAFE_RELEASE( m_pCamObj);
}

void i3GameResCamera::Destroy( void)
{
	I3_SAFE_RELEASE( m_pCamObj);
}

void i3GameResCamera::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pCamObj != nullptr)
	{
		m_pCamObj->OnBuildObjectList( List);
	}

	i3GameRes::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct ALIGN1 GAMERES_CAMERA
	{
		UINT8		m_ID[4] = { 'I', 'G', 'C', '0' };
		OBJREF64	m_ref = 0;
		UINT32		pad[2] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3GameResCamera::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;

	Result = i3GameRes::OnSave( pResFile);
	I3_CHKIO( Result);

	pack::GAMERES_CAMERA data;

	data.m_ref = (OBJREF64)pResFile->GetObjectPersistID( m_pCamObj);

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3GameResCamera::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;

	Result = i3GameRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	pack::GAMERES_CAMERA data;

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_ref > 0)
	{
		i3CameraObject * pObj = (i3CameraObject*)pResFile->FindObjectByID( (OBJREF)data.m_ref);
		I3ASSERT( pObj != nullptr);

		SetCamObject( pObj);
	}

	return Result;
}