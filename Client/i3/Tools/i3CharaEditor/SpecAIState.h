#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSpecAIState dialog

class CSpecAIState : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecAIState)

protected:
	AI *			m_pCurAI;

protected:
	void			_UpdateAnimComboList(void);
	void			OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	CSpecAIState(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecAIState();

// Dialog Data
	enum { IDD = IDD_SPEC_AISTATE };

	virtual void	SetObject( i3ElementBase * pObj);
	void UpdateAIStatePanel();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_NameCtrl;
	CComboBox m_AnimCtrl;
	CEdit m_IDCtrl;
	CEdit m_RunScriptCtrl;
	CEdit m_FinishScriptCtrl;
	CButton m_TermAnimCtrl;
	CButton m_TermTimeCtrl;
	CEdit m_TimeLengthCtrl;
	CComboBox m_NextStateCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnCancel();
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CEdit m_BlendTimeCtrl;
	CComboBox m_BlendTypeCtrl;
	CButton m_LoopCtrl;
	afx_msg void OnCbnSelchangeCbAnim();
	afx_msg void OnCbnSelchangeCbBlendtype();
	afx_msg void OnCbnSelchangeCbNextstate();
	afx_msg void OnBnClickedChkAnimLoop();
	afx_msg void OnBnClickedChkTermTime();
	afx_msg void OnBnClickedChkTermWithanim();
	CRichEditCtrl m_SrcTextCtrl;
	CRichEditCtrl m_CommentTextCtrl;
	afx_msg void OnEnKillfocusEdBlend();
	afx_msg void OnEnKillfocusEdCommenttext();
	afx_msg void OnEnKillfocusEdFinishScript();
	afx_msg void OnEnKillfocusEdId();
	afx_msg void OnEnKillfocusEdName();
	afx_msg void OnEnKillfocusEdRunScript();
	afx_msg void OnEnKillfocusEdSrctext();
	afx_msg void OnEnKillfocusEdTimelength();
	CComboBox m_FilterCtrl;
	CButton m_DisplayPathCtrl;
	afx_msg void OnCbnKillfocusCbFilter();
	afx_msg void OnBnClickedChkViewpath();
	CEdit m_ED_AnimName;
	CButton m_CHK_OnlyName;
	afx_msg void OnBnClickedChkOnlyname();
	afx_msg void OnEnKillfocusEdAnimname();
	afx_msg void OnSize(UINT nType, int cx, int cy);

};
