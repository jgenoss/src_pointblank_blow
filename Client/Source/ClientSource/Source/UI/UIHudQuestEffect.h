#pragma once

#include "iHudBase.h"
#include "UISlide.h"

class UIHudManager;

#define MAINTAIN_HUD_SEC 1.3
#define MAX_BG_SHAPE_STATE 11
#define UI_STATE_CHANGE_DIFF 5
#define BG_STATE_CHANGE_TERM 0.05f

namespace QE
{
	enum EFFECT_STATE
	{
		NONE,					// 애니메이션 되고있지 않은 상태. 다음 애니메이션 시작을 기다림.
		START,					// 애니메이션을 시작함. 마크의 초기 위치 설정 및 Visible 처리.
		MOVE_MARK,				// 마크의 위치를 원래의 위치로 이동하는 과정. UI_STATE_CHANGE_DIFF 만큼 Update마다 x축 이동함.
		MOVE_MARK_END,			// 마크의 이동이 끝났으며 프레임 윈도우의 애니메이션을 시작. 초기 크기 세팅 및 Visible 처리.
		RESIZE_FRAME,			// 프레임 윈도우를 리사이징 하는 과정. 마찬가지로 UI_STATE_CHANGE_DIFF 만큼 Update마다 커짐.
		RESIZE_FRAME_END,		// 프레임 윈도우의 리사이징이 끝났으며 세팅 해 둔 텍스트들을 Visible 처리함. 퍼펙트 클리어 여부에 따라 관련 컨트롤도 초기 세팅함.
		CHANGE_BGSTATE,			// 일반적인 메달 클리어의 경우 DeltaTime 만 누적하여 QE::END로 넘어감. 그렇지 않을 경우 반짝이 이펙트의 버튼쉐입을 바꿈.
		END						// 메달 이펙트가 모두 끝났으며 InitializeMedalInfo 함수를 통해 관련 컨트롤들을 모두 초기화 및 Visible Off 처리. QE::NONE 으로 돌아감.
	};
}

class UIHudQuestEffect : public iHudBase
{
public:
	UIHudQuestEffect(UIHudManager* p);
	virtual ~UIHudQuestEffect();

	virtual void			InitializeAtLoad( i3UIScene * pScene) override;
	virtual void			SetParameter(INT32 arg1, INT32 arg2 = -1) override;
	virtual void			event_start_battle(INT32 arg, const i3::user_data& UserData) override;
	virtual void			Update( REAL32 rDeltaSeconds ) override;

	QE::EFFECT_STATE		GetEffectState(void) const { return m_eEffectState; }


protected:
	void					InitializeMedalInfo( void );

private:
	QE::EFFECT_STATE	m_eEffectState;
	UINT8				m_iBGShapeIdx;
	bool				m_bIsMaxLevel;
	VEC3D				m_vDefaultMarkPos;
	VEC2D				m_vDefaultFrameSize;
	REAL32				m_rElapsedTime;
	REAL32				m_rBGElapsedTime;

private:
	i3UIFrameWnd*		m_pMedalFrame;
	i3UIButtonImageSet* m_pMedalMark;
	i3UIButtonImageSet* m_pMedalMarkBG;
	i3UIStaticText*		m_pMedalName;
	i3UIStaticText*		m_pMedalLevel;
};