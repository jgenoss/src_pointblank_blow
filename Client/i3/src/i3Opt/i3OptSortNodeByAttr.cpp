#include "i3OptPCH.h"
#include "i3OptSortNodeByAttr.h"
#include "i3OptCollapseTransform.h"
#include "i3OptCollapseNode.h"
#include "i3OptBuildOcTree.h"
#include "i3OptMipMapGen.h"
#include "i3OptConvertDXTTexture.h"
#include "i3OptSplitGeometry.h"
#include "i3OptCreateBoundBox.h"

I3_CLASS_INSTANCE( i3OptSortNodeByAttr);

struct ATTR_USED_INFO
{
	//UINT32			nCRCCode;	//Attr의 CRC Code
	const i3::fixed_string*	pMetaName = nullptr;	//CRC대신 사용..
	i3RenderAttr *			pAttr = nullptr;		//CRC가 같더라도 값이 틀린경우의 대표값
	INT32					nUsedCnt = 0;	//사용 횟수
};

struct ALL_ATTRS_AT_GEOMETRY
{
	i3Geometry	*	pGeometry = nullptr;
	i3AttrSet	*	m_pAttr = nullptr;
};

i3OptSortNodeByAttr::i3OptSortNodeByAttr(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

i3OptSortNodeByAttr::~i3OptSortNodeByAttr(void)
{
	I3_SAFE_RELEASE( m_pNewRoot);
}

//같은 attr종류이며 같은 값을 가지고 있는가. 그렇다면 그 attr의 index반환, 그렇지않다면 -1반환
INT32 i3OptSortNodeByAttr::_HasSameAttr( i3RenderAttr * pTargetAttr, i3AttrSet * pAttrSet)
{
	for( INT32 j = 0; j < pAttrSet->GetAttrCount(); ++j)
	{
		i3RenderAttr * pRenderAttr = (i3RenderAttr *)pAttrSet->GetAttr( j);

		if( pRenderAttr->same_of( pTargetAttr->meta()) && (pRenderAttr->IsSame( pTargetAttr)))
		{
			return j;
		}
	}

	return -1;
}

//같은 attr종류이며 같은값을 가진 attr이 존재하는가, 그렇다면 그 usingstate의 index를 반환하고, 그렇지않다면 -1반환
INT32 i3OptSortNodeByAttr::_HasSameAttrUse( i3RenderAttr * pAttr)
{
	INT32	i;
	INT32	nCnt		= INT32(m_AttrUseInfoList.size());
//	UINT32	nCRCCode	= pAttr->GetMeta()->getHashCode();
	const i3::fixed_string& AttrMetaName = pAttr->meta()->class_name();

	for( i = 0; i < nCnt; ++i)
	{
		ATTR_USED_INFO * pUsedInfo = m_AttrUseInfoList[i];
		if( AttrMetaName == *pUsedInfo->pMetaName && pUsedInfo->pAttr->IsSame( pAttr))
		{
			return i;
		}
	}

	return -1;
}

//인자(pAttr)이 사용에 대한 정보를 추가한다.
void i3OptSortNodeByAttr::_AddUsingState( i3RenderAttr * pAttr)
{
	INT32 nSameAttrIndex = _HasSameAttrUse( pAttr);

	if( nSameAttrIndex == -1)
	{
		ATTR_USED_INFO * pUsedInfo = (ATTR_USED_INFO*)i3MemAlloc( sizeof( ATTR_USED_INFO));

		pUsedInfo->pMetaName = &pAttr->meta()->class_name();		// GetMeta()->getHashCode();
		pUsedInfo->nUsedCnt = 1;
		pUsedInfo->pAttr	= pAttr;

		//I3TRACE("ADDUSINGSTATE: %s : %d\n", pAttr->GetMeta()->GetClassName(), pAttr->GetMeta()->getHashCode());

		m_AttrUseInfoList.push_back( pUsedInfo);
	}
	else
	{
		ATTR_USED_INFO * pUsedInfo = m_AttrUseInfoList[nSameAttrIndex];

		pUsedInfo->nUsedCnt++;
	}
}

//주어진 노드의 부모를 모두 뒤져 사용된 attribute들을 하나의 attrset에 담는다.
void i3OptSortNodeByAttr::_GatherAttrList( i3Node * pNode, i3AttrSet * pCurAttr)
{
	i3Node * pParent = pNode->GetParent();
	while( pParent != nullptr)
	{
		if( i3::kind_of<i3AttrSet* >(pParent))
		{
			i3AttrSet * pAttr = (i3AttrSet*)pParent;

			INT32 nAttrCnt = pAttr->GetAttrCount();
			for( INT32 i = 0; i < nAttrCnt; ++i)
			{
				i3RenderAttr	* pParentRenderAttr	= pAttr->GetAttr( i);

				if( _HasSameAttr( pParentRenderAttr, pCurAttr) == -1)
				{
					pCurAttr->AddAttr( pParentRenderAttr);
				}
			}
		}

		pParent = pParent->GetParent();
		if( pParent == getTraceRoot())
		{
			pParent = nullptr;
		}
	}
}

bool i3OptSortNodeByAttr::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::kind_of<i3Geometry* >(pNode))
	{
		i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();

		_GatherAttrList( pNode, pAttrSet);

		if( pAttrSet->GetAttrCount() > 0)
		{
			ALL_ATTRS_AT_GEOMETRY * pInfo = (ALL_ATTRS_AT_GEOMETRY*)i3MemAlloc( sizeof( ALL_ATTRS_AT_GEOMETRY));

			pInfo->m_pAttr		= pAttrSet;
			pInfo->pGeometry	= (i3Geometry*)pNode;

			m_AttrInfoList.push_back( pInfo);
		}
		else
		{
			// 메모리서 삭제
			I3_MUST_ADDREF( pAttrSet);
			I3_MUST_RELEASE( pAttrSet);

			return false;
		}
	}
	else if( i3::kind_of<i3OcTreeNode* >(pNode))
	{
		_Rec_SortNode( pNode);

		{
			i3OptCollapseNode Opt;

			for( INT32 i = 0; i < pNode->GetChildCount(); ++i)
			{
				i3Node * pChildNode = pNode->GetChild( i);

				if( (pChildNode != nullptr) && ( i3::kind_of<i3AttrSet*>(pChildNode) ) )
				{
					Opt.Trace( pChildNode);

					i3Node * pNewNode = Opt.getTraceRoot();

					if( pChildNode != pNewNode)
					{
						pNode->RemoveChild( pChildNode);

						pNode->InsertChild( i, pNewNode);
					}
				}
			}
		}
	}

	return true;
}

INT32 i3OptSortNodeByAttr::_GetMaxUsedItem( i3Node * pNode)
{
	INT32 j;
	INT32 nUsedInfoCnt = INT32(m_AttrUseInfoList.size());
	for(INT32 i = 0; i < nUsedInfoCnt; ++i)
	{
		ATTR_USED_INFO * pChild = m_AttrUseInfoList[i];

		I3MEM_SAFE_FREE( pChild);
	}

	m_AttrUseInfoList.clear();

	INT32 nChildCnt = pNode->GetChildCount();
	for(INT32 i = 0; i < nChildCnt; ++i)
	{
		i3Node * pChildNode = (i3Node*)pNode->GetChild( i);
		if( i3::kind_of< i3AttrSet*>(pChildNode))
		{
			i3AttrSet * pChild = (i3AttrSet* )pChildNode;

			INT32 nAttrCnt = pChild->GetAttrCount();
			for( j = 0; j < nAttrCnt; ++j)
			{
				i3RenderAttr * pAttr = (i3RenderAttr*)pChild->GetAttr( j);

				_AddUsingState( pAttr);
			}
		}
	}

	INT32 nMaxCntIndex	= -1;
	INT32 nMaxNumber	= -1;

	for(size_t i = 0; i < m_AttrUseInfoList.size(); ++i)
	{
		ATTR_USED_INFO * pChild = m_AttrUseInfoList[i];

		if( pChild->nUsedCnt > nMaxNumber)
		{
			nMaxCntIndex = i;
			nMaxNumber = pChild->nUsedCnt;
		}
		else if( pChild->nUsedCnt == nMaxNumber)
		{
			if( (*pChild->pMetaName != i3TextureBindAttr::static_meta()->class_name()) &&
				(*pChild->pMetaName != i3LuxMapBindAttr::static_meta()->class_name()) && 
				(*pChild->pMetaName != i3NormalMapBindAttr::static_meta()->class_name()) && 
				(*pChild->pMetaName != i3SpecularMapBindAttr::static_meta()->class_name()) &&
				(*pChild->pMetaName != i3EmissiveMapBindAttr::static_meta()->class_name()))
			{
				nMaxCntIndex = i;
			}
		}
	}

	return nMaxCntIndex;
}

bool i3OptSortNodeByAttr::_Rec_SortNode( i3Node * pRoot)
{
	if( pRoot == nullptr)
		return false;

	if( i3::kind_of<i3Geometry* >(pRoot))
		return false;

	_SortNodeByAttrUseCount( pRoot);

	for( INT32 i = 0; i < pRoot->GetChildCount(); ++i)
	{
		i3Node * pChild = pRoot->GetChild( i);

		if( pChild != nullptr && i3::kind_of<i3OcTreeNode*>(pChild) == false)
		{
			_Rec_SortNode( pChild);
		}
	}

	return true;
}

bool i3OptSortNodeByAttr::_SortNodeByAttrUseCount( i3Node * pNewRoot)
{
	if( pNewRoot == nullptr || pNewRoot->GetChildCount() <= 0)
		return true;

	INT32 nMostManyUsedItem = _GetMaxUsedItem( pNewRoot);

	if( nMostManyUsedItem != -1)
	{
		ATTR_USED_INFO * pUsedInfo = m_AttrUseInfoList[nMostManyUsedItem];
		
		i3ClassMeta* meta = i3ClassMetaSystem::i()->find_class_meta(*pUsedInfo->pMetaName);
		i3RenderAttr * pNewAttr = (i3RenderAttr *)meta->create_instance();
			//(i3RenderAttr *)CREATEINSTANCE( i3ClassMeta::FindClassMetaByHash( pUsedInfo->nCRCCode));

		pUsedInfo->pAttr->CopyTo( pNewAttr, I3_COPY_INSTANCE);

		i3AttrSet * pAttrSet		= i3AttrSet::new_object();
		i3AttrSet * pExceptAttrSet	= i3AttrSet::new_object();

		pAttrSet->AddAttr( pNewAttr);

		{
			if( i3::same_of<i3TextureEnableAttr*>(pNewAttr)
				&& (((i3TextureEnableAttr*)pNewAttr)->Get() == true))
			{
				i3TextureFilterAttr * pFilterAttr = i3TextureFilterAttr::new_object_ref();
				pFilterAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT);

				pAttrSet->AddAttr( pFilterAttr);
			}
		}

		_DivideChildByAttr( pAttrSet, pExceptAttrSet, pUsedInfo->pAttr, pNewRoot);

		{
			if( pExceptAttrSet->GetChildCount() <= 0)
			{
				if( i3::kind_of<i3AttrSet*>(pNewRoot))
				{
					INT32 i;
					i3AttrSet * pParentAttrSet = (i3AttrSet*)pNewRoot;

					for( i = 0; i < pAttrSet->GetAttrCount(); ++i)
					{
						i3RenderAttr * pNewChildAttr = pAttrSet->GetAttr( i);

						I3_MUST_ADDREF(pNewChildAttr);

						pAttrSet->RemoveAttr( pNewChildAttr);
						pParentAttrSet->AddAttr( pNewChildAttr);

						I3_MUST_RELEASE(pNewChildAttr);

						--i;
					}

					for( i = 0; i < pAttrSet->GetChildCount(); ++i)
					{
						i3Node * pNewChildNode = pAttrSet->GetChild( i);
						pNewChildNode->ChangeParent(pNewRoot);

						--i;
					}

					I3_SAFE_RELEASE( pAttrSet);
					I3_SAFE_RELEASE( pExceptAttrSet);

					return _SortNodeByAttrUseCount( pNewRoot);
				}
				else
				{
					pNewRoot->AddChild( pAttrSet);

					I3_SAFE_RELEASE( pAttrSet);
					I3_SAFE_RELEASE( pExceptAttrSet);
				}
			}
			else
			{
				pNewRoot->AddChild( pAttrSet);
				pNewRoot->AddChild( pExceptAttrSet);

				I3_SAFE_RELEASE( pAttrSet);
				I3_SAFE_RELEASE( pExceptAttrSet);
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool i3OptSortNodeByAttr::_DivideChildByAttr( i3AttrSet * Parent, i3AttrSet * pExecptParent, i3RenderAttr * pAttr, i3Node * pPrevParent)
{
	for( INT32 i = 0; i < pPrevParent->GetChildCount(); ++i)
	{
		i3Node * pNode = pPrevParent->GetChild( i);
		if( i3::kind_of<i3AttrSet*>(pNode))
		{
			i3AttrSet * pAttrSet = (i3AttrSet*)pNode;

			I3_MUST_ADDREF(pAttrSet);

			INT32 nSameAttrIndex = _HasSameAttr( pAttr, pAttrSet);

			pPrevParent->RemoveChild( pAttrSet);

			if( nSameAttrIndex != -1)
			{
				i3RenderAttr * pSameAttr = ( i3RenderAttr*)pAttrSet->GetAttr( nSameAttrIndex);
				pAttrSet->RemoveAttr( pSameAttr);

				Parent->AddChild( pAttrSet);
			}
			else
			{
				pExecptParent->AddChild( pAttrSet);
			}

			I3_MUST_RELEASE(pAttrSet);

			--i;
		}
	}

	return true;
}

void i3OptSortNodeByAttr::Trace( i3Node * pRoot)
{
	I3_SAFE_RELEASE( m_pNewRoot);

	m_pNewRoot = i3Node::new_object();

	{
		CallOptimizer( i3OptCollapseTransform::static_meta());
	}

	{
		CallOptimizer( i3OptCollapseNode::static_meta());
	}

	m_AttrUseInfoList.clear();

	switch( m_Phase)
	{
		case PHASE_COMMON_SG :
			i3SceneOptimizer::Trace( m_pSg->getCommonSg());
			break;

		case PHASE_INSTANCE_SG :
			i3SceneOptimizer::Trace( m_pSg->getInstanceSg());
			break;
	}

	{
		
		for(size_t i = 0; i < m_AttrInfoList.size(); ++i)
		{
			ALL_ATTRS_AT_GEOMETRY * pInfo = m_AttrInfoList[i];

			i3AttrSet * pAttrSet = pInfo->m_pAttr;
			pInfo->pGeometry->ChangeParent(pAttrSet);

			m_pNewRoot->AddChild( pAttrSet);

			I3MEM_SAFE_FREE( pInfo);
		}

		m_AttrInfoList.clear();
	}

	SetTraceRoot( m_pNewRoot);

	{
		_Rec_SortNode( m_pNewRoot);

		{
			CallOptimizer( i3OptCollapseNode::static_meta());
		}
	}

	{
		CallOptimizer( i3OptCreateBoundBox::static_meta());
	}
}
