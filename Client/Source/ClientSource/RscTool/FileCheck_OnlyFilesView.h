#pragma once


// FileCheck_OnlyFilesView 뷰입니다.
#include "i3MFCListCtrl.h"

struct DiffResult_PackScriptResult;

class FileCheck_OnlyFilesView : public CView
{
	DECLARE_DYNCREATE(FileCheck_OnlyFilesView)

public:
	void		InitCaption(const wchar_t* caption);
	void		InitList( const DiffResult_PackScriptResult& diff_result, int l_r);

protected:
	FileCheck_OnlyFilesView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~FileCheck_OnlyFilesView();


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

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
};


