#pragma once

//class CNavMesh;

class CPanelNavMeshEdit : public i3TDKDialogBase
{
	DECLARE_DYNAMIC( CPanelNavMeshEdit )

public:
	CPanelNavMeshEdit( CWnd* pParent = NULL );
	virtual ~CPanelNavMeshEdit();

public:
	enum 
	{
		IDD = IDD_NAVMESH_EDIT 
	};

public:
	void		LevelEditModeChanged( LEVEL_EDIT_MODE editMode );

public:
	CButton		m_radioVertex;
	CButton		m_radioPoly;

	CButton		m_btnEditAdd;
	CButton		m_btnEditDel;
	CButton		m_btnCreatePoly;
	CButton		m_btnClearNoUse;
	CButton		m_btnInvertNormal;
	CButton		m_btnCutPoly;

	CSliderCtrl		m_sliderVScale;
	CStatic			m_txtVScale;

	float		m_fMinVScale;
	float		m_fMaxVScale;

private:
	DECLARE_MESSAGE_MAP()

private:
	virtual void	DoDataExchange(CDataExchange* pDX) override;

	virtual BOOL	OnInitDialog() override;

	virtual void	OnOK() override {}
	virtual void	OnCancel() override {}

public:
	afx_msg void 	OnBnClickedNavmeshRadioVertex();
	afx_msg void 	OnBnClickedNavmeshRadioPoly();

	afx_msg void 	OnBnClickedNavmeshEditAdd();
	afx_msg void 	OnBnClickedNavmeshEditDel();
	afx_msg void 	OnBnClickedNavmeshEditCreatePoly();
	afx_msg void 	OnBnClickedNavmeshEditCutPoly();
	afx_msg void 	OnBnClickedNavmeshEditClearNoUse();
	afx_msg void 	OnBnClickedNavmeshEditInvertNormal();

	afx_msg void 	OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	
};
