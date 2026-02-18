#pragma once

#include "Encrypter.h"

class i3EncrypterView;

class i3EncrypterDoc : public CDocument
{
protected:
	i3EncrypterDoc();
	DECLARE_DYNCREATE(i3EncrypterDoc)

public:
	i3EncrypterView*	GetActiveView();

public:
	CEncrypter		m_enc;

public:
	BOOL	SplitFileName( const std::wstring& path, std::wstring &filename, std::wstring &dir, std::wstring &ext );

public:
	virtual BOOL	OnNewDocument();
	virtual BOOL	OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL	OnSaveDocument(LPCTSTR lpszPathName);

	virtual void	Serialize(CArchive& ar);

public:
	virtual ~i3EncrypterDoc();

protected:
	DECLARE_MESSAGE_MAP()

public:
	
};


