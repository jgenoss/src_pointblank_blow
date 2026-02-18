#pragma once


// FileCheckView 뷰입니다.

// 탭뷰로 View를 2개를 만들어야함..(일단 하나만 제작하고, 재활용 염두에 둘것..)
class FileCheck_PSTreeView;
class FileCheck_PSListView;
class FileCheck_RSCListView;

class PackScriptFile;
struct RSCFileResult;

class FileCheck_ResultView;
enum PackMode;

class FileCheckView : public CView
{
	DECLARE_DYNCREATE(FileCheckView)

protected:
	FileCheckView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~FileCheckView();
	
public:
	void		InitCaption(PackMode pm);
	
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	
	void		Update_OnActivateView();
	void		Update_AfterActivateView();

	void		Update_OnDeactivateView();

	void		Update_ResultView(const PackScriptFile* f);
	
	void		Update_ResultView(const RSCFileResult* r);


private:
	CSplitterWndEx	m_splitter;
	CSplitterWndEx  m_col0_splitter;

	FileCheck_PSTreeView*		m_PSTreeView;
	FileCheck_PSListView*		m_PSListView;
	FileCheck_RSCListView*		m_RSCListView;
	FileCheck_ResultView*		m_ResultView[2];

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnInitialUpdate();
};


