#include "stdafx.h"
#include "i3TDKSceneGraphTreeCtrl.h"
#include "i3TDKGlobalVariable.h"
#include "resource.h"

void i3TDKSceneGraphTreeCtrl::_OnAttrSet( i3AttrSet * pAttrSet, HTREEITEM hItem)
{
	INT32 i;
	i3RenderAttr * pAttr;

	for( i = 0; i < pAttrSet->GetAttrCount(); i++)
	{
		pAttr = pAttrSet->GetAttr( i);

		AddTreeItem( pAttr, hItem);
	}
}

void i3TDKSceneGraphTreeCtrl::_OnLightSet( i3LightSet * pLightSet, HTREEITEM hItem)
{
	INT32 i;
	i3RenderAttr * pAttr;

	for( i = 0; i < pLightSet->GetLightAttrCount(); i++)
	{
		pAttr = pLightSet->GetLightAttr( i);

		AddTreeItem( pAttr, hItem);
	}
}

void i3TDKSceneGraphTreeCtrl::_OnGeometry( i3Geometry * pGeo, HTREEITEM hItem)
{
	INT32 i;
	i3RenderAttr * pAttr;
			
	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
	{
		pAttr = pGeo->GetGeometryAttr( i);

		AddTreeItem( pAttr, hItem);
	}
}

void i3TDKSceneGraphTreeCtrl::_OnOcTree( i3OcTreeNode * pNode, HTREEITEM hItem)
{
	INT32 i;

	for( i = 0; i < 8; i++)
	{
		i3Node * pChild = pNode->getChildOctree( (i3OcTreeNode::DIVIDE_NODE) i);

		if( pChild != nullptr)
			Rec_SetTree( hItem, pChild);
	}
}

void i3TDKSceneGraphTreeCtrl::_OnColliderSet( i3ColliderSet * pCollSet, HTREEITEM hItem)
{
	INT32 i;

	for( i = 0; i < pCollSet->GetColliderCount(); i++)
	{
		i3Collider * pColl = pCollSet->GetCollider( i);
		HTREEITEM hChildItem;

		hChildItem = AddTreeItem( pColl, hItem);

		AddTreeItem( pColl->GetCollidee(), hChildItem);
	}
}

void i3TDKSceneGraphTreeCtrl::_OnSkin( i3Skin * pNode, HTREEITEM hItem)
{
	INT32 i, j;
	i3Skin * pSkin = (i3Skin *) pNode;

	for( i = 0; i < (INT32) pSkin->getShapeCount(); i++)
	{
		i3Shape * pShape = pSkin->getShape( i);
		HTREEITEM hChildItem;

		hChildItem = AddTreeItem( pShape, hItem);

		for( j = 0; j < pShape->getGeometryAttrCount(); j++)
		{
			i3GeometryAttr * pGeoAttr = pShape->getGeometryAttr( j);

			AddTreeItem( pGeoAttr, hChildItem);
		}

	}
}

void i3TDKSceneGraphTreeCtrl::_OnBody( i3Body * pSkin, HTREEITEM hItem)
{
	INT32 i;

	for( i = 0; i < (INT32) pSkin->getLODCount(); i++)
	{
		i3LOD * pLOD = pSkin->getLOD( i);
		HTREEITEM hChildItem;

		hChildItem = AddTreeItem( pLOD, hItem);

		if( pLOD->getShapeNode() != nullptr)
		{
			Rec_SetTree( hChildItem, pLOD->getShapeNode());
		}
	}
}

void i3TDKSceneGraphTreeCtrl::_OnBinaryPartitionNode( i3BinaryPartitionNode * pBSP, HTREEITEM hItem)
{
	if( pBSP->getFront() != nullptr)
	{
		Rec_SetTree( hItem, pBSP->getFront());
	}

	if( pBSP->getBack() != nullptr)
	{
		Rec_SetTree( hItem, pBSP->getBack());
	}
}

void i3TDKSceneGraphTreeCtrl::_OnTimeSequence( i3TimeSequence * pNode, HTREEITEM hItem)
{
	for( INT32 i = 0; i < pNode->GetSequenceCount(); i++)
	{
		i3TimeSequenceInfo * pInfo = pNode->GetSequence( i);

		HTREEITEM hChildItem = AddTreeItem( pInfo, hItem);
		if( pInfo->GetNode() != nullptr)
		{
			Rec_SetTree( hChildItem, pInfo->GetNode());
		}
	}
}
