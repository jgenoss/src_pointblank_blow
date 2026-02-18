#include "i3FrameworkPCH.h"
#include "i3GameResPath.h"

#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3GameResPath);

i3GameResPath::i3GameResPath()
{
	m_Type	= I3_GAMERES_PATH;
}

i3GameResPath::~i3GameResPath()
{
	I3_SAFE_RELEASE( m_pPath);
}

void i3GameResPath::Destroy( void)
{
	I3_SAFE_RELEASE( m_pPath);
}

void	i3GameResPath::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameRes::CopyTo( pDest, method);

	i3GameResPath * pDestPath = (i3GameResPath *) pDest;

	if( method == I3_COPY_REF)
	{
		pDestPath->setPath( m_pPath);
	}
	else
	{
		i3GamePath * pPath = i3GamePath::new_object_ref();

		m_pPath->CopyTo( pPath, method);

		pDestPath->setPath( m_pPath);
	}
}

void	i3GameResPath::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pPath != nullptr)
	{
		m_pPath->OnBuildObjectList( List);
	}

	i3GameRes::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct GAMERES_PATH
	{
		UINT8		m_ID[4] = { 'R', 'S', 'P', '1' };
		OBJREF64	m_refPath = 0;

		UINT32		pad[2] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32	i3GameResPath::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::GAMERES_PATH	data;

	Result = i3GameRes::OnSave( pResFile);
	I3_CHKIO( Result);
	
	data.m_refPath = (OBJREF64)pResFile->GetObjectPersistID( m_pPath);

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3GameResPath::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::GAMERES_PATH data;

	Result = i3GameRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( i3::generic_is_niequal( (char*)data.m_ID, "RSP1", 4) == false)
	{
		I3TRACE("Not a Valid ID: %s\n", (char*)data.m_ID);
		return STREAM_ERR;
	}

	if( data.m_refPath > 0)
	{
		i3GamePath * pPath = (i3GamePath*)pResFile->FindObjectByID((OBJREF)data.m_refPath);
		I3ASSERT( pPath != nullptr);

		setPath( pPath);
	}

	return Result;
}