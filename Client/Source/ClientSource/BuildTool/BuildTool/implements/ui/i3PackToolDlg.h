#pragma once

#include "CustomUGEditBrowse.h"
#include "CustomStatic.h"
#include "CustomButton.h"

class ThreadPool;

namespace ui
{
	class i3PackToolTree;

	class i3PackToolDlg : public CDialogEx
	{
		DECLARE_DYNAMIC(i3PackToolDlg)

	public:
		i3PackToolDlg(CWnd* pParent = NULL);
		virtual ~i3PackToolDlg();

		void OnClose();

		void SetPackSrcWorkdir(const std::string& strSrcWorkdir)	{ m_strPackSrcWorkdir = strSrcWorkdir; }
		const std::string& GetPackSrcWorkdir() const				{ return m_strPackSrcWorkdir; }

		void SetPackTgtPath(const std::string& strTgtPath)			{ m_strPackTgtPath = strTgtPath; }
		const std::string& GetPackTgtPath() const					{ return m_strPackTgtPath; }

		void SetBrowsePackPathText(const std::wstring& wstrPath);
		void SetBrowseSourcePathText(const std::wstring& wstrPath);

		i3PackToolTree* GetTreeCtrl() const	{ return m_treeCtrl.get(); }
	
	private:
		CBrush m_bkGndBrush;
		std::unique_ptr<i3PackToolTree> m_treeCtrl;

		CustomStatic m_textSourcePath;
		CustomUGEditBrowseCtrl m_browseSourcePath;

		CustomStatic m_textPackPath;
		CustomUGEditBrowseCtrl m_browsePackPath;
		CFont m_font;

		std::string m_strPackSrcWorkdir;
		std::string m_strPackTgtPath;

		CustomButton m_btnNewPack;
		CustomButton m_btnSavePack;
		CustomButton m_btnAddFiles;
		CustomButton m_btnAddFolder;

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);
		virtual BOOL OnInitDialog();

		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	};
}