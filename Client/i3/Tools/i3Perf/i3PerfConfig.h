#if !defined( __I3_PERF_CONFIG_H)
#define __I3_PERF_CONFIG_H

#define MAX_CAM_RECORD			5

class i3PerfConfig : public i3NamedElement
{
	I3_CLASS_DEFINE( i3PerfConfig);
protected:
	char		m_szPath[ MAX_PATH];

	char		m_szLastI3S[MAX_PATH];
	INT32		m_Width;
	INT32		m_Height;
	MATRIX		m_CamPos[ MAX_CAM_RECORD];

	BOOL		m_bShadow;

public:
	i3PerfConfig(void);
	virtual ~i3PerfConfig(void);

	const char *		getLastI3S(void)						{ return m_szLastI3S; }
	void				setLastI3S( const char * pszPath)		{ i3String::Copy( m_szLastI3S, pszPath, MAX_PATH ); }

	MATRIX *			getCamRecordPos( INT32 idx)				{ return & m_CamPos[ idx]; }
	void				setCamRecordPos( INT32 idx, MATRIX * pMtx)
	{
		i3Matrix::Copy( &m_CamPos[idx], pMtx);
	}

	BOOL				getShadowState(void)					{ return m_bShadow; }
	void				setShadowState( BOOL bFlag)				{ m_bShadow = bFlag; }

	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	void						Save(void);

	static i3PerfConfig *		Load( const char * pszName);
	static bool					Save( const char * pszName, i3PerfConfig * pConfig);
};

#endif
