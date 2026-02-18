#pragma once

class CPropEditorProfile
{
public:
	INT32	m_MainWndX, m_MainWndY;
	INT32	m_MainWndCX, m_MainWndCY;

public:
	CPropEditorProfile(void);
	virtual ~CPropEditorProfile(void);

public:
	void	Load( void);
	void	Save( void);
};
