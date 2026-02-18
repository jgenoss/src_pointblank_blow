#pragma once


// FileCheck_ProgressBarView 뷰입니다.
#include "i3MFCButton.h"
#include "TextProgressCtrl.h"

class FileCheck_ProgressMD5View;
struct DiffResult_PackScriptResult;

class FileCheck_ProgressBarView : public CView
{
	DECLARE_DYNCREATE(FileCheck_ProgressBarView)
public:

	FileCheck_ProgressMD5View* GetProgressMD5View() const { return m_MD5View; }
	void		SetProgressMD5View(FileCheck_ProgressMD5View* MD5View) { m_MD5View = MD5View; }
	
	void		InitProgressBar(const DiffResult_PackScriptResult& diff_result);

	void		StartProgress(size_t num_list);
	void		PauseProgress(size_t next_index);
	void		ResumeProgress(size_t next_index);
	void		StopProgress(size_t next_index);
	void		EndProgress();

	void		SetProgressPos(size_t next_index);

	void		SetRedrawProgressCtrl(BOOL bTrue);


protected:
	FileCheck_ProgressBarView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~FileCheck_ProgressBarView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	FileCheck_ProgressMD5View* m_MD5View;

	CTextProgressCtrl		m_progress_ctrl;

	i3MFCButton				m_button_Start;
	i3MFCButton				m_button_PauseResume;
	i3MFCButton				m_button_Stop;
	
	i3::wstring				m_str_status;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


