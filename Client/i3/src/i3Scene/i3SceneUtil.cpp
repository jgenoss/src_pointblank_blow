#include "i3SceneDef.h"
#include "i3Base.h"
#include "i3SceneUtil.h"
#include "i3AllNodes.h"
#include "i3AllAttrs.h"
#if defined ( I3_PHYSX)
	#include "i3PhysixDefine.h"
	#include "i3PhysixShapeSet.h"
#endif

i3Node * i3Scene::FindNodeByType( i3Node * pRoot, i3ClassMeta * pMeta, bool bRecursive)
{
	INT32 i;
	i3Node * pResult;

	if( pRoot == nullptr)
		return nullptr;

	if( pRoot->kind_of(pMeta))
		return pRoot;

	if( bRecursive)
	{
		if( i3::same_of<i3Body*>(pRoot))
		{
			i3Body * pBody = (i3Body *) pRoot;

			for( i = 0; i < (INT32) pBody->getLODCount(); i++)
			{
				i3LOD * pLOD = pBody->getLOD( i);

				pResult = FindNodeByType( pLOD->getShapeNode(), pMeta, true);
				if( pResult != nullptr)
					return pResult;
			}
		}
		else if( i3::kind_of< i3TimeSequence* >(pRoot))
		{
			i3TimeSequence * pSeq = (i3TimeSequence *) pRoot;

			for( i = 0; i < pSeq->GetSequenceCount(); i++)
			{
				i3TimeSequenceInfo * pInfo = pSeq->GetSequence( i);

				FindNodeByType( pInfo->GetNode(), pMeta, true);
			}
		}
		else if( i3::same_of<i3BinaryPartitionNode*>(pRoot))
		{
			i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

			if( pBSP->getFront() != nullptr)
				FindNodeByType( pBSP->getFront(), pMeta, true);

			if( pBSP->getBack() != nullptr)
				FindNodeByType( pBSP->getBack(), pMeta, true);
		}
		else if( i3::kind_of<i3OcTreeNode*>(pRoot))
		{
			i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

			for( i = 0; i < 8; i++)
			{
				i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

				if( pChild != nullptr)
				{
					pResult = FindNodeByType( pChild, pMeta, true);
					if( pResult != nullptr)
						return pResult;
				}
			}
		}

		for( i = 0; i < pRoot->GetChildCount(); i++)
		{
			pResult = FindNodeByType( pRoot->GetChild( i), pMeta, true);
			if( pResult != nullptr)
				return pResult;
		}
	}

	return nullptr;
}

void i3Scene::FindNodeByType( i3Node * pRoot, i3ClassMeta * pMeta, i3::vector<i3Node*>& List, bool bRecursive)
{
	if( pRoot == nullptr)
		return;

	if( pRoot->kind_of( pMeta))
		List.push_back( pRoot);

	if( bRecursive)
	{
		INT32 i;

		if( i3::same_of<i3Body* >(pRoot))
		{
			i3Body * pBody = (i3Body *) pRoot;

			for( i = 0; i < (INT32) pBody->getLODCount(); i++)
			{
				i3LOD * pLOD = pBody->getLOD( i);

				FindNodeByType( pLOD->getShapeNode(), pMeta, List, true);
			}
		}
		else if( i3::same_of<i3TimeSequence* >(pRoot))
		{
			i3TimeSequence * pTimeSeq = (i3TimeSequence *) pRoot;

			for( i = 0; i < pTimeSeq->GetSequenceCount(); i++)
			{
				i3TimeSequenceInfo * pInfo = pTimeSeq->GetSequence( i);

				FindNodeByType( pInfo->GetNode(), pMeta, List, true);
			}
		}
		else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
		{
			i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

			if( pBSP->getFront() != nullptr)
				FindNodeByType( pBSP->getFront(), pMeta, List, true);

			if( pBSP->getBack() != nullptr)
				FindNodeByType( pBSP->getBack(), pMeta, List, true);
		}
		else if( i3::kind_of<i3OcTreeNode* >(pRoot))
		{
			i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

			for( i = 0; i < 8; i++)
			{
				i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

				if( pChild != nullptr)
				{
					FindNodeByType( pChild, pMeta, List, true);
				}
			}
		}

		for( i = 0; i < pRoot->GetChildCount(); i++)
		{
			FindNodeByType( pRoot->GetChild( i), pMeta, List, true);
		}
	}
}

i3Node * i3Scene::FindNodeByExactType( i3Node * pRoot, i3ClassMeta * pMeta, bool bRecursive)
{
	if( pRoot == nullptr)
		return nullptr;

	if( pRoot->same_of( pMeta))
		return pRoot;

	if( bRecursive)
	{
		INT32 i;
		i3Node * pResult;

		if( i3::same_of<i3Body* >(pRoot))
		{
			i3Body * pBody = (i3Body *) pRoot;

			for( i = 0; i < (INT32) pBody->getLODCount(); i++)
			{
				i3LOD * pLOD = pBody->getLOD( i);

				pResult = FindNodeByExactType( pLOD->getShapeNode(), pMeta, true);
				if( pResult != nullptr)
					return pResult;
			}
		}
		else if( i3::same_of<i3TimeSequence* >(pRoot))
		{
			i3TimeSequence * pSeq = (i3TimeSequence *) pRoot;

			for( i = 0; i < pSeq->GetSequenceCount(); i++)
			{
				i3TimeSequenceInfo * pInfo = pSeq->GetSequence( i);

				FindNodeByExactType( pInfo->GetNode(), pMeta, true);
			}
		}
		else if( i3::same_of<i3BinaryPartitionNode*>(pRoot))
		{
			i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

			if( pBSP->getFront() != nullptr)
				FindNodeByExactType( pBSP->getFront(), pMeta, true);

			if( pBSP->getBack() != nullptr)
				FindNodeByExactType( pBSP->getBack(), pMeta, true);
		}
		else if( i3::kind_of<i3OcTreeNode*>(pRoot))
		{
			i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

			for( i = 0; i < 8; i++)
			{
				i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

				if( pChild != nullptr)
				{
					pResult = FindNodeByExactType( pChild, pMeta, true);
					if( pResult != nullptr)
						return pResult;
				}
			}
		}

		for( i = 0; i < pRoot->GetChildCount(); i++)
		{
			pResult = FindNodeByExactType( pRoot->GetChild( i), pMeta, true);
			if( pResult != nullptr)
				return pResult;
		}
	}

	return nullptr;
}

void i3Scene::FindNodeByExactType( i3Node * pRoot, i3ClassMeta * pMeta, i3::vector<i3Node*>& List, bool bRecursive)
{
	if( pRoot == nullptr)
		return;

	if( pRoot->same_of( pMeta))
		List.push_back( pRoot);

	if( bRecursive)
	{
		INT32 i;

		if( i3::same_of<i3Body* >(pRoot))
		{
			i3Body * pBody = (i3Body *) pRoot;

			for( i = 0; i < (INT32) pBody->getLODCount(); i++)
			{
				i3LOD * pLOD = pBody->getLOD( i);

				FindNodeByExactType( pLOD->getShapeNode(), pMeta, List, true);
			}
		}
		else if( i3::same_of<i3TimeSequence* >(pRoot))
		{
			i3TimeSequence * pSeq = (i3TimeSequence *) pRoot;

			for( i = 0; i < pSeq->GetSequenceCount(); i++)
			{
				i3TimeSequenceInfo * pInfo = pSeq->GetSequence( i);

				FindNodeByExactType( pInfo->GetNode(), pMeta, List, true);
			}
		}
		else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
		{
			i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

			if( pBSP->getFront() != nullptr)
				FindNodeByExactType( pBSP->getFront(), pMeta, List, true);

			if( pBSP->getBack() != nullptr)
				FindNodeByExactType( pBSP->getBack(), pMeta, List, true);
		}
		else if( i3::kind_of<i3OcTreeNode*>(pRoot))
		{
			i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

			for( i = 0; i < 8; i++)
			{
				i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

				if( pChild != nullptr)
				{
					FindNodeByExactType( pChild, pMeta, List, true);
				}
			}
		}
	
		for( i = 0; i < pRoot->GetChildCount(); i++)
		{
			FindNodeByExactType( pRoot->GetChild(i), pMeta, List, true);
		}
	}
}

void	i3Scene::FindNodeByExactType( i3Node * pRoot, i3ClassMeta * pMeta, I3_SG_TRAVERSE_PROC pProc, void * pData, bool bRecursive)
{
	if( pRoot == nullptr)
		return;

	if( pRoot->same_of( pMeta))
		pProc( pRoot, pData, nullptr);

	if( bRecursive)
	{
		INT32 i;

		if( i3::same_of<i3Body* >(pRoot))
		{
			i3Body * pBody = (i3Body *) pRoot;

			for( i = 0; i < (INT32) pBody->getLODCount(); i++)
			{
				i3LOD * pLOD = pBody->getLOD( i);

				FindNodeByExactType( pLOD->getShapeNode(), pMeta, pProc, pData, true);
			}
		}
		else if( i3::same_of<i3TimeSequence* >(pRoot))
		{
			i3TimeSequence * pSeq = (i3TimeSequence *) pRoot;

			for( i = 0; i < pSeq->GetSequenceCount(); i++)
			{
				i3TimeSequenceInfo * pInfo = pSeq->GetSequence( i);

				FindNodeByExactType( pInfo->GetNode(), pMeta, pProc, pData, true);
			}
		}
		else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
		{
			i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

			if( pBSP->getFront() != nullptr)
				FindNodeByExactType( pBSP->getFront(), pMeta, pProc, pData, true);

			if( pBSP->getBack() != nullptr)
				FindNodeByExactType( pBSP->getBack(), pMeta, pProc, pData, true);
		}
		else if( i3::kind_of<i3OcTreeNode* >(pRoot))
		{
			i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

			for( i = 0; i < 8; i++)
			{
				i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

				if( pChild != nullptr)
				{
					FindNodeByExactType( pChild, pMeta, pProc, pData, true);
				}
			}
		}

		for( i = 0; i < pRoot->GetChildCount(); i++)
		{
			FindNodeByExactType( pRoot->GetChild(i), pMeta, pProc, pData, true);
		}
	}
}

i3Node * i3Scene::FindNodeByName( i3Node * pRoot, const char * pszName, bool bRecursive)
{
	if( pRoot == nullptr)
		return nullptr;

	if( pRoot->hasName())
	{
		if( i3::generic_is_iequal( pRoot->GetNameString(), (char *) pszName) )
			return pRoot;
	}

	if( bRecursive)
	{
		INT32 i;
		i3Node * pResult;

		if( i3::same_of<i3Body* >(pRoot))
		{
			i3Body * pBody = (i3Body *) pRoot;

			for( i = 0; i < (INT32) pBody->getLODCount(); i++)
			{
				i3LOD * pLOD = pBody->getLOD( i);

				pResult = FindNodeByName( pLOD->getShapeNode(), pszName, true);
				if( pResult != nullptr)
					return pResult;
			}
		}
		else if( i3::same_of<i3TimeSequence* >(pRoot))
		{
			i3TimeSequence * pSeq = (i3TimeSequence *) pRoot;

			for( i = 0; i < pSeq->GetSequenceCount(); i++)
			{
				i3TimeSequenceInfo * pInfo = pSeq->GetSequence( i);

				pResult = FindNodeByName( pInfo->GetNode(), pszName, true);
				if( pResult != nullptr)
					return pResult;
			}
		}
		else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
		{
			i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

			if( pBSP->getFront() != nullptr)
			{
				pResult = FindNodeByName( pBSP->getFront(), pszName, true);
				if( pResult != nullptr)
					return pResult;
			}

			if( pBSP->getBack() != nullptr)
			{
				pResult = FindNodeByName( pBSP->getBack(), pszName, true);
				if( pResult != nullptr)
					return pResult;
			}
		}
		else if( i3::kind_of<i3OcTreeNode* >(pRoot))
		{
			i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

			for( i = 0; i < 8; i++)
			{
				i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

				if( pChild != nullptr)
				{
					pResult = FindNodeByName( pChild, pszName, true);
					if( pResult != nullptr)
						return pResult;
				}
			}
		}

		for( i = 0; i < pRoot->GetChildCount(); i++)
		{
			pResult = FindNodeByName( pRoot->GetChild(i), pszName, true);
			if( pResult != nullptr)
				return pResult;
		}
	}

	return nullptr;
}

void i3Scene::FindNodeByName( i3Node * pRoot, const char * pszName, i3::vector<i3Node*>& List, bool bRecursive)
{
	if( pRoot == nullptr)
		return;

	if( pRoot->hasName())
	{
		if( i3::generic_is_iequal( pRoot->GetNameString(), (char *) pszName) )
		{
			List.push_back( pRoot);
		}
	}

	if( bRecursive)
	{
		INT32 i;

		if( i3::same_of<i3Body* >(pRoot))
		{
			i3Body * pBody = (i3Body *) pRoot;

			for( i = 0; i < (INT32) pBody->getLODCount(); i++)
			{
				i3LOD * pLOD = pBody->getLOD( i);

				FindNodeByName( pLOD->getShapeNode(), pszName, List, true);
			}
		}
		else if( i3::same_of<i3TimeSequence* >(pRoot))
		{
			i3TimeSequence * pSeq = (i3TimeSequence *) pRoot;

			for( i = 0; i < pSeq->GetSequenceCount(); i++)
			{
				i3TimeSequenceInfo * pInfo = pSeq->GetSequence( i);

				FindNodeByName( pInfo->GetNode(), pszName, List, true);
			}
		}
		else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
		{
			i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

			if( pBSP->getFront() != nullptr)
				FindNodeByName( pBSP->getFront(), pszName, List, true);

			if( pBSP->getBack() != nullptr)
				FindNodeByName( pBSP->getBack(), pszName, List, true);
		}
		else if( i3::kind_of<i3OcTreeNode* >(pRoot))
		{
			i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

			for( i = 0; i < 8; i++)
			{
				i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

				if( pChild != nullptr)
				{
					FindNodeByName( pChild, pszName, List, true);
				}
			}
		}

		for( i = 0; i < pRoot->GetChildCount(); i++)
		{
			FindNodeByName( pRoot->GetChild(i), pszName, List, true);
		}
	}
}

i3RenderAttr * i3Scene::FindAttrByType( i3Node * pRoot, i3ClassMeta * pMeta, bool bRecursive)
{
	i3RenderAttr * pAttr;
	INT32 i;

	if( pRoot == nullptr)
		return nullptr;

	if( i3::kind_of<i3AttrSet* >(pRoot))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pRoot;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( pAttr->kind_of( pMeta))
			{
				return pAttr;
			}
		}
	}
	else if( i3::same_of<i3Geometry* >(pRoot))
	{
		i3Geometry * pGeo = (i3Geometry *) pRoot;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			pAttr = pGeo->GetGeometryAttr( i);

			if( pAttr->kind_of( pMeta))
			{
				return pAttr;
			}
		}
	}
	else if( i3::same_of<i3Body* >(pRoot))
	{
		i3Body * pBody = (i3Body *) pRoot;

		for( i = 0; i < (INT32) pBody->getLODCount(); i++)
		{
			i3LOD * pLOD = pBody->getLOD( i);

			pAttr = FindAttrByType( pLOD->getShapeNode(), pMeta, bRecursive);
			if( pAttr != nullptr)
				return pAttr;
		}
	}
	else if( i3::same_of<i3Particle* >(pRoot))
	{
		i3Particle * pParticle = (i3Particle *) pRoot;

		if( pParticle->GetMesh() != nullptr)
		{
			pAttr = FindAttrByType( pParticle->GetMesh(), pMeta, bRecursive);

			if( pAttr != nullptr)
				return pAttr;
		}
	}
	else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

		if( pBSP->getFront() != nullptr)
		{
			pAttr = FindAttrByType( pBSP->getFront(), pMeta, true);
			if( pAttr != nullptr)
				return pAttr;
		}

		if( pBSP->getBack() != nullptr)
		{
			pAttr = FindAttrByType( pBSP->getBack(), pMeta, true);
			if( pAttr != nullptr)
				return pAttr;
		}
	}
	else if( i3::kind_of<i3OcTreeNode* >(pRoot))
	{
		i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

		for( i = 0; i < 8; i++)
		{
			i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);
			i3RenderAttr * pAttr;

			if( pChild != nullptr)
			{
				pAttr = FindAttrByType( pChild, pMeta, true);
				if( pAttr != nullptr)
					return pAttr;
			}
		}
	}

	if( bRecursive)
	{
		INT32 i;
		i3RenderAttr * pAttr;

		for( i = 0; i < pRoot->GetChildCount(); i++)
		{
			pAttr = FindAttrByType( pRoot->GetChild(i), pMeta, true);
			if( pAttr != nullptr)
				return pAttr;
		}
	}

	return nullptr;
}

void i3Scene::FindAttrByType( i3Node * pRoot, i3ClassMeta * pMeta, i3::vector<i3RenderAttr*>& List, bool bRecursive)
{
	UINT32 i;

	if( pRoot == nullptr)
		return;

	if( i3::kind_of< i3AttrSet*>(pRoot))
	{
		i3RenderAttr * pAttr;

		i3AttrSet * pAttrSet = (i3AttrSet *) pRoot;		

		for( i = 0; i < (UINT32) pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( pAttr->kind_of( pMeta))
			{
				List.push_back( pAttr);
			}
		}
	}
	else if( i3::same_of<i3Geometry* >(pRoot))
	{
		i3RenderAttr * pAttr;

		i3Geometry * pGeo = (i3Geometry *) pRoot;

		for( i = 0; i < (UINT32) pGeo->GetGeometryAttrCount(); i++)
		{
			pAttr = pGeo->GetGeometryAttr( i);

			if( pAttr->kind_of( pMeta))
			{
				List.push_back( pAttr);
			}
		}
	}
	else if( i3::same_of<i3Particle* >(pRoot))
	{
		i3Particle * pParticle = (i3Particle *) pRoot;

		if( pParticle->GetMesh() != nullptr)
		{
			FindAttrByType( pParticle->GetMesh(), pMeta, List, bRecursive);
		}
	}
	else if( i3::same_of<i3Body* >(pRoot))
	{
		i3Body * pSkin2 = (i3Body *) pRoot;

		for( i = 0; i < pSkin2->getLODCount(); i++)
		{
			i3LOD * pLOD = pSkin2->getLOD( i);

			FindAttrByType( pLOD->getShapeNode(), pMeta, List, bRecursive);
		}
	}
	else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

		if( pBSP->getFront() != nullptr)
			FindAttrByType( pBSP->getFront(), pMeta, List, true);

		if( pBSP->getBack() != nullptr)
			FindAttrByType( pBSP->getBack(), pMeta, List, true);
	}
	else if( i3::kind_of< i3OcTreeNode*>(pRoot))
	{
		i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

		for( i = 0; i < 8; i++)
		{
			i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

			if( pChild != nullptr)
			{
				FindAttrByType( pChild, pMeta, List, true);
			}
		}
	}

	if( bRecursive)
	{
		INT32 i;

		for( i = 0; i < pRoot->GetChildCount(); i++)
		{
			FindAttrByType( pRoot->GetChild(i), pMeta, List, true);
		}
	}
}

void	i3Scene::SortAttrFromEnableAttrToBindAttr( i3Node * pRoot )
{
	UINT32 i;

	if( pRoot == nullptr)
		return;

	if( i3::kind_of< i3AttrSet*>(pRoot))
	{

		i3AttrSet * pAttrSet = (i3AttrSet *) pRoot;		

		i3::vector<i3RenderAttr*>& vecAttr = pAttrSet->GetAttrs();
		
		struct comp
		{
			bool operator()( const i3RenderAttr* l, const i3RenderAttr* r) const
			{
				bool is_enableAttr_left = i3::kind_of<i3AbstractBoolAttr*>(l);
				bool is_enableAttr_right = i3::kind_of<i3AbstractBoolAttr*>(r);
				if ( is_enableAttr_left == is_enableAttr_right)
					return false;
				return ( is_enableAttr_left == true );
			}
		};
		
		i3::stable_sort(vecAttr.begin(), vecAttr.end(), comp());
	}
	else if( i3::same_of<i3Particle* >(pRoot))
	{
		i3Particle * pParticle = (i3Particle *) pRoot;

		if( pParticle->GetMesh() != nullptr)
		{
			SortAttrFromEnableAttrToBindAttr( pParticle->GetMesh() );
		}
	}
	else if( i3::same_of<i3Body* >(pRoot))
	{
		i3Body * pSkin2 = (i3Body *) pRoot;

		for( i = 0; i < pSkin2->getLODCount(); i++)
		{
			i3LOD * pLOD = pSkin2->getLOD( i);

			SortAttrFromEnableAttrToBindAttr( pLOD->getShapeNode() );
		}
	}
	else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

		if( pBSP->getFront() != nullptr)
			SortAttrFromEnableAttrToBindAttr( pBSP->getFront() );

		if( pBSP->getBack() != nullptr)
			SortAttrFromEnableAttrToBindAttr( pBSP->getBack() );
	}
	else if( i3::kind_of< i3OcTreeNode*>(pRoot))
	{
		i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

		for( i = 0; i < 8; i++)
		{
			i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

			if( pChild != nullptr)
			{
				SortAttrFromEnableAttrToBindAttr( pChild );
			}
		}
	}

	{
		for(INT32 i = 0; i < pRoot->GetChildCount(); i++)
		{
			SortAttrFromEnableAttrToBindAttr( pRoot->GetChild(i) );
		}
	}
	
	
}
//
// LOD가 존재할 경우 해당 LOD 아래에 있는 것들만 모아놓는다.
// 
void	i3Scene::FindAttrByType_LOD(i3Node * pRoot, UINT32 LODIndex, i3ClassMeta * pMeta, i3::vector<i3RenderAttr*>& List)
{
	UINT32 i;

	if( pRoot == nullptr)
		return;

	if( i3::same_of<i3Particle* >(pRoot))
	{
		i3Particle * pParticle = (i3Particle *) pRoot;

		if( pParticle->GetMesh() != nullptr)
		{
			FindAttrByType_LOD( pParticle->GetMesh(), LODIndex, pMeta, List);
		}
	}
	else if( i3::same_of<i3Body* >(pRoot))
	{
		i3Body * pSkin2 = (i3Body *) pRoot;
		const UINT32 numLOD = pSkin2->getLODCount();

		if ( LODIndex < numLOD)
		{
			i3LOD* pLOD = pSkin2->getLOD(LODIndex);
			FindAttrByType( pLOD->getShapeNode(), pMeta, List, true);		// 일단 LOD에 진입했으면 몽땅 빨아들이기 위해 FindAttrByType으로 대신 호출..
		}
	}
	else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

		if( pBSP->getFront() != nullptr)
			FindAttrByType_LOD( pBSP->getFront(), LODIndex, pMeta, List);

		if( pBSP->getBack() != nullptr)
			FindAttrByType_LOD( pBSP->getBack(), LODIndex, pMeta, List);
	}
	else if( i3::kind_of< i3OcTreeNode*>(pRoot))
	{
		i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

		for( i = 0; i < 8; i++)
		{
			i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

			if( pChild != nullptr)
			{
				FindAttrByType_LOD( pChild, LODIndex, pMeta, List);
			}
		}
	}

	for(INT32 i = 0; i < pRoot->GetChildCount(); i++)
	{
		FindAttrByType_LOD( pRoot->GetChild(i), LODIndex, pMeta, List);
	}
	
}

//
//   LOD외에 놓인 attr외의 나머지 attr을 모은다.. 
//

void	i3Scene::FindAttrByType_NonLOD(i3Node * pRoot, i3ClassMeta * pMeta, i3::vector<i3RenderAttr*>& List)
{
	UINT32 i;

	if( pRoot == nullptr)
		return;

	if( i3::kind_of< i3AttrSet*>(pRoot))
	{
		i3RenderAttr * pAttr;

		i3AttrSet * pAttrSet = (i3AttrSet *) pRoot;		

		for( i = 0; i < (UINT32) pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( pAttr->kind_of( pMeta))
			{
				List.push_back( pAttr);
			}
		}
	}
	else if( i3::same_of<i3Geometry* >(pRoot))
	{
		i3RenderAttr * pAttr;

		i3Geometry * pGeo = (i3Geometry *) pRoot;

		for( i = 0; i < (UINT32) pGeo->GetGeometryAttrCount(); i++)
		{
			pAttr = pGeo->GetGeometryAttr( i);

			if( pAttr->kind_of( pMeta))
			{
				List.push_back( pAttr);
			}
		}
	}
	else if( i3::same_of<i3Particle* >(pRoot))
	{
		i3Particle * pParticle = (i3Particle *) pRoot;

		if( pParticle->GetMesh() != nullptr)
		{
			FindAttrByType_NonLOD( pParticle->GetMesh(), pMeta, List);
		}
	}
	else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))						// i3Body 노드에서의 LOD처리 조건구현을 제거함..
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

		if( pBSP->getFront() != nullptr)
			FindAttrByType_NonLOD( pBSP->getFront(), pMeta, List);

		if( pBSP->getBack() != nullptr)
			FindAttrByType_NonLOD( pBSP->getBack(), pMeta, List);
	}
	else if( i3::kind_of< i3OcTreeNode*>(pRoot))
	{
		i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

		for( i = 0; i < 8; i++)
		{
			i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

			if( pChild != nullptr)
			{
				FindAttrByType_NonLOD( pChild, pMeta, List);
			}
		}
	}

	for(INT32 i = 0; i < pRoot->GetChildCount(); i++)
	{
		FindAttrByType_NonLOD( pRoot->GetChild(i), pMeta, List);
	}
	
}


UINT32			i3Scene::FindMaxLODCount(i3Node* pRoot)
{
	UINT32 MaxResult = 0;

	if (pRoot == nullptr)
		return 0;
	if( i3::same_of<i3Particle* >(pRoot))
	{
		i3Particle * pParticle = (i3Particle *) pRoot;

		if( pParticle->GetMesh() != nullptr)
		{
			 UINT32 ChildResult = FindMaxLODCount( pParticle->GetMesh() );
			 MaxResult = (ChildResult > MaxResult) ? ChildResult : MaxResult;
		}
	}
	else if( i3::same_of<i3Body* >(pRoot) )
	{
		i3Body * pSkin2 = (i3Body *) pRoot;
		UINT32 ChildResult = pSkin2->getLODCount();
		MaxResult = (ChildResult > MaxResult) ? ChildResult : MaxResult;
	}
	else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

		if( pBSP->getFront() != nullptr)
		{
			UINT32 ChildResult = FindMaxLODCount( pBSP->getFront());
			MaxResult = (ChildResult > MaxResult) ? ChildResult : MaxResult;
		}
		if( pBSP->getBack() != nullptr)
		{
			UINT32 ChildResult = FindMaxLODCount( pBSP->getBack());
			MaxResult = (ChildResult > MaxResult) ? ChildResult : MaxResult;
		}
	}
	else if( i3::kind_of< i3OcTreeNode*>(pRoot))
	{
		i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

		for( INT32 i = 0; i < 8; i++)
		{
			i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

			if( pChild != nullptr)
			{
				UINT32 ChildResult = FindMaxLODCount( pChild);
				MaxResult = (ChildResult > MaxResult) ? ChildResult : MaxResult;
			}
		}
	}

	for(INT32 i = 0; i < pRoot->GetChildCount(); i++)
	{
		UINT32 ChildResult = FindMaxLODCount(pRoot->GetChild(i));
		MaxResult = (ChildResult > MaxResult) ? ChildResult : MaxResult;
	}

	return MaxResult;
}

i3RenderAttr * i3Scene::FindAttrByExactType( i3Node * pRoot, i3ClassMeta * pMeta, bool bRecursive)
{
	INT32 i;
	i3RenderAttr * pAttr;

	if( pRoot == nullptr)
		return nullptr;

	if( i3::same_of<i3AttrSet* >(pRoot))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pRoot;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( pAttr->same_of( pMeta))
			{
				return pAttr;
			}
		}
	}
	else if( i3::same_of<i3Geometry* >(pRoot))
	{
		i3Geometry * pGeo = (i3Geometry *) pRoot;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			pAttr = pGeo->GetGeometryAttr( i);

			if( pAttr->same_of( pMeta))
			{
				return pAttr;
			}
		}
	}
	else if( i3::same_of<i3Particle* >(pRoot))
	{
		i3Particle * pParticle = (i3Particle *) pRoot;

		if( pParticle->GetMesh() != nullptr)
		{
			pAttr = FindAttrByExactType( pParticle->GetMesh(), pMeta, bRecursive);

			if( pAttr != nullptr)
				return pAttr;
		}
	}
	else if( i3::same_of<i3Body* >(pRoot))
	{
		i3Body * pBody = (i3Body *) pRoot;

		for( i = 0; i < (INT32) pBody->getLODCount(); i++)
		{
			i3LOD * pLOD = pBody->getLOD( i);

			pAttr = FindAttrByExactType( pLOD->getShapeNode(), pMeta, bRecursive);
			if( pAttr != nullptr)
				return pAttr;
		}
	}
	else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

		if( pBSP->getFront() != nullptr)
		{
			pAttr = FindAttrByExactType( pBSP->getFront(), pMeta, true);
			if( pAttr != nullptr)
				return pAttr;
		}

		if( pBSP->getBack() != nullptr)
		{
			pAttr = FindAttrByExactType( pBSP->getBack(), pMeta, true);
			if( pAttr != nullptr)
				return pAttr;
		}
	}
	else if( i3::kind_of<i3OcTreeNode* >(pRoot))
	{
		i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

		for( i = 0; i < 8; i++)
		{
			i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

			if( pChild != nullptr)
			{
				pAttr = FindAttrByExactType( pChild, pMeta, true);
				if( pAttr != nullptr)
					return pAttr;
			}
		}
	}

	if( bRecursive)
	{
		for( i = 0; i < pRoot->GetChildCount(); i++)
		{
			pAttr = FindAttrByExactType( pRoot->GetChild(i), pMeta, true);
			if( pAttr != nullptr)
				return pAttr;
		}
	}

	return nullptr;
}

void i3Scene::FindAttrByExactType( i3Node * pRoot, i3ClassMeta * pMeta, i3::vector<i3RenderAttr*>& List, bool bRecursive)
{
	INT32 i;
	i3RenderAttr * pAttr;

	if( pRoot == nullptr)
		return;

	if( i3::same_of<i3AttrSet* >(pRoot))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pRoot;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( pAttr->same_of( pMeta))
			{
				List.push_back( pAttr);
			}
		}
	}
	else if( i3::same_of<i3Geometry* >(pRoot))
	{
		i3Geometry * pGeo = (i3Geometry *) pRoot;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			pAttr = pGeo->GetGeometryAttr( i);

			if( pAttr->kind_of( pMeta))
			{
				List.push_back( pAttr);
			}
		}
	}
	else if( i3::same_of<i3Particle* >(pRoot)) 
	{
		i3Particle * pParticle = (i3Particle *) pRoot;

		if( pParticle->GetMesh() != nullptr)
		{
			FindAttrByExactType( pParticle->GetMesh(), pMeta, List, bRecursive);
		}
	}
	else if( i3::same_of<i3Body* >(pRoot))
	{
		i3Body * pBody = (i3Body *) pRoot;

		for( i = 0; i < (INT32) pBody->getLODCount(); i++)
		{
			i3LOD * pLOD = pBody->getLOD( i);

			FindAttrByExactType( pLOD->getShapeNode(), pMeta, List, bRecursive);
		}
	}
	else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

		if( pBSP->getFront() != nullptr)
			FindAttrByExactType( pBSP->getFront(), pMeta, List, true);

		if( pBSP->getBack() != nullptr)
			FindAttrByExactType( pBSP->getBack(), pMeta, List, true);
	}
	else if( i3::kind_of<i3OcTreeNode* >(pRoot))
	{
		i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

		for( i = 0; i < 8; i++)
		{
			i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

			if( pChild != nullptr)
			{
				FindAttrByExactType( pChild, pMeta, List, true);
			}
		}
	}

	if( bRecursive)
	{
		for( i = 0; i < pRoot->GetChildCount(); i++)
		{
			FindAttrByExactType( pRoot->GetChild(i), pMeta, List, true);
		}
	}
}

// 해당 Node의 Parent를 따라가며, i3TextureBindAttr를 찾는다.
i3RenderAttr *		i3Scene::FindLastAttr( i3Node * pNode, i3ClassMeta * pMeta)
{
	INT32 i;
	i3RenderAttr * pAttr;

	if( pNode == nullptr)
		return nullptr;

	while( pNode != nullptr)
	{
		if( i3::kind_of< i3AttrSet*>(pNode))
		{
			i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

			for( i = 0; i < pAttrSet->GetAttrCount(); i++)
			{
				pAttr = pAttrSet->GetAttr( i);

				if( pAttr->same_of( pMeta))
				{
					return pAttr;
				}
			}
		}

		pNode = pNode->GetParent();
	}

	return nullptr;
}

static i3Node * _Rec_CreateClone( i3Node * pSrc, i3Node * pDest, I3_COPY_METHOD method)
{
	INT32 i, Count, idx;
	i3Node * pNew, * pChild;
//	i3ClassMeta * pMeta;

	for( i = 0, Count = 0; i < pSrc->GetChildCount(); i++)
	{
		pChild = pSrc->GetChild( i);

		if( pChild->IsFlag( I3_NODEFLAG_VOLATILE) == false)
			Count++;
	}

	if( Count > 0)
	{
		i3::vector<i3Node*>& List = pDest->GetChildList();
		List.resize( Count);
	}

	for( i = 0, idx = 0; i < pSrc->GetChildCount(); i++)
	{
		pChild = pSrc->GetChild( i);

		if( pChild->IsFlag( I3_NODEFLAG_VOLATILE))
			continue;

	//	pMeta = pChild->GetMeta();

		pNew = pChild->create_instance(); // (i3Node *) CREATEINSTANCE( pMeta);
		pChild->CopyTo( pNew, method);

		I3_SAFE_ADDREF( pNew);

		pDest->SetChild( idx, pNew);
		pNew->SetParent( pDest);
		idx++;

		_Rec_CreateClone( pChild, pNew, method);
	}

	// i3SceneObject 의 내부에서 Call 하는 BuildBoneList 함수가 올바르게 동작하기 위해서는
	// Child Node가 모두 완성되고 난 뒤에 호출되어야 한다. ???
	//pSrc->CopyTo( pNew, method);

	return pDest;
}

static void _BuildBoneListProc( i3Node * pNode)
{
	INT32 i;

	pNode->OnInitAfterLoad();

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		i3Node * pChild = pNode->GetChild( i);

		_BuildBoneListProc( pChild);
	}
}

i3Node * i3Scene::CreateClone( i3Node * pRoot, I3_COPY_METHOD method)
{
	i3Node * pNew;
//	i3ClassMeta * pMeta;

//	pMeta = pRoot->GetMeta();
	pNew = pRoot->create_instance(); //(i3Node *) CREATEINSTANCE( pMeta);
	pRoot->CopyTo( pNew, method);

	_Rec_CreateClone( pRoot, pNew, method);

	// i3SceneObject Node의 경우, Cloning되고 난 후,
	// 반드시 BuildBoneList() 함수를 호출하여 내부의 Bone List를
	// 재 생성해야 한다.
	_BuildBoneListProc( pNew);

	return pNew;
}

void i3Scene::RemoveRenderAttr( i3Node * pRoot, i3ClassMeta * pMeta, bool bRecursive)
{
	if( i3::same_of<i3AttrSet* >(pRoot))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pRoot;
		INT32 i;
		i3RenderAttr * pAttr;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( pAttr->same_of( pMeta))
			{
				pAttrSet->RemoveAttr( pAttr);
				i--;
			}
		}
	}
	
	if( bRecursive)
	{
		INT32 i;

		if( i3::same_of<i3Body* >(pRoot))
		{
			i3Body * pBody = (i3Body *) pRoot;

			for( i = 0; i < (INT32) pBody->getLODCount(); i++)
			{
				i3LOD * pLOD= pBody->getLOD( i);

				RemoveRenderAttr( pLOD->getShapeNode(), pMeta, true);
			}
		}
		else if( i3::same_of<i3BinaryPartitionNode* >(pRoot))
		{
			i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pRoot;

			if( pBSP->getFront() != nullptr)
				RemoveRenderAttr( pBSP->getFront(), pMeta, true);

			if( pBSP->getBack() != nullptr)
				RemoveRenderAttr( pBSP->getBack(), pMeta, true);
		}
		else if( i3::kind_of<i3OcTreeNode* >(pRoot))
		{
			i3OcTreeNode * pOctree = (i3OcTreeNode *) pRoot;

			for( i = 0; i < 8; i++)
			{
				i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

				if( pChild != nullptr)
				{
					RemoveRenderAttr( pChild, pMeta, true);
				}
			}
		}

		for( i = 0; i < pRoot->GetChildCount(); i++)
		{
			RemoveRenderAttr( pRoot->GetChild(i), pMeta, true);
		}
	}
}

static void *	s_pUserData;
static i3MatrixStack *	s_pMatrixStack;
static I3_SG_TRAVERSE_PROC s_pUserProc;
static UINT32 s_DisableFlag = 0;

static I3SG_TRAVERSAL_RESULT Rec_Traverse( i3Node * pNode)
{
	INT32 i;
	bool bTrans;
	I3SG_TRAVERSAL_RESULT rv;

	if( i3::same_of<i3Body* >(pNode))
	{
		i3Body * pBody = (i3Body *) pNode;

		for( i = 0; i < (INT32) pBody->getLODCount(); i++)
		{
			i3LOD * pLOD = pBody->getLOD( i);

			if( (pLOD != nullptr) && (pLOD->getShapeNode() != nullptr))
				Rec_Traverse( pLOD->getShapeNode());
		}
	}
	else if( i3::same_of<i3BinaryPartitionNode* >(pNode))
	{
		i3BinaryPartitionNode * pBSP = (i3BinaryPartitionNode *) pNode;

		if( pBSP->getFront() != nullptr)
			Rec_Traverse( pBSP->getFront());

		if( pBSP->getBack() != nullptr)
			Rec_Traverse( pBSP->getBack());
	}
	else if( i3::same_of<i3OcTreeNode* >(pNode))
	{
		i3OcTreeNode * pOctree = (i3OcTreeNode *) pNode;

		for( i = 0; i < 8; i++)
		{
			i3Node * pChild = pOctree->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

			if( pChild != nullptr)
			{
				rv = s_pUserProc( pChild, s_pUserData, s_pMatrixStack);

				if( rv == I3SG_TRAVERSAL_CONTINUE)
				{
					rv = Rec_Traverse( pChild);
				}

				if( rv == I3SG_TRAVERSAL_STOP)
					return I3SG_TRAVERSAL_STOP;
			}
		}
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		i3Node * pChild = pNode->GetChild(i);

		bTrans = false;

		if( (pChild->GetFlag() & s_DisableFlag) != 0)
			continue;

		if( s_pMatrixStack != nullptr)
		{
			// Matrix Stack을 Update한다.
			if( pChild->IsFlag( I3_NODEFLAG_TRANSFORM))
			{
				I3ASSERT( i3::kind_of<i3Transform*>(pChild));
				i3Transform * pTrans = (i3Transform *) pChild;

				s_pMatrixStack->PushPre( pTrans->GetCacheMatrix(), pTrans->GetMatrix());
				bTrans = true;
			}
		}

		rv = s_pUserProc( pChild, s_pUserData, s_pMatrixStack);

		if( rv == I3SG_TRAVERSAL_CONTINUE)
		{
			rv = Rec_Traverse( pChild);
		}

		if( bTrans)
		{
			s_pMatrixStack->Pop();
		}

		if( rv == I3SG_TRAVERSAL_STOP)
			return I3SG_TRAVERSAL_STOP;
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Scene::Traverse( i3Node * pRoot, I3_SG_TRAVERSE_PROC pUserProc, void * pUserData, i3MatrixStack * pStack, UINT32 flagMask)
{
	bool bTrans = false;
	I3SG_TRAVERSAL_RESULT rv;

	s_pUserData = pUserData;
	s_pUserProc = pUserProc;
	s_pMatrixStack = pStack;
	s_DisableFlag = flagMask;

	if( pStack != nullptr)
	{
		//pStack->Reset();

		// Matrix Stack이 있다면 Update한다.
		if( pRoot->IsFlag( I3_NODEFLAG_TRANSFORM))
		{
			i3Transform * pTrans = (i3Transform *) pRoot;

			pStack->PushPre( pTrans->GetCacheMatrix(), pTrans->GetMatrix());
			bTrans = true;
		}
	}

	rv = s_pUserProc( pRoot, s_pUserData, s_pMatrixStack);

	if( rv == I3SG_TRAVERSAL_CONTINUE)
		Rec_Traverse( pRoot);

	if( bTrans)
	{
		pStack->Pop();
	}
}

struct __TRAVERSEINFO
{
	i3EventReceiver * m_pReceiver = nullptr;
	INT32			m_GroupIdx = 0;
	INT32			m_Style = 0;
	UINT32			m_GroupMask = 0;
} ;

static I3SG_TRAVERSAL_RESULT _i3Scene_InitColliderSetProc( i3Node * pNode, void * pUserData)
{
	INT32 i;

	__TRAVERSEINFO * pInfo = (__TRAVERSEINFO *) pUserData;

	if( i3::kind_of<i3ColliderSet* >(pNode))
	{
		i3ColliderSet * pCollSet = (i3ColliderSet *) pNode;

		pCollSet->SetEventReceiver( pInfo->m_pReceiver);
		if( pInfo->m_GroupIdx != -1 )
			pCollSet->SetGroup( pInfo->m_GroupIdx); 
		pCollSet->SetStyle( pInfo->m_Style);
	}

#if defined (I3_PHYSX)
	else if( i3::same_of<i3Transform2* >(pNode))
	{
		i3Transform2 * pTrans = (i3Transform2 *) pNode;
		i3PhysixShapeSet * pShapeSet = pTrans->getShapeSet();

		if( pShapeSet != nullptr)
		{
			pShapeSet->setEventReceiver( pInfo->m_pReceiver);
			if( pInfo->m_GroupIdx != -1 )
				pShapeSet->setShapeGroup( static_cast<UINT16>(pInfo->m_GroupIdx));

			pShapeSet->setShapeGroupMask( pInfo->m_GroupMask);
		}
	}
	else if( i3::same_of<i3ShapeSetContainer* >(pNode))
	{
		i3ShapeSetContainer * pContainer = (i3ShapeSetContainer *) pNode;
		i3PhysixShapeSet * pShapeSet = pContainer->getShapeSet();

		if( pShapeSet != nullptr)
		{
			pShapeSet->setEventReceiver( pInfo->m_pReceiver);
			if( pInfo->m_GroupIdx != -1 )
				pShapeSet->setShapeGroup( static_cast<UINT16>(pInfo->m_GroupIdx));

			pShapeSet->setShapeGroupMask( pInfo->m_GroupMask);
		}
	}
	else if( i3::same_of<i3Body* >(pNode))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
		I3ASSERT( pSkel != nullptr);

		for( i = 0; i < pSkel->getBoneCount(); i++)
		{
			if( pSkel->isShapeSet( i))
			{
				i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i);

				pShapeSet->setEventReceiver( pInfo->m_pReceiver);
				if( pInfo->m_GroupIdx != -1 )
					pShapeSet->setShapeGroup( static_cast<UINT16>(pInfo->m_GroupIdx));
				pShapeSet->setShapeGroupMask( pInfo->m_GroupMask);
				//pShapeSet->setGroup( pInfo->m_GroupIdx);
			}
		}
	}
#endif

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Scene::InitColliderSet( i3Node * pRoot, INT32 GrpIdx, UINT32 grpMask, INT32 Style, i3EventReceiver * pReceiver)
{
	__TRAVERSEINFO Info;

	Info.m_pReceiver = pReceiver;
	Info.m_GroupIdx = GrpIdx;
	Info.m_Style = Style;
	Info.m_GroupMask = grpMask;

	Traverse( pRoot, (I3_SG_TRAVERSE_PROC) _i3Scene_InitColliderSetProc, (void *) &Info);
}

void i3Scene::MoveChildren( i3Node * pDest, i3Node * pSrc)
{
	i3Node * pChild;

	while( pSrc->GetChildCount() > 0)
	{
		pChild = pSrc->GetChild( 0);

		pChild->ChangeParent(pDest);
	}
}

void i3Scene::SetMaterial( i3Node * pRoot, COLORREAL * pDiffuse, COLORREAL * pSpecular, COLORREAL * pEmissive, REAL32 shininess, bool bRecursive)
{
	INT32 i;

	if( i3::kind_of<i3AttrSet* >(pRoot)) 
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pRoot;
		i3RenderAttr * pAttr;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( i3::same_of<i3MaterialAttr* >(pAttr))
			{
				i3MaterialAttr * pMat = (i3MaterialAttr *) pAttr;

				if( pDiffuse != nullptr)
					pMat->SetDiffuse( pDiffuse);

				if( pSpecular != nullptr)
					pMat->SetSpecular( pSpecular);

				if( pEmissive != nullptr)
					pMat->SetEmissive( pEmissive);

				if( shininess > 0.0f)
					pMat->SetShininess( shininess);
			}
		}
	}

	if( bRecursive == false)
		return;

	for( i = 0; i < pRoot->GetChildCount(); i++)
	{
		i3Node * pChild = pRoot->GetChild( i);

		SetMaterial( pChild, pDiffuse, pSpecular, pEmissive, shininess, true);
	}
}

struct __I3_MODIFY_DYNAMIC_STATE_INFO
{
	UINT32	m_AddState = 0;
	UINT32	m_RemoveState = 0;
} ;

static I3SG_TRAVERSAL_RESULT _i3Scene_ModifyDynamicStateProc( i3Node * pNode, void * pUserData)
{
#if defined (I3_PHYSX)
	__I3_MODIFY_DYNAMIC_STATE_INFO * pInfo = (__I3_MODIFY_DYNAMIC_STATE_INFO *) pUserData;

	if( i3::same_of<i3Transform2* >(pNode))
	{
		i3Transform2 * pTrans = (i3Transform2 *) pNode;

		pTrans->modifyDynamicState( pInfo->m_AddState, pInfo->m_RemoveState);
	}
	else if( i3::same_of<i3ShapeSetContainer* >(pNode))
	{
		i3ShapeSetContainer * pContainer = (i3ShapeSetContainer *) pNode;
		
		pContainer->ModifyDynamicState( pInfo->m_AddState, pInfo->m_RemoveState);
	}
	else if( i3::same_of<i3Body* >(pNode))
	{
		i3Body * pBody = (i3Body *) pNode;
		if( pBody->getFrameLOD() != nullptr)
		{
			i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
			I3ASSERT( pSkel != nullptr);

			pSkel->modifyDynamicState( pInfo->m_AddState, pInfo->m_RemoveState);
		}
	}
#endif
	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Scene::ModifyDynamicState( i3Node * pRoot, UINT32 addState, UINT32 removeState)
{
	__I3_MODIFY_DYNAMIC_STATE_INFO info;

	info.m_AddState = addState;
	info.m_RemoveState = removeState;

	Traverse( pRoot, (I3_SG_TRAVERSE_PROC) _i3Scene_ModifyDynamicStateProc, (void *) &info);
}

struct __I3_DYNAMIC_BONE_INFO
{
	char * pszName = nullptr;
	VEC3D * pVecLinear = nullptr;
	VEC3D * pVecAngular = nullptr;
	VEC3D *	pVecFBLin = nullptr;
	VEC3D *	pVecFBAng = nullptr;
	REAL32	fLinearDamp = 0.0f;
	REAL32	fAngularDamp = 0.0f;
} ;

static I3SG_TRAVERSAL_RESULT _i3Scene_SetDynamicProc( i3Node * pNode,  void * pUserData )
{
#if defined (I3_PHYSX)
	__I3_DYNAMIC_BONE_INFO * pInfo = (__I3_DYNAMIC_BONE_INFO *) pUserData;

	if( i3::same_of<i3Transform2* >(pNode))
	{
		i3Transform2 * pTrans = (i3Transform2 *) pNode;
		
		if( pInfo->pVecFBLin != nullptr )
			pTrans->setFBLinearVelocity( pInfo->pVecFBLin );
		if( pInfo->pVecFBAng != nullptr )
			pTrans->setFBAngularVelocity( pInfo->pVecFBAng );

		if( pInfo->pVecLinear != nullptr )
			pTrans->setLinearVelocity( pInfo->pVecLinear );
		if( pInfo->pVecAngular != nullptr )
			pTrans->setAngularVelocity( pInfo->pVecAngular );

		if( pInfo->fLinearDamp != pTrans->getLinearDamping() && pInfo->fLinearDamp != 0xFFFFFFFF )
			pTrans->setLinearDamping( pInfo->fLinearDamp );
		if( pInfo->fAngularDamp != pTrans->getAngularDamping() && pInfo->fAngularDamp != 0xFFFFFFFF )
			pTrans->setAngularDamping( pInfo->fAngularDamp );
		
	}
	else if( i3::same_of<i3Body* >(pNode))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
		I3ASSERT( pSkel != nullptr);

		for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
		{
			i3Bone * pBone = pSkel->getBone( i );
			if( pInfo->pszName != nullptr )
			{
				if( i3::generic_is_iequal( pBone->getNameString(), pInfo->pszName ) == false )
					continue;
			}

			if( pSkel->isShapeSet( i ) )
			{
				i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );

				if( pShapeSet == nullptr )
					continue;

				if( pInfo->pVecFBLin != nullptr )
					pShapeSet->setFBLinearVelocity( pInfo->pVecFBLin );
				if( pInfo->pVecFBAng != nullptr )
					pShapeSet->setFBAngularVelocity( pInfo->pVecFBAng );

				if( pInfo->pVecLinear != nullptr )
					pShapeSet->setLinearVelocity( pInfo->pVecLinear );
				if( pInfo->pVecAngular != nullptr )
					pShapeSet->setAngularVelocity( pInfo->pVecAngular );

				if( pInfo->fLinearDamp != pShapeSet->getLinearDamping() && pInfo->fLinearDamp != 0xFFFFFFFF )
					pShapeSet->setLinearDamping( pInfo->fLinearDamp );
				if( pInfo->fAngularDamp != pShapeSet->getAngularDamping() && pInfo->fAngularDamp != 0xFFFFFFFF )
					pShapeSet->setAngularDamping( pInfo->fAngularDamp );
			}
		}
	}
#endif
	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Scene::SetDynamicVelocity( i3Node * pRoot, char * pszName, VEC3D * pFBLin, VEC3D * pFBAng, VEC3D * pLinear, VEC3D * pAngular )
{
	__I3_DYNAMIC_BONE_INFO info;
	info.pszName		= pszName;
	info.pVecLinear		= pLinear;
	info.pVecAngular	= pAngular;
	info.pVecFBAng		= pFBAng;
	info.pVecFBLin		= pFBLin;
	info.fAngularDamp	= (REAL32) 0xFFFFFFFF;
	info.fLinearDamp	= (REAL32) 0xFFFFFFFF;

	Traverse( pRoot, (I3_SG_TRAVERSE_PROC) _i3Scene_SetDynamicProc, (void *) &info );
}

void i3Scene::SetDynamicDamping( i3Node * pRoot, REAL32 fLinear, REAL32 fAngular )
{
	__I3_DYNAMIC_BONE_INFO info;

	info.pszName		= nullptr;
	info.pVecLinear		= nullptr;
	info.pVecAngular	= nullptr;
	info.pVecFBAng		= nullptr;
	info.pVecFBLin		= nullptr;
	info.fLinearDamp	= fLinear;
	info.fAngularDamp	= fAngular;

	Traverse( pRoot, (I3_SG_TRAVERSE_PROC) _i3Scene_SetDynamicProc, (void *) &info );
}

static I3SG_TRAVERSAL_RESULT _i3Scene_SetSleepingProc( i3Node * pNode,  void * pUserData )
{
	bool * pEnable = (bool *) pUserData;
	VEC3D	vZero;
	
#if defined (I3_PHYSX)
	if( i3::same_of<i3Transform2* >(pNode))
	{
		i3Transform2 * pTrans = (i3Transform2 *) pNode;
		
		pTrans->setSleeping( *pEnable );
	}
	else if( i3::same_of<i3Body* >(pNode))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
		I3ASSERT( pSkel != nullptr);

		for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
		{
			if( pSkel->isShapeSet( i ) )
			{
				i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );

				if( pShapeSet == nullptr )
					continue;
				
				pShapeSet->setSleeping( *pEnable );
			}
		}
	}
#endif
	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Scene::SetSleeping( i3Node * pRoot, bool bEnable )
{
	Traverse( pRoot, (I3_SG_TRAVERSE_PROC) _i3Scene_SetSleepingProc, (void *) &bEnable );
}

#if defined (I3_PHYSX)
struct __I3_PAIR_FLAG_INFO
{
	NxShape * pShape = nullptr;
	NxActor * pActor = nullptr;
	UINT32	nFlags = 0;
};

static I3SG_TRAVERSAL_RESULT _i3Scene_SetPairFlagProc( i3Node * pNode, void * pUserData )
{
	INT32	i;
	__I3_PAIR_FLAG_INFO * pInfo = (__I3_PAIR_FLAG_INFO *) pUserData;

	if( i3::same_of<i3Transform2* >(pNode))
	{
		i3Transform2 * pTrans = (i3Transform2 *) pNode;

		i3PhysixShapeSet * pShapeSet = pTrans->getShapeSet();
		if( pShapeSet != nullptr )
		{
			if( pInfo->pActor != nullptr )
			{
				g_pPhysixCtx->setActorPairFlags( pInfo->pActor, pShapeSet->getNxActor(), pInfo->nFlags );
			}

			if( pInfo->pShape != nullptr )
			{
				for( i = 0; i < pShapeSet->getNxShapeCount(); i++ )
					g_pPhysixCtx->setShapePairFlags( pInfo->pShape, pShapeSet->getNxShape( i ), pInfo->nFlags );
			}
		}
	}
	else if( i3::same_of<i3ShapeSetContainer* >(pNode))
	{
		i3ShapeSetContainer * pContainer = (i3ShapeSetContainer *) pNode;
		
		i3PhysixShapeSet * pShapeSet = pContainer->getShapeSet();
		if( pShapeSet != nullptr )
		{
			if( pInfo->pActor != nullptr )
				g_pPhysixCtx->setActorPairFlags( pInfo->pActor, pShapeSet->getNxActor(), pInfo->nFlags );

			if( pInfo->pShape != nullptr )
			{
				for( i = 0; i < pShapeSet->getNxShapeCount(); i++ )
					g_pPhysixCtx->setShapePairFlags( pInfo->pShape, pShapeSet->getNxShape( i ), pInfo->nFlags );
			}
		}
	}
	else if( i3::same_of<i3Body* >(pNode))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
		I3ASSERT( pSkel != nullptr);

		for( i = 0; i < pSkel->getBoneCount(); i++ )
		{
			if( pSkel->isShapeSet( i ) )
			{
				i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
				if( pShapeSet == nullptr )
					continue;

				if( pInfo->pActor != nullptr )
					g_pPhysixCtx->setActorPairFlags( pInfo->pActor, pShapeSet->getNxActor(), pInfo->nFlags );

				if( pInfo->pShape != nullptr )
				{
					for( INT32 j = 0; j < pShapeSet->getNxShapeCount(); j++ )
						g_pPhysixCtx->setShapePairFlags( pInfo->pShape, pShapeSet->getNxShape( j ), pInfo->nFlags );
				}
			}
		}
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3Scene::SetActorPairFlag( i3Node * pRoot, NxActor * pActor, UINT32 nFlags )
{
	__I3_PAIR_FLAG_INFO	Info;
	Info.pActor = pActor;
	Info.pShape = nullptr;
	Info.nFlags	= nFlags;
	Traverse( pRoot, (I3_SG_TRAVERSE_PROC) _i3Scene_SetPairFlagProc, (void *) &Info );
}

void i3Scene::SetShapePairFlag( i3Node * pRoot, NxShape * pShape, UINT32 nFlags )
{
	__I3_PAIR_FLAG_INFO Info;
	Info.pActor	= nullptr;
	Info.pShape	= pShape;
	Info.nFlags	= nFlags;
	Traverse( pRoot, (I3_SG_TRAVERSE_PROC) _i3Scene_SetPairFlagProc, (void *) &Info );
}
#endif

i3Node * i3Scene::BuildSphereMesh( REAL32 radius, bool bTri, bool bLine, UINT32 hdiv, UINT32 vdiv, COLORREAL * pLineColor, COLORREAL * pAmbient, COLORREAL * pDiffuse)
{
	UINT32 i, j, idx;
	REAL32 x, y, z, ystep, scale, sy;
	REAL32 deg, step;
	i3VertexFormat format;
	i3GeometryAttr * pGeoAttr;
	i3Geometry * pGeo;
	i3Node * pRoot;
	i3AttrSet * pLineAttrSet = nullptr;
	i3AttrSet * pTriAttrSet = nullptr;
	i3VertexArray * pVA;
	i3IndexArray * pIA;
	UINT32 VertexCount, VertexPerLineCount, TotalTriCount, TotalLineCount, TriPerLine, BodyLineCount;
	VEC3D vec, norm;
	VEC2D uv;

	I3ASSERT( hdiv >= 3);
	I3ASSERT( vdiv >= 1);

	pRoot = i3Node::new_object();

	if( bTri)
	{
		pTriAttrSet = i3AttrSet::new_object_ref();

		if( pDiffuse != nullptr)
		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			pAttr->Set( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		if( pAmbient != nullptr)
		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();

			if( pDiffuse != nullptr)
				pAttr->SetDiffuse( pDiffuse);
			pTriAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild( pTriAttrSet);
	}

	if( bLine)
	{
		pLineAttrSet = i3AttrSet::new_object_ref();

		if( pLineColor != nullptr)
		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			pAttr->Set( pLineColor);

			pLineAttrSet->AddAttr( pAttr);
		}

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set(false);
			pLineAttrSet->AddAttr( pAttr);
		}

		pRoot->AddChild( pLineAttrSet);
	}


	TotalLineCount = (vdiv + 2);
	BodyLineCount = vdiv;
	VertexPerLineCount = hdiv;

	VertexCount = (VertexPerLineCount * BodyLineCount) + 2;
	TriPerLine = (VertexPerLineCount * 2) * BodyLineCount;
	TotalTriCount = TriPerLine + (hdiv * 2);	// 아래와 위에 Fan 구조로 덮개가 붙는다.

	// 높이 스텝
	ystep = 1.0f / (vdiv + 1);

	// 회전값 스텝
	step = 360.0f / hdiv;

	// Vertex Array
	{
		format.SetHasPosition( true);
		format.SetHasNormal( true);
		format.SetHasColor( false);
		format.SetTextureCoordSetCount( 1);
		format.SetTextureCoordCount( 2);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, VertexCount, 0);
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();

		// 최상/하단 Vertex 2개는 항상 선두에 추가한다.
		i3Vector::Set( &vec, 0.0f, radius * 0.5f, 0.0f);
		pVA->SetPosition( 0, &vec);
		pVA->SetNormal( 0, &vec);
		i3Vector::Set( &uv, 0.0f, 0.0f);
		pVA->SetTextureCoord( 0, 0, &uv);

		i3Vector::Set( &vec, 0.0f, radius * -0.5f, 0.0f);
		pVA->SetPosition( 1, &vec);
		pVA->SetNormal( 1, &vec);
		i3Vector::Set( &uv, 0.0f, 1.0f);

		idx = 2;
		y = ystep;

		for( i = 0; i < BodyLineCount; i++)
		{
			deg = 0.0f;
			sy = i3Math::vcos( y * 2.0f) * 0.5f * radius;
			scale = i3Math::vsin( y * 2.0f) * 0.5f * radius;

			for( j = 0; j < hdiv; j++)
			{
				x = i3Math::sin( I3_DEG2RAD( deg));
				z = i3Math::cos( I3_DEG2RAD( deg));

				i3Vector::Set( &vec, x * scale, sy, z * scale);

				i3Vector::Normalize( &norm, &vec);

				pVA->SetPosition( idx, &vec);
				pVA->SetNormal( idx, &norm);

				i3Vector::Set( &uv, ((REAL32)j / (hdiv - 1)), (((REAL32)i + 1) / (BodyLineCount + 2)));

				pVA->SetTextureCoord( 0, idx, &uv);

				idx++;
				deg += step;
			}

			y += ystep;
		}

		pVA->Unlock();
	}

	// Index Array for Triangle
	if( bTri)
	{
		UINT32 count, sidx, preidx = 0;
		UINT32 i1, i2, i3;
		bool bBody;

		count = ((hdiv * 3) * 2);			//  Caps
		count += (BodyLineCount - 1) * ((hdiv * 2) * 3);

		pIA = i3IndexArray::new_object_ref();
		pIA->Create( count, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;
		sidx = 2;

		// Caps
		for( j = 0; j <= (UINT32) vdiv; j++)
		{
			for( i = 0; i < (UINT32) hdiv; i++)
			{
				bBody = false;

				if( j == 0)
				{
					i1 = 0;
					i2 = sidx + i;
					i3 = sidx + i + 1;

					if( i3 >= (sidx + hdiv))
						i3 = sidx;
				}
				else if( j == vdiv)
				{
					i1 = preidx + i + 1;
					i2 = preidx + i;
					i3 = 1;

					if( i1 >= (preidx + hdiv))
						i1 = preidx;
				}
				else
				{
					i1 = (preidx + i);
					i2 = sidx + i;
					i3 = sidx + i + 1;

					if( i3 >= (sidx + hdiv))
						i3 = sidx;
					bBody = true;
				}

				pIA->SetIndex( idx,	i1);	idx++;
				pIA->SetIndex( idx,	i2);	idx++;
				pIA->SetIndex( idx,	i3);	idx++;

				if( bBody)
				{
					//i1 = i2;
					i2 = i3;
					i3 = preidx + i + 1;

					if( i3 >= (preidx + hdiv))
						i3 = preidx;

					pIA->SetIndex( idx,	i1);	idx++;
					pIA->SetIndex( idx,	i2);	idx++;
					pIA->SetIndex( idx,	i3);	idx++;
				}
			}

			preidx = sidx;

			sidx += hdiv;
			if( sidx == VertexCount)
				sidx = VertexCount - hdiv;
		}

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, count / 3);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pTriAttrSet->AddChild( pGeo);
	}

	// Index Array for Line
	if( bLine)
	{
		UINT32 count, sidx, preidx = 0;
		UINT32 i1, i2, i3, i4;

		count = TotalTriCount * 2 * 2;

		pIA = i3IndexArray::new_object_ref();
		pIA->Create( count, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;
		sidx = 2;

		for( j = 0; j <= (UINT32) vdiv; j++)
		{
			for( i = 0; i < (UINT32) hdiv; i++)
			{
				if( j == 0)
					i1 = 0;
				else if( j == vdiv)
					i1 = 1;
				else
					i1 = (preidx + i);

				i2 = sidx + i;
				i3 = sidx + i;
				i4 = sidx + i + 1;

				if( i4 >= (sidx + hdiv))
					i4 = sidx;

				pIA->SetIndex( idx,	i1);	idx++;
				pIA->SetIndex( idx,	i2);	idx++;
				pIA->SetIndex( idx,	i3);	idx++;
				pIA->SetIndex( idx,	i4);	idx++;
			}

			preidx = sidx;

			sidx += hdiv;
			if( sidx == VertexCount)
				sidx = VertexCount - hdiv;
		}

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, count >> 1);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pLineAttrSet->AddChild( pGeo);
	}

	return pRoot;
}

i3Node * i3Scene::BuildAxisMesh( INT32 hdiv, REAL32 Length, REAL32 rad, bool bTri, bool bLine, COLORREAL * pAxisColor, COLORREAL * pConeColor)
{
	i3Node * pRoot;
	i3AttrSet * pLineRoot;
	i3AttrSet * pConeRoot;
	i3VertexFormat format;
	i3VertexArray * pVA;
	i3IndexArray * pIA;
	i3GeometryAttr * pGeoAttr;
	i3Geometry * pGeo;
	VEC3D vec;
	INT32 i, VertexCount;
	UINT32 idx;

	pRoot = i3Node::new_object();

	pLineRoot = i3AttrSet::new_object_ref();
	pRoot->AddChild( pLineRoot);

	pConeRoot = i3AttrSet::new_object_ref();
	pRoot->AddChild( pConeRoot);

	if( pAxisColor != nullptr)
	{
		i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

		pAttr->Set( pAxisColor);
		pLineRoot->AddAttr( pAttr);
	}

	if( pConeColor != nullptr)
	{
		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

			pAttr->Set( pConeColor);
			pConeRoot->AddAttr( pAttr);
		}

		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();

			pAttr->SetDiffuse( pConeColor);
			pConeRoot->AddAttr( pAttr);
		}
	}

	pGeo = i3Geometry::new_object_ref();
	pLineRoot->AddChild( pGeo);

	format.SetHasPosition( true);
	format.SetHasNormal( false);

	// 몸통 선
	{
		i3VertexArray * pVA;

		pVA = i3VertexArray::new_object_ref();

		pVA->Create( &format, 2, 0);
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();

		i3Vector::Set( &vec, 0.0f, 0.0f, 0.0f);
		pVA->SetPosition( 0, &vec);

		i3Vector::Set( &vec, Length, 0.0f, 0.0f);
		pVA->SetPosition( 1, &vec);

		pVA->Unlock();

		pGeoAttr = i3GeometryAttr::new_object_ref();
		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 1, 0);

		pGeo->AppendGeometryAttr( pGeoAttr);
	}

	pGeo = i3Geometry::new_object_ref();
	pConeRoot->AddChild( pGeo);

	// Vertex Array for Cone
	{
		REAL32 x, y, z, deg, step;
		VEC3D norm, p0, p1;
		i3VertexFormat fmt;

		fmt.SetHasPosition( true);
		fmt.SetHasNormal( true);

		VertexCount = (hdiv * 3) + 1;

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &fmt, VertexCount, 0);		// 전체 둘레를 20등분한다. + (끝점 2개)
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();

		idx = 0;
		x = Length * 0.75f;
		deg = 0.0f;

		REAL32 norm_x = sinf( atan2f( rad, x));

		step = 360.0f / (REAL32) hdiv;

		// 뾰족한 끝 부분.
		for( i = 0; i < hdiv; i++)
		{
			y = i3Math::sin( I3_DEG2RAD( deg));
			z = i3Math::cos( I3_DEG2RAD( deg));

			// Positions
			i3Vector::Set( &p0, Length, 0.0f, 0.0f);
			pVA->SetPosition( i + (hdiv * 0), &p0);

			i3Vector::Set( &p1, x, y * rad, z * rad);
			pVA->SetPosition( i + (hdiv * 1), &p1);			// 좌표값은 같지만, Normal이 다르다.
			pVA->SetPosition( i + (hdiv * 2), &p1);

			// Normal
			{
				i3Vector::Set( &norm, norm_x, y, z);
				i3Vector::Normalize( &norm, &norm);
			}

			pVA->SetNormal( i + (hdiv * 0), &norm);
			pVA->SetNormal( i + (hdiv * 1), &norm);

			i3Vector::Set( &norm, -1.0f, 0.0f, 0.0f);
			pVA->SetNormal( i + (hdiv * 2), &norm);

			deg += step;
		}

		i3Vector::Set( &vec, x, 0.0f, 0.0f);
		pVA->SetPosition( (hdiv * 3), &vec);

		i3Vector::Set( &norm, -1.0f, 0.0f, 0.0f);
		pVA->SetNormal(  (hdiv * 3), &norm);

		pVA->Unlock();
	}

	// Triangles
	if( bTri)
	{
		UINT32 count, sidx;
		UINT32 i1, i2, i3;

		count = (hdiv * 2);			//  Caps

		pIA = i3IndexArray::new_object_ref();
		pIA->Create( count * 3, 0, pVA->GetCount());				// Line List
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		idx = 0;
		sidx = 0;

		// Cone 부분
		for( i = 0; i < hdiv; i++)
		{
			i1 = i + hdiv;
			i2 = i;

			if( i < (hdiv - 1))
				i3 = i1 + 1;
			else
				i3 = 0 + hdiv;

			pIA->SetIndex( idx + 0,	i1);
			pIA->SetIndex( idx + 1,	i2);
			pIA->SetIndex( idx + 2,	i3);

			idx += 3;
		}

		// 바닥 부분
		sidx = (hdiv * 2);
		for( i = 0; i < hdiv; i++)
		{
			i1 = i;
			i2 = hdiv;

			if( i < (hdiv - 1))
				i3 = i1 + 1;
			else
				i3 = 0;

			pIA->SetIndex( idx + 0,	i3 + sidx);
			pIA->SetIndex( idx + 1,	i2 + sidx);
			pIA->SetIndex( idx + 2,	i1 + sidx);

			idx += 3;
		}

		pIA->Unlock();

		// Geometry Attr for Lines
		{
			pGeoAttr = i3GeometryAttr::new_object_ref();

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, count);
			pGeoAttr->SetIndexArray( pIA);
		}

		pGeo->AppendGeometryAttr( pGeoAttr);
	}

	return pRoot;
}

i3Node * i3Scene::BuildBoxMesh( REAL32 size, bool bTri, bool bLine, COLORREAL * pTriColor, COLORREAL * pLineColor)
{
	i3Node * pRoot;
	i3AttrSet * pTriRoot;
	i3AttrSet * pLineRoot;
	i3VertexArray * pVA;

	pRoot = i3Node::new_object();

	{
		VEC3D vmin, vmax;

		i3Vector::Set( &vmax, size * 0.5f, size * 0.5f, size * 0.5f);
		i3Vector::Scale( &vmin, &vmax, -1.0f);

		pVA = i3VertexArray::CreateBox( &vmin, &vmax, false, true, nullptr);
	}

	if( bTri)
	{
		i3Geometry * pGeo;
		i3GeometryAttr * pGeoAttr;

		pTriRoot = i3AttrSet::new_object_ref();
		pRoot->AddChild( pTriRoot);

		if( pTriColor != nullptr)
		{
			// Color Attribute
			{
				i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

				pAttr->Set( pTriColor);
				pTriRoot->AddAttr( pAttr);
			}

			// Material Attribute
			{
				i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();

				pAttr->SetDiffuse( pTriColor);
				pTriRoot->AddAttr( pAttr);
			}
		}

		pGeo = i3Geometry::new_object_ref();
		pTriRoot->AddChild( pGeo);

		pGeoAttr = i3GeometryAttr::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, pVA->GetCount() / 3);		
	}

	if( bLine)
	{
		i3IndexArray * pIA;
		i3Geometry * pGeo;
		i3GeometryAttr * pGeoAttr;

		pLineRoot = i3AttrSet::new_object_ref();
		pRoot->AddChild( pLineRoot);

		// Lighting Disable
		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set(false);
			pLineRoot->AddAttr( pAttr);
		}

		if( pLineColor != nullptr)
		{
			// Color Attribute
			{
				i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

				pAttr->Set( pLineColor);
				pLineRoot->AddAttr( pAttr);
			}
		}

		pGeo = i3Geometry::new_object_ref();
		pLineRoot->AddChild( pGeo);

		pGeoAttr = i3GeometryAttr::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		// Index Array
		pIA = i3IndexArray::new_object_ref();

		pIA->Create( 24, 0, pVA->GetCount());
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		// 위쪽 사각형
		pIA->SetIndex(	0,	17);		// x1, y1, z1 -> x2, y1, z1
		pIA->SetIndex(	1,	11);
		pIA->SetIndex(	2,	11);		// x2, y1, z1 -> x2, y1, z2
		pIA->SetIndex(	3,	10);
		pIA->SetIndex(	4,	10);		// x2, y1, z2 -> x1, y1, z2
		pIA->SetIndex(	5,	4);
		pIA->SetIndex(	6,	4);			// x1, y1, z2 -> x1, y1, z1
		pIA->SetIndex(	7,	17);

		// 아래족 사각형
		pIA->SetIndex(	8,	14);		// x1, y2, z1 -> x2, y2, z1
		pIA->SetIndex(	9,	8);
		pIA->SetIndex(	10,	8);			// x2, y2, z1 -> x2, y2, z2
		pIA->SetIndex(	11,	6);
		pIA->SetIndex(	12,	6);			// x2, y2, z2 -> x1, y2, z2
		pIA->SetIndex(	13,	0);
		pIA->SetIndex(	14,	0);			// x1, y2, z2 -> x1, y2, z1
		pIA->SetIndex(	15,	14);

		// 세로 선 (4개)
		pIA->SetIndex(	16,	17);		// x1, y1, z1 -> x1, y2, z1
		pIA->SetIndex(	17,	14);
		pIA->SetIndex(	18,	11);		// x2, y1, z1 -> x2, y2, z1
		pIA->SetIndex(	19,	8);
		pIA->SetIndex(	20,	7);			// x2, y1, z2 -> x2, y2, z2
		pIA->SetIndex(	21,	6);
		pIA->SetIndex(	22,	1);			// x1, y1, z2 -> x1, y2, z2
		pIA->SetIndex(	23,	0);

		pIA->Unlock();

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 12, 0);
		pGeoAttr->SetIndexArray( pIA);
	}

	return pRoot;
}