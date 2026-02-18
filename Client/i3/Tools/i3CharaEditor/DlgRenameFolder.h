#pragma once

enum RENAME_TYPE
{
	RENAME_TYPE_ANIMATIONFOLDER,
	RENAME_TYPE_AUTOAISTATEANIM,
};

class AI;

// CDlgRenameFolder dialog
class CDlgRenameFolder : public CDialog
{
	DECLARE_DYNAMIC(CDlgRenameFolder)

public:
	CDlgRenameFolder(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRenameFolder();

// Dialog Data
	enum { IDD = IDD_DLG_RENAMEFOLDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	char		m_szFilter[ MAX_PATH ];
	char		m_szChangeName[ MAX_PATH ];

	RENAME_TYPE	m_nRenameType;
	AI *		m_pAI;
	GICShapeAI *	m_pShapeAI;

	virtual void OnOK();

	i3Animation *	FindAnimFromAnimList( const char * pszName);
	void		ChangeAnim(void);
	INT32		FindStartPosSameName( const char * pszName, const char * pszCon);
	bool		CheckTrueSameInclude( const char * pszPath, const char * pszName, INT32 iStartPos = -1);
	i3Animation * CheckAnimFileName( const char * pszPath, const char * pszName);

public:
	CEdit		m_EdtFilterNameCtrl;
	CEdit		m_EdtChangeNameCtrl;
	afx_msg void OnEnChangeEditFolderFilter();
	afx_msg void OnEnChangeEditFolderRename();
	afx_msg void OnBnClickedOk();

	void		setRenameType( RENAME_TYPE type)		{ m_nRenameType = type; }
	void		setAI( AI * pAI)						{ m_pAI = pAI; }
};
