#if !defined( __I3UI_CAPTION_CONTROL_H)
#define __I3UI_CAPTION_CONTROL_H

#include "i3UIControl.h"
#include "Uniscribe/i3UIText2.h"

class I3_EXPORT_FRAMEWORK i3UICaptionControl : public i3UIControl
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3UICaptionControl, i3UIControl);

//private:
protected:
	i3UIText *				m_pText = nullptr;

protected:
	i3UIFont *				m_pFont = nullptr;

	// 간단한 캡션. Derived Class에서 사용.
	I3UI_FONT_QUALITY		m_FontQuality = I3UI_FONT_QUALITY_ANTIALIASED;
	i3::rc_wstring			m_strCaption;


	/////////////////////////////////////////////////////////////////////
	// PropertyGrid 용 변수들입니다. 이외에는 사용 금지
	/////////////////////////////////////////////////////////////////////
	LOGFONTA					m_lf;	// [initialize at constructor]

	INT32					m_nLimit = 0;
	INT32					m_MaxTextCount = 0;
	bool					m_bLimitByteSize = false;

	// Margin
	INT32					m_SpaceX = 0;
	INT32					m_SpaceY = 0;

	// Caret update
	REAL32					m_rCaretTime = 0.0f;

	// Ellipsis
	bool					m_bUpdateEllipsis = false;
	bool					m_bVisibleEllipsis = false;
	REAL32					m_rEllipsisWidth = 0.0f;
	INT32					m_nEllipsisSpace = 0;
	
protected:
	UINT32			_CalcTextWeight(void);
	void			_CreateText( void);
	i3UIFont*		_GetUIFont( void);

	virtual void	_UpdateShapeState(void) override;	// 현재 state에 따른 visible state를 update한다.

	void			_UpdateTextCaret( REAL32 rDeltaSeconds);

	template<class StringType>
	void			_SetTextEllipsis( const StringType& wstrText);	// virtaul키워드 제거.엔진상에서는 이 함수가 더 파생되지 않음(13.05.31.수빈)
	void			_SwapTextEllipsis( i3::wstring& strTmp);
	
	void			_TextEllipsisInternal();

public:
	i3UICaptionControl(void);
	virtual ~i3UICaptionControl(void);

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual INT32	AdjustSpriteOrder( INT32 hSpriteEnd = -1, bool bRec = true) override;

	virtual	void	OnUpdateTextPos(void);

	void			SetMaxTextCount( INT32 cnt, bool bRecreate = true);
	INT32			getMaxTextCount(void)					{ return m_MaxTextCount; }

	INT32			getLetterLimit(void) const				{ return m_nLimit;			}
	bool			getLimitByteSize(void) const			{ return m_bLimitByteSize;	}

	void			SetBlock( INT32 nStart, INT32 nEnd);

	virtual void	SetCaption( const WCHAR16* pszCaption);

	virtual void	SetText( const i3::wliteral_range& wTextRng);
	virtual void	SetText( const i3::rc_wstring& wstrText);
	virtual void	SetText( const wchar_t* wstrText);
	virtual void	SetText( const i3::wstring& wstrText);
	virtual void	SetText( const i3::stack_wstring& wstrText);

	void			SetText( const char* pText);
	void			SetTextMB( const char* pText);
	const WCHAR16 *	getText(void) const;
	INT32				  getTextLength() const;
	const i3::const_wchar_range& getTextRange() const;

	void			SetTextRectSize(UINT32 nWidth, UINT32 nHeight);		

	virtual void	SetTextSpace( INT32 nX, INT32 nY);	
	INT32			getTextSpaceX(void)						{ return m_SpaceX; }
	INT32			getTextSpaceY(void)						{ return m_SpaceY; }

	virtual void	SetTextSpaceofLetters( INT32 nX, INT32 nY);	
	INT32			getTextSpaceofLettersX(void);
	INT32			getTextSpaceofLettersY(void);

	i3UIText	 *	getUIText(void)		const				{ return m_pText;	}
	
	virtual void	OnEnabled( bool bEnable) override;

	virtual void	OnSize( REAL32 cx, REAL32 cy) override;

	virtual void	OnQueryStrings( i3::vector<i3::rc_string>& StrList) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

	virtual void	UpdateRenderObjects() override;

	virtual bool	GetTextRect( i3::pack::RECT* pRect);
	virtual void	SetTextRect( i3::pack::RECT* pRect);

	virtual void	OnInitAfterLoad() override;
	virtual void	OnCompletedLoading(void) override;

	virtual void	OnDraw( i3UIRenderer * pRenderer) override;

	virtual bool	OnRevive( i3RenderContext * pCtx) override;
	virtual bool	OnLostDevice( bool bLostDevice) override;

	static	void	RegisterLuaFunction(LuaState * pState);

	//////////////////////////////////////////////////////////////////////////////
	// 이곳에 있는 함수들은 말 그대로("Text") UIText에 접근해주는 역할을 합니다.
	// UIText와 같은 변수들을 한번더 가지고 있을 필요가 없다고 생각합니다. 
	I3UI_TEXT_ALIGN_X	getTextAlignX(void);
	I3UI_TEXT_ALIGN_Y	getTextAlignY(void);
	virtual void		SetTextAlign( I3UI_TEXT_ALIGN_X AlignX, I3UI_TEXT_ALIGN_Y AlignY);

	virtual UINT32				getTextColor(void);
	virtual COLOR*				getTextColorPtr(void);
	virtual void				SetTextColor( UINT32 uColor);
	virtual void				SetTextColor( COLOR* pCol);
	virtual void				SetTextMargin( INT32 iMarginX, INT32 iMarginY);
	
	UINT32				getTextShadowColor(void);
	void				SetTextShadowColor( UINT32 uColor);

	I3UI_TEXT_SHADOW	getTextShadowType(void);
	void				SetTextShadowType( I3UI_TEXT_SHADOW shadow);

	UINT32				getTextStyle(void);
	void				setTextStyle( UINT32 style);
	void				addTextStyle( UINT32 style);
	void				removeTextStyle( UINT32 style);
	bool				isTextStyle( UINT32 style);

	bool				isTextMultiLine(void);
	void				setTextMultiLine( bool bFlag);

	bool				isTextReadOnly(void);
	void				setTextReadOnly( bool bFlag);

	bool				isTextAutoLineFeed(void);
	void				setTextAutoLineFeed( bool bFlag);

	INT32				getLinePitch(void);
	void				setLinePitch( INT32 pitch);

	bool				isTextLineFeedSplitWord( void);
	void				setTextLineFeedSplitWord( bool bFlag);

	void				SetTextCaretPos( INT32 idx);
	//////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////
	//Font 관련 함수들 (다른 Text 속성들과는 다르게 CaptionControl이 들고 있습니다.)
	void				SetFont( LOGFONTA* plf);
	void				SetFont( const char * pszFont, INT32 FontSize, UINT32 weight = FW_NORMAL, bool bRecreate = true);
	const char *		getFontName(void)							{ return m_lf.lfFaceName; }
	INT32				getFontSize(void)							{ return m_lf.lfHeight; }
	UINT32				getFontWeight(void)							{ return m_lf.lfWeight; }

	I3UI_FONT_QUALITY	getFontQuality(void)						{ return m_FontQuality;	}
	void				SetFontQuality( I3UI_FONT_QUALITY quality)	{ m_FontQuality = quality;	}
	const LOGFONTA*		GetLogFont(void) const						{ return &m_lf; }
	i3UIFont*			ObtainUIFont( void)							{ return _GetUIFont(); }
	//////////////////////////////////////////////////////////////////////////////

	virtual void		resetSprites( bool bRelease);
	virtual void		EnableCtrl( bool bTrue, bool bRecursive = false);


	/** \brief Ellipsis 여부
		\desc 기본적으로 true이다. */
	virtual bool IsEllipsisEnable( void)						{ return true; }
	bool		 IsVisibleEllipsis()	{ return m_bVisibleEllipsis; }

protected:	
	I3UI_TEXT_ALIGN_X		m_AlignX = I3UI_TEXT_ALIGN_X_LEFT;
	I3UI_TEXT_ALIGN_Y		m_AlignY = I3UI_TEXT_ALIGN_Y_TOP;

	I3UI_TEXT_SHADOW		m_TextShadowType = I3UI_TEXT_SHADOW_NONE;

	COLOR			m_TextColor;
	COLOR			m_TextShdowColor;

	INT32			m_SpaceofLettersX = 0;
	INT32			m_SpaceofLettersY = 0;

	INT32			m_nLinePitch = 0;
	bool			m_bLineFeed = false;
	bool			m_bLineFeedSplitWord = false;

	// PropertyGrid 관련..
#if defined( I3_DEBUG)

public:
	virtual void	GetTDKGridProperties(i3::vector<PROPERTYINFO*>& List) override;
	virtual void	OnTDKPropertyChanged( const char* szName) override;
	
	virtual void	GetMessagesForPropertyGrid( i3::vector<PROPERTYMSG*>& List) override;
#endif
private:

	template<class StringType>
	void			SetTextImpl(const StringType& str);

protected:	
	void			_SwapText( i3::wstring& strTmp);

	// Test Automation 관련
public:
	virtual i3UIControl *	TA_ScanText(const wchar_t * pszText, bool bMustBeSame, bool bSubControl);
};

#endif
