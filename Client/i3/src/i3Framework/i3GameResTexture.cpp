#include "i3FrameworkPCH.h"
#include "i3GameResTexture.h"

I3_CLASS_INSTANCE( i3GameResTexture);

i3GameResTexture::i3GameResTexture(void)
{
	m_Type	= I3_GAMERES_TEXTURE;
}

i3GameResTexture::~i3GameResTexture(void)
{
	I3_SAFE_RELEASE( m_pTex);
}

void i3GameResTexture::Destroy( void)
{
	I3_SAFE_RELEASE( m_pTex);
}

bool i3GameResTexture::Load( const char * pszFile)
{
	i3ImageFile file;
	i3Texture * pTex;

	pTex = file.Load( pszFile);
	I3ASSERT( pTex != nullptr);

	setTexture( pTex);

	return true;
}

void i3GameResTexture::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameRes::CopyTo( pDest, method);

	if( !i3::same_of<i3GameResTexture* >(pDest))
		return;

	i3GameResTexture * pDestTex = (i3GameResTexture *) pDest;

	pDestTex->setTexture( getTexture());
}

void	i3GameResTexture::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( isExternal() == false)
	{
		if( m_pTex != nullptr)
		{
			m_pTex->OnBuildObjectList( List);
		}
	}

	i3GameRes::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 GAMERES_TEXTURE
	{
		UINT8		m_ID[4] = { 'R', 'T', 'X', '1' };
		OBJREF64	m_pTex = 0;
		UINT32		pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3GameResTexture::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAMERES_TEXTURE data;

	Result = i3GameRes::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_pTex = (OBJREF64) pResFile->GetObjectPersistID( getTexture());

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3GameResTexture::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAMERES_TEXTURE data;

	Result = i3GameRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( isExternal())
	{
	}
	else
	{
		if( data.m_pTex != 0)
		{
			i3Texture * pTex = (i3Texture *) pResFile->FindObjectByID( (OBJREF) data.m_pTex);
			I3ASSERT( pTex != nullptr);

			setTexture( pTex);
		}
	}

	return Result;
}
