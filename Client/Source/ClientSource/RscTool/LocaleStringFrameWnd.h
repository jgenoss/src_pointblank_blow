#pragma once


// LocaleStringFrameWnd 프레임입니다.
#include "i3MFCListCtrl.h"
typedef i3::shared_ptr<struct LocaleStringSet>	LocaleStringSetPtr;
struct PefTxtDiffInputInfo;

class LocaleStringFrameWnd : public CFrameWndEx
{
	DECLARE_DYNCREATE(LocaleStringFrameWnd)

public:
	void SetInfo(const PefTxtDiffInputInfo& input_info, const LocaleStringSetPtr& exp, const LocaleStringSetPtr& local);
	i3MFCListCtrl*	GetListCtrl() { return &m_listCtrl; }

protected:
	
	LocaleStringFrameWnd();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~LocaleStringFrameWnd();

private:
	i3MFCListCtrl	m_listCtrl;

	LocaleStringSetPtr	m_exp_string_set;
	LocaleStringSetPtr	m_local_string_set;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


