#ifndef __I3UI_LISTVIEW_BOX_H__
#define __I3UI_LISTVIEW_BOX_H__

#include "i3UIControl.h"

class i3UIListView_Item;
class i3UIListView_Cell;
class i3UIScrollBar;
class LVITEM_DATA;
class i3UIListView_Header;

struct TEMPLATE_INFO : i3::class_common_pool<TEMPLATE_INFO> 
{
	INT32						m_nCol = 0;
	i3::vector<i3::rc_string>	m_TemplateList;
};

//text type 인 경우 이 정보를 바탕으로 align이 결정된다
//List를 통해 관리되며, Column의 개수만큼 생성된다. 
struct I3UI_LV_TEXTINFO
{
	INT32	m_nColumn = 0;
	
	INT32	m_nTextAlignX = 0;
	INT32	m_nTextAlignY = 0;
};

class i3UITemplate_LVSep;

typedef void(*I3UI_USER_FILTER_CALLBACK)(i3UIControl * pThis);

typedef INT32 I3UI_LVBOX_SCROLL_STYLE;
#define I3UI_LVBOX_SCROLL_STYLE_AUTO	0x00000000
#define I3UI_LVBOX_SCROLL_STYLE_SHOW	0x00000001
#define I3UI_LVBOX_SCROLL_STYLE_HIDE	0x00000002

class I3_EXPORT_FRAMEWORK i3UIListView_Box : public i3UIControl
{
	I3_EXPORT_CLASS_DEFINE( i3UIListView_Box, i3UIControl);

protected:
	// ================= variables =================
	INT32		m_nColumnCount = 1;
	bool		m_bClipItem = false;
	bool		m_bSingleSelect = false;
	bool		m_bFillWithEmptyItems = false;
	bool		m_bDisableEmptyItems = false;
	bool		m_bNoSelection = false;
	bool		m_bResizeHeader = false;
	bool		m_bApplyChangeData = false;
	
	i3::pack::RECT		m_rtMargin;
	VEC2D		m_vItemGap;

	// Item
	i3UIListView_Item*	m_pLoadedItem = nullptr;
	i3UIListView_Item*	m_pBoundItem = nullptr;
	i3::vector<i3UIListView_Item*>	m_ItemList;					//i3UIListView_Item*
	INT32				m_nRowCount = 0;
	INT32				m_nVisibleItemCount = 0;
	REAL32				m_BoundHeight = 0.0f;

	i3::vector<TEMPLATE_INFO*> m_TemplateInfo;				//TEMPLATE_INFO*

	bool				m_bIsMouseOver = false;
	INT32				m_nIdx = 0;
	INT32				m_nItemIdx = 0;
	INT32				m_nDataIdx = 0;

	i3UIListView_Cell*	m_pCurOverCell = nullptr;
	

	// Header
	i3UIListView_Header*	m_pHeader = nullptr;

	// Data
	i3::vector<LVITEM_DATA*> m_DataList;						//LVITEM_DATA*
	INT32			m_nFirstVisibleDataIndex = 0;

	// ScrollBar
	i3UIScrollBar*	m_pScrollBar = nullptr;
	REAL32			m_fWidthScrollBar = 20.0f;
	REAL32			m_fScrollBarMargin = 0.0f;
	
	I3UI_LVBOX_SCROLL_STYLE m_nScrollStyle = I3UI_LVBOX_SCROLL_STYLE_AUTO;
#if defined( I3_DEBUG)
	i3::vector<i3::rc_string> m_ScrollStyleList;
#endif // I3_DEBUG

	bool					m_bDefaultTemplateState = false;
	i3UITemplate_LVSep*     m_pTSepDefault = nullptr;

	bool			m_bPreviewMode = true;

	i3::vector<i3UIControl*> m_pImageBoxList;

	// ================= functions =================
	void			_OnScroll(void);
	
	void			_RecalcItemCount( void);
	void			_RepositionItems( void);
	void			_RepositionData( void);

	void			_PlaceScrollBar( void);
	void			_HideScrollBar( void);
	
	i3UIListView_Item*		_NewObjectFrom( i3UIListView_Item* pSrcItem);
	void					_ReleaseObject( i3UIListView_Item* pItem);

	virtual void	_UpdateShapeState(void) override;

	INT32			_getNumberOfItemsNeeded( void);
	void			_AdjustItemCountTo( INT32 nCount);
	void			_CheckResizeHeader( void);

	virtual void	_ForceVisible( bool bFlag) override;

	/** \brief Sprite Layer를 구분하는 것을 하나의 그룹으로 묶어준다.
		\desc 이것을 해주어야 Rendering 횟수를 줄일 수 있다. */
	void			_ReOrderingForLayer(void);

public:
	i3UIListView_Box();
	virtual ~i3UIListView_Box(); 

	void			ApplyChanges( void);
	void			Redraw( void);

	virtual bool	OnPreviewMode( void) override;
	virtual void	OnEditMode( void) override;

	void			getItemBox( i3Rect* pRt);
	void			getFullRect( i3UIListView_Cell* pCellToFind, i3Rect* pRt);

	INT32			getCellCount( void);
	INT32			getCellCount( INT32 nIdx);
	
	void			SetTemplateInfo( INT32 nCol, const char* pszName);
	void			SwapTemplateInfo( INT32 nCol, i3::vector<i3::rc_string>& List);

	void			GetTemplateName( INT32 nCol, char* pBuf, INT32 nBufSize);
	void			GetTemplateName( INT32 nCol, INT32 nTemplateIdx, char* pBuf, INT32 nBufSize);
	const i3::rc_string* GetTemplateName( INT32 nCol, INT32 nTemplateIdx) const;
	const TEMPLATE_INFO* GetTemplateInfo( INT32 nCol) const;

	bool			getIsMouseOverValidItem( void) { return m_bIsMouseOver; }
	void			setIsMouseOverValidItem( bool bOver) { m_bIsMouseOver = bOver; }
	
	INT32			GetTemplateCount( INT32 nCol);

	// ================= Header =================
	i3UIListView_Header*	getHeader( void)			{	return m_pHeader;	}
	void					BindHeader( i3UIListView_Header* pLVHeader);
	void					UnBindHeader( void);
	void					SetHeaderText(const i3::vector<i3::rc_wstring>& StrList);
protected:
	void			_ReposHeader( void);
	
public:
	bool			isHeaderResizable( void)		{	return m_bResizeHeader;	}

	// ================= ScrollBar =================
	bool			isScrollBarEnable( void);
	REAL32			getScrollBarWidth( void)	{	return m_fWidthScrollBar;	}
	i3UIScrollBar*	getScrollBar( void)			{	return m_pScrollBar;	}

	// ================= Item =================
	i3UIListView_Item*		getBoundItem( void)		{ return m_pBoundItem;	}
	void					BindItem( i3UIListView_Item* pLVItem);
	void					UnBindItem( void);
protected:
	void			_ReposBoundItem( void);
public:
	
	INT32			GetItemIndexByPos( POINT point);
	INT32			GetDataIndexByPos( POINT point);
	INT32			GetItemIndex(const i3UIListView_Item & item);
	INT32			getItemCount( void)		{	return m_nVisibleItemCount;	}
	i3::vector<i3UIListView_Item*>&	getItemList( void)		{	return m_ItemList;	}

	// ================= Cell ==================
	void					setCurrentOverCell( i3UIListView_Cell* pCell);
	i3UIListView_Cell*		getCurrentOverCell( void) { return m_pCurOverCell; }

	void			OnItemSizeChanged( REAL32 cx, REAL32 cy);
	void			OnCellTypeChanged( i3UIListView_Item* pFrom);
	void			OnCellSizeChanged( i3UIListView_Item* pFrom);
	void			OnCellCountChanged( INT32 n);
	void			OnSeperatorChanged( REAL32 w);
	void			OnTextChanged(i3UIListView_Item* pFrom, int nIndex, wchar_t* wstrText, int nParam1, int nParam2);

	INT32			GetCellIndexByPos( INT32 itemIdx, POINT point);
	
	// ================= Data =================
	LVITEM_DATA*	NewItem( void);
	void			AddItem( LVITEM_DATA* pData);
	void			InsertItem( INT32 nIdx, LVITEM_DATA* pData);
	void			DeleteItem( INT32 nIdx);
	INT32			getItemIndex() {return m_nItemIdx;}
	INT32			getDataIndex() {return m_nDataIdx;}
	void			setItemIndex(INT32 Idx) { m_nItemIdx = Idx;}
	void			setDataIndex(INT32 Idx) { m_nDataIdx = Idx;}
	bool			isCheckedData(INT32 Idx,INT32 CellIdx);

	//	다음은 모두 지우기 위해서 만든 함수..(11.05.31.수빈)
	void			DeleteAllItems();
	//

	void			SetItemCell( INT32 nItemIdx, INT32 nCellIdx, INT32 pData);
	void			SetItemCell( INT32 nItemIdx, INT32 nCellIdx, const char* pData);
	void			SetItemCell( INT32 nItemIdx, INT32 nCellIdx, const wchar_t* pData);
	void			SetItemCell( INT32 nItemIdx, INT32 nCellIdx, i3::vector<i3::string>& List);
	void			SetItemCellTextColor( INT32 nItemIdx, INT32 nCellIndex, COLOR * pCol, bool bRedraw = true);
	void			RestoreItemCellTextColor(INT32 nItemIdx, INT32 nCellIndex, bool bRedraw = true);

	INT32			getDataCount( void)		{	return (INT32)m_DataList.size();	}
	LVITEM_DATA*	getData( INT32 nIdx);
	void			getDataFromCell(INT32 nItemIdx, INT32 nCellIdx, i3::wstring& wstrOut);
	
	bool			isSelectedData( INT32 nDataIdx);
	void			UnSelectAllData( void);
	void			SelectData( INT32 nDataIdx, bool bSelect);
	void			ScrollToIndex( INT32 nDataIdx);
	
	bool			isItemSelected( i3UIListView_Item* pItem);

	void			SortByCellIndex( INT32 nBtnIdx, bool bAscending);

	i3::vector<LVITEM_DATA*>&	getDataList( void)		{	return m_DataList;	}

	i3UIControl*	getCellChild( INT32 nRow, INT32 nCol);
	i3UIControl*	getHeaderButton( INT32 nCol);			// --> 콘트롤 접근이 필요해서 추가.(11.07.06.수빈)

	// ================= Inherited members =================
	virtual bool	CreateBaseSceneObjects() override;

	virtual void	OnBindTemplate( void) override;
	virtual void	OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2) override;
	virtual void	OnEnabled( bool bEnable) override;
	virtual void	OnSize( REAL32 cx, REAL32 cy) override;
	virtual void	UpdateRenderObjects() override;

	/** \brief Sprite 할당
		\desc ListView는 하위 Item들에 대해서 미리 할당한다. */
	virtual void	prepareSprites(void) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	virtual void	OnDraw( i3UIRenderer * pRenderer) override;

	virtual void	OnMouseWheel( UINT32 nFlag, short zDelta, POINT point) override;

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

	virtual void	OnInitAfterLoad( void) override;

	virtual bool	isOnMouse( I3UI_OVERLAP_INFO * pInfo) override;

#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void	OnTDKPropertyChanged( const char* szName) override;
#endif

	static	void	RegisterLuaFunction(LuaState * pState);

	bool IsDummyLine() { return m_bDefaultTemplateState; }
	void SetDummyLine(bool bState);

protected:
	i3::vector<I3UI_LV_TEXTINFO*> m_TextInfo;		//I3UI_LV_TEXTINFO*
	LOGFONTA				m_lfDefault;
	
	
	COLOR					m_colDefault = { 255, 255, 255, 255 };
	COLOR					m_colText_MouseOver = { 255, 255, 255, 255 };
	COLOR					m_colText_Selected = { 255, 255, 255, 255 };

public:
	void				SetCellTextAlign( INT32 nColumn, INT32 nXAlign, INT32 nYAlign);

	INT32				getCellTextAlignX( INT32 nColumn);
	INT32				getCellTextAlignY( INT32 nColumn);
	LOGFONTA*			getDefaultFont( void)	{	return &m_lfDefault;	}

	
	COLOR*				getDefaultColor( void) {	return &m_colDefault;	}
	COLOR*				getMouseOverTextColor() { return &m_colText_MouseOver; }
	COLOR*				getSelectedTextColor() { return &m_colText_Selected; }

	void				UpdateAllComponents( void);

	void				OnItemTemplateChanged( i3UIListView_Item* pFrom);
	INT32				GetFirstVisibleIndex() const { return m_nFirstVisibleDataIndex; }

	void				SetCellCallback( INT32 nItemIdx, INT32 nCellIdx, void * pFunc);
	void				DeleteTemplateList(int nCol);

protected:
	I3UI_USER_FILTER_CALLBACK m_UserFilterFunc = nullptr;
public:
	void			setUserFilterFunc(I3UI_USER_FILTER_CALLBACK func) { m_UserFilterFunc = func;}

	////////////////////////////////////////////////////
	// Test Automation용 함수
	// i3UIControl의 함수 정의 참조
	//
	// ListView Cell
	public:
	virtual i3UIControl *	TA_FindCtrlByPath( const char * pszPathToken) override;
	virtual i3UIControl *	TA_ScanText(const wchar_t * pszText, bool bMustBeSame, bool bSubControl) override;
	virtual bool	TA_Select(const wchar_t * pszText, bool bMustBeSame) override;
};

#endif //__I3UI_LISTVIEW_BOX_H__
