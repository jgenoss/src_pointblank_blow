#include "i3GfxType.h"
#include "i3Font.h"

I3_CLASS_INSTANCE( i3Font, i3NamedElement);

i3Font::i3Font(void)
{
	m_Height = 0.0f;
	m_GroupCount = 0;
	m_pGroups = NULL;
}

i3Font::~i3Font(void)
{
	INT32 i;

	if( m_pGroups == NULL)
	{
		for( i = 0; i < m_GroupCount; i++)
		{
			if( m_pGroups[i].m_pTable != NULL)
			{
				I3MEM_SAFE_FREE( m_pGroups[i].m_pTable);
			}
		}

		i3MemFree( m_pGroups);
		m_pGroups = NULL;
	}
}

void i3Font::SetGroupCount( UINT16 count)
{
	m_GroupCount = count;

	if( m_pGroups == NULL)
	{		
		m_pGroups = (I3FONTGROUPINFO *)i3MemAlloc( sizeof(I3FONTGROUPINFO) * m_GroupCount);		
	}
	else
	{		
		m_pGroups = (I3FONTGROUPINFO *)i3MemReAlloc( (void *) m_pGroups, sizeof(I3FONTGROUPINFO) * m_GroupCount);		
	}

	I3ASSERT( m_pGroups);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

typedef struct PACKED _i3TagFontHeader
{
	UINT32		m_GroupCount;
	REAL32		m_Height;
} I3_FONT_FILE_HEADER;

typedef struct PACKED _i3TagFontGroupInfo
{
	UINT16				m_StartCode;
	UINT16				m_EndCode;
	char				m_TextureName[MAX_NAME];
} I3_FONT_GROUP_INFO;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Font::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;

	Rc = SaveToFile( pResFile->GetStream());

	return Rc;
}

UINT32 i3Font::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc;

	Rc = LoadFromFile( pResFile->GetStream());

	return Rc;
}

UINT32 i3Font::LoadFromFile( const char * pszFileName)
{
	i3FileStream Stream;
	BOOL Rv;
	UINT32 Result;

	Rv = Stream.Open( pszFileName, STREAM_READ);
	if( Rv == FALSE)
	{
		i3Log( "i3Font::LoadFromFile()", "Could not open %s file.", pszFileName);
		return STREAM_ERR;
	}

	Result = LoadFromFile( &Stream);

	Stream.Close();

	return Result;
}

UINT32 i3Font::LoadFromFile( i3Stream * pStream)
{
	I3_FONT_FILE_HEADER Header;
	UINT32 Rc, Result = 0;
	UINT32 i, sp, count;

	Rc = pStream->Read( &Header, sizeof(I3_FONT_FILE_HEADER));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Font::LoadFromFile()", "Could not read font file header.");
		return STREAM_ERR;
	}
	Result += Rc;

	I3ASSERT( Header.m_GroupCount > 0);

	// Group Count의 설정.
	// 이때, 필요한 메모리 공간도 확보한다.
	SetGroupCount( (UINT16) Header.m_GroupCount);

	// Font의 높이 값 설정.
	m_Height = Header.m_Height;

	// Group Table 읽어 들이기.
	{
		I3_FONT_GROUP_INFO * pGroupInfo;
		
		pGroupInfo = (I3_FONT_GROUP_INFO *)i3MemAlloc( sizeof(I3_FONT_GROUP_INFO) * m_GroupCount);		

		Rc = pStream->Read( pGroupInfo, sizeof(I3_FONT_GROUP_INFO) * m_GroupCount);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Font::LoadFromFile()", "Could not read font group information.");
			return STREAM_ERR;
		}
		Result += Rc;

		for( i = 0; i < m_GroupCount; i++)
		{
			m_pGroups[i].m_StartCode	= pGroupInfo[i].m_StartCode;
			m_pGroups[i].m_EndCode		= pGroupInfo[i].m_EndCode;
			m_pGroups[i].m_pTexture		= NULL;

			// Texture 이름 복사
			// 실제 Texture에 대한 Binding은 이후로 미룬다.
			// 또는 실제 Text를 출력하는 Module에서 처리할 수도 있다.
			sp = i3String::Length( pGroupInfo[i].m_TextureName);
			if( sp > 0)
			{				
				m_pGroups[i].m_szTextureName = (char *)i3MemAlloc( sp + 1);				
				i3String::Copy( m_pGroups[i].m_szTextureName, pGroupInfo[i].m_TextureName);
			}

			count = m_pGroups[i].m_EndCode - m_pGroups[i].m_StartCode + 1;

			// CHAR Info.를 위한 Buffer 할당.			
			m_pGroups[i].m_pTable = (I3FONTCHARINFO *)i3MemAlloc( sizeof(I3FONTCHARINFO) * count);			 
			I3ASSERT( m_pGroups[i].m_pTable);
			
			// Char Info.는 항상 순서대로 저장되어 있으며, Alignment는 적용하지 않는다.
			Rc = pStream->Read( m_pGroups[i].m_pTable, sizeof(I3FONTCHARINFO) * count);
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3Font::LoadFromFile()", "Could not read character code information.");
				return STREAM_ERR;
			}
			Result += Rc;
		}

		I3MEM_SAFE_FREE( pGroupInfo);
	}

	return Result;
}

UINT32 i3Font::SaveToFile( const char * pszFileName)
{
	i3FileStream Stream;
	BOOL Rv;
	UINT32 Result;

	Rv = Stream.Open( pszFileName, STREAM_WRITE);
	if( Rv == FALSE)
	{
		i3Log( "i3Font::SaveToFile()", "Could not create %s file.", pszFileName);
		return STREAM_ERR;
	}

	Result = SaveToFile( &Stream);

	Stream.Close();

	return Result;
}

UINT32 i3Font::SaveToFile( i3Stream * pStream)
{
	UINT32 Rc, Result = 0;
	UINT32 i, count;

	// File Header
	{
		I3_FONT_FILE_HEADER Header;

		Header.m_GroupCount = m_GroupCount;
		Header.m_Height		= m_Height;

		Rc = pStream->Write( &Header, sizeof(I3_FONT_FILE_HEADER));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Font::SaveToFile()", "Could not write font file header.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Group Information
	{
		I3_FONT_GROUP_INFO * pGroupInfo;
		
		pGroupInfo = (I3_FONT_GROUP_INFO *)i3MemAlloc( sizeof(I3_FONT_GROUP_INFO) * m_GroupCount);		 
		I3ASSERT( pGroupInfo != NULL);

		for( i = 0; i < m_GroupCount; i++)
		{
			pGroupInfo[i].m_StartCode		=	m_pGroups[i].m_StartCode;
			pGroupInfo[i].m_EndCode			=	m_pGroups[i].m_EndCode;

			i3String::Copy( pGroupInfo[i].m_TextureName, m_pGroups[i].m_szTextureName);
		}

		Rc = pStream->Write( pGroupInfo, sizeof(I3_FONT_GROUP_INFO) * m_GroupCount);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Font::SaveToFile()", "Could not write font group information");
			return STREAM_ERR;
		}
		Result += Rc;

		I3MEM_SAFE_FREE( pGroupInfo);
	}

	// Code Information
	for( i = 0; i < m_GroupCount; i++)
	{
		count = m_pGroups[i].m_EndCode - m_pGroups[i].m_StartCode + 1;

		Rc = pStream->Write( m_pGroups[i].m_pTable, sizeof(I3FONTCHARINFO) * count);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Font::LoadFromFile()", "Could not write code table information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}