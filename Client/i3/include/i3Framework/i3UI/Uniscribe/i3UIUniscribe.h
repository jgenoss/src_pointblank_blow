#pragma once

#include "Usp10.h"
#include "Uniscribe/i3UIText2.h"

struct SCRIPT_RUN
{
	INT32				_width = 0;
	INT32				_logIdx = 0;		//Logical Index

	SCRIPT_ANALYSIS*	_psa = nullptr;

	wchar_t*			_pChars = nullptr;
	int					_nChars = 0;

	WORD*				_pGlyphs = nullptr;
	int					_nGlyphs = 0;

	int*				_piAdvances = nullptr;
	GOFFSET*			_pGlyphOffsets = nullptr;
	SCRIPT_LOGATTR*		_pLogAttrs = nullptr;
	SCRIPT_VISATTR*		_pVisAttrs = nullptr;
	int*				_piJustify = nullptr;
	WORD*				_pwLogClust = nullptr;

	HFONT				_hFont = nullptr;

	ABC*				_pABC = nullptr;
	ABC*				_pRenderABC = nullptr;

	INT32				_lineIdx = 0;
	i3UIFont* _pFont = nullptr;
};

struct SCRIPTSENTENCE
{
	INT32 _begin = 0;
	INT32 _end = 0;
	bool _newfont = false;
	DWORD _code = 0;
};

struct SERVICECOUNTRY
{
	int _index = 0;
	int _extendSize = 0;
	char _countryName[MAX_PATH] = { 0 };
	char _fontName[MAX_PATH] = { 0 };
	char _fontFileName[MAX_PATH] = { 0 };
};

enum I3UITEXT_ALIGNMENT
{
	I3UITA_LEFT = 0,
	I3UITA_CENTER,
	I3UITA_RIGHT,
	I3UITA_JUSTIFICATION,

	I3UITA_COUNT
};

class I3_EXPORT_FRAMEWORK i3UIUniscribe : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3UIUniscribe, i3ElementBase);

friend i3UIText;

private:
	//Uniscribe related
	SCRIPT_ITEM*		m_pItems = nullptr;

	INT32*				m_piVisToLog = nullptr;		//visual & logical order of items
	INT32*				m_piLogToVis = nullptr;

	struct SCRIPTBUFFER {
		WORD*	_pwGlyphs = nullptr;
		WORD*	_pwLogClust = nullptr;
		SCRIPT_VISATTR*	_psva = nullptr;
		int*	_piAdvance = nullptr;
		GOFFSET* _pGlyphOffset = nullptr;
		SCRIPT_LOGATTR* _psla = nullptr;

		~SCRIPTBUFFER()
		{
			I3MEM_SAFE_FREE(_pwGlyphs);
			I3MEM_SAFE_FREE(_pwLogClust);
			I3MEM_SAFE_FREE(_psva);
			I3MEM_SAFE_FREE(_piAdvance);
			I3MEM_SAFE_FREE(_pGlyphOffset);
			I3MEM_SAFE_FREE(_psla);
		}
	};

	SCRIPTBUFFER *		m_pBuf = nullptr;

	//Other member variables
	bool				m_bFontFallback = false;	
	HFONT				m_hFallbackFont = nullptr;

	HDC					m_hDC = nullptr;
	HFONT				m_hCurFont = nullptr;

	bool				m_bWordBreak = false;

	i3Texture *			m_pTransTex = nullptr;

	i3Texture*			m_pCacheTex = nullptr;

protected:

	SCRIPTBUFFER *		getBuffer( INT32 idx)		{ return &m_pBuf[ idx]; }
	INT32		_ScriptItemize( const wchar_t* pszText, INT32 nTextLen);
	INT32		_ScriptLayOut( INT32 count);
	INT32		_ScriptShape( const wchar_t* pText, INT32 nChars, SCRIPT_ANALYSIS* psa, WORD** ppwOutGlyphs, WORD** ppwOutLogClust, SCRIPT_VISATTR** ppsva);
	void		_ScriptPlace( WORD* pwGlyphs, INT32 nGlyphs, SCRIPT_VISATTR* psva, SCRIPT_ANALYSIS* psa, INT32** ppiOutAdvance, GOFFSET** ppOutGlyphOffset, ABC* pABC);
	void		_ScriptJustify( void);
	void		_ScriptBreak( const wchar_t* pText, int nChars, SCRIPT_ANALYSIS* psa, SCRIPT_LOGATTR* pOutLogAttr);
	void		_ScriptTextOut( const i3::vector<SCRIPT_RUN*>& RunList, i3::vector<i3UIGlyph*>& GlyphList, INT32 nFontHeight);
	void		_ScriptTextOut( i3UIFont* pUIFont, const i3::vector<SCRIPT_RUN*>& RunList, INT32 nFontHeight);

	INT32		_ScriptCacheGetHeight( void);
	INT32		_GetDefTabWidth( void);
	INT32		_ScriptGetGlyphABCWidth( WORD wGlyph, ABC* pABC = nullptr, HDC hdc = nullptr, SCRIPT_CACHE* psc = nullptr);

	void		_ReadyBuffers( INT32 count);
	void		_FreeBackingStorages( void);

	//Runs		
	SCRIPT_RUN*		__CreateRun( const SCRIPT_RUN* pRun, HANDLE hFont);
	void			__ReleaseRun( SCRIPT_RUN* pRun);

	INT32			_GetRunWidth( const SCRIPT_RUN* pRun);

	INT32		_GetClusterABCWidth( const SCRIPT_RUN* pRun, const i3::vector<i3UIGlyph*>& GlyphList, INT32 nBegin, INT32 nEnd, ABC* abc, ABC* pOutABC);
	INT32		_GetClusterABCWidthLTR( const SCRIPT_RUN* pRun, const i3::vector<i3UIGlyph*>& GlyphList, INT32 nBegin, INT32 nEnd, ABC* abc, ABC* pOutABC);
	
	void		_CreateUITextSimple( STR16 pStr, i3UIText* pUIText);

	i3UILetter* _FindSoftBreak( i3UILetter* pFirst, i3UILetter* pCur);
	
	void		_BuildFontImage( i3UIGlyph * pGlyph, i3Texture * pTex);

public:
	i3UIUniscribe();
	virtual ~i3UIUniscribe();

	void		Init( void);

	bool		CreateUIText(i3UIText* pUIText, const wchar_t* pStr, INT32 nStrLen, i3::vector<SCRIPT_RUN*>& OutRunList,
		i3::vector<i3UIGlyph*>& OutGlyphList, i3UIFont* pNewFont = nullptr, int nPreRoofIndex = 0);
	void		ReleaseRuns( i3::vector<SCRIPT_RUN*>& RunList);
	
	INT32		GenClusterInfo( const SCRIPT_RUN* pRun, const i3::vector<i3UIGlyph*>& GlyphList, INT32 idx, INT32 nTotalGlyphCount, 
								SPRITE_INFO* pOutInfo, ABC* pOutABC, INT32* pOutClusterWidth);

	void		ReleaseRun( SCRIPT_RUN* pRun)	{	__ReleaseRun( pRun);	}
	i3UIFont*	FontFallback(SCRIPTSENTENCE* pScriptSentence, i3UIFont* pOriFont);
};

extern i3UIUniscribe* g_pUniscribe;