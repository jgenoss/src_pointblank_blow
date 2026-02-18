#include "pch.h"
#include "UIPopupGachaResult.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"
#include "UIPhaseGachaShop.h"
#include "UIUtil.h"

#include "../TextSet.h"

#include "ClanGameContext.h"
#include "GachaContext.h"
#include "ShopContext.h"

I3_CLASS_INSTANCE( UIPopupGachaResult);//, UIPopupBase);

extern "C" 
{
	INT32 Result_Clieck_Ok(LuaState* L)
	{
		UIPopupGachaResult * pThis = static_cast<UIPopupGachaResult*>(GameUI::GetPopup(UPW_GACHA_RESULT));
		if( pThis != nullptr)
			pThis->OnClieck_OK();
		return 0;
	}
}

LuaFuncReg UIPopup_Gacha_Result[] =
{
	{ "Clieck_Ok",	Result_Clieck_Ok },

	{nullptr,			nullptr}
};

UIPopupGachaResult::UIPopupGachaResult() : m_bDelay(false), m_rDelayTime(0.f)
{

}

UIPopupGachaResult::~UIPopupGachaResult()
{

}

/*virtual*/ void UIPopupGachaResult::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PBRe_PopUp_Lottery_congratulations.i3UIs", UIPopup_Gacha_Result);
}

/*virtual*/ void UIPopupGachaResult::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);

}

/*virtual*/ void UIPopupGachaResult::OnLoadAllScenes()
{

}

/*virtual*/ void UIPopupGachaResult::OnUnloadAllScenes()
{

	for(INT32 i = 0; i< 4; i++)
	{
		m_goods_img_ctrl[i]->RestoreSizePos();
		I3_SAFE_RELEASE(m_goods_img_ctrl[i]);
	}

	I3_SAFE_RELEASE(m_pFlameNode);
	I3_SAFE_RELEASE(m_pNewNode);
	Delete3DViewBox();

}

/*virtual*/ bool UIPopupGachaResult::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	LuaState * L = _CallLuaFunction( "__init");
	if( L != nullptr)
	{
		_EndLuaFunction(L, 0);
	}

	return true;
}


/*	virtual*/ void	UIPopupGachaResult::OnEntranceEnd( void)
{
	//기본정보를 세팅합니다.

	m_pNewNode = i3Node::new_object();
	m_pFlameNode = i3AttrSet::new_object();

	m_pNewNode->AddChild(m_pFlameNode);

	SetInputDisable(false);
	CreateViewEffect();

	SetUiText();
}

bool UIPopupGachaResult::OnExitStart( void)
{	
	GetMainFrame()->EnableFloatWin(UFW_CHAT);
	GetMainFrame()->EnableFloatWin(UFW_USERLIST);	

	return base_type::OnExitStart();
}


/*virtual*/ void	UIPopupGachaResult::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	i3UIButtonImageSet* btnset = nullptr;
		
	btnset = (i3UIButtonImageSet*) pScene->FindChildByName( "i3UIButtonImageSetRandom");
	m_goods_img_ctrl[0] = GetItemImageMgr()->CreateImgSetCtrl( btnset);

	btnset = (i3UIButtonImageSet*) pScene->FindChildByName( "i3UIButtonImageSetItem");
	m_goods_img_ctrl[1] = GetItemImageMgr()->CreateImgSetCtrl( btnset);

	btnset = (i3UIButtonImageSet*) pScene->FindChildByName( "i3UIButtonImageSetLucky");
	m_goods_img_ctrl[2] = GetItemImageMgr()->CreateImgSetCtrl( btnset);

	btnset = (i3UIButtonImageSet*) pScene->FindChildByName( "i3UIButtonImageSetHonor");
	i3UICaptionControl* btnCap = (i3UICaptionControl*)btnset->FindChildByName("i3UIButtonComposedHonor");
	if(btnCap)
		btnCap->SetText(GAME_RCSTRING("STBL_IDX_BUTTON_OK"));

	m_goods_img_ctrl[3] = GetItemImageMgr()->CreateImgSetCtrl( btnset);
	
	i3UIFrameWnd * pCharaWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "bg");
	if( pCharaWnd != nullptr)
		Create3DViewBox( pCharaWnd);
}

/*virtual*/ bool UIPopupGachaResult::OnClick( i3UIControl * pControl )
{
	if(i3::same_of<i3UIButtonComposed3*>(pControl) == true)
	{
		// 버튼 클릭시 사운드를 끄고 OK 버튼 클릭 사운드로 통일 합니다.
		g_pSndMng->StartLotterySnd(GSND_LOTTERY_POPUPCHECK);
	}

	return false;
}

/*virtual*/ bool UIPopupGachaResult::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();

	if( pKeyboard->GetStrokeState( I3I_KEY_ENTER))
	{
		if( OnKey_Enter())
			return true;
	}

	return false;
}

bool	UIPopupGachaResult::OnKey_Enter( void)
{
	g_pSndMng->StartLotterySnd(GSND_LOTTERY_POPUPCHECK);

	UIPopupGachaResult::OnClieck_OK();
	return true;		// 처리된 경우에만 true
}

void UIPopupGachaResult::_UpdatePopup(REAL32 rDeltaSeconds)
{
}

void UIPopupGachaResult::SetUiText()
{
	if(GachaContext::i()->GetGachaRewardCnt() < 1)
	{
#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		I3ASSERT("ERR! - There is no gacha reward data..");
#endif
		OnClieck_OK();
		return;
	}

	GachaContext::GACHA_REAWARD reward;	
	GachaContext::i()->PopGachaReward(reward);

	UINT32 pitemid =  CShopContext::i()->ConvertGoodsID(reward.m_rewardGoodsID);

	i3::rc_wstring itemNameStr;
	usf::GetShopItemName(itemNameStr, pitemid);

	//const SHOP_GOODS* rewardGoods = CShopContext::i()->GetGoods(pitemid);
	i3::wstring itemPeroidStr;
	//if(rewardGoods)
	//{
	//	usf::GetShopPeriodCountString(const_cast<SHOP_GOODS*>(rewardGoods), itemPeroidStr);
	//}

	usf::GetShopPeriodCountStringByGoodsID(reward.m_rewardGoodsID, itemPeroidStr);

	i3::wstring itemNameStrFinal;
	i3::sprintf(itemNameStrFinal, L"%s %s", itemNameStr, itemPeroidStr);

	if(pitemid == 0)
	{
#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		{
			I3ASSERT(0 && "ERR! - failed to find item id for goodsid");
			I3TRACE("ERR! - failed to find item id for goodsid[%d]", reward.m_rewardGoodsID);
		}
#endif
		OnClieck_OK();
		return;
	}
	GachaContext::i()->ShowJackpotAnnounceChat(pitemid);

	LuaState * L = nullptr;

	for(INT32 i = 0 ; i< 14; i++)
	{
		i3Node * pFindNode = nullptr;
		pFindNode = i3Scene::FindNodeByName(m_pSgRes[i]->getScene(), "i3TimeSequence");
		if (pFindNode !=nullptr)
		{
			pFindNode->OnChangeTime(10000.f);
		}
	}

	switch(reward.m_result)
	{
		case GACHA_RESULT_LUCKY:
		{
			for(INT32 i = 0 ; i< 14; i++)
			{
				i3Node * pFindNode = nullptr;
				pFindNode = i3Scene::FindNodeByName(m_pSgRes[i]->getScene(), "i3TimeSequence");
				if (pFindNode !=nullptr)
				{
					pFindNode->OnChangeTime(0.f);
				}
			}
			GetItemImageMgr()->SelectImage( m_goods_img_ctrl[2], pitemid);
			L = _CallLuaFunction( "SetItemNameLucky");
			
			// Lucky 당첨이 되면 당첨 사운드가 출력.
			g_pSndMng->StartLotterySnd(GSND_LOTTERY_LUCKY);
		}
		break;

		case GACHA_RESULT_WIN:
		{
			for(INT32 i = 0 ; i< 14; i++)
			{
				i3Node * pFindNode = nullptr;
				pFindNode = i3Scene::FindNodeByName(m_pSgRes[i]->getScene(), "i3TimeSequence");
				if (pFindNode !=nullptr)
				{
					pFindNode->OnChangeTime(0.f);
				}
			}
			GetItemImageMgr()->SelectImage( m_goods_img_ctrl[1], pitemid);
			L = _CallLuaFunction( "SetItemName");
			
			// Win 아이템은 다른 처리를 합니다.
			g_pSndMng->StartLotterySnd(GSND_LOTTERY_CONGRATULATION);
		}
		break;

		case GACHA_RESULT_RANDOM:
		{
			GetItemImageMgr()->SelectImage( m_goods_img_ctrl[0], pitemid);
			L = _CallLuaFunction( "SetItemNameRandom");
		}
		break;

		case GACHA_RESULT_GLORY:
			{
				INT32 gachaItemKey = GachaContext::i()->GetGachaRstGoodsItemKey();
				GACHA_GROUP gachaGrp = GachaContext::i()->GetGachaRstGoodsGroup();

				const GACHA_ITEM_INFO* gachaItemInfo = GachaContext::i()->GetGachaItemInfo(gachaGrp, gachaItemKey);

				INT32 numPurchasedForGlory = 0;
				if(gachaItemInfo)
					numPurchasedForGlory = gachaItemInfo->m_i32GloryCount;

				GetItemImageMgr()->SelectImage( m_goods_img_ctrl[3], pitemid);
				
				L = _CallLuaFunction( "SetHonorResultTexts");

				if( L != nullptr)
				{
					i3::wstring gloryCaption = GAME_STRING("STR_LUCKYSHOP_GLORYITEM_POPUP_TITLE"); // 영예아이템지급
					i3::wstring gloryTitle; // STR_LUCKYSHOP_GLORYITEM_POPUP 축하 합니다!\n %d회 구매하셨으므로, %s 아이템을 드립니다.
					i3::sprintf(gloryTitle, GAME_STRING("STR_LUCKYSHOP_GLORYITEM_POPUP"), numPurchasedForGlory, itemNameStrFinal.c_str());
					
					i3Lua::PushStringUTF16To8(L, gloryCaption);
					i3Lua::PushStringUTF16To8(L, gloryTitle);
					i3Lua::PushStringUTF16To8(L, itemNameStrFinal.c_str(), itemNameStrFinal.size()); // 아이템이름
					_EndLuaFunction(L, 3);
				}
			}
			return;

		default:
			{
#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
				{
					I3ASSERT(0 && "ERR! - Invalid gacha result");
					I3TRACE("ERR! - Invalid gacha result=[%d]", reward.m_result);
				}
#endif
				OnClieck_OK();
				return;
			}
	}
	
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, itemNameStrFinal.c_str(), itemNameStrFinal.size());	// 아이템이름
		_EndLuaFunction(L, 1);
	}
}

void UIPopupGachaResult::OnClieck_OK()
{
	if( GachaContext::i()->GetGachaRewardCnt() > 0)
	{
		SetUiText();
	}
	else
	{
		GetMainFrame()->ClosePopup(UPW_GACHA_RESULT);
		GachaContext::i()->SetMyBuy(false);
	}
}


void UIPopupGachaResult::Create3DViewBox( i3UIControl * pParent)
{
	I3ASSERT( pParent != nullptr);

	m_p3DView = i3UI3DViewBox::new_object();
	I3ASSERT( m_p3DView);

	I3ASSERT( pParent);
	pParent->AddChild( m_p3DView);

	m_p3DView->setSize( pParent->getWidth()-2, pParent->getHeight()-2);
	m_p3DView->setPos( 1, 1);
}

void UIPopupGachaResult::Delete3DViewBox( void)
{
	if( m_p3DView != nullptr)
	{
		i3GameNode * pParent = m_p3DView->getParent();
		pParent->RemoveChild( m_p3DView);
		I3_SAFE_RELEASE( m_p3DView);
	}
}

void UIPopupGachaResult::CreateViewEffect()
{

	VEC3D vPos;
	i3GameRes * pRes[14] = {nullptr,};
	i3Node * pFindNode = nullptr;
			
	pRes[0] = 	g_pFramework->LoadResource( "Effect/H/Flame01.i3s", 0);
	pRes[1] = 	g_pFramework->LoadResource( "Effect/H/Flame02.i3s", 0);
	pRes[2] = 	g_pFramework->LoadResource( "Effect/H/Flame03.i3s", 0);
	pRes[3] = 	g_pFramework->LoadResource( "Effect/H/Flame04.i3s", 0);
	pRes[4] = 	g_pFramework->LoadResource( "Effect/H/Flame05.i3s", 0);
	pRes[5] = 	g_pFramework->LoadResource( "Effect/H/Flame06.i3s", 0);
	pRes[6] = 	g_pFramework->LoadResource( "Effect/H/Flame07.i3s", 0);
	pRes[7] = 	g_pFramework->LoadResource( "Effect/H/Flame08.i3s", 0);
	pRes[8] =	g_pFramework->LoadResource( "Effect/H/Flame09.i3s", 0);
	pRes[9]  = 	g_pFramework->LoadResource( "Effect/H/Flame10.i3s", 0);
	pRes[10] = 	g_pFramework->LoadResource( "Effect/H/Flame11.i3s", 0);
	pRes[11] = 	g_pFramework->LoadResource( "Effect/H/Flame12.i3s", 0);
	pRes[12] = 	g_pFramework->LoadResource( "Effect/H/Flame13.i3s", 0);
	pRes[13] = 	g_pFramework->LoadResource( "Effect/H/Flame14.i3s", 0);

	for(INT32 i = 0 ; i< 14; i++)
	{
		m_pSgRes[i] = (i3GameResSceneGraph *) pRes[i];
		m_pFlameNode->AddChild( m_pSgRes[i]->getScene());
		pFindNode = i3Scene::FindNodeByName(m_pSgRes[i]->getScene(), "i3TimeSequence");
		if (pFindNode !=nullptr)
		{
			pFindNode->OnChangeTime(10000.f);
		}
	}

	i3ParticleRenderNode * pParticleNode = i3ParticleRenderNode::new_object_ref();
	m_pNewNode->AddChild( pParticleNode);

	m_p3DView->Create3DView();
	m_p3DView->Set3DView( m_pNewNode);
	m_p3DView->OnVisibleScene();
	MATRIX * pCamMtx = m_p3DView->get3DCamera()->GetMatrix();
	i3Matrix::Identity( pCamMtx);
	i3Matrix::SetPos( pCamMtx, 0.17f, 0.92f, 90.0f, 1.0f);
	m_p3DView->get3DCamera()->SetFOV(I3_DEG2RAD( 41.5f));
	i3Vector::Set( &vPos, 0.f, 0.0f, -7.0f);

}