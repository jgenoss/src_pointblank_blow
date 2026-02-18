#pragma once

class CGuiEditorProfile
{
public:
	char	m_szWorkDir[MAX_PATH];
	char	m_szExportDir[MAX_PATH];

	INT32	m_MainWndX,		m_MainWndY;
	INT32	m_MainWndCX,	m_MainWndCY;
public:
	CGuiEditorProfile(void);
	virtual ~CGuiEditorProfile(void);

public:
	void	Save( void);
	void	Load( void);
};
