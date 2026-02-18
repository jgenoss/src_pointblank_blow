#include "pch.h"
#include "UIHudDefAutoChange.h"

#include "UIHudDefine.h"
#include "UIHudUtil.h"
#include "UI/UIUtil.h"
#include "UI/UIHudManager.h"

#include "./StageBattle/ScoreContext.h"
#include "BattleSlotContext.h"
#include "MyRoomInfoContext.h"

#include "i3Base/profile/profile.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/format_string.h"


static i3Light * s_pLight = nullptr;

UIHudDefAutoChange::UIHudDefAutoChange(UIHudManager* p) : iHudBase(p), m_rPopupElapsedTime(0.f),
														m_bStarted(false), m_p3DView(nullptr), m_pObject(nullptr)
{
}

UIHudDefAutoChange::~UIHudDefAutoChange()
{
//	_ReleaseLight();
}

void UIHudDefAutoChange::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "i3UIFrameWnd0");
	iHudBase::AddFrameWnd(pScene, "Effect");

	i3UIFrameWnd * pCharaWnd= static_cast<i3UIFrameWnd*>(pScene->FindChildByName("Effect"));

	char strTemplate[MAX_STRING_COUNT] = {0,};
	for(INT32 i = 0; i < MAXTEAMSLOT; i++)
	{
		i3::snprintf( strTemplate, MAX_STRING_COUNT, "i3UIButtonImageSet%d",i); 
		m_apImgSetCtrl[i] = (i3UIButtonImageSet*) pScene->FindChildByName( strTemplate);
	}
	m_pTeamText = (i3UIStaticText*) pScene->FindChildByName( "i3UIStaticText4");

	if( pCharaWnd != nullptr)
	{
		_Create3DViewBox( pCharaWnd);
		_CreateSwitchObject();
	}
}

void UIHudDefAutoChange::ClearAtUnload( i3UIScene * pScene)
{
	_DestroySwitchObject();
	_Destroy3DViewBox();
}

void UIHudDefAutoChange::Update( REAL32 rDeltaSeconds )
{
	i3_prof_func();
	if (IsEnable() && m_pObject)
	{
		m_pObject->OnUpdate(rDeltaSeconds);
	}

//	_UpdatePopup(rDeltaSeconds);
}

void UIHudDefAutoChange::SetEnable(bool enable)
{
	iHudBase::SetEnable(enable);

	//if( enable == false)
	//	_ReleaseLight();

	return;
}

void UIHudDefAutoChange::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	SetEnable(false);
}
void UIHudDefAutoChange::event_first_respawn_battle(INT32 arg, const i3::user_data& UserData)
{
	SetEnable(false);
}
void UIHudDefAutoChange::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
//	SetEnable( false );
}
void UIHudDefAutoChange::event_respawn_battle(INT32 arg, const i3::user_data& UserData)
{
	SetEnable(false);
}

void UIHudDefAutoChange::SetParameter(INT32 arg1, INT32 arg2)
{
	i3_prof_func();

	if(m_bStarted == false)
	{
		m_bStarted = true;
		m_rPopupElapsedTime = 0.0f;
		m_pObject->OnChangeTime(0.0f);

		//_AddLight();

		// 이미 자동공수교대 전환으로 팀이바뀌어져있을수있으니 슬롯인덱스값을이용한 팀판별을한다.
		INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();
		CHARACTER_TEAM_TYPE teamType = BattleSlotContext::i()->getSvrTeamTypeValue(mySlotIdx);

		//if(BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_RED)
		if(teamType & CHARACTER_TEAM_RED)
		{
			// RED to BLUE
			m_apImgSetCtrl[0]->SetShapeIdx(0);
			m_apImgSetCtrl[1]->SetShapeIdx(0);

			if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
				MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
				m_pTeamText->SetText(GAME_RCSTRING("STR_CONVOY_CHTEAM_REDTOBLUE"));
			else if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
				MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_NORMAL))
				m_pTeamText->SetText(GAME_RCSTRING("STR_TBL_INGAME_HUD_CONVOY_FIRST_BLUE"));
			else if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) == true)
				m_pTeamText->SetText(GAME_RCSTRING("STR_BOMB_CHTEAM_REDTOBLUE"));
			else
				m_pTeamText->SetText(GAME_RCSTRING("STR_ELIMINATE_CHTEAM_REDTOBLUE"));
		}
		else
		{
			// BLUE to RED
			m_apImgSetCtrl[0]->SetShapeIdx(1);
			m_apImgSetCtrl[1]->SetShapeIdx(1);

			if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
				MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
				m_pTeamText->SetText(GAME_RCSTRING("STR_CONVOY_CHTEAM_BLUETORED"));
			else if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
				MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_NORMAL))
				m_pTeamText->SetText(GAME_RCSTRING("STR_TBL_INGAME_HUD_CONVOY_FIRST_RED"));
			else if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) == true)
				m_pTeamText->SetText(GAME_RCSTRING("STR_BOMB_CHTEAM_BLUETORED"));
			else
				m_pTeamText->SetText(GAME_RCSTRING("STR_ELIMINATE_CHTEAM_BLUETORED"));
		}

		iHudBase::SetEnable(true);
	}
}

void UIHudDefAutoChange::_Create3DViewBox( i3UIControl * pParent)
{
	I3ASSERT( pParent != nullptr);

	m_p3DView = i3UI3DViewBox::new_object();
	I3ASSERT( m_p3DView);

	I3ASSERT( pParent);
	pParent->AddChild( m_p3DView);

	m_p3DView->setSize( pParent->getWidth()-2, pParent->getHeight()-2);
	m_p3DView->setPos( 1, 1);
}

void UIHudDefAutoChange::_Destroy3DViewBox( void)
{
	if( m_p3DView != nullptr)
	{
		i3GameNode * pParent = m_p3DView->getParent();
		pParent->RemoveChild( m_p3DView);
		I3_SAFE_RELEASE( m_p3DView);
	}
}

void UIHudDefAutoChange::_CreateSwitchObject( void)
{
	m_rPopupElapsedTime = 0.0f;

	i3GameResObject * pRes = nullptr;

	if( BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_BLUE)
		pRes = i3::kind_cast<i3GameResObject*>( g_pFramework->QuaryResource( "Switch_Offence.i3Obj") );
	else
		pRes = i3::kind_cast<i3GameResObject*>( g_pFramework->QuaryResource( "Switch_Defense.i3Obj") );

	m_pObject = i3Object::new_object();
	I3ASSERT(m_pObject != nullptr);

	if( pRes)
	{
		i3Object* pObj = pRes->getGameObject();

		const INT32 numState = pObj->getStateCount();

		for (INT32 i = 0 ; i < numState ; ++i)
		{
			I3_OBJECT_STATE* pObjState = pObj->getState(i);

			i3Node* pNode =	pObjState->m_pSg->getInstanceSg();

			pNode->RemoveFlag(I3_NODEFLAG_ALPHASORT);			// 오브젝트의 알파소트 플래그가 뽑기상점에서 현재 버그가 나므로, 일단 플래그를 꺼둡니다.
		}														// 근본 해결이 아니기 때문에, 지속적으로 제가 뽑기상점에서의 알파소트 버그를 계속 살펴보겠습니다.
		
		m_pObject->Create(g_pFramework, pRes);
		m_pObject->OnChangeTime(0.0f);
	}


	m_pObject->Create(g_pFramework, pRes);
	m_pObject->OnChangeTime(0.0f);

	m_p3DView->Create3DView();
	m_p3DView->Set3DView( m_pObject->GetNode() );

	m_p3DView->OnVisibleScene();
	m_p3DView->get3DCamera()->addStyle(I3_CAM_STYLE_PROJECTION);	
	m_p3DView->get3DCamera()->addStyle(I3_CAM_STYLE_FULLFRAME);

	MATRIX * pCamMtx = m_p3DView->get3DCamera()->GetMatrix();
	i3Matrix::Identity( pCamMtx);
	i3Matrix::SetPos( pCamMtx, 0.f, 0.f, 4.8f, 1.0f);
	m_p3DView->get3DCamera()->SetFOV(I3_DEG2RAD( 45.0f));		
}

void UIHudDefAutoChange::_DestroySwitchObject( void)
{
	if(m_pObject != nullptr)
	{
		i3Node * pParent = m_pObject->GetNode()->GetParent();
		if(pParent != nullptr )
			pParent->RemoveChild( m_pObject->GetNode() );
		I3_SAFE_RELEASE(m_pObject);
	}
}

void UIHudDefAutoChange::_AddLight( void)
{
	VEC3D d_rot;
	COLORREAL d_diffuse, d_specular;

	if( s_pLight == nullptr)
	{
		s_pLight = i3Light::new_object();
		s_pLight->SetLightType( I3G_LIGHT_DIRECTIONAL);

		i3Color::Set( &d_diffuse, 1.2f, 1.2f, 1.2f, 1.0f);
		i3Color::Set( &d_specular, 1.0f, 1.0f, 1.0f, 1.0f);
		i3Vector::Set( &d_rot, -12.0f, -34.0f, 4.0f);

		s_pLight->SetDiffuse( &d_diffuse);
		s_pLight->SetSpecular( &d_specular);

		MATRIX mtx;

		i3Matrix::SetTranslate( &mtx, 0.0f, 0.0f, getZ( &d_rot));
		i3Matrix::PostRotateX( &mtx, I3_DEG2RAD( getY( &d_rot)));
		i3Matrix::PostRotateY( &mtx, I3_DEG2RAD( getX( &d_rot)));

		s_pLight->SetWorldSpaceTransform( &mtx);

		g_pFramework->addLight( s_pLight);
	}
	else
	{
		I3_MUST_ADDREF( s_pLight);
	}

}

void UIHudDefAutoChange::_ReleaseLight( void)
{
	if( g_pFramework && s_pLight)
	{
		I3_MUST_RELEASE(s_pLight);

		if( s_pLight->GetRefCount() == 1)
		{
			g_pFramework->removeLight( s_pLight);
			s_pLight = nullptr;
		}
	}
}

void UIHudDefAutoChange::_UpdatePopup(REAL32 rDeltaSeconds)
{
//	if (iHudBase::IsEnable() == true)
//	{
//		m_rPopupElapsedTime += rDeltaSeconds;
//		if (m_rPopupElapsedTime >3.0f)
//		{
//	//		I3TRACE("- End : %f", m_rPopupElapsedTime);
//			//I3PRINTLOG(I3LOG_NOTICE, "- End : %f", m_rPopupElapsedTime);
//			iHudBase::SetEnable(false);
//		}
//	//	I3PRINTLOG(I3LOG_NOTICE, "- m_rPopupElapsedTime : %f", m_rPopupElapsedTime);
////		I3TRACE("- m_rPopupElapsedTime : %f", m_rPopupElapsedTime);
//	}
}