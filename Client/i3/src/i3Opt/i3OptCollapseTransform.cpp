#include "i3OptPCH.h"
#include "i3OptCollapseTransform.h"

I3_CLASS_INSTANCE( i3OptCollapseTransform);

i3OptCollapseTransform::i3OptCollapseTransform(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}


void i3OptCollapseTransform::OnGeometry( MATRIX * pMat, i3Geometry * pGeoNode)
{
	i3GeometryAttr * pGeoAttr; 
	INT32 i;

	for( i = 0; i < pGeoNode->GetGeometryAttrCount() ; i++)
	{
		pGeoAttr = (i3GeometryAttr *)pGeoNode->GetGeometryAttr(i); 
		pGeoAttr->Transform(pMat); 
	}
}

void i3OptCollapseTransform::OnTransform( MATRIX * pMat, i3Transform * pTransform)
{
	MATRIX ReMat; 
	MATRIX Des; 

	i3Matrix::Copy( &Des, pTransform->GetMatrix() ); 		
	i3Matrix::Mul( &ReMat, &Des, pMat);
	pTransform->SetMatrix(&ReMat);  

	// Animation 처리
}

void i3OptCollapseTransform::OnColliderSet( MATRIX * pMat, i3ColliderSet * pCollSet)
{
	/*
	INT32 i;
	i3Collider * pCollider;
	i3Collidee * pCollidee;

	for( i = 0; i < pCollSet->GetColliderCount(); i++)
	{
		pCollider = pCollSet->GetCollider( i);

		pCollidee = pCollider->GetCollidee();
		pCollidee->Transform( pMat, pCollidee);
	}
	*/
}


void i3OptCollapseTransform::OnShapeSetContainer( MATRIX * pMat, i3ShapeSetContainer * pShapeSetContainer)
{	
	MATRIX localMat, mtxNew;
	//MATRIX Des;

	i3PhysixShapeSet * pShapeSet = pShapeSetContainer->getShapeSet();
	if( pShapeSet != nullptr)
	{
		//NxActor * pActor = pShapeSet->getNxActor();
		for( INT32 i = 0; i < pShapeSet->getNxShapeCount(); i++)
		{
			NxShape * pShape = pShapeSet->getNxShape( i);
			NxMat34 mat = pShape->getLocalPose();
			NXU::Copy( &localMat, &mat);
			i3Matrix::Mul( &mtxNew, &localMat, pMat);
			NXU::Copy( &mat, &mtxNew);
			pShape->setLocalPose( mat);
		}
	}
}

void i3OptCollapseTransform::CollapseTransform( MATRIX * pMat, i3Node * pNode)
{
	INT32 i; 	

	if( i3::kind_of<i3Geometry* >(pNode))
	{
		//1. Geometry 일때 
		OnGeometry( pMat, (i3Geometry *) pNode);
	}
	else if( i3::kind_of<i3Transform* >(pNode))
	{
		//2. Transform 일때
		OnTransform( pMat, (i3Transform *) pNode);
		return;
	}
	else if( i3::kind_of<i3ColliderSet* >(pNode))
	{
		//3. CollidetSet일 때
		OnColliderSet( pMat, (i3ColliderSet *) pNode);
	}
	else if( i3::same_of<i3ShapeSetContainer* >(pNode))
	{
		OnShapeSetContainer( pMat, (i3ShapeSetContainer*) pNode);
	}
	else
	{	//그외 다른 Node 일때 
		i3Node * pChildNode = nullptr;

		for(i = 0; i < pNode->GetChildCount() ; i++)
		{
			pChildNode = pNode->GetChild(i); 
			CollapseTransform( pMat, pChildNode); 
		}
	}

	return; 
}

bool i3OptCollapseTransform::_IsNeedlessTransform( i3Transform * pTrans)
{
	INT32 i;
	i3Node * pChild;

	// Animation이 있다면 제거할 수 없다.
	if( pTrans->IsAnimated())
		return false;

	// Character에 있는 Attach Dummy 등이 제거되는 것을 막기 위해 추가한 코드
	if( pTrans->IsFlag( I3_NODEFLAG_ATTACH_POINT))
		return false;

	// Child 중에 i3Camera Node가 있다면 제거할 수 없다.
	for( i = 0; i < pTrans->GetChildCount(); i++)
	{
		pChild = pTrans->GetChild( i);

		if( i3::kind_of<i3Camera* >(pChild))
			return false;

		if( i3::kind_of<i3Skin* >(pChild))
			return false;

		if( i3::kind_of<i3Transform* >(pChild))
		{
			i3Transform * pChildTrans = (i3Transform *) pChild;

			if( pChildTrans->IsAnimated())
				return false;

			if( pChildTrans->IsFlag( I3_NODEFLAG_ATTACH_POINT))
				return false;
		}
	}

	return true;
}

bool i3OptCollapseTransform::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::same_of<i3Transform2* >(pNode))
	{
		// ShapeSet의 Shape의 포지션을 변경하기 위해 리스트에 추가
		// Local좌표를 변경합니다.
		m_ShapeTransList.push_back( static_cast<i3Transform2*>(pNode));
	}
	else if( i3::kind_of<i3Transform* >(pNode))
	{ 
		i3Transform * pTrans = (i3Transform *) pNode;

		if( _IsNeedlessTransform( pTrans))
		{
			// 불필요한 Transform을 List에 추가.
			m_TransList.push_back( pTrans);
		}
	}

	return true;
}

void i3OptCollapseTransform::Trace( i3Node * pRoot)
{
	i3Transform * pTrans;
	i3Node * pChild, * pParent;
	

	m_TransList.clear();
	m_ShapeTransList.clear();

	i3SceneOptimizer::Trace( pRoot);
	
	for(size_t i = 0; i < m_TransList.size(); i++)
	{
		pTrans = m_TransList[i];

		I3_MUST_ADDREF(pTrans);

		pParent = pTrans->GetParent();
		if( getTraceRoot() == pTrans)
		{
			// Root가 제거된다.
			pParent = i3Node::new_object();

			SetTraceRoot( pParent);
		}
		else
		{
			pParent->RemoveChild( pTrans);
		}

		// Child Node들을 Parent Node로 이동
		for( ; pTrans->GetChildCount() > 0;)
		{
			pChild = pTrans->GetChild( 0);
			pChild->ChangeParent( pParent);

			CollapseTransform( pTrans->GetMatrix(), pChild);
		}

		I3_MUST_RELEASE(pTrans);
	}


	
	{
		// i3Transform2의 Shape을 transform시켜주고 node는 identity로 세팅합니다.
		MATRIX localMat, mtxTemp;
		//MATRIX Des;
		for(size_t i = 0; i < m_ShapeTransList.size(); i++)
		{
			i3Transform2 * pTrans2 = m_ShapeTransList[i];
				
			i3PhysixShapeSet * pShapeSet = pTrans2->getShapeSet();
			
			if( pShapeSet != nullptr)
			{
				//NxActor * pActor = pShapeSet->getNxActor();

				NxMat34 mat;
				for( INT32 j = 0; j < pShapeSet->getNxShapeCount(); j++)
				{
					NxShape * pShape = pShapeSet->getNxShape( j);
					mat = pShape->getLocalPose();
					NXU::Copy( &localMat, &mat);
					i3Matrix::Mul( &mtxTemp, &localMat, pTrans2->GetMatrix());
					NXU::Copy( &mat, &mtxTemp);
					pShape->setLocalPose( mat);
				}
			}

			for( INT32 j = 0; j < pTrans2->GetChildCount(); j++)
			{
				pChild = pTrans2->GetChild( j);
				CollapseTransform( pTrans2->GetMatrix(), pChild);
			}

			i3Matrix::Identity( pTrans2->GetMatrix());
		}
	}
}

