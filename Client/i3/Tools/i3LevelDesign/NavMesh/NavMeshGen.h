#pragma once

//#include <i3Level/Recast/Recast.h>
//
//struct s_tri
//{
//	VEC3D v[3];
//};
//
//class logContext : public rcContext
//{
//public:
//	virtual void	doLog( const rcLogCategory category, const char* msg, const int len ) 
//	{
//		category;
//		msg;
//		len;
//	}
//};

//struct navMeshConfig
//{
//	float	cellSize;				// (float)	min = 0.1f, max = 1.0f
//	float	cellHeight;				// (float)	min = 0.1f, max = 1.0f
//	float	edgeMaxLen;				// (int)	min = 0,	max = 50
//	float	edgeMaxError;			// (float)	min = 0.1f,	max = 3.0f
//	float	regionMinSize;			// (int)	min = 0,	max = 150
//	float	regionMergeSize;		// (int)	min = 0,	max = 150
//	float	detailSampleDist;		// (int)	min = 0,	max = 16
//	float	detailSampleMaxError;	// (int)	min = 0,	max = 16
//
//	
//	void	setDefaultConfig()
//	{
//		this->cellSize				= 0.3f;
//		this->cellHeight			= 0.2f;
//		this->edgeMaxLen			= 12.0f;
//		this->edgeMaxError			= 1.3f;
//		this->regionMinSize			= 8.0f;
//		this->regionMergeSize		= 20.0f;
//		this->detailSampleDist		= 6.0f;
//		this->detailSampleMaxError	= 1.0f;
//	}
//};

class CNavMeshGen
{
public:
	CNavMeshGen();
	virtual ~CNavMeshGen();

//public:
//	enum GENERATE_TYPE
//	{
//		GENERATE_BY_GEOMETRY = 0,
//		GENERATE_BY_TRANSFORM2,
//
//		MAX_GENERATE_TYPE,
//	};
//
//public:
//	int		GetWorldTriangleCount() const
//	{
//		return m_ntri;
//	}
//
//	s_tri	GetWorldTriangle( int idx )
//	{
//		s_tri ret;
//
//		for( int i = 0; i < 3; ++i )
//		{
//			int n = m_tris[ idx *3 + i ];
//
//			ret.v[ i ].x = m_verts[ n*3 + 0 ];
//			ret.v[ i ].y = m_verts[ n*3 + 1 ];
//			ret.v[ i ].z = m_verts[ n*3 + 2 ];
//		}
//
//		return ret;
//	}
//
//public:
//	rcPolyMesh*			GetPolyMesh()			{ return m_pmesh; }
//	rcPolyMeshDetail*	GetPolyDetailMesh()		{ return m_dmesh; }
//
//public:
//	bool		DoGenerate( i3World* world, const navMeshConfig& cfg );
//
//private:	
//	bool		_DoGenerate( const navMeshConfig& cfg );
//
//	bool		_CalculateVertexList_byGeometry();
//	bool		_CalculateVertexList_byTransform2();
//
//	void		_CalculateWTM( i3Node* node, RT_MATRIX& wmat );
//	void		_CleanUp();
//
//private:
//	i3World*	m_world;
//
//private:
//	GENERATE_TYPE	m_GenType;
//
//private:
//	float*		m_verts;
//	int*		m_tris;
//
//	int			m_nvert;
//	int			m_ntri;
//
//	D3DXVECTOR3			m_min;
//	D3DXVECTOR3			m_max;
//
//	// recast
//private:
//	logContext*		m_ctx;	
//
//	//
//	rcConfig				m_cfg;
//	unsigned char*			m_triareas;
//	rcHeightfield*			m_solid;
//	rcCompactHeightfield*	m_chf;
//	rcContourSet*			m_cset;
//
//	rcPolyMesh*				m_pmesh;
//	rcPolyMeshDetail*		m_dmesh;
//
//	bool	m_keepInterResults;
};