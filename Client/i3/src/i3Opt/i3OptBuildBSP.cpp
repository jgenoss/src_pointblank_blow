#include "i3OptPCH.h"
#include "i3OptBuildBSP.h"
#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/generic_string_cat.h"

I3_CLASS_INSTANCE( i3OptBuildBSP);

i3OptBuildBSP::i3OptBuildBSP(void)
{
	m_Class = CLASS_SCENEGRAPH;
}

i3OptBuildBSP::~i3OptBuildBSP(void)
{
	_RemoveAllBSPGeometry();
}

void i3OptBuildBSP::_RemoveBSPGeometry( i3BSPGeometry * pBSPGeo )
{
	I3MEM_SAFE_FREE_POINTER( pBSPGeo->pTri );
	I3_SAFE_RELEASE( pBSPGeo->pBoundBox );
	pBSPGeo->pGeometry = nullptr;
}

void i3OptBuildBSP::_RemoveAllBSPGeometry( void )
{
	INT32	i;
	i3BSPGeometry * pBSPGeo;

	for( i = 0; i < _getBSPGeometryCount(); i++ )
	{
		pBSPGeo = _getBSPGeometry( i );
		
		_RemoveBSPGeometry( pBSPGeo );

		I3MEM_SAFE_FREE_POINTER( pBSPGeo );
	}

	m_BSPGeometryList.clear();
}

void i3OptBuildBSP::_CreateRoot( void )
{
	i3BSPNode * pBSPNode = i3BSPNode::new_object();
	I3ASSERT( pBSPNode );
	pBSPNode->Create();
	addNode( pBSPNode );
}

void i3OptBuildBSP::_CalcMaxVec( VEC3D * pVecMax, VEC3D * pSrcVec, bool bInit )
{
	if( bInit )
		i3Vector::Copy( pVecMax, pSrcVec );

	// 삼각형에서 최소 좌표를 구한다.
	if( i3Vector::GetX( pVecMax ) < i3Vector::GetX( pSrcVec ) )	{ i3Vector::SetX( pVecMax, i3Vector::GetX( pSrcVec ) );	}
	
	// 삼각형에서 최소 좌표를 구한다.
	if( i3Vector::GetY( pVecMax ) < i3Vector::GetY(pSrcVec ) )	{ i3Vector::SetY( pVecMax, i3Vector::GetY( pSrcVec ) );	}
	
	// 삼각형에서 최소 좌표를 구한다.
	if( i3Vector::GetZ( pVecMax ) < i3Vector::GetZ( pSrcVec ) )	{ i3Vector::SetZ( pVecMax, i3Vector::GetZ( pSrcVec ) ); }
}

void i3OptBuildBSP::_CalcMinVec( VEC3D * pVecMin, VEC3D * pSrcVec, bool	bInit )
{
	if( bInit )
		i3Vector::Copy( pVecMin, pSrcVec );

	// 삼각형에서 최소 좌표를 구한다.
	if( i3Vector::GetX( pVecMin ) > i3Vector::GetX( pSrcVec ) )	{ i3Vector::SetX( pVecMin, i3Vector::GetX( pSrcVec ) ); }

	// 삼각형에서 최소 좌표를 구한다.
	if( i3Vector::GetY( pVecMin ) > i3Vector::GetY(pSrcVec ) )	{ i3Vector::SetY( pVecMin, i3Vector::GetY( pSrcVec ) ); }
	
	// 삼각형에서 최소 좌표를 구한다.
	if( i3Vector::GetZ( pVecMin ) > i3Vector::GetZ( pSrcVec ) )	{ i3Vector::SetZ( pVecMin, i3Vector::GetZ( pSrcVec ) );	}
}

bool i3OptBuildBSP::_CalcMaxVec( VEC3D * pVecMax, VEC3D * pSrcVec, UINT32 nStyle )
{
	if( nStyle & I3_BSP_MINMAX_X )
	{
		// 삼각형에서 최대 좌표를 구한다.
		if( i3Vector::GetX( pVecMax ) < i3Vector::GetX( pSrcVec ) )	{ i3Vector::SetX( pVecMax, i3Vector::GetX( pSrcVec ) );	return true;	}
	}
	else if( nStyle & I3_BSP_MINMAX_Y )
	{
		// 삼각형에서 최대 좌표를 구한다.
		if( i3Vector::GetY( pVecMax ) < i3Vector::GetY(pSrcVec ) )	{ i3Vector::SetY( pVecMax, i3Vector::GetY( pSrcVec ) );	return true;	}
	}
	else if( nStyle & I3_BSP_MINMAX_Z )
	{
		// 삼각형에서 최대 좌표를 구한다.
		if( i3Vector::GetZ( pVecMax ) < i3Vector::GetZ( pSrcVec ) )	{ i3Vector::SetZ( pVecMax, i3Vector::GetZ( pSrcVec ) );	return true;	}
	}

	return false;
}

bool i3OptBuildBSP::_CalcMinVec( VEC3D * pVecMin, VEC3D * pSrcVec, UINT32 nStyle )
{
	if( nStyle & I3_BSP_MINMAX_X )
	{
		// 삼각형에서 최소 좌표를 구한다.
		if( i3Vector::GetX( pVecMin ) > i3Vector::GetX( pSrcVec ) )	{ i3Vector::SetX( pVecMin, i3Vector::GetX( pSrcVec ) );	return true;	}
	}
	else if( nStyle & I3_BSP_MINMAX_Y )
	{
		// 삼각형에서 최소 좌표를 구한다.
		if( i3Vector::GetY( pVecMin ) > i3Vector::GetY(pSrcVec ) )	{ i3Vector::SetY( pVecMin, i3Vector::GetY( pSrcVec ) );	return true;	}
	}
	else if( nStyle & I3_BSP_MINMAX_Z )
	{
		// 삼각형에서 최소 좌표를 구한다.
		if( i3Vector::GetZ( pVecMin ) > i3Vector::GetZ( pSrcVec ) )	{ i3Vector::SetZ( pVecMin, i3Vector::GetZ( pSrcVec ) );	return true;	}
	}

	return false;
}

void i3OptBuildBSP::_BuildTriList( i3BSPNode * pNode, i3Geometry * pGeometry )
{
	UINT32	j;
	UINT32	nPrimCount;
	i3VertexArray * pVA;
	i3IndexArray *	pIA;
	i3BSPTriIndex * pTri;
	i3GeometryAttr * pGeometryAttr;
	i3RenderAttr * pRenderAttr;
	
	pRenderAttr = pGeometry->GetGeometryAttr( 0 );
	I3ASSERT( pRenderAttr );

	if( i3::kind_of<i3GeometryAttr*>(pRenderAttr))
	{
		pGeometryAttr = (i3GeometryAttr *) pRenderAttr;
	
		pVA = pGeometryAttr->GetVertexArray();
		pIA = pGeometryAttr->GetIndexArray();

		nPrimCount = pGeometryAttr->GetPrimitiveCount();

		pNode->setPartCount( (INT32) nPrimCount );

		if( pIA )
		{
			pIA->Lock();

			for( j = 0; j < nPrimCount; j++ )
			{
				pTri = (i3BSPTriIndex *) i3MemAlloc( sizeof( i3BSPTriIndex ) );
				I3ASSERT( pTri );

				pTri->m_pVA				= pVA;
				pTri->m_nIndex[0]		= (UINT16) pIA->GetIndex( j * 3 );
				pTri->m_nIndex[1]		= (UINT16) pIA->GetIndex( j * 3 + 1);
				pTri->m_nIndex[2]		= (UINT16) pIA->GetIndex( j * 3 + 2);

				pNode->setPart( j, pTri );
			}

			pIA->Unlock();
		}
		else
		{
			for( j = 0; j < nPrimCount; j++ )
			{
				pTri = (i3BSPTriIndex *) i3MemAlloc( sizeof( i3BSPTriIndex ) );
				I3ASSERT( pTri );

				pTri->m_pVA				= pVA;
				pTri->m_nIndex[0]		= (UINT16) ( j * 3 );
				pTri->m_nIndex[1]		= (UINT16) ( j * 3 + 1 );
				pTri->m_nIndex[2]		= (UINT16) ( j * 3 + 2 );

				pNode->setPart( j, pTri );
			}
		}
	}
}

// 삼각형의 최소 최대 영역 판별
void i3OptBuildBSP::_SortTriList( i3BSPNode * pNode, VEC4D * pPlane, MATRIX * pWorldMat )
{
	INT32	i;
	UINT32	nFlags;
	
	i3BSPTriIndex * pSrcTri;
	i3VertexArray * pVA;

	VEC3D	VecTriPos1, VecTriPos2, VecTriPos3;
	VEC3D	VecMin, VecMax;

	if( i3Vector::GetX( pPlane ) == 1.0f )			nFlags = I3_BSP_MINMAX_X;
	else if( i3Vector::GetY( pPlane ) == 1.0f )		nFlags = I3_BSP_MINMAX_Y;
	else											nFlags = I3_BSP_MINMAX_Z;
	
	for( i = 0; i < pNode->getPartCount(); i++ )
	{
		pSrcTri = (i3BSPTriIndex *) pNode->getPart( i );

		if( pSrcTri->m_pWorld == nullptr)
			pSrcTri->m_pWorld = pWorldMat;
		
		pVA = pSrcTri->m_pVA;
		pVA->Lock();

		pVA->GetPosition( pSrcTri->m_nIndex[ 0 ], &VecTriPos1 );
		pVA->GetPosition( pSrcTri->m_nIndex[ 1 ], &VecTriPos2 );
		pVA->GetPosition( pSrcTri->m_nIndex[ 2 ], &VecTriPos3 );

		pVA->Unlock();

		i3Vector::TransformCoord( &VecTriPos1, &VecTriPos1, pWorldMat );
		i3Vector::TransformCoord( &VecTriPos2, &VecTriPos2, pWorldMat );
		i3Vector::TransformCoord( &VecTriPos3, &VecTriPos3, pWorldMat );

		_CalcMinVec( &VecMin, &VecTriPos1, true );
		_CalcMaxVec( &VecMax, &VecTriPos1, true );
		pSrcTri->min = 0;
		pSrcTri->max = 0;

		if( _CalcMinVec( &VecMin, &VecTriPos2, nFlags ) )
			pSrcTri->min = 1;
		if( _CalcMaxVec( &VecMax, &VecTriPos2, nFlags ) )
			pSrcTri->max = 1;

		if( _CalcMinVec( &VecMin, &VecTriPos3, nFlags ) )
			pSrcTri->min = 2;
		if( _CalcMaxVec( &VecMax, &VecTriPos3, nFlags ) )
			pSrcTri->max = 2;
	}
}

void i3OptBuildBSP::_BuildTriObject( i3BSPGeometry * pOutBSPGeo, i3Geometry * pGeo )
{
	INT32	i;
	UINT32	j;
	UINT32	nPrimCount;
	i3VertexArray * pVA;
	i3IndexArray *	pIA;
	i3GeometryAttr * pGeoAttr;
	i3RenderAttr * pRenderAttr;
	
	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++ )
	{
		pRenderAttr = pGeo->GetGeometryAttr( i );

		if( i3::kind_of<i3GeometryAttr* >(pRenderAttr))
		{
			pGeoAttr = (i3GeometryAttr *) pRenderAttr;
		
			pVA = pGeoAttr->GetVertexArray();
			pIA = pGeoAttr->GetIndexArray();

			nPrimCount = pGeoAttr->GetPrimitiveCount();

			pOutBSPGeo->m_nTriCount	= nPrimCount;
			pOutBSPGeo->pTri		= (i3BSPTriIndex *) i3MemAlloc( sizeof(i3BSPTriIndex) * nPrimCount );

			if( pIA )
			{
				pIA->Lock();

				for( j = 0; j < nPrimCount; j++ )
				{
					pOutBSPGeo->pTri->m_pVA				= pVA;
					pOutBSPGeo->pTri->m_nIndex[0]		= (UINT16) pIA->GetIndex( j * 3 );
					pOutBSPGeo->pTri->m_nIndex[1]		= (UINT16) pIA->GetIndex( j * 3 + 1);
					pOutBSPGeo->pTri->m_nIndex[2]		= (UINT16) pIA->GetIndex( j * 3 + 2);
				}

				pIA->Unlock();
			}
			else
			{
				for( j = 0; j < nPrimCount; j++ )
				{
					pOutBSPGeo->pTri->m_pVA				= pVA;
					pOutBSPGeo->pTri->m_nIndex[0]		= (UINT16) ( j * 3 );
					pOutBSPGeo->pTri->m_nIndex[1]		= (UINT16) ( j * 3 + 1 );
					pOutBSPGeo->pTri->m_nIndex[2]		= (UINT16) ( j * 3 + 2 );
				}
			}
		}
	}
}

void i3OptBuildBSP::_CalcSpaceBound( i3BSPNode * pRoot )
{
	INT32	i;
	VEC3D	VecMin, VecMax;
	
	i3BSPGeometry * pBSPGeo;
	i3BoundBox *	pBox;

	// 플래너 생성을 위한 최대 최소 영역 설정
	// 쇼팅
	// 이미 기준 플래너가 세팅되어 있어야 한다.
	pBSPGeo = (i3BSPGeometry *) pRoot->getPart( 0 );
	pBox	= pBSPGeo->pBoundBox;

	// 최소 결정
	_CalcMinVec( &VecMin, pBox->GetMin(), true );
	// 최대 결정
	_CalcMaxVec( &VecMax, pBox->GetMax(), true );

	for( i = 1; i < pRoot->getPartCount(); i++ )
	{	
		pBSPGeo = (i3BSPGeometry*) pRoot->getPart( i );
		pBox	= pBSPGeo->pBoundBox;

		_CalcMinVec( &VecMin, pBox->GetMin(), false );
		_CalcMaxVec( &VecMax, pBox->GetMax(), false);
	}

	pBox = pRoot->GetBound();
	if( pBox == nullptr)
	{
		pBox = i3BoundBox::new_object();
		I3ASSERT( pBox );
	}
	else
	{
		I3_MUST_ADDREF(pBox);
	}

	pBox->SetMinMax( &VecMin, &VecMax );

	pRoot->SetBound( pBox );
}

// 플래너 세팅
// 이미 쇼팅되어 있어야한다.
void i3OptBuildBSP::_CalcPlane( VEC4D * pOutPlane, i3BSPNode * pParent, UINT32 nType )
{
	INT32	i;
	VEC3D	vMax, vMin;
	i3BoundBox * pBoundBox;
	i3BSPGeometry * pBSPGeo;
	i3BSPTriIndex * pTri;

	// 플래너 세팅전에 쇼팅되어 있어야 합니다.
	if( nType & I3_BSP_TYPE_GEOMETRY )
	{
		// 공간상의 중심으로 한다..
		if( nType & I3_BSP_DIVIDE_OBJCOUNT )
		{
			if( pParent->getPartCount() > 1 )
			{
				pBSPGeo = (i3BSPGeometry*) pParent->getPart( pParent->getPartCount() / 2 );
			}
			else
			{
				// 오브젝트가 하나뿐이다.. 공간중심으로 분할 세팅
				pBSPGeo = (i3BSPGeometry*) pParent->getPart( 0 );
			}

			pBoundBox = pBSPGeo->pBoundBox;
			I3ASSERT( pBoundBox );
			i3Vector::Add( &m_MidPoint, pBoundBox->GetMax(), pBoundBox->GetMin() );
			i3Vector::Scale( &m_MidPoint, &m_MidPoint, 0.5f );
		}
		else if( nType & I3_BSP_DIVIDE_SPACE )
		{
			// 먼저 전체 공간을 구한다.
			pBSPGeo = (i3BSPGeometry *) pParent->getPart( 0 );
			
			pBoundBox = pBSPGeo->pBoundBox;
			I3ASSERT( pBoundBox );

			_CalcMinVec( &vMin, pBoundBox->GetMin(), true );
			_CalcMaxVec( &vMax, pBoundBox->GetMax(), true );

			if( pParent->getPartCount() > 1 )
			{
				for( i = 1; i < pParent->getPartCount(); i++ )
				{
					pBSPGeo = (i3BSPGeometry *) pParent->getPart( i );

					pBoundBox = pBSPGeo->pBoundBox;
					I3ASSERT( pBoundBox );

					_CalcMinVec( &vMin, pBoundBox->GetMin(), false );
					_CalcMaxVec( &vMax, pBoundBox->GetMax(), false );
				}
			}
			
			i3Vector::Add( &m_MidPoint, &vMax, &vMin );
			i3Vector::Scale( &m_MidPoint, &m_MidPoint, 0.5f );
		}
		else if( nType & I3_BSP_DIVIDE_BOUNDBOX )
		{
			pBoundBox = pParent->GetBound();
			I3ASSERT( pBoundBox );

			i3Vector::Add( &m_MidPoint, pBoundBox->GetMin(), pBoundBox->GetMax() );
			i3Vector::Scale( &m_MidPoint, &m_MidPoint, 0.5f );
		}
	}
	else
	{	
		pTri = (i3BSPTriIndex *) pParent->getPart( pParent->getPartCount() / 2 );
		pTri->m_pVA->Lock();
		pTri->m_pVA->GetPosition( pTri->m_nIndex[ pTri->min ], &m_MidPoint );
		pTri->m_pVA->Unlock();
	}

	// 자식 노드에 세팅할 노드 타입
	if( nType & I3_BSP_MINMAX_X )
	{
		i3Vector::Set( pOutPlane, 1.0f, 0.0f, 0.0f, i3Vector::GetX( &m_MidPoint ) );
	}
	else if( nType & I3_BSP_MINMAX_Y )
	{
		i3Vector::Set( pOutPlane, 0.0f, 1.0f, 0.0f, i3Vector::GetY( &m_MidPoint ) );
	}
	else if( nType & I3_BSP_MINMAX_Z )
	{
		i3Vector::Set( pOutPlane, 0.0f, 0.0f, 1.0f, i3Vector::GetZ( &m_MidPoint ) );
	}
	else
	{
		I3ASSERT_0;
	}
}

void i3OptBuildBSP::_FindNewVertex( VEC3D * pOut, VEC3D * pDef, VEC3D * pDest, REAL32 d, REAL32 f, REAL32 w)
{
	VEC3D	TempVec;
	REAL32	temp1, temp2;

	temp1 = i3Math::abs( w - d );
	temp2 = i3Math::abs( w - f );
	i3Vector::Sub( &TempVec, pDest, pDef );
	i3Vector::Scale( &TempVec, &TempVec, temp1 / (temp1 + temp2) );
	i3Vector::Add( pOut, pDef, &TempVec );
}

void i3OptBuildBSP::_AddBSPVertex( i3::vector<i3BSPVertex*>& VertexList, VEC3D * pPos, i3BSPTriIndex * pTri, INT32 idx )
{
	i3BSPVertex * pNewVertex;

	pNewVertex = (i3BSPVertex*) i3MemAlloc( sizeof(i3BSPVertex) );

	i3Vector::Copy( &pNewVertex->m_Pos, pPos );
	if( pTri->m_pVA->HasNormal() )
		pTri->m_pVA->GetNormal( pTri->m_nIndex[ idx ], &pNewVertex->m_Normal );
	if( pTri->m_pVA->HasColor() )
		pTri->m_pVA->GetColor( pTri->m_nIndex[ idx ], &pNewVertex->m_Color );
	if( pTri->m_pVA->GetTextureCoordCount() > 0 )
		pTri->m_pVA->GetTextureCoord( 0, pTri->m_nIndex[ idx ], &pNewVertex->m_UV );

	VertexList.push_back( pNewVertex );
}

void i3OptBuildBSP::_AddBSPTri( i3::vector<i3BSPTriIndex*>& OutList, UINT16 idx1, UINT16 idx2, UINT16 idx3 )
{
	i3BSPTriIndex * pNewTri;

	pNewTri = (i3BSPTriIndex*) i3MemAlloc( sizeof( i3BSPTriIndex ) );
	pNewTri->m_nIndex[0]	= idx1;
	pNewTri->m_nIndex[1]	= idx2;
	pNewTri->m_nIndex[2]	= idx3;

	OutList.push_back( pNewTri );
}

// 리턴이 있다면 추가된 Tri
INT32 i3OptBuildBSP::_DivideTri( i3::vector<i3BSPTriIndex*>& OutList, i3BSPTriIndex * pTri, i3::vector<i3BSPVertex*>& NewVertexList )
{
	VEC3D	Vec1, Vec2, Vec3, NewVec1, NewVec2;
	VEC3D	vD1, vD2, vF;
	VEC3D	v1, v2, v3;
	VEC3D	* pv1, * pv2,  *pv3;

	MATRIX	InvMat;

	INT32	nSid1, nSid2, nSid3;
	UINT16	nIdx1, nIdx2, nIdx3;
	
	REAL32	i1, i2, i3;
	REAL32	d1, d2, f;
	REAL32	x, y, z, w;
	
	INT32	nCount = 0;

	i3VertexArray * pVA;

	if( pTri->m_pDividePlane )
	{
		pVA	= pTri->m_pVA;

		i3Matrix::Inverse( &InvMat, 0, pTri->m_pWorld );
		// 버택스 인덱스
		nIdx1	= pTri->m_nIndex[0];
		nIdx2	= pTri->m_nIndex[1];
		nIdx3	= pTri->m_nIndex[2];

		I3ASSERT( nIdx1 < pVA->GetCount() );
		I3ASSERT( nIdx2 < pVA->GetCount() );
		I3ASSERT( nIdx3 < pVA->GetCount() );

		pVA->Lock();

		pVA->GetPosition( nIdx1, &v1 );
		pVA->GetPosition( nIdx2, &v2 );
		pVA->GetPosition( nIdx3, &v3 );

		i3Vector::TransformCoord( &Vec1, &v1, pTri->m_pWorld );
		i3Vector::TransformCoord( &Vec2, &v2, pTri->m_pWorld );
		i3Vector::TransformCoord( &Vec3, &v3, pTri->m_pWorld );

		x = i3Vector::GetX( pTri->m_pDividePlane );
		y = i3Vector::GetY( pTri->m_pDividePlane );
		z = i3Vector::GetZ( pTri->m_pDividePlane );
		w = i3Vector::GetW( pTri->m_pDividePlane );
		
		if( x == 1.0f )
		{
			// x축
			i1 = i3Vector::GetX( &Vec1 );
			i2 = i3Vector::GetX( &Vec2 );
			i3 = i3Vector::GetX( &Vec3 );
		}
		else if( y == 1.0f )
		{
			// y축
			i1 = i3Vector::GetY( &Vec1 );
			i2 = i3Vector::GetY( &Vec2 );
			i3 = i3Vector::GetY( &Vec3 );
		}
		else
		{
			// z축
			i1 = i3Vector::GetZ( &Vec1 );
			i2 = i3Vector::GetZ( &Vec2 );
			i3 = i3Vector::GetZ( &Vec3 );
		}

		if( i1 <= w )		nSid1	= 1;
		else				nSid1	= 2;

		if( i2 <= w )		nSid2	= 1;
		else				nSid2	= 2;

		if( i3 <= w )		nSid3	= 1;
		else				nSid3	= 2;

		if( nSid1 != nSid2 )
		{
			if( nSid2 == nSid3 )
			{
				// v1 is alone
				i3Vector::Copy( &vF, &Vec1 );
				i3Vector::Copy( &vD1, &Vec2 );
				i3Vector::Copy( &vD2, &Vec3 );

				pv1	= &v1;
				pv2 = &v2;
				pv3 = &v3;

				f	= i1;
				d1	= i2;
				d2	= i3;

				if( pTri->m_nSide == 1 )
				{
					if( nSid1 == 1 )	nCount = 1;
					else				nCount = 2;
				}
				else
				{
					if( nSid1 == 1 )	nCount = 2;
					else				nCount = 1;
				}
			}
			else
			{
				// v2 is alone
				i3Vector::Copy( &vF, &Vec2 );
				i3Vector::Copy( &vD1, &Vec3 );
				i3Vector::Copy( &vD2, &Vec1 );

				pv1	= &v2;
				pv2 = &v3;
				pv3 = &v1;

				f	= i2;
				d1	= i3;
				d2	= i1;

				if( pTri->m_nSide == 1 )
				{
					if( nSid2 == 1 )	nCount = 1;
					else				nCount = 2;
				}
				else
				{
					if( nSid2 == 1 )	nCount = 2;
					else				nCount = 1;
				}
			}
		}
		else
		{
			// v3 is alone
			i3Vector::Copy( &vF, &Vec3 );
			i3Vector::Copy( &vD1, &Vec1 );
			i3Vector::Copy( &vD2, &Vec2 );

			pv1	= &v3;
			pv2 = &v1;
			pv3 = &v2;

			f	= i3;
			d1	= i1;
			d2	= i2;

			if( pTri->m_nSide == 1 )
			{
				if( nSid3 == 1 )	nCount = 1;
				else				nCount = 2;
			}
			else
			{
				if( nSid3 == 1 )	nCount = 2;
				else				nCount = 1;
			}
		}

		_FindNewVertex( &NewVec1, &vD1, &vF, d1, f, w );
		_FindNewVertex( &NewVec2, &vD2, &vF, d2, f, w );

		i3Vector::TransformCoord( &NewVec1, &NewVec1, &InvMat );
		i3Vector::TransformCoord( &NewVec2, &NewVec2, &InvMat );

		// 버택스를 만듭니다.( 일단 Pos, Normal만 세팅해줍시다..)
		if( nCount == 2 )
		{
			_AddBSPVertex( NewVertexList, pv2, pTri, 0 );
			_AddBSPVertex( NewVertexList, pv3, pTri, 1 );
			_AddBSPVertex( NewVertexList, &NewVec1, pTri, 2 );
			_AddBSPVertex( NewVertexList, &NewVec2, pTri, 2 );

			// 1, 2, N2
			_AddBSPTri( OutList, (UINT16) (NewVertexList.size() - 4),
				(UINT16) (NewVertexList.size() - 3),
				(UINT16) (NewVertexList.size() - 1) );

			// 1, N2, N1
			_AddBSPTri( OutList, (UINT16) (NewVertexList.size() - 4),
				(UINT16) (NewVertexList.size() - 1),
				(UINT16) (NewVertexList.size() - 2) );
		}
		else if( nCount == 1 )
		{
			// 버택스를 만듭니다.( 일단 Pos, Normal만 세팅해줍시다..)
			_AddBSPVertex( NewVertexList, pv1,		 pTri, 2 );
			_AddBSPVertex( NewVertexList, &NewVec1, pTri, 0 );
			_AddBSPVertex( NewVertexList, &NewVec2, pTri, 1 );

			_AddBSPTri( OutList, (UINT16) (NewVertexList.size() - 3),
				(UINT16) (NewVertexList.size() - 2),
				(UINT16) (NewVertexList.size() - 1) );
		}

		pVA->Unlock();
	}

	pTri->m_nFlag = nCount;

	return nCount;
}

void i3OptBuildBSP::_SetGeometryName( i3Geometry * pOut, i3Geometry * pSrcGeo, INT32 nSide )
{
	INT32	i, nameLen;
	char	pNewName[256];
	const char *	pName;
	char	addNameNum[32];

	pName = pSrcGeo->GetName();
	nameLen = i3::generic_string_size( pName );

	memset( pNewName, 0, sizeof(pNewName) );

	if( m_nGeoCount > 1 )
	{
		I3ASSERT( nameLen <= sizeof(pNewName));
		for( i = 0; i < nameLen; i++ )
		{
			if( pName[i] == ':' )
				break;

			pNewName[i] = pName[i];
		}
	}
	else
		i3::safe_string_copy( pNewName, pName, 256 );
	
	sprintf( addNameNum, ":%d_%d", m_nGeoCount, nSide );
	i3::generic_string_cat( pNewName, addNameNum  );

	pOut->SetName( (const char *) pNewName );
}

void i3OptBuildBSP::_CreateGeometry( i3Geometry * pOutGeo, i3BSPNode * pNode, i3Geometry * pSrcGeometry, INT32 nFlag )
{
	INT32	j;

	i3GeometryAttr * pGeoAttr, * pSrcGeoAttr;
	i3VertexArray * pVA;
	i3VertexArray * pSrcVA;

	i3IndexArray *	pIA		= nullptr;
	i3IndexArray *	pSrcIA	= nullptr;

	i3VertexFormat * pFormat;

	INT32		*	pVertexMap = nullptr;

	i3BSPTriIndex * pPartIndex;
	i3BSPVertex *	pVertex;

	i3::vector<i3BSPVertex*>	NewVertexList;
	i3::vector<i3BSPTriIndex*>	AddableList;

	COLORREAL	Color;
	VEC3D		v1, v2, v3;
	VEC2D		Vec2;

	INT32	nDivideCount;
	INT32	nPartCount;
	INT32	nVertexCount;
	INT32	nFlagCount;
	INT32	nMaxVertexCount;
	INT32	nPrimCount;

	INT32	nCount;
	INT32	nIndexCount;

	UINT16	idx1, idx2, idx3;

	nPartCount	= pNode->getPartCount();

	I3ASSERT( nPartCount );
	
	I3ASSERT( pOutGeo );
	I3_MUST_ADDREF(pOutGeo);
	pOutGeo->SetFlag( pSrcGeometry->GetFlag() );

	_SetGeometryName( pOutGeo, pSrcGeometry, nFlag );
	
	// create geometryattr
	pGeoAttr = i3GeometryAttr::new_object();
	
	nPrimCount	= 0;
	{
		pSrcGeoAttr = (i3GeometryAttr *) pSrcGeometry->GetGeometryAttr( 0 );
		pSrcVA = pSrcGeoAttr->GetVertexArray();
		pSrcIA	= pSrcGeoAttr->GetIndexArray();

		pFormat = pSrcVA->GetFormat();

		if( m_nStyle & I3_BSP_OPT_COLOR )
			pFormat->SetHasColor( false );
		if( m_nStyle & I3_BSP_OPT_TEXCOORD )
		{
			pFormat->SetTextureCoordCount( 0 );
			pFormat->SetTextureCoordSetCount( 0 );
		}

		// 만약 분할되야 한다면..?
		nDivideCount	= 0;		// 분할 횟수
		nVertexCount	= 0;		// 추가된 버택스 수
		if( m_nStyle & I3_BSP_NODE_BINARY )
		{
			for(INT32 i = 0; i < nPartCount; i++ )
			{
				pPartIndex = (i3BSPTriIndex *) pNode->getPart( i );

				pPartIndex->m_nSide = nFlag + 1;
				pPartIndex->m_nFlag	= 0;
				nVertexCount = _DivideTri( AddableList, pPartIndex, NewVertexList );
				if( nVertexCount > 0 )		nDivideCount++;
			}
		}

		// 버택스를 새로 만들어준다.
		pVA = i3VertexArray::new_object();
		I3ASSERT( pVA );
		// 분할 횟수만큼 해당 버택스는 대체되므로 빼줘야된다.
		nMaxVertexCount	= pSrcVA->GetCount();

		// check used vertex
		pVertexMap = (INT32 *) i3MemAlloc( sizeof( INT32 ) * nMaxVertexCount );
		i3mem::Fill( pVertexMap, -1, sizeof( INT32 ) * nMaxVertexCount );

		nMaxVertexCount = 0;
		for( INT32 i = 0; i < nPartCount; i++ )
		{
			pPartIndex = (i3BSPTriIndex *) pNode->getPart( i );

			if( pVertexMap[ pPartIndex->m_nIndex[ 0 ] ] == -1 )
			{
				pVertexMap[ pPartIndex->m_nIndex[ 0 ] ] = nMaxVertexCount;
				nMaxVertexCount++;
			}

			if( pVertexMap[ pPartIndex->m_nIndex[ 1 ] ] == -1 )
			{
				pVertexMap[ pPartIndex->m_nIndex[ 1 ] ] = nMaxVertexCount;
				nMaxVertexCount++;
			}


			if( pVertexMap[ pPartIndex->m_nIndex[ 2 ] ] == -1 )
			{
				pVertexMap[ pPartIndex->m_nIndex[ 2 ] ] = nMaxVertexCount;
				nMaxVertexCount++;
			}

			nPrimCount++;
		}

		nPrimCount += INT32(NewVertexList.size()) / 3;

		pVA->Create( pFormat, nMaxVertexCount + NewVertexList.size(), pSrcVA->GetUsageFlag() );
		
		pVA->Lock();
		pSrcVA->Lock();

		// 포지션
		if( pVA->HasPosition() )
		{
			for(INT32 i = 0; i < pNode->getPartCount(); i++ )
			{
				pPartIndex = (i3BSPTriIndex *) pNode->getPart( i );
				
				idx1 = pPartIndex->m_nIndex[0];
				idx2 = pPartIndex->m_nIndex[1];
				idx3 = pPartIndex->m_nIndex[2];

				I3ASSERT( pVertexMap[ idx1 ] >= 0 );
				I3ASSERT( pVertexMap[ idx2 ] >= 0 );
				I3ASSERT( pVertexMap[ idx3 ] >= 0 );

				pSrcVA->GetPosition( idx1, &v1 );
				pVA->SetPosition( pVertexMap[ idx1 ], &v1 );
				
				pSrcVA->GetPosition( idx2, &v2 );
				pVA->SetPosition( pVertexMap[ idx2 ], &v2 );
				
				pSrcVA->GetPosition( idx3, &v3 );
				pVA->SetPosition( pVertexMap[ idx3 ], &v3 );
			}

			for(size_t i = 0; i < AddableList.size(); i++ )
			{
				pPartIndex = AddableList[i];

				idx1 = pPartIndex->m_nIndex[0];
				idx2 = pPartIndex->m_nIndex[1];
				idx3 = pPartIndex->m_nIndex[2];

				pVertex = NewVertexList[ idx1 ];
				pVA->SetPosition( nMaxVertexCount + idx1, &pVertex->m_Pos );
				
				pVertex = NewVertexList[ idx2 ];
				pVA->SetPosition( nMaxVertexCount + idx2, &pVertex->m_Pos );
				
				pVertex = NewVertexList[ idx3 ];
				pVA->SetPosition( nMaxVertexCount + idx3, &pVertex->m_Pos );
			}
		}

		// 노말
		if( pVA->HasNormal() )
		{	
			for( INT32 i = 0; i < pNode->getPartCount(); i++ )
			{
				pPartIndex = (i3BSPTriIndex *) pNode->getPart( i );
				
				idx1	= pPartIndex->m_nIndex[0];
				idx2	= pPartIndex->m_nIndex[1];
				idx3	= pPartIndex->m_nIndex[2];

				I3ASSERT( pVertexMap[ idx1 ] >= 0 );
				I3ASSERT( pVertexMap[ idx2 ] >= 0 );
				I3ASSERT( pVertexMap[ idx3 ] >= 0 );

				pSrcVA->GetNormal( idx1, &v1 );
				pVA->SetNormal( pVertexMap[ idx1 ], &v1 );
				
				pSrcVA->GetNormal( idx2, &v2 );
				pVA->SetNormal( pVertexMap[ idx2 ], &v2 );
				
				pSrcVA->GetNormal( idx3, &v3 );
				pVA->SetNormal( pVertexMap[ idx3 ], &v3 );
			}
			
			for( size_t i = 0; i < AddableList.size(); i++ )
			{
				pPartIndex = AddableList[i];

				idx1 = pPartIndex->m_nIndex[0];
				idx2 = pPartIndex->m_nIndex[1];
				idx3 = pPartIndex->m_nIndex[2];

				pVertex = NewVertexList[ idx1 ];
				pVA->SetNormal( nMaxVertexCount + idx1, &pVertex->m_Normal );

				pVertex = NewVertexList[ idx2 ];
				pVA->SetNormal( nMaxVertexCount + idx2, &pVertex->m_Normal );
				
				pVertex = NewVertexList[ idx3 ];
				pVA->SetNormal( nMaxVertexCount + idx3, &pVertex->m_Normal );
			}
		}

		// 컬러
		if( pVA->HasColor() )
		{
			for(INT32 i = 0; i < pNode->getPartCount(); i++ )
			{
				pPartIndex = (i3BSPTriIndex *) pNode->getPart( i );

				idx1 = pPartIndex->m_nIndex[0];
				idx2 = pPartIndex->m_nIndex[1];
				idx3 = pPartIndex->m_nIndex[2];

				I3ASSERT( pVertexMap[ idx1 ] >= 0 );
				I3ASSERT( pVertexMap[ idx2 ] >= 0 );
				I3ASSERT( pVertexMap[ idx3 ] >= 0 );

				pSrcVA->GetColor( idx1, &Color );
				pVA->SetColor( pVertexMap[ idx1 ], &Color );
				
				pSrcVA->GetColor( idx2, &Color );
				pVA->SetColor( pVertexMap[ idx2 ], &Color );
				
				pSrcVA->GetColor( idx3, &Color );
				pVA->SetColor( pVertexMap[ idx3 ], &Color );
			}
			
			for(size_t i = 0; i < AddableList.size(); i++ )
			{
				pPartIndex = AddableList[i];

				idx1 = pPartIndex->m_nIndex[0];
				idx2 = pPartIndex->m_nIndex[1];
				idx3 = pPartIndex->m_nIndex[2];

				pVertex = NewVertexList[ idx1 ];
				pVA->SetColor( nMaxVertexCount + idx1, &pVertex->m_Color );
				
				pVertex = NewVertexList[ idx2 ];
				pVA->SetColor( nMaxVertexCount + idx2, &pVertex->m_Color );
				
				pVertex = NewVertexList[ idx3 ];
				pVA->SetColor( nMaxVertexCount + idx3, &pVertex->m_Color );
			}
		}

		// UV 세팅
		for( j = 0; j < (INT32) pFormat->GetTextureCoordSetCount(); j++ )
		{	
			for(INT32 i = 0; i < pNode->getPartCount(); i++ )
			{
				pPartIndex = (i3BSPTriIndex *) pNode->getPart( i );
				
				idx1 = pPartIndex->m_nIndex[0];
				idx2 = pPartIndex->m_nIndex[1];
				idx3 = pPartIndex->m_nIndex[2];

				I3ASSERT( pVertexMap[ idx1 ] >= 0 );
				I3ASSERT( pVertexMap[ idx2 ] >= 0 );
				I3ASSERT( pVertexMap[ idx3 ] >= 0 );

				pSrcVA->GetTextureCoord( j, idx1, &Vec2 );
				pVA->SetTextureCoord( j, pVertexMap[ idx1 ], &Vec2 );
				
				pSrcVA->GetTextureCoord( j, idx2, &Vec2 );
				pVA->SetTextureCoord( j, pVertexMap[ idx2 ], &Vec2 );

				pSrcVA->GetTextureCoord( j, idx3, &Vec2 );
				pVA->SetTextureCoord( j, pVertexMap[ idx3 ], &Vec2 );
			}

			for(size_t i = 0; i < AddableList.size(); i++ )
			{
				pPartIndex = AddableList[i];

				idx1 = pPartIndex->m_nIndex[0];
				idx2 = pPartIndex->m_nIndex[1];
				idx3 = pPartIndex->m_nIndex[2];

				pVertex = NewVertexList[idx1 ];
				pVA->SetTextureCoord( j, nMaxVertexCount + idx1, &pVertex->m_UV );
				
				pVertex = NewVertexList[idx2 ];
				pVA->SetTextureCoord( j, nMaxVertexCount + idx2, &pVertex->m_UV );
				
				pVertex = NewVertexList[idx3 ];
				pVA->SetTextureCoord( j, nMaxVertexCount + idx3, &pVertex->m_UV );
			}
		}

		pSrcVA->Unlock();
		pVA->Unlock();

		pGeoAttr->SetVertexArray( pVA, pSrcGeoAttr->GetPrimitiveType(), nPrimCount, 0 );

		// 인덱스어레이
		if( pSrcIA != nullptr )
		{	
			pIA = i3IndexArray::new_object();
			pIA->Create( ( nPartCount + INT32(AddableList.size()) - nDivideCount) * 3, pSrcIA->GetUsageFlag(), pVA->GetCount() );

			pIA->Lock();
			// 인덱스 세팅
			nDivideCount	= 0;
			nCount			= 0;
			nIndexCount		= 0;
			for( INT32 i = 0; i < nPartCount; i++ )
			{
				pPartIndex = (i3BSPTriIndex *) pNode->getPart( i );

				if( pPartIndex->m_nFlag == 0 )
				{	
					idx1 = pPartIndex->m_nIndex[ 0 ];
					idx2 = pPartIndex->m_nIndex[ 1 ];
					idx3 = pPartIndex->m_nIndex[ 2 ];

					I3ASSERT( pVertexMap[ idx1 ] >= 0 );
					I3ASSERT( pVertexMap[ idx2 ] >= 0 );
					I3ASSERT( pVertexMap[ idx3 ] >= 0 );

					pIA->SetIndex( nIndexCount * 3,		pVertexMap[ idx1 ] );
					pIA->SetIndex( nIndexCount * 3 + 1, pVertexMap[ idx2 ] );
					pIA->SetIndex( nIndexCount * 3 + 2,	pVertexMap[ idx3 ] );
				}
				else if( pPartIndex->m_nFlag > 0 )
				{
					nFlagCount = pPartIndex->m_nFlag;
					for( j = 0; j < nFlagCount; j++ )
					{	
						if( j == 1 )
							nIndexCount++;

						pPartIndex = AddableList[nCount];
						pIA->SetIndex( nIndexCount * 3,		nMaxVertexCount + pPartIndex->m_nIndex[ 0 ] );
						pIA->SetIndex( nIndexCount * 3 + 1, nMaxVertexCount + pPartIndex->m_nIndex[ 1 ] );
						pIA->SetIndex( nIndexCount * 3 + 2, nMaxVertexCount + pPartIndex->m_nIndex[ 2 ] );
						
						nCount++;
					}
				}
				else
				{
					I3ASSERT_0;
				}

				nIndexCount++;
			}

			pIA->Unlock();

			pGeoAttr->SetIndexArray( pIA );
		}
		
		pOutGeo->AppendGeometryAttr( (i3RenderAttr *) pGeoAttr );

		I3MEM_SAFE_FREE_POINTER( pVertexMap );
	}

	// release
	for(size_t i = 0; i < NewVertexList.size(); i++ )
	{
		pVertex = NewVertexList[i];
		I3MEM_SAFE_FREE_POINTER( pVertex );
	}
	NewVertexList.clear();
	
	for(size_t i = 0; i < AddableList.size(); i++ )
	{
		pPartIndex = AddableList[i];
		I3MEM_SAFE_FREE_POINTER( pPartIndex );
	}
	AddableList.clear();
}

i3BoundBox * i3OptBuildBSP::_CreateBoundBoxForWorld( i3Geometry * pGeometry, MATRIX * pWorldMat )
{
	INT32	i;
	VEC3D	vMin, vMax;
	VEC3D	pt[ 8 ];
	VEC3D	* pMin, * pMax;

	i3BoundBox *	pBox		= nullptr;
	i3BoundBox *	pForBSPBox	= nullptr;

	pBox = pGeometry->GetBound();

	if( pBox == nullptr)
	{
		pBox = i3BoundBox::new_object();
		I3_MUST_ADDREF(pBox);
		pGeometry->GetWrappingBound( pBox );
		pGeometry->SetBound( pBox );
	}
	else
	{
		I3_MUST_ADDREF(pBox);
		pGeometry->GetWrappingBound( pBox );
	}

	pMin = pBox->GetMin();
	pMax = pBox->GetMax();

	i3Vector::Set( &pt[0], i3Vector::GetX( pMin), i3Vector::GetY( pMin), i3Vector::GetZ( pMin));
	i3Vector::Set( &pt[1], i3Vector::GetX( pMin), i3Vector::GetY( pMin), i3Vector::GetZ( pMax));
	i3Vector::Set( &pt[2], i3Vector::GetX( pMin), i3Vector::GetY( pMax), i3Vector::GetZ( pMin));
	i3Vector::Set( &pt[3], i3Vector::GetX( pMin), i3Vector::GetY( pMax), i3Vector::GetZ( pMax));
	i3Vector::Set( &pt[4], i3Vector::GetX( pMax), i3Vector::GetY( pMin), i3Vector::GetZ( pMin));
	i3Vector::Set( &pt[5], i3Vector::GetX( pMax), i3Vector::GetY( pMin), i3Vector::GetZ( pMax));
	i3Vector::Set( &pt[6], i3Vector::GetX( pMax), i3Vector::GetY( pMax), i3Vector::GetZ( pMin));
	i3Vector::Set( &pt[7], i3Vector::GetX( pMax), i3Vector::GetY( pMax), i3Vector::GetZ( pMax));

	i3Matrix::TransformPoints( pt, pt, 8, pWorldMat );

	// 최대 최소를 결정
	_CalcMinVec( &vMin, &pt[ 0 ], true );
	_CalcMaxVec( &vMax, &pt[ 0 ], true );

	for( i = 1; i < 8; i++ )
	{
		_CalcMinVec( &vMin, &pt[ i ], false );
		_CalcMaxVec( &vMax, &pt[ i ], false );
	}

	pForBSPBox = i3BoundBox::new_object();
	pForBSPBox->Reset();
	pForBSPBox->SetMinMax( &vMin, &vMax );

	return pForBSPBox;
}

bool i3OptBuildBSP::_CheckRemoveOfVertex( i3::vector<i3BSPTriIndex*>& OutList, const i3::vector<i3BSPTriIndex*>& List, i3VertexArray * pVA, VEC3D * pVec, VEC3D * pVec2, INT32 idx )
{
	INT32	nVertexNum = 0;
	VEC3D	v1, v2, v3;
	VEC3D	vEdge;
	bool	bCheck;

	i3BSPTriIndex * pDestTri;
	
	for(size_t i = 0; i < List.size(); i++ )
	{
		pDestTri = List[i];

		pVA->GetPosition( pDestTri->m_nIndex[ 0 ], &v1 );
		pVA->GetPosition( pDestTri->m_nIndex[ 1 ], &v2 );
		pVA->GetPosition( pDestTri->m_nIndex[ 2 ], &v3 );

		// 꼭지점이 같은지 비교
		bCheck = false;
		if( i3Vector::IsSame( pVec, &v1 ) )
		{
			nVertexNum	= 1;
			bCheck = true;
		}
		else
		{
			if( i3Vector::IsSame( pVec, &v2 ) )
			{
				nVertexNum	= 2;
				bCheck = true;
			}
			else
			{
				if( i3Vector::IsSame( pVec, &v3 ) )
				{
					nVertexNum	= 3;
					bCheck = true;
				}
			}
		}

		if( bCheck == true )
		{
			bCheck = false;
			// 빗변을 검사
			if( i3Vector::IsSame( pVec2, &v1 ) )
			{
				if( nVertexNum == 2 )	// 다음 검색에 사용할 빗변
					i3Vector::Copy( &vEdge, &v3 );
				else
					i3Vector::Copy( &vEdge, &v2 );

				bCheck = true;
			}
			else
			{
				if( i3Vector::IsSame( pVec2, &v2 ) )
				{
					if( nVertexNum == 1 )
						i3Vector::Copy( &vEdge, &v3 );
					else
						i3Vector::Copy( &vEdge, &v1 );

					bCheck = true;
				}
				else
				{
					if( i3Vector::IsSame( pVec2, &v3 ) )
					{
						if( nVertexNum == 1 )
							i3Vector::Copy( &vEdge, &v2 );
						else
							i3Vector::Copy( &vEdge, &v1 );

						bCheck = true;
					}
				}
			}

			if( bCheck == true )
			{// 꼭지점을 공유하며 접한 Tri이다
				OutList.push_back( pDestTri );
				if( INT32(i) == idx )	// 처음의 기준이 된 Tri로 돌아온다면 꼭지점을 없앨 수 있다.
					return true;
				else	// 이 Tri를 기준으로 다음 Tri를 검색한다.
					return _CheckRemoveOfVertex( OutList, List, pVA, pVec, &vEdge, idx );
			}
		}
	}

	return false;
}

void i3OptBuildBSP::_RemoveOfVertex( const i3::vector<i3BSPTriIndex*>& List, i3VertexArray * pVA )
{
	INT32	j, m;
	INT32	nEdgeIdx;
	VEC3D	vVertex, vEdge;
	VEC3D	vTemp;
	i3::vector<i3BSPTriIndex*>	AttachList;
	i3BSPTriIndex * pTri;

//	List	NewList; 쓰이지 않음.

	for(size_t i = 0; i < List.size(); i++ )
	{
		pTri = List[i];

		// 3번 검사
		for( j = 0; j < 3; j ++ )
		{
			pVA->GetPosition( pTri->m_nIndex[ j ], &vVertex );
			if( j == 2 )
				nEdgeIdx = pTri->m_nIndex[ 0 ];
			else
				nEdgeIdx = pTri->m_nIndex[ j + 1 ];
			
			pVA->GetPosition( nEdgeIdx, &vEdge );

			AttachList.clear();

			if( _CheckRemoveOfVertex( AttachList, List, pVA, &vVertex, &vEdge, i ) )
			{
				// 꼭지점을 기준으로 붙어 있는 Tri이다.
				// 기준 Tri의 한 점을 기준으로 붙여준다.
				for(size_t k = 0;  k < AttachList.size(); k++ )
				{
					pTri = AttachList[k];

					for( m = 0; m < 3; m++ )
					{
						pVA->GetPosition( pTri->m_nIndex[ m ], &vTemp );

						if( i3Vector::IsSame( &vVertex, &vTemp ) )
						{
							pTri->m_nIndex[ m ] = (UINT16) nEdgeIdx;
							break;
						}
					}
				}
			}
		}
	}
}

void i3OptBuildBSP::_SummaryForCollision( i3Geometry * pGeometry )
{
	INT32	i;
	UINT32	j, k;
	i3RenderAttr * pRenderAttr;
	i3GeometryAttr * pGeoAttr;

	i3VertexArray *	pVA;
	i3IndexArray *	pIA;

	VEC3D	v1, v2, v3;
	VEC3D	vNormal, vDestNormal;

	REAL32	d, dd;

	UINT8	* pMap = nullptr;

	i3::vector<i3BSPTriIndex*>	list;
//	List	attachlist;		// 쓰이지 않아 주석..

	i3BSPTriIndex *	pTri;

	for( i = 0; i < pGeometry->GetGeometryAttrCount(); i++ )
	{
		pRenderAttr = pGeometry->GetGeometryAttr( i );

		if( i3::kind_of<i3GeometryAttr* >(pRenderAttr))
		{
			pGeoAttr = (i3GeometryAttr *) pRenderAttr;

			pIA	= pGeoAttr->GetIndexArray();
			pVA	= pGeoAttr->GetVertexArray();

			pMap = (UINT8 *) i3MemAlloc( sizeof( pGeoAttr->GetPrimitiveCount() ) );
			
			for( j = 0; j < pGeoAttr->GetPrimitiveCount(); j++ )
			{	
				if( pMap[ j ] == 1 )
				{
					// 이미 계산한 것은 스킵
					continue;
				}

				pVA->GetPosition( pIA->GetIndex( j * 3 ),		&v1 );
				pVA->GetPosition( pIA->GetIndex( j * 3 + 1 ), &v2 );
				pVA->GetPosition( pIA->GetIndex( j * 3 + 2 ), &v3 );

				for( k = 0; k < list.size(); k++ )
				{
					pTri = list[k];
					I3MEM_SAFE_FREE_POINTER( pTri );
				}
				list.clear();

				// 기준 노멀
				i3Vector::Normal( &vNormal, &v1, &v2, &v3 );
				d = -( i3Vector::GetX( &vNormal ) * i3Vector::GetX( &v1 )
						+ i3Vector::GetY( &vNormal ) * i3Vector::GetY( &v1 )
						+ i3Vector::GetZ( &vNormal ) * i3Vector::GetZ( &v1 ) );

				// 기준 Tri
				pTri = (i3BSPTriIndex *) i3MemAlloc( sizeof( i3BSPTriIndex ) );
				pTri->m_nIndex[ 0 ] = (UINT16) pIA->GetIndex( j * 3 );
				pTri->m_nIndex[ 1 ] = (UINT16) pIA->GetIndex( j * 3 + 1 );
				pTri->m_nIndex[ 2 ] = (UINT16) pIA->GetIndex( j * 3 + 2 );
				list.push_back( pTri );

				pMap[ j ] = 1;	// 이미 검사했다..

				for( k = j + 1; k < pGeoAttr->GetPrimitiveCount(); k++ )
				{	
					pVA->GetPosition( pIA->GetIndex( k * 3 ),		&v1 );
					pVA->GetPosition( pIA->GetIndex( k * 3 + 1 ), &v2 );
					pVA->GetPosition( pIA->GetIndex( k * 3 + 2 ), &v3 );

					i3Vector::Normal( &vDestNormal, &v1, &v2, &v3 );

					// 같은 평면상이면 리스트에 추가한다.
					if( i3Vector::IsSame( &vNormal, &vDestNormal ) )
					{
						dd = -( i3Vector::GetX( &vDestNormal ) * i3Vector::GetX( &v1 )
							+ i3Vector::GetY( &vDestNormal ) * i3Vector::GetY( &v1 )
							+ i3Vector::GetZ( &vDestNormal ) * i3Vector::GetZ( &v1 ) );

						if( d == dd )
						{
							pTri = (i3BSPTriIndex *) i3MemAlloc( sizeof( i3BSPTriIndex ) );
							pTri->m_nIndex[ 0 ] = (UINT16) pIA->GetIndex( k * 3 );
							pTri->m_nIndex[ 1 ] = (UINT16) pIA->GetIndex( k * 3 + 1 );
							pTri->m_nIndex[ 2 ] = (UINT16) pIA->GetIndex( k * 3 + 2 );

							list.push_back( pTri );
							pMap[ k ] = 1;	// 기준 Tri와 같은 평면이라 계산 되었음을 체크
						}
					}
				}

				// 정점 제거 및 변 축약을 한다.
				for( k = 0; k < list.size(); k++ )
				{
					//pTri = (i3BSPTriIndex *) list.GetItem( k );

					// attachlist.Clear();		// 쓰이지 않아 주석..(2012.05.10.수빈)
					
					//
					_RemoveOfVertex( list, pVA );
				}
			}

			I3MEM_SAFE_FREE_POINTER( pMap );
		}
	}
}

i3BSPGeometry * i3OptBuildBSP::_CreateGeometryByTriIndex( i3BSPNode * pOutLeft, i3BSPNode * pOutRight, VEC4D * pPlane, i3BSPGeometry * pBSPGeo )
{
	INT32	i, nBothCount, nFrontCount, nBackCount;
	REAL32	Dot, w;

	VEC3D	Vec;
	VEC3D	VecNormal;
	
	i3Geometry * pNewGeometry	= nullptr;
	i3BoundBox * pBoundBox		= nullptr;
	i3VertexArray * pVA			= nullptr;
	
	i3BSPNode GeoPartFront;		// front side
	i3BSPNode GeoPartBack;		// back side
	i3BSPNode GeoPartBoth;

	i3BSPNode NewGeoPart;	// created new TriList
	i3BSPGeometry * pNewBSPGeo;	// BSP geometry data
	i3BSPGeometry *	pNoneDivideGeo = nullptr;
	i3BSPTriIndex * pTriIndex;	// dividing tri

	m_nGeoCount++;

	if( m_nDivideCount < m_nGeoCount )
		m_nDivideCount = m_nGeoCount;

	// 노말벡터를 기준으로 위치 파악을 한다.
	i3Vector::Set( &VecNormal, i3Vector::GetX( pPlane ), i3Vector::GetY( pPlane ), i3Vector::GetZ( pPlane ) );
	w = i3Vector::GetW( pPlane );
	
	// 지오메트리의 TriList 생성
	NewGeoPart.Create();
	
	_BuildTriList( &NewGeoPart, pBSPGeo->pGeometry );
	
	// Tri 쇼팅( 각 버텍스의 포지션에 맞춰 순서를 정한다. )
	_SortTriList( &NewGeoPart, pPlane, &pBSPGeo->WorldMat );

	// 비교한다.
	for( i = 0; i < NewGeoPart.getPartCount(); i++ )
	{
		pTriIndex = (i3BSPTriIndex *) NewGeoPart.getPart( i );
		pVA = pTriIndex->m_pVA;

		pVA->Lock();
		pVA->GetPosition( pTriIndex->m_nIndex[ pTriIndex->max ], &Vec );

		i3Vector::TransformCoord( &Vec, &Vec, &pBSPGeo->WorldMat );

		Dot = i3Vector::Dot( &VecNormal, &Vec );

		if( Dot <= w )
		{
			// 안쪽이다.
			GeoPartFront.addPart( pTriIndex );
		}
		else
		{	
			pVA->GetPosition( pTriIndex->m_nIndex[ pTriIndex->min ], &Vec );

			i3Vector::TransformCoord( &Vec, &Vec, &pBSPGeo->WorldMat );

			Dot = i3Vector::Dot( &VecNormal, &Vec );

			if( Dot <=  w )
			{
				if( m_nStyle & I3_BSP_NODE_BINARY )
				{
					// 겹친다면 쪼개줄 플래너 세팅
					pTriIndex->m_pDividePlane	= pPlane;
					GeoPartFront.addPart( pTriIndex );
					GeoPartBack.addPart( pTriIndex );
				}
				else if( m_nStyle & I3_BSP_NODE_GRAPH )
				{
					GeoPartBoth.addPart( pTriIndex );
				}
			}
			else
			{
				// 뒤쪽이다
				GeoPartBack.addPart( pTriIndex );
			}
		}

		pVA->Unlock();
	}

	// 오브젝트를 분할하여 각 노드에 넣는다.
	//I3TRACE( "Object divide - Min( %.2f, %.2f, %.2f ) Max( %.2f, %.2f, %.2f ), Plane ( %.2f, %.2f, %.2f, %.2f ), Left : %d, Right : %d\n",
	//	i3Vector::GetX( pBoundBox->GetMin() ), i3Vector::GetY( pBoundBox->GetMin() ), i3Vector::GetZ( pBoundBox->GetMin() ),
	//	i3Vector::GetX( pBoundBox->GetMax() ), i3Vector::GetY( pBoundBox->GetMax() ), i3Vector::GetZ( pBoundBox->GetMax() ),
	//	i3Vector::GetX( pPlane ), i3Vector::GetY( pPlane ), i3Vector::GetZ( pPlane ), i3Vector::GetW( pPlane ),
	//	GeoPartFront.getPartCount(), GeoPartBack.getPartCount() );

	nBothCount	= GeoPartBoth.getPartCount();
	nFrontCount = GeoPartFront.getPartCount();
	nBackCount	= GeoPartBack.getPartCount();

	if( nBothCount > 0)
	{
		// 지오메트리를 만든다. ( NoneDivideUnit )
		// 재구성한 오브젝트를 리스트에 추가해준다.
		pNoneDivideGeo = (i3BSPGeometry *) i3MemAlloc( sizeof( i3BSPGeometry ) );

		pNoneDivideGeo->m_nTriCount		= nBothCount;
		pNoneDivideGeo->pTri			= (i3BSPTriIndex *) i3MemAlloc( sizeof(i3BSPTriIndex) * GeoPartBoth.getPartCount() );
		for( i = 0; i < GeoPartBoth.getPartCount(); i++ )
		{
			pTriIndex = (i3BSPTriIndex *) GeoPartBoth.getPart( i );
			i3mem::Copy( &pNoneDivideGeo->pTri[ i ], pTriIndex, sizeof(i3BSPTriIndex) );
		}

		pNewGeometry = i3Geometry::new_object();
		I3_MUST_ADDREF(pNewGeometry);
		_CreateGeometry( pNewGeometry, &GeoPartBoth, pBSPGeo->pGeometry, 0 );
		pNoneDivideGeo->pGeometry		= pNewGeometry;
		m_GeoList.push_back( pNewGeometry );

		// 바운드 박스를 세팅한다.
		pBoundBox = _CreateBoundBoxForWorld( pNewGeometry, &pBSPGeo->WorldMat );
		pNoneDivideGeo->pBoundBox		= pBoundBox;
		// 월드 트랜스폼
		i3Matrix::Copy( &pNoneDivideGeo->WorldMat, &pBSPGeo->WorldMat );
		_addBSPGeometry( pNoneDivideGeo );	// 임시 오브젝트 리스트에 추가
	}

	if( nFrontCount > 0 )
	{
		if( nBackCount > 0 )
		{
			// 지오메트리를 만든다. ( Left )
			// 생성한 지오메트리를 리스트에 추가해준다.
			pNewBSPGeo = (i3BSPGeometry *) i3MemAlloc( sizeof( i3BSPGeometry ) );
			
			pNewBSPGeo->m_nTriCount		= nFrontCount;
			pNewBSPGeo->pTri			= (i3BSPTriIndex *) i3MemAlloc( sizeof(i3BSPTriIndex) * nFrontCount );
			for( i = 0; i < nFrontCount; i++ )
			{
				pTriIndex = (i3BSPTriIndex *) GeoPartFront.getPart( i );
				i3mem::Copy( &pNewBSPGeo->pTri[ i ], pTriIndex, sizeof(i3BSPTriIndex) );
			}

			pNewGeometry = i3Geometry::new_object();
			I3_MUST_ADDREF(pNewGeometry);
			_CreateGeometry( pNewGeometry, &GeoPartFront, pBSPGeo->pGeometry, 0 );
			pNewBSPGeo->pGeometry		= pNewGeometry;
			m_GeoList.push_back( pNewGeometry );

			// 바운드 박스를 세팅한다.
			pBoundBox = _CreateBoundBoxForWorld( pNewGeometry, &pBSPGeo->WorldMat );
			pNewBSPGeo->pBoundBox		= pBoundBox;
			// 월드 트랜스폼
			i3Matrix::Copy( &pNewBSPGeo->WorldMat, &pBSPGeo->WorldMat );
			pOutLeft->addPart( pNewBSPGeo );
			_addBSPGeometry( pNewBSPGeo );	// 임시 오브젝트 리스트에 추가
		
			// 지오메트리를 만든다. ( Right )
			// 생성한 지오메트리를 리스트에 추가해준다.
			pNewBSPGeo = (i3BSPGeometry *) i3MemAlloc( sizeof( i3BSPGeometry ) );
			
			pNewBSPGeo->m_nTriCount		= nBackCount;
			pNewBSPGeo->pTri			= (i3BSPTriIndex *) i3MemAlloc( sizeof(i3BSPTriIndex) * nBackCount );
			for( i = 0; i < nBackCount; i++ )
			{
				pTriIndex = (i3BSPTriIndex *) GeoPartBack.getPart( i );
				i3mem::Copy( &pNewBSPGeo->pTri[ i ], pTriIndex, sizeof(i3BSPTriIndex) );
			}

			pNewGeometry = i3Geometry::new_object();
			I3_MUST_ADDREF(pNewGeometry);
			_CreateGeometry( pNewGeometry, &GeoPartBack, pBSPGeo->pGeometry, 1 );
			pNewBSPGeo->pGeometry		= pNewGeometry;
			m_GeoList.push_back( pNewGeometry );

			// 바운드 박스를 세팅한다.
			pBoundBox = _CreateBoundBoxForWorld( pNewGeometry, &pBSPGeo->WorldMat );
			pNewBSPGeo->pBoundBox		= pBoundBox;
			// 월드 트랜스폼
			i3Matrix::Copy( &pNewBSPGeo->WorldMat, &pBSPGeo->WorldMat );
			pOutRight->addPart( pNewBSPGeo );
			_addBSPGeometry( pNewBSPGeo );	// 임시 오브젝트 리스트에 추가
		}
		else
		{
			// 한쪽에 추가
			pOutLeft->addPart( pBSPGeo );
		}
	}
	else
	{
		if( nBackCount > 0 )
		{
			// 한쪽에 추가
			pOutRight->addPart( pBSPGeo );
		}
	}

	// 트리리스트 해제
	for( i = 0; i < NewGeoPart.getPartCount(); i++ )
	{
		pTriIndex = (i3BSPTriIndex *) NewGeoPart.getPart( i );
		I3MEM_SAFE_FREE_POINTER( pTriIndex );
	}

	return pNoneDivideGeo;
}

void i3OptBuildBSP::CreateNode( i3BSPNode * pRoot )
{
	INT32	i;
	INT32	nFrontCount, nBackCount;
	INT32	nBothCount;
	INT32	nDepthCount;
	UINT32	nFlags;
	REAL32	Dot, x, y, z, w;
	VEC3D	VecNormal;
	VEC4D	vPlane;
	VEC4D * pPlane					= nullptr;
	
	i3BoundBox *	pBoundBox		= nullptr;
	i3BSPGeometry * pBSPGeo			= nullptr;
	i3BSPNode *		pFrontNode		= nullptr;
	i3BSPNode *		pBackNode		= nullptr;
	i3BSPNode *		pBothNode		= nullptr;
	i3BSPNode *		pParentNode		= nullptr;

	nDepthCount = pRoot->m_nCount;
	
	if( nDepthCount > I3_BSP_DEFAULT_DEPTH )
	{
		pRoot->setLeaf(true);
		return;
	}

	if( pRoot->m_nDivideCount > 3 )
	{
		pRoot->setLeaf(true);
	}

	// 부모에 포함된 오브젝트를 분할 플래너에 따라 분류
	pPlane = pRoot->getPlane();
	x = i3Vector::GetX( pPlane );
	y = i3Vector::GetY( pPlane );
	z = i3Vector::GetZ( pPlane );
	w = i3Vector::GetW( pPlane );
	i3Vector::Set( &VecNormal, x, y, z );

	// 기준 플래너가 겹치는지 판별
	// 현재는 플래너에 따른 오브젝트 분할을 할지 아니면 플래너를 새로 세팅할지 여부 결정해야된다.
	// 플래너를 새로 세팅한다하더라도 겹치는 경우가 생길 것이다.
	pFrontNode = i3BSPNode::new_object();
	pBackNode = i3BSPNode::new_object();
	pBothNode	= i3BSPNode::new_object();

	// 플래너에 따른 오브젝트를 나눠준다.
	for( i = 0; i < pRoot->getPartCount(); i++ )
	{
		pBSPGeo = (i3BSPGeometry *) pRoot->getPart( i );

		pBoundBox = pBSPGeo->pBoundBox;	// transformed boundbox
		
		Dot = i3Vector::Dot( &VecNormal, pBoundBox->GetMax() );

		if( Dot <= w )
		{
			// 안쪽이다.
			pFrontNode->addPart( pBSPGeo );
		}
		else
		{	
			Dot = i3Vector::Dot( &VecNormal, pBoundBox->GetMin() );

			if( Dot <= w )
			{
				// 겹친다
				// 오브젝트를 분할한다.
				pBSPGeo = _CreateGeometryByTriIndex( pFrontNode, pBackNode, pPlane, pBSPGeo );
				if( pBSPGeo != nullptr )
				{
					pBothNode->addPart( pBSPGeo );
				}
			}
			else
			{
				pBackNode->addPart( pBSPGeo );
			}
		}
	}

	nFlags	= I3_BSP_TYPE_GEOMETRY | I3_BSP_DIVIDE_OBJCOUNT;

	// 추가하기전에 분할면을 세팅해준다.
	if( i3Vector::GetX( pPlane ) == 1.0f )		{	nFlags |= I3_BSP_MINMAX_Y;	}
	else if( i3Vector::GetY( pPlane ) == 1.0f )	{	nFlags |= I3_BSP_MINMAX_Z;	}
	else										{	nFlags |= I3_BSP_MINMAX_X;	}

	nFrontCount	= pFrontNode->getPartCount();
	nBackCount	= pBackNode->getPartCount();
	nBothCount	= pBothNode->getPartCount();

	if( nBothCount > 0 )
	{
		// 부모에 분할된 노드 추가
		_CalcSpaceBound( pBothNode );
		_CalcPlane( &vPlane, pBothNode, nFlags );
		pBothNode->setPlane( &vPlane );
		pRoot->AddChild( pBothNode );
	}
	else
	{
		I3_SAFE_RELEASE( pBothNode );
	}

	if( nFrontCount > 0 )
	{
		// 부모에 분할된 노드 추가
		_CalcSpaceBound( pFrontNode );
		_CalcPlane( &vPlane, pFrontNode, nFlags );
		pFrontNode->setPlane( &vPlane );
		pRoot->AddChild( pFrontNode );
	}
	else
	{
		I3_SAFE_RELEASE( pFrontNode );
	}

	if( nBackCount > 0 )
	{
		// 부모에 분할된 노드 추가
		_CalcSpaceBound( pBackNode );
		_CalcPlane( &vPlane, pBackNode, nFlags );
		pBackNode->setPlane( &vPlane );
		pRoot->AddChild( pBackNode );
	}
	else
	{
		I3_SAFE_RELEASE( pBackNode );
	}
	
	if( nFrontCount > 0 )
	{
		if( nBackCount == 0 )
		{	
			if( nBothCount == 0 )
			{
				// 분할이 안되는 녀석이다 다른 플래너로 분할을 시도해보자.
				// 프론트 노드에 대해 분할 여부를 찾아야된다.
				// 현재 노드를 부모노드로 올려붙인다.
				pRoot->RemoveAllChild();
				pParentNode = (i3BSPNode *)pRoot->GetParent();
				for( i = 0; i < pParentNode->GetChildCount(); i++ )
				{
					if( pParentNode->GetChild( i ) == pRoot )
					{
						pParentNode->RemoveChild( pRoot );
						break;
					}
				}
				I3_SAFE_RELEASE( pRoot );
				pParentNode->AddChild( pFrontNode );
				pFrontNode->m_nDivideCount++;
			}
		}
	}
	else
	{
		// 백노드에 대해 분할여부를 찾아야된다.
		if( nBackCount > 0 )
		{
			if( nBothCount == 0 )
			{
				pRoot->RemoveAllChild();
				pParentNode = (i3BSPNode *) pRoot->GetParent();
				for( i = 0; i < pParentNode->GetChildCount(); i++ )
				{
					if( pParentNode->GetChild( i ) == pRoot )
					{
						pParentNode->RemoveChild( pRoot );
						break;
					}
				}
				I3_SAFE_RELEASE( pRoot );
				pParentNode->AddChild( pBackNode );
				pBackNode->m_nDivideCount++;
			}
		}
		else
		{	
			if( nBothCount > 0 )
			{	
				// both Geometrys up to parent node
				pRoot->RemoveAllChild();
				pParentNode = (i3BSPNode *) pRoot->GetParent();
				
				for( i = 0; i < pParentNode->GetChildCount(); i++ )
				{
					if( pParentNode->GetChild( i ) == pRoot )
					{
						pParentNode->RemoveChild( pRoot );
						break;
					}
				}
				I3_SAFE_RELEASE( pRoot );
				pParentNode->AddChild( pBothNode );
				pBothNode->m_nDivideCount++;
			}
			else
			{
				// 분할이 아니다.
				pRoot->setLeaf(true);
				return;
			}
		}
	}

	//I3TRACE( "Object Partitial with Plane( %.0f, %.0f, %.0f, %.2f ), Front : %d, Back : %d, Both : %d\n",
	//		x, y, z, w,
	//		nFrontCount, nBackCount, nBothCount );

	if( pBothNode )
	{
		pBothNode->m_nCount = nDepthCount + 1;
		CreateNode( pBothNode );
	}
	if( pFrontNode )
	{
		pFrontNode->m_nCount = nDepthCount + 1;
		CreateNode( pFrontNode );
	}
	if( pBackNode )
	{
		pBackNode->m_nCount = nDepthCount + 1;
		CreateNode( pBackNode );
	}
}

void i3OptBuildBSP::ReBuildNode( i3Node * pNode, i3BSPNode * pBSPNode )
{
	INT32	i, j;

	VEC4D * pPlane;
	
	i3Geometry *	pGeometry		= nullptr;
	i3BSPGeometry * pBSPGeo			= nullptr;

	i3Node *		pNewNode		= nullptr;
	i3Transform *	pTransform		= nullptr;

	i3BSPNode * pChild;
	i3BSPNode NewObjectList;
	char	pNodeName[128];
	char	pPlaneName[ 32 ];
	char	pSideName[ 32 ];

	// 현재 노드의 분할면을 바운드 박스로 만든다.
	pPlane = pBSPNode->getPlane();

	// X축 기준
	if( i3Vector::GetX( pPlane ) == 1.0f )		{	i3::safe_string_copy( pPlaneName, "Divide X -", 32);	}
	// Y축 기준
	else if( i3Vector::GetY( pPlane ) == 1.0f )	{	i3::safe_string_copy( pPlaneName, "Divide Y -", 32);	}
	// Z축 기준
	else										{	i3::safe_string_copy( pPlaneName, "Divide Z -", 32);	}

	// 노드를 만들어봅시다.
	for( i = 0; i < pBSPNode->GetChildCount(); i++ )
	{
		pChild = (i3BSPNode *) pBSPNode->GetChild( i );

		if( i == 0 )		{ i3::safe_string_copy( pSideName, " Both",  32 );	}
		else if( i == 1 )	{ i3::safe_string_copy( pSideName, " Front", 32 );	}
		else				{ i3::safe_string_copy( pSideName, " Back",  32 );	}

		sprintf( pNodeName, "%s%s", pPlaneName, pSideName );

		if( pChild == nullptr )
			return;

		if( !pChild->IsLeaf() )
		{		
			// 자식 노드를 새로 추가해준다.(바운드 박스가 세팅되어 있다 )
			pNewNode = i3Node::new_object();
			I3ASSERT( pNewNode );

			pNewNode->SetBound( pChild->GetBound() );

			pNewNode->SetName( pNodeName );

			pNode->AddChild( pNewNode );

			// 열려 있는 노드의 오브젝트는 삭제해준다.
			for( j = 0; j < pChild->getPartCount(); j++ )
			{
				pBSPGeo = (i3BSPGeometry *) pChild->getPart( j );

				for(size_t k = 0; k < m_GeoList.size(); k++ )
				{
					pGeometry = m_GeoList[ k ];

					if( pGeometry == pBSPGeo->pGeometry )
					{
						I3_SAFE_RELEASE( pGeometry );
						i3::vu::set_value_force(m_GeoList, k, static_cast<i3Geometry*>(0) );
					}
				}
			}
			
			// 자식 노드에 대해 분할 작업
			ReBuildNode( pNewNode, pChild );
		}
		else
		{
			// 분할 공간이 아니다
			// 오브젝트를 노드에 추가
			pNewNode = i3Node::new_object();
			I3ASSERT( pNewNode );

			pNewNode->SetBound( pChild->GetBound() );

			pNewNode->SetName( pNodeName );

			pNode->AddChild( pNewNode );

			for( j = 0; j < pChild->getPartCount(); j++ )
			{
				pBSPGeo = (i3BSPGeometry *) pChild->getPart( j );
				
				// 지오메트리를 새로 만든다.
				/*NewObjectList.ReleaseParts();
				for( k = 0; k < pBSPGeo->m_nTriCount; k++ )
					NewObjectList.addPart( &pBSPGeo->pTri[ k ] );
				pGeometry = i3Geometry::new_object();
				_CreateGeometry( pGeometry, &NewObjectList, pBSPGeo->pGeometry );*/

				// 부모 노드의 트랜스폼을 세팅
				pTransform = i3Transform::new_object();
				I3ASSERT( pTransform );
				pTransform->SetMatrix( &pBSPGeo->WorldMat );
				pTransform->AddChild( pBSPGeo->pGeometry );
				pTransform->SetName( pBSPGeo->pGeometry->GetName() );

				pNewNode->AddChild( pTransform );
			}
		}
	}
}

INT32 i3OptBuildBSP::_FindGeometryByAddress( void * pAddress )
{
	
	i3BSPGeometry * pBSPGeo;

	for(size_t i = 0; i < m_BSPGeometryList.size(); i++ )
	{
		pBSPGeo = _getBSPGeometry( INT32(i) );
		if( pBSPGeo->pGeometry == pAddress )
			return INT32(i);
	}

	return -1;
}


bool i3OptBuildBSP::OnNode( i3Node * pNode, i3Node * pParent )
{
	INT32	idx;
	
	i3Geometry *	pGeo;
	i3BSPGeometry * pBSPGeo = nullptr;

	MATRIX *		pWorld;
	i3BoundBox *	pBox;

	if( i3::kind_of<i3Geometry* >(pNode))
	{
		pGeo = (i3Geometry *) pNode;

		// Transform
		pWorld = m_pStack->GetTop();

		// 저장해둔다.
		idx = _FindGeometryByAddress( pGeo );
		if( idx != -1 )	{	pBSPGeo = _getBSPGeometry( idx );	}
		else
		{
			pBSPGeo = (i3BSPGeometry*) i3MemAlloc( sizeof( i3BSPGeometry ) );
			
			_BuildTriObject( pBSPGeo, pGeo );
			pBSPGeo->pGeometry	= pGeo;
			i3Matrix::Copy( &pBSPGeo->WorldMat, pWorld );
			// Create boundbox with transform
			pBox = _CreateBoundBoxForWorld( pGeo, pWorld );
			pBSPGeo->pBoundBox = pBox;

			_addBSPGeometry( pBSPGeo );
		}

		getNode( 0 )->addPart( pBSPGeo );	// 지오메트리를 임시 노드에 넣는다.
	}

	return true;
}

// 오브젝트의 바운드 박스를 월드맵에 맞춰 계산한다.
void i3OptBuildBSP::Trace( i3Node * pRoot)
{
	UINT32		nFlags = I3_BSP_TYPE_GEOMETRY | I3_BSP_DIVIDE_OBJCOUNT | I3_BSP_MINMAX_X;
	VEC4D		vPlane;
	
	i3Node	*	pNewNode;
	i3BSPNode * pBSPNode;
	SetMatrixStackEnable(true);

	// 임시 데이터용 노드 생성
	_CreateRoot();
	pBSPNode = getNode( 0 );
	// 오브젝트의 바운드 박스를 월드에 맞춰 계산한다.
	i3SceneOptimizer::Trace( pRoot);

	I3TRACE( "Original Collision Object Count : %d\n", pBSPNode->getPartCount() );
	
	// 현재 노드의 가장 큰 바운드 박스 계산
	_CalcSpaceBound( pBSPNode );
	// 플래너 세팅
	// 임시 노드에서 바운드박스의 위치에 따라 정렬
	_CalcPlane( &vPlane, pBSPNode, nFlags );
	pBSPNode->setPlane( &vPlane );
	CreateNode( pBSPNode );

	I3TRACE( "Total Divided Object : %d, Partitial Depth : %d\n", m_nDivideCount, m_nMaxDepth );

	// Post Process
	// 노드 재정렬
	pNewNode = i3Node::new_object();
	I3ASSERT( pNewNode );
	ReBuildNode( pNewNode, pBSPNode );

	pRoot = pNewNode;

	_RemoveAllBSPGeometry();

	/*
	for( INT32 i = 0; m_GeoList.GetCount(); i++ )
	{
		i3Geometry * pGeo = (i3Geometry *) m_GeoList.GetItem( i );

		I3_SAFE_RELEASE( pGeo );
	}
*/
	m_GeoList.clear();

	SetTraceRoot( pRoot);
}
