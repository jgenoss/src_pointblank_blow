#include "i3FrameworkPCH.h"
#include "i3GameResAnimation.h"

//I3_CLASS_INSTANCE( i3GameResAnimation, i3GameRes);
I3_CLASS_INSTANCE( i3GameResAnimation);

i3GameResAnimation::i3GameResAnimation(void)
{
	m_Type	= I3_GAMERES_ANIMATION;
}

i3GameResAnimation::~i3GameResAnimation(void)
{	
	I3_SAFE_RELEASE( m_pAnim);
}

void i3GameResAnimation::Destroy( void)
{
	I3_SAFE_RELEASE( m_pAnim);
}

bool i3GameResAnimation::Load( const char * pszFile)
{
	i3Animation * pAnim;

	pAnim = i3Animation::LoadFromFile( pszFile);
	if( pAnim == nullptr)
	{
		i3Log( "i3GameResAnimation::Load()", "Could not load %s animation resource.", pszFile);
		return false;
	}

	setAnimation( pAnim);
	I3_MUST_RELEASE( pAnim);	// LoadFromFile()에 AddRef()가 추가 되었기때문에 한번 릴리즈 필요.(2012.09.11.수빈)
	return true;
}

void i3GameResAnimation::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameRes::CopyTo( pDest, method);

	if( ! i3::same_of<i3GameResAnimation* >(pDest))
		return;

	i3GameResAnimation * pRes = (i3GameResAnimation *) pDest;

	pRes->setAnimation( getAnimation());
}

void	i3GameResAnimation::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( isExternal() == false)
	{
		if( m_pAnim != nullptr)
			m_pAnim->OnBuildObjectList( List);
	}

	i3GameRes::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 GAMERES_ANIMATION
	{
		UINT8		m_ID[4] = { 'G', 'R', 'A', '1' };
		OBJREF64	m_pAnim = 0;

		UINT32		pad[2] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32	i3GameResAnimation::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAMERES_ANIMATION data;

	Result = i3GameRes::OnSave( pResFile);
	I3_CHKIO( Result);
	
	data.m_pAnim = (OBJREF64) pResFile->GetObjectPersistID( m_pAnim);

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3GameResAnimation::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAMERES_ANIMATION data;

	Result = i3GameRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( isExternal())
	{
		Load( GetName());
	}
	else
	{
		if( data.m_pAnim != 0)
		{
			i3Animation * pAnim = (i3Animation *) pResFile->FindObjectByID( (OBJREF) data.m_pAnim);
			I3ASSERT( pAnim != nullptr);

			setAnimation( pAnim);
		}
	}

	return Result;
}
