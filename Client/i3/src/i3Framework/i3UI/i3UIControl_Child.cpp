#include "i3FrameworkPCH.h"
#include "i3UIControl.h"
#include "i3UIManager.h"
#include "i3UIRenderer.h"

//#include "i3UIListView.h"
//#include "i3UIListViewItem.h"

bool i3UIControl::_ResizeControl(void)
{
	/*
	INT32 i = 0;

	REAL32 rx = i3UI::getUIResolutionWidth(); //i3UI::getViewer()->GetViewWidth() * 0.9765625e-3f; // / I3UI_RESOLUTION_WIDTH;
	REAL32 ry = i3UI::getUIResolutionHeight(); //i3UI::getViewer()->GetViewHeight() * 0.13020833333333333333333333333333e-2f; // / I3UI_RESOLUTION_HEIGHT;

	if( m_pData->m_pShape)
	{
		INT32 nShapeCount = m_pData->m_pShape->getShapeCount();		

		for(i=0; i<nShapeCount; i++)
		{
			// Shape의 크기를 화면 해상도에 맞게 변경합니다.

			i3UIImage *pImage = m_pData->m_pShape->getShape((I3UI_CTRL_SHAPE) i);

			pImage->setWidth( (pImage->getWidth() * rx));
			pImage->setHeight( (pImage->getHeight() * ry));			

			pImage->m_nX = (pImage->m_nX * rx);
			pImage->m_nY = (pImage->m_nY * ry);			
		}
	}
	
	//
	setSize( (getOriginalWidth() * rx), (getOriginalHeight() * ry));
	*/
	
	return true;
}

void i3UIControl::ShowTop(void)
{
	AdjustSpriteOrder( g_pUIRenderer->getTail());
}


INT32 i3UIControl::AdjustSpriteOrder( INT32 hSpriteEnd, bool bRec)
{
	/*
	if( g_pUIRenderer == nullptr)
		return -1;

	if( m_hSprites == nullptr || m_hSprites[0] == -1)
		return hSpriteEnd;

	if( hSpriteEnd != -1)
	{
		g_pUIRenderer->AdjustOrder( hSpriteEnd, m_hSprites, m_nShapeCount);
	}

	i3GameNode * pChild = getFirstChild();

	// 
	INT32 idxEnd = hSpriteEnd;
	if( m_nShapeCount > 0)
	{
		idxEnd = m_hSprites[ m_nShapeCount - 1];
	}

	if( bRec)
	{
		while( pChild != nullptr)
		{
			if( i3::kind_of<i3UIControl*>(pChild))
			{
				i3UIControl * pCtrl = (i3UIControl *) pChild;
				if( pCtrl->isEnable() && !pCtrl->isStyle( I3UI_STYLE_MANUAL_ORDER))
				{
					idxEnd = pCtrl->AdjustSpriteOrder( idxEnd);
				}
			}

			pChild = (i3GameNode *) pChild->getNext();
		}
	}

	return idxEnd;
	*/

	return 0;
}

void i3UIControl::setAncestor( i3UIScene * pScene)
{
#if defined( I3_DEBUG) && 0
	if( pScene != nullptr)
	{
		I3TRACE( "SetAncestor %s -> %s\n", this->GetName(), pScene->GetName());
	}
	else
	{
		I3TRACE( "SetAncestor %s -> nullptr\n", this->GetName());
	}
#endif
	m_pAncestor = pScene;
}

void i3UIControl::AddControl( i3UIControl * pChild, bool bAddToGg )
{
	// Game Graph에 연결
	if( bAddToGg)
	{
		i3GameNode::AddChild( pChild);
	}

	// Child의 모든 Sprite들을 Parent::Sprite들의 후미로 이동
	if( i3UI::getToolDevMode())
		AdjustSpriteOrder();

	i3GameNode * pParent = this;
	while( (pParent != nullptr) && !i3::same_of<i3UIScene*>(pParent))
	{
		if( i3::same_of<i3UIFrameWnd*>(pParent) )
		{
			if( ((i3UIFrameWnd*)pParent)->isTopFrameWnd())
			{
				pChild->setTopFrameWnd( static_cast<i3UIFrameWnd*>(pParent));
				break;
			}
		}

		pParent = pParent->getParent();
	}
}

void i3UIControl::RemoveControl( i3UIControl * pObj)
{
	// Scene Graph에 연결 해제 및 릴리즈
	//GetNode()->RemoveChild( pObj->GetNode());

	// Game Graph에 연결 해제 및 릴리즈
	if( pObj != nullptr)
	{
		pObj->EnableCtrl( false);
		pObj->setTopFrameWnd( nullptr);
	}

	i3GameNode::RemoveChild( pObj);
}

void i3UIControl::RemoveAllControl()
{
	if( i3::same_of<i3UIFrameWnd*>(this) )
	{
		i3GameNode *pChild = this->getFirstChild();

		while( pChild != nullptr)
		{
			static_cast<i3UIControl*>(pChild)->setTopFrameWnd( nullptr);
			pChild = pChild->getNext();
		}
	}

	i3GameNode::RemoveAllChild();
}

i3UIControl *	i3UIControl::FindCtrlByName( const char * pszName, bool bRecursive)
{
	i3UIControl * pChild = (i3UIControl *) getFirstChild();

	while( pChild != nullptr)
	{
		if( pChild->hasName())
		{
			if( i3::generic_is_iequal( pszName, pChild->GetNameString()) )
			{
				return pChild;
			}
		}

		if( bRecursive)
		{
			i3UIControl * pRv = pChild->FindCtrlByName( pszName, true);

			if( pRv != nullptr)
				return pRv;
		}

		pChild = (i3UIControl *) pChild->getNext();
	}

	return nullptr;
}

static bool sRec_IsChildControl( i3UIControl * pDest, i3UIControl * pSrc)
{
	if( pDest == pSrc)
		return true;

	i3UIControl * pChild = (i3UIControl*) pSrc->getFirstChild();

	while( pChild)
	{
		if( sRec_IsChildControl( pDest, pChild))
			return true;

		pChild = (i3UIControl*) pChild->getNext();
	}

	return false;
}

bool i3UIControl::IsChildControl( i3UIControl * pCtrl)
{
	return sRec_IsChildControl( pCtrl, this);
}
