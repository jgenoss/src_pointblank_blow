#if !defined( __OPTION_INFO_H)
#define __OPTION_INFO_H
#define MAX_WORKHISTORY 3


class COptionInfo
{
public:
	char			m_szWorkPath[ MAX_PATH];
	char			m_szLivePath[ MAX_PATH];
	char			m_szScriptPath[ MAX_PATH];
	//char			m_szWorkHistory[MAX_WORKHISTORY][MAX_PATH];
	i3::vector<i3::rc_string>	m_ExceptList;

public:
	COptionInfo(void);
	virtual ~COptionInfo(void);

	bool		isExceptFile( char * pszPath);

	void		Load(void);
	void		Save(void);
};

#endif
