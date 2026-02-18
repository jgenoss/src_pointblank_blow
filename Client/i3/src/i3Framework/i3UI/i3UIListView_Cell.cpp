#include "i3FrameworkPCH.h"
#include "i3UIListView_Cell.h"
#include "i3UITemplate_LVCell.h"
#include "i3UIScene.h"
#include "i3UIListView_Item.h"
#include "i3UIComboBase.h"
#include "i3UIControl_Dummy.h"

#include "i3UIManager.h"
#include "i3UIRenderer.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

I3_CLASS_INSTANCE( i3UIListView_Cell);

i3UIListView_Cell::i3UIListView_Cell()
{
#if defined( I3_DEBUG)
	SetName( "ItemListView_Cell");
#endif
}

i3UIListView_Cell::~i3UIListView_Cell()
{
	/*i3GameNode* pChild = getFirstChild();
	if( pChild != nullptr)
	{
		RemoveControl( (i3UIControl*)pChild);
		I3_SAFE_RELEASE( pChild);
	}*/
}

void i3UIListView_Cell::_UpdateShapeState(void)
{
	setShapeVisible( I3UI_LVCELL_SHAPE_CELL, true);
}

void i3UIListView_Cell::AddCellChild( i3UITemplate* pTemplate)
{
	I3ASSERT( pTemplate != nullptr);

	//Child는 1개밖에 올 수 없다.
	i3GameNode* pChild = getFirstChild();
	if( pChild != nullptr)
		return;

	//Ci3UIEditorView::DropObject와 거의 마찬가지
	i3ClassMeta* pMeta = pTemplate->GetInstanceMeta();
	I3ASSERT( pMeta != nullptr && pMeta->kind_of( i3UIControl::static_meta()));
	
	i3UIControl * pControl = (i3UIControl*)pMeta->create_instance(); //	CREATEINSTANCE( pMeta);
	
	pControl->SetTemplate( pTemplate);
	pControl->setPos(0,0,0);
	
	//pControl->setSize(&m_Size);		//m_ChildRect?
	
	pControl->Init();
	pControl->EnableCtrl( true);

	AddControl( pControl);

	// Layer를 Manual로 저절하는 것은 SpriteOrder를 자동으로 하지 않도록 한다.
	if( pControl->isStyle( I3UI_STYLE_NO_VT))
		addStyle( I3UI_STYLE_MANUAL_ORDER);

	// 로딩이 다 되었다고 간주해버린다.
	if( isState( I3UI_STATE_WAITLOAD) == false)
	{
		OnCompletedLoading();
	}

	// set ancestor
	pControl->setAncestor( getAncestor() );
}

void i3UIListView_Cell::SetCellChild( const TEMPLATE_INFO* Info)
{
	I3ASSERT( Info != nullptr);
	
	i3GameNode* pChild = getFirstChild();
	if( pChild != nullptr)
		return;

	i3UIControl_Dummy* pControl_Dummy = i3UIControl_Dummy::new_object();
	I3ASSERT( pControl_Dummy != nullptr);

	pControl_Dummy->SetTemplate( this->getTemplate());

	for( size_t cnt = 0; cnt < Info->m_TemplateList.size(); ++cnt)
	{
		i3UITemplate* pTemplate = i3UI::FindTemplateByName( Info->m_TemplateList[cnt].c_str());
		I3ASSERT( pTemplate != nullptr);

		pControl_Dummy->AddChildControl( pTemplate);
	}

	pControl_Dummy->setPos(0,0,0);
	//pControl_Dummy->setSize(&m_Size);		//m_ChildRect?
	pControl_Dummy->Init();
	pControl_Dummy->EnableCtrl( true);

	AddControl( pControl_Dummy);
	I3_MUST_RELEASE( pControl_Dummy);

	// Layer를 Manual로 저절하는 것은 SpriteOrder를 자동으로 하지 않도록 한다.
	if( pControl_Dummy->isStyle( I3UI_STYLE_NO_VT))
		addStyle( I3UI_STYLE_MANUAL_ORDER);

	// 로딩이 다 되었다고 간주해버린다.
	if( isState( I3UI_STATE_WAITLOAD) == false)
	{
		OnCompletedLoading();
	}

	// set ancestor
	pControl_Dummy->setAncestor( getAncestor() );
}

void i3UIListView_Cell::RemoveCellChild( void)
{
	i3GameNode* pChild = getFirstChild();
	if( pChild != nullptr && i3::kind_of<i3UIControl*>(pChild))
	{
		RemoveControl( (i3UIControl*)pChild);
	}
}

void i3UIListView_Cell::ReplaceCellChild( i3UITemplate* pTemplate)
{
	I3ASSERT( pTemplate != nullptr);

	i3UIControl* pChild = getCellChild();	// != getChildTemplate
	
	I3ASSERT( pChild != nullptr);

	pChild->SetTemplate( pTemplate);
}

void i3UIListView_Cell::OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2)
{
	switch( event)
	{
	case I3UI_EVT_ENTERMOUSE :	m_bHasTtTimer = true;		break;
	case I3UI_EVT_LEAVEMOUSE :	_LoseTimer();				break;
	}

	if( event != I3UI_EVT_MOUSEWHEEL )
		ParentNotify( event, param1, param2);
}

void i3UIListView_Cell::SetSizeNoNotify( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::SetSizeNoNotify( cx, cy);

	if( m_pTemplate != nullptr)
	{
		m_pTemplate->AdjustLayout( cx, cy, m_pRects, m_nShapeCount);
	}
}

void i3UIListView_Cell::OnSize( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::OnSize(cx, cy);
	
	setChildRect( 0, 0, (INT32)cx, (INT32)cy);
}

void i3UIListView_Cell::OnDraw( i3UIRenderer * pRenderer)
{
	m_bCalcDoneAccumPos = false;	// 초기화

	if( this->isEnable() == false)
		return;

	if( isAllocedSprite() )
	{
		INT32 i;

		for( i = 0; i < getShapeCount(); i++)
		{
			INT32 hSprite = getSpriteHandle( i);

			if( hSprite != -1)
				pRenderer->Draw_Kick( hSprite);
		}
	}

	if( m_pText != nullptr && isEnable() )
	{
		m_pText->OnDraw( pRenderer);
	}

	if( getCellChild() != nullptr)
	{
		//pRenderer->changeDepth();
		pRenderer->DepthPlus();

		getCellChild()->OnDraw( pRenderer);

		pRenderer->DepthMinus();
	}
}

void i3UIListView_Cell::OnLButtonDown( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnLButtonDown( nFlag, point);

	i3GameNode* pParent = getParent();
	if( i3::same_of<i3UIListView_Item*>(pParent))
	{
		i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
		pItem->OnLButtonDown( nFlag, point);
	}
}

void i3UIListView_Cell::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnLButtonDblClk( nFlag, point);

	i3GameNode* pParent = getParent();
	if( i3::same_of<i3UIListView_Item*>(pParent))
	{
		i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
		pItem->OnLButtonDblClk( nFlag, point);
	}
}

void i3UIListView_Cell::OnLButtonUp( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnLButtonUp( nFlag, point);

	i3GameNode* pParent = getParent();
	if( i3::same_of<i3UIListView_Item*>(pParent))
	{
		i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
		pItem->OnLButtonUp( nFlag, point);
	}
}

/*virtual*/ void i3UIListView_Cell::OnRButtonDown( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnRButtonDown( nFlag, point);

	i3GameNode* pParent = getParent();
	if( i3::same_of<i3UIListView_Item*>(pParent))
	{
		i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
		pItem->OnRButtonDown( nFlag, point);
	}
}

/*virtual*/ void i3UIListView_Cell::OnRButtonUp( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnRButtonUp( nFlag, point);
 
	i3GameNode* pParent = getParent();
	if( i3::same_of<i3UIListView_Item*>(pParent))
	{
		i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
		pItem->OnRButtonUp( nFlag, point);
	}
}

void i3UIListView_Cell::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UICaptionControl::OnEnterMouse( x, y, keys);

	i3GameNode* pParent = getParent();
	if(  i3::same_of<i3UIListView_Item*>(pParent) )
	{
		i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
		pItem->OnEnterMouse( x, y, keys);
	}
}

void i3UIListView_Cell::OnLeaveMouse( void)
{
	i3UICaptionControl::OnLeaveMouse();

	i3GameNode* pParent = getParent();
	if(  i3::same_of<i3UIListView_Item*>(pParent))
	{
		i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
		pItem->OnLeaveMouse();
	}
}

i3UIControl* i3UIListView_Cell::OnSelect( void)
{
	i3GameNode* pParent = getParent();
	if(  i3::same_of<i3UIListView_Item*>(pParent))
	{ 
		i3UIListView_Item* pItem = (i3UIListView_Item*)pParent;
		return pItem;
	}

	return this;
}

bool i3UIListView_Cell::isOnMouse(I3UI_OVERLAP_INFO * pInfo)
{
	// 컨트롤 내 있으면 true 리턴
	if( i3UIControl::isOnMouse(pInfo))
		return true;

	i3UIControl * pTemp = (i3UIControl *) getFirstChild();

	while(pTemp != nullptr)
	{
		if(i3::kind_of<i3UIComboBase*>(pTemp)) 
		{
			if(pTemp->isOnMouse(pInfo))
				return true;
		}

		pTemp = (i3UIControl *) pTemp->getNext();
	}

	return false;
}


//////////////////////////////////////////
// Test Automation
void	i3UIListView_Cell::TA_GetPathID(char * pszID, INT32 limit)
{
	// Cell의 상위 부모인 ListView_Item의 Index로 Cell의 Path를 표현
	i3UIListView_Item * pParent = (i3UIListView_Item *)getParent();
	I3ASSERT(pParent != nullptr);
	//I3ASSERT(pParent->kind_of(i3UIListView_Item::static_meta()));

	INT32 idx = pParent->getCellIndex( *this);
	if (idx == -1)
	{
		i3UIControl::TA_GetPathID(pszID, limit);
		return;
	}

	sprintf_s(pszID, limit, "LVCell(%d)", idx);
}