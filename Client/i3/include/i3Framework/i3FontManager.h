#ifndef __I3_FONT_MANAGER_H__
#define	__I3_FONT_MANAGER_H__

class i3Font;
class i3StringTable;
class i3ResourceManager;

class I3_EXPORT_FRAMEWORK i3FontManager : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3FontManager, i3ElementBase );

protected:

	static i3FontManager	*m_pInstance;

#ifdef I3_DEBUG
	i3Font				*m_pDebugFont = nullptr;
#endif

	i3ResourceManager	*m_pResourceManger = nullptr;

	i3Font				**m_pFont = nullptr;
	i3StringTable		**m_pStringTable = nullptr;

	INT32				m_nMaxFont = 0;
	INT32				m_nMaxStringTable = 0;
	
public:
	i3FontManager(void);
	~i3FontManager(void);

	static i3FontManager *GetInstance(){ return  m_pInstance; }

	// 생성
	bool Create( i3ResourceManager *pResMgr, INT32 nMaxFont, INT32 nMaxStringTable, const char *szDebugFontPath );

#ifdef I3_DEBUG
	i3Font*			GetDebugFont(){ return m_pDebugFont; }
#endif

	// 폰트 제거하기
	void			ReleaseFont( INT32 nIndex )
	{
		I3ASSERT( 0 <= nIndex && nIndex < m_nMaxFont );
        I3_SAFE_RELEASE( m_pFont[nIndex] );
	}

	// 폰트 생성하기
	bool			CreateFont( INT32 nIndex, const char* szPath, bool bReferLangCode = true );
	
	// 폰트 복사( 포인터만 복사 & 레퍼런스 증가 )
	i3Font*			CopyFont( INT32 DestType, INT32 SrcType );

	// 인덱스로 폰트 가져오기
	i3Font*			GetFont( INT32 nIndex )
	{
		I3ASSERT( 0 <= nIndex && nIndex < m_nMaxFont );
		return m_pFont[nIndex];
	}

	// 이름으로 폰트 가져오기
	virtual i3Font*	GetFontByName( const char *szName );

	// 스트링 테이블 해제
	void			ReleaseStringTable( INT32 nIndex )
	{
		I3ASSERT( 0 <= nIndex && nIndex < m_nMaxStringTable );
		I3_SAFE_RELEASE( m_pStringTable[nIndex] );
	}

	// 스트링 테이블 생성하가
	bool CreateStringTable( INT32 nIndex, const char* szPath, bool bReferLangCode = true);

	// 인덱스로 스트링 테이블 가져오기
	i3StringTable*	GetStringTable( INT32 nIndex )
	{
		I3ASSERT( 0 <= nIndex && nIndex < m_nMaxStringTable );
		return m_pStringTable[nIndex];
	}

	// 스트링 테이블에서 지정된 인덱스 스트링 얻기
	const WCHAR16*	GetStringByIndex( INT32 nIndex, INT32 nStringIndex );

	// 스트링 테이블에서 지정된 라벨의 스트링 얻기
	const WCHAR16*	GetString( INT32 nIndex, const char* szLabel );

	// 스트링 테이블에서 지정된 라벨의 스트링 Index 얻기
	INT32 GetStringIndex( INT32 nIndex, const char* szLabel );
};

#endif // __FONT_MANAGER_H__
