#pragma once

#include "mmc.h"
#include "i3TDKRegKeyCtrl.h"
#include "i3TDKRegDataCtrl.h"

// i3RegistryCtrl
//enum REGISTRYCTRL_SORT_TYPE {
//	SORT_TYPE_KEY,
//	SORT_TYPE_TIME,
//};

class CImageViewCtrl : public i3TDKImageViewCtrl
{
public:
	void SetScroll();
	virtual void	OnDraw( HDC dc);
};

class i3TDKRegistryCtrlImpl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKRegistryCtrlImpl)

public:
	i3TDKRegistryCtrlImpl();
	virtual ~i3TDKRegistryCtrlImpl();

protected:
	DWORD				m_dwStyle;

	CImageList *		m_pImageList;
	INT32				m_nIndexOfFirstTDKImage;	//TDK res에서 add된 icon의 첫번째 index

	i3TDKRegKeyCtrl		m_ListViewCtrl;
	i3TDKRegKeyCtrl		m_ListViewCtrl2;
	CImageViewCtrl		m_ImageView;
	i3TDKRegDataCtrl	m_ListDataCtrl;
	CTreeCtrl			m_TreeCtrl;
	i3RenderContext *	m_pContext;

	i3RegKey	*		m_pRoot;
	i3RegKey	*		m_pCurWeaponKey;
	HTREEITEM			m_pCurItem;
	INT32				m_nCurRow;

	HTREEITEM			m_SrcItem;
	i3RegData	*		m_SrcData;

	char				m_szTimeStamp[64];
	char				m_szTimeStampString[65];

public:
	void				SetRoot( i3RegKey * pRoot);
	i3RegKey *			GetRoot() { return m_pRoot; }

	void				SetTreeInfo( TVITEM * pInfo, i3RegKey * pKey);
	void				UpdateTree( HTREEITEM hItem, i3RegKey * pKey);
	HTREEITEM			AddTree( HTREEITEM hParent, i3RegKey * pKey);
	void				Rec_AddTree( HTREEITEM hParent, i3RegKey * pKey);
	void				UpdateAll( void);

	void				OnDelete( HTREEITEM hItem);
	void				Rec_SortTree( i3RegKey * pKey, REGISTRYCTRL_SORT_TYPE sort_type );
	void				Rec_CheckTimeStamp( i3RegKey* pKey );
	void				ProcessCommonRClickOnList(i3TDKRegKeyCtrl& listViewCtrl, NMHDR * pNMHDR, LRESULT * pResult);

public:
	BOOL				Create( DWORD dwStyle,const RECT &rect, CWnd * pParentWnd, UINT nID);

	//Menu Function
	void				_AddNewKey(HTREEITEM hParentItem);
	void				_AddTimeStampData( HTREEITEM hParentItem );		//+
	void				_GenerateTimeStamp();							//+
	void				_DeleteKey(HTREEITEM hItem);
	void				_RenameKey(HTREEITEM hItem);
	void				_CopyKey( HTREEITEM hItem);
	void				_PasteKey( HTREEITEM hItem);

	void				_CopyData( INT32 nRow);
	void				_PasteData();

	void				_FindKey();		

	void				_AddNewData(HTREEITEM hParentItem, INT32 nNewDataType);	
	void				_AddNewData(i3TDKRegKeyCtrl& listViewCtrl, HTREEITEM hParentItem, INT32 nNewDataType);
	void				_DeleteData(INT32 nRow);
	void				_RenameData(INT32 nRow);

	void				_CollapseNode(HTREEITEM item);	
	void				_ExpandNode(HTREEITEM item);

	void				_ImportKey( HTREEITEM item);
	void				_ExportKey( HTREEITEM item);

	void				_ExprotSqlCreate( HTREEITEM item ); 
	void				_ExprotSqlUpdate( HTREEITEM item ); 

	void				_Sort( REGISTRYCTRL_SORT_TYPE sorttype );

	void				OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

protected:
	void				UpdateTimeStamp();

	//Pef 파일안에 데이터 복사시에 만든 시간 갱신한다.
	//현재 시간으로 갱신한다.
	void _UpdateDataTimeStamp(i3RegKey* pKey);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRClickTree( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnNMClickListCtrl( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnNMRClickListCtrl( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnNMRClickListCtrl3( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTVNEndLabelEdit( NMHDR * pNMHDR, LRESULT * pResult);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


