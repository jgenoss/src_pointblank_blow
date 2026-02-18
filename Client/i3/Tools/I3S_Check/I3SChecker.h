#if !defined( __I3S_CHECKER_H)
#define __I3S_CHECKER_H

class I3SChecker
{
public:
	char	m_szTargetPath[ MAX_PATH];
	char	m_szWorkPath[ MAX_PATH];
	char	m_szOutputPath[ MAX_PATH];
	bool	m_bCheckHidden;

	bool	m_bCheckIndexedGeometry;
	bool	m_bCheckTangentBasis;
	bool	m_bCheckTextureFormat;
	bool	m_bCheckBoundBox;
	bool	m_bCheckBoundSphere;
	bool	m_bCheckVertexBlend;
	bool	m_bCheckTexCoordSet;
	bool	m_bCheckVertexColor;
	bool	m_bCheckStaticTransform;

	INT32	m_Count;
	FILE *	m_pOutStream;

	i3::vector<i3::rc_string>	m_LogList;

	// Temporary members
	INT32	m_BBoxCount;

protected:
	void		_ResetLog( const char * pszFile);
	void		_AddLog( const char * pszFormat, ...);
	void		_FlushLog(void);

	void		_CHK_Geometry( i3Geometry * pGeo);

	void		_CHK_AttrSet( i3AttrSet * pAttrSet);
	void		_CHK_StaticTransform( i3Transform * pTrans);
	void		_CHK_Body( i3Body * pBody);

	void		_CHK_BoundBox(void);
	void		_CHK_Texture( i3Texture * pTex);

	void		_CHK_PhysixShapeSet( i3PhysixShapeSet * pShapeSet);

public:
	I3SChecker(void);
	~I3SChecker(void);

	char *		getTargetPath(void)						{ return m_szTargetPath; }
	void		setTargetPath( const char * pszPath)	{ strcpy( m_szTargetPath, pszPath); }

	char *		getWorkPath(void)						{ return m_szWorkPath; }
	void		setWorkPath( const char * pszPath)		{ strcpy( m_szWorkPath, pszPath); }

	char *		getOutputPath(void)						{ return m_szOutputPath; }
	void		setOutputPath( const char * pszPath)	{ strcpy( m_szOutputPath, pszPath); }

	bool		getCheckHiddenState(void)				{ return m_bCheckHidden; }
	void		setCheckHiddenState( bool bState)		{ m_bCheckHidden = bState; }

	void		CheckFile( const char * pszFile);

	INT32		Check(void);

	static bool _I3SCallback( INT32 Level, char * pszPath, void * pFileInfo, void * pUserData);
	static I3SG_TRAVERSAL_RESULT _CheckNodeProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);
};

#endif