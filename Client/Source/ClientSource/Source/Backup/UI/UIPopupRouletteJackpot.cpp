#include "pch.h"
#include "UIPopupRouletteJackpot.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"

#include "../GuiNotifyReceiver.h"
#include "../TextSet.h"
#include "i3Base/string/ext/sprintf.h"


I3_CLASS_INSTANCE( UIPopupRouletteJackpot);//, UIPopupBase);

UIPopupRouletteJackpot::UIPopupRouletteJackpot()
{
	m_p3DView = NULL;

	m_nIndex = 0;
}

UIPopupRouletteJackpot::~UIPopupRouletteJackpot()
{
}

void UIPopupRouletteJackpot::_ModifyControl( void)
{
}

/*virtual*/ void UIPopupRouletteJackpot::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "UIRe/Scene/Popup/PBRe_PopUp_Jackpot.i3UIe");

}
/*virtual*/ void UIPopupRouletteJackpot::OnLoadAllScenes()
{
	if (GetScene(0) != NULL)
	{
		i3UIFrameWnd * pCharaWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "Effect");
		if( pCharaWnd != NULL)
			Create3DViewBox( pCharaWnd);
	}
}

/*virtual*/ void UIPopupRouletteJackpot::OnUnloadAllScenes()
{
	Delete3DViewBox();
}

/*virtual*/ bool UIPopupRouletteJackpot::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	_ModifyControl();
	

	CreateViewEffect();

	SetName();

	

	return true;
}


/*	virtual*/ void	UIPopupRouletteJackpot::OnEntranceEnd( void)
{
	g_pSndMng->StartRouletteSnd(GSND_ROULETTE_JACKPOT,m_p3DView->getPos(),false);
}

void UIPopupRouletteJackpot::Create3DViewBox( i3UIControl * pParent)
{
	I3ASSERT( pParent != NULL);

	m_p3DView = i3UI3DViewBox::new_object();
	I3ASSERT( m_p3DView);

	I3ASSERT( pParent);
	pParent->AddChild( m_p3DView);

	m_p3DView->setSize( pParent->getWidth()-2, pParent->getHeight()-2);
	m_p3DView->setPos( 1, 1);
}

void UIPopupRouletteJackpot::Delete3DViewBox( void)
{
	if( m_p3DView != NULL)
	{
		i3GameNode * pParent = m_p3DView->getParent();
		pParent->RemoveChild( m_p3DView);
		I3_SAFE_RELEASE( m_p3DView);
	}
}
void UIPopupRouletteJackpot::CreateViewEffect()
{

	VEC3D vPos;

	
	i3GameRes * pRes = g_pFramework->LoadResource( "Effect/H/Roulette_Homerun.i3s", 0);
	i3GameResSceneGraph * pSgRes = (i3GameResSceneGraph *) pRes;
	i3Node * pNewNode = i3Node::new_object_ref();

	i3Node * pFindNode = i3Scene::FindNodeByName(pSgRes->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);
	}
	

	pRes = g_pFramework->LoadResource( "Effect/H/Flame01.i3s", 0);
	i3GameResSceneGraph * pSgRes1 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes1->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}
	
	
	pRes = g_pFramework->LoadResource( "Effect/H/Flame02.i3s", 0);
	i3GameResSceneGraph * pSgRes2 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes2->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}

	pRes = g_pFramework->LoadResource( "Effect/H/Flame03.i3s", 0);
	i3GameResSceneGraph * pSgRes3 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes3->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}

	pRes = g_pFramework->LoadResource( "Effect/H/Flame04.i3s", 0);
	i3GameResSceneGraph * pSgRes4 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes4->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}
	pRes = g_pFramework->LoadResource( "Effect/H/Flame05.i3s", 0);
	i3GameResSceneGraph * pSgRes5 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes5->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}


	pRes = g_pFramework->LoadResource( "Effect/H/Flame06.i3s", 0);
	i3GameResSceneGraph * pSgRes6 = (i3GameResSceneGraph *) pRes;
	pFindNode= i3Scene::FindNodeByName(pSgRes6->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}


	pRes = g_pFramework->LoadResource( "Effect/H/Flame07.i3s", 0);
	i3GameResSceneGraph * pSgRes7 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes7->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}


	pRes = g_pFramework->LoadResource( "Effect/H/Flame08.i3s", 0);
	i3GameResSceneGraph * pSgRes8 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes8->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}

	pRes = g_pFramework->LoadResource( "Effect/H/Flame09.i3s", 0);
	i3GameResSceneGraph * pSgRes9 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes9->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}


	pRes = g_pFramework->LoadResource( "Effect/H/Flame10.i3s", 0);
	i3GameResSceneGraph * pSgRes10 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes10->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}


	pRes = g_pFramework->LoadResource( "Effect/H/Flame11.i3s", 0);
	i3GameResSceneGraph * pSgRes11 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes11->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}


	pRes = g_pFramework->LoadResource( "Effect/H/Flame12.i3s", 0);
	i3GameResSceneGraph * pSgRes12 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes12->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}


	pRes = g_pFramework->LoadResource( "Effect/H/Flame13.i3s", 0);
	i3GameResSceneGraph * pSgRes13 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes13->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}


	pRes = g_pFramework->LoadResource( "Effect/H/Flame14.i3s", 0);
	i3GameResSceneGraph * pSgRes14 = (i3GameResSceneGraph *) pRes;
	pFindNode = i3Scene::FindNodeByName(pSgRes14->getScene(), "i3TimeSequence");
	if (pFindNode !=NULL)
	{
		pFindNode->OnChangeTime(0.f);

	}





	{
		pNewNode->AddChild( pSgRes->getScene());
		pNewNode->AddChild( pSgRes1->getScene());
		pNewNode->AddChild( pSgRes2->getScene());
		pNewNode->AddChild( pSgRes3->getScene());
		pNewNode->AddChild( pSgRes4->getScene());
		pNewNode->AddChild( pSgRes5->getScene());
		pNewNode->AddChild( pSgRes6->getScene());
		pNewNode->AddChild( pSgRes7->getScene());
		pNewNode->AddChild( pSgRes8->getScene());
		pNewNode->AddChild( pSgRes9->getScene());
		pNewNode->AddChild( pSgRes10->getScene());
		pNewNode->AddChild( pSgRes11->getScene());
		pNewNode->AddChild( pSgRes12->getScene());
		pNewNode->AddChild( pSgRes13->getScene());
		pNewNode->AddChild( pSgRes14->getScene());

	

	}

	m_p3DView->Create3DView();
	m_p3DView->Set3DView( pNewNode);

	m_p3DView->OnVisibleScene();
	MATRIX * pCamMtx = m_p3DView->get3DCamera()->GetMatrix();
	i3Matrix::Identity( pCamMtx);
	i3Matrix::SetPos( pCamMtx, 0.17f, 0.92f, 90.0f, 1.0f);
	m_p3DView->get3DCamera()->SetFOV(I3_DEG2RAD( 41.5f));
	i3Vector::Set( &vPos, 0.f, 0.0f, -7.0f);

}

void UIPopupRouletteJackpot::SetName()
{
	i3::string Name;
	UINT32 pitemid =  g_pGameContext->ConvertGoodsID(g_pGameContext->GetRSBaseInfo()->_ui32JackPotGoodsID);
	INT32 goodsClass = GET_ITEM_FLAG_TYPE(pitemid);
	INT32 num = GET_ITEM_FLAG_NUMBER( pitemid);

	switch((ITEM_TYPE) goodsClass)
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
		{
			WEAPON_CLASS_TYPE type = (WEAPON_CLASS_TYPE) GET_ITEM_FLAG_CLASS( pitemid);
			CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(type, num);
			pInfo->GetWeaponName( Name);
		}
		break;
	case ITEM_TYPE_CHARA:
	case ITEM_TYPE_HEADGEAR:
		{
			const CEquipInfo * pEquipInfo = g_pCharaInfoDataBase->GetInfo(pitemid);
			if( pEquipInfo)
			{
				g_pCharaInfoDataBase->GetCharacterName(pitemid, Name); //sizeof(outItemName));
			}		
			else
			{
				i3::sprintf(Name, "%s \nItem:%d", INVALID_TEXT,   pitemid);
			}
		}
		break;
	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_WRAP_PERIOD:
	case ITEM_TYPE_WRAP_COUNT:

		{
			ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(pitemid);
			if( pInfo )
			{
				g_pShopItemInfoDataBase->GetItemName(pitemid, Name);
			}
			else
				i3::sprintf(Name, "%s\nItem:%d", INVALID_TEXT, pitemid);
		}
		break;
	case ITEM_TYPE_POINT:	// 포인트일 경우. 2012.01.13 노준영.
		{
			UINT32 ui32Point = GET_POINTITEM( pitemid );
			i3::sprintf(Name,"%d",ui32Point );
		}
		break;
	}


	LuaState * L = _CallLuaFunction(0, "setName");

	if( L != NULL)
	{


		i3Lua::PushA2UTF8String( L, Name.c_str());
		_EndLuaFunction( L, 1);
	}




}