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

// i3UIEditorDoc.h : interface of the Ci3UIEditorDoc class
//


#pragma once

#include "UIProject.h"

class Ci3UIEditorDoc : public CDocument
{
protected: // create from serialization only
	Ci3UIEditorDoc();
	DECLARE_DYNCREATE(Ci3UIEditorDoc)

	DECLARE_MESSAGE_MAP()

public:
	virtual ~Ci3UIEditorDoc();

protected:
	bool			m_bFirst;
	bool			m_bLoaded;
	
	i3UIScene	*	m_pScene;

	//void	SetCurrentProject( CUIProject* pProject);

	virtual BOOL SaveModified() override;
	
	void			_AdjustResolution( void);

	void			_CloseUIScene(void);

public:
	void			setCurrentScene( i3UIScene * pScene, const char * pszFileName, bool bAdd = true);
	i3UIScene	*	getCurrentScene( void)		{	return m_pScene;}

	void			SetActiveScene();

	void			setUILibrary( i3UILibrary * pLib);
	void			getUILibrary(void);

	void			SetResolution( UINT32 width, UINT32 height);

	bool			isLoaded()	{	return m_bLoaded;}

	virtual BOOL OnNewDocument() override;
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName) override;
	virtual void OnCloseDocument() override;
	virtual void Serialize(CArchive& ar) override;
	virtual void DeleteContents() override;
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame) override;
	virtual void PreCloseFrame( CFrameWnd* pFrame) override;
	virtual BOOL IsModified( void) override;
	virtual void SetModifiedFlag( BOOL bModified = TRUE) override;

	i3UIScene *	ImportScene( const char * pszPath = NULL);
	bool		ExportScene( i3UIControl * pRoot, const char * pszPath = NULL);
	
	void CreateTemplateLibrary(void);
	void ImportTemplateLibrary(const char * pszLibPath = NULL);

	void ExportTemplateLibrary(bool isNewFormat, const char * pszLibPath = NULL);
	void ExportTemplateLibrary_Depricated(const char * pszLibPath = NULL);

	void SaveTemplateLibrary(bool isNewFormat);
	void SaveTemplateLibrary_Deprecated(void);
	void AutoSaveTemplateLibrary(void);	// (임시)TemplateLibrary 파일을 XML -> Binary로 변경하는 함수(차후 제거하겠습니다.) - 2014.6.13 한상훈

	void			CheckSaveTemplateLibrary( void);

	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	
	void AddFontList(std::string path);

};


