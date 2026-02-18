#pragma once


// PackScriptFileFrameWnd frame

#include "PackScriptFileTaskPane.h"
#include "PackScriptRealFileListPane.h"
#include "PackScriptForceCopyToQAPane.h"

#include "PackScriptFileExplorerTreeView.h"
#include "PackScriptFileExplorerListView.h"


class PackScriptFile;

class PackScriptFileFrameWnd : public CFrameWndEx
{
	DECLARE_DYNCREATE(PackScriptFileFrameWnd)
protected:
	PackScriptFileFrameWnd();           // protected constructor used by dynamic creation
	virtual ~PackScriptFileFrameWnd();
public:
	void	SetPackScriptFile(PackScriptFile* psf);
	

	class PackScriptFileExplorerListView*	GetExplorerListView() const { return m_explorer_listview; }

	PackScriptRealFileListPane*	GetPackScriptRealFileListPane()  { return &m_contentspane; } 
	PackScriptForceCopyToQAPane* GetPackScriptForceCopyToQAPane() { return &m_forcecopypane; }

private:
	PackScriptFile*				m_psf;
	
	i3::wstring					m_load_save_path;
	PackScriptFileTaskPane		m_taskpane;
	PackScriptRealFileListPane	m_contentspane;
	PackScriptForceCopyToQAPane m_forcecopypane;

	PackScriptFileExplorerListView*	m_explorer_listview;
	CSplitterWndEx		m_splitter;

protected:

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
};


