#pragma once

class CUIEditorProfile
{
public:
	char	m_szWorkDir[MAX_PATH];
	char	m_szExportDir[MAX_PATH];

	INT32	m_MainWndX,		m_MainWndY;
	INT32	m_MainWndCX,	m_MainWndCY;
public:
	CUIEditorProfile(void);
	virtual ~CUIEditorProfile(void);

public:
	void	Save( void);
	void	Load( void);
};
