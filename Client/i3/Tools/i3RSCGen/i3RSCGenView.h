// i3RSCGenView.h : iCi3RSCGenView 클래스의 인터페이스
//
#pragma once

#include <set>
#include "PaneTree.h"
#include "PaneSpec.h"
#include "PaneState.h"

#include "RSCNode.h"


class Ci3RSCGenView : public CView
{
protected: // serialization에서만 만들어집니다.
	Ci3RSCGenView();
	DECLARE_DYNCREATE(Ci3RSCGenView)

public:
	i3TDKSplitterWnd	m_wndSplit1;
	i3TDKSplitterWnd	m_wndSplit2;

	CPaneTree			m_PaneTree;
	CPaneSpec			m_PaneSpec;
	CPaneState			m_PaneState;

	void				OpenRSCFile( const char * pszFileName);
	void				AddFile( const char * szFileName);

protected:
	void				AddFileToMap( const char * );
	bool				CheckSameName( const char * );

	// 2011-04-27 김재진
	bool				m_bFirst;
	bool				m_bFile;
	char				m_szWorkDir[MAX_PATH];
	char				m_szWork[MAX_PATH];
	////
	RSCNode			*	m_pRoot;
	char				m_szFileName[MAX_PATH];

	i3::unordered_map< i3::fixed_string, RSCNode*> m_NodeMap;	// 중복노드가 없는 것으로 보임..

	RSCNode			*	m_pTypeRoot[I3RSC_TYPE_COUNT];

	void				_ClearAll();
	i3::vector<i3::rc_string>	m_ExceptList;

	RSCNode			*	_GetTypeRoot( INT32 nRscType);
	void				CreateRootScene();
	void				SetRoot( RSCNode * pRoot);

	RSCNode *			FindByRelPath( const char * pszRelPath);

	void				ScanAllNodes( RSCNode * pRoot);


	
	void				AddDirectory( const char * szFileName);
	BOOL				LookForFiles( const char * pszPath, RSCNode * pParent);

	
	void				_ReadRSCFile( i3FileStream * pStream);


	void				BuildRSCFile( const char * szRSCFileName);
	void				ExportRSCBin( char * pszFileName);
	void				_Rec_BuildRSCFile( i3IniParser * pParser, RSCNode * pNode);
	void				_Rec_AddExportList( RSCNode * pParent, i3::vector_multiset<RSCNode*, struct RSCNode_ExportComp>& ExportList);
	
	void				MakeExceptList( void);
	BOOL				IsAcceptable( char * pszStr);

	void				CreateNodeInfo( RSCNode * pNode, INT32 nStyle);
// 특성
public:
	Ci3RSCGenDoc* GetDocument() const;

// 작업
public:

// 재정의
	public:
	virtual void OnDraw(CDC* pDC) override;  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo) override;
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) override;
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) override;

// 구현
public:
	virtual ~Ci3RSCGenView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool m_UpStageFile;
// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	// 2011-04-27 김재진
	bool AutoWorkPath(const char* pszFileName, bool bPrsvDir = false);
	bool WorkingFolderYN(const char* pszNewFolder);
	/////
};

#ifndef _DEBUG  // i3RSCGenView.cpp의 디버그 버전
inline Ci3RSCGenDoc* Ci3RSCGenView::GetDocument() const
   { return reinterpret_cast<Ci3RSCGenDoc*>(m_pDocument); }
#endif

