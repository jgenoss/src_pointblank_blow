#include "i3GfxType.h"
#include "i3GfxOption.h"

I3_CLASS_INSTANCE( i3GfxOption);

i3GfxOption::i3GfxOption(void)
{
	m_Option.m_bKeepPersist = TRUE;
	m_Option.m_bLuxMapEnable = TRUE;
	m_Option.m_ShadowMapSize = 256;
	m_Option.m_bNormalMapEnable = TRUE;
	m_Option.m_bSpecularMapEnable = TRUE;
	m_Option.m_bEnvironmentMapEnable = TRUE;
	m_Option.m_bEmissiveMapEnable = TRUE;
	m_Option.m_Format = I3G_IMAGE_FORMAT_BGRX_8888;
	m_Option.m_ForceMipmapFilter = I3G_TEXTURE_FILTER_NONE;
}


UINT32 i3GfxOption::Save( const char * pszPath)
{
	i3FileStream stream;
	UINT32 Rv;
	
	if( stream.Create( pszPath, STREAM_WRITE) == false)
	{
		I3PRINTLOG(I3LOG_FATAL,  "%s Gfx Option 파일을 저장할 수 없습니다.", pszPath);
		return STREAM_ERR;
	}

	Rv = Save( &stream);
	if( Rv == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	stream.Close();

	return Rv;
}

UINT32 i3GfxOption::Save( i3Stream * pStream)
{
	UINT32 Rv;

	Rv = pStream->Write( &m_Option, sizeof(m_Option));
	if( Rv == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Gfx Option 파일을 저장할 수 없습니다.");
		return STREAM_ERR;
	}

	return Rv;
}

UINT32 i3GfxOption::Load( const char * pszPath)
{
	i3FileStream stream;
	UINT32 Rv;
	
	if( stream.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		I3PRINTLOG(I3LOG_FATAL,  "%s Gfx Option 파일을 읽어들일 수 없습니다.", pszPath);
		return STREAM_ERR;
	}

	Rv = Load( &stream);
	if( Rv == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	stream.Close();

	return Rv;
}

UINT32 i3GfxOption::Load( i3Stream * pStream)
{
	UINT32 Rv;

	Rv = pStream->Read( &m_Option, sizeof(m_Option));
	if( Rv == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Gfx Option 파일을 읽어들일 수 없습니다.");
		return STREAM_ERR;
	}

	m_Option.m_bNormalMapEnable			= m_Option.m_bNormalMapEnable		&& m_bForceNormalMapEnable;
	m_Option.m_bSpecularMapEnable		= m_Option.m_bNormalMapEnable		&& m_bForceSpecularMapEnable;
	m_Option.m_bEnvironmentMapEnable	= m_Option.m_bEnvironmentMapEnable	&& m_bForceReflectMapEnable;
	m_Option.m_bEmissiveMapEnable		= m_Option.m_bEmissiveMapEnable		&& m_bForceEmissiveMapEnable;

	if( m_bForceShadowMapEnable == false)
	{
		m_Option.m_ShadowType = 0;
	}

	return Rv;
}

UINT32 i3GfxOption::OnSave( i3ResourceFile * pResFile)
{
	return Save( pResFile->GetStream());
}

UINT32 i3GfxOption::OnLoad( i3ResourceFile * pResFile)
{
	return Load( pResFile->GetStream());
}
