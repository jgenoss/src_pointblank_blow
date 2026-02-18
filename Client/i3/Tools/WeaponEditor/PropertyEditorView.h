// PropertyEditorView.h : iCPropertyEditorView 클래스의 인터페이스
//
#pragma once

class CPropertyEditorView : public CView
{
protected: // serialization에서만 만들어집니다.
	CPropertyEditorView();
	DECLARE_DYNCREATE(CPropertyEditorView)

private:
	i3RegistrySet		m_Registry;
	i3TDKRegistryCtrlImpl	m_RegCtrl;
	CString				m_CurFileName;


	// 특성
public:
	CPropertyEditorDoc* GetDocument() const;


protected:
	BOOL	_OpenFile(LPCTSTR pFilePath);
	BOOL	_SaveRegistry(const char * pszFileName);
	BOOL	_SaveRegistryByUTF8(const char * pszFileName);



// 재정의
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);


protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현
public:
	virtual ~CPropertyEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileNew();
	afx_msg void OnExportXml();
	afx_msg void OnImportXml();
	afx_msg void OnExportString();
	afx_msg void OnImportString();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};

#ifndef _DEBUG  // PropertyEditorView.cpp의 디버그 버전
inline CPropertyEditorDoc* CPropertyEditorView::GetDocument() const
   { return reinterpret_cast<CPropertyEditorDoc*>(m_pDocument); }
#endif

