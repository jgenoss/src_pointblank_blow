#pragma once

//
// 탭콘트롤에 View를 삽입하기 편하게 하면서, 디폴트 CView에 넣기 편하게 커스터마이징... (CTabView 참고하면서, CView를 사용하지 않도록)
// (CMFCTabCtrl / CMFCTabBaseCtrl 내부 구현 코드는 코드량 쩔음....)
//

class i3MFCTabViewCtrl;

struct i3MFCTabViewCtrlCallback
{
	virtual void OnActivateView(i3MFCTabViewCtrl* ctrl, CView* oldView, CView* newView, 
		int oldTabNum, int newTabNum) {}
	virtual ~i3MFCTabViewCtrlCallback() {}
};


class i3MFCTabViewCtrl : public CMFCTabCtrl
{
	DECLARE_DYNCREATE(i3MFCTabViewCtrl)
public:
	i3MFCTabViewCtrl();
	virtual ~i3MFCTabViewCtrl();

	BOOL Create(CWnd* parent);
	void Embed_OnSize( int cx_view, int cy_view);
	int AddView(CRuntimeClass* pViewClass, const CString& strViewLabel, int iIndex = -1, CCreateContext* pContext = NULL);

	int FindTab(HWND hWndView) const;
	BOOL RemoveView(int iTabNum);
	BOOL SetActiveView(int iTabNum);
	CView* GetActiveView() const; // active view or NULL

	CView* GetView(int iTabNum) const;
	
	void	SetCallback(i3MFCTabViewCtrlCallback* cb) { m_cb = cb; }
	i3MFCTabViewCtrlCallback*	GetCallback() const { return m_cb; }

private:
	
	i3MFCTabViewCtrlCallback*		m_cb;
	int								m_iOldTab;
	CView*		GetParentView() const;
	CDocument*	GetDocument() const;
	virtual void FireChangeActiveTab(int nNewTab);
	virtual BOOL FireChangingActiveTab(int nNewTab);
	void		ChangeActiveTabImpl(int nNewTab);

	//{{AFX_MSG(CTabView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
	

};