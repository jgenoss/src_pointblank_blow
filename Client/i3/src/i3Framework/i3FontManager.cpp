#include "i3FrameworkPCH.h"
#include "i3FontManager.h"

#include "i3ResourceManager.h"

//I3_CLASS_INSTANCE( i3FontManager, i3ElementBase );
I3_CLASS_INSTANCE( i3FontManager);

i3FontManager *i3FontManager::m_pInstance = nullptr;

// 생성자
i3FontManager::i3FontManager(void)
{
	I3ASSERT( m_pInstance == nullptr );
	m_pInstance = this;
}

// 소멸자
i3FontManager::~i3FontManager(void)
{
	I3ASSERT( m_pInstance == this );
	m_pInstance = nullptr;

#ifdef I3_DEBUG
	I3_SAFE_RELEASE( m_pDebugFont );
#endif

	INT32 i;

	if( m_pFont != nullptr)
	{
		for( i = 0; i< m_nMaxFont; i++)
		{	
			I3_SAFE_RELEASE( m_pFont[i] );
		}

		I3MEM_SAFE_FREE_POINTER( m_pFont );
	}

	if( m_pStringTable != nullptr)
	{
		for( i = 0; i< m_nMaxStringTable; i++)
		{	
			I3_SAFE_RELEASE( m_pStringTable[i] );
		}

		I3MEM_SAFE_FREE_POINTER( m_pStringTable );
	}
}

// 폰트 매니저 생성
bool i3FontManager::Create( i3ResourceManager *pResMgr, INT32 nMaxFont, INT32 nMaxStringTable, const char *szDebugFontPath )
{
	I3ASSERT( m_pFont == nullptr );
	I3ASSERT( m_pStringTable == nullptr );

	m_pResourceManger = pResMgr;

	m_nMaxFont = nMaxFont;
	m_nMaxStringTable = nMaxStringTable;

	if( nMaxFont > 0 )
	{
		//m_pFont = (i3Font**)I3MEM_ALLOC( m_hFontMemID, sizeof( i3Font* ) * nMaxFont );
		m_pFont = (i3Font**)i3MemAlloc( sizeof( i3Font* ) * nMaxFont );
		I3ASSERT( m_pFont != nullptr );
		memset( m_pFont, 0, sizeof( i3Font* ) * nMaxFont );
	}

	if( nMaxStringTable > 0 )
	{
		//m_pStringTable = (i3StringTable**)I3MEM_ALLOC( m_hStringTableMemID, sizeof( i3StringTable* ) * nMaxStringTable );
		m_pStringTable = (i3StringTable**)i3MemAlloc( sizeof( i3StringTable* ) * nMaxStringTable );
		I3ASSERT( m_pStringTable != nullptr );
		memset( m_pStringTable, 0, sizeof( i3StringTable* ) * nMaxStringTable );
	}

#ifdef I3_DEBUG
	if( szDebugFontPath != nullptr )
	{
		I3ASSERT( m_pResourceManger != nullptr );
		I3ASSERT( m_pDebugFont == nullptr );
		m_pDebugFont = m_pResourceManger->LoadFont( szDebugFontPath );
		if( m_pDebugFont == nullptr )
		{
			i3Log( "i3FontManager::Create", "Could not create debug font. (%s)", szDebugFontPath );
			return false;
		}
	}
#endif

	return true;
}

// 폰트 생성
bool i3FontManager::CreateFont( INT32 nIndex, const char* szPath, bool bReferLangCode )
{
	I3_SAFE_RELEASE( m_pFont[nIndex] );
	m_pFont[nIndex] = m_pResourceManger->LoadFont( szPath, bReferLangCode );
	if( m_pFont[nIndex] == nullptr )
	{
		i3Log( "i3FontManager::CreateFont", "Could not create font. (%s)", szPath );
		return false;
	}
	return true;
}

// 폰트 복사( 포인터만 복사 & 레퍼런스 증가 )
i3Font*	i3FontManager::CopyFont( INT32 nDest, INT32 nSrc )
{
	I3_REF_CHANGE( m_pFont[nDest], m_pFont[nSrc] );
	return m_pFont[nDest];
}

// 이름으로 폰트 가져오기
i3Font* i3FontManager::GetFontByName( const char *szName )
{
	INT32 i;
	for( i = 0; i < m_nMaxFont; i++ )
	{
		if( m_pFont[i] != nullptr )
		{
			I3ASSERT( m_pFont[i]->GetFontName() != nullptr );
			if( i3::generic_is_iequal( szName, m_pFont[i]->GetFontName() ) )
			{
				return m_pFont[i];
			}
		}
	}

	return nullptr;
}

// String Table 생성
bool i3FontManager::CreateStringTable( INT32 nIndex, const char* szPath, bool bReferLangCode )
{
	I3ASSERT( 0 <= nIndex && nIndex < m_nMaxStringTable );
	I3_SAFE_RELEASE( m_pStringTable[nIndex] );
	m_pStringTable[nIndex] = m_pResourceManger->LoadStringTable( szPath, bReferLangCode );
	if( m_pStringTable[nIndex] == nullptr )
	{
		i3Log( "i3FontManager::CreateStringTable", "Failed to create string table. (%s)", szPath );
		return false;
	}

	return true;
}

// 스트링 테이블에서 지정된 인덱스 스트링 얻기
const WCHAR16*	i3FontManager::GetStringByIndex( INT32 nIndex, INT32 nStringIndex )
{
	I3ASSERT( 0 <= nIndex && nIndex < m_nMaxStringTable );
	if( m_pStringTable[nIndex] == nullptr )
	{
		I3TRACE( "ERROR : Could not find StringTable[%d]", nIndex );
		I3ASSERT_0;
		return nullptr;
	}
	return m_pStringTable[nIndex]->GetString( nStringIndex );
}

// 스트링 테이블에서 지정된 라벨의 스트링 얻기
const WCHAR16*	i3FontManager::GetString( INT32 nIndex, const char* szLabel )
{
	I3ASSERT( 0 <= nIndex && nIndex < m_nMaxStringTable );
	if( m_pStringTable[nIndex] == nullptr )
	{
		I3TRACE( "ERROR : Could not find StringTable[%d]", nIndex );
		I3ASSERT_0;
		return nullptr;
	}
	return m_pStringTable[nIndex]->GetString(szLabel);
}

// 스트링 테이블에서 지정된 라벨의 스트링 인덱스 얻기
INT32 i3FontManager::GetStringIndex( INT32 nIndex, const char* szLabel )
{
	I3ASSERT( 0 <= nIndex && nIndex < m_nMaxStringTable );
	if( m_pStringTable[nIndex] == nullptr )
	{
		I3TRACE( "ERROR : Could not find StringTable[%d]", nIndex );
		I3ASSERT_0;
		return -1;
	}
	return m_pStringTable[nIndex]->GetStringIndex( szLabel );
}

