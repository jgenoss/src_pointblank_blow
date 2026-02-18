#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "ColoredListCtrl.h"
// cDlgCreateNewWeapon 대화 상자입니다.

class cDlgCreateNewWeapon : public CDialog
{
	DECLARE_DYNAMIC(cDlgCreateNewWeapon)

public:
	cDlgCreateNewWeapon(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~cDlgCreateNewWeapon();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NEW_WEAPON };

	void InsertWeaponList(const std::vector<std::string >& vWeaponList);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString		m_edit_new_weapon_name;
	CComboBox	m_combo_weapon_list;
	CString		m_str_selected_weapon_name;
	CEdit		m_edit_search_weapon;

	CColoredListCtrl m_list_curr_weapons;

	typedef std::vector<CString > vStrListCon;
	typedef std::vector<vStrListCon > vvStrListCon;
	vvStrListCon m_vvStrList;

	typedef std::map<CString, CString> mStrListCon;
	mStrListCon m_mWeaponTargetResName;

	vStrListCon m_vNewResourceLOD;
	vStrListCon m_vNewAnimation;

	CString m_strCreatedWeaponName;

	virtual BOOL	OnInitDialog();
	LRESULT			OnKickIdle();
	bool			ReadFiles(std::vector<CString>& vFolders, std::vector<CString>& vFiles, CString strRootPath, CString strSubPath);
	bool			CheckResourceFiles(const CString& strSelectPath);
private:

	afx_msg void OnBnClickedButton1();
	afx_msg void OnCbnSelchangeComboWeaponList();
	afx_msg void OnCbnEditchangeComboWeaponList();
	afx_msg void OnNMThemeChangedComboWeaponList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnEditupdateComboWeaponList();
	afx_msg void OnEnChangeEditSearchWeapon();
	afx_msg void OnUpdate(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEvent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListCurrWeapons(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedButtonCreateNewWeapon();
	//CString m_edit_new_explain;
	CString m_edit_new_explain;
};
