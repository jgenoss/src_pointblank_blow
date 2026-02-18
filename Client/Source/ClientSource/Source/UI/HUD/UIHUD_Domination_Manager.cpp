#include "pch.h"

#include "UIHUD_Domination_Manager.h"

#include "../UIBattleFrame.h"
#include "../../Mode/Domination/DominationSkillSystem.h"
#include "DomiRevivalController.h"

#include "../../BattleHUD.h"
#include "../../ConfigRenderBattle.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

namespace DominationUI
{
	void DinoHp::create()
	{
		m_pHUDRoot = i3AttrSet::new_object();
		m_pTexture = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD3);

		//
		m_pSprite = i3Sprite2D::new_object();
		m_pSprite->SetTexture( m_pTexture);
		m_pSprite->Create( 1, true, true);
		m_pSprite->SetEnable( false);

		INT32 w = g_pViewer->GetViewWidth();
		INT32 h = g_pViewer->GetViewHeight();
		REAL32 _x = ((w-180.f)/2.f);
		REAL32 _y = ((h-14.f)/2.f) + 60.f;

		m_pSprite->SetRect( 0, _x, _y, 180.f, 14.f);
		m_pSprite->SetColor( 0, 255, 255, 255, 255);
		m_pSprite->SetEnable( 0, false);
		m_pSprite->SetTextureCoord( 0, 74.f, 487.f, 248.f, 495.f);

		//
		m_pText	= i3TextNodeDX2::new_object();
		m_pText->Create( BATTLE_CHARA_SET, FONT_COUNT_32, 15, FW_NORMAL, false, false, ANTIALIASED_QUALITY);
		m_pText->setSize( 70, 50);
		m_pText->SetEnable( false);

		//
		m_pHUDRoot->AddChild( m_pSprite);
		m_pHUDRoot->AddChild( m_pText);
		if(CHud::i())
			CHud::i()->GetNode()->AddChild( m_pHUDRoot);
	}

	void DinoHp::destroy()
	{
		I3_SAFE_RELEASE(m_pSprite); 
		I3_SAFE_RELEASE(m_pText);

		I3_SAFE_RELEASE( m_pTexture);
		I3_SAFE_NODE_RELEASE( m_pHUDRoot);
	}

	void DinoHp::print( bool enable, CGameCharaBase* chara /*= nullptr*/)
	{
		enable = (chara != nullptr) ? enable : false;
		if( enable && chara)
		{
			//
			REAL32 max = chara->getHpTagMaxHp();
			REAL32 hp = chara->getCharaInfo()->GetHP();

			REAL32 ratio = (hp > 0) ? (hp / (REAL32)max) : (0.f);
			REAL32 offset = (248.0f - 74.f) * (1.f - ratio);

			VEC2D _x, _y;
			i3Vector::Set( &_x, 74.f + offset, 248.0f + offset);
			i3Vector::Set( &_y, 487.0f, 495.0f );

			m_pSprite->SetTextureCoord( 0, _x.x, _y.x, _x.y, _y.y);


			//
			VEC3D vec;
			m_pSprite->GetLeftTop( 0, &vec);
			VEC2D* size = m_pSprite->GetSize( 0);

			i3::rc_wstring wstr_nick = BattleSlotContext::i()->getNickForSlot( chara->getCharaNetIndex(), CHARA::BUFFER_NEW_AI);
			m_pText->SetText( wstr_nick);

			UINT32 x, y;
			x = (UINT32)(vec.x + size->x - m_pText->getTextWidth());
			y = (UINT32)(vec.y - m_pText->getTextHeight());
			m_pText->setPos( x, y);
		}

		m_pSprite->SetEnable( 0, enable);
		m_pText->SetEnable( enable);
	}
};


I3_CLASS_INSTANCE(UIHUDDominationMgr);//, i3GameNode);

UIHUDDominationMgr::UIHUDDominationMgr(void)
{
	ZeroMemory( m_pHudList, sizeof( m_pHudList) );
}

UIHUDDominationMgr::~UIHUDDominationMgr(void)
{
	UIHUDDominationMgr::SetEnableAll(false);

	m_dino_hp.destroy();

	for( INT32 i = 0; i < HUD_DMN_MAX; i++ )
	{
		m_pHudList[i]->Destroy();					// 이런 함수의 호출은 소거방식이 무언가 잘못되었다는걸 의미하지만, 릭제거를 위해서 필요..
													// (단순 Release시, 순환참조오류로.. 레퍼런스 카운팅이 제거되지 않음)
	}
}


void UIHUDDominationMgr::Create( i3GameNode* pParent)
{
	i3GameNode::Create( pParent);

	m_pHudList[HUD_DOMI_SKILL]			= CUIHudDomi_Skill::new_object();
	m_pHudList[HUD_DOMI_INDICATOR ]		= CUIHudDomi_Indicator::new_object();
	m_pHudList[HUD_DOMI_COMBO]			= CUIHudDomi_Combo::new_object();
	m_pHudList[HUD_DOMI_SPECIAL_SCORE]	= CUIHudDomi_SpecialScore::new_object();
	m_pHudList[HUD_DOMI_FLOW]			= CUIHudDomi_Flow::new_object();
	m_pHudList[HUD_DOMI_WAVE_RESULT]	= CUIHudDomi_WaveResult::new_object();
	m_pHudList[HUD_DOMI_ROUND_RESULT]	= CUIHudDomi_RoundResult::new_object();
	m_pHudList[HUD_DOMI_SCORE_QUEUE]	= UIHUDDomi_ScoreQueue::new_object();
	m_pHudList[HUD_DOMI_USER_STATE]		= CUIHudDomi_UserState::new_object();

	for( INT32 i = 0; i < HUD_DMN_MAX; i++ )
	{
		m_pHudList[ i]->OnCreate( pParent);
	}
}

void UIHUDDominationMgr::Initialize(void)
{
	for( INT32 i = 0; i < HUD_DMN_MAX; i++ )
	{
		if ((i == HUD_DOMI_WAVE_RESULT) || (i == HUD_DOMI_ROUND_RESULT))
		{
			EnableHUD((HUD_DOMINATION_TYPE)i, false); continue;
		}
		else if (i == HUD_DOMI_SKILL)
		{
			EnableHUD((HUD_DOMINATION_TYPE)i, true);
			static_cast<CUIHudDomi_Skill*>(GetHUD((HUD_DOMINATION_TYPE)i))->HideNotice(true); // 강제 끔
		}

		EnableHUD( (HUD_DOMINATION_TYPE)i, true);
	}

	m_dino_hp.create();
}


void UIHUDDominationMgr::OnUpdate( REAL32 rDeltaSec)
{
	if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_F5))
		SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_INPUT, I3I_KEY_F5);

	if( SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_CHK_VISIBLE, dui::intermission))
	{
		bool is_enable_skill_shop = UIBattleFrame::i()->IsOpenPopup(UBP_SKILL_SHOP);
		CDominationSkillSystem::i()->SetEnableSkillShopHud( true, is_enable_skill_shop);
	}
	else
	{
		CDominationSkillSystem::i()->SetEnableSkillShopHud( false, false);
	}
}

void UIHUDDominationMgr::EnableHUD( HUD_DOMINATION_TYPE type, bool bEnable)
{
	I3_BOUNDCHK( type, HUD_DMN_MAX);
	I3ASSERT( m_pHudList[ type]);

	if ( m_pHudList[ type]->isEnable() == bEnable)	return;		// 일치하면 튕겨낸다..

	if (bEnable)
	{
		bool res = m_pHudList[type]->OnEntranceStart();
		I3ASSERT(res);
	}
	else
	{
		bool res = m_pHudList[type]->OnExitStart();
		I3ASSERT(res);
	}
}



//=========================================
// 이벤트 함수들
//=========================================
bool UIHUDDominationMgr::SetHudEvent( HUD_DOMINATION_TYPE type, INT32 nEventNum, INT32 arg)
{
	return false;
}

