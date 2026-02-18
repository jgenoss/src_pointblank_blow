#include "i3FrameworkPCH.h"
#include "i3GameResRespawn.h"

//I3_CLASS_INSTANCE( i3GameResRespawn, i3GameRes);
I3_CLASS_INSTANCE( i3GameResRespawn);

i3GameResRespawn::i3GameResRespawn()
{
	m_Type		= I3_GAMERES_RESPAWN;
}

i3GameResRespawn::~i3GameResRespawn()
{
	I3_SAFE_RELEASE( m_pRespawn);
}

void i3GameResRespawn::Destroy( void)
{
	I3_SAFE_RELEASE( m_pRespawn);
}

void i3GameResRespawn::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameRes::CopyTo( pDest, method);

	i3GameResRespawn * pRespawnRes = (i3GameResRespawn *) pDest;

	if( method == I3_COPY_REF)
	{
		pRespawnRes->setRespawnObj( getRespawnObj());
	}
	else
	{
		i3RespawnObj * pRespawn = i3RespawnObj::new_object_ref();

		getRespawnObj()->CopyTo( pRespawn, method);

		pRespawnRes->setRespawnObj( pRespawn);
	}
}

void i3GameResRespawn::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pRespawn != nullptr)
	{
		m_pRespawn->OnBuildObjectList( List);
	}

	i3GameRes::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 1)
#endif

namespace pack
{
	struct RESPAWN
	{
		UINT8		m_ID[4] = { 'R', 'S', 'R', '1' };
		OBJREF64	m_RespawnObj = 0;
		UINT32		pad[3] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3GameResRespawn::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::RESPAWN data;

	Result = i3GameRes::OnSave( pResFile);
	I3_CHKIO( Result);
	
	data.m_RespawnObj = (OBJREF64)pResFile->GetObjectPersistID( getRespawnObj());

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3GameResRespawn::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::RESPAWN data;

	Result = i3GameRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Result);
	Result += Rc;

	if( data.m_RespawnObj > 0)
	{
		i3RespawnObj * pObj = (i3RespawnObj*)pResFile->FindObjectByID((OBJREF)data.m_RespawnObj);
		I3ASSERT( pObj);

		setRespawnObj( pObj);
	}

	return Result;
}
