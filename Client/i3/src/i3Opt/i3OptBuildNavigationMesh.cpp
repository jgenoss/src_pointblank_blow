#include "i3OptPCH.h"
#include "i3OptBuildNavigationMesh.h"
#include "i3OptUtil.h"

I3_CLASS_INSTANCE( i3OptBuildNavigationMesh);

inline
bool CheckMesh( const VEC3D *pV1, const VEC3D *pV2 , const VEC3D *pV3 )
{
	REAL32 rDot;
	VEC3D d1;
	VEC3D d2;
	i3Vector::Sub( &d1, pV1, pV2);
	i3Vector::Sub( &d2, pV3, pV2);
	rDot = i3Math::abs( i3Vector::Dot( &d2, &d1 ) );
	rDot /= i3Vector::Length( &d1 ) * i3Vector::Length( &d2 );

	static REAL32 rComp = i3Math::cos( I3_DEG2RAD( 5.0f ) );

	if( rDot > rComp )
	{
		I3TRACE( "rDot > rComp : %f > %f\n",	rDot, rComp );
	}

	return true;
}

i3OptBuildNavigationMesh::i3OptBuildNavigationMesh(void)
{
	m_Class = CLASS_SCENEGRAPH;
}

i3OptBuildNavigationMesh::~i3OptBuildNavigationMesh(void)
{
	I3_SAFE_RELEASE( m_pProgressCtrl );
}

INT32 i3OptBuildNavigationMesh::FindPos( i3Vector3Array * pArray, VEC3D * pPos)
{
	INT32 i;

	for( i = 0; i < (INT32) pArray->GetVectorCount(); i++)
	{
		if( (i3Vector::GetX( pArray->GetVector( i)) == i3Vector::GetX( pPos)) &&
			(i3Vector::GetY( pArray->GetVector( i)) == i3Vector::GetY( pPos)) &&
			(i3Vector::GetZ( pArray->GetVector( i)) == i3Vector::GetZ( pPos)) )
		{
			return i;
		}
	}

	return -1;
}

i3NavigationMesh * i3OptBuildNavigationMesh::ConvertIndexTriList( i3IndexArray * pIA, i3VertexArray * pVA)
{
	INT32 i, PosCount, TriIndex, NormCount, TriCount;
	INT32 idx1, idx2, idx3;
	VEC3D pos1, pos2, pos3, norm;
	i3Vector3Array * pArray	= nullptr;
	i3NavigationMesh * pMesh = nullptr;
	i3::pack::NAVMESH_TRI * pTri = nullptr;

	if( pIA->GetCount() <= 0)
		return nullptr;

	if( pVA->GetCount() <= 0)
		return nullptr;

	pIA->Lock();
	pVA->Lock();

	PosCount = 0;
	NormCount = 0;
	TriCount = pIA->GetCount() / 3;

	pTri = new i3::pack::NAVMESH_TRI[ TriCount ];

	pArray = i3Vector3Array::new_object();
	pArray->SetVectorCount( TriCount * 3 );

	for( i = 0, TriIndex = 0; i < (INT32) pIA->GetCount(); i += 3)
	{
		idx1 = pIA->GetIndex( i);
		idx2 = pIA->GetIndex( i + 1);
		idx3 = pIA->GetIndex( i + 2);

		pVA->GetPosition( idx1, &pos1);
		pVA->GetPosition( idx2, &pos2);
		pVA->GetPosition( idx3, &pos3);

		i3Vector::Normal( &norm, &pos1, &pos2, &pos3);
		if( i3Vector::Length( &norm ) < 0.00001f)
			continue;

		CheckMesh( &pos1, &pos2, &pos3 );

		// Position 1 Index
		idx1 = FindPos( pArray, &pos1);
		if( idx1 == -1)
		{
			pArray->SetVector( PosCount, &pos1);
			idx1 = PosCount;
			PosCount++;
		}

		// Position 2 Index
		idx2 = FindPos( pArray, &pos2);
		if( idx2 == -1)
		{
			pArray->SetVector( PosCount, &pos2);
			idx2 = PosCount;
			PosCount++;
		}

		// Position 3 Index
		idx3 = FindPos( pArray, &pos3);
		if( idx3 == -1)
		{
			pArray->SetVector( PosCount, &pos3);
			idx3 = PosCount;
			PosCount++;
		}

		// Triangle Indices
		pTri[ TriIndex].nVertex[0] = (UINT16) idx1;
		pTri[ TriIndex].nVertex[1] = (UINT16) idx2;
		pTri[ TriIndex].nVertex[2] = (UINT16) idx3;

		TriIndex++;
	}

	pIA->Unlock();
	pVA->Unlock();

	if( TriIndex > 0)
	{
		pMesh = i3NavigationMesh::new_object();

		I3ASSERT( TriIndex < I3_MAX_NVMESH_TRICOUNT );
		pMesh->SetTriangleCount( TriIndex );

		// 개수를 맞춘 Position Array를 새롭게 작성한다.
		OptimizePosition( pMesh, pArray, PosCount);

		for( i = 0; i < TriIndex; i++)
		{
			pMesh->SetTriangle( i, & pTri[i]);
		}
	}

	I3_SAFE_RELEASE( pArray );
	I3_SAFE_DELETE_ARRAY( pTri );

	return pMesh;
}

i3NavigationMesh * i3OptBuildNavigationMesh::ConvertTriList( i3VertexArray * pVA)
{
	INT32 i, PosCount, TriIndex, NormCount, TriCount;
	INT32 idx1, idx2, idx3;
	VEC3D pos1, pos2, pos3, norm;
	i3Vector3Array * pArray		= nullptr;
	i3NavigationMesh * pMesh	= nullptr;
	i3::pack::NAVMESH_TRI * pTri		= nullptr;

	if( pVA->GetCount() <= 0)
		return nullptr;

	pVA->Lock();

	PosCount = 0;
	NormCount = 0;
	TriCount = pVA->GetCount() / 3;

	pTri = new i3::pack::NAVMESH_TRI[TriCount];
	I3ASSERT( pTri != nullptr);

	pArray = i3Vector3Array::new_object();
	pArray->SetVectorCount( TriCount * 3);

	for( i = 0, TriIndex = 0; i < (INT32) pVA->GetCount(); i += 3)
	{
		pVA->GetPosition( i, &pos1);
		pVA->GetPosition( i + 1, &pos2);
		pVA->GetPosition( i + 2, &pos3);

		i3Vector::Normal( &norm, &pos1, &pos2, &pos3);
		if( i3Vector::Length( &norm ) < 0.00001f)
			continue;

		CheckMesh( &pos1, &pos2, &pos3 );

		// Position 1 Index
		idx1 = FindPos( pArray, &pos1);
		if( idx1 == -1)
		{
			pArray->SetVector( PosCount, &pos1);
			idx1 = PosCount;
			PosCount++;
		}

		// Position 2 Index
		idx2 = FindPos( pArray, &pos2);
		if( idx2 == -1)
		{
			pArray->SetVector( PosCount, &pos2);
			idx2 = PosCount;
			PosCount++;
		}

		// Position 3 Index
		idx3 = FindPos( pArray, &pos3);
		if( idx3 == -1)
		{
			pArray->SetVector( PosCount, &pos3);
			idx3 = PosCount;
			PosCount++;
		}

		// Triangle Indices
		pTri[ TriIndex].nVertex[0] = (UINT16) idx1;
		pTri[ TriIndex].nVertex[1] = (UINT16) idx2;
		pTri[ TriIndex].nVertex[2] = (UINT16) idx3;

		TriIndex++;
	}

	pVA->Unlock();

	if( TriIndex > 0)
	{
		pMesh = i3NavigationMesh::new_object();

		I3ASSERT( TriIndex < I3_MAX_NVMESH_TRICOUNT );
		pMesh->SetTriangleCount( TriIndex );

		// 개수를 맞춘 Position Array를 새롭게 작성한다.
		{
			OptimizePosition( pMesh, pArray, PosCount);
		}

		for( i = 0; i < TriIndex; i++)
		{
			pMesh->SetTriangle( i, & pTri[i]);
		}
	}

	I3_SAFE_RELEASE( pArray );
	I3_SAFE_DELETE_ARRAY( pTri );

	return pMesh;
}

i3NavigationMesh * i3OptBuildNavigationMesh::ConvertIndexTriStrip( i3GeometryAttr * pGeoAttr)
{
	INT32 i, PosCount, TriIndex, NormCount, TriCount;
	INT32 idx1, idx2, idx3;
	INT32 vidx1, vidx2, vidx3;
	VEC3D pos1, pos2, pos3, norm;
	i3Vector3Array * pArray		= nullptr;
	i3VertexArray * pVA			= nullptr;
	i3IndexArray * pIA			= nullptr;
	INT32 j, accm, PrimLen;
	i3NavigationMesh * pMesh	= nullptr;
	i3::pack::NAVMESH_TRI * pTri		= nullptr;

	pIA = pGeoAttr->GetIndexArray();
	pVA = pGeoAttr->GetVertexArray();

	if( pVA->GetCount() <= 0)
		return nullptr;

	pIA->Lock();
	pVA->Lock();

	PosCount = 0;
	NormCount = 0;
	TriCount = 0;

	if( pGeoAttr->GetPrimitiveCount() == 1)
	{
		TriCount = pIA->GetCount() - 2;
	}
	else
	{
		for( i = 0; i < (INT32) pGeoAttr->GetPrimitiveCount(); i++)
		{
			TriCount += pGeoAttr->GetPrimitiveLength( i);
		}
	}

	pTri = new i3::pack::NAVMESH_TRI[ TriCount];

	pArray = i3Vector3Array::new_object();
	pArray->SetVectorCount( TriCount * 3);

	accm = 0;
	TriIndex = 0;

	for( j = 0; j < (INT32) pGeoAttr->GetPrimitiveCount(); j++)
	{
		vidx1 = pIA->GetIndex( accm);		accm++;
		vidx2 = pIA->GetIndex( accm);		accm++;
		vidx3 = pIA->GetIndex( accm);		accm++;

		if( pGeoAttr->GetPrimitiveCount() == 1)
		{
			PrimLen = pIA->GetCount() - 2;
		}
		else
		{
			PrimLen = pGeoAttr->GetPrimitiveLength( j);
		}

		for( i = 0; i < (INT32) PrimLen; i++)
		{
			pVA->GetPosition( vidx1, &pos1);
			pVA->GetPosition( vidx2, &pos2);
			pVA->GetPosition( vidx3, &pos3);

			i3Vector::Normal( &norm, &pos1, &pos2, &pos3);
			if( i3Vector::Length( &norm ) < 0.00001f)
				continue;

			// Position 1 Index
			idx1 = FindPos( pArray, &pos1);
			if( idx1 == -1)
			{
				pArray->SetVector( PosCount, &pos1);
				idx1 = PosCount;
				PosCount++;
			}

			// Position 2 Index
			idx2 = FindPos( pArray, &pos2);
			if( idx2 == -1)
			{
				pArray->SetVector( PosCount, &pos2);
				idx2 = PosCount;
				PosCount++;
			}

			// Position 3 Index
			idx3 = FindPos( pArray, &pos3);
			if( idx3 == -1)
			{
				pArray->SetVector( PosCount, &pos3);
				idx3 = PosCount;
				PosCount++;
			}

			// Triangle Indices
			pTri[ TriIndex].nVertex[0] = (UINT16) idx1;
			pTri[ TriIndex].nVertex[1] = (UINT16) idx2;
			pTri[ TriIndex].nVertex[2] = (UINT16) idx3;

			// Swap Index
			if( (i & 1))
			{
				// 홍수
				vidx1 = vidx3;
				vidx3 = pIA->GetIndex( accm);		accm++;
			}
			else
			{
				// 짝수
				vidx1 = vidx2;
				vidx2 = pIA->GetIndex( accm);		accm++;
			}
			TriIndex++;
		}
	}

	pIA->Unlock();
	pVA->Unlock();

	if( TriIndex > 0)
	{
		pMesh = i3NavigationMesh::new_object();

		I3ASSERT( TriIndex < I3_MAX_NVMESH_TRICOUNT );
		pMesh->SetTriangleCount( TriIndex );

		// 개수를 맞춘 Position Array를 새롭게 작성한다.
		{
			OptimizePosition( pMesh, pArray, PosCount);
		}

		for( i = 0; i < TriIndex; i++)
		{
			pMesh->SetTriangle( i, & pTri[ i]);
		}
	}

	I3_SAFE_RELEASE( pArray );
	I3_SAFE_DELETE_ARRAY( pTri );

	return pMesh;
}

i3NavigationMesh *i3OptBuildNavigationMesh::ConvertTriStrip( i3GeometryAttr * pGeoAttr)
{
	INT32 i, PosCount, TriIndex, NormCount, TriCount;
	INT32 idx1, idx2, idx3;
	INT32 vidx1, vidx2, vidx3;
	VEC3D pos1, pos2, pos3, norm;
	i3Vector3Array * pArray		= nullptr;
	i3VertexArray * pVA			= nullptr;
	INT32 j, accm, PrimLen;
	i3NavigationMesh * pMesh	= nullptr;
	i3::pack::NAVMESH_TRI * pTri		= nullptr;

	pVA = pGeoAttr->GetVertexArray();

	if( pVA->GetCount() <= 0)
		return nullptr;

	pVA->Lock();

	PosCount = 0;
	NormCount = 0;
	TriCount = 0;

	if( pGeoAttr->GetPrimitiveCount() == 1)
	{
		TriCount = pVA->GetCount() - 2;
	}
	else
	{
		for( i = 0; i < (INT32) pGeoAttr->GetPrimitiveCount(); i++)
		{
			TriCount += pGeoAttr->GetPrimitiveLength( i);
		}
	}

	pTri = new i3::pack::NAVMESH_TRI[ TriCount];
	I3ASSERT( pTri != nullptr);

	pArray = i3Vector3Array::new_object();
	pArray->SetVectorCount( TriCount * 3);

	accm = 0;
	TriIndex = 0;

	for( j = 0; j < (INT32) pGeoAttr->GetPrimitiveCount(); j++)
	{
		vidx1 = accm;		accm++;
		vidx2 = accm;		accm++;
		vidx3 = accm;		accm++;

		if( pGeoAttr->GetPrimitiveCount() == 1)
		{
			PrimLen = pVA->GetCount() - 2;
		}
		else
		{
			PrimLen = pGeoAttr->GetPrimitiveLength( j);
		}

		for( i = 0; i < (INT32) PrimLen; i++)
		{
			pVA->GetPosition( vidx1, &pos1);
			pVA->GetPosition( vidx2, &pos2);
			pVA->GetPosition( vidx3, &pos3);

			i3Vector::Normal( &norm, &pos1, &pos2, &pos3);
			if( i3Vector::Length( &norm ) < 0.00001f)
				continue;

			// Position 1 Index
			idx1 = FindPos( pArray, &pos1);
			if( idx1 == -1)
			{
				pArray->SetVector( PosCount, &pos1);
				idx1 = PosCount;
				PosCount++;
			}

			// Position 2 Index
			idx2 = FindPos( pArray, &pos2);
			if( idx2 == -1)
			{
				pArray->SetVector( PosCount, &pos2);
				idx2 = PosCount;
				PosCount++;
			}

			// Position 3 Index
			idx3 = FindPos( pArray, &pos3);
			if( idx3 == -1)
			{
				pArray->SetVector( PosCount, &pos3);
				idx3 = PosCount;
				PosCount++;
			}

			// Triangle Indices
			pTri[ TriIndex].nVertex[0] = (UINT16) idx1;
			pTri[ TriIndex].nVertex[1] = (UINT16) idx2;
			pTri[ TriIndex].nVertex[2] = (UINT16) idx3;

			// Swap Index
			if( (i & 1))
			{
				// 홍수
				vidx1 = vidx3;
				vidx3 = accm;		accm++;
			}
			else
			{
				// 짝수
				vidx1 = vidx2;
				vidx2 = accm;		accm++;
			}
			TriIndex++;
		}
	}

	pVA->Unlock();

	if( TriIndex > 0)
	{
		pMesh = i3NavigationMesh::new_object();

		I3ASSERT( TriIndex < I3_MAX_NVMESH_TRICOUNT );
		pMesh->SetTriangleCount( TriIndex );

		// 개수를 맞춘 Position Array를 새롭게 작성한다.
		OptimizePosition( pMesh, pArray, PosCount);

		for( i = 0; i < TriIndex; i++)
		{
			pMesh->SetTriangle( i, & pTri[i]);
		}
	}

	I3_SAFE_RELEASE( pArray );
	I3_SAFE_DELETE_ARRAY( pTri );

	return pMesh;
}

void i3OptBuildNavigationMesh::OptimizePosition( i3NavigationMesh * pMesh, i3Vector3Array * pArray, INT32 Count)
{
	I3TRACE( "i3OptBuildNavigationMesh::OptimizePosition()\n" );
	INT32 i;
	REAL32 x, y;
	i3Vector2Array * pNew = i3Vector2Array::new_object_ref();

	pNew->SetVectorCount( Count);

	if( m_bRotateAxisFor3DMAX )
	{	// 3DS MAX 좌표계로 변환할 경우
		for( i = 0; i < Count; i++)
		{
			x = -1.0f * i3Vector::GetX( pArray->GetVector( i ) );
			y = i3Vector::GetY( pArray->GetVector( i ) );

			pNew->SetVector( i, x, y );
			I3TRACE( "%d : %9f, %9f\n", i, x, y );
		}
	}
	else
	{	// 그대로 가져갈 경우
		for( i = 0; i < Count; i++)
		{
			x = i3Vector::GetX( pArray->GetVector( i ) );
			y = i3Vector::GetZ( pArray->GetVector( i ) );

			pNew->SetVector( i, x, y );
			I3TRACE( "%d : %9f, %9f\n", i, x, y );
		}
	}

	pMesh->SetPositionArray( pNew);
}

bool i3OptBuildNavigationMesh::Rec_Find( i3Node * pNode )
{
	INT32 i;
	i3Node * pChild;

	if( i3::kind_of<i3Geometry* >(pNode))
	{
		i3Geometry *pGeometry = (i3Geometry*)pNode;
		INT32 nGeoAttrCount = pGeometry->GetGeometryAttrCount();
		I3ASSERT( nGeoAttrCount == 1 );
		m_pGeometryAttr = (i3GeometryAttr*)pGeometry->GetGeometryAttr( 0 );
		return true;
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);
		if( Rec_Find( pChild) )
		{
			return true;
		}
	}

	return false;
}

void i3OptBuildNavigationMesh::SetProgressCtrl( i3ToolProgressCtrl *pCtrl )
{
	I3_REF_CHANGE( m_pProgressCtrl, pCtrl);
}

void i3OptBuildNavigationMesh::SetProgressRange( INT32 nRange )
{
	if( m_pProgressCtrl )
	{
		m_pProgressCtrl->SetRange( nRange );
	}
}

void i3OptBuildNavigationMesh::SetProgressPos( INT32 nPos )
{
	if( m_pProgressCtrl )
	{
		m_pProgressCtrl->SetPos( nPos );
	}
}

// 만들기
bool i3OptBuildNavigationMesh::Build( i3Node * pRoot, i3ToolProgressCtrl *pProgressCtrl )
{
	SetProgressCtrl( pProgressCtrl );
	SetProgressRange( 0 );

	i3NavigationMesh *pNavMesh = nullptr;

	m_pGeometryAttr = nullptr;

	// 입력된 Scene에서 Geometry를 찾아서 m_pGeometryAttr에 포인터 저장.
	if( Rec_Find( pRoot) )
	{
		// 네비게이션 메시를 제작
		pNavMesh = BuildNavigationMesh( m_pGeometryAttr );
		if( pNavMesh != nullptr )
		{
			// 탐색 트리를 생성
			if( pNavMesh->CreateSearchTree() )
			{
				SetTraceRoot( pNavMesh);
				return true;
			}
		}
	}

	SetProgressPos( 0 );

	return false;
}

i3NavigationMesh * i3OptBuildNavigationMesh::BuildNavigationMesh( i3GeometryAttr * pGeoAttr )
{
	I3ASSERT( pGeoAttr != nullptr );

	i3NavigationMesh * pMesh	= nullptr;
	i3VertexArray * pVA			= nullptr;
	i3IndexArray * pIA			= nullptr;

	pVA = pGeoAttr->GetVertexArray();
	pIA = pGeoAttr->GetIndexArray();

	switch( pGeoAttr->GetPrimitiveType())
	{
	case I3G_PRIM_TRILIST :
		{
			if( pIA != nullptr)
				pMesh = ConvertIndexTriList( pIA, pVA);
			else
				pMesh = ConvertTriList( pVA);
		}
		break;

	case I3G_PRIM_TRISTRIP :
	case I3G_PRIM_TRIFAN :
		{
			if( pIA != nullptr)
				pMesh = ConvertIndexTriStrip( pGeoAttr);
			else
				pMesh = ConvertTriStrip( pGeoAttr);
		}
		break;
	}

	if( pMesh != nullptr)
	{
		pMesh->SetProgressCtrl( m_pProgressCtrl );
		SetProgressRange( pMesh->GetTriangleCount() );
		SetProgressPos( 0 );

		// 인접한 이웃 찾기
		if( ! BuildGraphInfo( pMesh ) )
		{
			i3Log( "i3OptBuildNavigationMesh::BuildNavigationMesh", "BuildGarphInfo()" );
			I3_SAFE_RELEASE( pMesh );
			return nullptr;
		}

		SetProgressPos( 0 );

		// 모든 삼각형이 연결되어 있는지 여부 검사
		if( ! CheckNeighborConnection( pMesh ) )
		{
			return false;
		}

		SetProgressPos( 0 );

		// 길찾기 데이터 생성
		if( ! pMesh->BuildNavigationInfo() )
		{
			i3Log( "i3OptBuildNavigationMesh::BuildNavigationMesh", "BuildNavigationInfo()" );
			I3_SAFE_RELEASE( pMesh );
			return nullptr;
		}
	}

	SetProgressPos( 0 );

	return pMesh;
}

// 그래프 정보 생성
bool i3OptBuildNavigationMesh::BuildGraphInfo( i3NavigationMesh *pMesh )
{
	I3ASSERT( pMesh != nullptr );
	I3ASSERT( pMesh->GetTriangleCount() > 0 );

	bool *pError = new bool[ pMesh->GetTriangleCount() ];
	bool bRet = pMesh->BuildGraphInfo( pError );
	if( ! bRet )
	{
		i3Log( __FUNCTION__, "Failed to Build Graph." );
		i3Log( __FUNCTION__, "Exporting VLOG file to Alchemy2I3.vlog" );

		bool bLastError = true;
		i3OptVLog( "c 127 127 255\n" );

		// 메쉬 디버깅 정보 출력. VLog
		UINT16 i;
		for( i = 0; i < pMesh->GetTriangleCount(); i++ )
		{
			if( bLastError != pError[i] )
			{
				bLastError = pError[i];
				if( pError[i] )
				{
					i3OptVLog( "c 127 127 255\n" );
				}
				else
				{
					i3OptVLog( "c 255 127 127\n" );
				}
			}

			VEC2D *pVec[3];
			pVec[0] = pMesh->GetPosition( pMesh->GetTriangle( i )->nVertex[0] );
			pVec[1] = pMesh->GetPosition( pMesh->GetTriangle( i )->nVertex[1] );
			pVec[2] = pMesh->GetPosition( pMesh->GetTriangle( i )->nVertex[2] );

			i3OptVLog( "t\n" );
			i3OptVLog( "%f %f %f\n", i3Vector::GetY( pVec[0] ), 0.0f, i3Vector::GetX( pVec[0] ) );
			i3OptVLog( "%f %f %f\n", i3Vector::GetY( pVec[1] ), 0.0f, i3Vector::GetX( pVec[1] ) );
			i3OptVLog( "%f %f %f\n", i3Vector::GetY( pVec[2] ), 0.0f, i3Vector::GetX( pVec[2] ) );
		}
	}

	I3_SAFE_DELETE_ARRAY( pError );
    return bRet;
}

// 모든 삼각형이 서로 연결되어 있는지 여부 검사
// 이 함수는 툴에서만 사용함.
bool i3OptBuildNavigationMesh::CheckNeighborConnection( i3NavigationMesh *pMesh )
{
	I3ASSERT( pMesh != nullptr );
	bool *pChecked = new bool[ pMesh->GetTriangleCount() ];

	if( pMesh->CheckNeighborConnection( pChecked ) )
	{
		I3_SAFE_DELETE_ARRAY( pChecked );
		return true;
	}

	i3Log( "i3NavigationMesh::CheckNeighborConnection", "Connection Problem found" );
	i3Log( "i3NavigationMesh::CheckNeighborConnection", "Exporting VLOG file to Alchemy2I3.vlog" );

	bool bLastCheck = true;
	i3OptVLog( "c 127 127 255\n" );

	// 메쉬 디버깅 정보 출력. VLog
	UINT16 i;
	for( i = 0; i < pMesh->GetTriangleCount(); i++ )
	{
		if( bLastCheck != pChecked[i] )
		{
			bLastCheck = pChecked[i];
			if( pChecked[i] )
			{
				i3OptVLog( "c 127 127 255\n" );
			}
			else
			{
				i3OptVLog( "c 255 127 127\n" );
			}
		}

		VEC2D *pVec[3];
		pVec[0] = pMesh->GetPosition( pMesh->GetTriangle( i )->nVertex[0] );
		pVec[1] = pMesh->GetPosition( pMesh->GetTriangle( i )->nVertex[1] );
		pVec[2] = pMesh->GetPosition( pMesh->GetTriangle( i )->nVertex[2] );

		i3OptVLog( "t\n" );
		i3OptVLog( "%f %f %f\n", i3Vector::GetY( pVec[0] ), 0.0f, i3Vector::GetX( pVec[0] ) );
		i3OptVLog( "%f %f %f\n", i3Vector::GetY( pVec[1] ), 0.0f, i3Vector::GetX( pVec[1] ) );
		i3OptVLog( "%f %f %f\n", i3Vector::GetY( pVec[2] ), 0.0f, i3Vector::GetX( pVec[2] ) );
	}

	I3_SAFE_DELETE_ARRAY( pChecked );

	return false;
}

