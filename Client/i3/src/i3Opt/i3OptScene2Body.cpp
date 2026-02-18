#include "i3OptPCH.h"
#include "i3OptScene2Body.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/itl/algorithm/sort.h"

I3_CLASS_INSTANCE( i3OptScene2Body);

i3OptScene2Body::i3OptScene2Body(void)
{
	m_Class = CLASS_SCENEGRAPH;
}

/*
static INT32 _SortByNameProc( i3Node * p1, i3Node * p2)
{
	return _stricmp( p1->GetName(), p2->GetName());
}
*/

namespace
{
	bool _SortByNameProc( const i3Node * p1, const i3Node * p2)
	{
		return _stricmp( p1->GetName(), p2->GetName()) < 0;
	}
}


void i3OptScene2Body::_Skin2Body( i3Skin * pSkin, i3Body * pBody, i3LOD * pLOD)
{
	INT32 i, j;
	i3Node * pChild, * pParent;
	i3Geometry * pGeo;

	I3_MUST_ADDREF(pSkin);

	// Skin의 Child Node들을 모두 옮긴다.
	for( ; pSkin->GetChildCount() > 0;)
	{
		pChild = pSkin->GetChild( 0);
		pChild->ChangeParent( pBody);
	}

	// 각 Shape에 포함된 i3GeometryAttr들을 모두 i3Geometry Node로 이동
	{
		pGeo = i3Geometry::new_object_ref();

		for( i = 0; i <  (INT32) pSkin->getShapeCount(); i++)
		{
			i3Shape * pShape = pSkin->getShape( i);

			for( j = 0; j < pShape->getGeometryAttrCount(); j++)
			{
				pGeo->AppendGeometryAttr( pShape->getGeometryAttr( j));
			}
		}
	}

	// Skin의 Parent Node에서 제거
	pParent = pSkin->GetParent();
	if( pParent != nullptr)
	{
		// Skin의 제거
		pParent->RemoveChild( pSkin);
		pParent->AddChild( pGeo);


		///////////////
		I3_MUST_ADDREF(pParent);

		i3Node * pGrandParent = pParent->GetParent();
		if( pGrandParent != nullptr)
		{
			pGrandParent->RemoveChild( pParent);

			m_pLastParent = pGrandParent;
		}

		pLOD->getShapeNode()->AddChild( pParent);

		I3_MUST_RELEASE( pParent);
	}
	else
	{
		pLOD->getShapeNode()->AddChild( pGeo);
	}

	if( pSkin->GetCurrentAnim() != nullptr)
	{
		pBody->PlayAnim( pSkin->GetCurrentAnim());
	}

	I3_MUST_RELEASE( pSkin);
}

i3Node *	i3OptScene2Body::_FindParentBone( i3Node * pNode, i3Node ** pPrevParent)
{
	i3Node * pParent = pNode->GetParent();
	i3Node * pOldParent = pNode;

	while( pParent != nullptr)
	{
		if( i3::kind_of<i3Transform* >(pParent))
		{
			if( ! pParent->IsFlag( I3_NODEFLAG_CONTROL | I3_NODEFLAG_STATIC))
			{
				if( pPrevParent != nullptr)
					*pPrevParent = pOldParent;

				return pParent;
			}
		}

		pOldParent = pParent;
		pParent = pParent->GetParent();
	}

	return nullptr;
}

void i3OptScene2Body::_Rec_BuildBone( i3Node * pNode, INT32 depth, i3Skeleton * pSkel, i3LOD * pLOD)
{
	INT32 i;
	i3Node * pChild, * pParent;
	i3Bone * pBone;

	if( i3::same_of<i3Transform* >(pNode))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		if( !pTrans->IsFlag( I3_NODEFLAG_CONTROL | I3_NODEFLAG_STATIC))
		{
			pBone = pSkel->getBone( m_BoneIndex);

			pBone->setName( pNode->GetNameString());
			pBone->setMatrix( pTrans->GetMatrix());
			pBone->setDepth( depth);

			i3::string str( pNode->GetNameString());
			pSkel->getBoneMapString()->insert( i3::vector_map<i3::string, INT32>::value_type(pNode->GetNameString(), m_BoneIndex) );

			pParent = (i3Node *) _FindParentBone( pNode, nullptr);
			if( pParent != nullptr)
			{
				INT32 idx = pSkel->FindBoneByName( (char *) pParent->GetName());
				I3ASSERT( idx != -1);

				pBone->setParentIndex( idx);
			}
			else
			{
				pBone->setParentIndex( -1);
			}

			depth++;
			m_BoneIndex++;
		}
	}
	else if( i3::same_of<i3Transform2* >(pNode))
	{
		i3Transform2 * pTrans = (i3Transform2 *) pNode;

		if( !pTrans->IsFlag( I3_NODEFLAG_CONTROL ) )
		{
			i3PhysixShapeSet * pShapeSet = i3PhysixShapeSet::new_object_ref();
			pTrans->getShapeSet()->CopyTo( pShapeSet, I3_COPY_INSTANCE );

			pSkel->setShapeSet( m_BoneIndex, pShapeSet );

			pBone = pSkel->getBone( m_BoneIndex);

			pBone->setName( pNode->GetNameString());
			pBone->setMatrix( pTrans->GetMatrix());
			pBone->setDepth( depth);

			i3::string str( pNode->GetNameString());
			pSkel->getBoneMapString()->insert( i3::vector_map<i3::string, INT32>::value_type(pNode->GetNameString(), m_BoneIndex) );

			pParent = (i3Node *) _FindParentBone( pNode, nullptr);
			if( pParent != nullptr)
			{
				INT32 idx = pSkel->FindBoneByName( (char *) pParent->GetName());
				I3ASSERT( idx != -1);

				pBone->setParentIndex( idx);
			}
			else
			{
				pBone->setParentIndex( -1);
			}

			depth++;
			m_BoneIndex++;
		}
	}

	{
		if( pNode->GetChildCount() > 1)
		{
			i3::vector<i3Node*>& ChildList = pNode->GetChildList();
			
			i3::sort(ChildList.begin(), ChildList.end(), &_SortByNameProc);
//			ChildList->Sort( (COMPAREPROC)_SortByNameProc);
		}
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		_Rec_BuildBone( pChild, depth, pSkel, pLOD);
	}
}

//
//   pNode가 반드시 레퍼런스카운팅 1이상 확보된 것으로 가정해야 한다...
//   리턴되는 객체는 널값이거나, 반드시 레퍼런스카운팅이 추가 확보된 객체가 되도록 해야한다..(2012.11.01.수빈)
//

i3Node * i3OptScene2Body::_Rec_RemoveTransform( i3Node * pNode, i3LOD * pLOD)
{

	i3Node* pResult = pNode;

	i3Node * pParent = pNode->GetParent();			// 원래 있던 부모를 보존....함수본체에서 계속 활용됨..

	if( i3::kind_of<i3Transform* >(pNode)) 
	{
		if( pNode->IsFlag( I3_NODEFLAG_CONTROL))
		{
			// 필요없기 때문에 제거한다.
			if( pParent == nullptr)
			{
				if( pNode->GetChildCount() > 0)
				{
					if( pNode->GetChildCount() == 1)
					{
						pResult = pNode->GetChild( 0);
						I3_MUST_ADDREF(pResult);
					}
					else
					{
						// 빈 Node를 만들고 현재 노드를 대신하게 한다 .
						pResult = i3Node::new_object();
						i3Scene::MoveChildren( pResult, pNode);
					}

					I3_MUST_RELEASE(pNode);	// 이 변수는 이제 자식이 없으므로 삭제...(조건에 의해 부모도 없음)
					pResult = _Rec_RemoveTransform( pResult, pLOD);
					return pResult;
				}
			}
			else
			{
				i3Scene::MoveChildren( pParent, pNode);
				pParent->RemoveChild( pNode);
				return nullptr;			// 널 리턴....대신할게 없기 때문이다.....
			}
		}
		else if( pNode->IsFlag( I3_NODEFLAG_STATIC) )
		{
			// 일반 다른 Node와 동일하게 간주한다.

			if( i3::same_of<i3Transform2* >(pNode)) 
			{
				i3BoneRef * pBoneRef = i3BoneRef::new_object_ref();
				INT32 idxBone = pLOD->getSkeleton()->FindBoneByName( (char *) pNode->GetName());
				I3ASSERT( idxBone != -1);

				pBoneRef->SetName( pNode->GetName() ); // szName);
				pBoneRef->setBoneIndex( idxBone);

				if( pParent != nullptr)
				{
					// Transform2는 이미 본의 매트릭스 값이 있습니다.
					I3_MUST_ADDREF(pNode);
					pParent->RemoveChild( pNode);

					for(INT32 i = 0; i < pNode->GetChildCount(); i++ )
						pBoneRef->AddChild( pNode->GetChild( i ) );

					pParent->AddChild( pBoneRef);
					I3_MUST_RELEASE(pNode);

					return nullptr;				// 널리턴.. 
				}
				else
				{
					pResult = pBoneRef;
					
					// Transform2는 이미 본의 매트릭스 값이 있습니다.
					for(INT32 i = 0; i < pNode->GetChildCount(); i++ )
						pBoneRef->AddChild( pNode->GetChild( i ) );

					I3_MUST_RELEASE(pNode);
				}
			}
		}
		else
		{
			// 이외의 Transform Node는 Bone으로 간주되었을 것이기 때문에
			// i3BoneRef Node로 대체한다.
			i3BoneRef * pBoneRef = i3BoneRef::new_object_ref();
			INT32 idxBone;

			// 이하의 Child들이 Bone 이외의 Geometry 등을 포함한 Node가 있는지 확인하고 그것에
			// 대해서만 처리한다.
			// 왜냐하면 Bone으로 대체된 Child들은 추가할 필요가 없기 때문...

			idxBone = pLOD->getSkeleton()->FindBoneByName( (char *) pNode->GetName());
			I3ASSERT( idxBone != -1);

			pBoneRef->SetName( pNode->GetName() ); // szName);
			pBoneRef->setBoneIndex( idxBone);

			//I3TRACE( "BoneRef : %s\n", pBoneRef->GetName() );

			if( pNode->GetChildCount() == 0 )
			{
				if ( pParent )
					pParent->AddChild(pBoneRef);
					
				pBoneRef->AddChild( pNode);
				pNode->SetFlag( I3_NODEFLAG_STATIC );

				if (pParent)
				{
					pParent->RemoveChild(pNode);
					return nullptr;
				}

				pResult = pBoneRef;
			}
			else
			{	
				for(INT32 i = 0; i < pNode->GetChildCount(); i++ )
				{	
					i3Node* pChild = pNode->GetChild( i );

					i3Node * pReturn = _Rec_RemoveTransform( pChild, pLOD);

					if( pReturn != nullptr )
					{
						pBoneRef->AddChild( pReturn );
						I3_MUST_RELEASE(pReturn);						// 릴리즈가 없으면 릭이 됩니다...(2012.11.01.수빈)
					}
					else
						i--;
				}
			}

			if( pParent != nullptr)
			{	
				I3_MUST_ADDREF(pNode);
				pParent->RemoveChild( pNode);
				pParent->AddChild( pBoneRef);
				I3_MUST_RELEASE(pNode);

				return nullptr;
			}
			else
			{
				pResult = pBoneRef;
			}
		}
	}


	for(INT32 i = 0; i < pNode->GetChildCount(); i++)
	{
		i3Node* pChild = pNode->GetChild( i);

		i3Node * pReturn = _Rec_RemoveTransform( pChild, pLOD);
		if( pReturn == nullptr)
		{
			i--;
		}
		else
		{
			I3_MUST_RELEASE(pReturn);
		}
	}
	
	I3_MUST_ADDREF(pResult);

	return pResult;
}


void i3OptScene2Body::_Scene2Body( i3Node * pNode, i3Body * pBody, i3LOD * pLOD)
{
	i3::vector<i3Node*> TransList;
	i3Node  * pClone;

	// Bone Hierarchy를 검색한다.
	i3Scene::FindNodeByType( pNode, i3Transform::static_meta(), TransList);

	// I3_NODEFLAG_CONTROL이 있는 Node는 제거
	{
		for(size_t i = 0; i < TransList.size(); i++)
		{
			i3Node * pTemp = TransList[i];

			if( i3::same_of<i3Transform2*>(pTemp))
				continue;

			else if( pTemp->IsFlag( I3_NODEFLAG_CONTROL | I3_NODEFLAG_STATIC) )
			{
				//TransList.Delete( i);
				TransList.erase(TransList.begin() + i);
				i--;
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
		i3Node * pNewClone;

		pClone = i3Scene::CreateClone( pNode, I3_COPY_INSTANCE);
		I3_MUST_ADDREF(pClone);						// 강제로 릴리즈 가능하도록 레퍼런스카운팅을 킨다..(2012.11.01.수빈)

		pNewClone = _Rec_RemoveTransform( pClone, pLOD);		// 리턴값역시 내부에서 레퍼런스카운팅이 돌도록 조절한다. (2012.11.01.수빈)

		pLOD->setShapeNode( pNewClone);
		
		I3_SAFE_RELEASE(pNewClone);
	}
}

void i3OptScene2Body::Trace( i3Node * pRoot)
{
	i3::vector<i3Node*>	SkinList;
	i3::vector<i3Skeleton*>	SkelList;
	i3Skin * pSkin;
	i3Body * pBody;
	i3Skeleton * pSkel;
	i3LOD * pLOD;

	// i3Skin Node를 모두 검색한다.
	i3Scene::FindNodeByType( pRoot, i3Skin::static_meta(), SkinList);

	if( SkinList.size() > 0)
	{
		// Skinning Mesh를 위한 Body
		// Skin들이 사용하는 Skeleton들을 구한다. 단, 중복되지 않도록...
		for(size_t i = 0; i < SkinList.size(); i++)
		{
			pSkin = (i3Skin *) SkinList[i];

			pSkel = pSkin->getSkeleton(0);

			SkelList.push_back( pSkel);
		}
		
		i3::vu::remove_duplicates(SkelList);

		// 동일한 Skeleton을 사용하는 것들 단위로 i3Body Node를 생성하여 대체한다.
		for(size_t i = 0; i < SkelList.size(); i++)
		{
			pSkel = SkelList[i];

			m_pLastParent = nullptr;

			pBody = i3Body::new_object_ref();
			pLOD = i3LOD::new_object_ref();

			{
				i3Node * pNode = i3Node::new_object_ref();

				pLOD->setShapeNode( pNode);
			}

			pLOD->setSkeleton( pSkel);
			pBody->AddLOD( pLOD);

			for( INT32 j = 0; j < INT32(SkinList.size()); j++)
			{
				pSkin = (i3Skin *) SkinList[j];
				if( pSkin->getSkeleton(0) != pSkel)
					continue;

				_Skin2Body( pSkin, pBody, pLOD);

				//SkinList.Remove( pSkin);
				i3::vu::remove(SkinList, pSkin);
				j--;
			}

			if( m_pLastParent != nullptr)
			{
				m_pLastParent->AddChild( pBody);
			}
			else
			{
				SetTraceRoot( pBody);
			}
		}
	}
	else
	{
		// 일반 Scene-Graph를 Body로 정리...
		pBody = i3Body::new_object_ref();
		pLOD = i3LOD::new_object_ref();

		_Scene2Body( pRoot, pBody, pLOD);

		pBody->AddLOD( pLOD);

		SetTraceRoot( pBody);
	}

}
