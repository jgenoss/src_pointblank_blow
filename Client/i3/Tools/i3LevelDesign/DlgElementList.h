#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define		MSK_PRIM_BOX		0x00000001
#define		MSK_PRIM_SPHERE		0x00000002
#define		MSK_RESPAWN			0x00000004
#define		MSK_TRIGGER			0x00000008
#define		MSK_WORLD			0x00000010
#define		MSK_CHARA			0x00000020
#define		MSK_OBJECT			0x00000040
#define		MSK_LIGHT			0x00000080
#define		MSK_CAMERA			0x00000100
#define		MSK_PATH			0x00000200
#define		MSK_STATIC_OBJECT	0x00000400
#define		MSK_PRIM_CAPSULE	0x00000800

enum FILTER_SELECT_MODE
{
	FILTER_SELECT,
	FILTER_UNSELECT,
	FILTER_INVERT
};

// CDlgElementList dialog

class CDlgElementList : public CDialog
{
	DECLARE_DYNAMIC(CDlgElementList)
protected:

	void	_ReplaceControls( int cx, int cy);
	void	_AssignList(void);
	void	_UpdateElement( INT32 idx, i3LevelElement3D * pElm);

	void	GetSelectedItems( i3::vector<i3LevelElement3D*>& List);
	void	MakeNewName( char * pszDest, const char * pszFormat, INT32 idx);

	UINT32		GetFilter(void);
	bool	IsVisibleObject( i3LevelElement3D * pElm, UINT32 mask);

	void	SelectAllFilters( FILTER_SELECT_MODE mode);

public:
	CDlgElementList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgElementList();

// Dialog Data
	enum { IDD = IDD_DLG_ELEMENTLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_SearchNameCtrl;
	CListCtrl m_ElmListCtrl;
	CButton m_SelAllCtrl;
	CButton m_UnselAllCtrl;
	CButton m_SelInvertCtrl;
	CButton m_OKCtrl;
	CButton m_CancelCtrl;
	CStatic m_ListGroupCtrl;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnSelall();
	afx_msg void OnBnClickedBtnSelinvert();
	afx_msg void OnBnClickedBtnUnselall();
protected:
	virtual void OnOK() override;
public:
	afx_msg void OnEnChangeEdSearchname();
	afx_msg void OnBnClickedBtnRename();
	afx_msg void OnBnClickedChkVisible();
	afx_msg void OnBnClickedBtnVisibleSelall();
	afx_msg void OnBnClickedBtnVisibleUnselall();
	afx_msg void OnBnClickedBtnVisibleSelinvert();
};
