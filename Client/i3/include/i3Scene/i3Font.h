#ifndef __I3_FONT_H__
#define __I3_FONT_H__

class i3TextAttr;

struct I3_GLYPH_INFO
{
	INT32 nTextureIndex = 0;
	INT32 nWidth = 0, nHeight = 0;
	VEC2D UV[2];
} ;

class I3_EXPORT_SCENE i3Font : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3Font, i3NamedElement );
protected:
	i3::vector<i3Texture*> m_pTextureList;	// 텍스쳐 리스트

	INT32			m_nNumTexture = 0;		// 텍스쳐 개수
	INT32			m_nTextureWidth = 0;	// 이미지 크기 가로
	INT32			m_nTextureHeight = 0;	// 이미지 크기 세로
	INT32			m_nStartX = 0;			// 폰트 이미지 시작 X 위치
	INT32			m_nStartY = 0;			// 폰트 이미지 시작 Y 위치
	INT32			m_nSpaceX = 0;			// 폰트 간격 가로
	INT32			m_nSpaceY = 0;			// 폰트 간격 세로
	INT32			m_nDefaultGlyphWidth = 0;	// 기본 폰트 사이즈 가로
	INT32			m_nDefaultGlyphHeight = 0;	// 기본 폰트 사이즈 세로
	UINT8			m_nASCIIGlyphWidth[128] = { 0 };	// ASCII 문자폭

	INT32			m_nNumCol = 0;
	INT32			m_nNumRow = 0;
	INT32			m_nNumCharPerImage = 0;

	UINT16			m_NumGlyph = 0;
	WCHAR16			*m_pDictionary = nullptr;

protected:
	bool	LoadDictionary( const char *szFileName );
	bool	LoadDictionary( i3Stream * pStream);

	bool	SaveDictionary( const char *szFileName );
	bool	SaveDictionary( i3Stream * pStream);

public:
	~i3Font();

	bool Create( const char *szPath );

	bool Create(
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
	);

	void SetTexture( INT32 nIndex, i3Texture *pTexture );
	void SetGlyph( INT32 nIndex, WCHAR16 nGlyph, INT32 Width = -1 );

    void	SetStartX( INT32 nStartX )		{ m_nStartX = nStartX;	}
	void	SetStartY( INT32 nStartY )		{ m_nStartY = nStartY;	}
	INT32	GetStartX()						{ return m_nStartX;	}
	INT32	GetStartY()						{ return m_nStartY;	}

	void CalcGlyphCountPerImage();

	const char *GetFontName() const				{ return GetName();	}
	const i3::rc_string&	GetFontNameString() const { return GetNameString(); }

	UINT32 GetNumTexture()					{ return m_nNumTexture; }
	i3Texture* GetTexture( INT32 nIndex )	{ return m_pTextureList[nIndex ]; }

	i3TextAttr* CreateTextAttr( INT32 nMaxText );

	void GetGlyphInfoByIndex( I3_GLYPH_INFO *pInfo, UINT16 GyphIndex );
	void GetGlyphInfoByChar( I3_GLYPH_INFO *pInfo, WCHAR16 wCh );

	UINT16 GetCharIndex( WCHAR16 wCh );
	INT32  GetGlyphWidth( INT32 nIndex ){ return ( nIndex < 128 ) ? m_nASCIIGlyphWidth[nIndex] : m_nDefaultGlyphWidth; }
	INT32  GetGlyphHeight(){ return m_nDefaultGlyphHeight; }

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	static i3Font *	LoadFromFile( const char * pszPath);
	static i3Font * LoadFromFile( i3Stream * pStream);

	bool			SaveToFile( const char * pszPath);
	bool			SaveToFile( i3Stream * pStream);
};

#endif //__I3_FONT_H__
