#include "i3OptPCH.h"
#include "i3OptBuildCommonSg.h"
#include "i3OptCollapseNode.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3OptBuildCommonSg);

i3OptBuildCommonSg::i3OptBuildCommonSg(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style = OPT_STYLE_APPLY_SG;
}

i3OptBuildCommonSg::~i3OptBuildCommonSg(void)
{
	I3_SAFE_RELEASE( m_pSg);
}

struct ATTRLIST_BUILD_INFO
{
	i3ClassMeta *	m_pMeta = nullptr;
	i3::vector<i3RenderAttr*>* m_pAttrList;
	i3::vector<i3Node*>*	m_pGeoList;
} ;

bool i3OptBuildCommonSg::_IsAllSameAttr( const i3::vector<i3RenderAttr*>& List) const
{
	i3RenderAttr * pAttr, * pFirstAttr;

	if( List.size() <= 0)
		return false;

	pFirstAttr = List[0];

	for(size_t i = 1; i < List.size(); i++)
	{
		pAttr = List[i];

		if( pAttr->IsSame( pFirstAttr) == false)
			return false;
	}

	return true;
}

static I3SG_TRAVERSAL_RESULT _AttrListProc( i3Node * pNode, void * pUserData)
{
	ATTRLIST_BUILD_INFO * pInfo = (ATTRLIST_BUILD_INFO *) pUserData;

	if( i3::kind_of<i3AttrSet*>(pNode))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;
		INT32 i;
		i3RenderAttr * pAttr;
		bool bFind = false;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( pAttr->same_of( pInfo->m_pMeta))
			{
				pInfo->m_pAttrList->push_back( pAttr);
				bFind = true;
				break;
			}
		}

		// 해당 Meta Type의 Attribute가 확인되었다면
		// 이 Node에 영향을 받는 모든 Geometry들의 개수를 확인한다.
		if( bFind)
		{
			i3Scene::FindNodeByExactType( pNode, i3Geometry::static_meta(), *pInfo->m_pGeoList);
		}
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3OptBuildCommonSg::Optimize( i3SceneGraphInfo * pSg)
{
	// 모든 Attribute 종류에 대해서 Sg.에서 검색한다.
	i3::vector<i3ClassMeta*>	MetaList;

	i3::vector<i3RenderAttr*>	AttrList;
	i3::vector<i3Node*>			GeoList;
	INT32 TotalGeoCount;
	i3ClassMeta * pMeta;
	i3AttrSet * pAttrSet = nullptr;
	i3Node * pNode;

	i3SceneOptimizer::Optimize( pSg);

	i3RenderAttr::static_meta()->get_all_derived_metas(MetaList); //->GetAllDerivedClasses( MetaList);

	pNode = m_pSg->getInstanceSg();

	// Common Sg.의 위치를 확인한다.
	if( m_pSg->getCommonSg() != nullptr)
	{
		pAttrSet = (i3AttrSet *) i3Scene::FindNodeByType( m_pSg->getCommonSg(), i3AttrSet::static_meta());
	}

	if( pAttrSet == nullptr)
	{
		pAttrSet = i3AttrSet::new_object_ref();

		if( m_pSg->getCommonSg() != nullptr)
		{
			pAttrSet->AddChild( m_pSg->getCommonSg());
		}

		m_pSg->setCommonSg( pAttrSet);
	}

	// 전체 Geometry의 개수를 구해둔다.
	{
		i3::vector<i3Node*> tempList;

		i3Scene::FindNodeByExactType( pNode, i3Geometry::static_meta(), tempList);

		TotalGeoCount = INT32(tempList.size());
	}
	
	for(size_t i = 0; i < MetaList.size(); i++)
	{
		pMeta = MetaList[i];

		if( pMeta->get_class_meta_style() == i3::cms_abstract )
			continue;

		if( pMeta->kind_of( i3GeometryAttr::static_meta()))
			continue;

		if( pMeta->same_of( i3SpriteAttr::static_meta()))
			continue;

		if( pMeta->same_of( i3Sprite2DAttr::static_meta()))
			continue;

		if( pMeta->same_of( i3BillboardAttr::static_meta()))
			continue;

		if( pMeta->same_of( i3QuadAttr::static_meta()))
			continue;

		if( pMeta->same_of( i3TextAttr::static_meta()))
			continue;

		if( pMeta->same_of( i3UserAttr::static_meta()))
			continue;

		// 해당 Meta Type의 Attribute를 모두 검색한다.
		AttrList.clear();
		GeoList.clear();

		ATTRLIST_BUILD_INFO info;

		info.m_pAttrList	= &AttrList;
		info.m_pGeoList		= &GeoList;
		info.m_pMeta		= pMeta;

		i3Scene::Traverse( pNode, (I3_SG_TRAVERSE_PROC) _AttrListProc, &info);
		//i3Scene::FindAttrByExactType( pNode, pMeta, &AttrList);
		
		// 수집이 Traverse에 의해 일어나고, 끝나면 중복노드를 그때 제거..
		i3::vu::remove_duplicates(GeoList);

		if( AttrList.size() <= 0)
			continue;

		if( INT32(GeoList.size()) < TotalGeoCount)
		{
			// 발견된 Attr이 모든 Geometry에 대한 것이 아니기 때문에
			// Common Sg로 추출해 낼 수 없다.
			continue;
		}

		// 발견된 모든 Attr이 모두 동일한 값을 가지고 있는지 확인
		if( _IsAllSameAttr( AttrList) == false)
			continue;

		// 첫번째 Attr을 제외하고 모두 제거한다.
		i3RenderAttr * pAttr = AttrList[0];
		I3_MUST_ADDREF( pAttr);

		pAttrSet->AddAttr( pAttr);

		i3Scene::RemoveRenderAttr( pNode, pMeta);
	}
	
	

	// 불필요하게 된 Node들을 제거.
	{
		i3OptCollapseNode opt;

		opt.Optimize( getSg());
	}

	if((m_bAddToInstanceSg) && (m_pSg->getCommonSg() != nullptr) && (m_pSg->getInstanceSg() != nullptr))
	{
		// Common Node로 붙이지 않고, Instance Sg의 Root로 추가한다.
		i3Node * pRoot = m_pSg->getCommonSg();

		while( pRoot->GetChildCount() > 0)
		{
			pRoot = pRoot->GetChild( 0);
		}

		pRoot->AddChild( m_pSg->getInstanceSg());

		m_pSg->setInstanceSg( pRoot);
		m_pSg->setCommonSg(nullptr);
	}
}

const char * i3OptBuildCommonSg::getDesc(void)
{
	return "공통 SceneGraph 생성. (Rendering 성능 향상)";
}
