#ifndef __I3_NAVIGATION_MESH_H__
#define __I3_NAVIGATION_MESH_H__

#include "i3Node.h"

typedef UINT16	I3NM_INDEX;
typedef	UINT8	I3NM_DIR;

#define I3_MAX_NVMESH_TRICOUNT	0xFFFF	// 삼각형 최대 개수. 65535개 (0 ~ 65534 )
#define	I3_NVMESH_INDEX_NONE	0xFFFF	// 존재하지 않는 삼각형 인덱스를 나타냄. (65535)

#define	I3_MAX_NVMESH_DIR		3		// 최대 이웃의 개수
#define	I3_NVMESH_DIR_NONE		3		// 존재하지 않는 길찾기 방향을 나타냄. 0b11

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

typedef struct PACKED _tag_I3_NAVMESH_TRI
{
	UINT16		nVertex[3];
	I3NM_INDEX	nNeighbor[I3_MAX_NVMESH_DIR];
} I3_NAVMESH_TRI;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

class i3NavigationMeshNode;
class i3ToolProgressCtrl;

class I3_EXPORT_SCENE i3NavigationMesh : public i3Node
{
	I3_CLASS_DEFINE( i3NavigationMesh );
protected:
	I3_NAVMESH_TRI		*m_pTri;
	//I3HMEM				m_hTriMemId;

	VEC2D				*m_pMiddlePos;
	//I3HMEM				m_hMiddlePosMemId;

	i3Vector2Array		*m_pPos;

	I3NM_INDEX			m_TriCount;

    UINT8				*m_pNavInfo;
	//I3HMEM				m_hNavInfoMemId;
	INT32				m_NavInfoSize;

	i3NavigationMeshNode	*m_pRoot;

	i3ToolProgressCtrl		*m_pProgressCtrl;

public:
	i3NavigationMesh();
	virtual ~i3NavigationMesh();

	i3Vector2Array *	GetPositionArray(void)			{ return m_pPos; }
	UINT32				GetPositionCount(void)			{ return m_pPos->GetVectorCount(); }
	VEC2D *				GetPosition( INT32 idx)			{ return m_pPos->GetVector( idx); }
	void				SetPositionCount( I3NM_INDEX count);
	void				SetPositionArray( i3Vector2Array * pArray);

	I3_NAVMESH_TRI *	GetTriangleArray(void)			{ return m_pTri; }
	I3_NAVMESH_TRI *	GetTriangle( INT32 idx)			{ return m_pTri + idx; }
	I3NM_INDEX			GetTriangleCount(void)			{ return m_TriCount; }
	void				SetTriangleCount( INT32 count);
	void				SetTriangle( INT32 idx, INT32 vidx1, INT32 vidx2, INT32 vidx3, INT32 normidx)
	{
		m_pTri[idx].nVertex[0] = (UINT16) vidx1;
		m_pTri[idx].nVertex[1] = (UINT16) vidx2;
		m_pTri[idx].nVertex[2] = (UINT16) vidx3;
	}
	void				SetTriangle( INT32 idx, I3_NAVMESH_TRI * pTri)
	{
		m_pTri[idx].nVertex[0] = (UINT16) pTri->nVertex[0];
		m_pTri[idx].nVertex[1] = (UINT16) pTri->nVertex[1];
		m_pTri[idx].nVertex[2] = (UINT16) pTri->nVertex[2];
	}

	VEC2D*			GetTriMiddlePos( UINT16 nIndex )
	{
		if( m_pMiddlePos == NULL ) return NULL;
		I3ASSERT( nIndex < m_TriCount );
		return &m_pMiddlePos[ nIndex ];
	}

	virtual void	Transform( MATRIX * pMatrix, i3NavigationMesh * pDest);

	// 길찾기 정보 설정
	// nStart : 시작점
	// nDest  : 목표점
	void			SetPathDir( I3NM_INDEX nStart, I3NM_INDEX nDest, I3NM_DIR nDir );

	// 길찾기 방향 얻기
	// nStart : 시작점
	// nDest  : 목표점
	I3NM_DIR		GetPathDir( I3NM_INDEX nStart, I3NM_INDEX nDest );

	// 이웃의 방향 얻기
	I3NM_DIR		GetNeighborDirFromTriIndex( I3NM_INDEX nIndex, I3NM_INDEX nFind )
	{
		I3NM_DIR i;
		for( i = 0; i < I3_MAX_NVMESH_DIR; i++ )
		{
			if( m_pTri[ nIndex ].nNeighbor[ i ] == nFind )
			{
				return i;
			}
		}
		return I3_NVMESH_DIR_NONE;
	}

	// 이웃한 메쉬인지 여부
	BOOL			IsNeighbor( I3NM_INDEX nIndex, I3NM_INDEX nNeighbor )
	{
		I3ASSERT( nIndex != I3_NVMESH_INDEX_NONE );
		I3ASSERT( nNeighbor != I3_NVMESH_INDEX_NONE );
		I3NM_DIR i;
		for( i = 0; i < I3_MAX_NVMESH_DIR; i ++ )
		{
			if( m_pTri[ nIndex ].nNeighbor[ i ] == nNeighbor )
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	// 방향으로 이웃의 인덱스 얻기
	I3NM_INDEX		GetNeighborIndex( I3NM_INDEX nIndex, I3NM_DIR nDir )
	{
		//I3ASSERT( m_pTri != NULL && nIndex < m_TriCount );
		//I3ASSERT( 0 <= nDir && nDir < I3_MAX_NVMESH_DIR );
		return m_pTri[ nIndex ].nNeighbor[ nDir ];
	}

	// 방향으로 이웃 얻기
	I3_NAVMESH_TRI*	GetNeighbor( I3NM_INDEX nIndex, I3NM_DIR nDir )
	{
		//I3ASSERT( m_pTri != NULL && nIndex < m_TriCount );
		//I3ASSERT( 0 <= nCount && nCount < I3_MAX_NVMESH_DIR );
		I3NM_INDEX nNeborIndex = m_pTri[ nIndex ].nNeighbor[ nDir ];
		if( nNeborIndex == I3_NVMESH_INDEX_NONE )
		{
			return NULL;
		}
		I3ASSERT( nNeborIndex < m_TriCount );
		return &m_pTri[nNeborIndex];
	}

	// 주어진 좌표에 해당되는 메쉬의 인덱스 얻기
	I3NM_INDEX		GetMeshIndex( VEC2D *pPos );

	// 현재 좌표와 목표 좌표가 주어졌을 때, 이동에 필요한 중계 좌표 목록 얻기
	// pTargetPos	(Out)	: 결과값. (최종/임시 이동 목표)
	// nCount		(In)	: pTargetPos의 버퍼 크기
	// pStartPos	(In)	: 현재 위치
	// pEndPos		(In)	: 최종 목표 위치
	// rNearDistance(In)	: 목표와의 거리에 대한 허용 오차
	// Return	: -1 ( 갈 수 없음.), 0 ( 바로 갈 수 있음.), 양수 ( 돌아서 갈 수 있음. 중계포지션 개수 )
	INT32			GetPathDirection( VEC2D *pTargetPos, INT32 nCount, VEC2D *pStartPos, VEC2D *pEndPos );

	// 지정된 인덱스에 해당되는 메쉬가 주어진 좌표값을 포함하는 지 여부
	BOOL			CheckTriContain( I3NM_INDEX nIndex, VEC2D *pPos );

	void			RecCheckNeighBorConnection( I3NM_INDEX nIndex, BOOL *pChecked, INT32 &nCount );
	BOOL			CheckNeighborConnection( BOOL *pChecked );

	void			SetProgressCtrl( i3ToolProgressCtrl *pCtrl );
	void			SetProgressRange( INT32 nRange );
	void			SetProgressPos( INT32 nPos );

	BOOL			BuildGraphInfo( BOOL *pError = NULL );
	BOOL			BuildNavigationInfo();
	BOOL			CreateSearchTree();

	virtual void OnBuildObjectList( i3List * pList );
	virtual UINT32 OnSave( i3ResourceFile * pResFile );
	virtual UINT32 OnLoad( i3ResourceFile * pResFile );
};

#endif //__I3_NAVIGATION_MESH_H__
