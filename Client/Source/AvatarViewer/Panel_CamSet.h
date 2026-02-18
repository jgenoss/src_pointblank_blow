#pragma once


// CPanel_CamSet

class CPanel_CamSet : public CDockablePane
{
	DECLARE_DYNAMIC(CPanel_CamSet)

public:
	VEC3D	m_vTranslation;
	VEC2D	m_vRotation;
	REAL32	m_rFOV;

	CMFCButton m_btnApply;

	CPanel_CamSet();
	virtual ~CPanel_CamSet();

protected:
	CMFCPropertyGridCtrl m_wndPropList;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);

	afx_msg void	OnUpdateBtn(CCmdUI *pCmdUI);
	afx_msg void	OnBtnClick(void);

	void		Init( void);
	void		GetPropItem( CMFCPropertyGridProperty* pProp );
	void		SetPropItem( CMFCPropertyGridProperty* pProp );

	void		SetCamera( void);

	void		ApplyCamera( void);
};


