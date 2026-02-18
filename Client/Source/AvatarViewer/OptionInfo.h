#if !defined( __OPTION_INFO_H)
#define __OPTION_INFO_H

class COptionInfo : public i3ElementBase
{
	I3_CLASS_DEFINE( COptionInfo, i3ElementBase);
public:
	BOOL		m_bShadowEnable;
	INT32		m_ShadowSize;

	BOOL		m_bGlow;

	COLORREAL	m_Ambient;
	REAL32		m_LightIntensity;
	COLORREAL	m_LightDiffuse;
	COLORREAL	m_LightSpecular;
	REAL32		m_LightTheta;
	REAL32		m_LightRho;
	REAL32		m_LightDist;

	COLORREAL	m_BackColor;

	BOOL		m_bGridShow;

	char		m_szReflectMap[ MAX_PATH];

	TEAM_TYPE	m_eTeamColor;
	VEC3D		m_vTranslation;
	VEC2D		m_vRotation;

protected:
	INT32		_GetINT32( const char * pszEntry, INT32 def);
	void		_SetINT32( const char * pszEntry, INT32 val);

	REAL32		_GetREAL32( const char * pszEntry, REAL32 def);
	void		_SetREAL32( const char * pszEntry, REAL32 val);

	void		_GetString( const char * pszEntry, char * pszStr, INT32 len, const char * pszDef);
	void		_SetString( const char * pszEntry, const char * pszStr);

	void		_GetVEC3D( const char * pszEntry, VEC3D * pCol, REAL32 x, REAL32 y, REAL32 z);
	void		_SetVEC3D( const char * pszEntry, VEC3D * pCol);

	void		_GetVEC4D( const char * pszEntry, VEC4D * pCol, REAL32 x, REAL32 y, REAL32 z, REAL32 w);
	void		_SetVEC4D( const char * pszEntry, VEC4D * pCol);

public:
	COptionInfo(void);
	virtual ~COptionInfo(void);

	void			Load(void);
	void			Save(void);
};

#endif
