#pragma once


typedef struct _codeinfo
{
	char	m_szBlock[MAX_PATH];
	INT32	m_nStart;
	INT32	m_nEnd;
}CODEINFO;

// CScriptWnd
//Reference: http://www.scintilla.org/ScintillaDoc.html -> A simple sample using Scintilla from C++ on Windows.
class CScriptWnd : public CWnd	//CDockablePane
{
	DECLARE_DYNAMIC(CScriptWnd)

protected:
	DECLARE_MESSAGE_MAP()

	bool		m_bPreviousModifyState;

	bool		m_bFileOpened;					// true => file opened, false => block opened 
	
	char		m_pszFullPath[MAX_PATH];		// same as i3UIScene::getScriptFile(). updated when "OpenFile()" is called.
	char		m_pszBlockPath[MAX_PATH];		// temporarily created file. "OpenFile()" actually opens this file.
	char		m_pszBlock[MAX_PATH];			// currently working block..

	char		m_WindowName[MAX_PATH];

	// Find
	INT32		m_SearchFlags;
	INT32		m_SearchDirection;
	char		m_SearchText[MAX_PATH];
	bool		m_bFindForward;
	INT32		m_nInitialPos;

	bool		m_bReload;

	i3::vector<i3::rc_string> m_FoundList;
protected:
	
	void		SetAStyle(int style, COLORREF fore, COLORREF back=RGB( 255, 255, 255 ), int size=-1, const char *face=0);
	LRESULT		SendEditor(UINT Msg, WPARAM wParam=0, LPARAM lParam=0);
	void		GetRange(int start, int end, char *text);
	
	// Extracts code block between two [block] tags from [srcfile] and then writes to [destfile]
	bool	_ExtractBlock( const char* srcfile, const char* pszblock, const char* destfile);

	// Inserts whole text from [srcfile] into [destfile] in between two [block] tags
	bool	_InsertBlock( const char* srcfile, const char* pszblock, const char* destfile);

	// 
	//bool	_FindBlock( const char* pszfile, const char* pszblock);

	void	_DeleteBlockFile( void);

	void	_OpenFile( const char* filename);
	void	_SaveFile( const char* filename);

public:
	CScriptWnd();
	virtual ~CScriptWnd();

	void	AddSearchFlag(INT32 flag) {m_SearchFlags |= flag;}
	void	DelSearchFlag(INT32 flag) { m_SearchFlags &= ~flag; }
	void	SetSearchDir(INT32 dir) { m_SearchDirection = dir; }
	void	SetFindForward(bool bState) {m_bFindForward = bState;}
	void	SetSearchText(i3::string str) { i3::string_ncopy_nullpad(m_SearchText, str.c_str(), sizeof(m_SearchText)); }
	void	AddFindList(i3::rc_string str) { m_FoundList.push_back(str); }
	void	DelFindList(INT32 nIndex) { i3::vu::erase_index(m_FoundList, nIndex); }
	i3::rc_string GetFindList(INT32 nIndex) {return m_FoundList[nIndex].c_str();}
	INT32 GetFindListSize() { return m_FoundList.size(); }
	void		Initialize( void);
	void	AddText(char* data);
	void	New( void);

	// File 
	void			SetFilename( const char* filename);
	const char*		GetFilename( void);
	void			OpenFile( void);
	void			SaveFile( void);

	// Block
	void	OpenBlock( const char* block);
	void	SaveBlock( const char* block);
	void	RenameBlock( const char* oldname, const char* newname);
	
	void	Save( void);
		
	bool	IsModified( void);
	bool	SaveIfModified( void);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void	GoToLine( void);
	void	Find( void);
	void	FindForward( void);
	void	FindBackward( void);
	void	OnKeyDownF3( void);
	void	OnKeyDownShiftF3( void);

	bool	isFileOpened( void)		{	return m_bFileOpened;	}
	
	void	setWindowName( const char* pszName);
	void	getWindowName( char* buf, INT32 nBufSize);

	void	ReOpen( void);

	// File Watch
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	void	setNeedToReload( void)	{	m_bReload = true;	}

protected:
	afx_msg void	OnContextMenu( CWnd* pWnd, CPoint pos);

	void			OnInsertEventHandler( void);
	void			TestFunc( void);

public:
	INT32			getCurrentPos( void);
	void			SetCurrentPosTo( INT32 nPos);
	void			AppendText( const char* pszText);
	void			InsertText( const char* pszText, INT32 nPos);

	INT32			getTextLen( void);
	void			getTextBuf( char* pBuf, INT32 nSize);	// buffer¸¦ copy... T.T

//	void			getBlockname( char* pBuf, INT32 nBufSize)	{	i3::string_ncopy_nullpad( pBuf, m_pszBlock, nBufSize);	}
};


