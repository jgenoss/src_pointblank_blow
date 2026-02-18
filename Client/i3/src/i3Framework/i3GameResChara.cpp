#include "i3FrameworkPCH.h"
#include "i3GameResChara.h"

I3_CLASS_INSTANCE( i3GameResChara);


i3GameResChara::i3GameResChara(void)
{
	m_Type	 = I3_GAMERES_CHARA;
}

i3GameResChara::~i3GameResChara(void)
{
	I3_SAFE_RELEASE( m_pChara);
}

void i3GameResChara::Destroy( void)
{
	if( m_pChara != nullptr)
	{
		I3_SAFE_RELEASE( m_pChara);
	}

	i3GameResSceneGraph::Destroy();
}

void	i3GameResChara::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameResSceneGraph::CopyTo( pDest, method);

	i3GameResChara * pCharaRes = (i3GameResChara *) pDest;

	if( method == I3_COPY_REF)
	{
		pCharaRes->setChara( getChara());
	}
	else
	{
		i3Chara * pChara = (i3Chara *) i3GameNode::CreateClone( getChara(), method);

		pCharaRes->setChara( pChara);
	}
}

void	i3GameResChara::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pChara != nullptr)
	{
		m_pChara->OnBuildObjectList( List);
	}

	i3GameResSceneGraph::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 GAMERES_CHARA
	{
		UINT8		m_ID[4] = { 'R', 'C', 'H', '1' };
		OBJREF64	m_pChara = 0;
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32	i3GameResChara::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::GAMERES_CHARA data;

	Result = i3GameResSceneGraph::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_pChara = (OBJREF64) pResFile->GetObjectPersistID( getChara());

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3GameResChara::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::GAMERES_CHARA data;

	Result = i3GameResSceneGraph::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_pChara != 0)
	{
		i3Chara * pChara = (i3Chara *) pResFile->FindObjectByID( (OBJREF) data.m_pChara);
		I3ASSERT( pChara != nullptr);

		setChara( pChara);
	}

	return Result;
}
