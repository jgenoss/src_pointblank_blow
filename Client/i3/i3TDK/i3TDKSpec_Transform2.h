#pragma once


// i3TDKSpec_Transform2 dialog

class i3TDKSpec_Transform2 : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Transform2)
protected:
	i3Transform2 *		m_pTrans = nullptr;

public:
	i3TDKSpec_Transform2(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Transform2::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Transform2();

	virtual void SetObject( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_TRANSFORM2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedTdkBtnShapeset();
};
