#pragma once
#include "afxcmn.h"


struct PACK_NAME
{
	CString packName;
};

// CPanelTreeLive 대화 상자입니다.

class CPanelTreeLive : public CDialog
{
	DECLARE_DYNAMIC(CPanelTreeLive)

protected:
	INT32			m_idxIconNormal;
	INT32			m_idxIconSelect;

	std::map<CString, CString> m_ExportFile;

	CFileFind		m_FileFinder;

	PACK_NAME  m_PackData[4000];
	INT32			m_iPackDataArr;

public:
	void			MakeTree(void);

protected:
	void			_Rec_SetPackTree(i3PackNode * pNode, CString pszPath, HTREEITEM hParent);
	HTREEITEM		FindParentItem(i3PackNode * pNode, HTREEITEM hParent = TVI_ROOT);

	void			FindCheckItem(HTREEITEM hItem);
	void			AddExportFile(HTREEITEM hItem);
	void			PreExport(const char * pszPath);

	void			_Rec_Export( i3FileStream * pFile, i3PackNode * pNode, const char * pszPath, CString strFileList);

	void			CheckChildItem(HTREEITEM hItem);
	void			UncheckChildItem(HTREEITEM hItem);

	void			OnExport(void);

public:
	CPanelTreeLive(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPanelTreeLive();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PANEL_TREE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_TreeCtrl;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClickPackTree2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkPackTree2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickPackTree2(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnTvnSelchangedPackTree2(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnTvnDeleteitemPackTree2(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
};
