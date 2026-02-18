#include "i3SceneDef.h"
#include "i3NavigationMesh.h"
#include "i3NavigationMeshNode.h"
#include "i3NavigationMeshPathFinder.h"

#include "stdio.h"

I3_CLASS_INSTANCE( i3NavigationMesh);

i3NavigationMesh::~i3NavigationMesh()
{
	I3_SAFE_RELEASE( m_pProgressCtrl );
	I3_SAFE_RELEASE( m_pPos );
	I3MEM_SAFE_FREE_POINTER( m_pTri );
	I3MEM_SAFE_FREE_POINTER( m_pMiddlePos );
	I3MEM_SAFE_FREE_POINTER( m_pNavInfo );

}

void i3NavigationMesh::SetPositionCount( I3NM_INDEX count)
{
	if( m_pPos == nullptr)
	{
		m_pPos = i3Vector2Array::new_object();
	}

	m_pPos->SetVectorCount( count);
}

void i3NavigationMesh::SetPositionArray( i3Vector2Array * pArray)
{
	I3_REF_CHANGE( m_pPos, pArray);
}

void i3NavigationMesh::SetTriangleCount( INT32 count )
{
	/*
	I3MEM_SAFE_FREE( m_pTri,		m_hTriMemId );
	I3MEM_SAFE_FREE( m_pMiddlePos,	m_hMiddlePosMemId );
	*/
	I3MEM_SAFE_FREE_POINTER( m_pTri	); 
	I3MEM_SAFE_FREE_POINTER( m_pMiddlePos ); 

	I3ASSERT( 0 < count && count < I3_MAX_NVMESH_TRICOUNT );
	m_TriCount = (I3NM_INDEX)count;

	if( count > 0)
	{
		//m_hTriMemId = i3MemAlloc( sizeof(i3::pack::NAVMESH_TRI) * count);
		m_pTri = (i3::pack::NAVMESH_TRI *) i3MemAlloc( sizeof(i3::pack::NAVMESH_TRI) * count);
		I3ASSERT( m_pTri);

		//m_hMiddlePosMemId = i3MemAlloc( sizeof(VEC2D) * count );
		m_pMiddlePos = (VEC2D *) i3MemAlloc( sizeof(VEC2D) * count );
		I3ASSERT( m_pMiddlePos);
	}
}

void i3NavigationMesh::Transform( MATRIX * pMatrix, i3NavigationMesh * pMesh )
{
	UINT32 i;
	i3NavigationMesh * pDest = pMesh;

	I3ASSERT( i3::same_of<i3NavigationMesh*>(pMesh));

	// Transform Positions
	if( m_pPos != nullptr)
	{
		VEC2D * pSrcVec, * pDestVec;

		pSrcVec = GetPositionArray()->GetVectorArray();
		pDestVec = pDest->GetPositionArray()->GetVectorArray();

		for( i = 0; i < m_pPos->GetVectorCount(); i++)
		{
			i3Vector::TransformCoord( pDestVec, pSrcVec, pMatrix);
			pSrcVec++;
			pDestVec++;
		}
	}
}

inline bool IsMemberOfTri( i3::pack::NAVMESH_TRI &Tri, UINT16 nIndex )
{
	if( Tri.nVertex[0] != nIndex && Tri.nVertex[1] != nIndex && Tri.nVertex[2] != nIndex )
	{
		return false;
	}

	return true;
}

void i3NavigationMesh::SetProgressCtrl( i3ToolProgressCtrl *pCtrl )
{
	I3_REF_CHANGE( m_pProgressCtrl, pCtrl);
}

void i3NavigationMesh::SetProgressRange( INT32 nRange )
{
	if( m_pProgressCtrl )
	{
		m_pProgressCtrl->SetRange( nRange );
	}
}

void i3NavigationMesh::SetProgressPos( INT32 nPos )
{
	if( m_pProgressCtrl )
	{
		m_pProgressCtrl->SetPos( nPos );
	}
}

// 이웃하는 삼각형 인덱스 찾아서 저장하고, 중점 좌표 계산 해서 보관.
bool i3NavigationMesh::BuildGraphInfo( bool *pError )
{
	I3TRACE( "i3NavigationMesh::BuildGraphInfo()\n" );

	bool bRet = true;
	bool bDupTri = false;
	bool bDupNeighbor = false;
	bool bNoNeighbor = false;
	I3NM_INDEX i, k;

	if( pError != nullptr ) memset( pError, 0, sizeof(bool) * m_TriCount );

	SetProgressRange( m_TriCount );
	SetProgressPos( 0 );
	for( i = 0; i < m_TriCount; i++ )
	{
		SetProgressPos( i );
		m_pTri[i].nNeighbor[0] = I3_NVMESH_INDEX_NONE;	// 초기화. 연결된 삼각형이 없음.
		m_pTri[i].nNeighbor[1] = I3_NVMESH_INDEX_NONE;	// 초기화. 연결된 삼각형이 없음.
		m_pTri[i].nNeighbor[2] = I3_NVMESH_INDEX_NONE;	// 초기화. 연결된 삼각형이 없음.

		for( k = 0; k < m_TriCount; k++ )
		{
			if( i == k ) continue;

			bool bMember[3];

			bMember[0] = IsMemberOfTri( m_pTri[k], m_pTri[i].nVertex[0] );
			bMember[1] = IsMemberOfTri( m_pTri[k], m_pTri[i].nVertex[1] );
			bMember[2] = IsMemberOfTri( m_pTri[k], m_pTri[i].nVertex[2] );

			if( bMember[0] && bMember[1] && bMember[2] )
			{
				bDupTri = true;
				if( pError != nullptr ) pError[i] = true;
				bRet = false;
			}
			else
			{
				INT32 m, n;
				for( m = 0; m < 3; m++ )
				{
					n = ( m + 1 ) % 3;

					if( bMember[m] && bMember[n] )
					{
						if( m_pTri[i].nNeighbor[m] != I3_NVMESH_INDEX_NONE )
						{
							bDupNeighbor = true;
							if( pError != nullptr ) pError[i] = true;
							bRet = false;
						}
						else
						{
							m_pTri[i].nNeighbor[m] = k;
						}
					}
				}
			}
		}

		if(	m_pTri[i].nNeighbor[0] == I3_NVMESH_INDEX_NONE &&
			m_pTri[i].nNeighbor[1] == I3_NVMESH_INDEX_NONE &&
			m_pTri[i].nNeighbor[2] == I3_NVMESH_INDEX_NONE )
		{
			bNoNeighbor = true;
			if( pError != nullptr ) pError[i] = true;
			bRet = false;
		}

		// 중점좌표 계산
		i3Vector::Scale(	&m_pMiddlePos[i],	m_pPos->GetVector( m_pTri[i].nVertex[0] ), 0.333333f );
		i3Vector::ScaleAdd( &m_pMiddlePos[i],	m_pPos->GetVector( m_pTri[i].nVertex[1] ), 0.333333f );
		i3Vector::ScaleAdd( &m_pMiddlePos[i],	m_pPos->GetVector( m_pTri[i].nVertex[2] ), 0.333333f );

#if 0
		I3TRACE( "%d : ( Vertex : %3d, %3d, %3d ), ( Center : %f, %f )\n",
			i, m_pTri[i].nVertex[0], m_pTri[i].nVertex[1], m_pTri[i].nVertex[2],
			i3Vector::GetX( &m_pTri[i].vecMiddle ), i3Vector::GetY( &m_pTri[i].vecMiddle ) );
#endif
	}

	if( bDupTri )
	{
		i3Log( "i3NavigationMesh::BuildGarphInfo()", "Duplicated Triangle is found." );
	}

	if( bDupNeighbor )
	{
		i3Log( "i3NavigationMesh::BuildGarphInfo()", "Duplicated Neighbor." );
	}

	if( bNoNeighbor )
	{
		i3Log( "i3NavigationMesh::BuildGarphInfo()", "Neighbor is not found." );
	}

	return bRet;
}

void i3NavigationMesh::RecCheckNeighBorConnection( I3NM_INDEX nIndex, bool *pChecked, INT32 &nCount )
{
	pChecked[ nIndex ] = true;

	INT32 i;
	for( i = 0; i < 3; i++ )
	{
		I3NM_INDEX nNeighbor = m_pTri[ nIndex ].nNeighbor[ i ];
		if( nNeighbor != I3_NVMESH_INDEX_NONE )
		{
			if( ! pChecked[ nNeighbor ] )
			{
				nCount++;
				SetProgressPos( nCount );
				RecCheckNeighBorConnection( nNeighbor, pChecked, nCount );
			}
		}
	}
}

// 모든 삼각형이 서로 연결되어 있는지 여부 검사
// 이 함수는 툴에서만 사용함.
bool i3NavigationMesh::CheckNeighborConnection( bool *pChecked )
{
	bool bRet = false;

	memset( pChecked, 0, m_TriCount * sizeof(bool) );

	SetProgressRange( m_TriCount );
	SetProgressPos( 0 );

	INT32 nCount = 0;
	RecCheckNeighBorConnection( 0, pChecked, nCount );

	UINT32 i;
	for( i = 0; i < m_TriCount; i++ )
	{
		if( ! pChecked[i] )
		{
			goto ExitLabel;
		}
	}

	bRet = true;
ExitLabel:

	return bRet;
}

// 길찾기 정보 설정
void i3NavigationMesh::SetPathDir( I3NM_INDEX nStart, I3NM_INDEX nDest, I3NM_DIR nDir )
{
	I3ASSERT( nDir < I3_NVMESH_DIR_NONE );

	I3NM_DIR nPrevValue = GetPathDir( nStart, nDest );
	if( nPrevValue != I3_NVMESH_DIR_NONE )
	{
		if( nPrevValue != nDir )
		{
			I3TRACE( "WARN : %d -> %d, nPrev = %d, nCur = %d\n", nStart, nDest, nPrevValue, nDir );
		}
	}

	INT32 nBitIndex = ( ( nDest * m_TriCount ) + nStart ) << 1;
	INT32 nByteIndex = nBitIndex >> 3;	// nBitIndex / 8
	INT32 nByteShift = nBitIndex & 7;	// nBitIndex % 8
	I3NM_DIR nEx = (I3NM_DIR)( ~( I3NM_DIR(I3_NVMESH_DIR_NONE) << nByteShift ) );
	m_pNavInfo[ nByteIndex ] &= nEx;
	m_pNavInfo[ nByteIndex ] |= nDir << nByteShift;
}

// 길찾기 정보 얻기
I3NM_DIR i3NavigationMesh::GetPathDir( I3NM_INDEX nStart, I3NM_INDEX nDest )
{
	INT32 nBitIndex = ( ( nDest * m_TriCount ) + nStart ) << 1;
	INT32 nByteIndex = nBitIndex >> 3;	// nBitIndex / 8
	INT32 nByteShift = nBitIndex & 7;	// nBitIndex % 8
	return I3NM_DIR( ( m_pNavInfo[ nByteIndex ] >> nByteShift ) & 3 );
}


// 길찾기 정보 테이블 작성
bool i3NavigationMesh::BuildNavigationInfo()
{
	I3TRACE( "i3NavigationMesh::BuildNavigationInfo]\n" );

	I3ASSERT( m_pNavInfo == nullptr );
	I3ASSERT( m_TriCount > 0 );

	m_NavInfoSize = ( m_TriCount * m_TriCount ) >> 2;

	//m_hNavInfoMemId = i3MemAlloc( m_NavInfoSize );
	m_pNavInfo = (UINT8*) i3MemAlloc( m_NavInfoSize );
	memset( m_pNavInfo, 0xFFFFFFFF, m_NavInfoSize );

	i3NavigationMeshPathFinder *pFinder = i3NavigationMeshPathFinder::new_object();

	SetProgressRange( m_TriCount );
	SetProgressPos( 0 );

	I3NM_INDEX i, j, k;
	for( i = 0; i < m_TriCount; i++ )
	{
		SetProgressPos( 0 );
		for( j = 0; j < m_TriCount; j++ )
		{
			SetProgressPos( j );
			if( i == j ) continue;

			//I3TRACE( "Start : %d, End : %d\n", i, j );

			pFinder->Create( this, i, j, 1.0f );
			if( ! pFinder->Run() )
			{
				i3::pack::NAVMESH_TRI *pStart = GetTriangle( i );
				i3::pack::NAVMESH_TRI *pEnd = GetTriangle( j );
				VEC2D *vecStart[3];
				VEC2D *vecEnd[3];
				vecStart[0] = GetPosition( pStart->nVertex[0] );
				vecStart[1] = GetPosition( pStart->nVertex[1] );
				vecStart[2] = GetPosition( pStart->nVertex[2] );
				vecEnd[0] = GetPosition( pEnd->nVertex[0] );
				vecEnd[1] = GetPosition( pEnd->nVertex[1] );
				vecEnd[2] = GetPosition( pEnd->nVertex[2] );
				i3Log( "i3NavigationMesh::BuildNavigationInfo",
					"\r\nCould not Found Path. (%d->%d)"
					"\r\nStart Mesh Vertex ( %9.3f, %9.3f ), ( %9.3f, %9.3f ), ( %9.3f, %9.3f )"
					"\r\nGoal Mesh Vertex ( %9.3f, %9.3f ), ( %9.3f, %9.3f ), ( %9.3f, %9.3f )",
						i, j,
						i3Vector::GetX( vecStart[0] ), i3Vector::GetY( vecStart[0] ),
						i3Vector::GetX( vecStart[1] ), i3Vector::GetY( vecStart[1] ),
						i3Vector::GetX( vecStart[2] ), i3Vector::GetY( vecStart[2] ),
						i3Vector::GetX( vecEnd[0] ), i3Vector::GetY( vecEnd[0] ),
						i3Vector::GetX( vecEnd[1] ), i3Vector::GetY( vecEnd[1] ),
						i3Vector::GetX( vecEnd[2] ), i3Vector::GetY( vecEnd[2] )
					);

				return false;
			}
			const i3::vector<i3NvMeshPathNode*>& Solution = pFinder->GetSoultion();
			INT32 nCount = (INT32)Solution.size();
			I3ASSERT( nCount >= 2 );

			I3NM_INDEX nPrevIndex, nCurIndex;
			I3NM_DIR nTriDir;
			i3NvMeshPathNode *pNode;

			// 첫번째 노드는 종점 노드이므로 결과값을 저장하지 않는다.
			pNode = Solution[0];
			nCurIndex = pNode->m_nIndex;
			I3ASSERT( j == nCurIndex );
			nPrevIndex = nCurIndex;

			// 두번째 노드 부터 값을 저장한다.
			for( k = 1; k < nCount; k++ )
			{
				pNode = Solution[k];
				nCurIndex = pNode->m_nIndex;

				nTriDir = GetNeighborDirFromTriIndex( nCurIndex, nPrevIndex );
				I3ASSERT( nTriDir != I3_NVMESH_DIR_NONE );

				SetPathDir( nCurIndex, j, nTriDir );

				// Test
				#if defined( I3_DEBUG)
				I3NM_DIR nTest = GetPathDir( nCurIndex, j );
				I3ASSERT( nTest == nTriDir );
				#endif

				nPrevIndex = nCurIndex;
			}

			// 마지막 노드 검사
			I3ASSERT( nCurIndex == i );
		}
	}

	I3_SAFE_RELEASE( pFinder );

#if 1
	I3TRACE( "Check Navigation Direction Info\n" );
	for( i = 0; i < m_TriCount; i++ )
	{
		for( j = 0; j < m_TriCount; j++ )
		{
			if( i == j ) continue;

			I3NM_DIR nDir = GetPathDir( i, j );
			if( nDir == I3_NVMESH_DIR_NONE )
			{
				I3TRACE( "%3d -> %3d : Direction Value is not specified.", i, j );
			}
			else
			{
				INT32 nCheckCount = 0;

				// 이웃의 인덱스를 얻어와서...
				I3NM_INDEX nIndex = GetNeighborIndex( i, nDir );
				while( nIndex != j )
				{	// 그 이웃이 종점이 아니라면...

					// 그 다음 이웃을 구함
					nDir = GetPathDir( nIndex, j );
					nIndex = GetNeighborIndex( nIndex, nDir );

					// 만약 시작점으로 돌아 왔다면, 에러!!!
					if( nIndex == i )
					{
						i3Log( "i3NavigationMesh::BuildNavigationInfo", "ERROR : Endless Track Found!!!. Return to Begin Point." );
						I3ASSERT_0;
						return false;
					}

					nCheckCount++;
					if( nCheckCount > I3_MAX_NVMESH_TRICOUNT )
					{
						i3Log( "i3NavigationMesh::BuildNavigationInfo", "ERROR : Endless Track Found!!!." );
						I3ASSERT_0;
						return false;
					}
				}
			}
		}
	}
	I3TRACE( "Done.\n" );
#endif

	return true;
}

bool	i3NavigationMesh::CreateSearchTree()
{
	I3ASSERT( m_pRoot == nullptr );
	m_pRoot = i3NavigationMeshNode::new_object();

	i3::vector<i3NavigationMeshNode*> LeafList;

	i3BoundBox *pRootBox = i3BoundBox::new_object();

	I3NM_INDEX i;
	// Leaf 노드 생성.
	// 바운딩 박스 구하기.
	SetProgressRange( m_TriCount );
	SetProgressPos( 0 );
	for( i = 0; i < m_TriCount; i++ )
	{
		SetProgressPos( i );
		i3NavigationMeshNode *pNode = i3NavigationMeshNode::new_object();
        
		pNode->SetTriagleIndex( i );
        
		i3::pack::NAVMESH_TRI * pTri = &m_pTri[i];

		i3BoundBox *pBox = i3BoundBox::new_object();

		#if defined( I3_FIXED_POINT)
			VEC2D vec;

			i3Vector::Copy( &vec, GetPosition( pTri->nVertex[0]));
			pBox->ExtendByVec( &vec);

			i3Vector::Copy( &vec, GetPosition( pTri->nVertex[1]));
			pBox->ExtendByVec( &vec);

			i3Vector::Copy( &vec, GetPosition( pTri->nVertex[2]));
			pBox->ExtendByVec( &vec);
		#else
			pBox->ExtendByVec( GetPosition( pTri->nVertex[0] ) );
			pBox->ExtendByVec( GetPosition( pTri->nVertex[1] ) );
			pBox->ExtendByVec( GetPosition( pTri->nVertex[2] ) );
		#endif

		pNode->SetBound( pBox );

		pRootBox->ExtendByBoundBox( pBox );

		LeafList.push_back( pNode );

		//I3TRACE( "%3d : ( %9.2f, %9.2f ) ~ ( %9.2f, %9.2f )\n", i, i3Vector::GetX( pBox->GetMin() ), i3Vector::GetY( pBox->GetMin() ), i3Vector::GetX( pBox->GetMax() ), i3Vector::GetY( pBox->GetMax() ) );
		//I3TRACE( "        %9.2f, %9.2f\n", i3Vector::GetX( GetPosition( pTri->nVertex[0] ) ), i3Vector::GetY( GetPosition( pTri->nVertex[0] ) ) );
		//I3TRACE( "        %9.2f, %9.2f\n", i3Vector::GetX( GetPosition( pTri->nVertex[1] ) ), i3Vector::GetY( GetPosition( pTri->nVertex[1] ) ) );
		//I3TRACE( "        %9.2f, %9.2f\n", i3Vector::GetX( GetPosition( pTri->nVertex[2] ) ), i3Vector::GetY( GetPosition( pTri->nVertex[2] ) ) );
	}

	m_pRoot->SetBound( pRootBox ); 

	SetProgressRange( m_TriCount );
	SetProgressPos( 0 );
	for( i = 0; i < m_TriCount; i++ )
	{
		SetProgressPos( i );
		i3NavigationMeshNode *pNode = LeafList[i];
		m_pRoot->AddNvMeshNodeLeaf( pNode );
	}

	AddChild( m_pRoot );

	return true;
}

I3NM_INDEX i3NavigationMesh::GetMeshIndex( VEC2D *pPos )
{
	I3ASSERT( m_pRoot != nullptr );
	return m_pRoot->GetMeshIndex( this, pPos );
}

// pTargetPos	(Out)	: 결과값. (최종/임시 이동 목표)
// nCount		(In)	: pTargetPos의 버퍼 크기
// pStartPos	(In)	: 현재 위치
// pEndPos		(In)	: 최종 목표 위치
// rNearDistance(In)	: 목표와의 거리에 대한 허용 오차
INT32 i3NavigationMesh::GetPathDirection( VEC2D *pTargetPos, INT32 nCount, VEC2D *pStartPos, VEC2D *pEndPos )
{
	I3ASSERT( nCount > 0 );

	I3NM_INDEX nStartIndex, nCurIndex, nEndIndex;
	VEC2D *pOut = &pTargetPos[0];

	nStartIndex = GetMeshIndex( pStartPos );
	if( nStartIndex == I3_NVMESH_INDEX_NONE )
	{
		//I3TRACE( "Failed to find Navigation Mesh Start Index.\n" );
		return -1;
	}

	nEndIndex = GetMeshIndex( pEndPos );
	if( nEndIndex == I3_NVMESH_INDEX_NONE )
	{
		//I3TRACE( "Failed to find Navigation Mesh End Index.\n" );
		return -1;
	}

	// 같은 메쉬안에 있을 경우 바로 이동.
	if( nStartIndex == nEndIndex )
	{
		return 0;
	}

	nCurIndex = nStartIndex;

	INT32 i;

	for( i = 0; i < nCount; i++ )
	{
		if( nCurIndex == nEndIndex )
		{
			// 목표점에 도달했으면 중단
			break;
		}

		// 진행 방향 얻기
		I3NM_DIR nDir;
		nDir = GetPathDir( nCurIndex, nEndIndex );
		if( nDir == I3_NVMESH_DIR_NONE )
		{
			//I3TRACE( "Failed to find Navigation Direction.\n" );
			return -1;
		}

		// 이웃 삼각형과 공유하는 면의 중점을 구한다.
		VEC2D vecSideMiddlePos;
		VEC2D *pSideVertex[2];

		pSideVertex[0] = m_pPos->GetVector( m_pTri[ nCurIndex ].nVertex[ nDir ] );
		pSideVertex[1] = m_pPos->GetVector( m_pTri[ nCurIndex ].nVertex[ ( nDir + 1 ) % 3 ] );

		i3Vector::Add( &vecSideMiddlePos, pSideVertex[0], pSideVertex[1] );
		i3Vector::Scale( &vecSideMiddlePos, &vecSideMiddlePos, 0.5f );

		// 이웃 삼각형과 공유하는 면의 중점과 시작 좌표간의 거리를 계산한다.
		VEC2D vecDir;
		REAL32 rDistanceSq;
		i3Vector::Sub( &vecDir, &vecSideMiddlePos, pStartPos );
		rDistanceSq = i3Vector::LengthSq( &vecDir );

		// 공유면의 중점
		i3Vector::Copy( pOut, &vecSideMiddlePos );

		// 다음점 얻기
		nCurIndex = GetNeighborIndex( nCurIndex, nDir );
		pOut++;
	}

	return i;
}

bool i3NavigationMesh::CheckTriContain( I3NM_INDEX nIndex, VEC2D *pPos )
{
	I3ASSERT( nIndex != I3_NVMESH_INDEX_NONE );

	VEC3D vecLine, vecDir;
	VEC2D *pVecPos2D[3];
	VEC3D vecPos[3];

	i3Vector::Set( &vecLine, i3Vector::GetX( pPos ), -1.0f, i3Vector::GetY( pPos ) );
	i3Vector::Set( &vecDir, 0.0f, 2.0f, 0.0f );

	pVecPos2D[0] = m_pPos->GetVector( m_pTri[nIndex].nVertex[0] );
	pVecPos2D[1] = m_pPos->GetVector( m_pTri[nIndex].nVertex[1] );
	pVecPos2D[2] = m_pPos->GetVector( m_pTri[nIndex].nVertex[2] );

	i3Vector::Set( &vecPos[0], i3Vector::GetX( pVecPos2D[0] ), 0.0f, i3Vector::GetY( pVecPos2D[0] ) );
	i3Vector::Set( &vecPos[1], i3Vector::GetX( pVecPos2D[1] ), 0.0f, i3Vector::GetY( pVecPos2D[1] ) );
	i3Vector::Set( &vecPos[2], i3Vector::GetX( pVecPos2D[2] ), 0.0f, i3Vector::GetY( pVecPos2D[2] ) );

	return i3Math::TestTriPoint( &vecPos[0], &vecPos[1], &vecPos[2], &vecLine, &vecDir );
}

void i3NavigationMesh::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
    if( m_pPos != nullptr )
	{
		m_pPos->OnBuildObjectList( List );
	}

	//if( m_pRoot != nullptr )
	//{
	//	m_pRoot->OnBuildObjectList( pList );
	//}

	i3Node::OnBuildObjectList( List );
}

#define I3_NVMESH_VERSION	2

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 NVMESH
	{
		UINT8				m_nVersion = I3_NVMESH_VERSION;
		OBJREF				m_RefPos = 0;
		I3NM_INDEX			m_TriCount = 0;
		OBJREF				m_RefRoot = 0;
		INT32				m_NavInfoSize = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3NavigationMesh::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::NVMESH Data;

	// Node
	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Data.m_RefPos		= pResFile->GetObjectPersistID( m_pPos );
	Data.m_TriCount		= m_TriCount;
	Data.m_RefRoot		= pResFile->GetObjectPersistID( m_pRoot );
	Data.m_NavInfoSize	= m_NavInfoSize;

	Rc = pStream->Write( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NavigationMesh::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	Rc = pStream->Write( m_pTri, sizeof(i3::pack::NAVMESH_TRI) * m_TriCount );
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NavigationMesh::OnSave()", "Could not write m_pTri.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( m_NavInfoSize > 0 )
	{
		Rc = pStream->Write( m_pNavInfo, m_NavInfoSize );
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3NavigationMesh::OnSave()", "Could not write m_pNavInfo.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3NavigationMesh::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3NavigationMesh::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::NVMESH Data;

	// node
	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NavigationMesh::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( Data.m_nVersion != I3_NVMESH_VERSION )
	{
		i3Log( "i3NavigationMesh::OnLoad()", "Invalid version." );
		return STREAM_ERR;
	}
	m_pPos				= (i3Vector2Array*)pResFile->FindObjectByID( Data.m_RefPos );
	I3_SAFE_ADDREF( m_pPos);
	m_TriCount			= Data.m_TriCount;
	m_pRoot				= (i3NavigationMeshNode*)pResFile->FindObjectByID( Data.m_RefRoot );
	m_NavInfoSize		= Data.m_NavInfoSize;

	{
		I3MEM_SAFE_FREE_POINTER( m_pTri ); 
		if( m_TriCount > 0)
		{
			m_pTri = (i3::pack::NAVMESH_TRI *) i3MemAlloc( sizeof(i3::pack::NAVMESH_TRI) * m_TriCount);
			I3ASSERT( m_pTri);
		}

		Rc = pStream->Read( m_pTri, sizeof(i3::pack::NAVMESH_TRI) * m_TriCount );
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3NavigationMesh::OnLoad()", "Could not read m_pTri.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	if( m_NavInfoSize > 0 )
	{
		I3MEM_SAFE_FREE_POINTER(m_pNavInfo); 
		I3ASSERT( m_NavInfoSize == ( m_TriCount * m_TriCount ) >> 2 );
		
		m_pNavInfo = (UINT8*)i3MemAlloc( m_NavInfoSize );

		Rc = pStream->Read( m_pNavInfo, m_NavInfoSize );
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3NavigationMesh::OnLoad()", "Could not read m_pNavInfo.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}
