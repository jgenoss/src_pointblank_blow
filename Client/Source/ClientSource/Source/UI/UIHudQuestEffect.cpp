#include "pch.h"
#include "UIHudQuestEffect.h"

#include "MedalManager.h"
#include "MedalSetBase.h"
#include "UIHudDefine.h"
#include "UIHudUtil.h"
#include "UI/UIUtil.h"

#include "i3Base/profile/profile.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/format_string.h"


UIHudQuestEffect::UIHudQuestEffect(UIHudManager* p) : iHudBase(p)
{
	m_bIsMaxLevel = false;
	m_iBGShapeIdx = 0;
	m_rElapsedTime = 0.0f;
	m_rBGElapsedTime = 0.0f;
	m_eEffectState = QE::NONE;
}

UIHudQuestEffect::~UIHudQuestEffect()
{
}

void UIHudQuestEffect::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "Questusereffect");

	m_pMedalFrame= static_cast<i3UIFrameWnd*>(pScene->FindChildByName("i3UIFrameWnd"));
	m_pMedalMark = static_cast<i3UIButtonImageSet*>(pScene->FindChildByName("i3UIButtonImageSet"));
	m_pMedalMarkBG = static_cast<i3UIButtonImageSet*>(pScene->FindChildByName("i3UIButtonImageSet0"));
	m_pMedalName = static_cast<i3UIStaticText*>(pScene->FindChildByName("i3UIStaticText"));
	m_pMedalLevel = static_cast<i3UIStaticText*>(pScene->FindChildByName("i3UIStaticText3"));

	m_bIsMaxLevel = false;
	m_iBGShapeIdx = 0;
	m_rElapsedTime = 0.0f;
	m_rBGElapsedTime = 0.0f;
	m_eEffectState = QE::NONE;

	m_vDefaultMarkPos = *(m_pMedalMark->getPos());
	m_vDefaultFrameSize = *(m_pMedalFrame->getSize());

}

void UIHudQuestEffect::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	iHudBase::SetEnable(true);

	m_pMedalFrame->EnableCtrl(false);
	m_pMedalMark->EnableCtrl(false);
	m_pMedalMarkBG->EnableCtrl(false);

}

void UIHudQuestEffect::Update( REAL32 rDeltaSeconds )
{
	i3_prof_func();

	switch (m_eEffectState)
	{
	case QE::NONE:
		{

		}
		break;
	case QE::START:
		{
			REAL32 nPosX = m_pMedalFrame->getSize()->x / 2;
			REAL32 nPosY = m_pMedalMark->getPos()->y;
			REAL32 nOffSetX = m_pMedalMark->getPos()->x;

			m_pMedalMark->setPos( nPosX + nOffSetX , nPosY );
			m_pMedalMark->EnableCtrl(true);

			m_eEffectState = QE::MOVE_MARK;
		}
		break;
	case QE::MOVE_MARK:
		{
			VEC3D * vCurrentPos = m_pMedalMark->getPos();

			vCurrentPos->x -= UI_STATE_CHANGE_DIFF;

			if ( vCurrentPos->x <= m_vDefaultMarkPos.x )
			{
				m_pMedalMark->setPos( &m_vDefaultMarkPos );
				m_eEffectState = QE::MOVE_MARK_END;
			}
			else
			{
				m_pMedalMark->setPos( vCurrentPos );
			}
		}
		break;
	case QE::MOVE_MARK_END:
		{
			// UI 상에서 마크의 위치와 프레임의 기본 크기가 맞지 않는 부분이 있어 마크 크기의 반을 기본 크기로 세팅 ...
			m_pMedalFrame->setSize( m_pMedalMark->getSize()->x / 2 , m_vDefaultFrameSize.y );
			m_pMedalFrame->EnableCtrl(true);
			m_pMedalName->EnableCtrl(false);
			m_pMedalLevel->EnableCtrl(false);

			m_eEffectState = QE::RESIZE_FRAME;
		}
		break;
	case QE::RESIZE_FRAME:
		{
			VEC2D * vCurrentSize = m_pMedalFrame->getSize();

			vCurrentSize->x += UI_STATE_CHANGE_DIFF;

			if ( vCurrentSize->x >= m_vDefaultFrameSize.x )
			{
				m_pMedalFrame->setSize( &m_vDefaultFrameSize );
				m_eEffectState = QE::RESIZE_FRAME_END;
			}
			else
			{
				m_pMedalFrame->setSize( vCurrentSize );
			}

		}
		break;
	case QE::RESIZE_FRAME_END:
		{
			m_rElapsedTime = 0.0f;
			m_rBGElapsedTime = 0.0f;
			m_pMedalName->EnableCtrl(true);
			m_pMedalLevel->EnableCtrl(true);
			if ( m_bIsMaxLevel )
			{
				m_pMedalMarkBG->EnableCtrl(true);
				m_pMedalMarkBG->SetShapeIdx(m_iBGShapeIdx ++);
			}
			m_eEffectState = QE::CHANGE_BGSTATE;
		}
		break;
	case QE::CHANGE_BGSTATE:
		{
			m_rElapsedTime += rDeltaSeconds;
			m_rBGElapsedTime += rDeltaSeconds;

			if ( m_rElapsedTime > MAINTAIN_HUD_SEC )
			{
				m_eEffectState = QE::END;
			}

			if ( m_bIsMaxLevel)
			{
				if ( m_rBGElapsedTime > BG_STATE_CHANGE_TERM )
				{
					m_rBGElapsedTime = 0.0f;
					if ( m_iBGShapeIdx >= MAX_BG_SHAPE_STATE )
					{
						m_pMedalMarkBG->SetShapeIdx( MAX_BG_SHAPE_STATE - 1 );
					}
					else
					{
						m_pMedalMarkBG->SetShapeIdx(m_iBGShapeIdx ++);
					}
				}
			}
		}
		break;
	case QE::END:
		{
			InitializeMedalInfo();
			m_eEffectState = QE::NONE;
		}
		break;
	default:
		{
			I3ASSERT("INVALID STATE!");
		}
		break;
	}
}

void UIHudQuestEffect::InitializeMedalInfo( void )
{
	m_pMedalFrame->EnableCtrl(false);
	m_pMedalMark->EnableCtrl(false);
	m_pMedalMarkBG->EnableCtrl(false);

	m_iBGShapeIdx = 0;
	m_pMedalName->SetText("");
	m_pMedalLevel->SetText("");
}

void UIHudQuestEffect::SetParameter(INT32 arg1, INT32 arg2)
{
	i3_prof_func();

	MedalCompleteInfo * pData = (MedalCompleteInfo *)arg1;

	MedalSetBase * pBase = CMedalManager::i()->getMedalSet(pData->_type);
	if(pBase == nullptr)
		return;

	MEDAL_INFO * pMedal = pBase->getMedal(pData->_idx);
	if(pMedal == nullptr)
		return;

	i3::stack_wstring	wstrLevel;
	i3::sprintf(wstrLevel, GAME_RCSTRING("STR_UI_MEDAL_EFFECT_REWARD"),pData->_level);

	UINT16				iMedalShapeIdx;
	const i3::rc_wstring&	wstrMedalName = GAME_QUEST_RCSTRING(pMedal->m_szNameKey);

	iMedalShapeIdx = pMedal->m_ui16ImageIdx;

 	m_bIsMaxLevel = CMedalManager::i()->IsPerfectClearMedal(pData);
	m_pMedalMark->SetShapeIdx(iMedalShapeIdx);
	m_pMedalName->SetText(wstrMedalName);
	m_pMedalLevel->SetText(wstrLevel);

	m_eEffectState = QE::START;
}