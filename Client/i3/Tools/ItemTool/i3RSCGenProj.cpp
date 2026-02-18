#include "StdAfx.h"
#include "i3RSCGenProj.h"
#include "i3Base/string/compare/generic_is_niequal.h"

I3_CLASS_INSTANCE( i3RSCGenProj); //, i3PersistantElement);

i3RSCGenProj::i3RSCGenProj(void)
{
	m_pRoot	= NULL;
}

i3RSCGenProj::~i3RSCGenProj(void)
{
	I3_SAFE_RELEASE( m_pRoot);
}

void i3RSCGenProj::SetRoot( RSCNode * pRoot)
{
	if( pRoot != NULL)
	{
		pRoot->AddRef();
	}

	I3_SAFE_RELEASE( m_pRoot);

	m_pRoot = pRoot;
}

#if defined (I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

typedef struct ALIGN4 _tagi3PersistRSCProj
{
	char	m_ID[4];
	OBJREF	m_pRoot;
}I3_PERSIST_RSC_PROJ;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

void	i3RSCGenProj::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pRoot != NULL)
	{
		m_pRoot->OnBuildObjectList( List);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

UINT32	i3RSCGenProj::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	I3_PERSIST_RSC_PROJ proj;
	i3mem::FillZero( &proj, sizeof( I3_PERSIST_RSC_PROJ));

	strcpy( proj.m_ID, "RSCF");

	proj.m_pRoot	= (OBJREF)pResFile->GetObjectPersistID( m_pRoot);

	Rc = pStream->Write( &proj, sizeof( I3_PERSIST_RSC_PROJ));
	if( Rc == STREAM_ERR)
	{
		return Rc;
	}
	Result += Rc;

	return Result;
}

UINT32	i3RSCGenProj::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	I3_PERSIST_RSC_PROJ proj;

	Rc = pStream->Read( &proj, sizeof( I3_PERSIST_RSC_PROJ));
	if( Rc == STREAM_ERR)
	{
		return Rc;
	}
	Result += Rc;

	if( i3::generic_is_niequal( proj.m_ID, "RSCF", 4) )
	{
		if( proj.m_pRoot != NULL)
		{
			RSCNode * pRoot = (RSCNode *)pResFile->FindObjectByID( proj.m_pRoot);
			I3ASSERT( pRoot);

			SetRoot( pRoot);
		}
	}

	return Result;
}