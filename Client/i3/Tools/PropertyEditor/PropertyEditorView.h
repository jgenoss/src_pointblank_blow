// PropertyEditorView.h : iCPropertyEditorView 클래스의 인터페이스
//
#pragma once

class CPropertyEditorDoc;

#include "CSearchToolBar_I.h"


class CPropertyEditorView : public CView, 
	public CSearchComboBoxCallBack
{
protected: // serialization에서만 만들어집니다.
	CPropertyEditorView();
	DECLARE_DYNCREATE(CPropertyEditorView)

private:
	i3RegistrySet_Impl m_Registry;
	i3TDKRegistryCtrl_Impl	m_RegCtrl;
	CString m_CurFileName;
	bool m_bQuit;
	bool m_bSizeBarClicked;
	int m_nBarIndex;

	// 특성
public:
	CPropertyEditorDoc* GetDocument() const;


protected:
	BOOL	_OpenFile(LPCTSTR pFilePath);
	BOOL	_SaveRegistry(const char * pszFileName);
	BOOL	_SaveRegistryByUTF8(const char * pszFileName);
	
	void	_AutoMD5Process();
	bool	_DoTest(LPCTSTR pFilePath);
// 재정의
public:
	virtual void OnDraw(CDC* pDC) override;  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	void ReSizeWindow();

// CSearchComboBoxCallBack
public:
	virtual INT CSearchComboBoxCallBack_CollectCandidateByText(
		CandidateRst& rst, const char* text, DWORD opt=0);	
	virtual void CSearchComboBoxCallBack_OnSelectItem(void* arg);
	virtual void CSearchComboBoxCallBack_SelectClosestItem(const char* itemName, DWORD opt=0);

public:

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo) override;
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) override;
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) override;

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
	virtual void OnInitialUpdate() override;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpen(const std::string path);
	afx_msg void OnFileTest();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileNew();
	afx_msg void OnExportXml();
	afx_msg void OnImportXml();
	afx_msg void OnExportString();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnOption();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	

protected:
	CPoint m_CurrentPt;
	void ReSizeControl(POINT pt, int BarIndex);
public:
	afx_msg void OnLVFontDownSize();
	afx_msg void OnLvFontUpSize();
	afx_msg void OnUpdateUI_LVFontDownSize(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUI_LVFontUpSize(CCmdUI *pCmdUI);

	afx_msg void OnAdjustKeyRegDataCntToNationCount();
	afx_msg void OnUpdateAdjustPefDataCntToNationCountMenuItem(CCmdUI* pCmdUI);
};

#ifndef _DEBUG  // PropertyEditorView.cpp의 디버그 버전
inline CPropertyEditorDoc* CPropertyEditorView::GetDocument() const
   { return reinterpret_cast<CPropertyEditorDoc*>(m_pDocument); }
#endif

