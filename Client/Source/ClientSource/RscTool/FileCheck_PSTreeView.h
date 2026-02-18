#pragma once

//
// 일단은 PackScriptFolder 루트를 복사해서 쓸 것... 
// (체크용도이기 때문에 일단은 정확하고 시간을 길게 끄는 방식으로 한다.)
// (가능 하다면, observer 방식으로 전체 변경했어야 했다...) --> 이 부분은 리팩토링 단계이다.
// observer 방식인 경우에는 모든 UI의 통제는 옵저버의 콜백함수로 처리되어야함..
//

// FileCheck_PSTreeView 뷰입니다.
//

#include "i3MFCTreeCtrl.h"
class PackScriptFolder; 

class FileCheck_PSTreeView : public CView
{
	DECLARE_DYNCREATE(FileCheck_PSTreeView)
public:

	void		UpdatePackScriptFolder();
	void					SetPSListView(class FileCheck_PSListView* pView) { m_pPSListView = pView; }
	FileCheck_PSListView*	GetPSListView() const { return m_pPSListView; }
	
	void		SelectFolder(const PackScriptFolder* folder);

protected:
	FileCheck_PSTreeView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~FileCheck_PSTreeView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
private:
	CStatic						m_title;
	i3MFCTreeCtrl				m_treeCtrl;
	FileCheck_PSListView*		m_pPSListView;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
};


