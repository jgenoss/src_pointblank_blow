#if !defined( __OPTION_INFO_H)
#define __OPTION_INFO_H

class COptionInfo
{
public:
	char		m_szWorkPath[ MAX_PATH];
	char		m_szExportResFolder[MAX_PATH];
	BOOL		m_bExternTexture;

public:
	COptionInfo(void);
	virtual ~COptionInfo(void);

	void		Load(void);
	void		Save(void);
};

#endif
