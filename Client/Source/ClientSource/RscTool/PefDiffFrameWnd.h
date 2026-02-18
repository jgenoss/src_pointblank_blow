#pragma once


// PefDiffFrameWnd 프레임입니다.
#include "i3MFCListCtrl.h"

struct PefTxtDiffInputInfo;

class PefDiffFrameWnd : public CFrameWndEx
{
	DECLARE_DYNCREATE(PefDiffFrameWnd)
public:

	void SetInfo(const PefTxtDiffInputInfo& input_info, i3RegKey* exp_key, i3RegKey* local_key);
	i3MFCListCtrl*	GetListCtrl() { return &m_listCtrl; }

protected:
	PefDiffFrameWnd();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~PefDiffFrameWnd();
	


private:
	i3MFCListCtrl	m_listCtrl;
	
	i3RegKey*		m_exp_key;
	i3RegKey*		m_local_key;

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


