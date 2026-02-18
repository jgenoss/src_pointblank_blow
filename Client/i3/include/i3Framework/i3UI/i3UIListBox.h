#ifndef __I3UI_LISTBOX_H__
#define __I3UI_LISTBOX_H__

#include "i3UICaptionControl.h"		
#include "i3UIScrollBar.h"

#define I3UI_ITEMLIST_SIZE	512


//모든 string data는 WCHAR16으로 관리합니다..
class I3_EXPORT_FRAMEWORK i3UIListBox : public i3UICaptionControl	
{
	I3_EXPORT_CLASS_DEFINE( i3UIListBox, i3UICaptionControl);

protected:
	i3UIScrollBar*		m_pUIScrollBar = nullptr;
	REAL32				m_fWidthScrollBar = 20.0f;
	REAL32				m_fMarginUpScrollBar = 0.0f;
	REAL32				m_fMarginDownScrollBar = 0.0f;
	REAL32				m_fMarginRightScrollBar = 0.0f;

	INT32				m_TextPosY = 0;			//Scroll Position과 같은 값..

	i3::vector<i3::rc_wstring>	m_strListBox;	
	i3::vector<INT32>			m_strTextOffsetList;		// 구간계산을 위해, 실 사이즈보다 1많게 처리한다.

//	i3::vector<size_t>			m_SrcToRefinedMapper;		// 'Color'스트링 대응을 위한 매퍼..원본문자열에 칼라가 있는 경우 쓰게 된다..

	i3::rc_wstring		m_strDefaultItems;		// ';'로 구분된 item들의 문자열

	INT32				m_nCurSel = -1;
	INT32				m_nCurMouseOn = -1;
	
	POINT				m_CursorPos = { 0, 0 };
	
	I3COLOR				m_TextColor_MouseOver = { 255, 255, 255, 255 };
	I3COLOR				m_TextColor_Selected = { 255, 255, 255, 255 };
	DWORD				m_textclrTableIdx_MouseOver = DWORD(-1);
	DWORD				m_textclrTableIdx_Selected = DWORD(-1);

protected:
	virtual void	_UpdateShapeState(void) override;	// 현재 state에 따른 visible state를 update한다.

	void			_UpdateText( void);
	
	void			_UpdateScrollBarRect( void);		//Rect만 Update
	void			_UpdateScrollBarValue( void);		//Page Size등을 Update 후에 _UpdateScrollBarRect를 call

	// Click했을 때 Block처리 하기 위함
	INT32			_GetItemIndexByPos( POINT point);
	i3::pack::RECT			_GetItemRectByIndex( INT32 idx);
	
	void			_UpdateShapeSelected( INT32 idx);	//I3UI_LISTBOX_SHAPE_ITEM_SELECTED의 rect를 업데이트
	void			_UpdateShapeOnMouse( INT32 idx);	//I3UI_LISTBOX_SHAPE_ITEM_ONMOUSE의 rect를 업데이트

	void			_SetText( void);

	void			_InsertDefaultItemsToList( void);

	INT32			_AddItem( const WCHAR16 * pszItem);

	void			_UpdateSelMouseOverTextColor();

public:
	i3UIListBox();
	virtual ~i3UIListBox();

	/** \brief 아이템의 Text 길이에 맞춰 Size를 설정한다.
		\desc Width보다 큰경우는 늘려준다.
				Heigth보다 작은 경우는 줄여준다. */
	void			CalculateAutoSizeByItem( REAL32 rX, REAL32 rY);

	bool			Clear( void);
	INT32			AddItem( const WCHAR16* pszItem);
	INT32			InsertItem( INT32 nIdx, const WCHAR16* pszItem);
	bool			RemoveItem( const WCHAR16* pszItem);
	bool			DeleteItem( INT32 nIdx);
	bool			GetItem( INT32 nIdx, WCHAR16* pBuf, INT32 nBufSize);
	bool			SetItem( INT32 nIdx, const WCHAR16* pszItem);
	INT32			GetItemCount( void)		{	return (INT32)m_strListBox.size();	}

	INT32			GetCurSel( void)		{	return m_nCurSel;	}
	void			SetCurSel( INT32 nIdx);

	INT32			GetCurMouseOn(void)	const {
		return m_nCurMouseOn;
	}

	void			GetDefaultItems( WCHAR16* pBuf, INT32 nBufSize);
	void			SetDefaultItems( const i3::rc_wstring& strItems);

	I3COLOR			GetTextColor_Selected() const { return m_TextColor_Selected; }
	I3COLOR			GetTextColor_MouseOver() const { return m_TextColor_MouseOver; }
	void			SetTextColor_Selected(I3COLOR clr);
	void			SetTextColor_MouseOver(I3COLOR clr);


	virtual void	OnEnabled( bool bEnable) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnMouseMove( UINT32 nFlag, POINT point) override;
	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys) override;
	virtual void	OnMouseWheel( UINT32 nFlag, short zDelta, POINT point) override;
	virtual void	OnSize( REAL32 cx, REAL32 cy) override;
	virtual bool	OnKillFocus( i3UIControl * pByCtrl = nullptr) override;

	virtual i3UIControl*	OnSelect( void);
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

	virtual void	OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2) override;

	virtual void	OnBindTemplate(void) override;

	virtual void	UpdateRenderObjects() override;

	virtual bool	GetTextRect( i3::pack::RECT* pRect) override;

	//
	virtual void	Init( void) override;
	virtual void	OnInitAfterLoad() override;

	virtual bool	IsEllipsisEnable( void) override { return false; }

	virtual void	GripDraw( HDC hdc) override;

	void			SetScrollBarWidth( REAL32 width)			{ m_fWidthScrollBar = width; _UpdateScrollBarRect(); }
	REAL32			GetScrollBarWidth( void) const				{ return m_fWidthScrollBar; }

	void			SetScrollBarMarginUp( REAL32 margin)			{ m_fMarginUpScrollBar = margin; _UpdateScrollBarRect(); }
	REAL32			GetScrollBarMarginUp( void)	const				{ return m_fMarginUpScrollBar; }

	void			SetScrollBarMarginDown( REAL32 margin)			{ m_fMarginDownScrollBar = margin; _UpdateScrollBarRect(); }
	REAL32			GetScrollBarMarginDown( void) const				{ return m_fMarginDownScrollBar; }

	void			SetScrollBarMarginRight( REAL32 margin)			{ m_fMarginRightScrollBar = margin; _UpdateScrollBarRect(); }
	REAL32			GetScrollBarMarginRight( void) const			{ return m_fMarginRightScrollBar; }
	const i3::rc_wstring& GetDefaultItemString() const				{ return m_strDefaultItems; }
	void			ClearDefaultItemString()						{ m_strDefaultItems.clear(); }

#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void	OnTDKPropertyChanged( const char* szName) override;
	friend class i3UIComboBase;			// 콤보베이스에서 리스트박스의 프라이빗 멤버를 직접 접근하도록 처리해본다..
#endif

	static	void	RegisterLuaFunction(LuaState * pState);

	// Test Automation 관련
public:
	virtual i3UIControl *	TA_ScanText(const wchar_t * pszText, bool bMustBeSame, bool bSubControl);
	virtual bool	TA_Select(const wchar_t * pszText, bool bMustBeSame);
};

#endif //__I3UI_LISTBOX_H__