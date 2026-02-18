#pragma once


// FileCheck_ProgressMD5View 뷰입니다.
#include "i3MFCListCtrl.h"
#include "FileCheck_MD5Thread.h"

struct DiffResult_PackScriptResult;
class FileCheck_ProgressBarView;
class FileCheck_MD5ModifiedView;

class FileCheck_ProgressMD5View : public CView
{
	DECLARE_DYNCREATE(FileCheck_ProgressMD5View)

public:
	FileCheck_ProgressBarView* GetProgressBarView() const { return m_barView; }
	FileCheck_MD5ModifiedView* GetModifiedView() const { return m_modView; }

	void		SetProgressBarView(FileCheck_ProgressBarView* barView) { m_barView = barView; }
	void		SetModifiedView(FileCheck_MD5ModifiedView* modView) { m_modView = modView; }

	void		InitCaption(const wchar_t* caption);
	void		InitList( const DiffResult_PackScriptResult& diff_result);

	const DiffResult_PackScriptResult*	GetDiffResult() const { return m_diff_result; }
	i3MFCListCtrl*						GetListCtrl() { return &m_listCtrl; }
	
	void		Start();
	void		Pause();
	void		Resume();
	void		Stop();

protected:
	FileCheck_ProgressMD5View();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~FileCheck_ProgressMD5View();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:

	FileCheck_ProgressBarView*			m_barView;
	FileCheck_MD5ModifiedView*			m_modView;

	const DiffResult_PackScriptResult*  m_diff_result;

	CStatic					m_title;
	i3MFCListCtrl			m_listCtrl;

	FileCheck_MD5Queue		m_md5_queue;
	FileCheck_MD5Thread_Callback*	m_cb;


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
};


