#ifndef __I3UI_LISTVIEW_HEADER_H__
#define __I3UI_LISTVIEW_HEADER_H__

#include "i3UIControl.h"
#include "i3UIFont.h"

class i3UIListView_Item;
class i3UIButton;
class i3UITemplate_Button;
class i3UITemplate_LVSep;
class i3UIListView_Box;

typedef UINT32	I3UI_LVH_STYLE;
#define I3UI_LVH_STYLE_AUTOSORT		0x00000001		// 기본 Sort

// Box의 Column Count가 1인경우에는 Header를 설정할 수 있다..

class I3_EXPORT_FRAMEWORK i3UIListView_Header : public i3UIControl
{
	I3_EXPORT_CLASS_DEFINE( i3UIListView_Header, i3UIControl );

	friend class i3UIListView_Box;

protected:
	// ================= variables =================
	i3::vector<i3UIButton*>			m_ButtonList;
	i3::vector<class i3UIListView_Sep*>	m_SepList;
	i3::vector<bool>				m_IsIndividualHeaderAlign;

	I3UI_LVH_STYLE	m_HeaderStyle = 0;
	INT32			m_nSelectedSep = -1;
	REAL32			m_cursorOffset = 0.0f;
	
	i3UITemplate_Button*		m_pTemplate_Button = nullptr;
	i3UITemplate_LVSep*			m_pTemplate_Sep = nullptr;
	i3UIListView_Box*			m_pBoundBox = nullptr;

	bool*						m_pbNextSortAscending = nullptr;	// 버튼별 소팅에 일관성 없어서 동적 배열로 바꿉니다.

	i3::vector<i3::rc_wstring>	m_HeaderTexts;	///<< Items
	INT32			m_defXAlign = 0;	///<< 기본 XAlign 값
	INT32			m_defYAlign = 0;	///<< 기본 YAlign 값
	COLOR			m_defColor = { 255, 255, 255, 255 };		///<< 기본 Font 색상
	
	I3UI_FONTINFO	m_defFontInfo;	///<< 기본 Font

	// ================= functions =================
	void			_AddNewButton( void);
	void			_AddNewSep( void);
	void			_AdjustButtonCount( INT32 nButtonCount);
	void			_AdjustButtonRect( i3UIListView_Item* pBoundItem, REAL32 fHeight);

	INT32			_GetButtonIndex( i3UIButton* pButton);

public:
	INT32			_isPointInSep( POINT point);
protected:
	void			_OnChangeSepPos( INT32 nIdx, POINT point);

	virtual void	_UpdateShapeState(void) override;
	bool			_isResizable( void);

	virtual void	_ForceVisible( bool bFlag) override;

public:
	bool			isHeaderStyle( I3UI_LVH_STYLE style)		{ return (bool)((m_HeaderStyle & style) == style); }
	void			setHeaderStyle( I3UI_LVH_STYLE style)		{ m_HeaderStyle = style; }
	void			addHeaderStyle( I3UI_LVH_STYLE style)		{ m_HeaderStyle |= style; }
	void			removeHeaderStyle( I3UI_LVH_STYLE style)	{ m_HeaderStyle &= ~style; }


public:
	i3UIListView_Header();
	virtual ~i3UIListView_Header();

	i3::vector<i3UIButton*>&	getButtonList( void)		{	return m_ButtonList;	}

	void			UpdateShape( i3UIListView_Item* pBoundItem, REAL32 fHeight );
	
	void			ResizeLastButton( void);

	void			SetHeaderText( const i3::vector<i3::rc_wstring>& StrList);
	void			SetHeaderColor( COLOR*	pCol);
	void			SetHeaderAlign( INT32 xAlign, INT32 yAlign);
	void			SetIndividualHeaderAlign( INT32 Cloumn, INT32 xAlign, INT32 yAlign);

	/** \brief 헤더 Font를 설정합니다.
		\param[in] pszFontName Font Name
		\param[in] size Font Size
		\param[in] quality I3UI_FONT_QUALITY
		\param[in] style I3UI_FONT_STYLE */
	void			SetHeaderFont( const char * pszFontName, INT32 size = 10,
		I3UI_FONT_QUALITY quality = I3UI_FONT_QUALITY_ANTIALIASED, I3UI_FONT_STYLE style = 0);

	void			SetHeaderFont( LOGFONTA * lf);

	void			SetHeaderFontName( const char * pszFontName);
	
	void			Reset( void);

	void				SetBoundBox( i3UIListView_Box* pBox); //	{	m_pBoundBox = pBox;	}
	i3UIListView_Box*	getBoundBox( void)							{	return m_pBoundBox;	}

	// ================= Inherited members =================
	virtual void	OnBindTemplate(void) override;
	virtual void	OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2) override;
	virtual void	OnEnabled( bool bEnable) override;
	virtual void	prepareSprites( void) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

#if defined( I3_DEBUG)
	bool			m_bAutoSort = false;
	LOGFONTA			m_lf;
	I3UI_FONT_QUALITY	m_FontQuality = I3UI_FONT_QUALITY_ANTIALIASED;

	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void	OnTDKPropertyChanged( const char* szName) override;
#endif
};

#endif //__I3UI_LISTVIEW_HEADER_H__
