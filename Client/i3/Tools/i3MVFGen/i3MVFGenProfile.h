#if !defined( __EDITOR_PROFILE_H)
#define __EDITOR_PROFILE_H

class Ci3MVFGenProfile
{
public:
	char		m_szImportDir[MAX_PATH];
	char		m_szFileListName[MAX_PATH];
	char		m_szWorkDir[MAX_PATH];
	char		m_szExceptRull[ 4096];
	int			m_nAlign;

	INT32		m_MainWndX, m_MainWndY;
	INT32		m_MainWndCX, m_MainWndCY;

public:
	Ci3MVFGenProfile(void);
	virtual ~Ci3MVFGenProfile(void);

	void	Load(void);
	void	Save(void);

};

#endif