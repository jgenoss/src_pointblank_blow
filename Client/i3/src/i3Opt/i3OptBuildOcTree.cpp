#include "i3OptPCH.h"
#include "i3OptBuildOcTree.h"
#include "i3OptCollapseNode.h"
#include "i3OptBuildIndexedGeometry.h"
#include "i3GeometrySplitter.h"
#include "i3OptCollapseTransform.h"
#include "i3OptOctreeNode.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3OptBuildOcTree);

//#define		VLOG_PART_GEN
//#define	VLOG_OCTREE
//#define VLOG_SPLIT_MESH

i3OptBuildOcTree::i3OptBuildOcTree()
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

i3OptBuildOcTree::~i3OptBuildOcTree()
{
	I3_SAFE_RELEASE( m_pTreeRoot);
}

const char *	i3OptBuildOcTree::getDesc(void)
{
	return "Octree 구성";
}

bool i3OptBuildOcTree::isSupportProgress(void)
{
	return true;
}

void i3OptBuildOcTree::_ProgressSet( INT32 maxCount)
{
	ReportStart( 100, "Building octree nodes...");

	m_MaxProgress = maxCount;
	m_CurProgress = 0;
}

void i3OptBuildOcTree::_ProgressInc( const char * pszStr)
{
	INT32 percent;

	m_CurProgress ++;

	percent = (m_CurProgress * 100) / m_MaxProgress;

	i3::string_ncopy_nullpad( m_szProgress, pszStr, sizeof(m_szProgress));
	ReportProgress( percent, m_szProgress);

	I3TRACE( "%s\n", m_szProgress);
}

void i3OptBuildOcTree::_ProgressMsg( const char * pszStr)
{
	INT32 percent;

	percent = (m_CurProgress * 100) / m_MaxProgress;

	i3::string_ncopy_nullpad( m_szProgress, pszStr, sizeof(m_szProgress));
	ReportProgress( percent, m_szProgress);

	I3TRACE( "%s\n", m_szProgress);
}

void i3OptBuildOcTree::_ProgressEnd(void)
{
	ReportEnd();
}


bool i3OptBuildOcTree::OnNode( i3Node * pNode, i3Node * pParent)
{
	// i3Geometry Node를 만날 때마다 개별적으로 MESH_INFO를 생성한다.
	if( i3::kind_of<i3Geometry* >(pNode))
	{
		m_pTreeRoot->addMesh( (i3Geometry *) pNode);
	}

	return true;
}

I3SG_TRAVERSAL_RESULT _ConvertRuntimeVA( i3Node * pNode, void * pData, i3MatrixStack * pStack)
{
	if( i3::same_of<i3Geometry* >(pNode))
	{
		i3Geometry * pGeo = (i3Geometry *) pNode;
		INT32 i;
		i3RenderAttr * pAttr;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			pAttr = pGeo->GetGeometryAttr( i);

			if( !i3::kind_of<i3GeometryAttr* >(pAttr))
				continue;

			i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;
			i3VertexArray * pVA = pGeoAttr->GetVertexArray();

			if( i3::same_of<i3VertexArrayMem*>(pVA))
			{
				i3VertexArray * pNewVA = i3VertexArray::new_object_ref();

				pNewVA->Create( pVA->GetFormat(), pVA->GetCount(), pVA->GetUsageFlag());

				pVA->CopyTo( pNewVA, I3_COPY_INSTANCE);

				pGeoAttr->SetVertexArray( pNewVA);
			}
		}
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

// Todo : 실제 분할이 일어날 노드에대한 조건...을 만들어주어야합니다.
void i3OptBuildOcTree::Trace( i3Node * pRoot)
{
	// 미리 제거 가능한 Transform을 모두 제거한다.
	{
		i3OptCollapseTransform opt;

		opt.Optimize( getSg());

		i3Node * pNewRoot = opt.getTraceRoot();

		if( pNewRoot != pRoot)
		{
			SetTraceRoot( pNewRoot);

			pRoot = getTraceRoot();
		}
	}

	// 전체 Sg.에 포함되어 있는 Geometry에 관한 모든 정보를 별도로 List로 구성한다.
	// 이 정보는 이미 Optimize되어 있는 Sg.에 대해서도 Octree Build가 성공적으로
	// 이루어지기 위해, 각 Geometry에 적용되고 있는 RenderAttr들을 모두 포함한 것으로
	// 새롭게 구성한다.
	{
		m_pTreeRoot = i3OptOctreeNode::new_object();

		// Recursive하게 Part List의 Mesh들을 배분하면서
		// Octree를 구성한다.
		m_pTreeRoot->SetName( "Octree");
		m_pTreeRoot->setMaxDepth( m_MaxDepth);
		m_pTreeRoot->setK( getExtent());
		m_pTreeRoot->addOctreeStyle( I3_OCTST_ROOT);
	}

	i3SceneOptimizer::Trace( pRoot);

	{
		// Progress Report할 전체 개수를 예측한다.
		VEC3D sz;

		m_pTreeRoot->getWholeBound()->getSize( &sz);

		INT32 maxPrg = (INT32)(sz.x / m_CellSize) * (INT32)(sz.y / m_CellSize) * (INT32)(sz.z / m_CellSize);

		_ProgressSet( maxPrg);
	}

	// Octree 구성.
	{
		m_pTreeRoot->Build( m_pTreeRoot->getWholeBound(), this, "Octree");

		m_pTreeRoot->UpdateLocaleCode( m_pTreeRoot->getWholeBound(), true);
#if defined( I3_DEBUG)
		m_pTreeRoot->CheckLocaleCode();
#endif
	}

	// Opt-version의 Octree를 Sg-version으로 Converting
	{
		i3OcTreeNode * pNewRoot = m_pTreeRoot->ConvertTo(nullptr);

		SetTraceRoot( pNewRoot);
	}

	I3_SAFE_RELEASE( m_pTreeRoot);

	// 분리된 i3VertexArray는 i3VertexArrayMem Class이기 때문에
	// 실제 Rendering이 가능한 것으로 대체해준다.
	{
		i3Scene::Traverse( getTraceRoot(), _ConvertRuntimeVA, nullptr);
	}

	_ProgressEnd();
}