// SceneGraphTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "SceneGraphTreeCtrl.h"
#include "i3GraphDlg.h"

// CSceneGraphTreeCtrl

IMPLEMENT_DYNAMIC(CSceneGraphTreeCtrl, i3TDKSceneGraphTreeCtrl)
CSceneGraphTreeCtrl::CSceneGraphTreeCtrl()
{
	m_pLinkNode = i3Node::NewObject();
	m_pLinkNode->AddFlag( I3_NODEFLAG_VOLATILE);

	m_pAddPoint = NULL;
}

CSceneGraphTreeCtrl::~CSceneGraphTreeCtrl()
{
	if( m_pLinkNode != NULL)
	{
		m_pLinkNode->RemoveFromParent();
	}

	I3_SAFE_RELEASE( m_pLinkNode);
}

void CSceneGraphTreeCtrl::BuildRenderableColliderMesh(void)
{
	i3AttrSet * pAttrSet;

		pAttrSet = i3AttrSet::NewObjectRef();
		pAttrSet->AddFlag( I3_NODEFLAG_CONTROL);

		i3Node * pParent;

		pParent = m_pSgInfo->getInstanceSg();

		pParent->AddChild( pAttrSet);

		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::NewObjectRef();
			pAttr->Set( TRUE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::NewObjectRef();
			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::NewObjectRef();
			pAttr->Set( FALSE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::NewObjectRef();

			pAttr->Set( FALSE);
			pAttrSet->AddAttr( pAttr);
		}

		{
			i3ColorAttr * pAttr = i3ColorAttr::NewObjectRef();
			COLOR col;
			
			i3Color::Set( &col, (UINT8)(i3Math::Rand()%250 + 5), i3Math::Rand()%250 + 5, i3Math::Rand()%250 + 5, 128);

			pAttr->Set( &col);
			pAttrSet->AddAttr( pAttr);
		}


		i3Node * pMesh = i3SceneUtil::BuildRendableCollider( pParent, NULL, TRUE, TRUE);
		if( pMesh != NULL)
		{
			pAttrSet->AddChild( pMesh);
		}


	setSgInfo( getSgInfo());
}

void CSceneGraphTreeCtrl::OnChangeSelectItem( HTREEITEM hOld, HTREEITEM hNew)
{
	//Get Sellected Item     	
	if( hNew != NULL)
	{
		i3ElementBase * pObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hNew);

		g_pMainWnd->SetPropertyWnd( pObj); 
	}
}

void CSceneGraphTreeCtrl::OnSetSceneGraph(void)
{
	m_pAddPoint = NULL;

	m_pLinkNode->RemoveFromParent();
	m_pLinkNode->RemoveAllChild();

	if( getSgInfo() != NULL)
	{
		if( getSgInfo()->getCommonSg() != NULL)
		{
			getSgInfo()->getCommonSg()->AddChild( m_pLinkNode);

			m_pAddPoint = getSgInfo()->getCommonSg();
		}

		if( getSgInfo()->getInstanceSg() != NULL)
		{
			if( m_pAddPoint != NULL)
			{
				m_pLinkNode->AddChild( getSgInfo()->getInstanceSg());
			}
			else
			{
				m_pAddPoint = getSgInfo()->getInstanceSg();
			}
		}
	}
	else
	{
		m_pAddPoint = NULL;
	}
	
	g_pMainWnd->setSgInfo( getSgInfo());

}

BEGIN_MESSAGE_MAP(CSceneGraphTreeCtrl, i3TDKSceneGraphTreeCtrl)
END_MESSAGE_MAP()

// CSceneGraphTreeCtrl message handlers

