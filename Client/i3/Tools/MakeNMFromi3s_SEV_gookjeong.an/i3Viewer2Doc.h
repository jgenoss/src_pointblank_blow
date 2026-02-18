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

// i3Viewer2Doc.h : Ci3Viewer2Doc 클래스의 인터페이스
//


#pragma once

enum enumEngineFileType
{
	I3FILE_TYPE_UNKNOWN = 0,
	I3FILE_TYPE_SG,
	I3FILE_TYPE_ANIM,
	I3FILE_TYPE_UI,
};

class Ci3Viewer2Doc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	Ci3Viewer2Doc();
	DECLARE_DYNCREATE(Ci3Viewer2Doc)

// 특성입니다.
public:
	char		m_szFileName[ MAX_PATH];

	i3UIScene	*		m_pUI;

// 작업입니다.
public:
	void				_setCaption( const char * pszPath);
	bool				CloseI3S(void);
	bool				LoadI3S( const char * pszPath);
	bool				SaveI3S( const char * pszPath);
	bool				LoadAnimation( const char * pszPath);
	enumEngineFileType	_checkFileName( char * FileName);
	bool				LoadFile( const char * pszPath);

	bool				CloseUI(void);
	bool				LoadUI(const char * pszPath);

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();

// 구현입니다.
public:
	virtual ~Ci3Viewer2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};


