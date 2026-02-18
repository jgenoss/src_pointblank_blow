#pragma once


// ItemDetailView 프레임입니다.


#include "PefItemPackScriptPane.h"

class ItemDetailView;
typedef i3::shared_ptr<struct AssocFileSet>	AssocFileSetPtr;




class ItemDetailFrame : public CFrameWndEx
{
	DECLARE_DYNCREATE(ItemDetailFrame)
public:
	
	
	void SetAssocFileSet(const AssocFileSetPtr& ptr);
	
	ItemDetailView*			GetDetailView() const { return m_pView; }
	PefItemPackScriptPane*	GetPefItemPackScriptPane() { return &m_script_pane; }
protected:
	ItemDetailFrame();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~ItemDetailFrame();

protected:
		
	ItemDetailView*			m_pView;
	PefItemPackScriptPane	m_script_pane;

	//  

	//

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


