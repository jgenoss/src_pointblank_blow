#pragma once

#include "ScriptWnd.h"

// ScriptPane

class CScriptPane : public CDockablePane
{
	DECLARE_DYNAMIC(CScriptPane)

protected:
	CScriptWnd		m_ScriptWnd;
	bool			m_bScreen;

public:
	CScriptPane();
	virtual ~CScriptPane();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// File
	void			New( void);
	void			SetFilename( const char* filename);
	const char*		GetFilename( void);
	void			GetSimplename( char* buf);

	void			OpenFile( void);
	void			SaveFile( void);
		
	bool			SaveIfModified( void);
	bool			IsModified( void);

	// Block
	void			OpenBlock( const char* blockname);
	void			SaveBlock( const char* blockname);
	void			RenameBlock( const char* oldname, const char* newname);

	void			Save( void);
	void			ReOpen( void);

	bool			isFileOpened( void);

	virtual void OnPressCloseButton() override;

	//OnClose? 
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL DestroyWindow() override;
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	//if( IsModified())
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);


	CScriptWnd*	getScriptWnd( void)		{	return &m_ScriptWnd;	}
	
	bool		isBelongToScreen( void)		{	return m_bScreen;		}
	void		setBelongToScreen( void)	{	m_bScreen = true;	}

	void		setNeedToReload( void);	

	void		OnSavingScript( void);
};


