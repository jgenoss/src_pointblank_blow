#if !defined( __I3_UI_TEXT_MANAGER_H)
#define __I3_UI_TEXT_MANAGER_H

#include "Uniscribe/i3UIText2.h"
#include "Uniscribe/i3UIUniscribe.h"

#include "i3UIFontManager.h"

class I3_EXPORT_FRAMEWORK i3UITextManager : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3UITextManager, i3GfxResource);

	friend i3UIText;	
	friend i3UIUniscribe;

protected:
	i3UIFontManager *	m_pFontManager = nullptr;

	i3::vector<i3UILetter*>	m_LetterPageList;

	i3UILetter *	m_pFreeHead = nullptr;

	i3::vector<i3UIText*>	m_TextList;

	i3UIUniscribe*		m_pUniscribe = nullptr;

protected:
	void			_AddLetterPage(void);
	i3UILetter *	AllocLetter(void);
	void			FreeLetter( i3UILetter * pLetter);
	void			FreeLetterList( i3UILetter * pLetter);
	void			FreeAllLetterPage(void);

protected:
	void			AddText( i3UIText * pText);
	void			RemoveAllText(void);

public:
	i3UITextManager(void);
	virtual ~i3UITextManager(void);

	bool			Create( bool bLegacy = true);

	i3UIGlyph *		AddGlyph( i3UIFont * pFont, WORD wGlyph, bool* pOutExistOnVRAM)
	{
		return m_pFontManager->AddGlyph( pFont, wGlyph, pOutExistOnVRAM);
	}
	
	i3UIText *		CreateText( INT32 maxLength);
	void			FreeText( i3UIText * pText);
	void			ReparseAllText(void);
	INT32			getTextCount(void)						{ return (INT32)m_TextList.size(); }
	i3UIText *		getText( INT32 idx)						{ return m_TextList[idx]; }

	// Font System
	i3UIFontManager *	getFontManager(void)				{ return m_pFontManager; }
	i3Texture *		FindTexture( INT32 x, INT32 y, INT32 w, INT32 h)				{ return m_pFontManager->FindTexture( x, y, w, h); }
	INT32			FindFontTextureIndex( REAL32 x, REAL32 y, REAL32 w, REAL32 h)	{ return m_pFontManager->FindTextureIndex( x, y, w, h); }
	i3Texture *		GetFontTexture( INT32 idx)				{ return m_pFontManager->GetTexture( idx); }
	
	i3UIFont *		GetFont(	const char * pszName, 
								INT32 height, 
								bool bBold = false, 
								bool bItalic = false, 
								bool bUnderline = false, 
								bool bStrikeOut = false,
								I3UI_FONT_QUALITY quality = I3UI_FONT_QUALITY_ANTIALIASED)
	{
		return m_pFontManager->GetFont( pszName, height, bBold, bItalic, bUnderline, bStrikeOut, quality);
	}

	void			SetFontName( const char * pszName)
	{
		m_pFontManager->SetDefaultFontName( pszName);
	}

	void			SetFontCharaSet( UINT8 charaSet)
	{
		m_pFontManager->SetDefaultFontCharaSet( charaSet);
	}

	void			SetExternalFontSize( INT32 size)
	{
		m_pFontManager->setExternalFontSize( size);
	}

	virtual bool		Destroy( bool bLostDevice) override;
	virtual bool		isDestroyed(void) override { return true; }
	virtual bool		Revive( i3RenderContext * pCtx) override;
};

extern i3UITextManager *	g_pTextManager;

#endif
