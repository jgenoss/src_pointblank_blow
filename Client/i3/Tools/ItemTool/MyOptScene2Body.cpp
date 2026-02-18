#include "StdAfx.h"
#include "MyOptScene2Body.h"

cMyOptScene2Body::cMyOptScene2Body(void)
:m_nCnt(0)
{
}

cMyOptScene2Body::~cMyOptScene2Body(void)
{
}

void cMyOptScene2Body::_Scene2Body( i3Node * pNode, i3Body * pBody, i3LOD * pLOD)
{
	i3::vector<i3Node*> TransList;
	i3Node  * pClone;

	// Bone Hierarchy를 검색한다.
	i3Scene::FindNodeByType( pNode, i3Transform::static_meta(), TransList);

	std::vector<i3Node*> pTmpList;

	for (UINT32 i = 0; i < TransList.size(); ++i)
	{
		pTmpList.push_back(TransList[i]);
	}

	// I3_NODEFLAG_CONTROL이 있는 Node는 제거
	{
		for(UINT32 i = 0; i < TransList.size(); ++i)
		{
			i3Node * pTemp = TransList[i];

			CString tmp = pTemp->GetName();

			//if (tmp == "MagazineAnimatonDummy_Master")
			//	int a = 0;

			I3TRACE("Start : ");
			I3TRACE(pTemp->GetName());
			I3TRACE("\n");

			if( i3::same_of<i3Transform2*>(pTemp)) //->IsExactTypeOf( i3Transform2::GetClassMeta() ) ) //&& pTemp->IsFlag( I3_NODEFLAG_STATIC ) )
			{
				I3TRACE("Continue : ");
				I3TRACE(pTemp->GetName());
				I3TRACE("\n");
				continue;
			}
			else if( pTemp->IsFlag( I3_NODEFLAG_CONTROL | I3_NODEFLAG_STATIC) )
			{
				//TransList.Delete( i);
				//I3_SAFE_RELEASE(pTemp);
				I3TRACE("Delete Transform : ");
				I3TRACE(pTemp->GetName());
				I3TRACE("\n");

				TransList.erase(TransList.begin() + i);
				i--;

				I3TRACE("Delete Success : ");
				I3TRACE(pTemp->GetName());
				I3TRACE("\n");
			}
		}
	}

	if( TransList.size() > 0)
	{
		// i3Skeleton을 생성한다.
		i3Skeleton * pSkel = i3Skeleton::new_object_ref();

		m_BoneIndex = 0;

		pSkel->SetBoneCount( INT32(TransList.size() ) );
		pLOD->setSkeleton( pSkel);

		_Rec_BuildBone( pNode, 0, pSkel, pLOD);
	}

	// 전체 Scene의 Clone을 만들고, 그 중에 Bone으로 사용된 i3Transform에 대해서만
	// i3BoneRef Node로 교체한다.
	{
		// 		i3Node * pNewClone = NULL;
		// 
		// 		pClone = i3Scene::CreateClone( pNode, I3_COPY_INSTANCE);
		// 
		// 		//pNewClone = _Rec_RemoveTransform( pClone, pLOD);
		// 
		// 		pLOD->setShapeNode( pClone);
		// 
		// 		if( pClone != pNewClone )
		// 		{	
		// 			if (pNewClone)
		// 				pNewClone->Release();
		// 		}

		pClone = i3Scene::CreateClone( pNode, I3_COPY_REF);
		pClone->AddRef();
		
		i3Node * pNewClone = _My_Rec_RemoveTransform( pClone, pLOD);
		//pNewClone->Release();
		
		pLOD->setShapeNode( pNewClone);
 		if (pNewClone->GetRefCount() > 1)
 			pNewClone->Release();
// 
// 		if (pClone && pClone != pNewClone)
// 		{
// 			if (pClone->GetRefCount() == 0)
// 				pClone->AddRef();
// 			pClone->Release();
// 			pClone = NULL;
// 		}

		//if( pClone != pNewClone )
		{	
// 			if (pClone)
// 			{
// 				if (pClone->GetRefCount() == 0)
// 					pClone->AddRef();
// 				pClone->Release();
// 			}
		}
	}
}


i3Node * cMyOptScene2Body::_My_Rec_RemoveTransform( i3Node*& pNode, i3LOD * pLOD)
{
	INT32 i;
	i3Node * pChild, * pResult = pNode;
	//bool bRecurs = true;
	i3Node * pParent = pNode->GetParent();

	if( i3::kind_of<i3Transform*>(pNode) //->IsTypeOf( i3Transform::GetClassMeta()) 
		|| i3::kind_of<i3Transform2*>(pNode)) //->IsTypeOf( i3Transform2::GetClassMeta()))
	{
		if( pNode->IsFlag( I3_NODEFLAG_CONTROL))
		{
			// 필요없기 때문에 제거한다.
			if( pParent == NULL)
			{
				if( pNode->GetChildCount() > 0)
				{
					if( pNode->GetChildCount() == 1)
					{
						pResult = pNode->GetChild( 0);
						pResult->AddRef();
					}
					else
					{
						// 빈 Node를 만들고 현재 노드를 대신하게 한다 .
						pResult = i3Node::new_object();

						i3Scene::MoveChildren( pResult, pNode);
					}

					//pNode->AddRef();
					pNode->Release();
					pNode = NULL;

					pResult = _My_Rec_RemoveTransform( pResult, pLOD);

					return pResult;
				}
			}
			else
			{
				i3Scene::MoveChildren( pParent, pNode);

				pParent->RemoveChild( pNode);

				return NULL;
			}
		}
		else if( pNode->IsFlag( I3_NODEFLAG_STATIC) )
		{
			// 일반 다른 Node와 동일하게 간주한다.

			if( i3::same_of<i3Transform2*>(pNode)) //->IsExactTypeOf( i3Transform2::GetClassMeta() ) )
			{
				i3BoneRef * pBoneRef = i3BoneRef::new_object();
				INT32 idxBone;

				idxBone = pLOD->getSkeleton()->FindBoneByName( (char *) pNode->GetName());
				I3ASSERT( idxBone != -1);

				pBoneRef->SetName( pNode->GetName() ); // szName);
				pBoneRef->setBoneIndex( idxBone);

				if( pParent != NULL)
				{
					// Transform2는 이미 본의 매트릭스 값이 있습니다.
					pNode->AddRef();
					pParent->RemoveChild( pNode);

					for( i = 0; i < pNode->GetChildCount(); i++ )
						pBoneRef->AddChild( pNode->GetChild( i ) );

					pParent->AddChild( pBoneRef);

					pNode->Release();
					pNode = NULL;

					return NULL;
				}
				else
				{
					pResult = pBoneRef;

					// Transform2는 이미 본의 매트릭스 값이 있습니다.
					for( i = 0; i < pNode->GetChildCount(); i++ )
						pBoneRef->AddChild( pNode->GetChild( i ) );

					pNode->AddRef();
					pNode->Release();
					pNode = NULL;
				}
			}
		}
		else
		{
			if( pNode->GetChildCount() == 0 )
			{
				pNode->SetFlag( I3_NODEFLAG_STATIC );

				if( pParent != NULL )
				{	
					pNode->RemoveFromParent();
					pNode = NULL;

					return NULL;
				}
			}

			// 이외의 Transform Node는 Bone으로 간주되었을 것이기 때문에
			// i3BoneRef Node로 대체한다.
			i3BoneRef * pBoneRef = i3BoneRef::new_object();
			
			INT32 idxBone;

			// 이하의 Child들이 Bone 이외의 Geometry 등을 포함한 Node가 있는지 확인하고 그것에
			// 대해서만 처리한다.
			// 왜냐하면 Bone으로 대체된 Child들은 추가할 필요가 없기 때문...
			idxBone = pLOD->getSkeleton()->FindBoneByName( (char *) pNode->GetName());
			I3ASSERT( idxBone != -1);

			pBoneRef->SetName( pNode->GetName() ); // szName);
			pBoneRef->setBoneIndex( idxBone);

			for( i = 0; i < pNode->GetChildCount(); i++ )
			{	
				pChild = pNode->GetChild( i );

				i3Node * pReturn = _My_Rec_RemoveTransform( pChild, pLOD);

				if( pReturn != NULL )
				{

					//m_mTmpBone[pNode->GetName()].push_back(pReturn);
					pBoneRef->AddChild( pReturn );
					//if (pReturn == pChild)
					//	pChild->Release();
				}
				else
					i--;
			}

			//차일드를 정리하고 나면..새로만든 본으로 교체
			if( pParent != NULL)
			{	
				pNode->RemoveFromParent();
				pNode = NULL;
				pParent->AddChild( pBoneRef);
				pBoneRef->Release();
				return NULL;
			}
			else
			{
				pResult->Release();
				pResult = NULL;
				if (pBoneRef->GetRefCount() > 1)
					pBoneRef->Release();
				pResult = pBoneRef;
			}
		}
	}
	
// 	for( i = 0; i < pNode->GetChildCount(); i++)
// 	{
// 		pChild = pNode->GetChild( i);
// 
// 		i3Node * pReturn = _My_Rec_RemoveTransform( pChild, pLOD);
// 		if( pReturn == NULL )
// 		{
// 			i--;
// 		}
// 	}

	return pResult;
}