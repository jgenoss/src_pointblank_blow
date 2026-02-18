#pragma once

// CResTreeCtrl
class CResTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CResTreeCtrl)

protected:
	DWORD			m_dwStyle;
	CImageList	*	m_pImageList;
	
	HTREEITEM		m_pCurItem;
public:
	CResTreeCtrl();
	virtual ~CResTreeCtrl();

public:
	void		InitImageList( void);

	HTREEITEM	AddRes( i3LevelRes * pRes);
	HTREEITEM	AddTreeItem( HTREEITEM hParent, i3LevelResRef * pResRef);
	HTREEITEM	Rec_AddTreeItem( HTREEITEM hParent, i3LevelResRef * pResRef);

	void		DeleteRes( i3LevelRes * pRes );

	void		DeleteTreeItem( HTREEITEM hItem);
	
	void		SetItemInfo( TVITEM * pInfo, i3LevelResRef * pResRef = NULL);

protected:
	DECLARE_MESSAGE_MAP()
};


