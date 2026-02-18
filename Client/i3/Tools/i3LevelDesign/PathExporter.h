#if !defined( __PATH_EXPORTER_H)
#define __PATH_EXPORTER_H

struct PATHPOINT_INFO
{
	VEC3D	m_Point;
	REAL32	m_fRadius = 0.0f;
	REAL32	m_fTheta = 0.0f;
	INT32	m_UsedCount = 0;
} ;

struct PATHLINK_INFO
{
	i3::vector<UINT32>		m_PointList;		// Array of Index
	i3::vector<UINT32>		m_StartLinkList;
	i3::vector<UINT32>		m_EndLinkList;
} ;

class CPathExporter
{
protected:
	i3::vector<PATHPOINT_INFO*>	m_PointList;
	i3::vector<PATHLINK_INFO*>	m_LinkList;

	INT32			_FindPoint( VEC3D * pPt, REAL32 fRadius);
	INT32			_AddPoint( VEC3D * pPt, REAL32 fRadius, REAL32 fTheta);
	void			_RemoveAllPoints(void);

//	void			_FindLinkByPoint(  INT32 linkIndex, INT32 endPtIndex, List * pList);		// 쓰이지 않아 주석..
	void			_RemoveAllLinks(void);

	void			_BuildPointList(void);
	void			_BuildLinkList(void);

public:
	CPathExporter(void);
	virtual ~CPathExporter(void);

	bool	Export( const char * pszPath);

	bool	ExportCode( const char * pszPath );
	bool	ExportPersist( const char * pszPath );
};

#endif
