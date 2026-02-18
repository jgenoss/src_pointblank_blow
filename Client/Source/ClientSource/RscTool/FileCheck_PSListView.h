#pragma once

#include "i3MFCListCtrl.h"
class PackScriptFolder;
// FileCheck_PSListView 뷰입니다.
#include "i3Base/itl/variant/variant.h"

typedef i3::variant<class PackScriptFile*, class PackScriptFolder*> PSVariant;
typedef i3::shared_ptr<PSVariant>						PSVariantPtr;

class FileCheck_PSListView : public CView
{
	DECLARE_DYNCREATE(FileCheck_PSListView)
public:

	void					SetPSTreeView(class FileCheck_PSTreeView* pView) { m_pPSTreeView = pView; }
	FileCheck_PSTreeView*	GetPSTreeView() const { return m_pPSTreeView; }

	void	OpenMD5CheckFrameWnd();

	void	UpdateListCtrl(PackScriptFolder* pData);
	
	PSVariant*	GetListVariant(int index) const { return m_vec_varPS[index].get(); }

protected:
	FileCheck_PSListView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~FileCheck_PSListView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	FileCheck_PSTreeView* m_pPSTreeView;

	CStatic				m_title;
	i3MFCListCtrl		m_listCtrl;
	PackScriptFolder*	m_folder;


	i3::vector<PSVariantPtr>	m_vec_varPS;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
};


