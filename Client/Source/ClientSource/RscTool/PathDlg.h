#pragma once

#include "i3MFCButton.h"
// PathDlg 대화 상자입니다.

#include "PackPath.h"

class PathDlg : public CDialog
{
	DECLARE_DYNAMIC(PathDlg)

public:
	PathDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PathDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PATHDLG };
	
	const PackPathUnit*	GetResult() const { return &m_unit; }
	
	void		SetUnit(const PackPathUnit& unit);

	void		UpdateNationCombo_IfMediaPathChange(CEdit* edit, const i3::wstring& mediaPath);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	

private:
	CEdit		m_editPathName;
	CEdit		m_editPackScriptPath;
	CEdit		m_editMediaPath;
	CEdit		m_editQAPath;
	CEdit		m_editLivePath;

	i3MFCButton	m_button_PackScriptPath;
	i3MFCButton m_button_MediaPath;
	i3MFCButton m_button_QAPath;
	i3MFCButton m_button_LivePath;
	
	CComboBox	m_combo_Nation;


	PackPathUnit	m_unit;

	void		UpdateEditControls();
	void		UpdateNationCombo(const i3::wstring& mediaPath);

protected:
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnEnKillfocusEditMediaPath();
};
