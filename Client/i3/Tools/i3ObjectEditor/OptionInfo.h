#if !defined( __OPTION_INFO_H)
#define __OPTION_INFO_H

class COptionInfo
{
public:
	char		m_szWorkPath[ MAX_PATH];

	BOOL		m_bExportI3S;
	BOOL		m_bExportI3OBJ;
	BOOL		m_bGenMipmap;
	BOOL		m_bConvertDXT;
	BOOL		m_bRotateAxis;

	BOOL		m_bGlobalHull;
	BOOL		m_bDynamicHull;
	REAL32		m_Elasticity;
	REAL32		m_Friction;

public:
	COptionInfo(void);
	virtual ~COptionInfo(void);

	void		Load(void);
	void		Save(void);
};

#endif
