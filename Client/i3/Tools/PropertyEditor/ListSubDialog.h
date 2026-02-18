#pragma once
#include "afxwin.h"

#include "resource.h"       // ¡÷ ±‚»£

#define MAX_CONTROL 4

// CListSubDialog dialog
class CListSubDialog : public CDialog
{
	DECLARE_DYNAMIC(CListSubDialog)

public:
	CListSubDialog(int nItem, int nSubItem, CWnd* pParent = NULL);   // standard constructor
	CListSubDialog(CWnd* pParent = NULL);
	virtual ~CListSubDialog();

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

// Dialog Data
	enum { IDD = IDD_EDIT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
	CWnd* m_pParent;
	CStatic m_StaticCtrl[MAX_CONTROL];
	CEdit m_EditCtrl[MAX_CONTROL];

	CString m_NameString;
	CString m_ValueString;
	CString m_SeparationString;
	CString m_ReturnString;
	CString m_TitleString;

	int m_nItem;
	int m_nSubItem;
	bool m_bESC;

	void SetTextEditCtrl( int Index, LPCTSTR name, LPCTSTR value );
	void HideEditCtrl();

	void MakeReturnText();

public:
	void InitDialog();
	void InitDialogText( LPCTSTR Name, LPCTSTR Value, LPCTSTR Separation = _T(", ") );
	void SetTitle(LPCTSTR Nation, LPCTSTR Id);

	CString GetTextEdit();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNcDestroy();
	afx_msg void OnDestroy();
};
