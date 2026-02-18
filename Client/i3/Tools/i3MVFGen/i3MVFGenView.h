// i3MVFGenView.h : iCi3MVFGenView 클래스의 인터페이스
//


#pragma once

#include "PaneTree.h"
#include "PaneStatus.h"
#include "PaneSpec.h"

#include "MVFNode.h"
#include "MVFGenThread.h"

class Ci3MVFGenView : public CView
{
protected: // serialization에서만 만들어집니다.
	Ci3MVFGenView();
	DECLARE_DYNCREATE(Ci3MVFGenView)

public:
	i3TDKSplitterWnd	m_Splitter0;
	i3TDKSplitterWnd	m_Splitter1; 

	CPaneStatus		m_PaneStatus;
	CPaneTree		m_PaneTree;
	CPaneSpec		m_PaneSpec;

	MVFNode *		m_pRoot;

	i3StringList	m_ExceptList;
	i3StringList	m_NameList;

	MVFGenThread *	m_pThread;
	char			m_szFileName[ MAX_PATH];

	void	MakeExceptList(void);
	BOOL	IsAcceptable( char * pszStr);
	BOOL	LookForFiles( const char * pszPath, MVFNode * pParent);
	BOOL	ImportFromList( const char * szListFileName );
	BOOL	ExporttoList( MVFNode * pParent, INT32	*nCount, i3FileStream * pStream );
	void	SetRoot( MVFNode * pRoot);
	void	AddFile( const char * pszPath);
	void	AddFileFromList( const char * pszPath);
	void	AddDirectory( const char * pszPath);

	void	OpenMVF(const char* szMVFFileName );
	void	BuildMVF( const char * pszPath);

	void		Rec_BuildNodeFromMVF( MVFNode * pParent, i3MergedVolumeFile * pFile, I3_MVF_ENTRY * pEntry);
	MVFNode * 	BuildNodeFromMVF( i3MergedVolumeFile * pFile);

	static void _LogProc( INT32 type, MVFNode * pNode, char * pszMsg, void * pUserData);

// 특성
public:
	Ci3MVFGenDoc* GetDocument() const;

// 작업
public:

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
	virtual ~Ci3MVFGenView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileImport();
	afx_msg void OnOptionsOptions();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnFileOpen();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnListExport();
	afx_msg void OnListImport();
protected:
public:
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // i3MVFGenView.cpp의 디버그 버전
inline Ci3MVFGenDoc* Ci3MVFGenView::GetDocument() const
   { return reinterpret_cast<Ci3MVFGenDoc*>(m_pDocument); }
#endif

