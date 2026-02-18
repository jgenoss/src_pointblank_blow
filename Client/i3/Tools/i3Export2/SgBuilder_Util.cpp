#include "stdafx.h"
#include "SgBuilder.h"
#include "Util.h"
#include "i3Export.h"

#include "i3Base/string/compare/generic_icompare.h"
#include "i3Base/string/ext/generic_string_size.h"

i3Node *	SgBuilder::GetAttachPoint( INode * pINode, i3AttrSet * pHighAttrSet, INode * pSkinRoot)
{
	BONE_INFO * pParentBone;
	i3Node * pAttachNode;

	// 해당 INode에 대응하는 BONE_INFO를 검색한다.
	INT32 idx = findBoneByINode( pINode);

	if( idx == -1)
	{
		// 만약 해당 INode에 대한 것이 없다면...
		if( pSkinRoot == NULL)
		{
			// 일반적인 Node
			// Parent Node들을 따라가며 연결될 수 있는 Bone을 찾는다.
			pParentBone = FindParentBone( pINode, false);
		}
		else
		{
			// Skinning Bone.
			// Skinning된 Bone을 이용해 이미 생성되어 있는 i3Skeleton을 찾는다.
			idx = findBoneByINode( pSkinRoot);

			if( idx != -1)
				pParentBone = getBone( idx);
		}
	}
	else
	{
		// 찾았다면 해당 Bone으로....
		pParentBone = getBone( idx);
	}

	if( pParentBone == NULL)
	{
		// 완전히 Parent가 없는 경우.
		// 실제 있을 수 있다.
		// m_pRoot에 붙인다.
		pAttachNode = m_pRoot;
	}
	else
	{
		pAttachNode = pParentBone->m_pNode;
	}

	if( i3::kind_of<i3Body*>(pAttachNode)) //->IsTypeOf( i3Body::static_meta()))
	{
		// Skin이 된 경우에는 일반적인 Bone Hierarchy를 따르지 않고, Vertex가 Reference하고 있는 Bone을 기준으로
		// Attach 해야한다.
		i3Body * pBody = (i3Body *) pAttachNode;

		i3LOD * pLOD = pBody->getLOD( 0);
		i3Skeleton * pSkel = pLOD->getSkeleton();

		// Geometry를 i3Body에 붙이는 경우에는 LOD에 대한 Shape Node로 붙여야 한다.
		if( pSkinRoot)
		{
			if( pLOD->getShapeNode() == NULL)
			{
				// 기존 Shape Node가 없다.
				if( pHighAttrSet == NULL)
					pAttachNode = i3Node::new_object_ref();
				else
					pAttachNode = pHighAttrSet;

				pLOD->setShapeNode( pAttachNode);
			}
			else
			{
				if( i3::same_of<i3Node*>(pLOD->getShapeNode())) //->IsExactTypeOf( i3Node::static_meta()))
				{
					pAttachNode = pLOD->getShapeNode();
				}
				else
				{
					i3Node * pTemp = i3Node::new_object_ref();

					pTemp->AddChild( pLOD->getShapeNode());
					pLOD->setShapeNode( pTemp);

					if( pHighAttrSet != NULL)
					{
						// 기존의 Shape Ndoe와는 상위 Material을 공유하지 않으므로
						// 새로운 노드를 만들어 그것을 Shape Node로 설정하고.
						// 기존 Shape Node는 새 Node의 Child로 붙인다.
						pTemp->AddChild( pHighAttrSet);

						pAttachNode = pHighAttrSet;
					}
					else
					{
						pAttachNode = pTemp;
					}
				}
			}
		}
		else
		{
			// i3BoneRef로 붙인다.
			if( pSkel != NULL)
			{
				INT32 idxBone = pSkel->FindBoneByName( pINode->GetName());

				if( idxBone != -1)
				{
					i3BoneRef * pBoneRef = FindBoneRef( pBody, idxBone);

					if( pBoneRef != NULL)
					{
						// 기존에 이미 해당 Bone에 대한 i3BoneRef가 있다면 재활용
						pAttachNode = pBoneRef;
					}
					else
					{
						// 새 i3BoneRef Node를 만들어 붙인다.
						pBoneRef = i3BoneRef::new_object_ref();

						char szName[512];
						sprintf( szName, "%s_BoneRef", pINode->GetName());
						pBoneRef->SetName( szName);

						pBoneRef->setBoneIndex( idxBone);

						pBody->AddChild( pBoneRef);

						pAttachNode = pBoneRef;
					}
				}
				else
				{
					m_pExport->Log( pINode, "Bone에 대한 참조를 하고 있지만, 실제 Skeleton에서는 발견되지 않음.");
				}
			}
			else
			{
				m_pExport->Log( pINode, "Bone에 대한 참조를 하고 있지만, i3Body에 Skeleton이 없어 i3BoneRef로 붙이지 못함.");
			}
		}
	}
	else
	{
		if( pHighAttrSet != NULL)
		{
			pAttachNode->AddChild( pHighAttrSet);
			pAttachNode = pHighAttrSet;
		}
	}

	return pAttachNode;
}

i3BoneRef * SgBuilder::FindBoneRef( i3Body * pBody, INT32 idxBone)
{
	INT32 i;

	for( i = 0; i < pBody->GetChildCount(); i++)
	{
		i3Node * pChild = pBody->GetChild( i);

		if( i3::kind_of<i3BoneRef*>(pChild)) //->IsTypeOf( i3BoneRef::static_meta()))
		{
			i3BoneRef * pBoneRef = (i3BoneRef *) pChild;

			if( pBoneRef->getBoneIndex() == idxBone)
				return pBoneRef;
		}
	}

	return NULL;
}

void SgBuilder::CheckSameName(void)
{
	i3::vector<UINT32> nameList;

	_Rec_CheckSameName( m_pIRoot, nameList);
}

void SgBuilder::_Rec_CheckSameName( INode * pNode, i3::vector<UINT32>& List)
{
	INT32 len = i3::generic_string_size( pNode->GetName());

	if( len > 0)
	{
		UINT32 crc = CRC32( 0xFFFFFFFF, (UINT8 *) pNode->GetName(), len);

//		if( pList->IndexOf( (void *) crc) != -1)
		if ( i3::vu::int_index_of(List, crc) != -1)
		{
			m_pExport->Log( pNode, "이름이 동일한 노드가 존재합니다. Animation 및 Skinning 정보 구성에 문제가 발생할 수 있습니다.");
		}
		else
		{
			List.push_back(crc);
		}
	}

	INT32 i;
	INode * pChild;

	for( i = 0; i < pNode->NumberOfChildren(); i++)
	{
		pChild = pNode->GetChildNode( i);

		_Rec_CheckSameName( pChild, List);
	}
}

void SgBuilder::GatherBoneHier( BONE_INFO * pBone, i3::vector<BONE_INFO *>& List)
{
	while( pBone != NULL)
	{
		I3TRACE( "[%s] ", pBone->m_pINode->GetName());
		List.push_back( pBone);

		pBone = (BONE_INFO *) pBone->getParent();
	}

	I3TRACE( "\n");
}


///////////////////////////////////////////////////////////////////////////////////

static I3SG_TRAVERSAL_RESULT _SortNodeProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	i3::vector<i3Node*> TList;
	i3::vector<i3Node*> OList;

	// Child Node들을 분류한다.
	for(INT32 i = 0; i < pNode->GetChildCount(); i++)
	{
		i3Node * pChild = pNode->GetChild( i);

		if( pChild->IsFlag( I3_NODEFLAG_TRANSPARENT))
			TList.push_back( pChild);
		else
			OList.push_back( pChild);
	}

	// Opaque List Sorting
	if ( !OList.empty() )
	{
		for(size_t i = 0; i < (OList.size() - 1); i++)
		{
			for(size_t j = i + 1; j < OList.size(); j++)
			{
				i3Node * pChild0 = OList[i];
				i3Node * pChild1 = OList[j];

				if( i3::generic_icompare( pChild0->GetName(), pChild1->GetName()) > 0)
				{
					OList[j] = pChild0;		// Swap
					OList[i] = pChild1;
				}
			}
		}
	}
	// Transparent List Sorting
	if ( !TList.empty() )
	{
		for(size_t i = 0; i < (TList.size() - 1); i++)
		{
			for(size_t j = i + 1; j < TList.size(); j++)
			{
				i3Node * pChild0 = TList[i];
				i3Node * pChild1 = TList[j];

				if( i3::generic_icompare( pChild0->GetName(), pChild1->GetName()) > 0)
				{
					TList[j] = pChild0;		// Swap
					TList[i] = pChild1;
				}
			}
		}
	}

	// 해당 Child들을 다시 설정.
	{
		INT32 idx = 0;

		for(size_t i = 0; i < OList.size(); i++, idx ++)
		{
			pNode->SetChild( idx, OList[i]);
		}

		for(size_t i = 0; i < TList.size(); i++, idx ++)
		{
			pNode->SetChild( idx, TList[i]);
		}
	}
	
	return I3SG_TRAVERSAL_CONTINUE;
}

void SgBuilder::SortNode(void)
{
	if( m_pRoot == NULL)
		return;

	i3Scene::Traverse( m_pRoot, _SortNodeProc, NULL );
}
