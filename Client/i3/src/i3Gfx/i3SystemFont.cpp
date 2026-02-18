#include "i3GfxType.h"
#include "i3SystemFont.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3SystemFont);

i3SystemFont::~i3SystemFont(void)
{
	INT32 i;

	if( m_pGroups != nullptr)
	{
		for( i = 0; i < m_GroupCount; i++)
		{
			if( m_pGroups[i].m_pTable != nullptr)
			{
				i3MemFree( m_pGroups[i].m_pTable);
			}
		}

		i3MemFree( m_pGroups);
		m_pGroups = nullptr;
	}
}

void i3SystemFont::SetGroupCount( UINT16 count)
{
	m_GroupCount = count;

	if( m_pGroups == nullptr)
	{
		m_pGroups = (I3FONT_GROUPINFO *) i3MemAlloc( sizeof(I3FONT_GROUPINFO) * m_GroupCount);
	}
	else
	{
		m_pGroups = (I3FONT_GROUPINFO *) i3MemReAlloc( (void *) m_pGroups, sizeof(I3FONT_GROUPINFO) * m_GroupCount);
	}

	I3ASSERT( m_pGroups);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED FONT_FILE_HEADER
	{
		UINT32		m_GroupCount = 0;
		REAL32		m_Height = 0.0f;
	};

	struct PACKED FONT_GROUP_INFO
	{
		UINT16				m_StartCode = 0;
		UINT16				m_EndCode = 0;
		char				m_TextureName[MAX_NAME] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3SystemFont::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;

	Rc = SaveToFile( pResFile->GetStream());

	return Rc;
}

UINT32 i3SystemFont::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc;

	Rc = LoadFromFile( pResFile->GetStream());

	return Rc;
}

UINT32 i3SystemFont::LoadFromFile( const char * pszFileName)
{
	i3FileStream Stream;
	UINT32 Result;

	bool Rv = Stream.Open( pszFileName, STREAM_READ | STREAM_SHAREREAD);
	if( Rv == false)
	{
		i3Log( "i3SystemFont::LoadFromFile()", "Could not open %s file.", pszFileName);
		return STREAM_ERR;
	}

	Result = LoadFromFile( &Stream);

	Stream.Close();

	return Result;
}

UINT32 i3SystemFont::LoadFromFile( i3Stream * pStream)
{
	pack::FONT_FILE_HEADER Header;
	UINT32 Rc, Result = 0;
	UINT32 i, sp, count;

	Rc = pStream->Read( &Header, sizeof(pack::FONT_FILE_HEADER));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3SystemFont::LoadFromFile()", "Could not read font file header.");
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
		pack::FONT_GROUP_INFO * pGroupInfo;

		//INT32 idx; 
		//idx = i3MemAlloc( sizeof(I3_FONT_GROUP_INFO) * m_GroupCount);
		pGroupInfo = (pack::FONT_GROUP_INFO *) i3MemAlloc( sizeof(pack::FONT_GROUP_INFO) * m_GroupCount);

		Rc = pStream->Read( pGroupInfo, sizeof(pack::FONT_GROUP_INFO) * m_GroupCount);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SystemFont::LoadFromFile()", "Could not read font group information.");
			return STREAM_ERR;
		}
		Result += Rc;

		for( i = 0; i < m_GroupCount; i++)
		{
			m_pGroups[i].m_StartCode	= pGroupInfo[i].m_StartCode;
			m_pGroups[i].m_EndCode		= pGroupInfo[i].m_EndCode;
			m_pGroups[i].m_pTexture		= nullptr;

			// Texture 이름 복사
			// 실제 Texture에 대한 Binding은 이후로 미룬다.
			// 또는 실제 Text를 출력하는 Module에서 처리할 수도 있다.
			sp = i3::generic_string_size( pGroupInfo[i].m_TextureName);
			if( sp > 0)
			{
//				INT32 idx; 
//				idx = i3MemAlloc( sp + 1);
				m_pGroups[i].m_szTextureName = (char *) i3MemAlloc( sp + 1);

				i3::safe_string_copy( m_pGroups[i].m_szTextureName, pGroupInfo[i].m_TextureName, sp + 1);
			}

			count = m_pGroups[i].m_EndCode - m_pGroups[i].m_StartCode + 1;

			// CHAR Info.를 위한 Buffer 할당.			
			//idx = i3MemAlloc( sizeof(I3FONTCHARINFO) * count);
			m_pGroups[i].m_pTable = (i3::pack::FONT_CHARINFO *) i3MemAlloc( sizeof(i3::pack::FONT_CHARINFO) * count);
			I3ASSERT( m_pGroups[i].m_pTable);
			
			// Char Info.는 항상 순서대로 저장되어 있으며, Alignment는 적용하지 않는다.
			Rc = pStream->Read( m_pGroups[i].m_pTable, sizeof(i3::pack::FONT_CHARINFO) * count);
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3SystemFont::LoadFromFile()", "Could not read character code information.");
				i3MemFree(pGroupInfo);
				return STREAM_ERR;
			}
			Result += Rc;
		}

		i3MemFree( pGroupInfo);
	}

	return Result;
}

UINT32 i3SystemFont::SaveToFile( const char * pszFileName)
{
	i3FileStream Stream;
	UINT32 Result;

	bool Rv = Stream.Open( pszFileName, STREAM_WRITE);
	if( Rv == false)
	{
		i3Log( "i3SystemFont::SaveToFile()", "Could not create %s file.", pszFileName);
		return STREAM_ERR;
	}

	Result = SaveToFile( &Stream);

	Stream.Close();

	return Result;
}

UINT32 i3SystemFont::SaveToFile( i3Stream * pStream)
{
	UINT32 Rc, Result = 0;
	UINT32 i, count;

	// File Header
	{
		pack::FONT_FILE_HEADER Header;

		Header.m_GroupCount = m_GroupCount;
		Header.m_Height		= m_Height;

		Rc = pStream->Write( &Header, sizeof(pack::FONT_FILE_HEADER));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SystemFont::SaveToFile()", "Could not write font file header.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Group Information
	{
		pack::FONT_GROUP_INFO * pGroupInfo;

		//INT32 idx; 
		//idx = i3MemAlloc( sizeof(I3_FONT_GROUP_INFO) * m_GroupCount);
		pGroupInfo = (pack::FONT_GROUP_INFO *) i3MemAlloc( sizeof(pack::FONT_GROUP_INFO) * m_GroupCount);
		I3ASSERT( pGroupInfo != nullptr);

		for( i = 0; i < m_GroupCount; i++)
		{
			pGroupInfo[i].m_StartCode		=	m_pGroups[i].m_StartCode;
			pGroupInfo[i].m_EndCode			=	m_pGroups[i].m_EndCode;

			i3::safe_string_copy( pGroupInfo[i].m_TextureName, m_pGroups[i].m_szTextureName, MAX_NAME);
		}

		Rc = pStream->Write( pGroupInfo, sizeof(pack::FONT_GROUP_INFO) * m_GroupCount);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SystemFont::SaveToFile()", "Could not write font group information");
			i3MemFree(pGroupInfo);
			return STREAM_ERR;
		}
		Result += Rc;

		i3MemFree( pGroupInfo);
	}

	// Code Information
	for( i = 0; i < m_GroupCount; i++)
	{
		count = m_pGroups[i].m_EndCode - m_pGroups[i].m_StartCode + 1;

		Rc = pStream->Write( m_pGroups[i].m_pTable, sizeof(i3::pack::FONT_CHARINFO) * count);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SystemFont::LoadFromFile()", "Could not write code table information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}