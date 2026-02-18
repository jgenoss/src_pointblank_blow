#pragma once

#include "i3MFCListCtrl.h"

// PackScriptForceCopyToQAPane

class PackScriptForceCopyToQAPane : public CDockablePane
{
	DECLARE_DYNAMIC(PackScriptForceCopyToQAPane)

public:
	PackScriptForceCopyToQAPane();
	virtual ~PackScriptForceCopyToQAPane();

	void				SetForceCopyFileList(const i3::vector<i3::string>& forcecopy_list);
	const i3::vector<i3::string>& GetForceCopyFileList() const { return m_forcecopy_list; }

private:
	i3MFCListCtrl		m_listCtrl;
	i3::vector<i3::string> m_forcecopy_list;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


