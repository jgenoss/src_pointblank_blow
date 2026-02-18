#pragma once


// ExecutePackFrameWnd 프레임입니다.
#include "i3MFCListCtrl_Custom.h"
#include "PackMode.h"

class ExecutePackFrameWnd : public CFrameWndEx
{
	DECLARE_DYNCREATE(ExecutePackFrameWnd)
protected:
	ExecutePackFrameWnd();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~ExecutePackFrameWnd();

public:
	i3MFCListCtrl_Custom*	GetPackFileListCtrl() { return &m_packfile_list; }
	i3MFCListCtrl*			GetPackContentsListCtrl() { return &m_packcontents_list; }

	void					ClearPackList();


	void					AddRscTask(const i3::rc_string& rel_rsc_path, 
		const i3::vector<i3::rc_string>& rscnode_pathlist);

	void					AddPackScriptFile(const i3::wstring& strFilePath, PackMode mode);
	void					AddNormalFileList(const i3::vector<i3::rc_string>& resFileList, 
		const i3::string& item_name, PackMode mode);


private:

	i3MFCListCtrl_Custom	m_packfile_list;		// 칼람왼쪽 팩파일이름, 오른쪽 프로그레스바..
	i3MFCListCtrl			m_packcontents_list;	

	PackQueue_ProgressCallback* m_progress_callback;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


