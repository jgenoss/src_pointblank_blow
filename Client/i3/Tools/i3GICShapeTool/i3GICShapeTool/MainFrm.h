// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once
#include "CalendarBar.h"
#include "Resource.h"

#include "PanePreView.h"
#include "PaneTemplateView.h"
#include "PaneShapesView.h"
#include "PaneSpecificView.h"
#include "PaneLogView.h"
#include "PaneDesignView.h"
#include "Framework.h"


class CMainFrame : public CFrameWndEx
{
	
protected: // serialization에서만 만들어집니다.
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	CMainFrame();
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////
	//										Pane 
protected:
	CPaneTemplateView		m_Pane_TemplateView;
	CPaneShapesView			m_Pane_ShapesView;
	CPanePreView			m_Pane_PreView;
	CPaneSpecificView		m_Pane_SpecificView;
	CPaneLogView			m_Pane_LogView;
	CPaneDesignView			m_Pane_DesignView;

	CMFCToolBarImages		m_Pane_Images;

protected:
	void		_CreatePane( void);

public:
	CPaneSpecificView *		getSpecificView( void)				{ return &m_Pane_SpecificView; }
	CPaneDesignView *		getDesignView( void)				{ return &m_Pane_DesignView; }

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CMFCShellTreeCtrl m_wndTree;

	////////////////////////////////////////////////////////////////////////////////////////////
	//									Ribbon
protected:
	void		_CreateRibbonView( CMFCRibbonCategory * pCategory);
	void		_CreateRibbonFont( CMFCRibbonCategory * pCategory);
	void		_CreateRibbonShapeTool( CMFCRibbonCategory * pCategory);
	void		_CreateRibbonMode( CMFCRibbonCategory * pCategory);

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileNew();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileOpen();
	afx_msg void OnFilePrint();

	// grid
	afx_msg void OnView_Grid();
	afx_msg void OnUpdateView_Grid( CCmdUI* pCmdUI);
	afx_msg void OnView_GridSize();
	afx_msg void OnView_LinkSnap();
	afx_msg void OnUpdateView_LinkSnap( CCmdUI * pCmdUI);
	afx_msg void OnView_GridSnap();
	afx_msg void OnUpdateView_GridSnap( CCmdUI * pCmdUI);


	// Align
	afx_msg void OnTools_AlignLeft();
	afx_msg void OnUpdateTools_AlignLeft( CCmdUI * pCmdUI);

	afx_msg void OnTools_AlignCenter();
	afx_msg void OnUpdateTools_AlignCenter( CCmdUI * pCmdUI);

	afx_msg void OnTools_AlignRight();
	afx_msg void OnUpdateTools_AlignRight( CCmdUI * pCmdUI);

	afx_msg void OnTools_AlignTop();
	afx_msg void OnUpdateTools_AlignTop( CCmdUI * pCmdUI);

	afx_msg void OnTools_AlignMiddle();
	afx_msg void OnUpdateTools_AlignMiddle( CCmdUI * pCmdUI);

	afx_msg void OnTools_AlignBottom();
	afx_msg void OnUpdateTools_AlignBottom( CCmdUI * pCmdUI);

	afx_msg void OnMode_Select();
	afx_msg void OnUpdateMode_Select( CCmdUI * pCmdUI);

	afx_msg void OnMode_Link();
	afx_msg void OnUpdateMode_Link( CCmdUI * pCmdUI);

	void InitializeRibbon();

	/////////////////////////////////////////////////////////////////////////////////////
	//									Framework
protected:
	CFramework *		m_pFramework;
	i3Viewer *			m_pViewer;
	i3Texture *			m_pRenderTarget;
	i3Texture *			m_pRenderBuffer;

	RECT				m_rectRenderView;

	char				m_szFilePath[ MAX_PATH];
	bool				m_bChangeTemplate;				///< Template를 편집한 경우 true, 저장되면 false

	void		_CreateFramework( void);

public:
	// for Shapes View
	void		MouseMove( CPoint point);
	void		LButtonUp( UINT nFlags, CPoint point, bool bInsertShape);

	// for Diagram view
	/** \brief Diagram View에 Shape을 생성해서 붙여준다.
		*/
	void		DropShape( i3GICShape * pShape, CPoint point);

	i3GICDiagram *	getDiagram( void)				{ return m_Pane_DesignView.m_pDiagramView->getDiagram(); }

	void		saveTemplate( const char * pszPath)	{ getDiagram()->SaveTemplate( pszPath); m_bChangeTemplate = false; }
	void		loadTemplate( const char * pszPath) { getDiagram()->LoadTemplate( pszPath); }

	///////////////////////////////////////////////////////////////////////////////////////
	// font
private:
	CMFCRibbonFontComboBox* m_pCBFont;
	CMFCRibbonComboBox*		m_pCBFontSize;
	CMFCRibbonColorButton *	m_pBtnFontColor;
	CMFCRibbonColorButton *	m_pBtnFontBkColor;

	BOOL			m_bUnderline;
	BOOL			m_bItalic;
	BOOL			m_bStretch;
	
	// message
	afx_msg void	OnFont_Style();
	afx_msg void	OnUpdateFont_Style( CCmdUI * pCmdUI);
	afx_msg void	OnFont_Size();
	afx_msg void	OnUpdateFont_Size( CCmdUI * pCmdUI);
	afx_msg void	OnFont_Underline();
	afx_msg void	OnUpdateFont_Underline( CCmdUI * pCmdUI);
	afx_msg void	OnFont_Italic();
	afx_msg void	OnUpdateFont_Italic( CCmdUI * pCmdUI);
	afx_msg void	OnFont_Color();
	afx_msg void	OnUpdateFont_Color( CCmdUI * pCmdUI);
	afx_msg void	OnFont_BkColor();
	afx_msg void	OnUpdateFont_BkColor( CCmdUI * pCmdUI);
	afx_msg void	OnFont_Stretch();
	afx_msg void	OnUpdateFont_Stretch( CCmdUI * pCmdUI);


	///////////////////////////////////////////////////////////////////////////////////////
	//								Colors
protected:
	CList<COLORREF,COLORREF> m_lstMainColors;
	CList<COLORREF,COLORREF> m_lstAdditionalColors;
	CList<COLORREF,COLORREF> m_lstStandardColors;
};


