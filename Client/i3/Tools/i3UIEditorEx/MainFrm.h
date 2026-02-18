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

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "i3UIEFramework.h"

#include "Panel/UIPaneTimeLine.h"
#include "Panel/UIPaneWorkSpace.h"
#include "Panel/UIPaneScreen.h"
#include "Panel/UIPaneProperties.h"
#include "Panel/UIPaneTemplate.h"
#include "Panel/UIPaneScriptList.h"
#include "Panel/UIPaneOutput.h"
#include "Panel/UIPaneTemplateExplorer.h"

#include "FileWatcher.h"
#include "FileWatcher2.h"

#include "UIProject.h"

#include "ImageChecker.h"
#include "UIToolFileMgr.h"

#define		TEMPLATE_GALLERY_ICON_CX		64
#define		TEMPLATE_GALLERY_ICON_CY		58

class CScriptPane;
class CScriptWnd;

#define		ID_INITIAL_SCRIPT_PANE			100000

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	UINT32						m_nNationSel;

protected:  // control bar embedded members
	CMFCRibbonBar					m_wndRibbonBar;
	CMFCRibbonApplicationButton		m_MainButton;
	CMFCToolBarImages				m_PanelImages;
	CMFCRibbonStatusBar				m_wndStatusBar;
	CMFCRibbonStatusBarPane*		m_StatusBarPane1;		// on the left of Status Bar
	CMFCRibbonStatusBarPane*		m_StatusBarPane2;

	i3::rc_string					m_strVTexPath;

	//Renderer Create
	i3Viewer			*		m_pViewer;
	i3Framework			*		m_pFramework;

	//Panel
	CWnd						m_DummyWnd;
	CUIPaneTimeLine				m_paneTimeLine;
	CUIPaneScreen				m_paneScreen;
	CUIPaneProperties			m_paneProperties;
	CUIPaneTemplate				m_paneTemplate;
	CUIPaneScriptList			m_paneScriptList;
	CUIPaneOutput				m_paneOutput;
	
	CUIPaneTemplateExplorer		m_TemplateExplorer;

	UINT32						m_nIDResolution;	
	UINT32						m_nScriptPaneCnt;
	CMFCRibbonButton	*		m_pNationControl;

	bool						CreateRenderer(void);
	bool						CreatePanels(void);
//public:
	bool						_CreateRenderTargets( INT32 width, INT32 height);
public:
	void						_ReviveUIRenderTarget();
protected:
	// Clear Color
	i3TDKRibbonColor * m_pCOL_ClearColor;

public:
	const char*			GetNationString( int id ) const;

	//////////////////////////////////////////
	// Template Gallery 관련
protected:
	CMFCRibbonGallery *			m_pGallery_Template;
	i3::vector<CMFCToolBarImages*>	m_listBitmap;

	void						_ResetTemplateGallery(void);
	void						_DrawTemplateGallery(void);
	void						_DrawTemplate( HDC dc, HBRUSH hClearBrush, i3UITemplate * pTemplate);

	// Template Gallery 관련
	//////////////////////////////////////////

protected:
	INT32						m_DCRef;
	i3RenderTarget *			m_pUIRenderTarget;	// RenderTarget
	i3Texture *					m_pLTexture;		// render target의 내용을 copy할 texture

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);

	//resolution
	afx_msg void	OnResolutionChange(UINT id);
	afx_msg void	OnUpdateMenu_Resolution( CCmdUI * pCmdUI);
	void			_SetResolutionID( INT32 width, INT32 height);

public:
	bool			SetResolution( INT32 width, INT32 height);
	UINT32			getResolutionID( void)		{	return m_nIDResolution;	}

private:
	void	_ExportScene_Directory( const char* dir );

protected:
	
	
	DECLARE_MESSAGE_MAP()

	//initialize ribbons
	void InitializeRibbon();
	void CreateRibbon_MainCategory();
	void CreateRibbon_Home(void);
	void CreateRibbon_ProjectCategory();
	void CreateRibbon_Template();
	void CreateRibbon_VisualStyle();
	void CreateRibbon_QuickAccess();

	void CreateRibbon_Debug();

	void CreateRibbon_View( void);
public:
	void	SetWorkingFolder( const char * pszWorkingFolder);
	const char * getWorkingFolder(void);

	void	SetVirtualTexturePath(const i3::rc_string& strPath);
	const i3::rc_string& getVirtualTexturePath(void) const {	return m_strVTexPath;}

	void	SetCurrentScene( i3UIScene * pCurrent);

	i3UILibrary *	GetCurrentLibrary( void)	{	return m_paneTemplate.GetUILibrary(); }
	void	SetCurrentLibrary( i3UILibrary * pUILib);

	void	checkWorkingFolder();
	void	checkVirtualTexture();

	HDC		GetUIDC(void);
	void	ReleaseUIDC( HDC dc);

	afx_msg void OnProjectWorkingfolder();

	CWnd	*getDummyWindow(void) {	return &m_DummyWnd;}
	void	EnableExtraRenderTarget(bool bEnable);

	void	LinkVirtualTexture(void);
	void	OpenVirtualTexture();
	void	CreateVirtualTexture(bool bNewFormat);

	void	setEnableOtherScene(bool bEnable);
	void	returnToEditState();

	void	ClearNationControl();
	void	AddSubItem(CMFCRibbonBaseElement* pSubItem, int nIndex = -1);

	///////////////////////////////////////////////////////////////////////////////////////////
protected:
	bool	_IsInside( CWnd * pWnd, POINT pt);

public:
	bool	DragMoveObjectTemplate( POINT scrPt, bool bInsideFrame = false, i3ClassMeta * pInsideFrameMeta = NULL);	// template를 drag하여 screen에 배치하려 들때..
	bool	DropObject( i3UITemplate * pTemplate, POINT scrPt);
	afx_msg void OnProjectExporttemplatelibrary();
	afx_msg void OnProjectImporttemplatelibrary();
	afx_msg void OnProjectAdjustvirtualtexture();
	afx_msg void OnProjectAdjustVTexList();
	afx_msg void OnProjectCreatevirtualtexture();
	afx_msg void OnProjectCreateVTexList();

	afx_msg void OnProjectOpenvirtualtexture();
	afx_msg void OnProjectModifyVirtualtexture();
	afx_msg void OnProjectExtractAllRefTexture();
	afx_msg void OnTemplateAddanewtemplate();
	afx_msg void OnDebugDumpvirtualtexture();
	afx_msg void OnProjectRun();
	afx_msg void OnProjectStop();
	afx_msg void OnProjectSavetemplatelibrary();
	afx_msg void OnProjectSavetemplatelibrary_NEW();

	afx_msg void OnUpdateProjectRun(CCmdUI *pCmdUI);
	afx_msg void OnUpdateProjectStop(CCmdUI *pCmdUI);

	afx_msg void OnSelectSceneFromClient();

	afx_msg void OnModuleSelect();

	// Script
	afx_msg void OnSaveScript();
	afx_msg void OnUpdateSaveScript(CCmdUI *pCmdUI);
	afx_msg void OnCompileScript();		// To check errors..
	afx_msg void OnUpdateCompileScript(CCmdUI *pCmdUI);

	//View
	afx_msg void OnViewScriptPane( void);
	afx_msg void OnViewScreenPane( void);
	afx_msg void OnViewTemplatePane( void);
	afx_msg void OnViewTimelinePane( void);
	afx_msg void OnViewPropertiesPane( void);
	afx_msg void OnViewOutputPane( void);
	afx_msg void OnViewTemplateExplorerPane( void);

	afx_msg void OnUpdateViewScriptPane(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewScreenPane(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewTemplatePane(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewTimelinePane(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewPropertiesPane(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewOutputPane(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewTemplateExplorerPane(CCmdUI *pCmdUI);

	void	UpdatePropertyPane( void);

	void	UpdateStatusBar( bool bScreen);

	CUIPaneOutput*			getOutputPane( void)		{	return &m_paneOutput;		}	//?
	CUIPaneScreen*			getScreenPane( void)		{	return &m_paneScreen;		}
	CUIPaneProperties*		getPropertiesPane( void)		{	return &m_paneProperties;	}
	CUIPaneTemplate*		getTemplatePane( void)			{	return &m_paneTemplate;		}
	CUIPaneTemplateExplorer*getTemplateExplorer() { return &m_TemplateExplorer; }

	//==============================================================
	//	Script
	//==============================================================
protected:
	i3::vector<CScriptPane*>	m_PaneList;
	
	CScriptPane*		_getFocusedPane( void);

public:
	CScriptPane*	OnEditScript( const char* pszName, bool bFile, bool bScreen);	// Popup Menu에서 Script->Edit 선택
	void	OnCloseScript( CScriptPane* pPaneToBeClosed);					// Script Window가 닫힐 때 
	void	OnModifyScript( CScriptWnd* pWnd, bool bModified);				// Script의 내용이 변경되었을 때
	void	OnSavingScript( CScriptPane* pPane);							// Script를 저장할 때 

	void	OnFileChanged( bool bScreen);

	
	virtual BOOL OnCloseMiniFrame( CPaneFrameWnd* pWnd);
	virtual BOOL OnCloseDockingPane( CDockablePane* pPane);

	void		CloseAllScript();
	//OnRenameScript();
	//void	OnRemoveScript();

protected:
	BOOL			m_bFileWatcher;
	FileWatcher		m_FileWatcher;
	FileWatcher2	m_LuaWatcher;

public:
	void		CreateFileWatcher( void);
	void		SuspendFileWatcher( void);
	void		ResumeFileWatcher( void);
	void		TerminateFileWatcher( void);
	
	bool		IsScriptChanged( bool bScreen);
	void		SetScriptChanged( bool bScreen, bool bChanged);

	bool		IsLuaChanged( void)			{	return m_LuaWatcher.isModified();	}
	void		SetLuaChanged( bool bFlag)	{	m_LuaWatcher.setModified( bFlag);	}


public:
	INT32		getSelectedObjCount( void);
	void		getSelectedObjList( i3::vector<i3UIControl*>& Out);

	// File
	afx_msg void		OnExport( void);
	afx_msg void		OnUpdateExportUI( CCmdUI * pCmdUI);

	afx_msg void		OnExportLib( void);
	afx_msg void		OnUpdateExportLIbUI( CCmdUI * pCmdUI);

	// Edit
	// MainFrm_Home.cpp
	afx_msg void		OnCopy( void);
	afx_msg void		OnUpdateCopyUI( CCmdUI *pCmdUI);
	afx_msg void		OnCut( void);
	afx_msg void		OnUpdateCutUI( CCmdUI *pCmdUI);
	afx_msg void		OnPaste( void);
	afx_msg void		OnUpdatePasteUI( CCmdUI * pCmdUI);

	// Undo System
	afx_msg void		OnUndo( void);
	afx_msg void		OnRedo( void);
	afx_msg void		OnUpdateUndoButton( CCmdUI *pCmdUI);
	afx_msg void		OnUpdateRedoButton( CCmdUI *pCmdUI);

	// Align
	afx_msg void		OnUpdateAlignButton( CCmdUI * pCmdUI);
	afx_msg void		OnUpdateAlignCenterButton( CCmdUI * pCmdUI);
	afx_msg void		OnAlignLeft( void);
	afx_msg void		OnAlignRight( void);
	afx_msg void		OnAlignTop( void);
	afx_msg void		OnAlignBottom( void);
	afx_msg void		OnAlignCenter( void);

	afx_msg void		OnCascadeHorz( void);
	afx_msg void		OnCascadeVert( void);

	// Snap
	CMFCRibbonEdit *	m_pEditSnapSize;
	afx_msg void		OnSnapSize( void);
	afx_msg void		OnUpdateSnapSize( CCmdUI * pCmdUI);


	void				GotoViewTemplateEdit( i3UITemplate * pCtrl);

	void				ImportUIScene( void);
	void				ExportUIScene( i3UIControl * pRoot);



	// Nation
	afx_msg void OnUpdateMenu_Nation(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNation(UINT id);
	afx_msg void OnDebugExportall();
	afx_msg void OnUpdateDebugExportall(CCmdUI *pCmdUI);


	// Clear Color
	afx_msg void OnClicked_ClearColor();


private:
	CImageChecker m_ImgChecker;		//Image Check
	CUIToolFileMgr m_toolFileMgr;	//Folder Manager
	COLORREAL		m_ClearColor;

	void		_LoadClearColor(void);
	void		_SaveClearColor(void);
	void		_SetClearColor(void);

public:
	void UpdateVirtualTextureFiles();
	void UpdateNationFromVTex();
	void UpdateDefaultImageInfo();

	const COLORREAL &	getClearColor(void)	const			{ return m_ClearColor; }
};


