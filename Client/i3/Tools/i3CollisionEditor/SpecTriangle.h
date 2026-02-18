#pragma once
#include "afxwin.h"


// CSpecTriangle dialog

class CSpecTriangle : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecTriangle)

public:
	CSpecTriangle(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecTriangle();

	virtual void	SetObject( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_SPEC_TRIANGLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_TerrainCtrl;
	CComboBox m_ReverbCtrl;
	CEdit m_ElasticityCtrl;
	afx_msg void OnCbnSelchangeCbTerrain();
	afx_msg void OnCbnSelchangeCbReverb();
	afx_msg void OnEnKillfocusEdElasticity();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
