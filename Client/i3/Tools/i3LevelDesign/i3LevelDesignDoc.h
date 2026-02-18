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

// i3LevelEditorDoc.h : interface of the Ci3LevelDesignDoc class
//


#pragma once


class Ci3LevelDesignDoc : public CDocument
{
protected: // create from serialization only
	Ci3LevelDesignDoc();
	DECLARE_DYNCREATE(Ci3LevelDesignDoc)

// Attributes
public:
	char	m_szPath[MAX_PATH];


// Operations
public:
	void	NewLevel(void);
	bool	LoadLevel( const char * pszPath);
	bool	SaveLevel( const char * pszPath);
	void	CloseLevel(void);

	bool	IsSavedLevel(void)						{ return m_szPath[0] != 0; }

protected:
	void	_SetPath( const char * pszPath);

	void	CheckForLoadingError(void);

// Overrides
public:
	virtual BOOL OnNewDocument() override;
	virtual void Serialize(CArchive& ar) override;

// Implementation
public:
	virtual ~Ci3LevelDesignDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName) override;
	virtual void OnCloseDocument(void) override;
};


