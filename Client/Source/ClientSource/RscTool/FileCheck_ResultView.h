#pragma once

//
// FileCheck_ResultView 뷰입니다.
// Left/ Right로 2개를 운용할 것..
//
#include "i3MFCListCtrl.h"

enum PathMode;
struct PackScriptResult;
struct RSCFileResult;

class FileCheck_ResultView : public CView
{
	DECLARE_DYNCREATE(FileCheck_ResultView)
public:
	void		InitPathMode(PathMode m, int l_r, const wchar_t* caption);
	void		UpdateList(const PackScriptResult* res);
	void		UpdateList(const RSCFileResult* res);

	void		ClearList();

protected:
	FileCheck_ResultView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~FileCheck_ResultView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:

	CStatic					m_title;
	i3MFCListCtrl			m_listCtrl;


	PathMode				m_path_mode;
	int						m_l_r;


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
};


