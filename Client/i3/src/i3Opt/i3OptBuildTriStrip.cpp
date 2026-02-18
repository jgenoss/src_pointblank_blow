#include "i3OptPCH.h"
#include "i3OptBuildTriStrip.h"
#include "i3NvTriStrip.h"
#include "i3OptBuildIndexedGeometry.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/atoi.h"

//	Property Field Name
#define		FIELD_TRISTRIP_AUTO			"AutoSwitching"
#define		FIELD_TRISTRIP_PRIMTYPE		"TargetPrimType"
#define		FIELD_TRISTRIP_STITCHSTRIP	"StitchStrips"
#define		FIELD_TRISTRIP_MINSIZE		"MinStripSize"
#define		FIELD_TRISTRIP_VALIDATE		"ValidateEnabled"


//#define		RESULT_ON

#if defined( RESULT_ON)
static bool	g_bTraceOutput = true;
#else
static bool	g_bTraceOutput = false;
#endif

//I3_CLASS_INSTANCE( i3OptBuildTriStrip, i3SceneOptimizer);
I3_CLASS_INSTANCE( i3OptBuildTriStrip);

i3OptBuildTriStrip::i3OptBuildTriStrip(void)
{
#if !defined( I3_WINDOWS)
	#error "i3OptBuildTriStrip class can be use on the WIN32 platform.";
#endif

	m_Class = CLASS_GEOMETRY;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;

	m_BuildInfo.SetDefault();

	m_Error.Reset();
}

i3OptBuildTriStrip::~i3OptBuildTriStrip(void)
{
	if( m_pTempIndices)
	{
		delete [] m_pTempIndices;		
		m_pTempIndices = nullptr;
	}	

	if( m_pPrimitiveGroups)
	{
		delete [] m_pPrimitiveGroups;	
		m_pPrimitiveGroups = nullptr;
	}
}

void i3OptBuildTriStrip::SetProperty( char * pszFieldName, char * pszValue)
{
	if( i3::generic_is_iequal( pszFieldName, FIELD_TRISTRIP_AUTO) )
	{
		if (i3::generic_is_niequal(pszValue, "T", 1) ||
			i3::generic_is_niequal(pszValue, "Y", 1) )
			SetAutoSwitching( true);
		else if( i3::generic_is_niequal(pszValue, "F", 1) || 
				i3::generic_is_niequal(pszValue, "N", 1) )
			SetAutoSwitching(false);
		else
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);

		return;
	}
	else if( i3::generic_is_iequal( pszFieldName, FIELD_TRISTRIP_PRIMTYPE) ) 
	{
		if( i3::generic_is_iequal( pszValue, "I3G_PRIM_NONE") )
			SetTargetPrimType( I3G_PRIM_NONE);
		else if( i3::generic_is_iequal( pszValue, "I3G_PRIM_POINTLIST") )
			SetTargetPrimType( I3G_PRIM_POINTLIST);
		else if( i3::generic_is_iequal( pszValue, "I3G_PRIM_LINELIST") )
			SetTargetPrimType( I3G_PRIM_LINELIST);
		else if( i3::generic_is_iequal( pszValue, "I3G_PRIM_LINESTRIP") )
			SetTargetPrimType( I3G_PRIM_LINESTRIP);
		else if( i3::generic_is_iequal( pszValue, "I3G_PRIM_TRILIST") )
			SetTargetPrimType( I3G_PRIM_TRILIST);
		else if( i3::generic_is_iequal( pszValue, "I3G_PRIM_TRISTRIP") )
			SetTargetPrimType( I3G_PRIM_TRISTRIP);
		else if( i3::generic_is_iequal( pszValue, "I3G_PRIM_TRIFAN") )
			SetTargetPrimType( I3G_PRIM_TRIFAN);
		else if( i3::generic_is_iequal( pszValue, "I3G_PRIM_POINTSPRITE") )
			SetTargetPrimType( I3G_PRIM_POINTSPRITE);
		else
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);

		return;
	}
	else if( i3::generic_is_iequal( pszFieldName, FIELD_TRISTRIP_STITCHSTRIP) ) 
	{
		if (i3::generic_is_niequal(pszValue, "T", 1) ||
			i3::generic_is_niequal(pszValue, "Y", 1) )
			this->SetStripStitch( true);
		else if (i3::generic_is_niequal(pszValue, "F", 1) ||
				 i3::generic_is_niequal(pszValue, "N", 1) )
			this->SetStripStitch(false);
		else
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);

		return;
	}
	else if( i3::generic_is_iequal( pszFieldName, FIELD_TRISTRIP_MINSIZE) ) 
	{
		UINT32 nVal = (UINT32) i3::atoi( pszValue);
		this->SetStripMinSize( nVal);
	}
	else if( i3::generic_is_iequal( pszFieldName, FIELD_TRISTRIP_VALIDATE) ) 
	{
		if (i3::generic_is_niequal(pszValue, "T", 1) ||
			i3::generic_is_niequal(pszValue, "Y", 1))
			this->SetStripValidateEnabled( true);
		else if (i3::generic_is_niequal(pszValue, "F", 1) ||
			i3::generic_is_niequal(pszValue, "N", 1))
			this->SetStripValidateEnabled(false);
		else
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);

		return;
	}

	I3TRACE("(%s) SetProperty : Unknown field name (%s = %s)\n", 
		static_meta()->class_name(), pszFieldName, pszValue);
}

bool i3OptBuildTriStrip::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( pNode == nullptr)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return true;
	}

	i3ClassMeta * pMeta = pNode->meta();
	I3ASSERT( pMeta != nullptr);

	if( pMeta->kind_of( i3Geometry::static_meta()) ||
		pMeta->kind_of( i3Skin::static_meta()) )
	{
		//I3TRACE( "i3OptBuildTriStrip : OnNode (%s)\n", pMeta->GetClassName());

		Convert( pNode);
	}

	return true;
}

bool i3OptBuildTriStrip::Convert( i3Node * pNode)
{
	if( !pNode)	
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	m_Error.Reset();

	i3OptBuildIndexedGeometry opt;
	bool bNoneIndex = false;

	INT32 i = 0, j = 0;	
	INT32 nGeoAttrCount = 0;
	INT32 nNodeCount = 0;

	i3RenderAttr *		pAttr = nullptr;
	i3GeometryAttr *	pGeoAttr = nullptr;
	i3VertexArray *		pVA = nullptr;

	i3ClassMeta *		pMeta = pNode->meta();

	i3Geometry *		pGeometry = nullptr;
	i3Skin *			pSkin = nullptr;

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
		
			//	i3GeometryAttr 클래스 이면
			if( i3::same_of<i3GeometryAttr*>(pAttr))
			{
				pGeoAttr = (i3GeometryAttr *) pAttr;
				pVA = pGeoAttr->GetVertexArray();
				
				bNoneIndex = false;

				if( pVA != nullptr)
				{
					if( g_bTraceOutput)	I3TRACE("------------------------------------------------------\n");

					//	Auto switching
					if( m_BuildInfo._bAutoSwitching)
					{
						switch( pGeoAttr->GetPrimitiveType())
						{
						case I3G_PRIM_TRISTRIP:		m_BuildInfo._nTargetPrimType = I3G_PRIM_TRILIST;	break;
						case I3G_PRIM_TRILIST:		m_BuildInfo._nTargetPrimType = I3G_PRIM_TRISTRIP;	break;
						default:	break;
						}
					}
									
					switch( m_BuildInfo._nTargetPrimType)
					{
					case I3G_PRIM_TRISTRIP:	
						{							
							if( pGeoAttr->GetPrimitiveType() == I3G_PRIM_TRISTRIP)	//	이미 Strip 이면 캔슬
							{
								if( g_bTraceOutput)	I3TRACE("Convert : Alreay Triangle Strip!! canceled (NAME : %s)\n", (pGeometry) ? pGeometry->GetName() : "NullGeometry");

								SetResult( I3OPT_FAIL_NOT_USE);
								return false;
							}

							if( g_bTraceOutput)	I3TRACE("Convert : To build Triangle Strip (NAME : %s)\n", (pGeometry) ? pGeometry->GetName() : "NullGeometry");
						}
						break;
					case I3G_PRIM_TRILIST:
						{
							if( pGeoAttr->GetPrimitiveType() == I3G_PRIM_TRILIST)	//	이미 List 이면 캔슬
							{
								if( g_bTraceOutput)	I3TRACE("Convert : Alreay Triangle List!! canceled (NAME : %s)\n", (pGeometry) ? pGeometry->GetName() : "NullGeometry");

								SetResult( I3OPT_FAIL_NOT_USE);
								return false;
							}

							if( g_bTraceOutput)	I3TRACE("Convert : To build Triangle List (NAME : %s)\n", (pGeometry) ? pGeometry->GetName() : "NullGeometry");
						}
						break;
					case I3G_PRIM_TRIFAN:
						{
							if( g_bTraceOutput)	I3TRACE("Convert : Not yet support this primitive type (NAME : %s)\n", (pGeometry) ? pGeometry->GetName() : "NullGeometry");
						}
						break;
					default:	
						return false;
						break;
					}

					//	TriStrip은 IndexArray이 있어야 하기 때문에 임의로 IndexArray로 변환한다.
					if( pGeoAttr->GetIndexArray() == nullptr)
					{
						bNoneIndex = true;

						//	IndexArray로 생성
						if( opt.ConvertVertex2Indexed( pGeoAttr) == false)
						{
							SetResult( I3OPT_FAIL_GEN_INDEX);
							I3TRACE( "i3OptBuildTriStrip : Fail to generate index array.\n");
							return false;
						}
					}

					//	List <-> Strip 변환
					if( BuildStrip( pGeoAttr) == false)
					{
						SetResult( I3OPT_FAIL);
						I3TRACE( "i3OptBuildTriStrip : Fail to build triangle strip.\n");
						return false;
					}					

					//	원래 IndexArray가 없었던 Geometry라면 IndexArray를 제거한다.
					if( /*m_BuildInfo._bBuildIndex == false && */bNoneIndex == true)
					{
						//	IndexArray 제거
						if( opt.ConvertIndexed2Vertex( pGeoAttr) == false)
						{
							SetResult( I3OPT_FAIL);
							I3TRACE( "i3OptBuildTriStrip : Fail to unbind index array.\n");
							return false;
						}
					}						
				}
				else
				{
					SetResult( I3OPT_FAIL_NONE_VERTEX);
					return false;

				}	//	end of if pVA != nullptr	
			}	//	End of if(IsExactTypeOf)			
		}	//	End of for (nGeoAttrCount)
	}	//	End of for (nNodeCount)

	return true;
}

bool i3OptBuildTriStrip::BuildStrip( i3GeometryAttr * pGeoAttr)
{
	if( !pGeoAttr) 
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	m_Error.Reset();
	
	UINT32	i = 0;
	bool	ret = false;
	bool	validateEnabled = false;

	unsigned short	numSections = 0;			

	i3VertexArray * pVA = pGeoAttr->GetVertexArray();
	i3IndexArray *	pIA = pGeoAttr->GetIndexArray();
	i3IndexArray *	pNewIA = nullptr;
	
	UINT32			PrimCount = 0;
 
	if( !pVA) 
	{
		SetResult( I3OPT_FAIL_NONE_VERTEX);
		I3TRACE("BuildStrip : warning! Not include vertex array in geometry. We need a VertexArray.\n");
		return false;
	}

	if( !pIA) 
	{
		SetResult( I3OPT_FAIL_NONE_INDEX);
		I3TRACE("BuildStrip : warning! Not include index array in geometry. We need an IndexArray.\n");
		return false;
	}

	if( g_bTraceOutput)	I3TRACE("BuildStrip : Before Primitive count(%d)\n", pGeoAttr->GetPrimitiveCount());
	if( g_bTraceOutput)	I3TRACE("BuildStrip : Before Indices count(%d)\n", pIA->GetCount());

	//	Change primitive type
	switch( m_BuildInfo._nTargetPrimType)
	{		
	case I3G_PRIM_TRISTRIP:				//	Triangle List -> Triangle Strip
		{
			SetListsOnly( false);		//	false면 strip으로 변환

			SetStitchStrips(m_BuildInfo._bStitchStrips);

			//	Setting strips vertex min size
			SetMinStripSize( m_BuildInfo._nMinStripSize);	

			unsigned short *	pIndices = ExtractIndices( pIA);			//	i3IndexArray의 값을 unsigned short 배열로 받아온다.
			unsigned int		numIndices = pIA->GetCount();
									
			if( m_pPrimitiveGroups)
			{
				delete [] m_pPrimitiveGroups;
				m_pPrimitiveGroups = nullptr;
			}
			
			validateEnabled = m_BuildInfo._bValidateEnabled;

			//	Strip 변환
			ret = GenerateStrips( pIndices, numIndices, &m_pPrimitiveGroups, &numSections, validateEnabled);

			if( ret == false)
			{
				SetResult( I3OPT_FAIL);
				I3TRACE("BuildStrip : error! Failed to generate strips.\n");
				return false;
			}
			
			//	변환된 index값으로 새로 i3IndexArray를 생성
			if( MakeIndices( &pNewIA, numSections, m_pPrimitiveGroups) == false)
			{
				SetResult( I3OPT_FAIL);
				I3TRACE("BuildStrip : error! Failed to build Indices\n");
				
				i3::destroy_instance(pNewIA); 

				return false;
			}

			if( pNewIA)
			{
				PrimCount = (UINT32) numSections;
			}
			else
			{
				SetResult( I3OPT_FAIL_NOT_ENOUGH_MEM);
				I3TRACE("BuildStrip : error! Failed to create index array.\n");
				return false;
			}
		}
		break;				
	case I3G_PRIM_TRILIST:				//	Triangle Strip -> Triangle List
		{
			//	Triangle List로 새로 생성된 IndexArray
			if( ConvertIndexStrip2List( &pNewIA, pGeoAttr) == false)
			{
				SetResult( I3OPT_FAIL);
				I3TRACE("BuildStrip : error! Failed to build Indices\n");

				i3::destroy_instance(pNewIA); 
				return false;
			}

			if( pNewIA)
			{
				PrimCount = pNewIA->GetCount() / 3;
			}
			else
			{
				SetResult( I3OPT_FAIL_NOT_ENOUGH_MEM);
				I3TRACE("BuildStrip : error! Failed to create index array.\n");
				return false;
			}

			if( g_bTraceOutput)	I3TRACE("BuildStrip : After Indices count(%d)\n", pNewIA->GetCount());
		}
		break;		
	default:							//	Invalid type
		{
			SetResult( I3OPT_FAIL_NOT_SUPPORT);
			I3TRACE("BuildStrip : warning! Not support primitive type.\n");
			return false;
		}
		break;
	}

	//	Set VertexArray & IndexArray
	pGeoAttr->SetVertexArray( pVA, m_BuildInfo._nTargetPrimType, PrimCount);	
	pGeoAttr->SetIndexArray( pNewIA);	 

	//	Set Primitive Length
	if( numSections > 1)
	{
		UINT32 PrimLength = 0;
		for( i=0; i < (UINT32) numSections; i++)
		{
			PrimLength = m_pPrimitiveGroups[i].numIndices - 2;

			pGeoAttr->SetPrimitiveLength( i, PrimLength );		
		}
	}	

	if( g_bTraceOutput)	I3TRACE( "BuildStrip : After Primitive count(%d)\n", pGeoAttr->GetPrimitiveCount());	
	if( g_bTraceOutput)	I3TRACE( "BuildStrip : Success.\n\n");

	return true;
}
/*
void i3OptBuildTriStrip::RemapIndices( i3GeometryAttr * pGeoAttr)
{
	if( !pGeoAttr) 
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return;
	}

	m_Error.Reset();	
}*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool i3OptBuildTriStrip::MakeIndices( i3IndexArray ** pOutIA, unsigned short numSections, PrimitiveGroup * pPrimGroup)
{
	if( !pPrimGroup) 
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	if( numSections == 0) 
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	UINT32 i = 0, j = 0;
	UINT32 nTotalIndices = 0;

	for( i = 0; i < (UINT32) numSections; i++)
	{
		nTotalIndices += pPrimGroup[i].numIndices;	//	Total Indices count
	}

	if( nTotalIndices == 0)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		I3TRACE("MakeIndices : warning! Converted indices count is zero.\n");
		return false;
	}

	UINT32 cnt = 0;

	if( (*pOutIA) == nullptr)
	{ 
		(*pOutIA) = i3IndexArray::new_object();
	}

	if( (*pOutIA))
	{
		(*pOutIA)->Create( nTotalIndices, 0, I3G_IDX_32BITS);	//	i3IndexArray create
		
		(*pOutIA)->Lock();
		for( i=0; i < numSections; i++)		//	Loof of PrimLength
		{
			for( j=0; j < pPrimGroup[i].numIndices; j++)	//	loof of index count of PrimLength
			{
				(*pOutIA)->SetIndex( cnt++, pPrimGroup[ i].indices[ j]);		//	변환된 index 복사
			}
	
			if( g_bTraceOutput)	I3TRACE("BuildStrip : After Indices count[%d] = (%d)\n", i, pPrimGroup[ i].numIndices);
		}
		(*pOutIA)->Unlock();
	}
	else
	{
		SetResult( I3OPT_FAIL);
		I3TRACE("MakeIndices : error! Failed to generate i3IndexArray.\n");
		return false;
	}

	if( g_bTraceOutput)	I3TRACE("BuildStrip : After Total Indices count = (%d)\n", nTotalIndices);

	return true;
}

bool i3OptBuildTriStrip::ConvertIndexStrip2List( i3IndexArray ** pOutIA, i3GeometryAttr * pGeoAttr)
{
	if( !pGeoAttr) 
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return false;
	}

	UINT32 TriCount = 0;
	UINT32 cnt = 0, vertexNum = 0;
	UINT32 i = 0, j = 0;
	UINT16 val1 = 0, val2 = 0, val3 = 0;	//	i3IndexArray values
	INT32  Idx1 = 0, Idx2 = 0, Idx3 = 0;	//	i3IndexArray index num
	
	UINT32 PrimCount = pGeoAttr->GetPrimitiveCount();
	UINT32 PrimLengh = 0;

	UINT16 * pIndicesBuffer = nullptr;			//	index buffer
	
	i3IndexArray * pSrcIA = pGeoAttr->GetIndexArray();

	if( PrimCount > 1)		//	Primitive Length가 여러개이면
	{
		for( i = 0; i < PrimCount; i++)
		{
			TriCount += pGeoAttr->GetPrimitiveLength( i);
		}			
	}
	else
	{
		TriCount = pSrcIA->GetCount() - 2;
	}

	if( TriCount == 0)
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		I3TRACE("ConvertIndexStrip2List : warning! Triangle count is zero.\n");
		return false;
	}
	
	//	불필요한 index ( Null Stream Vertex) 가 제거된 새로운 값들을 복사할 index 버퍼 생성
	pIndicesBuffer = new UINT16 [ TriCount * 3];
	
	//	index 복사
	pSrcIA->Lock();
	for( i=0; i < PrimCount; i++)
	{
		Idx1 = cnt;		cnt++;
		Idx2 = cnt;		cnt++;
		Idx3 = cnt;		cnt++;

		if( PrimCount == 1)
		{
			PrimLengh = TriCount;
		}
		else
		{
			PrimLengh = pGeoAttr->GetPrimitiveLength( i);
		}

		for( j=0; j < PrimLengh; j++)
		{
			//	Get index value in source i3IndexArray
			val1 = (UINT16) pSrcIA->GetIndex( Idx1);
			val2 = (UINT16) pSrcIA->GetIndex( Idx2);
			val3 = (UINT16) pSrcIA->GetIndex( Idx3);

			if( PrimCount == 1)
			{
				//	3개의 vertex 중 2개이상 값이 같으면 제거한다. (Null stream vertex 이므로)
				if( val1 == val2 || val1 == val3 || val2 == val3)
				{
					//	제거된 인덱스들 확인
					//I3TRACE( "ConvertIndexStrip2List : Degenerate index (%d, %d, %d)\n", Idx1, Idx2, Idx3);	
				}
				else 
				{
					//	Copy indices value in buffer
					pIndicesBuffer[ vertexNum++] = val1;
					pIndicesBuffer[ vertexNum++] = val2;
					pIndicesBuffer[ vertexNum++] = val3;
				}
			}
			else 
			{	
				//	다음 Strip이 계속 연결되어 있다면
				if( j < PrimLengh - 1)
				{
					//	3개의 vertex 중 2개이상 값이 같으면 제거한다. (Null stream vertex 이므로)
					if( val1 == val2 || val1 == val3 || val2 == val3)
					{
						//	제거된 인덱스들 확인
						//I3TRACE( "ConvertIndexStrip2List : Degenerate index (%d, %d, %d)\n", Idx1, Idx2, Idx3);	
					}
					else 
					{
						pIndicesBuffer[ vertexNum++] = val1;
						pIndicesBuffer[ vertexNum++] = val2;
						pIndicesBuffer[ vertexNum++] = val3;
					}
				}
				else 
				{
					pIndicesBuffer[ vertexNum++] = val1;
					pIndicesBuffer[ vertexNum++] = val2;
					pIndicesBuffer[ vertexNum++] = val3;
				}
			}
			
			//	다음 Strip이 계속 연결되어 있다면 swap
			if( j < PrimLengh - 1)
			{
				//	swap index  
				if( (j & 1))
				{
					// 홀수
					Idx1 = Idx3;
					Idx3 = cnt;		cnt++;
				}
				else
				{
					//	짝수
					Idx1 = Idx2;
					Idx2 = cnt;		cnt++;
				}
			}
		}
	}		
	pSrcIA->Unlock();	

	if( (*pOutIA) == nullptr)
	{ 
		(*pOutIA) = i3IndexArray::new_object();
	}

	if( (*pOutIA) && pIndicesBuffer)
	{
		(*pOutIA)->Create( vertexNum, 0, I3G_IDX_32BITS);

		(*pOutIA)->Lock();
		for( i=0; i < vertexNum; i++)
		{
			//	Set Index value in i3IndexArray
			(*pOutIA)->SetIndex( i, pIndicesBuffer[ i]);			
		}
		(*pOutIA) ->Unlock();
	}
	else
	{
		SetResult( I3OPT_FAIL);

		delete [] pIndicesBuffer;

		I3TRACE("ConvertIndexStrip2List : error! Failed to generate i3IndexArray.\n");
		return false;
	}

	if( pIndicesBuffer)
	{
		delete [] pIndicesBuffer;	//	release indices buffer
		pIndicesBuffer = nullptr;
	}

	if( g_bTraceOutput) I3TRACE("ConvertIndexStrip2List : Copied indices (%d) / Ignored indices (%d)\n", vertexNum, (TriCount * 3) - vertexNum);

	return true;
}

unsigned short * i3OptBuildTriStrip::ExtractIndices( i3IndexArray * pIA)
{
	if( !pIA) 
	{
		SetResult( I3OPT_FAIL_INVALID_VAL);
		return nullptr;
	}

	UINT32 i = 0;
	UINT32 count = pIA->GetCount();

	if( m_pTempIndices)
	{
		delete [] m_pTempIndices;
		m_pTempIndices = nullptr;
	}

	m_pTempIndices = new unsigned short[ count];	//	동적 생성 (소멸자 호출시 제거됨)

	if( !m_pTempIndices)
	{
		SetResult( I3OPT_FAIL_NOT_ENOUGH_MEM);
		I3TRACE("GetIndexOrder : Can't alloc new array. Not enough memory.\n");
		I3ASSERT_0; 
	}
	else
	{
		memset( m_pTempIndices, 0, sizeof( unsigned short) * count);
	}
	
	pIA->Lock();
	for( i = 0; i < count; i++)
	{
		//	i3IndexArray의 index 값을 버퍼로 copy
		m_pTempIndices[ i] = (unsigned short) pIA->GetIndex( i);
	}
	pIA->Unlock();

	return m_pTempIndices;	//	버퍼 반환
}
