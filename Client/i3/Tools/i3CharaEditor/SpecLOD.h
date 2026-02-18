#pragma once
#include "afxwin.h"


// CSpecLOD dialog

class CSpecLOD : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecLOD)

protected:
	void		_Update(void);

public:
	CSpecLOD(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecLOD();

	virtual void	SetObject( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_SPEC_LOD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_SgCtrl;
	CEdit m_TriCountCtrl;
	CEdit m_BoneCountCtrl;
	afx_msg void OnCbnSelchangeCbSg();
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	CButton m_CHK_DisableShadowCast;
	afx_msg void OnBnClickedChkCastShadow();
};
