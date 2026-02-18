#include "i3OptPCH.h"
#include "i3OptSkinToSkin2.h"

I3_CLASS_INSTANCE( i3OptSkinToSkin2, i3SceneOptimizer);

i3OptSkinToSkin2::i3OptSkinToSkin2(void)
{
}

i3OptSkinToSkin2::~i3OptSkinToSkin2(void)
{
}

void i3OptSkinToSkin2::_MoveToSkin2( i3Skin * pSkin, i3Skin2 * pSkin2, i3SkinLOD * pSkinLOD)
{
	INT32 i, j;
	i3Node * pChild, * pParent;
	i3Geometry * pGeo;

	pSkin->AddRef();

	// Skin의 Child Node들을 모두 옮긴다.
	for( ; pSkin->GetChildCount() > 0;)
	{
		pChild = pSkin->GetChild( 0);

		pChild->AddRef();
		pSkin->RemoveChild( pChild);
		pSkin2->AddChild( pChild);
		pChild->Release();
	}

	// 각 Shape에 포함된 i3GeometryAttr들을 모두 i3Geometry Node로 이동
	{
		pGeo = i3Geometry::NewObjectRef();

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
	if( pParent != NULL)
	{
		// Skin의 제거
		pParent->RemoveChild( pSkin);

		pParent->AddChild( pGeo);


		///////////////
		pParent->AddRef();

		i3Node * pGrandParent = pParent->GetParent();
		if( pGrandParent != NULL)
		{
			pGrandParent->RemoveChild( pParent);

			m_pLastParent = pGrandParent;
		}

		pSkinLOD->getShapeNode()->AddChild( pParent);

		pParent->Release();
	}
	else
	{
		pSkinLOD->getShapeNode()->AddChild( pGeo);
	}

	if( pSkin->GetCurrentAnim() != NULL)
	{
		pSkin2->PlayAnim( pSkin->GetCurrentAnim());
	}

	pSkin->Release();
}

void i3OptSkinToSkin2::Trace( i3Node * pRoot)
{
	i3List	SkinList;
	i3List	SkelList;
	INT32 i, j;
	i3Skin * pSkin;
	i3Skin2 * pSkin2;
	i3Skeleton * pSkel;
	i3SkinLOD * pSkinLOD;

	m_pScene = pRoot;
	m_pScene->AddRef();

	// i3Skin Node를 모두 검색한다.
	i3Scene::FindNodeByType( pRoot, i3Skin::GetClassMeta(), &SkinList);

	SkelList.SetOnceMode( TRUE);

	// Skin들이 사용하는 Skeleton들을 구한다. 단, 중복되지 않도록...
	for( i = 0; i < SkinList.GetCount(); i++)
	{
		pSkin = (i3Skin *) SkinList.Items[i];

		SkelList.Add( pSkin->getSkeleton(0));	
	}

	// 동일한 Skeleton을 사용하는 것들 단위로 i3Skin2 Node를 생성하여 대체한다.
	for( i = 0; i < SkelList.GetCount(); i++)
	{
		pSkel = (i3Skeleton *) SkelList.Items[i];

		m_pLastParent = NULL;

		pSkin2 = i3Skin2::NewObjectRef();
		pSkinLOD = i3SkinLOD::NewObjectRef();

		{
			i3Node * pNode = i3Node::NewObjectRef();

			pSkinLOD->setShapeNode( pNode);
		}

		pSkinLOD->setSkeleton( pSkel);
		pSkin2->AddLOD( pSkinLOD);

		for( j = 0; j < SkinList.GetCount(); j++)
		{
			pSkin = (i3Skin *) SkinList.Items[j];
			if( pSkin->getSkeleton(0) != pSkel)
				continue;

			_MoveToSkin2( pSkin, pSkin2, pSkinLOD);
		}

		if( m_pLastParent != NULL)
		{
			m_pLastParent->AddChild( pSkin2);
		}
		else
		{
			m_pScene->Release();
			m_pScene = pSkin2;
			m_pScene->AddRef();
		}
	}

}
