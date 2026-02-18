#pragma once

enum RSC_CATEGORY
{
	RSC_CHARA = 0,
	RSC_SCENEGRAPH,
	RSC_LOD,
	RSC_ANIMATION,
	RSC_SOUND,
	RSC_TEXTURE,
	RSC_AI,

	RSC_TYPECOUNT
};

class cDlgWeaponEditor;
class cWeaponResourceList : public CTreeCtrl
	{
	DECLARE_DYNAMIC(cWeaponResourceList)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	HTREEITEM	m_hRootItem[ RSC_TYPECOUNT];

	cDlgWeaponEditor* m_pParent;
public:
	cWeaponResourceList();
	virtual ~cWeaponResourceList(){}

	void Create();
	void SetEventWnd(CWnd* pWnd) { m_pParent = (cDlgWeaponEditor*)pWnd; }

	void RemoveAllRes( i3ClassMeta * pType = NULL);
	void UpdateAllRes(void);

	void		UpdateAllAI(void);
	void		UpdateChara(void);
	void		UpdateRes( HTREEITEM& hItem, i3ResourceObject * pRes);
	void		_Rec_UpdateAI( HTREEITEM hParentItem, i3GameNode * pNode);
	void		GetInfoString( char * pszBuf, i3ResourceObject * pRes);

	HTREEITEM	AddRes( HTREEITEM hParentItem, i3ResourceObject * pRes);
	HTREEITEM	getRootItem( i3ResourceObject * pRes);
	HTREEITEM	FindRes( i3ResourceObject * pRes);

	void OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	void OnDelete( HTREEITEM hItem, i3ElementBase * pObj);
	void	RemoveRes( i3ResourceObject * pRes);
	void	RemoveRes( HTREEITEM hItem);
	void	UpdateAllLOD(void);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void	OnSetFocus(CWnd* pOldWnd);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void	OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
};
