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

// i3VTexDoc.h : interface of the Ci3VTexDoc class
//


#pragma once


class Ci3VTexDoc : public CDocument
{
protected: // create from serialization only
	Ci3VTexDoc();
	DECLARE_DYNCREATE(Ci3VTexDoc)

// Attributes
public:
	i3VirtualTexture *		m_pTex;
	char					m_szLoadPath[ MAX_PATH];

// Operations
public:
	bool					CreateNewVirualTexture(void);

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~Ci3VTexDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnToolCreatenewvirtualtexture();
	afx_msg void OnToolDumpvirtualtexture();
	afx_msg void OnToolVerifyvirtualtexture();
	afx_msg void OnToolDumpfonttexture();
};


