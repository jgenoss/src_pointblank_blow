#if !defined( __OPTION_INFO_H)
#define __OPTION_INFO_H

class COptionInfo
{
public:
	char		m_szWorkPath[ MAX_PATH];

public:
	COptionInfo(void);
	virtual ~COptionInfo(void);

	void		Load(void);
	void		Save(void);
};

#endif
