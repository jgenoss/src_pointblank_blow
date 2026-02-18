
// ControlToolDoc.h : interface of the CControlToolDoc class
//


#pragma once


class CControlToolDoc : public CDocument
{
protected: // create from serialization only
	CControlToolDoc();
	DECLARE_DYNCREATE(CControlToolDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CControlToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


