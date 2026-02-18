#pragma once

#include "i3MFCDialog.h"
#include "i3shelltreectrl.h"
#include "i3shelllistctrl.h"
#include "i3MFCButton.h"

namespace detail_folder_dialog { class CHolderView; }

class i3MFCFolderDialog : public i3MFCDialog
{
	DECLARE_DYNAMIC(i3MFCFolderDialog)
public:
	i3MFCFolderDialog();
	~i3MFCFolderDialog();

	BOOL CreateEx(CWnd* pParent, const CRect& rc);
	
	const i3::wstring&	GetResultPath() const { return m_strResult; }

	void SelectFolder( const i3::wstring& wstrPath);


private:

	CSplitterWnd m_cSplitter;
	CFrameWndEx* m_pFrame;
	i3ShellTreeCtrl	m_treectrl;
	i3ShellListCtrl m_listctrl;

	CComboBoxEx		m_combo_fullpath;
	i3MFCButton		m_buttonOK;
	i3MFCButton		m_buttonCancel;

	i3::wstring		m_strResult;

	void UpdateFolderComboText();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	virtual BOOL OnInitDialog();
	void OnOKButton();
	void OnCancelButton();
	

	friend class detail_folder_dialog::CHolderView;
};
