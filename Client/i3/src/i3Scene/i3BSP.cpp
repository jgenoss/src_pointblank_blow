#include "i3SceneDef.h"
#include "i3BSPNode.h"
#include "i3BSP.h"


I3_CLASS_INSTANCE( i3BSP, i3NamedElement );

i3BSP::i3BSP(void)
{
	i3Vector::Set( &m_MinSize, 0.0f, 0.0f, 0.0f );
	i3Vector::Set( &m_MaxSize, 0.0f, 0.0f, 0.0f );

	m_pPrimOrder		= NULL;
}

i3BSP::~i3BSP(void)
{
	I3_SAFE_RELEASE( m_pPrimOrder );
}

void	i3BSP::_BuildPrimOrder( i3VertexArray * pVA, i3IndexArray * pIA )
{
	INT32	i;

	if( m_pPrimOrder == NULL )
	{
		m_pPrimOrder = i3List::NewObject();
		I3ASSERT( m_pPrimOrder );
	}
	else
		m_pPrimOrder->Clear();

	pVA->Lock();

	if( pIA == NULL )
	{
		m_pPrimOrder->SetCount( pVA->GetCount() );

		for( i = 0; i < m_pPrimOrder->GetCount(); i++ )
			m_pPrimOrder->SetItem( i, (void*) i );
	}
	else
	{	
		pIA->Lock();

		m_pPrimOrder->SetCount( pIA->GetCount() );
		for( i = 0; i < m_pPrimOrder->GetCount(); i++)
			m_pPrimOrder->SetItem( i,  (void*) pIA->GetIndex16( i ) );

		pIA->Unlock();
	}

	pVA->Unlock();
}

void i3BSP::_CheckWorldSize( i3VertexArray * pVA, UINT32 idx )
{
	VEC3D	VecPos;
	
	pVA->GetPosition( idx, &VecPos );
	if( i3Vector::GetX( &VecPos ) < _getMinX() )
		_setMinX( i3Vector::GetX( &VecPos ) );
	else if( i3Vector::GetX( &VecPos ) > _getMaxY() )
		_setMaxX( i3Vector::GetX( &VecPos ) );

	if( i3Vector::GetY( &VecPos ) < _getMinY() )
		_setMinY( i3Vector::GetY( &VecPos ) );
	else if( i3Vector::GetY( &VecPos ) > _getMaxY() )
		_setMaxY( i3Vector::GetY( &VecPos ) );

	if( i3Vector::GetZ( &VecPos ) < _getMinZ() )
		_setMinZ( i3Vector::GetZ( &VecPos ) );
	else if( i3Vector::GetZ( &VecPos ) > _getMaxZ() )
		_setMaxZ( i3Vector::GetZ( &VecPos ) );
}

void	i3BSP::Build(void)
{
}

typedef struct ALIGN4 _tagi3Cube
{
	VEC3D	m_Points[8];

	void	initialize() { memset( m_Points, 0, sizeof( VEC3D) * 8 ); }
} i3Cube;

BOOL i3BSP::_BuildConvexPoly( i3VertexArray * pVA, i3BSPNode * pNode )
{
	INT32	i, j;
	UINT32	idx;
	VEC3D	Vec1_1, Vec1_2, Vec1_3;
	VEC3D	Vec2_1, Vec2_2, Vec2_3;
	BOOL	result = FALSE;
	INT32	count;
	INT32	next;

	i3List	LeafList;

	i3List	* pPolyList;

	i3Cube	cube;
	
	next = 0;
	for( i = 0; i < pNode->getPolygonCount(); i++ )
	{
		if( pPolyList == NULL )
		{
			idx = getPrimOrder( pNode->getPolygon( i ) );
			pVA->GetPosition( idx, &Vec1_1 );

			idx = getPrimOrder( pNode->getPolygon( i ) + 1 );
			pVA->GetPosition( idx, &Vec1_2 );

			idx = getPrimOrder( pNode->getPolygon( i ) + 2 );
			pVA->GetPosition( idx, &Vec1_3 );

			pPolyList = i3List::NewObject();
			I3ASSERT( pPolyList );

			cube.initialize();
		}

		for( j = 0; i != j && j < pNode->getPolygonCount(); j++ )
		{
			idx = getPrimOrder( pNode->getPolygon( j ) );
			pVA->GetPosition( idx, &Vec2_1 );

			idx = getPrimOrder( pNode->getPolygon( j ) + 1 );
			pVA->GetPosition( idx, &Vec2_2 );

			idx = getPrimOrder( pNode->getPolygon( j ) + 2 );
			pVA->GetPosition( idx, &Vec2_3 );

			count = 0;

			// 추가가능한 폴리곤을 찾는다.
			if( i3Vector::IsSame( &Vec1_1, &Vec2_1 ) )
				count++;
			else if( i3Vector::IsSame( &Vec1_1, &Vec2_2 ) )
				count++;
			else if( i3Vector::IsSame( &Vec1_1, &Vec2_3 ) )
				count++;
			
			if( i3Vector::IsSame( &Vec1_2, &Vec2_1 ) )
				count++;
			else if( i3Vector::IsSame( &Vec1_2, &Vec2_2 ) )
				count++;
			else if( i3Vector::IsSame( &Vec1_2, &Vec2_3 ) )
				count++;

			if( i3Vector::IsSame( &Vec1_3, &Vec2_1 ) )
				count++;
			else if( i3Vector::IsSame( &Vec1_3, &Vec2_2 ) )
				count++;
			else if( i3Vector::IsSame( &Vec1_3, &Vec2_3 ) )
				count++;

			if( count == 2 )
			{
				// 인접한 폴리곤이다
				// if( // 사각형에 속하는가? )
				{
					// 임시 리스트에 넣어둔다
					pPolyList->Add( (void*) j );
				}
			}
		}
	}

	return result;
}

BOOL i3BSP::_CheckPartitionPlane( i3VertexArray * pVA, i3BSPNode * pNode , VEC4D * pOutPlane )
{
	//INT32	i;
	//UINT32	idx;
	//VEC3D	Vec1, Vec2, Vec3;
	BOOL	result = FALSE;
	
	


	return result;
}

void	i3BSP::CreateNode( i3VertexArray * pVA, i3BSPNode * pParent )
{
	INT32	i;
	UINT32	j;
	UINT32	idx;
	UINT32	vi1, vi2, vi3;

	VEC4D	Plane;
	VEC3D	VecPos, VecNormal;
	VEC3D	VecPos2;
	REAL32	d;
	REAL32	Dot;

	BOOL	bCheck = FALSE;

	// 생성할 노드 데이터
	i3BSPNode * pFrontNode;
	i3BSPNode * pBackNode;

	pVA->Lock();

	// Check Convex object

	// 프론트 페이스 노드를 추가
	pFrontNode = i3BSPNode::NewObject();
	I3ASSERT( pFrontNode );
	pFrontNode->Create();

	// 백페이스 노드를 추가
	pBackNode = i3BSPNode::NewObject();
	I3ASSERT( pBackNode );
	pBackNode->Create();

	// 기본이 되는 플랜을 세팅한다.
	{
		// 현재 노드들중에서 월드 사이즈의 최소 최대를 구한다.
		for( i = 0; i < pParent->getPolygonCount(); i++ )
		{
			idx = pParent->getPolygon( i );
			vi1 = getPrimOrder( idx );
			vi2 = getPrimOrder( idx + 1 );
			vi3 = getPrimOrder( idx + 2 );
			
			_CheckWorldSize( pVA, vi1 );
			_CheckWorldSize( pVA, vi2 );
			_CheckWorldSize( pVA, vi3 );
		}
		// 중심 포인트를 구한다.
		_CalcMidpoint();

		// 축 순환에 따른 플래너 세팅
		i3Vector::Set( &VecNormal, 0.0f, 0.0f, 0.0f );
		if( i3Vector::GetY( pParent->getPlane() ) == 1.0f )
			i3Vector::SetX( &VecNormal, 1.0f );
		else if( i3Vector::GetZ( pParent->getPlane() ) == 1.0f )
			i3Vector::SetY( &VecNormal, 1.0f );
		else
			i3Vector::SetZ( &VecNormal, 1.0f );

		d = -( i3Math::Mul( i3Vector::GetX( &m_MidPoint ), i3Vector::GetX( &VecNormal ) ) +
			i3Math::Mul( i3Vector::GetY( &m_MidPoint ), i3Vector::GetY( &VecNormal ) ) +
			i3Math::Mul( i3Vector::GetZ( &m_MidPoint ), i3Vector::GetZ( &VecNormal ) ) );

		i3Vector::Set( &Plane,
			i3Vector::GetX( &VecNormal ), i3Vector::GetY( &VecNormal ), i3Vector::GetZ( &VecNormal ),
			d );

		pParent->setPlane( &Plane );

		pFrontNode->setPlane( &Plane );
		pBackNode->setPlane( &Plane );
	}

	// 다른 폴리곤에 대해 플랜과 비교하여 자식 노드에 세팅
	for( i = 0; i < pParent->getPolygonCount(); i++ )
	{
		// 다음 폴리곤에 대해서 판별
		idx = pParent->getPolygon( i );
		vi1 = getPrimOrder( idx );
		vi2 = getPrimOrder( idx + 1 );
		vi3 = getPrimOrder( idx + 2 );

		pVA->GetPosition( vi1, &VecPos );
		Dot = i3Vector::Dot( &VecNormal, &VecPos );

		if( Dot >= 0.0f )
		{	
			pVA->GetPosition( vi2, &VecPos );
			Dot = i3Vector::Dot( &VecNormal, &VecPos );

			if( Dot >= 0.0f )
			{	
				pVA->GetPosition( vi3, &VecPos );
				Dot = i3Vector::Dot( &VecNormal, &VecPos );

				if( Dot >= 0.0f )
				{
					// 앞쪽에 위치한다
					pFrontNode->addPolygon( idx );
					bCheck = TRUE;
				}
				else
				{
					// 폴리곤이 겹친다.
					pFrontNode->addPolygon( idx );
					pBackNode->addPolygon( idx );
				}
			}
			else
			{
				// 폴리곤이 겹친다.
				pFrontNode->addPolygon( idx );
				pBackNode->addPolygon( idx );
			}
		}
		else
		{	
			pVA->GetPosition( vi2, &VecPos );
			Dot = i3Vector::Dot( &VecNormal, &VecPos );

			if( Dot < 0.0f )
			{	
				pVA->GetPosition( vi3, &VecPos );
				Dot = i3Vector::Dot( &VecNormal, &VecPos );

				if( Dot < 0.0f )
				{
					// 뒤쪽이다
					pBackNode->addPolygon( idx );
					bCheck = TRUE;
				}
				else
				{
					// 폴리곤이 겹친다.
					pFrontNode->addPolygon( idx );
					pBackNode->addPolygon( idx );
				}
			}
			else
			{
				// 폴리곤이 겹친다.
				pFrontNode->addPolygon( idx );
				pBackNode->addPolygon( idx );
			}
		}
	}

	pVA->Unlock();

	if( bCheck )
	{
		pParent->AddChild( pFrontNode );
		pParent->AddChild( pBackNode );
		// 이전의 폴리곤 데이터는 필요가 없다.
		pParent->ReleasePolygonList();

		CreateNode( pVA, pFrontNode );
		CreateNode( pVA, pBackNode );
	}
	else
	{
		I3_SAFE_RELEASE( pFrontNode );
		I3_SAFE_RELEASE( pBackNode );
	}
}

void	i3BSP::Compile( i3Geometry * pGeo )
{
	INT32	i;
	UINT32	j;

	i3GeometryAttr * pGeoAttr;
	i3IndexArray *pIA;
	i3VertexArray *pVA;

	i3BSPNode * pRoot;

	pRoot = i3BSPNode::NewObject();
	I3ASSERT( pRoot );

	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++ )
	{
		pGeoAttr = (i3GeometryAttr*) pGeo->GetGeometryAttr( i );

		pVA = pGeoAttr->GetVertexArray();
		pIA = pGeoAttr->GetIndexArray();

		// 버택스를 나열한다.
		_BuildPrimOrder( pVA, pIA );

		// 폴리곤 카운트를 루트에 저장
		for( j = 0; j < getPrimOrderCount(); j++ )
			pRoot->addPolygon( getPrimOrder( j ) );

		if( pGeoAttr->GetPrimitiveType() == I3G_PRIM_TRILIST )
		{
			// 재귀 호출해줘야된다.
			CreateNode( pVA, pRoot );
		}
		else
		{
		}
		
	}
}

UINT32 i3BSP::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	I3ASSERT( pStream );

	return Result;
}

UINT32 i3BSP::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	I3ASSERT( pStream );

	return Result;
}
