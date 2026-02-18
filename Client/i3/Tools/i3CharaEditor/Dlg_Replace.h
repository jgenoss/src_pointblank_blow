#pragma once
#include "afxwin.h"


// CDlg_Replace dialog

class CDlg_Replace : public CDialog
{
	DECLARE_DYNAMIC(CDlg_Replace)
protected:
	char	m_szSrc[512];
	char	m_szDest[ 512];

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the source string. </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-30. </remarks>
	///
	/// <returns>	null if it fails, else the source string. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	const char *	getSrcString(void)			{ return (const char *) m_szSrc; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the destination string. </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-30. </remarks>
	///
	/// <returns>	null if it fails, else the destination string. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	const char *	getDestString(void)			{ return (const char *) m_szDest; }

	bool	Execute( const char * pszSrc, const char * pszDest);

public:
	CDlg_Replace(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_Replace();

// Dialog Data

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Values that represent . </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-30. </remarks>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	enum { IDD = IDD_DLG_REPLACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_Src;
	CEdit m_ED_Dest;
protected:
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
};
