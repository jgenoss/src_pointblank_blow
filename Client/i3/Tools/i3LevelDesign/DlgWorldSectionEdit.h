#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgWorldSectionEdit dialog

class CDlgWorldSectionEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgWorldSectionEdit)
protected:
	enum CHK_STATE
	{
		UNCHECKED = 0,
		CHECKED = 1,
		NA = 2
	};

public:
	CDlgWorldSectionEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWorldSectionEdit();

// Dialog Data
	enum { IDD = IDD_DLG_WORLDSECTION };

protected:
	i3WorldSectionTable *		m_pTable;
	CImageList					m_ChkImageList;

	//Volatile, for UndoInfo
	i3LevelWorld		*		m_pWorld;

	void		_AssignValue(void);
	void		_RemoveAllSection(void);

	void		_SelectSection(void);
	void		_UpdateSection( INT32 idx, i3::pack::WORLD_SECTION_INFO * pInfo);

	void		_OnNewWorldSection(void);
	void		_OnDeleteWorldSection(void);

	void		_OnUpdateAllWorldSection(void);
	void		_OnUpdateWorldSection( INT32 idx);

	void		GetSelectedItems( i3::vector<i3::pack::WORLD_SECTION_INFO *>& List);

	void		SetShadowCheck( INT32 idx, CHK_STATE state);
	void		SetVisibilityCheck( INT32 idx, CHK_STATE state);
	CHK_STATE	GetShadowCheck( INT32 idx);
	CHK_STATE	GetVisibilityCheck( INT32 idx);

public:
	bool		Execute( i3WorldSectionTable * pTable);

	void		SetWorld(i3LevelWorld * pWorld)	{	m_pWorld = pWorld;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_SectionListCtrl;
	CListCtrl m_ShadowMaskListCtrl;
	CListCtrl m_VisibleMaskListCtrl;
	virtual BOOL OnInitDialog() override;
protected:
	bool						m_bEditted;
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnNMRclickSectionList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSectionList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusShadowMask(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusVisibilityMask(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkSectionList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickShadowMask(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickVisibilityMask(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSelallShadow();
	afx_msg void OnBnClickedBtnUnselallShadow();
	afx_msg void OnBnClickedBtnSelallVisible();
	afx_msg void OnBnClickedBtnUnselallVisible();
};
