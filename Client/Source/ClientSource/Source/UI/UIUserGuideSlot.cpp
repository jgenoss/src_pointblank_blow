#include "pch.h"
#include "UIMainFrame.h"
#include "UIUserGuideSlot.h"

I3_CLASS_INSTANCE( UIUserGuideSlot);

namespace 
{
	bool AttachToFrontTargetScene(i3GameNode * pParent, i3UIScene * pScene , i3UIScene * pTarget)
	{
		if( pTarget != nullptr ) 
		{
			i3GameNode * pPrev =  pParent->getFirstChild();

			while( (pPrev != nullptr) && (pPrev->getNext() != pScene) )
				pPrev = pPrev->getNext();

			if( pPrev != nullptr )
			{
				pPrev->SetNext( pScene->getNext() );
				pScene->SetNext( pTarget->getNext() );
				pTarget->SetNext(pScene);
				return true;
			}
		}

		return false;
	}
}

UIUserGuideSlot::UIUserGuideSlot()
{
	m_pBtnCoin = nullptr;
	m_pBtnCoinEffect = nullptr;
	
	m_TargetAttr.Reset();

	m_rShapeChangeTime = 0.0f;
	m_bRecievedCoin = false;
	m_bDuplicate = false;
}

UIUserGuideSlot::~UIUserGuideSlot()
{
}

/*virtual*/ void UIUserGuideSlot::_InitializeAtLoad(i3UIScene * pScene)
{
	UIBase::_InitializeAtLoad(pScene);

	m_pBtnCoin = i3::kind_cast<i3UIButtonImageSet*>(pScene->FindChildByName("BtnImgSet_UG_Coin"));
	I3ASSERT(m_pBtnCoin != nullptr);

	m_pBtnCoinEffect = i3::kind_cast<i3UIButtonImageSet*>(pScene->FindChildByName("BtnImgSet_UG_CoinEffect"));
	I3ASSERT(m_pBtnCoinEffect != nullptr);
}

/*virtual*/ void UIUserGuideSlot::_ClearAtUnload(i3UIScene * pScene)
{
	UIBase::_ClearAtUnload(pScene);

	m_pBtnCoin = nullptr;
	m_pBtnCoinEffect = nullptr;

	m_TargetAttr.Reset();

	m_rShapeChangeTime = 0.0f;
	m_bRecievedCoin = false;
	m_bDuplicate = false;
}

void UIUserGuideSlot::_AdjustSceneDepth(void)
{
	// 생성시 최상위로 올립니다.
	i3UIScene * pScene = GetScene(0);

	i3GameNode * pParent = pScene->getParent();

	if( pParent != nullptr)
	{
		if( AttachToFrontTargetScene( pParent, pScene, m_TargetAttr._pScene ) == false )
			pScene->ChangeParent( pParent);	
	}
}

/*virtual*/ void UIUserGuideSlot::OnCreate(i3GameNode * pParent)
{
	UIBase::OnCreate( pParent);

	// load scene
	AddScene( "Scene/Main/PointBlankRe_UserGuide_Slot.i3UIs");

}

void UIUserGuideSlot::EntranceSlot(GuideTargetAttr * pAttr, bool bDuplicate)
{
	m_pFrameWnd->EnableCtrl(true);
	SetGuideAttr(pAttr, bDuplicate);
}

void UIUserGuideSlot::ExitSlot(void)
{
	m_pFrameWnd->EnableCtrl(false);

	INT32 i32LastEvent = m_TargetAttr._i32EventIdx;
	m_TargetAttr.Reset();
	m_TargetAttr._i32EventIdx = i32LastEvent;
}

/*virtual*/ void UIUserGuideSlot::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	if(m_bRecievedCoin == false &&  m_bDuplicate == false)
	{
		m_pBtnCoin->EnableCtrl(false);
		m_pBtnCoinEffect->EnableCtrl(true);
		m_bRecievedCoin = true;

		PlaySound(UISND_USER_GUIDE_COIN);
		_AdjustSceneDepth();
	}

	UIBase::ProcessGameEvent(evt, arg, UserData);
}

void UIUserGuideSlot::OnUpdateGuideSlot(REAL32 rDeltaSeconds)
{
	if(m_bDuplicate == false)
	{
		if(m_rShapeChangeTime >= 0.1f)
		{
			if(m_bRecievedCoin == false)
			{
				OnUpdateCoinAnim(m_pBtnCoin);
			}
			else
			{
				OnUpdateCoinAnim(m_pBtnCoinEffect);	
			}

			m_rShapeChangeTime = 0.0f;
		}

		m_rShapeChangeTime += rDeltaSeconds;
	}
}

void UIUserGuideSlot::OnUpdateCoinAnim(i3UIButtonImageSet * pCtrl)
{
	if(pCtrl->isEnable() == false)
		return;

	INT32 i32ShapeIdx = pCtrl->GetShapeIdx();

	++i32ShapeIdx;

	if(i32ShapeIdx >= pCtrl->getShapeCount())
	{
		i32ShapeIdx = 0;
		
		if(pCtrl == m_pBtnCoinEffect)
		{
			pCtrl->EnableCtrl(false);
		}
	}

	pCtrl->SetShapeIdx(i32ShapeIdx);
}

bool UIUserGuideSlot::SendClickEvent(bool bDBClick)
{
	if( m_bRecievedCoin == true || (m_bDuplicate == false &&m_pBtnCoin->isEnable() == false) )
		return false;

	UINT16 u16EventIdx = static_cast<UINT16>(GetEventIndex());
	
	if(bDBClick == true)
	{
		if( u16EventIdx != 7)
			return false;
	}
	else
	{
		if( u16EventIdx == 7)
			return false;
	} 

	if( u16EventIdx < GUIDE_EVENT_MAX)
	{
		PACKET_NEW_GUIDE_COMPLETE_REQ ReqPacket;
		ReqPacket.m_ui16GuideEventIdx = u16EventIdx;
		GameEventSender::i()->SetEvent(EVENT_USER_GUIDE, &ReqPacket);
		return true;
	}

	return false;
}

void UIUserGuideSlot::SetGuideAttr(GuideTargetAttr * pAttr, bool bDuplicate)
{
	m_TargetAttr.CopyFrom(pAttr);

	SetCoinPosition();

	if(bDuplicate == false)
	{
		m_pBtnCoin->EnableCtrl(true);
		m_pBtnCoinEffect->EnableCtrl(false);
	}
	else
	{
		m_pBtnCoin->EnableCtrl(false);
		m_pBtnCoinEffect->EnableCtrl(false);
	}

	m_bDuplicate = bDuplicate;
	m_rShapeChangeTime = 0.0f;
	m_bRecievedCoin = false;

	_AdjustSceneDepth();
}

void UIUserGuideSlot::SetCoinPosition(void)
{
	if(m_TargetAttr._pCtrl == nullptr)
		return;

	VEC3D * pTargetPos = m_TargetAttr._pCtrl->getAccumulatedPos();
	
	// 대상 컨트롤의 우측 상단에 코인 출력
	REAL32 rFramePosX = pTargetPos->x + ( (m_TargetAttr._pCtrl->getWidth() * 0.5f) - (m_pBtnCoin->getWidth() * 0.5f) );
	REAL32 rFramePosY = pTargetPos->y + ( (m_TargetAttr._pCtrl->getHeight() * 0.5f) - (m_pBtnCoin->getHeight() * 0.5f) );

	m_pFrameWnd->setPos(rFramePosX, rFramePosY);
}

void UIUserGuideSlot::ProcessLvBoxScroll(i3UIListView_Box * pLvBox)
{
	if(m_bDuplicate == true)
		return;

	bool bCtrlEnable = true;

	i3UIListView_Item * pItem = i3::same_cast<i3UIListView_Item*>(m_TargetAttr._pCtrl);

	if(pItem != nullptr)
	{
		REAL32 rTargetHeight = pItem->getHeight();
		REAL32 rOriginHeight = pLvBox->getBoundItem()->getHeight();

		if(rOriginHeight > rTargetHeight)
			bCtrlEnable = false;
		else
			SetCoinPosition();

		if(m_bRecievedCoin == true)
			m_pBtnCoinEffect->EnableCtrl(bCtrlEnable);
		else
			m_pBtnCoin->EnableCtrl(bCtrlEnable);
	}
}

void UIUserGuideSlot::ProcessSlotScroll(bool bEnableCtrl)
{
	if(m_bDuplicate == true)
		return;

	if(m_bRecievedCoin == true)
		m_pBtnCoinEffect->EnableCtrl(bEnableCtrl);
	else
		m_pBtnCoin->EnableCtrl(bEnableCtrl);
}



bool UIUserGuideSlot::isEndAnimation(void)
{
	if(m_bRecievedCoin == true && m_pBtnCoinEffect->isEnable() == false)
		return true;

	return false;
}

bool UIUserGuideSlot::isSameTargetEvent(UINT32 ui32Event)
{
	if(m_TargetAttr._i32EventIdx > -1 && (UINT32)m_TargetAttr._i32EventIdx == ui32Event)
		return true;

	return false;
}

bool UIUserGuideSlot::isSameTargetCtrl(i3UIControl * pCtrl)
{
	// 메달슬롯은 예외처리
	if( GetEventIndex() == GUIDE_EVENT_1ST_MEDAL)
	{
		while(i3::kind_of<i3UIControl*>(pCtrl->getParent()))
			pCtrl = i3::kind_cast<i3UIControl*>(pCtrl->getParent());
	}

	if(m_TargetAttr._pCtrl == nullptr || pCtrl == nullptr)
		return false;

	return m_TargetAttr._pCtrl == pCtrl;
}

bool UIUserGuideSlot::OnRevive( i3RenderContext * pCtx )
{
	if( UIBase::OnRevive( pCtx) == false)
		return false;

	SetCoinPosition();

	return true;
}
