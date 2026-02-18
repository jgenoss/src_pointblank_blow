#if !defined( __OPTION_INFO_H)
#define __OPTION_INFO_H

class COptionInfo : public i3ElementBase
{
	I3SG_NODE_CLASS_DEFINE( COptionInfo, i3ElementBase);
public:
	char		m_szWorkPath[ MAX_PATH];
	COLORREAL	m_BackColor;

public:
	COptionInfo(void);
	virtual ~COptionInfo(void);

	void		Load(void);
	void		Save(void);

	void		_GetVEC4D( const char * pszEntry, VEC4D * pVec, REAL32 x, REAL32 y, REAL32 z, REAL32 w);
	void		_SetVEC4D( const char * pszEntry, VEC4D * pVec);
};

#endif
