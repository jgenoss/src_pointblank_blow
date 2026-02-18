
// LogViewerView.h : CLogViewerView 클래스의 인터페이스
//

#pragma once

#include "ScriptWnd.h"
#include "LogFile.h"
#include "LocalLogNet.h"
class CLogViewerView : public CView
{
protected: // serialization에서만 만들어집니다.
	CLogViewerView();
	DECLARE_DYNCREATE(CLogViewerView)

// 특성입니다.
public:
	CLogViewerDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CLogViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

	CScriptWnd		m_ScriptWnd;
	//CLocalLogNet m_Control;
	//CLocalLogNet m_Trans;
	CLogFile	m_LogFile;
	i3::string	m_sLogName;
	i3::string	m_sWorkingDiretory;

	i3::string m_sData;
	i3::deque<i3::string> m_vecData;
public:
	

	void		Search(INT32 nBeginDate, INT32 nEndDate);
	void		SetWorkingDirectory(i3::string Dir);
	i3::string	GetWorkingDirectory() {return m_sWorkingDiretory;}
	i3::string	GetLogName() { return m_sLogName; }
	void		SetLogName(i3::string sName);

	void		SetDediHack(bool hack) { m_LogFile.SetDediHack( hack ); }
	bool		GetDediHack() {	return m_LogFile.GetDediHack(); }
};

#ifndef _DEBUG  // LogViewerView.cpp의 디버그 버전
inline CLogViewerDoc* CLogViewerView::GetDocument() const
   { return reinterpret_cast<CLogViewerDoc*>(m_pDocument); }
#endif

