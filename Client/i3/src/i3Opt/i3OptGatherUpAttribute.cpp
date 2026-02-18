#include "i3OptPCH.h"
#include "i3OptGatherUpAttribute.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3OptGatherUpAttribute);

struct I3OPT_ATTR_INFO
{
	i3Node *		m_pGatherNode = nullptr;
	i3RenderAttr *	m_pAttr = nullptr;
	i3Node *		m_pPivotNode = nullptr;
};


i3OptGatherUpAttribute::i3OptGatherUpAttribute(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_pMeta = i3TextureBindAttr::static_meta();
}

void i3OptGatherUpAttribute::_RemoveAllAttrInfo(void)
{
	I3OPT_ATTR_INFO * pInfo;

	for(size_t i = 0; i < m_AttrInfoList.size(); i++)
	{
		pInfo = m_AttrInfoList[i];

		delete pInfo;
	}

	m_AttrInfoList.clear();
}

INT32 i3OptGatherUpAttribute::_FindAttrInfoByAttr( i3RenderAttr * pAttr)
{
	I3OPT_ATTR_INFO * pInfo;

	for(size_t i = 0; i < m_AttrInfoList.size(); i++)
	{
		pInfo = m_AttrInfoList[i];

		if( pInfo->m_pAttr->IsSame( pAttr))
			return i;
	}

	return -1;
}

bool i3OptGatherUpAttribute::_IsParentOfDifferAttr( i3Node * pNode, i3RenderAttr * pAttr)
{
	INT32 i;

	if( i3::kind_of<i3AttrSet*>(pNode))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;
		i3RenderAttr * pTempAttr;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pTempAttr = pAttrSet->GetAttr( i);

			if( pTempAttr->kind_of( pAttr->meta()))
			{
				if( pTempAttr->IsSame( pAttr) == false)
					return true;
			}
		}
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		i3Node * pChild = pNode->GetChild( i);
		bool bRv;

		bRv = _IsParentOfDifferAttr( pChild, pAttr);
		if( bRv == true)
			return bRv;
		
	}

	return false;
}

i3Node * i3OptGatherUpAttribute::_FindPivotNode( i3Node * pNode, i3RenderAttr * pAttr)
{
	while((pNode->GetParent() != nullptr) && (pNode != getTraceRoot()))
	{
		pNode = pNode->GetParent();

		if( i3::kind_of<i3Transform*>(pNode))
		{
			// i3Transform Node가 발견되면 그 즉시 Return.
			return pNode;
		}
		
		if( _IsParentOfDifferAttr( pNode, pAttr))
		{
			return pNode;
		}
	}

	return pNode;
}

void i3OptGatherUpAttribute::_FindAttrGroupByTx( i3Node * pNode, i3::vector<I3OPT_ATTR_INFO*>& List)
{
	
	I3OPT_ATTR_INFO * pInfo;

	for(size_t i = 0; i < m_AttrInfoList.size(); i++)
	{
		pInfo = m_AttrInfoList[i];

		if( pInfo->m_pPivotNode == pNode)
		{
			List.push_back( pInfo);
		}
	}
}

void i3OptGatherUpAttribute::_FindAttrGroupByValueAndPivot( i3RenderAttr * pAttr, i3Node * pPivotNode, i3::vector<I3OPT_ATTR_INFO*>& List)
{

	I3OPT_ATTR_INFO * pInfo;

	for(size_t i = 0; i < m_AttrInfoList.size(); i++)
	{
		pInfo = m_AttrInfoList[i];

		if( pInfo->m_pAttr->IsSame( pAttr) && (pInfo->m_pPivotNode == pPivotNode))
		{
			List.push_back(pInfo);
		}
	}
}

bool i3OptGatherUpAttribute::OnNode( i3Node * pNode, i3Node * pParent)
{
	INT32 i;
	i3RenderAttr * pAttr;

	if( i3::kind_of<i3Transform*>(pNode))
	{
		m_TxList.push_back(static_cast<i3Transform*>(pNode));	
	}
	else if( i3::kind_of<i3AttrSet* >(pNode))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( pAttr->same_of( m_pMeta))
			{
				//if( _FindAttrInfoByAttr( pAttr) == -1)
				{
					// Attr Info를 생성하여 추가한다.
					I3OPT_ATTR_INFO * pInfo = new I3OPT_ATTR_INFO;

					pInfo->m_pGatherNode = pNode;
					pInfo->m_pAttr = pAttr;
					pInfo->m_pPivotNode = _FindPivotNode( pAttrSet, pAttr);

					m_AttrInfoList.push_back( pInfo);
				}
			}
		}
	}

	return true;
}

static i3Node * _FindDirectChild( i3Node * pNode, i3Node * pParent)
{
	if( pNode == pParent)
		return nullptr;

	while((pNode != nullptr) && ( pParent->IsChild( pNode) == false))
	{
		pNode = pNode->GetParent();
	}

	return pNode;
}

void i3OptGatherUpAttribute::Trace( i3Node * pRoot)
{
	i3::vector<I3OPT_ATTR_INFO*> GroupList;
	
	if( m_pMeta == nullptr)
		return;

	m_TxList.clear();
	_RemoveAllAttrInfo();

	// 우선 Scene Graph를 돌면서, 해당 Attribute 정보와
	// i3Transform 노드에 대한 리스트를 작성한다.
	i3SceneOptimizer::Trace( pRoot);

	// 동일한 i3Transform Node를 Pivot으로 하는 AttrInfo들의 그룹을
	// 검색하여, 그들 모두가 동일한 Attr 값을 가지고 있다면 그 상위로
	// 다시 Pivot Node를 검색한다.
	for(size_t i = 0; i < m_TxList.size(); i++)
	{
		i3Transform * pTrans = m_TxList[i];
		bool bSame;
		I3OPT_ATTR_INFO * pSrc, *pDest;

		GroupList.clear();

		_FindAttrGroupByTx( pTrans, GroupList);

		if( GroupList.size() <= 0)
			continue;

		// 모두 동일한 값을 가진 Attr인지 확인한다.
		bSame = true;
		pSrc = GroupList[0];

		for(size_t j = 1; j < GroupList.size(); j++)
		{
			pDest = GroupList[j];

			if( pDest->m_pAttr->IsSame( pSrc->m_pAttr) == false)
			{
				bSame = false;
				break;
			}
		}

		if( bSame == false)
			continue;

		// 해당 i3Transform 아래에는 모두 동일한 값의 Attr만이 존재하기 때문에
		// 더욱 상위로 GatherUp할 수 있다.
		// 그러므로, 새로운 Pivot Node를 검색해 설정한다.
		pSrc->m_pPivotNode = _FindPivotNode( pTrans, pSrc->m_pAttr);
		for(size_t j = 1; j < GroupList.size(); j++)
		{
			pDest = GroupList[j];
			pDest->m_pPivotNode = pSrc->m_pPivotNode;
		}

		if( i3::kind_of<i3Transform*>(pSrc->m_pPivotNode)
			&& (pSrc->m_pPivotNode != getTraceRoot()))
		{
			// 새 Pivot Node가 역시 i3Transform 노드이면서 Root가 아니라면
			// 새롭게 더욱 상위로 GatherUp될 가능성이 있기 때문에
			// 다시 추가
		//	if( m_TxList.IndexOf( pSrc->m_pPivotNode) < i)
			size_t vec_idx = i3::vu::index_of(m_TxList, pSrc->m_pPivotNode);
			if (vec_idx < i || vec_idx == m_TxList.size() )
			{
				m_TxList.push_back( static_cast<i3Transform*>(pSrc->m_pPivotNode));
			}
		}
	}

	INT32 GrpCnt = 0;

	// 최대한 GatherUp할 수 있는 위치인 Pivot Node가 결정되었으므로,
	// 동일한 값을 가진 Attr들을 검색해 GatherUp Node를 결정한다.
	for( ; m_AttrInfoList.size() > 0;)
	{
		I3OPT_ATTR_INFO * pInfo = m_AttrInfoList[0];
		i3Node * pChild, * pPivotNode;
		i3AttrSet * pNewAttrSet, *pOldAttrSet;

		GroupList.clear();

		// 동일한 값과 동일한 Pivot Node를 가진 AttrInfo를 찾는다.
		_FindAttrGroupByValueAndPivot( pInfo->m_pAttr, pInfo->m_pPivotNode, GroupList);

		if( GroupList.size() <= 1)
		{
//			m_AttrInfoList.Delete( 0);
			m_AttrInfoList.erase(m_AttrInfoList.begin());
			continue;
		}

		GrpCnt++;

		// 새로운 GatherUp Node의 생성.
		pNewAttrSet = i3AttrSet::new_object_ref();
		{
			char szName[ MAX_PATH];
			sprintf( szName, "GatherUp %s", m_pMeta->class_name().c_str());
			pNewAttrSet->SetName( szName);
		}

		pNewAttrSet->AddAttr( pInfo->m_pAttr);

		pPivotNode = pInfo->m_pPivotNode;

		for(size_t i = 0; i < GroupList.size(); i++)
		{
			pInfo = GroupList[i];

			pOldAttrSet = (i3AttrSet *) pInfo->m_pGatherNode;

			// Pivot Node에 직접 붙은 조상 노드를 찾는다.
			pChild = _FindDirectChild( pInfo->m_pGatherNode, pPivotNode);
			
			if( pChild != nullptr)
			{
				// Relink
				pChild->ChangeParent(pNewAttrSet);
			}

			// 이전 Attribute의 제거.
			pOldAttrSet->RemoveAttr( pInfo->m_pAttr);

			// 해당 AttrInfo를 제거
			//m_AttrInfoList.Remove( pInfo);
			i3::vu::remove(m_AttrInfoList, pInfo);
			delete pInfo;
		}

		pPivotNode->AddChild( pNewAttrSet);
	}

	if( m_AttrInfoList.size() > 0)
	{
		I3TRACE( "There is leaf info.\n");
	}

	_RemoveAllAttrInfo();
}