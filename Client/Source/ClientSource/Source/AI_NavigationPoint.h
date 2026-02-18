#if !defined( __AI_NAVIGATIONPOINT_H )
#define __AI_NAVIGATIONPOINT_H

#define MAX_NAVIGATIONPOINTCOUNT		0xFFFF
#define MAX_NAVIGATIONPATHCOUNT			0xFFFF 

#define NAVIGATIONPOINT_TYPE_O			1
#define NAVIGATIONPOINT_TYPE_X			2

enum MISSION_STAGELIST
{
	STAGE_000 = 0,
};

typedef struct ALIGN4 _tagNavigationPoint
{
	VEC3D		m_VecPosition;
	REAL32		m_fRadius;
} NavigationPoint;

typedef struct ALIGN4 _tagNavigationPoint2
{
	VEC3D		m_VecPosition;
	REAL32		m_fRadius;
	REAL32		m_fTheta;
} NavigationPoint2;

typedef struct ALIGN4 _tagNavigationPathInfo
{
	INT32		m_nPointCount;					// 패스리스트 카운트
	UINT32 *	m_pPointIndices;				// 패스 리스트

	//UINT8		m_nLinkCount;
	//UINT32		m_Links[ 16 ];

	UINT8		m_nPrvPathCount;
	UINT32 *	m_pPrvPath;

	UINT8		m_nNextPathCount;				// 끝점에서 다음 패스로 이어지는 카운트
	UINT32 *	m_pNextPath;					// 패스 인덱스( NavigationPath *)
} NavigationPath;

class CAI_NavigationPoint : public i3ElementBase
{
	I3_CLASS_DEFINE( CAI_NavigationPoint, i3ElementBase );
protected:
	i3::vector<NavigationPoint*> m_PointList;
	i3::vector<NavigationPath*>	m_PathList;

#ifdef I3_DEBUG
	i3Geometry *	m_pGeo;
	i3VertexArray * m_pVA;
#endif

	INT32		m_nType;

	void		_RemovePointList( void );
	void		_RemovePathList( void );

	void		_AddNextPath( INT32 pathIdx, UINT32 nextIdx );
	void		_BuildPoint( NavigationPoint * pPointTable );
	void		_BuildPoint2( NavigationPoint2 * pPointTable );
	void		_BuildPath( NavigationPath * pPathTable );

	BOOL		_Import( const char * pszPath );

	UINT32		_FindStarter( NavigationPath * pPath, UINT32 PathIdx );

public:
	INT32		getPointCount( void )					{ return (INT32)m_PointList.size(); }
	NavigationPoint * getPoint( INT32 idx )				{ return m_PointList[idx]; }
	NavigationPoint2 * getPoint2( INT32 idx )			{ return (NavigationPoint2 *) m_PointList[idx]; }
	void		setPoint( INT32 idx, NavigationPoint * pPoint )			{ m_PointList[idx] = pPoint ; }
	void		setPoint2( INT32 idx, NavigationPoint2 * pPoint )		{ m_PointList[idx] = (NavigationPoint *)pPoint ; }
	void		addPoint( NavigationPoint * pPoint )	{ m_PointList.push_back( pPoint ); }
	void		addPoint2( NavigationPoint2 * pPoint )	{ m_PointList.push_back( (NavigationPoint *)pPoint ); }
	void		removePoint( NavigationPoint * pPoint )	{ i3::vu::remove(m_PointList, pPoint ); }
	void		removePoint2( NavigationPoint2 * pPoint )	{ i3::vu::remove(m_PointList, (NavigationPoint *)pPoint ); }

	INT32		getPathCount( void )					{ return (INT32)m_PathList.size(); }
	void		setPathCount( INT32 nCount )			{ m_PathList.resize( nCount ); }

	NavigationPath * getPath( INT32 idx )				{ return m_PathList[idx]; }
	void		setPath( INT32 idx, NavigationPath * pPath )	{ m_PathList[idx] = pPath; }
	void		addPath( NavigationPath * pPath )		{ m_PathList.push_back( pPath ); }
	void		removePath( NavigationPath * pPath )	{ i3::vu::remove(m_PathList, pPath ); }

	NavigationPoint * getPointWithPath( INT32 pathIdx, INT32 pointIdx )
	{
		I3ASSERT( getPathCount() > pathIdx );
		I3ASSERT( getPath( pathIdx )->m_nPointCount > pointIdx );
		return getPoint( getPath( pathIdx )->m_pPointIndices[ pointIdx ] );
	}

	NavigationPoint2 * getPoint2WithPath( INT32 pathIdx, INT32 pointIdx )
	{
		I3ASSERT( getPathCount() > pathIdx );
		I3ASSERT( getPath( pathIdx )->m_nPointCount > pointIdx );
		return getPoint2( getPath( pathIdx )->m_pPointIndices[ pointIdx ] );
	}

public:
	CAI_NavigationPoint();
	virtual ~CAI_NavigationPoint();

	void		Create( const char * pszPath );

	// 패스리스트에서 끝점이 이어지는 패스를 만든다.
	void		CalcEndToStart();

	VEC3D *		GetPosWithPath( UINT32 path, UINT32 point );
	REAL32		GetPointRangeWithPath( UINT32 path, UINT32 point );
	VEC3D *		GetRandPosWithPath( VEC3D * pStartPos, VEC3D * pOut, UINT32 path, UINT32 point, INT32 Sequential );
	REAL32		GetPointThetaWithPath( UINT32 path, UINT32 point );

	BOOL		RunSequentialPath( UINT32 * pPathIdx, UINT32 * pPointIdx, INT32 nSequential );

};

#endif // __AI_NAVIGATIONPOINT_H
