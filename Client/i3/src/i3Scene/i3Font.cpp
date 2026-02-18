#include "i3SceneDef.h"
#include "i3Font.h"

#include "i3TextAttr.h"
#include <stdio.h>
#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/ext/extract_filename.h"

I3_CLASS_INSTANCE( i3Font);

#define UNICODE_SIZE	65536

i3Font::~i3Font()
{
	
	for(size_t i = 0; i < m_pTextureList.size(); i++ )
	{
		i3Texture *pTex = m_pTextureList[i];
		I3_SAFE_RELEASE( pTex );
	}

	I3MEM_SAFE_FREE_POINTER( m_pDictionary);
}

// 파일에서 생성
bool i3Font::Create( const char *szPath )
{
	bool bRet = false;

	i3IniParser Parser;

	char szBuf[256];
	char szPathNoExt[MAX_PATH];
	char szNewPath[MAX_PATH];
	strncpy( szPathNoExt, szPath, sizeof( szPathNoExt) -1);
	i3::remove_ext( szPathNoExt );

	// Load Dictionary
	sprintf( (PSTR) szNewPath, (PSTR) "%s.dic", szPathNoExt );
	if( ! LoadDictionary( szNewPath ) )
	{
		goto Error;
	}

	if( ! Parser.OpenFromFile( szPath ) )
	{
		I3TRACE( "[i3Font::Create] Could not open file. (%s)\n", szPath );
		goto Error;
	}

	if( ! Parser.ReadSection( "ImageInfo" ) )
	{
		I3TRACE( "[i3Font::Create] Could not find section. \"ImageInfo\"\n" );
		goto Error;
	}

	Parser.GetString( "Name", "", szBuf, sizeof(szBuf) );
	if( szBuf[0] )
	{
		SetName( szBuf);
	}
	else
	{
		i3::extract_filetitle(szPath, szBuf);
		SetName( szBuf);
	}

	if( ! Parser.GetValue( "NumImage", &m_nNumTexture ) )
	{
		goto Error;
	}
	if( ! Parser.GetValue( "Width", &m_nTextureWidth ) )
	{
		goto Error;
	}
	if( ! Parser.GetValue( "Height", &m_nTextureHeight ) )
	{
		goto Error;
	}
	if( ! Parser.GetValue( "SpaceX", &m_nSpaceX ) )
	{
		goto Error;
	}
	if( ! Parser.GetValue( "SpaceY", &m_nSpaceY ) )
	{
		goto Error;
	}

	if( ! Parser.ReadSection( "GlyphWidth" ) )
	{
		I3TRACE( "WARNING : [i3Font::Create] Could not find section. \"GlyphWidth\"\n" );
		goto Error;
	}
	else
	{
		if( ! Parser.GetValue( "DefaultWidth", 0, &m_nDefaultGlyphWidth ) )
		{
			goto Error;
		}
		if( ! Parser.GetValue( "DefaultHeight", 0, &m_nDefaultGlyphHeight ) )
		{
			goto Error;
		}

		memset( m_nASCIIGlyphWidth, m_nDefaultGlyphWidth, 128 );

		INT32 i;
		for( i = 0; i < m_NumGlyph; i++ )
		{
			if( m_pDictionary[i] >= 128 ) break;

			char szLabel[32];
			sprintf( (PSTR) szLabel, (PSTR) "%04X", m_pDictionary[i] );
			INT32 nWidth;
			if( Parser.GetValue( szLabel, &nWidth ) )
			{
				I3ASSERT( 0 <= nWidth && nWidth <= 255 );
				I3ASSERT( i < 128);
				m_nASCIIGlyphWidth[i] = UINT8( nWidth );
			}
		}
	}

	// Create Textures
	INT32 i;
	for( i = 0; i < m_nNumTexture; i++ )
	{
		sprintf( (PSTR) szNewPath, (PSTR) "%s%d.i3i", szPathNoExt, i );

		i3ImageFile file;
		i3Texture *pTexture = file.Load( szNewPath );
		if( pTexture == nullptr )
		{
			I3ASSERT_0;
		}
		m_pTextureList.push_back( pTexture );

		//i3ImageFile file;
		//file.Save( "_Dump/dump.tga", pTexture );
	}

	CalcGlyphCountPerImage();

	bRet = true;
Error:
	Parser.Close();
	return bRet;
}

bool i3Font::Create(
			const char *szName,			// 폰트 이름
			INT32	nTextureCount,		// 텍스쳐 개수
			INT32	nTextureWidth,		// 텍스쳐 크기
			INT32	nTextureHeight,		// 텍스쳐 높이
			INT32	nStartX,			// 이미지에서 폰트 시작 위치 X
			INT32	nStartY,			// 이미지에서 폰트 시작 위치 Y
			INT32	nSpaceX,			// 문자 사이 간격
			INT32	nSpaceY,			// 문자 사이 간격
			INT32	nDefaultGlyphWidth,	// 문자 기본 가로 크기
			INT32	nDefaultGlyphHeight,// 문자 기본 세로 크기
			INT32	nNumGlyph			// 문자 개수
			)
{
	INT32 i;
	SetName( szName );

	m_nNumTexture		= nTextureCount;
	for( i = 0; i < m_nNumTexture; i++ )
	{
		m_pTextureList.push_back( nullptr );
	}

	m_nTextureWidth		= nTextureWidth;
	m_nTextureHeight	= nTextureHeight;

	m_nStartX			= nStartX;
	m_nStartY			= nStartY;
	m_nSpaceX			= nSpaceX;
	m_nSpaceY			= nSpaceY;

	m_nDefaultGlyphWidth	= nDefaultGlyphWidth;
	m_nDefaultGlyphHeight	= nDefaultGlyphHeight;

	memset( m_nASCIIGlyphWidth, m_nDefaultGlyphWidth, 128 );

	I3ASSERT( nNumGlyph < 65536 );
	m_NumGlyph			= (UINT16)nNumGlyph;

	I3MEM_SAFE_FREE_POINTER( m_pDictionary);

	//I3HMEM hMem = i3MemAlloc( sizeof(WCHAR16) * m_NumGlyph );
	m_pDictionary = (WCHAR16 *) i3MemAlloc( sizeof(WCHAR16) * m_NumGlyph );
	memset( m_pDictionary, 0, sizeof(WCHAR16) * m_NumGlyph );

	CalcGlyphCountPerImage();

	return	true;
}

void i3Font::SetTexture( INT32 nIndex, i3Texture *pTexture )
{
	I3ASSERT( pTexture != nullptr);
	I3ASSERT( 0 <= nIndex && nIndex < (INT32)m_pTextureList.size() );
	I3_REF_CHANGE( m_pTextureList[nIndex], pTexture);
}

void i3Font::SetGlyph( INT32 nIndex, WCHAR16 nGlyph, INT32 Width )
{
	I3ASSERT( 0 <= nIndex && nIndex < m_NumGlyph );
	I3ASSERT( 0 < nGlyph );

	m_pDictionary[ nIndex ] = nGlyph;

	if( nGlyph < 128 && Width >= 0 )
	{
		I3ASSERT( Width < 256 );
		m_nASCIIGlyphWidth[ nGlyph ] = (UINT8) Width;
	}
}

void i3Font::CalcGlyphCountPerImage()
{
	I3ASSERT( m_nSpaceX > 0 );
	I3ASSERT( m_nSpaceY > 0 );
	m_nNumCol = INT32( ( m_nTextureWidth - m_nStartX ) / m_nSpaceX );
	m_nNumRow = INT32( ( m_nTextureHeight - m_nStartY ) / m_nSpaceY );
	m_nNumCharPerImage = m_nNumCol * m_nNumRow;
}

i3TextAttr* i3Font::CreateTextAttr( INT32 nMaxText )
{
	i3TextAttr *pTextAttr = nullptr;

	pTextAttr = i3TextAttr::new_object();

	if( ! pTextAttr->Create( this, nMaxText ) )
	{
		I3_SAFE_RELEASE( pTextAttr );
	}

	return pTextAttr;
}

void i3Font::GetGlyphInfoByIndex( I3_GLYPH_INFO *pInfo, UINT16 GyphIndex )
{
	INT32 y, x, Index;

	// 텍스쳐 인덱스 계산
	I3ASSERT( m_nNumCharPerImage > 0 );
	I3ASSERT( GyphIndex < m_NumGlyph );
	pInfo->nTextureIndex = UINT32( GyphIndex / m_nNumCharPerImage );
	I3ASSERT( pInfo->nTextureIndex < m_nNumTexture );

	// 해당 텍스쳐 내에서의 인덱스 구하기
	Index = GyphIndex % m_nNumCharPerImage;

	// 폭 얻기
	pInfo->nWidth  = ( GyphIndex < 128 ) ? m_nASCIIGlyphWidth[GyphIndex] : m_nDefaultGlyphWidth;
	pInfo->nHeight = m_nDefaultGlyphHeight;

	// UV 좌표 계산
	x = Index % m_nNumCol;
	y = Index / m_nNumCol;
	REAL32 u1 = (REAL32)(m_nStartX + ( x * m_nSpaceX ));
	REAL32 u2 = u1 + (REAL32)pInfo->nWidth;
	REAL32 v1 = (REAL32)(m_nStartY + ( y * m_nSpaceY ));
	REAL32 v2 = v1 + (REAL32)pInfo->nHeight;

#if !defined( I3G_PSP)
	// Normalize UV to 0.0~1.0 range

	#if defined( I3_FIXED_POINT)
		u1 /= m_nTextureWidth;
		u2 /= m_nTextureWidth;
		v1 /= m_nTextureHeight;
		v2 /= m_nTextureHeight;
	#else
		REAL32 rcx, rcy;
		rcx = i3Math::Reciprocal( (REAL32)m_nTextureWidth);
		rcy = i3Math::Reciprocal( (REAL32)m_nTextureHeight);

		u1 *= rcx;	
		u2 *= rcx;
		v1 *= rcy;
		v2 *= rcy;
	#endif
#endif

	i3Vector::SetU( &pInfo->UV[0], u1 );
	i3Vector::SetV( &pInfo->UV[0], v1 );
	i3Vector::SetU( &pInfo->UV[1], u2 );
	i3Vector::SetV( &pInfo->UV[1], v2 );
}

void i3Font::GetGlyphInfoByChar( I3_GLYPH_INFO *pInfo, WCHAR16 wCh )
{
	UINT16 nIndex = GetCharIndex( wCh );
	GetGlyphInfoByIndex( pInfo, nIndex );
}

UINT16 i3Font::GetCharIndex( WCHAR16 wCh )
{
	INT32 nLow = 0;
	INT32 nHigh = m_NumGlyph - 1;
	INT32 nMid;

	while( nLow <= nHigh )
	{
		nMid = ( nLow + nHigh ) >> 1;
		if( m_pDictionary[nMid] < wCh )
		{
			nLow = nMid + 1;
		}
		else if( m_pDictionary[nMid] > wCh )
		{
			nHigh = nMid - 1;
		}
		else
		{
			return UINT16( nMid );
		}
	}

#if defined( I3_DEBUG) && !defined( I3_WIPI)
	I3TRACE( "[i3Font::GetCharIndex] Not registered character. code = %d\n", wCh );
#endif
	return 0;
}

void i3Font::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	
	i3Texture * pTex;

	for(size_t i = 0; i < m_pTextureList.size(); i++)
	{
		pTex = m_pTextureList[i];

		pTex->OnBuildObjectList( List);
	}

	i3NamedElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

#define I3_SIG_FONT_DICTIONARY	"I3FD"

namespace pack
{
	struct PACKED FONT_DICTIONARY_HEADER
	{
		char SIG[4] = { 'I', '3', 'F', 'D' };
		UINT8 nMajorVersion = 0;
		UINT8 nMinorVersion = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

bool i3Font::LoadDictionary( const char *szFileName )
{
	i3FileStream Stream;
	bool Rv;

	//Open Text File
	if ( ! Stream.Open( szFileName, STREAM_READ | STREAM_SHAREREAD ) )
	{
		I3TRACE1( "[i3Font::LoadDictionary] 사전 파일을 로드할수 없습니다. %s\n", szFileName ); 
		return false;
	}

	Rv = LoadDictionary( &Stream);

	Stream.Close(); 

	return Rv;
}

bool i3Font::LoadDictionary( i3Stream * pStream)
{
	pack::FONT_DICTIONARY_HEADER Header; 
	UINT32 Rc;

	Rc = pStream->Read( &Header, sizeof(Header) );
	if( Rc == STREAM_ERR)
	{
		I3TRACE( "[i3Font::LoadDictionary] Could not read header information\n");
		return false;
	}

	if( strncmp( Header.SIG, I3_SIG_FONT_DICTIONARY, 4 ) != 0 )
	{
		I3TRACE( "[i3Font::LoadDictionary] 사전 파일이 아닙니다.\n"); 
		return false;
	}

	if( Header.nMajorVersion != 1 || Header.nMinorVersion != 0 )
	{
		I3TRACE( "[i3Font::LoadDictionary] 사전 파일의 버전이 지원하지 않는 버전입니다.\n"); 
		return false;
	}

	Rc = pStream->Read( &m_NumGlyph, sizeof(INT32) );
	if( Rc == STREAM_ERR)
	{
		I3TRACE( "[i3Font::LoadDictionary] Could not read Glyph data\n");
		return false;
	}

	I3MEM_SAFE_FREE_POINTER( m_pDictionary);
	//I3HMEM hMem = i3MemAlloc( sizeof(WCHAR16) * m_NumGlyph);
	m_pDictionary = (WCHAR16 *) i3MemAlloc( sizeof(WCHAR16) * m_NumGlyph);

	UINT32 nReadSize = sizeof(WCHAR16) * m_NumGlyph;
	if( pStream->Read( m_pDictionary, nReadSize ) != nReadSize )
	{
		I3TRACE( "[i3Font::LoadDictionary] 사전 파일 읽기 에러.\n");
		return false;
	}

	return true;
}

bool i3Font::SaveDictionary( const char *szFileName )
{
	i3FileStream Stream;
	bool Rv;

	//Open Text File
	if ( ! Stream.Create( szFileName, STREAM_WRITE ) )
	{
		I3TRACE1( "[i3Font::LoadDictionary] 사전 파일을 생성할 수 없습니다. %s\n", szFileName ); 
		return false;
	}

	Rv = SaveDictionary( &Stream);

	Stream.Close(); 

	if( Rv == false)
	{
		return false;
	}
	return true;
}

bool i3Font::SaveDictionary( i3Stream * pStream)
{
	pack::FONT_DICTIONARY_HEADER Header; 
	UINT32 Rc;

	Header.nMajorVersion = 1;
	Header.nMinorVersion = 0;

	Rc = pStream->Write( &Header, sizeof(Header) );
	if( Rc == STREAM_ERR)
	{
		I3TRACE( "[i3Font::LoadDictionary] Could not read header information\n");
		return false;
	}

	Rc = pStream->Write( &m_NumGlyph, sizeof(INT32) );
	if( Rc == STREAM_ERR)
	{
		I3TRACE( "[i3Font::LoadDictionary] Could not write Glyph data\n");
		return false;
	}

	UINT32 nSize = sizeof(WCHAR16) * m_NumGlyph;
	if( pStream->Write( m_pDictionary, nSize ) != nSize )
	{
		I3TRACE( "[i3Font::LoadDictionary] 사전 파일 쓰기 에러.\n");
		return false;
	}

	return true;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 FONT
	{
		UINT8		m_nVersion = 0;
		INT32		m_TextureCount = 0;
		INT32		m_nTextureWidth = 0;
		INT32		m_nTextureHeight = 0;
		INT32		m_nSpaceX = 0, m_nSpaceY = 0;
		INT32		m_nDefaultGlyphWidth = 0;
		INT32		m_nDefaultGlyphHeight = 0;
		UINT8		m_nASCIIGlyphWidth[128] = { 0 };
		INT8		reserved[64] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#define I3FONT_VERSION 1

UINT32 i3Font::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::FONT data;
	
	OBJREF ref;

	// Named Element
	Result = i3NamedElement::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Header

	data.m_nVersion	= I3FONT_VERSION;
	data.m_TextureCount = m_nNumTexture;
	data.m_nTextureWidth = m_nTextureWidth;
	data.m_nTextureHeight = m_nTextureHeight;
	data.m_nSpaceX = m_nSpaceX;
	data.m_nSpaceY = m_nSpaceY;
	data.m_nDefaultGlyphWidth = m_nDefaultGlyphWidth;
	data.m_nDefaultGlyphHeight = m_nDefaultGlyphHeight;
	memcpy( data.m_nASCIIGlyphWidth, m_nASCIIGlyphWidth, sizeof( m_nASCIIGlyphWidth ) );

	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3TRACE( "i3Font::OnSave() - Could not save font data.\n");
		return STREAM_ERR;
	}
	Result += Rc;

	// Dictionary
	Rc = SaveDictionary( pStream);
	if( Rc == STREAM_ERR)
	{
		return STREAM_ERR;
	}
	Result += Rc;

	// Texture References
	for(size_t i = 0; i < m_pTextureList.size(); i++)
	{
		ref = (OBJREF) pResFile->GetObjectPersistID( m_pTextureList[i]);

		Rc = pStream->Write( &ref, sizeof(ref));
		if( Rc == STREAM_ERR)
		{
			I3TRACE( "i3Font::OnSave() - Could not write texture reference objects.\n");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3Font::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::FONT data;
	INT32 i;

	// Named Element
	Result = i3NamedElement::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Header
	Rc = pStream->Read( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3TRACE( "i3Font::OnLoad() - Could not read font data.\n");
		return STREAM_ERR;
	}
	Result += Rc;

	if( data.m_nVersion	!= I3FONT_VERSION )
	{
		I3TRACE( "i3Font::OnLoad() - Invalid i3Font version.\n");
		return STREAM_ERR;
	}

	m_nNumTexture = data.m_TextureCount;
	m_nTextureWidth = data.m_nTextureWidth;
	m_nTextureHeight = data.m_nTextureHeight;
	m_nSpaceX = data.m_nSpaceX;
	m_nSpaceY = data.m_nSpaceY;
	m_nDefaultGlyphWidth = data.m_nDefaultGlyphWidth;
	m_nDefaultGlyphHeight = data.m_nDefaultGlyphHeight;
	memcpy( m_nASCIIGlyphWidth, data.m_nASCIIGlyphWidth, sizeof( m_nASCIIGlyphWidth ) );

	// Dictionary
	Rc = LoadDictionary( pStream);
	if( Rc == STREAM_ERR)
	{
		return STREAM_ERR;
	}
	Result += Rc;

	// Texture References
	if( m_nNumTexture > 0)
	{
		OBJREF ref;
		i3Texture * pTex;

		m_pTextureList.resize( m_nNumTexture);

		for( i = 0; i < m_nNumTexture; i++)
		{
			Rc = pStream->Read( &ref, sizeof(ref));
			if( Rc == STREAM_ERR)
			{
				I3TRACE( "i3Font::OnLoad() - Could not read texture reference objects.\n");
				return STREAM_ERR;
			}
			Result += Rc;

			pTex = (i3Texture *) pResFile->FindObjectByID( ref);
			I3_MUST_ADDREF( pTex);
			m_pTextureList[i] = pTex;
		}
	}

	CalcGlyphCountPerImage();

	return Result;
}

i3Font * i3Font::LoadFromFile( const char * pszPath)
{
	i3ResourceFile file;
	i3PersistantElement * pElm;
	i3Font * pFont = nullptr;
	INT32 i;

	if( file.Load( pszPath) == STREAM_ERR)
		return nullptr;

	for( i = 0; i < file.GetObjectCount(); i++)
	{
		pElm = file.GetObject( i);

		if( i3::kind_of<i3Font*>(pElm))
		{
			pFont = (i3Font *) pElm;
			break;
		}
	}

	return pFont;
}

i3Font * i3Font::LoadFromFile( i3Stream * pStream)
{
	i3ResourceFile file;
	i3PersistantElement * pElm;
	i3Font * pFont = nullptr;
	INT32 i;

	file.Load( pStream);

	for( i = 0; i < file.GetObjectCount(); i++)
	{
		pElm = file.GetObject( i);

		if( i3::kind_of<i3Font*>(pElm))
		{
			pFont = (i3Font *) pElm;
			break;
		}
	}

	I3_SAFE_ADDREF( pFont);

	return pFont;
}

bool i3Font::SaveToFile( const char * pszPath)
{
	i3ResourceFile file;

	file.AddObject( this);

	if( file.Save( pszPath) == STREAM_ERR)
		return false;

	return true;
}

bool i3Font::SaveToFile( i3Stream * pStream)
{
	i3ResourceFile file;
 
	file.AddObject( this);

	if( file.Save( pStream) == STREAM_ERR)
	{
		return false;
	}

	return true;
}
