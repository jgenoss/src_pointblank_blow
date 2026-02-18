#pragma once

#if defined( I3_DEBUG)
#include "resource.h"
#include "afxwin.h"

// CDlgVRAMState dialog

class CDlgVRAMState : public CDialog
{
	DECLARE_DYNAMIC(CDlgVRAMState)

protected:
	void		_Update(void);

public:
	CDlgVRAMState(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVRAMState();

// Dialog Data
	enum { IDD = IDD_VRAM_STATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	CEdit m_ED_TotalVRAM;
	CEdit m_ED_RTSize;
	CEdit m_ED_RTCount;
	CEdit m_ED_TexCount;
	CEdit m_ED_TexSize;
	CEdit m_ED_VBCount;
	CEdit m_ED_VBSize;
	CEdit m_ED_IBCount;
	CEdit m_ED_IBSize;
	CEdit m_ED_ShaderCount;
	CEdit m_ED_ShaderSize;
	CEdit m_ED_EST_Total;
	CEdit m_ED_EST_Gap;
	afx_msg void OnBnClickedBtnRefresh();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBnClickedBtnRt();
	afx_msg void OnBnClickedBtnTex();
	afx_msg void OnBnClickedBtnVb();
	afx_msg void OnBnClickedBtnIb();
};

#endif
