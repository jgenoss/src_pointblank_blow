#pragma once

#include "i3MFCListCtrl.h"
// FileCheck_MD5ModifiedView 뷰입니다.
#include "DiffResult_PackScriptResult.h"
//struct DiffResult_PackScriptResult;

class FileCheck_MD5ModifiedView : public CView
{
	DECLARE_DYNCREATE(FileCheck_MD5ModifiedView)
public:
	void		InitCaption(const wchar_t* caption);
	void		InitDiffResult(const DiffResult_PackScriptResult& diff_result);

	void		InsertModifiedResult(size_t idx);
	void		DeleteAllResult();

	i3MFCListCtrl*	GetListCtrl() { return &m_listCtrl; }

	void		SetRedrawListCtrl(BOOL bTrue);

protected:
	FileCheck_MD5ModifiedView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~FileCheck_MD5ModifiedView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:

	const DiffResult_PackScriptResult*  m_diff_result;
	
	LeftRightResult_PackScriptResult		m_mod_result;

	CStatic			m_title;
	i3MFCListCtrl	m_listCtrl;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:

};


