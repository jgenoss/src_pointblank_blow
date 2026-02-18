#ifndef __I3_FONT_H__
#define __I3_FONT_H__

class i3TextAttr;

typedef struct
{
	INT32 nTextureIndex;
	INT32 nWidth, nHeight;
	RT_VEC2D UV[2];
} I3_GLYPH_INFO;

class I3_EXPORT_SCENE i3Font : public i3NamedElement
{
	I3_CLASS_DEFINE( i3Font );
protected:
	i3List			*m_pTextureList;	// 텍스쳐 리스트

	INT32			m_nNumTexture;		// 텍스쳐 개수
	INT32			m_nTextureWidth;	// 이미지 크기 가로
	INT32			m_nTextureHeight;	// 이미지 크기 세로
	INT32			m_nStartX;			// 폰트 이미지 시작 X 위치
	INT32			m_nStartY;			// 폰트 이미지 시작 Y 위치
	//INT32			m_nEndX;			// 폰트 이미지 끝 X 위치
	//INT32			m_nEndY;			// 폰트 이미지 끝 Y 위치
	INT32			m_nSpaceX;			// 폰트 간격 가로
	INT32			m_nSpaceY;			// 폰트 간격 세로
	INT32			m_nDefaultGlyphWidth;	// 기본 폰트 사이즈 가로
	INT32			m_nDefaultGlyphHeight;	// 기본 폰트 사이즈 세로
	UINT8			m_nASCIIGlyphWidth[128];	// ASCII 문자폭

	INT32			m_nNumCol;
	INT32			m_nNumRow;
	INT32			m_nNumCharPerImage;

	UINT16			m_NumGlyph;
	WCHAR16			*m_pDictionary;

protected:
	BOOL	LoadDictionary( const char *szFileName );
	BOOL	LoadDictionary( i3Stream * pStream);

	BOOL	SaveDictionary( const char *szFileName );
	BOOL	SaveDictionary( i3Stream * pStream);

public:
	i3Font();
	~i3Font();

	BOOL Create( const char *szPath );

	BOOL Create(
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

	const char *GetFontName()				{ return GetName();	}

	UINT32 GetNumTexture()					{ return m_nNumTexture; }
	i3Texture* GetTexture( INT32 nIndex )	{ return (i3Texture*)m_pTextureList->GetItem( nIndex ); }

	i3TextAttr* CreateTextAttr( INT32 nMaxText );

	void GetGlyphInfoByIndex( I3_GLYPH_INFO *pInfo, UINT16 GyphIndex );
	void GetGlyphInfoByChar( I3_GLYPH_INFO *pInfo, WCHAR16 wCh );

	UINT16 GetCharIndex( WCHAR16 wCh );
	INT32  GetGlyphWidth( INT32 nIndex ){ return ( nIndex < 128 ) ? m_nASCIIGlyphWidth[nIndex] : m_nDefaultGlyphWidth; }
	INT32  GetGlyphHeight(){ return m_nDefaultGlyphHeight; }

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	static i3Font *	LoadFromFile( const char * pszPath);
	static i3Font * LoadFromFile( i3Stream * pStream);

	BOOL			SaveToFile( const char * pszPath);
	BOOL			SaveToFile( i3Stream * pStream);
};

#endif //__I3_FONT_H__
