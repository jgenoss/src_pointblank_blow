// i3PackFileEditorDoc.h : Ci3PackFileEditorDoc 클래스의 인터페이스
//


#pragma once

#include "PackNode.h"

class Ci3PackFileEditorDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	Ci3PackFileEditorDoc();
	DECLARE_DYNCREATE(Ci3PackFileEditorDoc)

// 특성
public:
	i3PackFile *		m_pPack;
	INT32				m_I3SCount;
	INT32				m_I3ACount;
	INT32				m_ImageCount;
	INT32				m_BinaryCount;
	INT32				m_TotalFileCount;
	BOOL				m_bSwizzled;

protected:
	void		Rec_BuildPack( PackNode * pSrc, i3PackNode * pDestParent);
	void		OnCompileI3S( i3PackNode * pNode, char * pszPath);
	void		OnCompileI3A( i3PackNode * pNode, char * pszPath);
	void		OnCompileImage( i3PackNode * pNode, char * pszPath);
	void		OnCompileSound( i3PackNode * pNode, char * pszPath);
	void		OnCompileBinary( i3PackNode * pNode, char * pszPath);

	void		OnProcessImage( i3PackNode * pNode, i3Texture * pTex);

	INT32		GetLeafFileCount(void);

// 작업
public:
	BOOL		LoadProject( const char *szFileName );
	BOOL		ImportFileList( const char *szFileName );

	void		AddFile( char * pszPath);
	void		RemoveFile( PackNode * pNode);
	void		BuildPack( const char * pszPath);

// 재정의
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 구현
public:
	virtual ~Ci3PackFileEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	afx_msg void OnToolCompile();
	afx_msg void OnFileImport();
	afx_msg void OnToolExtracti3p();
	afx_msg void OnFileImportForPsp();
};


