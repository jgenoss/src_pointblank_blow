#pragma once

#include "i3Base/itl/map.h"
#include "i3Base/itl/set.h"
#include "i3Base/itl/algorithm/sort.h"
#include "i3Base/string/i3string.h"


//  [7/12/2012 dosun.lee]
//	추후 지워야 하는 소스. 디버깅이 안되서 쓰는 중....
//#define DSLEE_CUSTOM

//#if	defined( DSLEE_CUSTOM )
//#	if defined( _DEBUG )
//	
//	#include <set>
//	#include <map>
//	#include <vector>
//	#include <algorithm>
//
//		namespace ts = std;
//#	else
//		namespace ts = i3;
//#	endif
//#else
//	namespace ts = i3;
//#endif

#define NAVMESH_PRIMITIVE_FAR 80.0f

enum SELECT_NAVMESH_TYPE
{
	SEL_NAVMESH_EXCLUSIVE = 0,
	SEL_NAVMESH_INCLUSIVE,
	SEL_NAVMESH_ALTERNATIVE,
	SEL_NAVMESH_UNSELECT,
};

enum NAVMESH_GENERATE_TYPE
{
	NAVMESH_GENERATE_TRANSFORM2 = 0,
	NAVMESH_GENERATE_GEOMETRY,

	MAX_NAVMESH_GENERATE_TYPE,
};

struct I3_EXPORT_TDK navMeshConfig
{
	NAVMESH_GENERATE_TYPE	genType;

	float	cellSize;				// (float)	min = 0.1f, max = 1.0f
	float	cellHeight;				// (float)	min = 0.1f, max = 1.0f
	float	edgeMaxLen;				// (int)	min = 0,	max = 50
	float	edgeMaxError;			// (float)	min = 0.1f,	max = 3.0f
	float	regionMinSize;			// (int)	min = 0,	max = 150
	float	regionMergeSize;		// (int)	min = 0,	max = 150
	float	detailSampleDist;		// (int)	min = 0,	max = 16
	float	detailSampleMaxError;	// (int)	min = 0,	max = 16

	bool	monotonePartitioning;

	void	setDefaultConfig()
	{
		genType						= NAVMESH_GENERATE_TRANSFORM2;

		this->cellSize				= 0.3f;
		this->cellHeight			= 0.2f;
		this->edgeMaxLen			= 12.0f;
		this->edgeMaxError			= 1.3f;
		this->regionMinSize			= 8.0f;
		this->regionMergeSize		= 20.0f;
		this->detailSampleDist		= 6.0f;
		this->detailSampleMaxError	= 1.0f;

		monotonePartitioning = false;
	}
};

struct navMeshVertex
{
	union 
	{
		struct { float x, y, z; };
		struct { float f[3];		};
		struct { VEC3D vec3D;		};
	};

	bool	bRemoved = false;

public:
	navMeshVertex() : x(0.0f), y(0.0f), z(0.0f) {}
	navMeshVertex(const VEC3D& v) : x(v.x), y(v.y), z(v.z) {}
	navMeshVertex(const navMeshVertex& v) : x(v.x), y(v.y), z(v.z) {}
	navMeshVertex& operator=(const navMeshVertex& v);			// C2280 오류에 의한 추가 (2015.08.31.수빈)

	bool	operator == ( const navMeshVertex& rhs ) const;
	bool	operator != ( const navMeshVertex& rhs ) const;
	bool	operator <	( const navMeshVertex& rhs ) const;
};

struct navMeshPoly
{
	union
	{
		struct { int n[3];			};
		struct { int na, nb, nc;	};
	};
	
	VEC3D	normal;
	VEC3D	axis;
	VEC3D	center;
	VEC3D	vmin, vmax;

	float	angle;

	bool	bRemoved;

public:
	navMeshPoly();
	navMeshPoly( int _a, int _b, int _c );

	bool	operator == ( const navMeshPoly& rhs ) const;
	bool	operator != ( const navMeshPoly& rhs ) const;
	bool	operator <	( const navMeshPoly& rhs ) const;
};

struct navMeshPick
{
	int		idx;
	float	t;
	VEC3D	pc;
};

struct navMeshCuttingInfo
{
	enum CUTTING_TYPE
	{		
		LINE	= 0,
		POLY,
		CUTTING_FAIL,
	};

	CUTTING_TYPE	_type = CUTTING_FAIL;

	VEC3D	cutPosition;

	int		selPoly = -1;
	int		cutVertex = -1;
	int		otherPoly = -1;
	int		otherCutVertex = -1;
	int		cutLine[2] = { -1, -1 };

	void	reset()
	{
		_type = CUTTING_FAIL;

		selPoly			= -1;
		cutVertex		= -1;
		otherPoly		= -1;
		otherCutVertex	= -1;
		cutLine[0]		= -1;
		cutLine[1]		= -1;

		i3Vector::Set( &cutPosition, 0,0,0 );
	}
};

// 네비 메쉬 제네레이터 로딩 스레드 프로그레스바 게이지를 위한 함수 포인터
typedef void (*fnAutoGenLoadingRatio)( float, const char* );

namespace I3NAVMESH
{	
	//////////////////////////////////////////////////////////////////////////
	//  [7/13/2012 dosun.lee]
	//	i3Math에 비슷한 함수가 구현 되어 있지만 네비메쉬에서 다르게 작동 해야 할 필요가 있어 추가 함

	// 선분과 선분의 교차
	I3_EXPORT_TDK bool		IntersectLineToLine( const VEC2D* pta1, const VEC2D* pta2, const VEC2D* ptb1, const VEC2D* ptb2 );

	// 선분과 선분의 교차
	I3_EXPORT_TDK bool		IntersectLineToLine( const VEC3D* p1, const VEC3D* p2, const VEC3D* p3, const VEC3D* p4, VEC3D* pr1, VEC3D* pr2 );

	// 선분과 사각형의 교차
	I3_EXPORT_TDK bool		IntersectLineToRect( const VEC2D* p1, const VEC2D* p2, const i3::pack::RECT* r );

	// 선분과 삼각형의 교차
	I3_EXPORT_TDK bool		IntersectLineToTri( const VEC3D* p1, const VEC3D* p2, const VEC3D* n, const VEC3D* p0, VEC3D* pc,REAL32 *t );

	//////////////////////////////////////////////////////////////////////////

	struct AutoCS
	{
		AutoCS( CRITICAL_SECTION* pcs ) { ::EnterCriticalSection( pcs );  m_pcs = pcs; }
		~AutoCS()						{ ::LeaveCriticalSection( m_pcs ); }

		CRITICAL_SECTION*	m_pcs;
	};

#define AUTO_CS( cs ) AutoCS _auto( &cs );
}