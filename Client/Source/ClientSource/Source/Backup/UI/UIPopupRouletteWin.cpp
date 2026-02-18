#include "pch.h"
#include "UIPopupRouletteWin.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"
#include "UIRouletteView.h"


#include "../GuiNotifyReceiver.h"
#include "../TextSet.h"

#include "i3Base/string/ext/sprintf.h"

I3_CLASS_INSTANCE( UIPopupRoulettWin);//, UIPopupBase);


UIPopupRoulettWin::UIPopupRoulettWin()
{
	m_ImgBox = NULL;
	m_idxEffect = -1;

	m_p3DView = NULL;

	m_rElapsedTime = 0.f;
	m_rElapsedTime1 = 0.f;
}

UIPopupRoulettWin::~UIPopupRoulettWin()
{

	
}

void UIPopupRoulettWin::_ModifyControl( void)
{


}
void UIPopupRoulettWin::SetName()
{

		i3::string Name;
		UINT32 pitemid = g_pGameContext->GetRSItemID() ;
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
				//i3::safe_string_copy(Name, pInfo->GetName(), MAX_STRING_COUNT);
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
		}

	
	LuaState * L = _CallLuaFunction(0, "setItemName");
	
	if( L != NULL)
	{


		i3Lua::PushA2UTF8String( L, Name.c_str());
		_EndLuaFunction( L, 1);
	}




}

/*virtual*/ void UIPopupRoulettWin::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene( "UIRe/Scene/Popup/PBRe_PopUp_Win.i3UIe");

	
	
}

/*virtual*/ bool UIPopupRoulettWin::OnKey_Enter( void)
{
	ConfirmOK();
	return true;
}

/*virtual*/ bool UIPopupRoulettWin::OnKey_Escape( void)
{
	ConfirmOK();
	return true;
}

/*virtual*/ bool UIPopupRoulettWin::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	_ModifyControl();

	CreateViewEffect();
	
	
	return true;
}

void UIPopupRoulettWin::OnUpdate( REAL32 rDeltaSeconds)
{

	UIPopupBase::OnUpdate(rDeltaSeconds);

}
/*virtual*/ bool UIPopupRoulettWin::OnOKButton( void)
{
	ConfirmOK();

	return true;
}

void UIPopupRoulettWin::ConfirmOK(void)
{
	g_pFramework->GetUIMainframe()->TogglePopup( UPW_ROULETTE_WIN);
}

/*virtual*/ void UIPopupRoulettWin::OnLoadAllScenes()
{
	if (GetScene(0) != NULL)
	{
		
		m_ImgBox = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet0");
	
		m_ImgBox->SetVisible(FALSE);
		i3UIFrameWnd * pUIWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIFrameWnd1");
		pUIWnd->SetVisible(FALSE);


		i3UIFrameWnd * pUIWnd2 = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIFrameWnd");
		pUIWnd2->SetVisible(FALSE);
		
		i3UIFrameWnd * pUIWnd3 = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIFrameWnd3");
		pUIWnd3->SetVisible(FALSE);


		i3UIFrameWnd * pUWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIButtonImageSet0");
		pUWnd->SetVisible(FALSE);

		i3UIFrameWnd * pImageWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIImageBox0");
		pImageWnd->SetVisible(FALSE);


		

		i3UIFrameWnd * pCharaWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "Effect");
		if( pCharaWnd != NULL)
			Create3DViewBox( pCharaWnd);
		
		i3UIFrameWnd * pWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIFrameWnd2");
		
		i3Vector::Copy( &m_vOriginalSize, pWnd->getSize());
		m_slide.AddFrameWnd(pWnd, UISlide::POPUP);
		m_slide.SetOriginalPos(381.f,282.f);

		if( m_bScaleAnimation)
		{

		
			m_slide.SetTargetTime(false, 1.f / 10.f );
			m_slide.SetTargetTime(true, 1.f / 2.f);
		}
		else
		{
			m_slide.SetTargetTime(false, 0.f );
			m_slide.SetTargetTime(true, 0.f);
		}
	}
	else
	{
		m_slide.SetTargetTime(false, 0.f);
		m_slide.SetTargetTime(true, 0.f);
	}

	

	
}

/*	virtual*/ void	UIPopupRoulettWin::OnEntranceEnd( void)
{

	i3UIFrameWnd * pUIWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIFrameWnd1");
	pUIWnd->SetVisible(true);

	i3UIFrameWnd * pUWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIButtonImageSet0");
	pUWnd->SetVisible(true);

	i3UIFrameWnd * pImageWnd = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "i3UIImageBox0");
	pImageWnd->SetVisible(true);


	m_ImgBox->SetVisible(true);

	SetName();
	OpenEventWin(g_pGameContext->GetRSItemID());

	g_pSndMng->StartRouletteSnd(GSND_ROULETTE_WIN,m_p3DView->getPos(),false);

}

/*virtual*/ void UIPopupRoulettWin::OnUnloadAllScenes()
{

	



	I3_SAFE_RELEASE(m_ImgBox);

	Delete3DViewBox();
}

void UIPopupRoulettWin::OpenEventWin(INT32 ItemId)
{
	GetItemImageMgr()->SelectImage(m_ImgBox,ItemId);
}



void UIPopupRoulettWin::Create3DViewBox( i3UIControl * pParent)
{
	I3ASSERT( pParent != NULL);

	m_p3DView = i3UI3DViewBox::new_object();
	I3ASSERT( m_p3DView);

	I3ASSERT( pParent);
	pParent->AddChild( m_p3DView);

	m_p3DView->setSize( pParent->getWidth()-2, pParent->getHeight()-2);
	m_p3DView->setPos( 1, 1);
}

void UIPopupRoulettWin::Delete3DViewBox( void)
{
	if( m_p3DView != NULL)
	{
		i3GameNode * pParent = m_p3DView->getParent();
		pParent->RemoveChild( m_p3DView);
		I3_SAFE_RELEASE( m_p3DView);
	}
}

void UIPopupRoulettWin::CreateViewEffect()
{

	VEC3D vPos;


	{
	
		i3GameRes * pRes1 = g_pFramework->LoadResource( "Effect/H/Roulette_hit.i3s", 0);
		i3GameResSceneGraph * pSgRes1 = (i3GameResSceneGraph *) pRes1;
		i3Node * pNewNode = i3Node::new_object_ref();

		i3Node * pFindNode1 = i3Scene::FindNodeByName(pSgRes1->getScene(), "i3TimeSequence");
		if (pFindNode1 !=NULL)
		{
			pFindNode1->OnChangeTime(0.f);

		}
		

			
		i3GameRes * pRes = g_pFramework->LoadResource( "Effect/H/Flame01.i3s", 0);
		i3GameResSceneGraph * pSgRes = (i3GameResSceneGraph *) pRes;
		i3Node * pFindNode = i3Scene::FindNodeByName(pSgRes->getScene(), "i3TimeSequence");
		if (pFindNode !=NULL)
		{
			pFindNode->OnChangeTime(0.f);

		}

		
		
		pRes = g_pFramework->LoadResource( "Effect/H/Flame01.i3s", 0);
		i3GameResSceneGraph * pSgRess = (i3GameResSceneGraph *) pRes;
		pFindNode = i3Scene::FindNodeByName(pSgRess->getScene(), "i3TimeSequence");
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




		if (g_pGameContext->GetRSItemGrade() != RS_ITEM_GRADE_LOSE)
		{
			pNewNode->AddChild( pSgRes1->getScene());
			pNewNode->AddChild( pSgRes->getScene());
			pNewNode->AddChild( pSgRes2->getScene());
			pNewNode->AddChild( pSgRes3->getScene());
			pNewNode->AddChild( pSgRes4->getScene());
			pNewNode->AddChild( pSgRes5->getScene());
			pNewNode->AddChild( pSgRes6->getScene());
			pNewNode->AddChild( pSgRes7->getScene());
			pNewNode->AddChild( pSgRes8->getScene());
			pNewNode->AddChild(pSgRess->getScene());

		}

		m_p3DView->Create3DView();
		m_p3DView->Set3DView( pNewNode);
	
	}


	m_p3DView->OnVisibleScene();
	MATRIX * pCamMtx = m_p3DView->get3DCamera()->GetMatrix();
	i3Matrix::Identity( pCamMtx);
	i3Matrix::SetPos( pCamMtx, 0.17f, 0.92f, 90.0f, 1.0f);
	m_p3DView->get3DCamera()->SetFOV(I3_DEG2RAD( 41.5f));
	i3Vector::Set( &vPos, 0.f, 0.0f, -7.0f);

}

