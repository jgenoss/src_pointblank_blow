#pragma once
#include "afxcmn.h"


// cDlgAIDriver 대화 상자입니다.
#include "ListAIDriver.h"
#include "AIDriverManager.h"
#include "afxwin.h"

class cDlgAIDriver : public CDialog
{
	DECLARE_DYNAMIC(cDlgAIDriver)

public:
	cDlgAIDriver(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~cDlgAIDriver();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_AIDRIVER_LIST };

	enum eSearchType
	{
		E_SEARCH_AIDRIVE,
		E_SEARCH_WEAPON,
		E_SEARCH_TABLE,
		MAX_SEARCH_TYPE,
	};

	void SetAIDriverData(mAIDriverCon* pAIDriverCon);
	void SetAIEnum(vAIEnumCon* pvAIEnum);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	mAIDriverCon	m_mAIDriverData;
	vAIEnumCon		m_vAIEnumData;
	eSearchType		m_eSearchType;
	CString			m_strCurrAIDriver;

	typedef std::vector<cAIDriverData*> vAIDriverCon;
	vAIDriverCon m_vAIDriver;

	typedef std::map<std::string, std::string > mStringListCon;
	mStringListCon m_mWeaponList;

	typedef std::map<std::string, std::vector<std::string> > mvecStringListCon;
	mvecStringListCon m_mvProcList;

	cListAIDriver m_list_aidriver;
	cListAIDriver m_list_aidriver_weapon;
	cListAIDriver m_list_aidriver_table;

public:
	virtual BOOL OnInitDialog();
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnBnClickedRadioAidriverSearch();
	afx_msg void OnBnClickedRadioWeaponSearch();
	afx_msg void OnBnClickedRadioTableSearch();

	CButton m_btn_aidriver_search;
	CButton m_btn_weapon_search;
	CButton m_btn_table_search;
	afx_msg void OnBnClickedAddWeapon();
};
