#include "i3FrameworkPCH.h"
#include "i3GameResEvent.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3GameResEvent);

i3GameResEvent::i3GameResEvent(void)
{
	m_Type			= I3_GAMERES_ACTION_EVENT;
}

i3GameResEvent::~i3GameResEvent(void)
{
	I3_SAFE_RELEASE( m_pTimeEvent);
}

void i3GameResEvent::Destroy( void)
{
	I3_SAFE_RELEASE( m_pTimeEvent);
}

void i3GameResEvent::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameRes::CopyTo( pDest, method);

	I3ASSERT( i3::same_of<i3GameResEvent* >(pDest));

	i3GameResEvent * pEvent = (i3GameResEvent*)pDest;

	pEvent->setTimeEvent( m_pTimeEvent);
}

void i3GameResEvent::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pTimeEvent != nullptr)
	{
		m_pTimeEvent->OnBuildObjectList( List);
	}

	i3GameRes::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push , 1)
#endif

namespace pack
{
	struct GAMERES_EVENT
	{
		UINT8		nID[4] = { 'P', 'G', 'E', '1' };
		UINT32		nVersion = 1;
		OBJREF64	pTimeEvent = 0;

		UINT32		pad[6] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3GameResEvent::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::GAMERES_EVENT data;

	Result = i3GameRes::OnSave( pResFile);
	I3_CHKIO( Result);
	
	if( m_pTimeEvent != nullptr)
	{
		data.pTimeEvent = (OBJREF64)pResFile->GetObjectPersistID( m_pTimeEvent);
	}

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3GameResEvent::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::GAMERES_EVENT data;

	Result = i3GameRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( i3::generic_is_niequal( (char*)data.nID, "PGE1", 4) == false)
	{
		I3TRACE("is not a valid gameresevent\n");
		return STREAM_ERR;
	}

	I3ASSERT( data.nVersion == 1);

	if( data.pTimeEvent > 0)
	{
		i3TimeEventGen * pEvent = (i3TimeEventGen*)pResFile->FindObjectByID( (OBJREF)data.pTimeEvent);
		I3ASSERT( pEvent != nullptr);

		setTimeEvent( pEvent);
	}

	return Result;
}