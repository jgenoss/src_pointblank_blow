#include "i3OptPCH.h"
#include "i3OptBuildProgressiveMesh.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/smart_ptr/make_shared.h"
#include "i3Base/itl/algorithm/sort.h"

I3_CLASS_INSTANCE( i3OptBuildProgressiveMesh);


/////////////////////////////////////////////////////////////////

void ERRINFO::Free(void)
{
	vNew.Free();
}


INT32	i3OptBuildProgressiveMesh::_CalcDimension( i3VertexArray * pVA)
{
	I3ASSERT( pVA != nullptr);

	if( pVA->HasPosition() == false)	return -1;

	INT32 dim = 3;
	if( pVA->GetTextureCoordCount() > 0)	m_uvDim = dim;		dim += 2;

	return dim;
}

//////////////////////////////////////////////////////////////////
i3OptBuildProgressiveMesh::i3OptBuildProgressiveMesh(void)
{
	m_Class = CLASS_GEOMETRY;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

i3OptBuildProgressiveMesh::~i3OptBuildProgressiveMesh(void)
{
	Clear();
}

void i3OptBuildProgressiveMesh::Clear(void)
{
	INT32 i;

	I3_SAFE_RELEASE( m_pVA);
	I3_SAFE_RELEASE( m_pIA);

	m_pVA		= nullptr;
	m_pIA		= nullptr;
	m_PrimCount	= 0;

	if( m_pErr != nullptr)
	{
		for( i = 0;i < m_Vcnt; ++i)
		{
			ERRINFO * pInfo = m_pErr + i;

			pInfo->Free();
		}
	}

	I3MEM_SAFE_FREE( m_pErr);
	I3MEM_SAFE_FREE( m_pPlane);

	m_listErr.clear();
	m_listErrValue.clear();


//	I3_SAFE_RELEASE( m_plistPlane);
//	I3_SAFE_RELEASE( m_plistVSplit);
//	I3_SAFE_RELEASE( m_plistFace);

	m_plistVSplit.reset();
	m_plistFace.reset();

	I3_SAFE_DELETE_ARRAY( m_pQuadric);

/*
	if( m_pQuadric != nullptr)
	{
		for( i = 0;i < m_Vcnt; ++i)
		{
			QUADRIC * pQuad = m_pQuadric + i;

			pQuad->Free();
		}

		I3MEM_SAFE_FREE( m_pQuadric);
	}
*/

	m_Vcnt		= 0;
	m_Icnt		= 0;
}


i3TextureBindAttr * i3OptBuildProgressiveMesh::_findCurTex( i3Node * pNode)
{
	if( i3::kind_of<i3AttrSet* >(pNode))
	{
		i3AttrSet * pAs = (i3AttrSet *)pNode;

		i3RenderAttr * pFind = pAs->FindAttr( i3TextureBindAttr::static_meta());

		if( pFind != nullptr)
		{
			I3ASSERT( i3::same_of<i3TextureBindAttr*>(pFind));

			return (i3TextureBindAttr*)pFind;
		}
	}

	i3Node * pParent = pNode->GetParent();
	if( pParent != nullptr)
	{
		return _findCurTex( pParent);
	}

	return nullptr;
}
		

bool i3OptBuildProgressiveMesh::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( pNode == nullptr)	return true;

	UINT32 prec = i3System::SetFPUPrecision( I3_PC_64);

	i3::vector<i3GeometryAttr*> list;
	i3ClassMeta * pMeta = pNode->meta();
	I3ASSERT( pMeta != nullptr);

	if( pMeta->kind_of( i3Geometry::static_meta()))
	{
		m_pcurTextureBind = _findCurTex( pNode);

		i3Geometry * pGeo = (i3Geometry*)pNode;

		INT32 geomcnt = pGeo->GetGeometryAttrCount();
		for( INT32 i = 0;i < geomcnt; ++i)
		{
			i3GeometryAttr * pGeomAttr = (i3GeometryAttr*)pGeo->GetGeometryAttr( i);

			i3GeometryAttr2 * pNewGeom = i3GeometryAttr2::new_object();

			if( Build( pGeomAttr, pNewGeom) == true)
			{
				list.push_back( pNewGeom);	
			}
			else
			{
				I3_MUST_ADDREF( pGeomAttr);
				list.push_back( pGeomAttr);
				I3_SAFE_RELEASE( pNewGeom);
			}
		}

		pGeo->RemoveAllGeometryAttr();
		const size_t nNew = list.size();
		for( size_t i = 0; i < nNew; ++i)
		{
			i3GeometryAttr * pGeomAttr = list[i];
			pGeo->AppendGeometryAttr(pGeomAttr);
			I3_MUST_RELEASE( pGeomAttr);
		}
	}	

	i3System::SetFPUPrecision((I3_FPU_PRECISION) prec);

	return true;
}

////neighborhoods
INT32 i3OptBuildProgressiveMesh::_getOppositeVert( INT32 vid, i3::vector<INT32>& listOpp)
{
	QUADRIC * pQuad = m_pQuadric + vid;

	I3ASSERT( pQuad->plistEdge != nullptr);

	const i3::vector<ERRINFO*>& vec = pQuad->plistEdge->get_vector();

	for( size_t i = 0;i < vec.size(); ++i)
	{
		ERRINFO * pEdge = vec[i];
//		ERRINFO * pEdge = (ERRINFO*)pQuad->plistEdge->GetItem(i);

		if( pEdge->min == vid)
		{
			listOpp.push_back(pEdge->max);
		}
		else if( pEdge->max == vid)
		{
			listOpp.push_back(pEdge->min);
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL, "이상한 edge");
		}
	}

	return INT32(listOpp.size());
}

INT32 i3OptBuildProgressiveMesh::_addEdge( INT32 vid, ERRINFO * pInfo)
{
	QUADRIC * pQuad = m_pQuadric + vid;

	I3ASSERT( pQuad->plistEdge != nullptr);

	i3::vector_multiset<ERRINFO*, Neighbor_ERRINFO_Comp>::iterator it =	pQuad->plistEdge->find(pInfo);
	if ( it == pQuad->plistEdge->end() )
	{
		pQuad->plistEdge->insert(pInfo);		
	}

//	INT32 id = pQuad->plistEdge->FindItem( pInfo);
//	if( id == -1)
//	{
//		pQuad->plistEdge->Add( (void*)pInfo);
//	}

	return -1;
}

INT32 i3OptBuildProgressiveMesh::_removeEdge( INT32 vid, INT32 viDel)
{
	i3::vector_multiset<ERRINFO*, Neighbor_ERRINFO_Comp>* plist = _getEdge(vid);
	const i3::vector<ERRINFO*>& vec = plist->get_vector();

	INT32 cnt =0;
	for( size_t i = 0;i < vec.size(); ++i)
	{
		ERRINFO * pInfo = vec[i];
		if((pInfo->min == vid) && (pInfo->max == viDel))
		{
		//	plist->Delete(i);
			plist->erase( plist->begin() + i );
			i--;
			cnt++;
		}
		else if( pInfo->max == vid && pInfo->min == viDel)
		{
		//	plist->Delete(i);
			plist->erase(plist->begin() + i);
			i--;
			cnt++;
		}
	}

	I3ASSERT( cnt > 0);

	return plist->size();
}

i3::vector_multiset<ERRINFO*, Neighbor_ERRINFO_Comp>* i3OptBuildProgressiveMesh::_getEdge( INT32 vid)
{
	QUADRIC * pQuad = m_pQuadric + vid;

	I3ASSERT( pQuad->plistEdge != nullptr);
	
	return pQuad->plistEdge.get();
}

INT32 i3OptBuildProgressiveMesh::_addNeighbor( INT32 vid, INT32 fid)
{
	QUADRIC * pQuad = m_pQuadric + vid;

	I3ASSERT( pQuad->plistNeighbor != nullptr);
	size_t idx = pQuad->plistNeighbor->size();
	pQuad->plistNeighbor->push_back(fid);
	return idx;
}

i3::vector<INT32>* i3OptBuildProgressiveMesh::_getNeighbor( INT32 vid)
{
	QUADRIC * pQuad = m_pQuadric + vid;

	I3ASSERT( pQuad->plistNeighbor != nullptr);
	
	return pQuad->plistNeighbor.get();
}

void i3OptBuildProgressiveMesh::_setValidVertex( INT32 vid, bool bValid)
{
	QUADRIC * pQuad = m_pQuadric + vid;
	
	pQuad->_isValid = bValid;
}

bool i3OptBuildProgressiveMesh::_isValidVertex( INT32 vid)
{
	QUADRIC * pQuad = m_pQuadric + vid;
	
	return pQuad->_isValid;
}

INT32 i3OptBuildProgressiveMesh::_getVMarked( INT32 vid)
{
	QUADRIC * pQuad = m_pQuadric + vid;
	
	return pQuad->_mark;
}

INT32 i3OptBuildProgressiveMesh::_getFMarked( INT32 fid)
{
	I_FACE * pFace = m_pPlane + fid;

	return pFace->mark;
}

void i3OptBuildProgressiveMesh::_markVert( INT32 vid, INT32 mark)
{
	QUADRIC * pQuad = m_pQuadric + vid;
	
	pQuad->_mark = mark;
}

void i3OptBuildProgressiveMesh::_markface( INT32 fid, INT32 mark)
{
	I_FACE * pFace = m_pPlane + fid;

	pFace->mark = mark;
}

void i3OptBuildProgressiveMesh::_markCorners( INT32 vid, INT32 mark)
{
	INT32 j;

	i3::vector<INT32> * pNeighbor = _getNeighbor( vid);

	for( size_t i = 0;i < pNeighbor->size(); ++i)
	{
		INT32 fid = (*pNeighbor)[i];

		I_FACE * pFace = m_pPlane + fid;

		for( j = 0;j < 3; ++j)
		{
			_markVert( pFace->id[j], mark);
		}
	}
}

void i3OptBuildProgressiveMesh::_markNeighbors( INT32 vid, INT32 mark)
{
	i3::vector<INT32> * pNeighbor = _getNeighbor( vid);

	for(size_t i = 0;i < pNeighbor->size(); ++i)
	{
		INT32 fid = (*pNeighbor)[i];

		_markface( fid, mark);
	}
}

void i3OptBuildProgressiveMesh::_markNeighbors_delta( INT32 vid, INT32 markDelta)
{
	i3::vector<INT32> * pNeighbor = _getNeighbor( vid);

	for(size_t i = 0;i < pNeighbor->size(); ++i)
	{
		INT32 fid = (*pNeighbor)[i];

		_markface( fid, _getFMarked(fid) + markDelta);
	}
}

void i3OptBuildProgressiveMesh::_collect_Unmarked_corners( INT32 vid, i3::vector<INT32>& VertList)
{
	INT32 j;
	i3::vector<INT32> * pNeighbor = _getNeighbor( vid);

	for(size_t i = 0; i < pNeighbor->size(); ++i)
	{
		INT32 fid = (*pNeighbor)[i];

		I_FACE * pFace = m_pPlane + fid;

		for( j = 0;j < 3; ++j)
		{
			if( _getVMarked( pFace->id[j]) == 0)
			{
				VertList.push_back(pFace->id[j]);

				_markVert( pFace->id[j], 1);
			}
		}
	}
}

void i3OptBuildProgressiveMesh::_collect_Unmarked_neighbors( INT32 vid, i3::vector<INT32>& FaceList)
{

	i3::vector<INT32> * pNeighbor = _getNeighbor( vid);

	for(size_t i = 0;i < pNeighbor->size(); ++i)
	{
		INT32 fid = (*pNeighbor)[i];

		if( _getFMarked( fid) == 0)
		{
			FaceList.push_back(fid);
		}
	}
}

void i3OptBuildProgressiveMesh::_collect_vertx_star( INT32 vid, i3::vector<INT32>& Star)
{
	_markCorners( vid, 0);
	_markVert( vid, 1);
	_collect_Unmarked_corners( vid, Star);
}

void i3OptBuildProgressiveMesh::_collect_edge_neighbor( INT32 vid1, INT32 vid2, i3::vector<INT32>& FaceList)
{
	_markNeighbors( vid1, 1);
	_markNeighbors( vid2, 0);
	_collect_Unmarked_neighbors( vid1, FaceList);
}

i3NVector* i3OptBuildProgressiveMesh::_getVector( INT32 vid)
{
	return &((m_pQuadric+vid)->_NVec);
}

void i3OptBuildProgressiveMesh::_discontinuity_constraint(INT32 vid1, INT32 vid2, i3::vector<INT32>& FaceList)
{
	double M;

	I_FACE plane, *pFace;
	i3NVector	vc(m_CurDim), vDiff( m_CurDim), vNormal(m_CurDim);

	i3NVector * nv0 = _getVector( vid1);
	i3NVector * nv1 = _getVector( vid2);

	vDiff = (*nv1);
	vDiff -= (*nv0);
	
	double reg = vDiff.lengthsq3();
	//vDiff.normalize3();

	for(size_t i = 0; i < FaceList.size(); ++i)
	{
		INT32 fid = FaceList[i];

		pFace = m_pPlane + fid;

		plane.id[0] = pFace->id[0];
		plane.id[1] = pFace->id[1];
		plane.id[2] = pFace->id[2];
		
		_CalcPlaneNormal( &plane, &vNormal);

		vNormal.normalize3();
		vc = vDiff^vNormal;
		vc.normalize3();

		//face region
		plane.region = reg;

		//triangle plane
		M = - (vc.dot3((*nv0)));

		plane.plane[0] = vc[0];
		plane.plane[1] = vc[1];
		plane.plane[2] = vc[2];
		plane.plane[3] = M;

		i3Quadric3 q3( &plane);
		q3 *= 1000.0;

		q3.SetRegion( reg);
		q3 *= reg;
		
		(m_pQuadric+vid1)->_Quad += q3;
		(m_pQuadric+vid2)->_Quad += q3;
	}
}

void i3OptBuildProgressiveMesh::_constrain_Boundaries()
{
	i3::vector<INT32> vert;
	i3::vector<INT32> face;
	INT32 i;
	for(i = 0; i< m_Vcnt; ++i)
	{
		vert.clear();
		_collect_vertx_star( i, vert);

		for(size_t j =0; j < vert.size(); ++j)
		{
			INT32 vid = vert[j];
			if( i < vid)
			{
				face.clear();
				_collect_edge_neighbor( i, vid, face);
				if( face.size() == 1)
				{
					_discontinuity_constraint(i, vid, face);
				}
			}
		}

		if( _isClosedVertex( i) == false)
		{
			(m_pQuadric+i)->_Quad *= 100.0;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
INT32 i3OptBuildProgressiveMesh::BuildProgressive( INT32 nGoalFaceCount)
{
	m_Vcnt = m_pVA->GetCount();
	m_Icnt = m_PrimCount;

	if( m_Icnt < nGoalFaceCount)
		return m_Icnt;

	if( m_Icnt > 2)
	{
		nGoalFaceCount = MAX( 2, nGoalFaceCount);
	}
	else
	{
		return m_Icnt;
	}

	m_pPlane	= (I_FACE*)i3MemAlloc( sizeof( I_FACE) * m_Icnt);

	_WeldVertices();
	CountValidVertex();

	m_CurDim = _CalcDimension( m_pVA);

	I3TRACE("calc dimension: %d\n", m_CurDim);
	I3TRACE("Creating Quadrics..\n");

	_CreateQuadrics();

	m_pErr		= (ERRINFO*)i3MemAlloc( sizeof( ERRINFO) * (m_Icnt*3 + m_Vcnt));
	i3mem::FillZero( m_pErr, sizeof( ERRINFO) * (m_Icnt*3 + m_Vcnt));

//	m_plistErr		= BinList::new_object();
//	m_plistErrValue = BinList::new_object();
	m_listErr.clear();	m_listErrValue.clear();
//	m_plistErrValue->SetCompareProc((COMPAREPROC)_AllocCompareProc);			// 이 코드에서는 비교함수가 두종류라서 잘 구별해야함.(2012.05.21.수빈)


	m_plistVSplit	= i3::make_shared<i3::vector<I3GEOM_VSPLIT*> >();		
	m_plistFace		= i3::make_pscoped<i3::vector<I_FACE*> >();				
	m_plistFace->reserve(m_Icnt);											//	m_plistFace->SetAllocationUnit( m_Icnt);

	m_pVA->Lock();
	m_pIA->Lock();

	I3TRACE("Calculate planes..\n");
	_CalcPlane( m_pPlane, m_Icnt);
	_constrain_Boundaries();

	I3TRACE("Selecting pairs..\n");
	_selectPair( 0.1f);

	m_listErrValue.insert( m_listErr.begin(), m_listErr.end() );
//	m_plistErrValue->AddFromList( m_plistErr);

	I3TRACE("start contract pairs..\n");
	INT32 nCur = Construct_N_Contract( nGoalFaceCount);
	I3TRACE("done contract : %d remains\n", nCur);

	m_pVA->Unlock();
	m_pIA->Unlock();

	return nCur;
}

void  i3OptBuildProgressiveMesh::MakeVertexColor( i3GeometryAttr * pGeom)
{
	i3VertexArray * pVa = pGeom->GetVertexArray();
	
	if( pVa->HasColor())		return;
	if( m_pcurTextureBind == nullptr)	return;
	i3Texture * pTex = m_pcurTextureBind->GetTexture();
	if( pTex == nullptr)	return;

	i3VertexFormat format;
	i3VertexFormat *origin = pVa->GetFormat();

	format.SetFormat( origin->GetFormat());
	format.SetHasColor();

	i3VertexArray * pNewVa = i3VertexArray::new_object();
	pNewVa->Create( &format, pVa->GetCount(), pVa->GetUsageFlag());

	INT32 i;
	pTex->Lock(0);
	pNewVa->Lock();
	pVa->Lock();
	for( UINT32 idxSrc = 0;idxSrc < pVa->GetCount(); ++idxSrc)
	{
		if( pVa->HasPosition())
		{
			VEC3D	vPos;

			pVa->GetPosition( idxSrc, &vPos);
			pNewVa->SetPosition( idxSrc, &vPos);
		}

		if( pVa->HasNormal())
		{
			VEC3D	vNormal;

			pVa->GetNormal( idxSrc, &vNormal);
			pNewVa->SetNormal( idxSrc, &vNormal);
		}

		if( pVa->HasColor())
		{
			COLORREAL	Col;

			pVa->GetColor( idxSrc, &Col);
			pNewVa->SetColor( idxSrc, &Col);
		}

		if( pVa->HasTangent())
		{
			VEC3D	vTangent;

			pVa->GetTangent( idxSrc, &vTangent);
			pNewVa->SetTangent( idxSrc, &vTangent);
		}

		if( pVa->HasBinormal())
		{
			VEC3D	vBiNormal;

			pVa->GetBinormal( idxSrc, &vBiNormal);
			pNewVa->SetBinormal( idxSrc, &vBiNormal);
		}

		for( i = 0; i < pVa->GetFormat()->GetTextureCoordSetCount(); i++)
		{
			VEC2D pCoord;

			pVa->GetTextureCoord( i, idxSrc, &pCoord);
			pNewVa->SetTextureCoord( i, idxSrc, &pCoord);

			INT32 w = (INT32) (MINMAX( 0, (pCoord.x+1.0f) * (pTex->GetWidth()/2), pTex->GetWidth() - 1));
			INT32 h = (INT32) (MINMAX( 0, (pCoord.y+1.0f) * (pTex->GetHeight()/2), pTex->GetHeight() - 1));
			{
				COLORREAL	Col;
				pTex->GetPixel( w, h, &Col);
				pNewVa->SetColor( idxSrc, &Col);
			}
		}

		for( i = 0; i < pVa->GetFormat()->GetBlendIndexCount(); i++)
		{
			UINT32 BlendIndex = pVa->GetBlendIndex( i, idxSrc);

			pNewVa->SetBlendIndex( i, idxSrc, BlendIndex);
		}

		for( i = 0; i < pVa->GetFormat()->GetBlendWeightCount(); i++)
		{
			REAL32 BlendWeight = pVa->GetBlendWeight( i, idxSrc);

			pNewVa->SetBlendWeight( i, idxSrc, BlendWeight);
		}
	}

	pTex->Unlock( 0);
	pNewVa->Unlock();
	pVa->Unlock();

	pGeom->SetVertexArray( pNewVa);
}

bool i3OptBuildProgressiveMesh::Build( i3GeometryAttr * pGeoAttr, i3GeometryAttr2 * pGeoAttr2)
{
	I3TRACE("================================================================\n");
	I3TRACE("Starting Optimizing\n");
	Clear();

	if( pGeoAttr == nullptr || pGeoAttr2 == nullptr)
		return false;

	I3_REF_CHANGE( m_pVA, pGeoAttr->GetVertexArray());
	I3_REF_CHANGE( m_pIA, pGeoAttr->GetIndexArray());
	I3G_PRIMITIVE primtype	= pGeoAttr->GetPrimitiveType();

	if( m_pVA == nullptr || m_pIA == nullptr || primtype != I3G_PRIM_TRILIST)
		return false;
	
	m_PrimCount = pGeoAttr->GetPrimitiveCount();

	INT32 cnt = BuildProgressive( 6);
	if( m_PrimCount == cnt)
		return false;
	else
	{
		_DumpData( pGeoAttr2);
		return true;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// func
/////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 i3OptBuildProgressiveMesh::_addError( ERRINFO * pErr, bool bCalcErr, bool bAddEdge)
{
	//겹치는 Edge는 허용하지 않는다.
	//만..현재 제공되는 edge는 겹치지 못한다.
	INT32 idx;
	{
		if( bAddEdge)
		{
			_addEdge( pErr->min, pErr);
			_addEdge( pErr->max, pErr);
		}

		if( bCalcErr)
		{
			pErr->err = _calcErr( m_pQuadric, pErr);
			pErr->key = pErr->min;
		}

		i3::vector_multiset< ERRINFO* >::iterator it = m_listErr.insert( pErr);
		idx = (INT32)i3::distance(m_listErr.begin(), it);
		return idx;
	}
}

INT32 i3OptBuildProgressiveMesh::_removeErr( ERRINFO * pErr)
{
	i3::pair<i3::vector_multiset<ERRINFO*>::iterator, i3::vector_multiset<ERRINFO*>::iterator> pr = m_listErr.equal_range(pErr);
	
	i3::vector_multiset<ERRINFO*>::iterator it = i3::find(pr.first, pr.second, pErr);
	
	if (it == pr.second) return -1;

	INT32 dist = (INT32)i3::distance(m_listErr.begin(), it);
	m_listErr.erase(it);
	return dist;

//	return m_plistErr->Remove( pErr);
}

INT32 i3OptBuildProgressiveMesh::_removeErr( INT32 idx)
{
	if (m_listErr.empty() ) return -1;
	
	m_listErr.erase(m_listErr.begin() + idx);

	return m_listErr.size();
//	return m_plistErr->Delete( idx);
}

void i3OptBuildProgressiveMesh::_CalcPlaneNormal( I_FACE * pFace, i3NVector * pvNormal)
{
	i3NVector va(m_CurDim), vb(m_CurDim);
	i3NVector *v0, *v1, *v2;

	v0 = _getVector( pFace->id[0]);
	v1 = _getVector( pFace->id[1]);
	v2 = _getVector( pFace->id[2]);

	va = (*v1);	va -= (*v0);
	vb = (*v2);	vb -= (*v0);

	*pvNormal = va^vb;
}

INT32 i3OptBuildProgressiveMesh::_CalcPlane( I_FACE * pvPlane, INT32 ibCnt)
{
	INT32 i;
	i3NVector vc(m_CurDim);
	
	m_plistFace->clear();
	for( i = 0;i < ibCnt; i++)
	{
		I_FACE * pFace = pvPlane + i;

		_CalcPlaneNormal( pFace, &vc);

		//face region
		pFace->region = 0.5 * vc.length3();

		//I3ASSERT( pFace->region > 0.0f);

		i3NVector * pv1 = _getVector( pFace->id[0]);
		i3NVector * pv2 = _getVector( pFace->id[1]);
		i3NVector * pv3 = _getVector( pFace->id[2]);

		i3Quadric Q( pv1, pv2, pv3, pFace->region);
		Q *= pFace->region;

		for( INT32 k = 0;k < 3; ++k)
		{
			QUADRIC * pQuad = (m_pQuadric+pFace->id[k]);

			pQuad->_Quad += Q;
		}

		INT32 idx = m_plistFace->size();
		m_plistFace->push_back( pFace);
		I3ASSERT( idx == i);
		
		_addNeighbor( pFace->id[0], i);
		_addNeighbor( pFace->id[1], i);
		_addNeighbor( pFace->id[2], i);
		//I3TRACE("%d: %d, %d, %d\n", i, pFace->id[0], pFace->id[1], pFace->id[2]);
	}

	return ibCnt;
}


void i3OptBuildProgressiveMesh::_CreateQuadrics(void)
{
	INT32 i;

	bool buv	= false;
	//bool bnorm	= false;

	VEC3D pos;
	VEC2D uv;

	//if( m_pVA->HasNormal())	bnorm = true;
	if( m_pVA->GetTextureCoordCount() > 0) buv = true;
	
	m_pQuadric = new QUADRIC[m_Vcnt];
//	m_pQuadric	= (QUADRIC*)i3MemAlloc( sizeof( QUADRIC) * m_Vcnt);
//	i3mem::FillZero( m_pQuadric, sizeof( QUADRIC) * m_Vcnt);

	m_pVA->Lock();
	for( i = 0; i < m_Vcnt; ++i)
	{
		QUADRIC		* pQuad		= m_pQuadric + i;
		pQuad->_Quad.Alloc( m_CurDim);

		{
			i3NVector	* pVec = &pQuad->_NVec;
			pVec->Alloc( m_CurDim);

			m_pVA->GetPosition( i, &pos);
			_setVertex( i, &pos);

			/*
			if( bnorm)
			{
				m_pVA->GetNormal( i, &normal);
				_setNorm( i, &normal);
			}
			*/

			if( buv)
			{
				m_pVA->GetTextureCoord( 0, i, &uv);
				_setUV( i, &uv);
			}
		}

		pQuad->_isValid		= true;
		pQuad->plistEdge = i3::make_pscoped<i3::vector_multiset<ERRINFO*, Neighbor_ERRINFO_Comp> >();  
//		pQuad->plistEdge->SetCompareProc((COMPAREPROC)_AllocFindProc);
		pQuad->plistNeighbor = i3::make_pscoped<i3::vector<INT32> >(); 
	}
	m_pVA->Unlock();
}

INT32 i3OptBuildProgressiveMesh::_InitQuadrics( QUADRIC * pQuadric, I_FACE * pPlane)
{
	for( INT32 i = 0; i < m_Vcnt; ++i)
	{
		QUADRIC		* pQuad		= m_pQuadric + i;
		if( pQuad->_Quad.getReg() <= 0.0)
		{
			I3TRACE("%d : rage0\n", i);
			for( INT32 k = 0;k < m_Icnt/3; k++)
			{
				I_FACE * pFace = m_pPlane + k;
				for( INT32 j = 0;j< 3; ++j)
				{
					if( pFace->id[j] == i)
					{
						I3TRACE("error\n");
					}
				}
			}
		}
	}

	return m_Icnt;
}

double i3OptBuildProgressiveMesh::_getRegion( INT32 vid)
{
	return (m_pQuadric+vid)->_Quad.getReg();
}

void i3OptBuildProgressiveMesh::_getVertex( INT32 vid, VEC3D *pv)
{
	_getVertex( &(m_pQuadric+vid)->_NVec, pv);
}

void i3OptBuildProgressiveMesh::_getVertex( i3NVector * v, VEC3D *pv)
{
	i3Vector::Set( pv, (REAL32)(*v)[0], (REAL32)(*v)[1], (REAL32)(*v)[2]);
}

double * i3OptBuildProgressiveMesh::_getVertex( i3NVector * v)
{
	return v->get(0);
}

double * i3OptBuildProgressiveMesh::_getVertex( INT32 vid)
{
	return (m_pQuadric+vid)->_NVec.get(0);
}

void i3OptBuildProgressiveMesh::_setVertex( INT32 vid, VEC3D *pv)
{
	(m_pQuadric+vid)->_NVec[0] = (double)pv->x;
	(m_pQuadric+vid)->_NVec[1] = (double)pv->y;
	(m_pQuadric+vid)->_NVec[2] = (double)pv->z;
}

void i3OptBuildProgressiveMesh::_getUV( INT32 vid, VEC2D *pv)
{
	I3ASSERT( m_uvDim > 0);

	_getUV( &(m_pQuadric+vid)->_NVec, pv);
}

void i3OptBuildProgressiveMesh::_getUV( i3NVector *uv, VEC2D *pv)
{
	I3ASSERT( m_uvDim > 0);

	i3Vector::Set( pv, (REAL32)(*uv)[m_uvDim], (REAL32)(*uv)[m_uvDim+1]);
}

double * i3OptBuildProgressiveMesh::_getUV( i3NVector * v)
{
	I3ASSERT( m_uvDim > 0);

	return v->get(m_uvDim);
}

double * i3OptBuildProgressiveMesh::_getUV( INT32 vid)
{
	I3ASSERT( m_uvDim > 0);
	return (m_pQuadric+vid)->_NVec.get(m_uvDim);
}

void i3OptBuildProgressiveMesh::_setUV( INT32 vid, VEC2D *pv)
{
	I3ASSERT( m_uvDim > 0);

	(m_pQuadric+vid)->_NVec[m_uvDim] = (double)pv->x;
	(m_pQuadric+vid)->_NVec[m_uvDim+1] = (double)pv->y;
}

void i3OptBuildProgressiveMesh::_getNorm( INT32 vid, VEC3D *pv)
{
	I3ASSERT( m_normDim > 0);

	_getNorm( &(m_pQuadric+vid)->_NVec, pv);
}

void i3OptBuildProgressiveMesh::_getNorm( i3NVector * pv, VEC3D *pn)
{
	I3ASSERT( m_normDim > 0);

	i3Vector::Set( pn, (REAL32)(*pv)[m_normDim], (REAL32)(*pv)[m_normDim+1], (REAL32)(*pv)[m_normDim+2]);
}

double * i3OptBuildProgressiveMesh::_getNorm( INT32 vid)
{
	I3ASSERT( m_normDim > 0);
	return (m_pQuadric+vid)->_NVec.get(m_normDim);
}

double * i3OptBuildProgressiveMesh::_getNorm( i3NVector * v)
{
	I3ASSERT( m_normDim > 0);
	return v->get(m_normDim);
}

void i3OptBuildProgressiveMesh::_setNorm( INT32 vid, VEC3D *pv)
{
	I3ASSERT( m_normDim > 0);

	(m_pQuadric+vid)->_NVec[m_normDim] = (double)pv->x;
	(m_pQuadric+vid)->_NVec[m_normDim+1] = (double)pv->y;
	(m_pQuadric+vid)->_NVec[m_normDim+2] = (double)pv->z;
}

double i3OptBuildProgressiveMesh::_calcErr( QUADRIC * pQuad, ERRINFO * pInfo)
{
	double min_err;

	INT32 id1			= pInfo->min;
	INT32 id2			= pInfo->max;
	i3NVector * pvErr	= &pInfo->vNew;

	QUADRIC *pQuad1 = (pQuad+id1);
	QUADRIC *pQuad2 = (pQuad+id2);
	
	i3Quadric Quad;
	Quad.Alloc( pvErr->getDim());

	Quad = pQuad1->_Quad;
	Quad += pQuad2->_Quad;
	
	if( Quad.findOptimalVertPos( pvErr))
	{
	}
	else
	{
		double error1 = Quad.GetError( &pQuad1->_NVec);
		double error2 = Quad.GetError( &pQuad2->_NVec);

		min_err = MIN(fabs(error1 ) ,fabs(error2));
		if (error1 == min_err) 
		{
			*pvErr = pQuad1->_NVec;
		}
		else if( error2 == min_err)
		{
			*pvErr = pQuad2->_NVec;
		}
	}

	min_err = - Quad.GetError(pvErr);

	return min_err;
}


void i3OptBuildProgressiveMesh::_selectPair( double Threshold)
{
	i3::vector<INT32> listVertexStar;
	INT32 nErrCnt = 0;
	ERRINFO* pInfo;

	for( INT32 i=0;i < m_Vcnt;++i)
	{
		listVertexStar.clear();

		_collect_vertx_star( i, listVertexStar);
		for( size_t j=0; j< listVertexStar.size();j++)
		{
			INT32 vid= listVertexStar[j];
			if( i < vid)
			{
				pInfo = m_pErr + nErrCnt;

				pInfo->min = i;
				pInfo->max = vid;
				pInfo->vNew.Alloc( m_CurDim);

				_addError( pInfo, true, true);

				nErrCnt++;
			}
		}
	}
}

void i3OptBuildProgressiveMesh::_partition_marked_neighbors(INT32 vid, INT32 pivot, i3::vector<INT32>& lChange, i3::vector<INT32>& lDelete)
{
	i3::vector<INT32> * plist = _getNeighbor( vid);

	for(size_t i = 0;i < plist->size(); ++i)
	{
		INT32 fid = (*plist)[i];
		INT32 mark = _getFMarked( fid);

		if( mark > 0)
		{
			if( mark < pivot)	lChange.push_back(fid);
			else lDelete.push_back(fid);

			_markface( fid, 0);
		}
	}
}

void	i3OptBuildProgressiveMesh::_calcContract( ERRINFO * pMinErr, i3::vector<INT32>& lChange, i3::vector<INT32>& lDelete)
{
	INT32 key = pMinErr->key;
	INT32 other = (key == pMinErr->min) ? pMinErr->max : pMinErr->min;
	I3ASSERT( key == pMinErr->min || key == pMinErr->max);

	_markNeighbors( other, 0);
	_markNeighbors( key, 1);
	_markNeighbors_delta( other, 1);

	_partition_marked_neighbors(key, 2, lChange, lDelete);
	pMinErr->deltaPivot = INT32(lChange.size());
	_partition_marked_neighbors(other, 2, lChange, lDelete);
}

void i3OptBuildProgressiveMesh::_moveEdgeContract( ERRINFO * pMinErr)
{
	i3::vector<INT32> opp;
	i3::vector_multiset<ERRINFO*, Neighbor_ERRINFO_Comp>* lmax;
	INT32 key = pMinErr->key;
	INT32 other = ( key == pMinErr->min) ? pMinErr->max : pMinErr->min;

	_getOppositeVert( key, opp);
	lmax = _getEdge( other);

	const i3::vector<ERRINFO*>& vec = lmax->get_vector();

	for( size_t i = 0;i < vec.size(); ++i)
	{
		ERRINFO * pinfo = vec[i]; 

		INT32 id = -1;
		if( other == pinfo->min)	id = pinfo->max;
		else						id = pinfo->min;							

		I3ASSERT( id != -1);

		size_t id2 = INT32(i3::vu::index_of(opp, id));	//opp.Find((void*)id);

		if( id == key || id2 != opp.size() )	//현재 EDGE이거나 max와 연결된
		{
			_removeEdge( id, other);
			
//			lmax->Delete(i);
			lmax->erase(lmax->begin() + i);
			i--;

			if( id != key)
			{
				i3::pair<i3::vector_multiset<ERRINFO*, ErrValue_ERRINFO_Comp>::iterator, 
					i3::vector_multiset<ERRINFO*, ErrValue_ERRINFO_Comp>::iterator> pr = m_listErrValue.equal_range(pinfo);

				i3::vector_multiset<ERRINFO*, ErrValue_ERRINFO_Comp>::iterator it = i3::find(pr.first, pr.second, pinfo);

				if (it != pr.second) 
				{
					m_listErrValue.erase(it);
				}				

//				m_plistErrValue->Remove(pinfo);

			}
		}
		else
		{
			pinfo->min = MIN(id, key);
			pinfo->max = MAX(id, key);

			_addEdge( key, pinfo);
		}
	}

	lmax->clear();
}

void i3OptBuildProgressiveMesh::_applyContract( ERRINFO * pMinErr, i3::vector<INT32>& listChange, i3::vector<INT32>& listDelete, I3GEOM_VSPLIT * pVSplit)
{
	INT32 key = pMinErr->key;
	INT32 other = (key == pMinErr->min) ? pMinErr->max : pMinErr->min;
	I3ASSERT( key == pMinErr->min || key == pMinErr->max);

	//EDGE의 추가 제거시 함께 계산하기위해 미리 더해버린다.
	(m_pQuadric+key)->_NVec = pMinErr->vNew;
	(m_pQuadric+key)->_Quad += (m_pQuadric+other)->_Quad;

	pVSplit->nFaceCount = INT32(listDelete.size()); 
	i3::sort(listDelete.begin(), listDelete.end());		// 정수값의 오름차순이므로 비교함수가 없어도 동작됨..
//	plistDelete->Sort( (COMPAREPROC)_AllocFaceCompProc);

	if( pVSplit->nFaceCount > 0)
	{
		pVSplit->iFaceID = (INT32*)i3MemAlloc( sizeof( INT32) * pVSplit->nFaceCount*7);

		for(INT32 i = 0 ; i < pVSplit->nFaceCount; i++)
		{
			INT32 fid = listDelete[i];

			I_FACE * pFace  = m_pPlane + fid;

			INT32 idx = INT32(i3::vu::index_of( *m_plistFace, pFace));	// m_plistFace->Find( pFace);
			I3ASSERT( idx != INT32(m_plistFace->size()) );

			*(pVSplit->iFaceID + i*4) = idx;
			*(pVSplit->iFaceID + i*4+1) = pFace->originid[0];
			*(pVSplit->iFaceID + i*4+2) = pFace->originid[1];
			*(pVSplit->iFaceID + i*4+3) = pFace->originid[2];
			*(pVSplit->iFaceID + i*4+4) = pFace->id[0];
			*(pVSplit->iFaceID + i*4+5) = pFace->id[1];
			*(pVSplit->iFaceID + i*4+6) = pFace->id[2];
		}
	}

	i3::vector<INT32>* plistKeyNeighbor = _getNeighbor( key);
	pVSplit->nNewCount = plistKeyNeighbor->size();
	if( pVSplit->nNewCount > 0)
	{
		pVSplit->iNewID = (UINT32*)i3MemAlloc( sizeof(INT32)*pVSplit->nNewCount);
		for(INT32 i = 0;i < pVSplit->nNewCount;++i)
		{
			INT32 fid = (*plistKeyNeighbor)[i];

			I_FACE * pFace  = m_pPlane + fid;

			INT32 idx = INT32(i3::vu::index_of(*m_plistFace, pFace));   // m_plistFace->Find( pFace);
			I3ASSERT( idx != INT32(m_plistFace->size()) );

			for( INT32 j = 0; j < 3; ++j)
			{
				if( pFace->id[j] == key)
				{
					*(pVSplit->iNewID+ i) = pFace->originid[j];
				}
			}
		}
	}

	pVSplit->nSplitCount = INT32(listChange.size()) - pMinErr->deltaPivot;

	if( pVSplit->nSplitCount > 0)
	{
		pVSplit->iIndicId = (INT32*)i3MemAlloc( sizeof( INT32) * pVSplit->nSplitCount);
		pVSplit->iOldID = (UINT32*)i3MemAlloc( sizeof( INT32) * pVSplit->nSplitCount);

		for(INT32 i = pMinErr->deltaPivot ; i < INT32(listChange.size()); i++)
		{
			INT32 fid = listChange[i];

			I_FACE * pFace  = m_pPlane + fid;

			INT32 idx =  INT32(i3::vu::index_of(*m_plistFace, pFace));   // m_plistFace->Find( pFace); 
			I3ASSERT( idx != INT32(m_plistFace->size()) );

			for( INT32 j = 0; j < 3; ++j)
			{
				if( pFace->id[j] == other)
				{
					*(pVSplit->iIndicId + (i - pMinErr->deltaPivot)) = idx*3 + j;
					*(pVSplit->iOldID + (i - pMinErr->deltaPivot)) = pFace->originid[j];

					if( pFace->id[j] == other)
					{
						pFace->originid[j] = pFace->id[j];
						pFace->id[j] =  key;
					}
				}
			}

			_addNeighbor( key, fid);
		}
	}

	_ClearNeighbor( other);

	//remove unvalid
	for(size_t i = 0 ; i < listDelete.size() ; i++)
	{
		INT32 fid = listDelete[i]; 

		_UnlinkNeighbor( fid);

		I_FACE * pFace  = m_pPlane + fid;
		pFace->_isValid = false;
		
		i3::vu::remove(*m_plistFace, pFace);
//		m_plistFace->Remove( pFace);
	}

	_setValidVertex( other, false);
}

void i3OptBuildProgressiveMesh::_UnlinkNeighbor( INT32 vid, INT32 fid)
{
	i3::vector<INT32>* pNeighbor = _getNeighbor( vid);
	for( size_t i = 0;i < pNeighbor->size(); ++i)
	{
		INT32 finnerid =(*pNeighbor)[i];
		if( fid == finnerid)
		{
		//	pNeighbor->Delete( i);
			pNeighbor->erase(pNeighbor->begin() + i);
			i--;
		}
	}
}

void i3OptBuildProgressiveMesh::_ClearNeighbor( INT32 vid)
{
	i3::vector<INT32> * pNeighbor = _getNeighbor( vid);
	pNeighbor->clear();
}

void i3OptBuildProgressiveMesh::_UnlinkNeighbor( INT32 fid)
{
	INT32 i;
	I_FACE * pFace  = m_pPlane + fid;

	for( i = 0;i <3; ++i)
	{
		_UnlinkNeighbor( pFace->id[i], fid);
	}
}

void i3OptBuildProgressiveMesh::_updateError( INT32 vid)
{
	i3::vector_multiset<ERRINFO*, Neighbor_ERRINFO_Comp>* pList = _getEdge( vid);
	const i3::vector<ERRINFO*>& vec = pList->get_vector();

	for(size_t i = 0;i < vec.size(); ++i)
	{
		ERRINFO * pInfo = vec[i];

		{
			pInfo->err = _calcErr( m_pQuadric, pInfo);
			pInfo->key = pInfo->min;
			

			i3::pair<i3::vector_multiset<ERRINFO*, ErrValue_ERRINFO_Comp>::iterator, 
				i3::vector_multiset<ERRINFO*, ErrValue_ERRINFO_Comp>::iterator> pr = m_listErrValue.equal_range(pInfo);

			i3::vector_multiset<ERRINFO*, ErrValue_ERRINFO_Comp>::iterator it = i3::find(pr.first, pr.second, pInfo);

			if (it != pr.second) 
			{
				m_listErrValue.erase(it);
			}	

			m_listErrValue.insert(pInfo);
			
//			m_plistErrValue->Remove( pInfo);
//			m_plistErrValue->Add( pInfo);

		}
	}
}

ERRINFO * i3OptBuildProgressiveMesh::_FindMinimalErr(void)
{
	INT32 cnt = (INT32)m_listErrValue.size();
	if( cnt <= 0)	return nullptr;

	ERRINFO * pMinErr = m_listErrValue.get_vector()[0];

	m_listErrValue.erase( m_listErrValue.begin());
//	m_plistErrValue->Delete(0);

	return pMinErr;
}


INT32 i3OptBuildProgressiveMesh::Construct_N_Contract( INT32 targetFaceCount)
{
	i3::vector<INT32> listChange, listDelete;

	// contract vertices and generate vsplits
	while (INT32(m_plistFace->size()) > targetFaceCount)
	{
		ERRINFO * pMinErr = _FindMinimalErr();
		if( pMinErr == nullptr)
			return INT32(m_plistFace->size());

		if( _isValidVertex( pMinErr->min) == false || _isValidVertex( pMinErr->max) == false)
		{
			I3TRACE( "이런게 있으면..안되지...\n");
		//	m_plistErr->Remove( pMinErr);
			i3::pair<i3::vector_multiset<ERRINFO*>::iterator, i3::vector_multiset<ERRINFO*>::iterator> pr = m_listErr.equal_range(pMinErr);
			i3::vector_multiset<ERRINFO*>::iterator it = i3::find(pr.first, pr.second, pMinErr);
			
			if (it != pr.second)
				m_listErr.erase(it);

			continue;
		}

		INT32 key = pMinErr->key;
		INT32 other = (key == pMinErr->min) ? pMinErr->max : pMinErr->min;
		I3ASSERT( key == pMinErr->min || key == pMinErr->max);

		I3GEOM_VSPLIT * pNewSplit = (I3GEOM_VSPLIT*)i3MemAlloc( sizeof( I3GEOM_VSPLIT));
		{//vsplit update
			i3mem::FillZero( pNewSplit, sizeof( I3GEOM_VSPLIT));
			
			pNewSplit->vNewID = key;
			pNewSplit->vOldID = other;

			_getVertex( key,			&pNewSplit->v0);
			_getVertex( other,			&pNewSplit->v1);
			_getVertex( &pMinErr->vNew, &pNewSplit->vf);

			if( m_uvDim > 0)
			{
				_getUV( key,			&pNewSplit->uv0);
				_getUV( other,			&pNewSplit->uv1);
				_getUV( &pMinErr->vNew, &pNewSplit->uvf);
			}

			pNewSplit->err = (REAL32)pMinErr->err;
		}

		 listChange.clear();
		 listDelete.clear();

		_calcContract( pMinErr, listChange, listDelete);
		_moveEdgeContract( pMinErr);

		_applyContract( pMinErr, listChange, listDelete, pNewSplit);

	//	m_plistErr->Remove( pMinErr);
		i3::pair<i3::vector_multiset<ERRINFO*>::iterator, i3::vector_multiset<ERRINFO*>::iterator> pr = m_listErr.equal_range(pMinErr);
		i3::vector_multiset<ERRINFO*>::iterator it = i3::find(pr.first, pr.second, pMinErr);

		if (it != pr.second)
			m_listErr.erase(it);


		_updateError( key);
		m_plistVSplit->push_back( pNewSplit);
	}

	return INT32(m_plistFace->size());
}

void i3OptBuildProgressiveMesh::_DumpData(i3GeometryAttr2 * pTargetGeometry)
{
	pTargetGeometry->SetIndexArray( m_pIA);
	pTargetGeometry->SetVertexArray( m_pVA, I3G_PRIM_TRILIST, m_PrimCount, 0, false);
	pTargetGeometry->SetSplitData( m_plistVSplit);
	pTargetGeometry->OptimizeForSkin();
}

INT32 i3OptBuildProgressiveMesh::_WeldVertices()
{
	VEC3D v0;
	INT32 i, j, k;
	INT32 vCnt = m_pVA->GetCount();
	INT32 iCnt = m_pIA->GetCount()/3;

	i3::vector<INT32> list(vCnt, 0);

	m_pIA->Lock();
	m_pVA->Lock();

	for( i = 0;i < iCnt; i++)
	{
		I_FACE * pFace = m_pPlane + i;

		pFace->originid[0] = m_pIA->GetIndex( i*3);
		pFace->id[0] = m_pIA->GetIndex( i*3);

		pFace->originid[1] = m_pIA->GetIndex( i*3+1);
		pFace->id[1] = m_pIA->GetIndex( i*3+1);

		pFace->originid[2] = m_pIA->GetIndex( i*3+2);
		pFace->id[2] = m_pIA->GetIndex( i*3+2);
	}

	for( i = 0;i < vCnt; ++i)
	{
		INT32 cnt = list[i];
		if( cnt > 0) 
			continue;

		m_pVA->GetPosition( i, &v0);
		for( j = i+1; j < vCnt; ++j)
		{
			INT32 lcnt = list[j];
			if( lcnt > 0) 
				continue;

			if(m_pVA->IsSamePosition( j, &v0))
			{
				for( k = 0;k < iCnt; k++)
				{
					I_FACE * pFace = m_pPlane + k;
					for( INT32 l = 0; l < 3; ++l)
					{
						if( pFace->id[l] == j)
						{
							pFace->id[l] = i;
							//I3TRACE("%d ->%d\n", k*3+l, j, i);
						}
					}
				}

				lcnt++;
				list[j] = lcnt;
			}
		}
	}

	m_pIA->Unlock();
	m_pVA->Unlock();

	return 0;
}

bool i3OptBuildProgressiveMesh::_isClosedVertex( INT32 vid)
{
	if( _getNeighbor( vid)->size() == _getEdge( vid)->size() )
	{
		return true;
	}

	return false;
}

bool i3OptBuildProgressiveMesh::_isClosedVertex( ERRINFO * pInfo)
{
	if( (_getNeighbor( pInfo->min)->size() == _getEdge( pInfo->min)->size()) && 
		(_getNeighbor( pInfo->max)->size() == _getEdge( pInfo->max)->size()))
	{
		return true;
	}

	return false;
}

INT32 i3OptBuildProgressiveMesh::DumpGeometry( i3GeometryAttr * pGeom)
{
	m_pVA = pGeom->GetVertexArray();
	m_pIA = pGeom->GetIndexArray();

	m_pVA->Lock();
	for( UINT32 i = 0;i < m_pVA->GetCount(); ++i)
	{
		VEC3D vPos;
		m_pVA->GetPosition( i, &vPos);

		I3TRACE( "v %f %f %f\n", getX(&vPos),getY(&vPos),getZ(&vPos));
	}
	m_pVA->Unlock();

	m_pIA->Lock();
	for( UINT32 i = 0;i < pGeom->GetPrimitiveCount() *3; i+=3)
	{
		INT32 id1 = m_pIA->GetIndex( i);
		INT32 id2 = m_pIA->GetIndex( i + 1);
		INT32 id3 = m_pIA->GetIndex( i + 2);

		I3TRACE("f %d %d %d\n", id1+1, id2+1, id3+1);
	}
	m_pIA->Unlock();

	return 0;
}

INT32 i3OptBuildProgressiveMesh::DumpGeometry(void)
{
	UINT32 i;

	m_pVA->Lock();

	for( i = 0;i < m_pVA->GetCount(); ++i)
	{
		VEC3D vPos;
		m_pVA->GetPosition( i, &vPos);

		I3TRACE( "v %1.30f %1.30f %1.30f\n", getX(&vPos),getY(&vPos),getZ(&vPos));
	}
	m_pVA->Unlock();

	for( i = 0;i < m_pIA->GetCount()/3; i++)
	{
		I_FACE * pFace = m_pPlane + i;

		I3TRACE("f %d %d %d\n", pFace->id[0]+1, pFace->id[1]+1, pFace->id[2]+1);
	}
	return 0;
}

void i3OptBuildProgressiveMesh::CountValidVertex(void)
{
	m_pVA->Lock();
	m_pIA->Lock();

	INT32 i, realcnt;
	INT32 cnt = m_pVA->GetCount();
	INT32 icnt = m_pIA->GetCount();

	i3::vector<INT32> list(cnt, -1);

	for( i = 0; i< icnt/3; ++i)
	{
		//INT32 id = (INT32)m_pIA->GetIndex( i);
		I_FACE *face = m_pPlane+i;
		for( INT32 j = 0;j< 3; ++j)
		{
			list[ face->id[j] ] = 1;  
		}
	}

	realcnt = 0;
	for( i = 0;i < cnt; ++i)
	{
		INT32 id = list[i]; 
		if( id != -1)
			realcnt++;
	}

	m_pVA->Unlock();
	m_pIA->Unlock();

	I3TRACE("total: %d, valid: %d\n", cnt, realcnt);
}
