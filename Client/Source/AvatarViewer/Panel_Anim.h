#pragma once


// CPanel_Anim

class CPanel_Anim : public CDockablePane
{
	DECLARE_DYNAMIC(CPanel_Anim)

public:
	CPanel_Anim();
	virtual ~CPanel_Anim();

public:
	CTreeCtrl m_TreeCtrl;

	typedef std::map<int, CString> mAnimCon;
	mAnimCon m_mAnimNames;

	int m_nCnt;
	void FindAnimationFiles(const CString& strRoot, const CString& strSub, HTREEITEM& hRoot);
	void LoadAnimation();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnPaint();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void	OnTvnSelchangedTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMClickTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult);
};


