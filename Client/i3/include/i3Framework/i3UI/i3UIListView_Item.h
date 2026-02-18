#ifndef __I3UI_LISTVIEW_ITEM_H__
#define __I3UI_LISTVIEW_ITEM_H__

#include "i3UIControl.h"
#include "i3UIListView_Defines.h"
#include "i3UIFont.h"

class i3UIControl_Dummy;
class i3UIListView_Sep;
class i3UIListView_Cell;
class i3UIListView_Box;
class i3UITemplate_LVCell;
class i3UITemplate_LVSep;
class LVITEM_DATA;
typedef void (*I3UI_LISTVIEW_CELL_CALLBACK)( i3UIListView_Cell * pCell, void * pData);

class I3_EXPORT_FRAMEWORK i3UIListView_Item : public i3UIControl	
{
	I3_EXPORT_CLASS_DEFINE( i3UIListView_Item, i3UIControl );

	friend class i3UIListView_Box;

#if defined( I3_COMPILER_VC)
#pragma pack(push,4)
#endif
	struct ALIGN4 CELLINFO
	{
		ECELLTYPE	m_Type = ECELLTYPE::ECELLTYPE_TEXT;
		REAL32		m_Width = 0;
		wchar_t	m_wstrText[MAX_PATH] = { 0 };
		INT32		m_nTemplateWidth = 0;
		INT32		m_nTemplateHeight = 0;
		INT32		m_HorizontalIndex = 0;
		INT32		m_VerticalIndex = 0;
	};

	struct ALIGN4 SEPERATORINFO
	{
		UINT8	m_bHide = 0;
		COLOR	m_Color;
	};
#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

protected:
	// ================= variables =================
	UINT32				m_CurShape;			// [initialize at constructor]
	
	INT32				m_nCellCount;		// == m_CellList.GetCount() [initialize at constructor]
	
	i3::vector<i3UIListView_Cell*>		m_CellList;			// i3UIListView_Cell*
	i3::vector<i3UIListView_Sep*>		m_SepList;			// i3UIListView_Sep*
	i3::vector<CELLINFO*> 				m_CellInfoList;		// CELLINFO*
	i3::vector<SEPERATORINFO*>			m_SeperatorInfoList;// SEPERATORINFO*

	REAL32				m_cursorOffset = 0.0f;

	i3UIListView_Box*	m_pBoundBox = nullptr;

	i3UITemplate_LVCell*		m_pTemplate_Cell = nullptr;
	i3UITemplate_LVSep*			m_pTemplate_Sep = nullptr;
	
	REAL32				m_fSepWidth = 5.0f;
	
	INT32				m_nSelectedSep = -1;

	i3UIListView_Cell*	m_pCurOverCell = nullptr;

	LVITEM_DATA*		m_currLVITEM_DATA = nullptr;			// 널값 체크를 비교적 신경써야할것임..

	// ================= functions =================
	virtual void	_UpdateShapeState(void) override;

	void			_AddNewCell( i3UIScene * pAncestor);
	void			_AddNewSep();

	INT32			_isPointInSep( POINT point);

	virtual void	_ForceVisible( bool bFlag) override;

	bool			_SetDataForUserControl( i3UIControl* pCellChild, const char * pszData);
	bool			_SetDataForButtonImageSet( i3UIControl* pCellChild, const char * pszData);
	bool			_SetDataForDoubleText( i3UIControl* pCellChild, const char * pszData);
	bool			_SetDataForStaticText( i3UIControl* pCellChild, const char * pszData);
	bool			_SetDataForButton( i3UIControl* pCellChild, const char * pszData);
	bool			_SetDataForComposed3Button( i3UIControl* pCellChild, const char * pszData);
	bool			_SetDataForCheckBox( i3UIControl* pCellChild, const char * pszData);
	bool			_SetDataForImageBox( i3UIControl* pCellChild, const char * pszData);
	bool			_SetDataForComboBox( i3UIControl* pCellChild, const char * pszData);		
	bool			_SetDataForControlDummy( i3UIControl* pCellChild, const i3::vector<i3::string>& DataList,  
		INT32 nParam1, INT32 nParam2, COLOR* pCol, I3UI_FONTINFO * pFontInfo);

	void		_SetCellData( INT32 nCellIdx, const i3::vector<i3::string>& DataList, INT32 nParam1, INT32 nParam2,
		COLOR* pCol, I3UI_FONTINFO * pFontInfo, I3UI_LISTVIEW_CELL_CALLBACK func = nullptr);

public:
	i3UIListView_Item(INT32 nCellCount = 0);
	virtual ~i3UIListView_Item(); 

	INT32		getCellCount( void)		{	return m_nCellCount;	}
	REAL32		getCellWidth( INT32 nIdx);
	i3UIListView_Cell*	getCell( INT32 idx);
	i3UIListView_Sep*	getSep( INT32 idx);

	INT32		getCellIndex(const i3UIListView_Cell & cell);
	
	REAL32		getSepWidth( void)			{	return m_fSepWidth;			}
	void		setSepWidth( REAL32 w)		{	m_fSepWidth = w;			}

	void		setDisabled( bool bDisable);

	i3::vector<i3UIListView_Cell*>&		getCellList( void)	{	return m_CellList;		}
	i3::vector<i3UIListView_Sep*>&		getSepList( void)	{	return m_SepList;		}

	void		ChangeCellCount( INT32 nCount, i3UIScene * pAncestor, bool bLoading = false);
	void		ChangeCellType( INT32 nIdx, const CELLINFO* pCellInfo);
	ECELLTYPE	getCellType( INT32 nIdx);

	INT32		getSelectedSep( void)		{ return m_nSelectedSep;	}

	void				SetBoundBox( i3UIListView_Box* pBox)	{ m_pBoundBox = pBox;	}
	i3UIListView_Box*	getBoundBox( void)				{ return m_pBoundBox;	}

	UINT8		getSeperatorHideState( INT32 idx)
	{
		I3_BOUNDCHK( idx, (INT32)m_SeperatorInfoList.size());
		return m_SeperatorInfoList[ idx]->m_bHide;
	}
	void		setSeperatorHideState( INT32 idx, UINT8 state)
	{
		I3_BOUNDCHK( idx, (INT32)m_SeperatorInfoList.size());
		m_SeperatorInfoList[ idx]->m_bHide = state;
	}
	
	void		CopyMembersFrom( i3UIListView_Item* pSrcItem, i3UIScene * pAncestor);
	
	
	void		SetData( LVITEM_DATA* pData);
	void		_UpdateTextColor( COLOR* clrDefault);
	void		CalcRect( i3Rect* rtBox, i3Rect* rtItem);

	void		UpdateShapes( void);

	void		UpdateCellPosAndSize( REAL32 cx, REAL32 cy, bool bUpdateChildRect = false);

	void		SetComboItems( INT32 nCellIdx, const char* pszItems);
	void		SetButtonName( INT32 nCellIdx, const char* pszName);

	// ================= EditMode =================
	void		_OnChangeSepPos( INT32 nIdx, POINT point);
	bool		OnMouseMove_EditMode( UINT32 nFlag, POINT point);
	void		OnLButtonDown_EditMode( UINT32 nFlag, POINT point);
	void		OnLButtonUp_EditMode( UINT32 nFlag, POINT point);
	

	// ================= Inherited members =================

	virtual void	OnBindTemplate(void) override;
	virtual void	OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2) override;
	virtual void	OnEnabled( bool bEnable) override;
	virtual void	prepareSprites( void) override;

	virtual void	OnSize( REAL32 cx, REAL32 cy) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonDblClk( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys) override;
	virtual void	OnLeaveMouse( void) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	OnInitAfterLoad() override;

	virtual bool	isOnMouse( I3UI_OVERLAP_INFO * pInfo) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void	OnTDKPropertyChanged( const char* szName) override;
#endif

	virtual void	Init( void) override;

	void			SetSizeNoNotify( REAL32 cx, REAL32 cy);
	void			Reset( i3UIListView_Item* pItem);
	void			TemplateAlign(i3UIListView_Cell* pCell, int AlignX, int AlignY);
	void			InitItem();

#if defined( I3_DEBUG)
public:
	INT32				m_ItemID = -1;
#endif

	i3::vector<i3::rc_wstring> m_ListBoxString;

	//////////////////////////////////////////
	// Test Automation
public:
	virtual void	TA_GetPathID(char * pszFullPath, INT32 limit) override;
	virtual i3UIControl *	TA_FindCtrlByPath(const char * pszPathToken) override;
	virtual i3UIControl *	TA_ScanText(const wchar_t * pszText, bool bMustBeSame, bool bSubControl) override;

	void			 UpdateCell();
};

#endif //__I3UI_LISTVIEW_ITEM_H__
