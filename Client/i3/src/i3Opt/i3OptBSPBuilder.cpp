#include "i3OptPCH.h"
#include "i3OptBSPBuilder.h"
#include "i3OptBuildTriStrip.h"
#include "i3OptBuildIndexedGeometry.h"
#include "i3OptRemoveRedundantAttr.h"
#include "i3OptBuildCommonSg.h"
#include "i3OptGatherUpAttribute.h"
#include "i3OptCreateBoundBox.h"
#include "i3OptBSPGeometry.h"
#include "i3OptBSPNode.h"


I3_CLASS_INSTANCE( i3OptBSPBuilder);

i3OptBSPBuilder::i3OptBSPBuilder(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;

	m_Ctx.m_Condition = SPLIT_COND_DEPTH | SPLIT_COND_POLYCOUNT;
	m_Ctx.m_Depth = 0;
	m_Ctx.m_LimitCount = 800;
	m_Ctx.m_LimitDepth = 8;
	m_Ctx.m_Mode = SPLIT_MODE_HALFPLANE;
	m_Ctx.m_Plane = SPLIT_PLANE_XAXIS;
	m_Ctx.m_bSplitPoly = true;
	m_Ctx.m_NodeCount = 0;
}

i3OptBSPBuilder::~i3OptBSPBuilder(void)
{
	_RemoveAllGeo();
}

void	i3OptBSPBuilder::_RemoveAllGeo(void)
{
	i3OptBSPGeometry * pGeo;

	for(size_t i = 0; i < m_GeoList.size(); i++)
	{
		pGeo = m_GeoList[i];
		I3_SAFE_RELEASE( pGeo);
	}

	m_GeoList.clear();
}

void	i3OptBSPBuilder::_FlushDifferAttrs( i3OptBSPGeometry * pGeo)
{
	INT32 i;
	i3::vector<i3RenderAttr*>& List = pGeo->getAttrList();
	bool bProcess;

	// 지금까지 설정된 모든 Attr들을 List에 추가한다.

	for( i = 0; i < I3_ATTR_MAX; i++)
	{
		i3AttrStack * pStack = m_pAttrStack->GetStack( (I3_ATTRID)i);

		if( pStack == nullptr)
			continue;

		bProcess = true;

		switch( i)
		{
			case I3_ATTRID_SHADE_MODE :
			case I3_ATTRID_STENCIL_FUNC :
			case I3_ATTRID_STENCIL_OPERATION :
			case I3_ATTRID_STENCILTEST_ENABLE :
			case I3_ATTRID_PROJECT_MATRIX :
			case I3_ATTRID_VIEW_MATRIX :
			case I3_ATTRID_MODEL_VIEW_MATRIX :
			case I3_ATTRID_TEXTURE_COORD_SCALE :
			case I3_ATTRID_TEXTURE_COORD_OFFSET :
			case I3_ATTRID_BONEMATRIXLIST :
			case I3_ATTRID_TEXTURE_COORD_SOURCE :
			case I3_ATTRID_RENDER_ATTR_LIST :
			case I3_ATTRID_GEOMETRY :
			case I3_ATTRID_POINTSPRITE :
			case I3_ATTRID_QUAD :
			case I3_ATTRID_SPRITE :
			case I3_ATTRID_BILLBOARD :
			case I3_ATTRID_RENDERTARGET :
			case I3_ATTRID_CLEARRENDERTARGET :
			case I3_ATTRID_TEXT :
			case I3_ATTRID_GPUCOMMANDLIST :
			case I3_ATTRID_SPRITE2D :
			case I3_ATTRID_USER :
			case I3_ATTRID_VIEWPORT :
			case I3_ATTRID_TEXTDX :
			case I3_ATTRID_TEXTDX2: 
			case I3_ATTRID_TEXTURE_MATRIX :
			case I3_ATTRID_CLIPPING_ENABLE :	bProcess = false;	break;
		}

		// 모든 Attr들을 추가. (Default Attr들도 추가한다.)
		if((bProcess) && ( pStack->GetTop() != nullptr))
		{
			i3RenderAttr * pAttr = pStack->GetTop();
			I3_MUST_ADDREF( pAttr);
			List.push_back( pAttr);
		}
	}
}

bool i3OptBSPBuilder::_isTransparentNode( i3Node * pNode)
{
	while( pNode != nullptr)
	{
		if( pNode->IsFlag( I3_NODEFLAG_TRANSPARENT))
			return true;

		pNode = pNode->GetParent();
	}

	return false;
}

bool i3OptBSPBuilder::OnPreNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::kind_of<i3AttrSet*>(pNode))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

		m_pAttrStack->PushAttrs( pAttrSet->GetAttrs());
	}

	return true;
}

bool i3OptBSPBuilder::OnPostNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::kind_of<i3AttrSet*>(pNode))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

		m_pAttrStack->PopAttrs( pAttrSet->GetAttrs());
	}
	else if( i3::kind_of<i3Geometry*>(pNode))
	{
		// 새 BSP-Geometry를 등록한다.
		i3OptBSPGeometry *		pNew = i3OptBSPGeometry::new_object();

		_FlushDifferAttrs( pNew);

		pNew->setMatrix( m_pStack->GetTop());
		pNew->setGeometry( (i3Geometry *) pNode);
		pNew->setTransparent( _isTransparentNode( pNode));

		if( pNew->isTransparent())
			m_TransGeoList.push_back( pNew);
		else
			m_GeoList.push_back( pNew);

		m_TotalNodeCount++;
	}

	return true;
}

void i3OptBSPBuilder::_SearchAllGeometry( i3Node * pNode)
{
	m_pAttrStack = i3AttrStackManager::new_object();

	SetMatrixStackEnable(true);

	i3SceneOptimizer::Trace( pNode);

	I3_SAFE_RELEASE( m_pAttrStack);
}

// Triangle List가 아닌 Geometry라면 Non-Indexed Triangle List로 변환한다.
void i3OptBSPBuilder::_ConvertToTriList(void)
{
	i3OptBSPGeometry * pGeo;
	i3RenderAttr * pAttr;
	i3GeometryAttr * pGeoAttr;

	ReportSubStart( m_GeoList.size(), "Converting triangle lists...");

	for(size_t i = 0; i < m_GeoList.size(); i++)
	{
		pGeo = m_GeoList[i];

		i3Geometry * pGeoNode = pGeo->getGeometry();

		ReportSubProgress( i, pGeoNode->GetName());

		for(INT32 j = 0; j < pGeoNode->GetGeometryAttrCount(); j++)
		{
			pAttr = pGeoNode->GetGeometryAttr( j);

			if( !i3::same_of<i3GeometryAttr* >(pAttr))
				continue;

			pGeoAttr = (i3GeometryAttr *) pAttr;

			if( pGeoAttr->GetPrimitiveType() != I3G_PRIM_TRILIST)
			{
				// Triangle List가 아니라면 Triangle List로 변환
				i3OptBuildTriStrip opt;

				opt.SetAutoSwitching(false);
				opt.SetTargetPrimType( I3G_PRIM_TRILIST);

				opt.BuildStrip( pGeoAttr);
			}

			if( pGeoAttr->GetIndexArray() != nullptr)
			{
				// Index Array가 있다면 없앤다.
				i3OptBuildIndexedGeometry opt;

				opt.ConvertIndexed2Vertex( pGeoAttr);
			}
		}
	}

	ReportSubEnd();
}

void i3OptBSPBuilder::Optimize( i3SceneGraphInfo * pSgInfo)
{
	i3Node * pRoot;

	setSg( pSgInfo);

	m_TotalNodeCount = 0;

	ReportStart( 9, "BSP Generation");

	// 처리를 위해, pSgInfo의 Common Sg와 Instance Sg를 연결하여
	// 새로운 Instance Sg로 설정한다.
	// BSP가 생성되면 완전히 새로운 Sg.를 구성하여 Instance Sg로
	// 등록하게 되기 때문에 기존의 Node는 맘껏 수정되어도 무방하다.
	{
		if( pSgInfo->getCommonSg() != nullptr)
		{
			pRoot = pSgInfo->getCommonSg();

			if( pSgInfo->getInstanceSg() != nullptr)
			{
				i3Node * pTail = pRoot;

				while( pTail->GetChildCount() > 0)
					pTail = pTail->GetChild( 0);

				if( pTail->IsChild( pSgInfo->getInstanceSg()) == false)
					pTail->AddChild( pSgInfo->getInstanceSg());
			}
		}
		else
		{
			pRoot = pSgInfo->getInstanceSg();
		}

		if( pRoot == nullptr)			// Optimize할 것이 없다.
		{
			ReportEnd();
			return;
		}

		// 모든 Geometry들을 검색한다.
		ReportProgress( 0, "Searching geometries...");

		_SearchAllGeometry( pRoot);
		
	}

	// Geometry을 모두 Non-Indexed Triangle List로 변환한다.
	// 처리를 단순하게 하기 위한 준비.
	{
		ReportProgress( 1, "Converting meshes to triangle lists...");
		_ConvertToTriList();
	}

	//
	{
		i3Node * pNewRoot = i3Node::new_object_ref();
		i3BinaryPartitionNode * pBSP;
		i3OptBSPNode * pBSPRoot;

		m_Ctx.m_NodeCount = 0;

		// Opaque BSP
		{
			pBSPRoot = i3OptBSPNode::new_object();

			// Root Node를 구성하고, 발견된 모든 Geometry들을 Root에 추가한다.
			pBSPRoot->addGeometries( m_GeoList);

			ReportProgress( 2, "Building BSP tree with opaque parts of scene-graph...");

			// BSP Partitioning (Recursive Processing)
			m_Ctx.m_Depth = 0;
			ReportSubStart( m_TotalNodeCount, "Spliting geometries...");
			pBSPRoot->BuildBSP( &m_Ctx);
			ReportSubEnd();

			// Sg로 구성해 준다.
			ReportProgress( 3, "Building scene-graph from BSP...");
			pBSP = pBSPRoot->BuildSg();
			pBSP->SetName( "Opaque");

			pNewRoot->AddChild( pBSP);

			pBSPRoot->Dump();
			I3_SAFE_RELEASE( pBSPRoot);
		}

		// Transparent BSP
		{
			pBSPRoot = i3OptBSPNode::new_object();

			// Root Node를 구성하고, 발견된 모든 Geometry들을 Root에 추가한다.
			pBSPRoot->addGeometries( m_TransGeoList);

			// BSP Partitioning (Recursive Processing)
			ReportProgress( 4, "Building BSP tree with transparent parts of scene-graph...");
			m_Ctx.m_Depth = 0;
			pBSPRoot->BuildBSP( &m_Ctx);

			// Sg로 구성해 준다.
			ReportProgress( 5, "Building scene-graph from BSP...");
			pBSP = pBSPRoot->BuildSg();
			pBSP->SetName( "Transparent");
			pBSP->AddFlag( I3_NODEFLAG_TRANSPARENT, true);

			pNewRoot->AddChild( pBSP);

			pBSPRoot->Dump();
			I3_MUST_RELEASE(pBSPRoot);
		}

		m_pSg->setCommonSg(nullptr);
		m_pSg->setInstanceSg( pNewRoot);
	}

	__EP( 2);

	RESET_PROFILE();
	REPORT_PROFILE();

	// Remove Redundant Attributes
	{
		ReportProgress( 6, "Removing redundant attributes...");
		CallOptimizer( i3OptRemoveRedundantAttr::static_meta());
	}

	// 공통 Attribute들을 끌어 모아, Root를 생성한다.
	{
		ReportProgress( 7, "Constructing common scene-graph...");
		i3OptBuildCommonSg opt;

		opt.setAddToInstanceSgState( true);

		opt.Optimize( m_pSg);
	}

	// 하위에 남은 여러 다른 Attribute들에 대한 Gather-Up
	{
		i3OptGatherUpAttribute opt;

		opt.setMeta( i3LuxMapBindAttr::static_meta());

		ReportProgress( 8, "Reorganize scene-graph...");
		opt.Optimize( m_pSg);
	}
	{
		i3OptGatherUpAttribute opt;

		opt.setMeta( i3TextureBindAttr::static_meta());

		opt.Optimize( m_pSg);
	}
	{
		i3OptGatherUpAttribute opt;

		opt.setMeta( i3AlphaTestEnableAttr::static_meta());

		opt.Optimize( m_pSg);
	}

	ReportProgress( 9, "Creating bound boxes...");
	CallOptimizer( i3OptCreateBoundBox::static_meta());

	ReportEnd();
	//m_pBSPRoot->VLog();
}


bool i3OptBSPBuilder::isSupportProgress(void)
{
	return true;
}

const char *	i3OptBSPBuilder::getDesc(void)
{
	return "World에 대해 BSP Tree를 구성.";
}
