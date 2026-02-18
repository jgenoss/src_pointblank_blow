#include "stdafx.h"
#include "i3LevelChara.h"
#include "i3LevelResChara.h"

I3_CLASS_INSTANCE( i3LevelChara);

i3LevelChara::i3LevelChara(void)
{
	i3::safe_string_copy( m_szInstanceClass, "i3Chara", 128);
}

void i3LevelChara::BindRes( i3LevelRes * pRes)
{
	I3ASSERT( i3::kind_of<i3LevelResChara*>(pRes));

	i3LevelElement3D::BindRes( pRes);
	i3LevelResChara * pCharaRes = (i3LevelResChara *) pRes;

	i3Chara * pChara = pCharaRes->getChara();

	I3_SAFE_RELEASE( m_pChara);

	m_pChara = (i3Chara *) i3Chara::new_object();
	
	pChara->CopyTo( m_pChara, I3_COPY_INSTANCE);

	setRefObject( m_pChara);

	if( hasName())
	{
		m_pChara->SetName( GetName());
	}

	SetSymbol( m_pChara->getSceneObject());
}

i3GameObj * i3LevelChara::CreateGameInstance(void)
{
	I3_SAFE_ADDREF(m_pChara);

	return m_pChara;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 CHARA
	{
		UINT8		m_ID[4] = { 'T', 'C', 'H', '1' };
		UINT32		m_Reserved[16] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3LevelChara::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::CHARA Info;

	Result = i3LevelElement3D::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc  = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelChara::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32	i3LevelChara::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::CHARA Info;

	Result = i3LevelElement3D::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc  = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelChara::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}
