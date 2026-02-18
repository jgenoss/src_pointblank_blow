#pragma once

class CPropEditorProfile
{
public:
	CString m_WorkDirectoryPath;
	int m_BakFile;

public:
	CPropEditorProfile(void);
	virtual ~CPropEditorProfile(void);

public:
	void	Load( void);
	void	Save( void);
};
