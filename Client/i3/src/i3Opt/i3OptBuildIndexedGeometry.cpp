#include "i3OptPCH.h"
#include "i3OptBuildIndexedGeometry.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/string/compare/generic_is_niequal.h"

//	Property Field Name
#define	FIELD_INDEXED_AUTO		"AutoSwitching"
#define FIELD_INDEXED_BUILD		"BuildIndex"


//#define		RESULT_ON

#if defined( RESULT_ON)
static bool	g_bTraceOutput = true;
#else
static bool	g_bTraceOutput = false;
#endif

I3_CLASS_INSTANCE( i3OptBuildIndexedGeometry);

i3OptBuildIndexedGeometry::i3OptBuildIndexedGeometry(void)
{
	m_Class = CLASS_GEOMETRY;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;

	m_BuildInfo.SetDefault();

	//	Optimize Data 생성
	m_OptData.nPrimCount = 0;
	m_OptData.nPrimType = I3G_PRIM_NONE;
	m_OptData.nTriCount = 0;
	m_OptData.bIsPrimLength = false;

	m_OptData.nVertexCount = 0;
	m_OptData.nIndexCount = 0; 
	m_OptData.nTexSetCount = 0;
	m_OptData.nTexcoordCount = 2;	//	u , v

	m_Error.Reset();
}

i3OptBuildIndexedGeometry::~i3OptBuildIndexedGeometry(void)
{
}

void i3OptBuildIndexedGeometry::SetProperty( char * pszFieldName, char * pszValue)
{
	if( i3::generic_is_iequal( pszFieldName, FIELD_INDEXED_AUTO) )
	{
		if ( i3::generic_is_niequal(pszValue, "T", 1) ||
			 i3::generic_is_niequal(pszValue, "Y", 1) )
			SetAutoSwitching(true);
		else if( i3::generic_is_niequal(pszValue, "F", 1) ||
			 i3::generic_is_niequal(pszValue, "N", 1) )
			SetAutoSwitching(false);
		else
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);

		return;
	}
	else if( i3::generic_is_iequal( pszFieldName, FIELD_INDEXED_BUILD) ) 
	{
		if( i3::generic_is_niequal(pszValue, "T", 1) ||
			 i3::generic_is_niequal(pszValue, "Y", 1) )
			SetBuildIndex(true);
		else if(i3::generic_is_niequal(pszValue, "F", 1) ||
			 i3::generic_is_niequal(pszValue, "N", 1) )
			SetBuildIndex(false);
		else
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);

		return;
	}		

	I3TRACE("(%s) SetProperty : Unknown field name (%s = %s)\n", 
		this->static_meta()->class_name().c_str(), 
		pszFieldName, pszValue);	
}

bool i3OptBuildIndexedGeometry::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( pNode == nullptr)	return true;

	i3ClassMeta * pMeta = pNode->meta();
	I3ASSERT( pMeta != nullptr);

	if( pMeta->kind_of( i3Geometry::static_meta()) ||
		pMeta->kind_of( i3Skin::static_meta()))
	{
		//I3TRACE( "i3OptBuildIndexedGeometry : OnNode (%s)\n", pMeta->GetClassName());

		Convert( pNode);
	}	

	return true;
}

bool i3OptBuildIndexedGeometry::Convert( i3Node * pNode)
{
	if( pNode == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	m_Error.Reset();

	INT32 i = 0, j = 0;	
	INT32 nGeoAttrCount = 0;
	INT32 nNodeCount = 0;

	i3ClassMeta * pMeta = pNode->meta();

	i3Geometry *	pGeometry = nullptr;
	i3Skin *		pSkin = nullptr;
	i3RenderAttr *	pAttr = nullptr;

	if( pMeta->kind_of( i3Skin::static_meta()) )
	{
		pSkin = (i3Skin *) pNode;
		
		nNodeCount = pSkin->getShapeCount();
	}	
	else if( pMeta->kind_of( i3Geometry::static_meta()) )
	{
		pGeometry = (i3Geometry *) pNode;
		
		nNodeCount = 1;
	}

	for( i = 0; i < nNodeCount; i++)
	{
		if( pSkin)
			nGeoAttrCount = pSkin->getShape( i)->getGeometryAttrCount();
		else if( pGeometry)
			nGeoAttrCount = pGeometry->GetGeometryAttrCount();

		for( j = 0; j < nGeoAttrCount; j++)
		{
			if ( pSkin)
				pAttr = pSkin->getShape( i)->getGeometryAttr(j);
			else if( pGeometry)
				pAttr = pGeometry->GetGeometryAttr(j);

			if( pAttr == nullptr)
			{
				break;
			}

			//	GeometryAttr 이면 Draw Primitive Type을 확인후 Vertex를 변환한다.
			if( i3::kind_of<i3GeometryAttr*>(pAttr))
			{
				i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;
								
				if( pGeoAttr->GetVertexArray() != nullptr)
				{
					if( g_bTraceOutput)	I3TRACE("------------------------------------------------------\n");

					if( m_BuildInfo._bAutoSwitching)
					{
						//	index가 없으면 index array로 변환
						if( pGeoAttr->GetIndexArray() == nullptr)
							m_BuildInfo._bBuildIndex = true;
						//	index가 있으면 none index로 변환
						else
							m_BuildInfo._bBuildIndex = false;
					}
					
					if( m_BuildInfo._bBuildIndex)		
					{
						if( g_bTraceOutput)	I3TRACE("Convert : Build index (NAME : %s)\n", (pGeometry) ? pGeometry->GetName() : "NullGeometry" );

						return ConvertVertex2Indexed( pGeoAttr);
					}					
					else
					{					
						if( g_bTraceOutput)	I3TRACE("Convert : Unbuild index (NAME : %s)\n", (pGeometry) ? pGeometry->GetName() : "NullGeometry");

						return ConvertIndexed2Vertex( pGeoAttr);							
					}					
				}				
			}
		}	
	}

	SetResult( I3OPT_FAIL);
	I3TRACE("Convert : Not include GeometryAttr.\n");

	return false;
}

bool i3OptBuildIndexedGeometry::ConvertVertex2Indexed( i3GeometryAttr * pGeoAttr)
{
	if( pGeoAttr == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	i3VertexArray * pVA = pGeoAttr->GetVertexArray();
	i3IndexArray *	pVI = pGeoAttr->GetIndexArray();

	if( pVA == nullptr)
	{
		SetResult( I3OPT_FAIL_NONE_VERTEX);
		I3TRACE( "ConvertVertex2Indexed : Not include vertex array in geometry\n");
		return false;
	}

	if( pVI != nullptr)
	{
		SetResult( I3OPT_FAIL_NOT_USE);
		I3TRACE( "ConvertVertex2Indexed : Already include index array in geometry\n");
		return false;
	}

	if( g_bTraceOutput)	I3TRACE1( "Before vertex count = %d\n", pGeoAttr->GetVertexArray()->GetCount());
    
	switch( pGeoAttr->GetPrimitiveType())
	{
	case I3G_PRIM_POINTLIST:	
	case I3G_PRIM_LINELIST:	
	case I3G_PRIM_LINESTRIP:	
	case I3G_PRIM_TRIFAN:
		{
			SetResult( I3OPT_FAIL_NOT_SUPPORT);
			I3TRACE("ConvertVertex2Indexed : Not support primitive type.\n");

			return false;
		}
		break;			

	case I3G_PRIM_TRILIST:			
	case I3G_PRIM_TRISTRIP:
		{
			//	최적화해서 index 생성
			if( BuildOptimize( pGeoAttr) == false)
			{
				SetResult( I3OPT_FAIL);
				I3TRACE( "ConvertVertex2Indexed : Fail to optimize.\n");
				return false;
			}
		}
		break;
	
	default:	
		{
			SetResult( I3OPT_FAIL_INVALID_VAL);
			I3TRACE("ConvertVertex2Indexed : Invalid primitive type.\n");

			return false;
		}
		break;
	}

	if( g_bTraceOutput)	I3TRACE1( "Optimized vertex count = %d\n\n", m_OptData.nVertexCount);
	
	if( g_bTraceOutput)	I3TRACE2( "Primitive Count = %d Triangle Count = %d\n", m_OptData.nPrimCount, m_OptData.nTriCount);	
	if( g_bTraceOutput)	I3TRACE1( "Build Index Count = %d\n", m_OptData.nIndexCount);

	if( g_bTraceOutput)	I3TRACE( "ConvertVertex2Indexed : Success build index array in geometry\n\n");

	return true;
}

bool i3OptBuildIndexedGeometry::ConvertIndexed2Vertex( i3GeometryAttr * pGeoAttr)
{
	if( pGeoAttr == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	i3VertexArray * pVA = pGeoAttr->GetVertexArray();
	i3IndexArray *	pVI = pGeoAttr->GetIndexArray();

	if( pVA == nullptr)
	{
		SetResult( I3OPT_FAIL_NONE_VERTEX);
		I3TRACE( "ConvertIndexed2Vertex : Not include vertex array in geometry\n");
		return false;
	}

	if( pVI == nullptr)
	{
		SetResult( I3OPT_FAIL_NONE_INDEX);
		I3TRACE( "ConvertIndexed2Vertex : Not include index array in geometry\n");
		return false;
	}

	if( g_bTraceOutput)	I3TRACE1( "Before vertex count = %d\n", pGeoAttr->GetVertexArray()->GetCount());

	switch( pGeoAttr->GetPrimitiveType())
	{
	case I3G_PRIM_POINTLIST:
	case I3G_PRIM_LINELIST:
	case I3G_PRIM_LINESTRIP:
		{
			SetResult( I3OPT_FAIL_NOT_SUPPORT);
			I3TRACE("ConvertIndexed2Vertex : Not support primitive type.\n");

			return false;
		}
		break;		

	case I3G_PRIM_TRILIST:			
	case I3G_PRIM_TRISTRIP:
	case I3G_PRIM_TRIFAN:
		{
			//	Index를 풀어서 vertex array 새로 생성
			if( RemapIndices( pGeoAttr) == false)
			{
				SetResult( I3OPT_FAIL);
				I3TRACE( "ConvertIndexed2Vertex : Fail to remapped indices.\n");
				return false;
			}
		}
		break;
	
	default:
		{
			SetResult( I3OPT_FAIL_INVALID_VAL);
			I3TRACE("ConvertIndexed2Vertex : Invalid primitive type.\n");

			return false;
		}
		break;
	}

	if( g_bTraceOutput)	I3TRACE1( "Optimized vertex count = %d\n\n", m_OptData.nVertexCount);
	
	if( g_bTraceOutput)	I3TRACE2( "Primitive Count = %d Triangle Count = %d\n", m_OptData.nPrimCount, m_OptData.nTriCount);	
	if( g_bTraceOutput)	I3TRACE1( "Build Index Count = %d\n", m_OptData.nIndexCount);

	if( g_bTraceOutput)	I3TRACE( "ConvertVertex2Indexed : Success unbuild index array in geometry\n\n");

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool i3OptBuildIndexedGeometry::BuildOptimize( i3GeometryAttr * pGeoAttr)
{
	if( pGeoAttr == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	m_Error.Reset();

	bool	ret = false;
	UINT32	i = 0;

	i3VertexArray *		pVA = pGeoAttr->GetVertexArray();
	i3VertexFormat*		pFormat = pVA->GetFormat();

	i3VertexArray *		pNewVA = nullptr;
	i3IndexArray *		pNewIA = nullptr;

	//	버텍스 최적화한다. (만들어진 vetex와 index는 m_OptData 에 생성된다.)
	ret = BuildOptimizeToOpt( pGeoAttr, true);
	if( !ret)
	{
		SetResult( I3OPT_FAIL);
		I3TRACE( "BuildOptimize : Failed optimize\n");

		return false;
	}			
	
	pNewVA = i3VertexArray::new_object();	
	if( pNewVA == nullptr)
	{
		SetResult( I3OPT_FAIL_NOT_ENOUGH_MEM);
		I3TRACE( "BuildOptimize : Fail to generate new vertex array.\n");
		return false;
	}

	pNewIA = i3IndexArray::new_object();
	if( pNewIA == nullptr)
	{
		SetResult( I3OPT_FAIL_NOT_ENOUGH_MEM);
		I3TRACE( "BuildOptimize : Fail to generate new index array.\n");
		return false;
	}

	//	최적화된 버텍스 데이터( Vertex Array, Index Array)를 Geometry 버퍼로 옮긴다.
	ret = RenewVerticesFromOpt( pNewVA, pNewIA, pFormat);
	if( ret)
	{
		pGeoAttr->SetVertexArray( pNewVA, m_OptData.nPrimType, m_OptData.nPrimCount,0, false);	
		pGeoAttr->SetIndexArray( pNewIA);

		I3_MUST_RELEASE( pNewVA);
		I3_MUST_RELEASE( pNewIA);

		if( m_OptData.nPrimType == I3G_PRIM_TRISTRIP)
		{
			if( m_OptData.bIsPrimLength)
			{
				for( i = 0; i < m_OptData.nPrimCount; i++)
				{
					pGeoAttr->SetPrimitiveLength( i, m_OptData.nPrimLength[i]);
				}
			}
		}
	}
	else
	{
		SetResult( I3OPT_FAIL);
		I3TRACE( "BuildOptimize : Failed renew vertices from optimize data\n");

		I3_MUST_RELEASE( pNewVA);
		I3_MUST_RELEASE( pNewIA);

		return false;
	}

	return true;
}

bool i3OptBuildIndexedGeometry::RemapIndices( i3GeometryAttr * pGeoAttr)
{
	if( pGeoAttr == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	m_Error.Reset();

	bool	ret = false;
	UINT32	i = 0;

	i3VertexArray *		pVA = pGeoAttr->GetVertexArray();
	i3VertexFormat*		pFormat = pVA->GetFormat();

	i3VertexArray *		pNewVA = nullptr;

	//	index를 바탕으로 vertex 정보를 m_OptData에 Vertex를 푼다.
	ret = RemapIndicesToOpt( pGeoAttr);
	if( !ret)
	{
		SetResult( I3OPT_FAIL);
		I3TRACE( "RemapIndices : Failed unbind vertices to optimize data.\n");

		return false;
	}			

	pNewVA = i3VertexArray::new_object_ref();
	if( pNewVA == nullptr)
	{
		SetResult( I3OPT_FAIL_NOT_ENOUGH_MEM);
		I3TRACE( "RemapIndices : Fail to generate new vertex array.\n");
		return false;
	}

	//	m_OptData에 있는 버텍스 데이터를 Geometry 버퍼로 옮긴다.
	ret = RenewVerticesFromOpt( pNewVA, nullptr, pFormat);
	if( ret)
	{
		pGeoAttr->SetVertexArray( pNewVA, m_OptData.nPrimType, m_OptData.nPrimCount);	
		pGeoAttr->SetIndexArray(nullptr);

		if( m_OptData.nPrimType == I3G_PRIM_TRISTRIP)
		{
			if( m_OptData.bIsPrimLength)
			{
				for( i = 0; i < m_OptData.nPrimCount; i++)
				{
					pGeoAttr->SetPrimitiveLength( i, m_OptData.nPrimLength[i]);
				}
			}
		}
	}
	else
	{
		SetResult( I3OPT_FAIL);
		I3TRACE( "RemapIndices : Failed renew vertices from optimize data.\n");

		return false;
	}

	return true;
}

bool i3OptBuildIndexedGeometry::BuildOptimizeToOpt( i3GeometryAttr * pGeoAttr, bool bBuildIndex /* = true*/)
{
	if( pGeoAttr == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	UINT32	i = 0, j = 0;
	UINT32	count	= 0;
	bool	bUnique = false;
	
	i3VertexArray *	pSrcVA = pGeoAttr->GetVertexArray();	
	if( pSrcVA == nullptr)
	{
		SetResult( I3OPT_FAIL_NONE_VERTEX);
		I3TRACE("BuildOptimizeToOpt : None Vertex array data in Geometry (0x%x)\n", pGeoAttr);
		return false;
	}

	count		= pGeoAttr->GetVertexArray()->GetCount();

	//	Optimize Data 초기화	
	InitOpt( pGeoAttr, &m_OptData);
	
	//	최적화 작업 및 인덱스 생성
	pSrcVA->Lock();	
	if( m_OptData.nPrimType == I3G_PRIM_TRISTRIP)
	{
		if( m_OptData.bIsPrimLength)
		{
			UINT32 nPrimLength = 0;
			UINT32 nLengthOffset = 0;

			for( i = 0; i < m_OptData.nPrimCount; i++)
			{
				nPrimLength = m_OptData.nPrimLength[i] + 2;		

				for( j = 0; j < nPrimLength; j++)		//	Primitive Length 만큼 반복
				{
					//	Unique Vertex 정보인지 판단하여 Optimize Data에 저장
					bUnique = FindUniqueVertex( pSrcVA, j + nLengthOffset);

					if( bBuildIndex)
					{
						AddIndex( pSrcVA, j + nLengthOffset);		
					}
				}

				nLengthOffset += nPrimLength;

				if( g_bTraceOutput)	I3TRACE( "BuildOptimize : Triangle Strips count[%d] = %d\n", i, nPrimLength);
			}
		}
		else
		{
			for( i = 0; i < count; i++)
			{
				//	Unique Vertex 정보인지 판단하여 Optimize Data에 저장
				bUnique = FindUniqueVertex( pSrcVA, i);

				if( bBuildIndex)
				{
					AddIndex( pSrcVA, i);		
				}
			}		
		}
	}
	else
	{
		for( i = 0; i < count; i++)
		{
			//	Unique Vertex 정보인지 판단하여 Optimize Data에 저장
			bUnique = FindUniqueVertex( pSrcVA, i);

			if( bBuildIndex)
			{
				AddIndex( pSrcVA, i);		//	현재 Vertex 정보와 Optimize Data에 저장된 vertex를 비교하여 인덱스 추가			
			}
		}			
	}
	pSrcVA->Unlock();

	return true;
}

bool i3OptBuildIndexedGeometry::RenewVerticesFromOpt( i3VertexArray * pOutVA, i3IndexArray * pOutIA, i3VertexFormat * pFormat)
{	
	if( (pOutVA == nullptr && pOutIA == nullptr) || pFormat == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	UINT32 i=0;
	OPT_VERTICES * pOptData = &m_OptData;

	//	Vertex Array 복사
	if( pOutVA) 
	{
		if( pOptData->nVertexCount > 0)
		{
			pOutVA->Create( pFormat, pOptData->nVertexCount, 0); 

			//I3TRACE( "RenewVerticesFromOpt : Renew Vertex (%d)\n", pOptData->nVertexCount);
						
			pOutVA->Lock();
			{
				for( i=0; i<m_OptData.nVertexCount; i++)
				{
					CopyVertexFromOpt( pOutVA, i, i);					
				}
			}
			pOutVA->Unlock();
		}
		else
		{
			SetResult( I3OPT_FAIL_INVALID_VAL);
			I3TRACE("RenewVerticesFromOpt : warning! none built vertex data. can not build vertex array\n");
		}
	}
	else
	{
		I3TRACE("RenewVerticesFromOpt : Dest VertexArray is nullptr!\n");
		return false;		//	Vertex가 없으니 index 복사도 의미가 없다.
	}

	//	Index Array 복사
	if( pOutIA) 
	{
		if( pOptData->nIndexCount > 0)
		{
			pOutIA->Create( pOptData->nIndexCount, 0, pOutVA->GetCount()); 

			//I3TRACE( "RenewVerticesFromOpt : Renew Index Array (%d)\n", pOptData->nIndexCount);

			pOutIA->Lock();
			{
				//	반복해서 인덱스를 저장한다.
				for( i=0; i < pOptData->nIndexCount; i++)
				{
					pOutIA->SetIndex( i, (UINT32) pOptData->Indices[i]);		// 무슨 이유인지 몰라도, 16비트컨테이너원소에서 32비트값으로 넘어감..(변경있었나)
				}
			}
			pOutIA->Unlock();	
		}
		else
		{
			SetResult( I3OPT_FAIL_INVALID_VAL);
			I3TRACE("RenewVerticesFromOpt : warning! none builed index data. can not build index array\n");
		}
	}

	return true;
}

void i3OptBuildIndexedGeometry::InitOpt( i3GeometryAttr * pGeoAttr, OPT_VERTICES * pOpt)
{
	if( pGeoAttr == nullptr || pOpt == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return;
	}

	UINT32	i = 0;
	UINT32	count = 0;
 
	i3VertexArray *	pSrcVA	= pGeoAttr->GetVertexArray();
	i3IndexArray *	pSrcIA	= pGeoAttr->GetIndexArray();

	if( pSrcIA)
		count = pSrcIA->GetCount();			
	else
		count = pSrcVA->GetCount();			

	i3::vu::clear_capacity(pOpt->nPrimLength);

	//	Optimize Data 초기화	
	pOpt->nPrimCount	= pGeoAttr->GetPrimitiveCount();
	pOpt->nPrimType		= pGeoAttr->GetPrimitiveType();
	
	pOpt->nTriCount = 0;
	//	Triangle List
	if( pOpt->nPrimType == I3G_PRIM_TRILIST)
	{
		if( pSrcIA)
			pOpt->nTriCount = pSrcIA->GetCount() / 3;
		else
			pOpt->nTriCount = pSrcVA->GetCount() / 3;
	}
	//	Triangle Strip
	else if( pOpt->nPrimType == I3G_PRIM_TRISTRIP)
	{
		if( pOpt->nPrimCount > 1) 
		{
			UINT32 PrimLength = 0;

			pOpt->bIsPrimLength = true;

			for( i=0; i < (INT32) pOpt->nPrimCount; i++)
			{
				PrimLength = pGeoAttr->GetPrimitiveLength( i);
	
				//pOpt->nPrimLength.SetItem( i, (void*) PrimLength);			// 이 SetItem은 resize기능을 내포하는데, vector에는 이런 기능이 있어서는 안된다..
				i3::vu::set_value_force(pOpt->nPrimLength, i, PrimLength);

				pOpt->nTriCount += PrimLength;
			}			
		}
		else if( pOpt->nPrimCount == 1) 
		{
			pOpt->bIsPrimLength = false;

			if( pSrcIA)
				pOpt->nTriCount = pSrcIA->GetCount() - 2;
			else
				pOpt->nTriCount = pSrcVA->GetCount() - 2;
		}
	}	 
	else
	{
		SetResult( I3OPT_FAIL_NOT_SUPPORT);
		I3TRACE("InitOpt : warning! Not support primitive type.\n");
	}

	pOpt->nVertexCount = 0;
	pOpt->nIndexCount = 0;
	pOpt->nTexSetCount = pSrcVA->GetFormat()->GetTextureCoordSetCount();	//	텍스처 갯수
	
	if( pOpt->nTexSetCount > MAX_TEXTURE)
	{
		I3TRACE("i3OptBuildIndexedGeometry : Warning! This function support textures of max (%d)\n", MAX_TEXTURE);
		I3ASSERT_0;
	}

	pOpt->nTexcoordCount = pSrcVA->GetFormat()->GetTextureCoordCount();		//	텍스처 좌표 갯수
	pOpt->nBlendWeightCount = pSrcVA->GetBlendWeightCount();		//	블랜드 가중치 갯수
	I3ASSERT( pOpt->nBlendWeightCount <= 4);
	pOpt->nBlendIndexCount = pSrcVA->GetBlendIndexCount();			//	블랜 가중치 인덱스 갯수
	I3ASSERT( pOpt->nBlendIndexCount <= MAX_BLEND_INDEX);

	pOpt->vPositionArray.SetVectorCount( count);					//	Position 갯수
	pOpt->vNormalArray.SetVectorCount( count);						//	Normal 갯수
	pOpt->vColorArray.SetVectorCount( count);						//	Color 갯수
	pOpt->vTangentArray.SetVectorCount( count);						//	Tangent 갯수
	pOpt->vBiNormalArray.SetVectorCount( count);					//	BiNormal 갯수
	pOpt->vBlendWeightArray.SetVectorCount( count);					//	Blend 가중치 갯수

	for( i=0; i<pOpt->nTexSetCount; i++)
		pOpt->vTexcoordArray[i].SetVectorCount( count);				//	Texture Coord 갯수

	for( i=0; i<pOpt->nBlendIndexCount; i++)	
	{
		i3::vu::clear_capacity(pOpt->nBlendIndexArray[i]);			//	Blend 가중치 인덱스 갯수
	}

	i3::vu::clear_capacity(pOpt->Indices);						
	
	pOpt->bHasPosition = pSrcVA->HasPosition();
	pOpt->bHasNormal = pSrcVA->HasNormal();
	pOpt->bHasColor = pSrcVA->HasColor();
	pOpt->bHasTangent = pSrcVA->HasTangent();
	pOpt->bHasBiNormal = pSrcVA->HasBinormal();
	pOpt->bHasTexcoord = pOpt->nTexSetCount > 0 ? true : false;
	pOpt->bHasBlendIndex = pSrcVA->GetFormat()->GetBlendIndexCount() > 0 ? true : false;
	pOpt->bHasBlendWeight = pSrcVA->GetFormat()->GetBlendWeightCount() > 0 ? true : false;
}

bool i3OptBuildIndexedGeometry::RemapIndicesToOpt( i3GeometryAttr * pGeoAttr)
{
	if( pGeoAttr == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	UINT32	i = 0, j = 0;
	UINT32	count = 0;
	UINT16	Idx1 = 0, Idx2 = 0, Idx3 = 0;
	
	i3VertexArray *	pSrcVA = pGeoAttr->GetVertexArray();
	i3IndexArray *	pSrcIA = pGeoAttr->GetIndexArray();

	if( pSrcVA == nullptr)
	{
		SetResult( I3OPT_FAIL_NONE_VERTEX);
		I3TRACE("RemapIndicesToOpt : None Vertex array data in Geometry (0x%x)\n", pGeoAttr);
		return false;
	}

	if( pSrcIA == nullptr)
	{
		SetResult( I3OPT_FAIL_NONE_INDEX);
		I3TRACE("RemapIndicesToOpt : None Index array data in Geometry (0x%x)\n", pGeoAttr);
		return false;
	}

	count		= pSrcIA->GetCount();

	//	Optimize Data 초기화	
	InitOpt( pGeoAttr, &m_OptData);
	
	//	최적화 작업 및 인덱스 생성
	pSrcVA->Lock();	
	pSrcIA->Lock();	

	//	Triangle List
	if( m_OptData.nPrimType == I3G_PRIM_TRILIST)			
	{
		INT32 idxTri = 0;
		if( g_bTraceOutput)	I3TRACE( "RemapIndicesToOpt : Berore PrimCount = %d\n", m_OptData.nPrimCount);

		for( i = 0; i < count; i += 3)		//	1개의 Triangle은 Vertex를 3개씩 생성한다.
		{
			Idx1 = (UINT16) pSrcIA->GetIndex( i);
			Idx2 = (UINT16) pSrcIA->GetIndex( i + 1);
			Idx3 = (UINT16) pSrcIA->GetIndex( i + 2);

			//	의미없는 triangle은 무시하도록 합니다.
			if( Idx1 == Idx2 || Idx1 == Idx3 || Idx2 == Idx3)
			{
				if( g_bTraceOutput)	I3TRACE("RemapIndicesToOpt : Ignored indices (%d, %d, %d)\n", Idx1, Idx2, Idx3);
			}
			else
			{
				//	Index를 참조하여 Triangle List 만들기
				BuildTriangleFromIndex( pSrcVA, pSrcIA, i, idxTri * 3);

				idxTri++;
			}
		}	

		m_OptData.nPrimCount = idxTri;

		if( g_bTraceOutput)	I3TRACE( "RemapIndicesToOpt : After PrimCount = %d\n", m_OptData.nPrimCount);
		if( g_bTraceOutput)	I3TRACE( "RemapIndicesToOpt : Triangle List count = %d\n", count);
	}
	//	Triangle Strip
	else if( m_OptData.nPrimType == I3G_PRIM_TRISTRIP)
	{		
		if( m_OptData.bIsPrimLength)
		{
			UINT32 nPrimLength = 0;
			UINT32 nLengthOffset = 0;

			for( i = 0; i < m_OptData.nPrimCount; i++)
			{
				nPrimLength = m_OptData.nPrimLength[i] + 2;
				
				for( j = 0; j < nPrimLength; j++)		//	Primitive Length 만큼 반복
				{
					//	Index를 참조하여 Triangle Strip 만들기
					BuildStripFromIndex( pSrcVA, pSrcIA, j + nLengthOffset);
				}

				nLengthOffset += nPrimLength;

				if( g_bTraceOutput)	I3TRACE( "RemapIndicesToOpt : Triangle Strips count[%d] = %d\n", i, nPrimLength);
			}			
		}
		else
		{
			for( i = 0; i < count; i++)
			{
				//	Index를 참조하여 Triangle Strip 만들기
				BuildStripFromIndex( pSrcVA, pSrcIA, i);				
			}

			if( g_bTraceOutput)	I3TRACE( "RemapIndicesToOpt : Triangle Strips count = %d\n", count);
		}
	}
	
	pSrcIA->Unlock();
	pSrcVA->Unlock();	

	return true;
}

bool i3OptBuildIndexedGeometry::BuildTriangleFromIndex( i3VertexArray * pLockedVA, i3IndexArray * pLockedVI, UINT32 nIdx, INT32 idxOut)
{
	if( pLockedVA == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		I3TRACE("BuildTriangleFromIndex : Can not build triangle. None Vertex Buffer\n");
		return false;
	}

	if( pLockedVI == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		I3TRACE("BuildTriangleFromIndex : Can not build triangle. None Index Buffer\n");
		return false;
	}

	INT32	i = 0;
	UINT32	nVertexIdx = 0;	//	vertex의 index
	UINT32	nDebugIdx[3];

	//	Triangle Vertex 정보를 Optimize Data에 복사	
	for( i=0; i<3; i++)		
	{
		nVertexIdx = (UINT32) pLockedVI->GetIndex( nIdx + i);		//	IndexBuffer에서 Vertex의 Index를 받아온다.
		nDebugIdx[i] = nVertexIdx;	//	Debug 출력용

		if( CopyVertexToOpt( pLockedVA, nVertexIdx, idxOut + i))
		{
			m_OptData.nVertexCount++;		//	Optimize data에 Vertex Count 증가
		}
		else
		{
			SetResult( I3OPT_FAIL);
			I3TRACE("BuildTriangleFromIndex : Failed built triangle");
			return false;
		}
	}	

	//I3TRACE4("i3OptBuildIndexedGeometry : Built triangle from index(%d) = %d, %d, %d\n", nIdx, nDebugIdx[0], nDebugIdx[1], nDebugIdx[2]);
	
	return true;
}

bool i3OptBuildIndexedGeometry::BuildStripFromIndex( i3VertexArray * pLockedVA, i3IndexArray * pLockedVI, UINT32 nIdx)
{
	if( pLockedVA == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		I3TRACE("BuildStripFromIndex : Can not build triangle. None Vertex Buffer\n");
		return false;
	}

	if( pLockedVI == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		I3TRACE("BuildStripFromIndex : Can not build triangle. None Index Buffer\n");
		return false;
	}

	INT32	i = 0;
	UINT32	nVertexIdx = 0;	//	vertex의 index
	UINT32	nDebugIdx[3];

	//	Triangle Vertex 정보를 Optimize Data에 복사	
	nVertexIdx = (UINT32) pLockedVI->GetIndex( nIdx);		//	IndexBuffer에서 Vertex의 Index를 받아온다.
	nDebugIdx[i] = nVertexIdx;	//	Debug 출력용

	if( CopyVertexToOpt( pLockedVA, nVertexIdx, nIdx))
	{
		m_OptData.nVertexCount++;		//	Optimize data에 Vertex Count 증가
	}
	else
	{
		SetResult( I3OPT_FAIL);
		I3TRACE("BuildStripFromIndex : Failed built triangle");
		return false;
	}
	
	//I3TRACE2("i3OptBuildIndexedGeometry : Built Strip from index(%d) = %d\n", nIdx, nVertexIdx);
	
	return true;
}

bool i3OptBuildIndexedGeometry::FindUniqueVertex( i3VertexArray * pLockedVA, UINT32 nSrcIdx)
{		
	if( pLockedVA == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	//	Unique Vertex 정보인지 판별
	if( IsUniqueVertexFromOpt( pLockedVA, nSrcIdx) == true)
	{
		UINT32 nUniqueNum = m_OptData.nVertexCount;

		//	Unique면 Optimize buffer에 Vertex 복사
		if( CopyVertexToOpt( pLockedVA, nSrcIdx, nUniqueNum))	
		{
			m_OptData.nVertexCount++;		//	Optimize data에 Vertex Count 증가

			return true;
		}
	}
	/*else	//	Debug 코드
	{
		VEC3D		TempVec3D;
		pLockedVA->GetPosition( nSrcIdx, &TempVec3D);
		I3TRACE4( "FindUniqueVertex : Equel Vertex[%d] = (%f, %f, %f)\n", nSrcIdx, TempVec3D.vec[0], TempVec3D.vec[1], TempVec3D.vec[2]);
	}*/

	return false;
}

bool i3OptBuildIndexedGeometry::CopyVertexFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx)
{
	if( pLockedVA == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	UINT32 i = 0;
	OPT_VERTICES *	pOptData = &m_OptData;
	i3Vector3Array * pOptPosArray		= &pOptData->vPositionArray;
	i3Vector3Array * pOptNormalArray	= &pOptData->vNormalArray;
	i3Vector4Array * pOptColorArray		= &pOptData->vColorArray;
	i3Vector3Array * pOptTangentArray	= &pOptData->vTangentArray;
	i3Vector3Array * pOptBiNormalArray	= &pOptData->vBiNormalArray;
	COLORREAL		 color;

	//	Position 복사
	if( pOptData->bHasPosition)
	{
		VEC3D * pVec = pOptPosArray->GetVector( nOptIdx);
#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
		I3ASSERT( i3Math::isValid( pVec->x));
		I3ASSERT( i3Math::isValid( pVec->y));
		I3ASSERT( i3Math::isValid( pVec->z));
#endif

		pLockedVA->SetPosition( nSrcIdx,  pVec);		
	}

	//	Normal 복사
	if( pOptData->bHasNormal)
		pLockedVA->SetNormal( nSrcIdx, pOptNormalArray->GetVector( nOptIdx));		

	//	Vertex Color 복사
	if( pOptData->bHasColor)
	{
		i3Color::Set(	&color, 
						pOptColorArray->GetVector( nOptIdx)->vec[0], 
						pOptColorArray->GetVector( nOptIdx)->vec[1],
						pOptColorArray->GetVector( nOptIdx)->vec[2],
						pOptColorArray->GetVector( nOptIdx)->vec[3] );

		pLockedVA->SetColor( nSrcIdx, &color);								
	}

	//	Tanget 복사
	if( pOptData->bHasTangent)
		pLockedVA->SetTangent( nSrcIdx, pOptTangentArray->GetVector( nOptIdx));

	//	BiNormal 복사
	if( pOptData->bHasBiNormal)
		pLockedVA->SetBinormal( nSrcIdx, pOptBiNormalArray->GetVector( nOptIdx));

	//	Texture Coord 복사
	if( pOptData->bHasTexcoord)
	{
		for(i=0; i<pOptData->nTexSetCount; i++)
		{
			VEC3D * pTexcoord = pOptData->vTexcoordArray[ i].GetVector( nOptIdx);

			switch( pOptData->nTexcoordCount)
			{
			case 1:					//	1차원 텍스처 좌표
				break;
			case 2:					//	2차원 텍스처 좌표
				{
					VEC2D vTexcoord2D;
					i3Vector::Set( &vTexcoord2D, pTexcoord->vec[0], pTexcoord->vec[1]);
					pLockedVA->SetTextureCoord( i, nSrcIdx, &vTexcoord2D);
				}
				break;
			case 3:					//	3차원 텍스처 좌표
				{	
					VEC3D vTexcoord3D;
					i3Vector::Set( &vTexcoord3D, pTexcoord->vec[0], pTexcoord->vec[1], pTexcoord->vec[2]);
					//pLockedVA->SetTextureCoord( i, nSrcIdx, &vTexcoord3D);
				}
				break;
			default: I3ASSERT_0;	break;
			}
		}
	}

	//	Blend Index 복사
	if( pOptData->bHasBlendIndex)
	{
		for( i = 0; i < pOptData->nBlendIndexCount; i++)
		{
			UINT32 nBlendIndex = pOptData->nBlendIndexArray[i][nOptIdx];
				
			pLockedVA->SetBlendIndex( i, nSrcIdx, nBlendIndex);
		}
	}

	//	Blend Weight 복사
	if( pOptData->bHasBlendWeight)
	{
		VEC4D vBlendWeight;		
		i3Vector::Copy( &vBlendWeight, pOptData->vBlendWeightArray.GetVector( nOptIdx));

		for( i = 0; i < pOptData->nBlendWeightCount; i++)
		{
			pLockedVA->SetBlendWeight( i, nSrcIdx, vBlendWeight.vec[i]);
		}
	}

	return true;
}

bool i3OptBuildIndexedGeometry::CopyVertexToOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx)
{
	if( pLockedVA == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	UINT32 i = 0;
	OPT_VERTICES *	pOptData = &m_OptData;
	VEC2D		TempVec2D;
	VEC3D		TempVec3D;
	VEC4D		TempVec4D;		
	COLORREAL		TempColor;

	//	Unique Vertex 정보를 Optimize Data에 복사
	if( pOptData->bHasPosition)
	{
		pLockedVA->GetPosition( nSrcIdx, &TempVec3D);

#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
		I3ASSERT( i3Math::isValid( TempVec3D.x));
		I3ASSERT( i3Math::isValid( TempVec3D.y));
		I3ASSERT( i3Math::isValid( TempVec3D.z));
#endif

		pOptData->vPositionArray.SetVector( nOptIdx, &TempVec3D); 
	}

	if( pOptData->bHasNormal)
	{
		pLockedVA->GetNormal( nSrcIdx, &TempVec3D);
		pOptData->vNormalArray.SetVector( nOptIdx, &TempVec3D); 
	}
	
	if( pOptData->bHasColor)
	{
		pLockedVA->GetColor( nSrcIdx, &TempColor);

		TempVec4D.vec[0] = i3Color::GetR( &TempColor);		//	R
		TempVec4D.vec[1] = i3Color::GetG( &TempColor);		//	G
		TempVec4D.vec[2] = i3Color::GetB( &TempColor);		//	B
		TempVec4D.vec[3] = i3Color::GetA( &TempColor);		//	A

		pOptData->vColorArray.SetVector( nOptIdx, &TempVec4D); 
	}

	if( pOptData->bHasTangent)
	{
		pLockedVA->GetTangent( nSrcIdx, &TempVec3D);
		pOptData->vTangentArray.SetVector( nOptIdx, &TempVec3D); 
	}

	if( pOptData->bHasBiNormal)
	{
		pLockedVA->GetBinormal( nSrcIdx, &TempVec3D);
		pOptData->vBiNormalArray.SetVector( nOptIdx, &TempVec3D); 
	}

	if( pOptData->bHasTexcoord)
	{
		UINT32 i = 0;
		for(i=0; i< m_OptData.nTexSetCount; i++)
		{
			switch( m_OptData.nTexcoordCount)
			{
			case 1:			//	1차원 텍스처 좌표 (나중에 구현)
				{
					//i3Vector::Set( &TempVec3D, TempVec2D.vec[0], 0.0f, 0.0f);
					pOptData->vTexcoordArray[i].SetVector( nOptIdx, &TempVec3D); 
				}
				break;
			case 2:			//	2차원 텍스처 좌표
				{
					pLockedVA->GetTextureCoord(i, nSrcIdx, &TempVec2D);

					i3Vector::Set( &TempVec3D, TempVec2D.vec[0], TempVec2D.vec[1], 0.0f);
					pOptData->vTexcoordArray[i].SetVector( nOptIdx, &TempVec3D); 
				}
				break;
			case 3:			//	3차원 텍스처 좌표 (나중에 구현)
				{
					pOptData->vTexcoordArray[i].SetVector( nOptIdx, &TempVec3D); 
				}
				break;
			default:	I3ASSERT_0;	
				break;
			}					
		}
	}

	if( pOptData->bHasBlendIndex)
	{
		for( i = 0; i < pOptData->nBlendIndexCount; i++)
		{
			UINT32 nBlendIndex = pLockedVA->GetBlendIndex( i, nSrcIdx);

			pOptData->nBlendIndexArray[ i].push_back(nBlendIndex);
		}
	}

	if( pOptData->bHasBlendWeight)
	{
		i3Vector::Set( &TempVec4D, 0.0f, 0.0f, 0.0f, 0.0f);
		
		for( i = 0; i < pOptData->nBlendWeightCount; i++)
		{
			TempVec4D.vec[i] = pLockedVA->GetBlendWeight( i, nSrcIdx);				
		}

		pOptData->vBlendWeightArray.SetVector( nOptIdx, &TempVec4D);
	}

	return true;
}

bool i3OptBuildIndexedGeometry::AddIndex( i3VertexArray * pLockedVA, UINT32 nIdx)
{ 
	//	Optimize Data에 이미 Vertex가 저장되어 있어야만 찾아서 index를 저장할 수 있다.
	if( pLockedVA == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}
	
	INT32		nOptInx = -1;

	OPT_VERTICES *	pOptData = &m_OptData;

	//	pLockedVA에 있는 Vertex와 Optimize Data에 저장된 Vertex의 비교하여 index를 반환
	nOptInx = CampareVertexFromOpt( pLockedVA, nIdx);
	if( nOptInx < 0)	//	저장된 Vertex가 없다면
	{
		I3TRACE1("AddIndex : warning! Not matched index in optimize data (Idx : %d)\n", nIdx);
	}
	else
	{
		pOptData->Indices.push_back( (UINT16) nOptInx);	//	Vertex의 index를 추가

		//I3TRACE1("Add Index = %d\n", nOptInx);

		pOptData->nIndexCount++;	//	Index 갯수 증가

		return true;
	}

	return false;		//	기존의 Vertex와 공유할 index가 없다!!	
}

INT32 i3OptBuildIndexedGeometry::ComparePositionInOpt( UINT32 nIdx1, UINT32 nIdx2)
{
	VEC3D 	vOptPos1, vOptPos2;

	if( m_OptData.bHasPosition)
	{
		i3Vector::Copy( &vOptPos1, m_OptData.vPositionArray.GetVector( nIdx1));
		i3Vector::Copy( &vOptPos2, m_OptData.vPositionArray.GetVector( nIdx2));

		return (INT32) i3Vector::IsSame( &vOptPos1, &vOptPos2);
	}

	return -1;
}

INT32 i3OptBuildIndexedGeometry::ComparePositionFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx)
{
	if( pLockedVA == nullptr) I3ASSERT_0;
	
	if( m_OptData.bHasPosition)
	{			
		VEC3D * pOptPos = m_OptData.vPositionArray.GetVector( nOptIdx);

		return (INT32) pLockedVA->IsSamePosition( nSrcIdx, pOptPos);
	}

	return -1;
}

INT32 i3OptBuildIndexedGeometry::CompareNormalInOpt( UINT32 nIdx1, UINT32 nIdx2)
{
	VEC3D	vOptNormal1, vOptNormal2;

	if( m_OptData.bHasNormal)
	{
		i3Vector::Copy( &vOptNormal1, m_OptData.vNormalArray.GetVector( nIdx1));
		i3Vector::Copy( &vOptNormal2, m_OptData.vNormalArray.GetVector( nIdx2));

		return (INT32) i3Vector::IsSame( &vOptNormal1, &vOptNormal2);
	}

	return -1;
}

INT32 i3OptBuildIndexedGeometry::CompareNormalFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx)
{
	if( pLockedVA == nullptr) I3ASSERT_0;

	if( m_OptData.bHasNormal)
	{	
		VEC3D * pOptNormal = m_OptData.vNormalArray.GetVector( nOptIdx);

		return (INT32) pLockedVA->IsSameNormal( nSrcIdx, pOptNormal);
	}

	return -1;
}


INT32 i3OptBuildIndexedGeometry::CompareColorInOpt( UINT32 nIdx1, UINT32 nIdx2)
{
	VEC4D	vTempColor;
	COLORREAL	color1, color2;

	if( m_OptData.bHasColor)
	{
		i3Vector::Copy( &vTempColor, m_OptData.vColorArray.GetVector( nIdx1));
		i3Color::Set( &color1, vTempColor.vec[0], vTempColor.vec[1], vTempColor.vec[2], vTempColor.vec[3] );

		i3Vector::Copy( &vTempColor, m_OptData.vColorArray.GetVector( nIdx2));
		i3Color::Set( &color2, vTempColor.vec[0], vTempColor.vec[1], vTempColor.vec[2], vTempColor.vec[3] );

		return (INT32) i3Color::IsSame( &color1, &color2);
	}

	return -1;
}

INT32 i3OptBuildIndexedGeometry::CompareColorFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx)
{
	if( pLockedVA == nullptr)
	{
		I3ASSERT_0;
		return -1;
	}
	
	if( m_OptData.bHasColor)
	{
		VEC4D	vColor;
		COLORREAL	OptColor;

		i3Vector::Copy( &vColor, m_OptData.vColorArray.GetVector( nOptIdx));
		i3Color::Set( &OptColor, vColor.vec[0], vColor.vec[1], vColor.vec[2], vColor.vec[3] );

		return (INT32) pLockedVA->IsSameColor( nSrcIdx, &OptColor);
	}

	return -1;
}

INT32 i3OptBuildIndexedGeometry::CompareTangentInOpt( UINT32 nIdx1, UINT32 nIdx2)
{
	VEC3D	vOptTangent1, vOptTangent2;

	if( m_OptData.bHasTangent)
	{
		i3Vector::Copy( &vOptTangent1, m_OptData.vTangentArray.GetVector( nIdx1));
		i3Vector::Copy( &vOptTangent2, m_OptData.vTangentArray.GetVector( nIdx2));

		return (INT32) i3Vector::IsSame( &vOptTangent1, &vOptTangent2);
	}

	return -1;
}

INT32 i3OptBuildIndexedGeometry::CompareTangentFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx)
{
	if( pLockedVA == nullptr) I3ASSERT_0;

	if( m_OptData.bHasTangent)
	{	
		VEC3D * pOptTangent = m_OptData.vTangentArray.GetVector( nOptIdx);

		return (INT32) pLockedVA->IsSameTangent( nSrcIdx, pOptTangent);
	}

	return -1;
}

INT32 i3OptBuildIndexedGeometry::CompareBiNormalInOpt( UINT32 nIdx1, UINT32 nIdx2)
{
	VEC3D	vOptBiNormal1, vOptBiNormal2;

	if( m_OptData.bHasBiNormal)
	{
		i3Vector::Copy( &vOptBiNormal1, m_OptData.vBiNormalArray.GetVector( nIdx1));
		i3Vector::Copy( &vOptBiNormal2, m_OptData.vBiNormalArray.GetVector( nIdx2));

		return (INT32) i3Vector::IsSame( &vOptBiNormal1, &vOptBiNormal2);
	}

	return -1;
}

INT32 i3OptBuildIndexedGeometry::CompareBiNormalFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx)
{
	if( pLockedVA == nullptr) I3ASSERT_0;

	if( m_OptData.bHasBiNormal)
	{	
		VEC3D * pOptBiNormal = m_OptData.vBiNormalArray.GetVector( nOptIdx);

		return (INT32) pLockedVA->IsSameBinormal( nSrcIdx, pOptBiNormal);
	}

	return -1;
}

INT32 i3OptBuildIndexedGeometry::CompareBlendIndexFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx)
{
	if( pLockedVA == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		I3ASSERT_0;
	}
	
	INT32		ret = 1;
	INT32		i = 0;
	UINT32		j = 0;

	UINT32		nOptBlendIndex = 0;
	UINT32		nSrcBlendIndex = 0;

	//	차례대로 원본의 Blend Index에 참조된 Blend Weight값들을 하나씩 비교
	if( m_OptData.bHasBlendIndex)
	{		
		//	Debugging Info - 비교 값들을 보려면 활성화 하세요.
		/*{
			//	Src Value
			I3TRACE1( "CompareBlendWeightFromOpt : Src Blend Index(%d) (", nSrcIdx);
			for( i=0; i < pLockedVA->GetBlendIndexCount(); i++)
				I3TRACE1( " %d,", pLockedVA->GetBlendIndex( i, nSrcIdx));
			I3TRACE(")\n");

			//	Opt Value
			I3TRACE1( "CompareBlendWeightFromOpt : Opt Blend Index(%d) (", nOptIdx);
			for( j=0; j < m_OptData.nBlendIndexCount; j++)
				I3TRACE1( " %d,", (UINT32) m_OptData.nBlendIndexArray[ j].GetItem( nOptIdx));
			I3TRACE(")\n");
		}*/

		for( i=0; i < pLockedVA->GetBlendIndexCount(); i++)
		{
			nSrcBlendIndex	= pLockedVA->GetBlendIndex( i, nSrcIdx);		//	비교할 원본 Blend Index 

			for( j=0; j < m_OptData.nBlendIndexCount; j++)
			{
				nOptBlendIndex = m_OptData.nBlendIndexArray[ j][nOptIdx];	//	Optimize Data에 저장된 Blend Index

				//	순서가 달라도 같은 Blend Index가 있다면
				if( nSrcBlendIndex == nOptBlendIndex)
				{
					break;		//	동일한 Blend Index 이다.
				}
				else if( j == (m_OptData.nBlendIndexCount - 1))	
				{
					ret = 0;	//	동일한 Blend Index가 전혀 없다.
					goto LABEL_OUT;
				}
			}
		}
	}
	else
	{
		ret = -1;	//	Blend Weight 값이 저장되어 있지 않다.
	}

LABEL_OUT:
	return ret;
}


INT32 i3OptBuildIndexedGeometry::CompareBlendWeightFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx)
{
	if( pLockedVA == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		I3ASSERT_0;
	}

	INT32		ret = 1;
	INT32		i = 0;
	UINT32		j = 0;
	
	VEC4D	vOptBlendWeight = VEC4D( 0.0f, 0.0f, 0.0f, 0.0f );
	REAL32	fSrcBlendWeight = 0.0f;
	
	if( m_OptData.bHasBlendWeight)
	{			
		//	비교할 Optimize Data에 저장된 Blend Weight값
		i3Vector::Copy( &vOptBlendWeight, m_OptData.vBlendWeightArray.GetVector( nOptIdx));	

		//	Debugging Info - 비교 값들을 보려면 활성화 하세요.
		/*{
			I3TRACE1( "CompareBlendWeightFromOpt : Src Blend Weight(%d) (", nSrcIdx);
			for( i=0; i < pLockedVA->GetBlendWeightCount(); i++)
				I3TRACE1( " %.4f,", pLockedVA->GetBlendWeight( i, nSrcIdx));
			I3TRACE(")\n");

			I3TRACE1( "CompareBlendWeightFromOpt : Opt Blend Weight(%d) (", nOptIdx);
			for( j=0; j < m_OptData.nBlendWeightCount; j++)
				I3TRACE1( " %.4f,", vOptBlendWeight.vec[ j]);
			I3TRACE(")\n");
		}*/

		//	차례대로 하나씩 값을 비교
		for( i = 0; i < pLockedVA->GetBlendWeightCount(); i++)
		{
			fSrcBlendWeight	= pLockedVA->GetBlendWeight( i, nSrcIdx);			// 비교할 원본 Blend Weight 값

			for( j = 0; j < m_OptData.nBlendWeightCount; j++)
			{
				//	순서가 달라도 같은 Blend Weight가 있다면
				if( fSrcBlendWeight == vOptBlendWeight.vec[ j])
				{
					break;	//	동일한 Blend Weight 이다.
				}
				else if( j == (m_OptData.nBlendWeightCount - 1))	
				{
					ret = 0;	//	동일한 Blend Weight 값이 전혀 없다.
					goto LABEL_OUT;
				}
			}
		}	
	}
	else
	{
		ret = -1;	//	Blend Weight 값이 저장되어 있지 않다.
	}

LABEL_OUT:
	return ret;
}

INT32 i3OptBuildIndexedGeometry::CompareTextureCoordInOpt( UINT32 nIdx1, UINT32 nIdx2)
{
	UINT32		i = 0;
	INT32		ret = 1;
	VEC3D * Tex1 = nullptr;
	VEC3D * Tex2 = nullptr;

	if( m_OptData.bHasTexcoord)
	{
		//	2 이상이면 멀티 텍스처
		for( i=0; i < m_OptData.nTexSetCount; i++)
		{
			switch( m_OptData.nTexcoordCount)
			{	
			case 1:		//	TextureCoord 좌표계가 1차원일 경우 ( 나중에 구현!! by KOMET)
				{
					I3TRACE("i3OptBuildIndexedGeometry : This texture coordinate is not 2D!! not support\n"); 

					ret = 0;
					goto LABEL_OUT;
				}
				break;
			case 2:		//	TextureCoord 좌표계가 2차원일 경우만 일단 구현					
				{
					Tex1 = m_OptData.vTexcoordArray[i].GetVector( nIdx1);
					Tex2 = m_OptData.vTexcoordArray[i].GetVector( nIdx2);
					
					//	텍스처 좌표가 다르면
					//if( !i3Vector::IsSame( &Tex1, &Tex2))
					if( (Tex1->vec[0] != Tex2->vec[0]) || (Tex1->vec[1] != Tex2->vec[1]))
					{
						ret = 0;		
						goto LABEL_OUT;
					}
				}		
				break;	
			case 3:		//	TextureCoord 좌표계가 3차원일 경우 ( 나중에 구현!! by KOMET)
				{
					I3TRACE("i3OptBuildIndexedGeometry : This texture coordinate is not 2D!! not support\n"); 

					ret = 0;
					goto LABEL_OUT;
				}
				break;
			default:	I3ASSERT_0;	
				break;
			}
		}	
	}
	else
	{
		ret = -1;	//	TextureCoord 값이 없다.
	}

LABEL_OUT:
	return ret;	
}

INT32 i3OptBuildIndexedGeometry::CompareTextureCoordFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx, UINT32 nOptIdx)
{
	if( pLockedVA == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	UINT32		i = 0;
	INT32		ret = 1;
	//VEC2D	Tex1;

	if( m_OptData.bHasTexcoord)
	{
		//	2 이상이면 멀티 텍스처
		for( i=0; i < m_OptData.nTexSetCount; i++)
		{
			switch( m_OptData.nTexcoordCount)
			{	
			case 1:		//	TextureCoord 좌표계가 1차원일 경우 ( 나중에 구현!! by KOMET)
				{
					I3TRACE("i3OptBuildIndexedGeometry : This texture coordinate is not 2D!! not support\n"); 

					ret = 0;
					goto LABEL_OUT;
				}
				break;
			case 2:		//	TextureCoord 좌표계가 2차원일 경우만 일단 구현					
				{					
					VEC3D * Tex2 = m_OptData.vTexcoordArray[i].GetVector( nOptIdx);
					VEC2D temp;
					i3Vector::Set( &temp, Tex2->vec[0], Tex2->vec[1]);
					//pLockedVA->GetTextureCoord( i, nSrcIdx, &Tex1);					
					
					//	텍스처 좌표가 다르면
					//if( !i3Vector::IsSame( &Tex1, &Tex2))
					//if( (Tex1.vec[0] != Tex2->vec[0]) || (Tex1.vec[1] != Tex2->vec[1]))
					if( !pLockedVA->IsSameTextureCoord( i, nSrcIdx, &temp))
					{
						ret = 0;		
						goto LABEL_OUT;
					}
				}		
				break;	
			case 3:		//	TextureCoord 좌표계가 3차원일 경우 ( 나중에 구현!! by KOMET)
				{
					I3TRACE("i3OptBuildIndexedGeometry : This texture coordinate is not 2D!! not support\n"); 

					ret = 0;
					goto LABEL_OUT;
				}
				break;
			default:	I3ASSERT_0;	
				break;
			}
		}	
	}
	else
	{
		ret = -1;		//	Texture Coord 값이 없다.
	}

LABEL_OUT:
	return ret;				
}

bool i3OptBuildIndexedGeometry::IsUniqueVertexFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx)
{
	if( pLockedVA == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	UINT32	i=0;
	UINT32 nOptIdx = 0;
	OPT_VERTICES *		pOptData = &m_OptData;
	
	for( i=0; i<pOptData->nVertexCount; i++)
	{
		nOptIdx = i;	//	현재 비교중인 번호

		///////////////////////////////////////////////////////////////////////////////		Position이 같으면
		if( ComparePositionFromOpt( pLockedVA, nSrcIdx, nOptIdx) == 1)	
		{
			//////////////////////////////////////////////////////////////////////////		Normal이 같거나 없으면
			if( CompareNormalFromOpt( pLockedVA, nSrcIdx, nOptIdx) != 0)	
			{
				//////////////////////////////////////////////////////////////////////		Color가 같거나 없으면
				if( CompareColorFromOpt( pLockedVA, nSrcIdx, nOptIdx) != 0)		
				{
					//////////////////////////////////////////////////////////////////		Tangent가 같거나 없으면
					if( CompareTangentFromOpt( pLockedVA, nSrcIdx, nOptIdx) != 0)		
					{
						//////////////////////////////////////////////////////////////		Binormal이 같거나 없으면
						if( CompareBiNormalFromOpt( pLockedVA, nSrcIdx, nOptIdx) != 0)		
						{
							//////////////////////////////////////////////////////////		Texturecoord가 같거나 없으면
							if( CompareTextureCoordFromOpt( pLockedVA, nSrcIdx, nOptIdx) != 0)	
							{
								//////////////////////////////////////////////////////		Blend Index가 같거나 없으면
								if( CompareBlendIndexFromOpt( pLockedVA, nSrcIdx, nOptIdx) != 0)	
								{
									//////////////////////////////////////////////////		Blend Weight가 같거나 없으면
									if( CompareBlendWeightFromOpt( pLockedVA, nSrcIdx, nOptIdx) != 0)	
										return false;		//	Vertex가 완전히 같으면 Unique Vertex가 아니다.
								}
							}
						}
					}
				}
			}
		}
	}

	return true;	//	Unique Vertex 입니다.
}

INT32 i3OptBuildIndexedGeometry::CampareVertexFromOpt( i3VertexArray * pLockedVA, UINT32 nSrcIdx)
{
	if( pLockedVA == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	UINT32	i=0;
	UINT32 nOptIdx = 0;
	OPT_VERTICES *		pOptData = &m_OptData;
	
	for( i=0; i<pOptData->nVertexCount; i++)
	{
		nOptIdx = i;	//	현재 비교중인 번호

		if( ComparePositionFromOpt( pLockedVA, nSrcIdx, nOptIdx) == 1)	//	Position이 같으면
		{
			if( CompareNormalFromOpt( pLockedVA, nSrcIdx, nOptIdx) != 0)	//	Normal이 같거나 없으면
			{
				if( CompareColorFromOpt( pLockedVA, nSrcIdx, nOptIdx) != 0)		//	Color가 같거나 없으면
				{
					if( CompareTextureCoordFromOpt( pLockedVA, nSrcIdx, nOptIdx) != 0)	//	Texturecoord가 같거나 없으면
					{
						if( CompareBlendIndexFromOpt( pLockedVA, nSrcIdx, nOptIdx) != 0)	//	Blend Index가 같거나 없으면
						{
							if( CompareBlendWeightFromOpt( pLockedVA, nSrcIdx, nOptIdx) != 0)	//	Blend Weight가 같거나 없으면
								return (INT32) i;		//	Vertex가 완전히 같은 index 번호 반환
						}
					}
				}
			}
		}
	}

	return -1;	//	Optimize Data에 같은 Vertex가 없다
}

void i3OptBuildIndexedGeometry::DumpVertexComponents( i3VertexArray * pVA, UINT32 nDumpType, UINT32 count)
{
#if defined ( I3_DEBUG)
	pVA->Lock();	
	{
		VEC2D vVec2;
		VEC3D vVec3;
		COLORREAL color;
		UINT32 nMaxCount = 0;  

		if( count == -1)
			nMaxCount = pVA->GetCount();
		else
			nMaxCount = count;

		I3TRACE("* DumpGeometry - START\n");

		//	Dump Vertex Components
		for( UINT32 idx = 0; idx < nMaxCount; idx++)
		{
			//	Dump position
			if( (nDumpType & DUMP_TYPE_POSITION) || (nDumpType & DUMP_TYPE_ALL))
			{
				if( pVA->HasPosition() == true)
				{
					pVA->GetPosition( idx, &vVec3);
					I3TRACE("Dump position (%d)    : x %.4f, y %.4f, z %.4f\n", idx, vVec3.x, vVec3.y, vVec3.z);
				}
			}
			//	Dump Normal
			if( (nDumpType & DUMP_TYPE_NORMAL) || (nDumpType & DUMP_TYPE_ALL))
			{
				if( pVA->HasNormal() == true)
				{
					pVA->GetNormal( idx, &vVec3);
					I3TRACE("Dump normal (%d)      : x %.4f, y %.4f, z %.4f\n", idx, vVec3.x, vVec3.y, vVec3.z);
				}
			}
			//	Dump Color
			if( (nDumpType & DUMP_TYPE_COLOR) || (nDumpType & DUMP_TYPE_ALL))
			{
				if( pVA->HasColor() == true)
				{
					pVA->GetColor( idx, &color);
					I3TRACE("Dump color (%d)       : R %.4f, G %.4f, B %.4f A %.4f\n", idx, i3Color::GetR( &color), 
																				 i3Color::GetG( &color), 
																				 i3Color::GetB( &color), 
																				 i3Color::GetA( &color));
				}
			}
			//	Dump Texturecoord
			if( (nDumpType & DUMP_TYPE_TEXCOORD) || (nDumpType & DUMP_TYPE_ALL))
			{		
				INT32 nTextureSetCnt = pVA->GetFormat()->GetTextureCoordSetCount();

				for( INT32 which = 0; which < nTextureSetCnt; which++)
				{
					pVA->GetTextureCoord( which, idx, &vVec2);  

					I3TRACE("Dump texturecoord (%d - %d) : u %.4f, v %.4f\n", idx, which, vVec2.x, vVec2.y);
				}
			}
			//	Dump Blend Index
			if( (nDumpType & DUMP_TYPE_BLENDINDEX) || (nDumpType & DUMP_TYPE_ALL))
			{
				for( INT32 which = 0; which < pVA->GetBlendIndexCount(); which++)
				{
					if( which == 0)		I3TRACE("Dump BlendIndex (%d)  : ", idx);
				
					I3TRACE("%d = %d, ", which, pVA->GetBlendIndex( which, idx));

					if( which == pVA->GetBlendIndexCount() - 1)		I3TRACE("\n");
				}
			}
			//	Dump Blend Weight
			if( (nDumpType & DUMP_TYPE_BLENDWEIGHT) || (nDumpType & DUMP_TYPE_ALL))
			{
				for( INT32 which = 0; which < pVA->GetBlendWeightCount(); which++)
				{
					if( which == 0)		I3TRACE("Dump BlendWeight (%d) : ", idx);
				
					I3TRACE("%d = %d, ", which, pVA->GetBlendWeight( which, idx));

					if( which == pVA->GetBlendWeightCount() - 1)		I3TRACE("\n");
				}
			}
		}

		I3TRACE("* DumpGeometry - END\n");
	}
	pVA->Unlock();
#endif
}

