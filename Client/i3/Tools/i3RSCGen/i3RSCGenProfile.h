#if !defined( I3_RSCGEN_PROFILE_H_)
#define I3_RSCGEN_PROFILE_H__

class Ci3RSCGenProfile
{
public:
	char	m_szImportDir[MAX_PATH];
	char	m_szFileListName[MAX_PATH];
	i3::rc_string	m_strWorkDir;

	char	m_szExceptRull[4096];
	int		m_nAlign;

	INT32	m_MainWndX, m_MainWndY;
	INT32	m_MainWndCX, m_MainWndCY;

public:
	Ci3RSCGenProfile(void);
	virtual ~Ci3RSCGenProfile(void);

	void	Load( void);
	void	Save( void);
};

#endif	//