#pragma once


// ItemDetailView 뷰입니다.
#include "i3MFCListCtrl.h"

typedef i3::shared_ptr<struct AssocFileSet>	AssocFileSetPtr;

struct AssocResultFileBase;

class ItemDetailView : public CView
{
	DECLARE_DYNCREATE(ItemDetailView)

protected:
	ItemDetailView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~ItemDetailView();

public:

	void SetAssocFileSet(const AssocFileSetPtr& ptr);

	void SelectAssocFiles(const i3::vector<const AssocResultFileBase*>&  base_list);




public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	AssocFileSetPtr	m_AssocFileSet;
	i3MFCListCtrl	m_listCtrl;
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


