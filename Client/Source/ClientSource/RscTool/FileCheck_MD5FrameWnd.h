#pragma once


// FileCheck_MD5FrameWnd 프레임입니다.
typedef i3::shared_ptr<struct PackScriptResult>			PackScriptResultPtr;
typedef i3::shared_ptr<struct RSCFileResult>			RSCFileResultPtr;

#include "DiffResult_PackScriptResult.h"

class FileCheck_MD5FrameWnd : public CFrameWndEx
{
	DECLARE_DYNCREATE(FileCheck_MD5FrameWnd)
public:

	void SetInfo(const i3::vector<PackScriptResultPtr>& psr_list);
	void SetInfo(const i3::vector<RSCFileResultPtr>& rsc_list);


protected:
	FileCheck_MD5FrameWnd();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~FileCheck_MD5FrameWnd();

protected:
	DECLARE_MESSAGE_MAP()

private:
	
	void SetInfoImpl();

	CSplitterWndEx m_mainSplitter;
	CSplitterWndEx m_rowSplitter[2];			// 2는 2개의 칼람을 의미..
	
	i3::vector<PackScriptResultPtr>	m_psr_list;		// 보관한다..(멀티쓰레드 대비..)
	i3::vector<RSCFileResultPtr>	m_rsc_list;		// 보관한다..(멀티쓰레드 대비..)	
	DiffResult_PackScriptResult  m_diff_result_LR;	//

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


