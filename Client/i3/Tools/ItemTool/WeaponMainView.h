#pragma once

class cDlgWeaponEditor;
class cWeaponMainView : public CStatic
{
	DECLARE_DYNAMIC(cWeaponMainView)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
public:
	cWeaponMainView();
	virtual ~cWeaponMainView(){}

	void SetEventWnd(CWnd* pWnd) { m_pParent = (cDlgWeaponEditor*)pWnd; }
	cDlgWeaponEditor* m_pParent;

	DECLARE_MESSAGE_MAP()

	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};