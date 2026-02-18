#if !defined( __OPTION_INFO_H)
#define __OPTION_INFO_H

#define MAX_MRU			8
#define MAX_RECCAM		4

enum VIEWER_MODE
{
	VIEWER_MODE_NORMAL = 0,
	VIEWER_MODE_MESH,
	VIEWER_MODE_COLL,
	VIEWER_MODE_WIRE,
	VIEWER_MODE_SELONLY,
};

class COptionInfo : public i3ElementBase
{
	I3_CLASS_DEFINE( COptionInfo);
public:
	VIEWER_MODE	m_ViewerMode;
	BOOL		m_bShadowEnable;
	INT32		m_ShadowSize;

	BOOL		m_bGlow;

	COLORREAL	m_Ambient;
	COLORREAL	m_LightDiffuse;
	COLORREAL	m_LightSpecular;
	REAL32		m_LightTheta;
	REAL32		m_LightRho;
	REAL32		m_LightDist;

	COLORREAL	m_BackColor;

	BOOL		m_bGridShow;

	char		m_MRU[ MAX_MRU][MAX_PATH];

	VEC3D		m_CamPos[MAX_RECCAM];
	QUATERNION	m_CamRot[MAX_RECCAM];

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

	INT32			getMRUCount(void);
	const char *	getMRU( INT32 idx)				{ return (const char *) m_MRU[idx]; }
	void			addMRU( const char * pszPath);

	void			Load(void);
	void			Save(void);
};

#endif
