#include "i3SceneDef.h"
#include "i3Geometry.h"
#include "i3GeometryAttr.h"
#include "i3PointSpriteAttr.h"
#include "i3AttrStackManager.h"
#include "i3SceneTracer.h"
#include "i3QuadAttr.h"
#include "i3SpriteAttr.h"
#include "i3BillboardAttr.h"
#include "i3CommonNodeProcs.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/itl/container_util.h"

#include "i3Math/structure/aabb.h"

I3_CLASS_INSTANCE( i3Geometry);

I3_TRACE_RESULT I3SG_NODE_PROC( i3Geometry)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3Geometry * pGeo = (i3Geometry *) pNode;
	INT32 i;
	i3AttrStackManager * pStackManager = pTracer->GetAttrStackManager();
	bool bFirst = true;
	MATRIX tmp;

	if( pStackManager->isEnabled())
	{
		pStackManager->FlushPushedAttrs( pTracer->GetOutput());

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			i3RenderAttr * pAttr = pGeo->GetGeometryAttr( i);

			if( i3::same_of<i3GeometryAttr* >(pAttr))
			{
				i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

				pGeoAttr->setBoneMatrixListAttr( pTracer->getCurrentBoneMatrixList());
			}
			else if( i3::same_of<i3QuadAttr* >(pAttr))
			{
				if( bFirst)
				{
					_setInverseModelView3x3( &tmp, pTracer->GetModelViewMatrixStack(), pTracer);
				}

				((i3QuadAttr *)pAttr)->SetInverseViewMatrix( &tmp);
			}
			else if( i3::same_of<i3SpriteAttr* >(pAttr))
			{
				i3SpriteAttr * pSpriteAttr = ( i3SpriteAttr *) pAttr;

				if( pSpriteAttr->getFaceToCameraEnable())
				{
					if( bFirst)
					{
						_setInverseModelView3x3( &tmp, pTracer->GetModelViewMatrixStack(), pTracer);
						bFirst = false;
					}

					((i3SpriteAttr *)pAttr)->SetInverseViewMatrix( &tmp);
				}
			}
			else if( i3::same_of<i3BillboardAttr* >(pAttr))
			{
				if( bFirst)
				{
					_setInverseModelView3x3( &tmp, pTracer->GetModelViewMatrixStack(), pTracer);
					bFirst = false;
				}

				((i3BillboardAttr *)pAttr)->SetInverseViewMatrix( &tmp);
			}
			else if( i3::same_of<i3GeometryAttr2* >(pAttr))
			{
				i3GeometryAttr2 * pGeoAttr = (i3GeometryAttr2 *) pAttr;

				pGeoAttr->setBoneMatrixListAttr( pTracer->getCurrentBoneMatrixList());
			}
			else if( i3::same_of<i3SkinGeometryAttr* >(pAttr))
			{
				i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

				pGeoAttr->setBoneMatrixListAttr( pTracer->getCurrentBoneMatrixList());
			}

			pTracer->AddAttributeToOutput( pGeo->GetGeometryAttr(i));
		}
	}

	return I3_TRACE_RESULT_CONTINUE;
}


// 지오미트리는 말단인듯 하며, 따라서 자식노드 순회가 보이지 않는다..

void	i3Geometry::PreTrace(i3SceneTracer * pTracer)
{
	i3::aabb* topAABB = pTracer->TopAABBInStack();

	if (topAABB == nullptr)			// 이런 경우 바운딩 박스 구해봤자 의미없어서 얼른 리턴...(즉, 부모 노드 어딘가에 NODEFLAG_UPDATE_AABB같은게 없다면 처리 필요없음..)
		return;

	i3MatrixStack* pWorldMatStack =	pTracer->GetModelViewMatrixStack();
	MATRIX* pWorldMat =	pWorldMatStack->GetTop();
	i3BoneMatrixListAttr* boneMatAttr =	pTracer->getCurrentBoneMatrixList();
	
	INT32 numGeo = this->GetGeometryAttrCount();

	i3::aabb		aabbEachGeometry;
	i3::aabb		aabbEachAttr;

	for (INT32 i = 0; i < numGeo; ++i)
	{
		i3RenderAttr * pAttr = this->GetGeometryAttr(i);

		if (i3::kind_of<i3GeometryAttr* >(pAttr))
		{
			i3GeometryAttr * pGeoAttr = static_cast<i3GeometryAttr *>(pAttr);
			pGeoAttr->CalcUpdateAABB(aabbEachAttr, pWorldMat, boneMatAttr);
			aabbEachGeometry.merge(aabbEachAttr);
		}
	}

	topAABB->merge(aabbEachGeometry);

}


i3Geometry::i3Geometry(void)
{
	m_List.reserve( 16);

	_setTraceProc( I3SG_NODE_PROC( i3Geometry));
}

i3Geometry::~i3Geometry(void)
{
	RemoveAllGeometryAttr();
}

void i3Geometry::AppendGeometryAttr( i3RenderAttr * pAttr)
{
	m_List.push_back( pAttr);
	I3_MUST_ADDREF( pAttr);

#if defined( I3_DEBUG)
	pAttr->setOwner( this);
#endif
}

void i3Geometry::RemoveGeometryAttr( i3RenderAttr * pAttr)
{
	i3::vu::remove(m_List, pAttr);

	I3_MUST_RELEASE( pAttr);
}

void i3Geometry::RemoveAllGeometryAttr(void)
{
	
//	m_List.SafeReleaseAll();
	i3::cu::for_each_container(m_List, i3::fu::mem_fun_safe(&i3RenderAttr::Release));

	m_List.clear();
}

UINT32 i3Geometry::GetTotalVertexCount(void)
{
	UINT32 count = 0;
	i3RenderAttr * pAttr;
	i3VertexArray * pVA;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pAttr = GetGeometryAttr( i);

		if( i3::kind_of<i3GeometryAttr*>(pAttr))
		{
			i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

			pVA = pGeoAttr->GetVertexArray();

			count += pVA->GetCount();
		}
	}

	return count;
}

void i3Geometry::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	
	i3RenderAttr * pAttr;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pAttr = GetGeometryAttr( i);

		pAttr->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

UINT32 i3Geometry::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	OBJREF val;
	UINT32 cnt;


	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Geometry Attribute count
	cnt = (INT32)m_List.size();
	Rc = pResFile->GetStream()->Write( &cnt, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Geometry::OnSave()", "Could not write geometry attribute count.");
		return STREAM_ERR;
	}
	Result += Rc;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		val = pResFile->GetObjectPersistID( m_List[i]);

		Rc = pResFile->GetStream()->Write( &val, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Geometry::OnSave()", "Could not write geometry attribute's Id.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3Geometry::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3Geometry::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	UINT32 count;
	OBJREF val;
	INT32 i;
	i3RenderAttr * pAttr;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Geometry Attribute count
	Rc = pResFile->GetStream()->Read( &count, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Geometry::OnLoad()", "Could not read geometry attribute count.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( count > 0)
	{
		m_List.resize( count);
        
		for( i = 0; i < (INT32) count; i++)
		{
			Rc = pResFile->GetStream()->Read( &val, sizeof(OBJREF));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3Geometry::OnLoad()", "Could not read geometry attribute's Id.");
				return STREAM_ERR;
			}
			Result += Rc;

			pAttr = (i3RenderAttr *) pResFile->FindObjectByID( val);
			
			m_List[i] = pAttr;
			I3_MUST_ADDREF( pAttr);
		}
	}

	return Result;
}

bool i3Geometry::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3Node::OnSaveXML(pFile, pXML);

	if (bResult == false)
		return false;

	char szTemp[128];

	pXML->addAttr("ListSize", UINT32(m_List.size()));
	for (size_t i = 0; i < m_List.size(); i++)
	{
		i3::sprintf(szTemp, "List%d", i);
		pXML->addAttr(szTemp, OBJREF(pFile->GetObjectPersistID(m_List[i])));
	}
	return bResult;
}

bool i3Geometry::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3Node::OnLoadXML(pFile, pXML);

	if (bResult == false)
		return false;

	char szTemp[128];
	i3RenderAttr* pAttr;
	UINT32 listSize;
	pXML->getAttr("ListSize", &listSize);
	if (listSize > 0)
	{
		m_List.resize(listSize);
		OBJREF objref;
		for (UINT32 i = 0; i < listSize; i++)
		{
			i3::sprintf(szTemp, "List%d", i);
			pXML->getAttr(szTemp, &objref);

			pAttr = (i3RenderAttr *)pFile->FindObjectByID(objref);

			m_List[i] = pAttr;
			I3_MUST_ADDREF(pAttr);
		}
	}

	return bResult;
}

void i3Geometry::_GetWrappingBound( I3_GETBOUND_INFO * pInfo)
{
	INT32 i;
	i3RenderAttr * pAttr;
	i3MatrixStack * pStack = pInfo->m_pStack;
	i3BoundBox * pBox = pInfo->m_pBound;
	VEC3D vmin, vmax, temp;

	for( i = 0; i < GetGeometryAttrCount(); i++)
	{
		pAttr = GetGeometryAttr( i);

		if( i3::kind_of<i3GeometryAttr*>(pAttr))
		{
			i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

			if( pGeoAttr->isValidBound())
			{
				i3Vector::Copy( &vmin, (VEC3D *) pGeoAttr->getBoundMin());
				i3Vector::Copy( &vmax, (VEC3D *) pGeoAttr->getBoundMax());
			}
			else
			{
				pGeoAttr->CalcBoundBox( &vmin, &vmax);

				pGeoAttr->setBound( &vmin, &vmax);
			}

			i3Vector::TransformCoord( &vmin, &vmin, pStack->GetTop());
			i3Vector::TransformCoord( &vmax, &vmax, pStack->GetTop());

			i3Vector::Minimize( &temp, &vmin, &vmax);
			i3Vector::Maximize( &vmax, &vmin, &vmax);
			i3Vector::Copy( &vmin, &temp);

			if( pBox->IsEmpty())
			{
				pBox->SetMinMax( &vmin, &vmax);
			}
			else
			{
				pBox->ExtendByVec( &vmin);
				pBox->ExtendByVec( &vmax);
			}
		}
	}
}

void i3Geometry::CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, i3MatrixStack* matStack, bool twoSideEnabled)
{
	const INT32 numAttr = GetGeometryAttrCount();

	const MATRIX* pWorldMat = matStack->GetTop();

	for ( INT32 i = 0; i < numAttr; ++i )
	{
		i3GeometryAttr* pAttr = i3::kind_cast<i3GeometryAttr*>(GetGeometryAttr(i));
		if (pAttr)
		{
			pAttr->CalcOccMeshSet(occMeshSet, pWorldMat, twoSideEnabled);
		}
	}
}


void i3Geometry::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node::CopyTo( pDest, method);

	i3Geometry * pGeo = (i3Geometry *) pDest;
	INT32 i;

	// i3GeometryAttr은 Reference Copy과 Instance Copy와 관계 없이
	// 항상 Reference Copy 방식으로 Pointer만 공유한다.
	//
	// 이는 Vertex Array를 공유하도록 하기 위한 조치이다.
	pGeo->m_List.resize( GetGeometryAttrCount());

	for( i = 0; i < GetGeometryAttrCount(); i++)
	{
		i3RenderAttr * pAttr = GetGeometryAttr( i);

		i3RenderAttr * pNewAttr = pAttr->create_instance(); //(i3RenderAttr *) CREATEINSTANCE( pAttr->GetMeta());
		I3_MUST_ADDREF( pNewAttr);

		pAttr->CopyTo( pNewAttr, method);
		
		pGeo->m_List[i] = pNewAttr;
	}
}

void i3Geometry::OnLostDevice( bool bLostDevice)
{
	INT32 i;

	for( i = 0; i < this->GetGeometryAttrCount(); i++)
	{
		i3RenderAttr * pAttr = GetGeometryAttr( i);

		pAttr->OnLostDevice( bLostDevice);
	}

	i3Node::OnLostDevice( bLostDevice);
}

void i3Geometry::OnRevive( i3RenderContext * pCtx)
{
	INT32 i;

	for( i = 0; i < this->GetGeometryAttrCount(); i++)
	{
		i3RenderAttr * pAttr = GetGeometryAttr( i);

		pAttr->OnRevive( pCtx);
	}

	i3Node::OnRevive( pCtx);
}
