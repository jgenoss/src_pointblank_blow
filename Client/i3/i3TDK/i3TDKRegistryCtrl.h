#pragma once

#include "mmc.h"
#include "i3TDKRegKeyCtrl.h"
#include "i3TDKRegDataCtrl.h"

// i3RegistryCtrl
enum REGISTRYCTRL_SORT_TYPE {
	SORT_TYPE_KEY,
	SORT_TYPE_TIME,
};

class I3_EXPORT_TDK i3TDKRegistryCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKRegistryCtrl)

public:
	i3TDKRegistryCtrl() {}
	virtual ~i3TDKRegistryCtrl();

protected:
	DWORD				m_dwStyle = 0;

	CImageList *		m_pImageList = nullptr;
	INT32				m_nIndexOfFirstTDKImage = 0;	//TDK res에서 add된 icon의 첫번째 index

	i3TDKRegKeyCtrl		m_ListViewCtrl;
	i3TDKRegDataCtrl	m_ListDataCtrl;
	CTreeCtrl			m_TreeCtrl;

	i3RegKey	*		m_pRoot = nullptr;
	HTREEITEM			m_pCurItem = nullptr;
	INT32				m_nCurRow = -1;

	HTREEITEM			m_SrcItem = nullptr;
	i3RegData	*		m_SrcData = nullptr;

	TCHAR				m_szTimeStamp[64] = { 0 };
	TCHAR				m_szTimeStampString[65] = { 0 };

public:
	void				SetRoot( i3RegKey * pRoot);
	i3RegKey*			GetRoot() { return m_pRoot; }

	void				SetTreeInfo( TVITEM * pInfo, i3RegKey * pKey, const TCHAR* szKeyName);
	void				UpdateTree( HTREEITEM hItem, i3RegKey * pKey);
	HTREEITEM			AddTree( HTREEITEM hParent, i3RegKey * pKey);
	void				Rec_AddTree( HTREEITEM hParent, i3RegKey * pKey);
	void				UpdateAll( void);

	void				OnDelete( HTREEITEM hItem);
	void				Rec_SortTree( i3RegKey * pKey, REGISTRYCTRL_SORT_TYPE sort_type );
	void				Rec_CheckTimeStamp( i3RegKey* pKey );

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

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRClickTree( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnNMClickListCtrl( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnNMRClickListCtrl( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTVNEndLabelEdit( NMHDR * pNMHDR, LRESULT * pResult);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


