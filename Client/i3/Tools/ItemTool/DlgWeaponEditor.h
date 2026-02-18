#pragma once

#include "SplitDialog.h"
#include "WeaponMainView.h"
#include "WeaponAnimation.h"
#include "WeaponResourceList.h"
#include "WeaponProperty.h"
#include "DlgTimeTrack.h"
#include "DlgResourceControl.h"
#include "AIDriverManager.h"

class cRSCGen;
// cDlgWeaponEditor 대화 상자입니다.
class cDlgWeaponEditor : public CSplitDialog
{
	DECLARE_DYNAMIC(cDlgWeaponEditor)

public:
	cDlgWeaponEditor(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~cDlgWeaponEditor();

private:
// 대화 상자 데이터입니다.
	enum { IDD = IDD_WEAPON_EDIT };

	cWeaponResourceList	m_WeaponResList;
	cWeaponAnimation	m_WeaponAni;
	cWeaponResourceList	m_WeaponTrack;
	cWeaponMainView		m_WeaponRendering;
	cWeaponProperty		m_WeaponProp;

	CMyPane *m_PaneMain;
	CMyPane *m_PaneLeft;
	CMyPane *m_PaneRight;
	CMyPane *m_PaneBottom;

	cDlgTimeTrack* m_pDlgTimeTrack;
	cDlgResourceControl* m_pDlgResourceControl;

	bool	m_bOnceReset;
	bool	m_bIsVisible;

	cAIDriverManager*	m_pAIDriverMng;

	CString m_strFilePath;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	HWND	GetRenderHwnd() { return m_WeaponRendering.GetSafeHwnd(); }
	void	OnManualSetFocus();
	bool	OnOpenChara(const char* pszPath);
	void	OnCloseChara();
	void	OnManualOnSize(const int& cx, const int& cy);
	void	UpdateWeaponProp();
	void	OnLoadCharaEditor(const CString& strFolderName);
	void	UpdateDlgList();	//다이얼로그 리스트 갱신
	
	void	SetVisible(const bool& bVisible);
	bool	IsVisible() const { return m_bIsVisible; }

	CString GetFilePath() const { return m_strFilePath; }
	void	SetFilePath(const CString& strPath) { m_strFilePath = strPath; }

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMove(int x, int y);
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();

protected:
	virtual void OnOK();
	virtual void OnCancel();

public:
	afx_msg void OnBnClickedBtnOpenResdlg();
	afx_msg void OnBnClickedBtnOpenAidriver();
	afx_msg void OnBnClickedBtnCreate();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnAddAiEnum();
};
