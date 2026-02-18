#include "pch.h"
#include "UIHudFloatingBoard.h"
#include "MedalManager.h"
#include "MedalSetBase.h"
#include "UIHudUtil.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

namespace
{
	UI::tmLinearSlideMtd g_slide_mtd;

	bool g_check_slide;
	REAL32 elapse_time;
}

UIHudFloatingBoard::UIHudFloatingBoard(UIHudManager* p) : iHudBase(p)
{
	g_check_slide = false;
	m_slide_call.set_obj(this);
	m_Slide.SetSlideEndCallBack(&m_slide_call);
}

UIHudFloatingBoard::~UIHudFloatingBoard()
{
}


/*********************************/
void UIHudFloatingBoard::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "MedalBoardFrame");

	UI::tmSlideAttContainer list(pScene);
	list.add("MedalBoardFrame");
	list.set_target_by_offset(-800,0);

	m_Slide.add( list );
	m_Slide.move_target();

	m_StaticText = FindControl<i3UIStaticText*>(pScene, "Medal_Text");
}

void UIHudFloatingBoard::ClearAtUnload( i3UIScene * pScene)
{
	m_Slide.clear();
}

void	UIHudFloatingBoard::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	m_Slide.OnUpdate(rDeltaSeconds);

	if( g_check_slide )
	{
		elapse_time += rDeltaSeconds;
		if( elapse_time > 1.f )
		{
			g_slide_mtd.invert = false;
			m_Slide.start(&g_slide_mtd, 0.4f);
			g_check_slide = false;
		}
	}
}

/*********************************/
void UIHudFloatingBoard::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	iHudBase::SetEnable(true);
}

void UIHudFloatingBoard::SetParameter(INT32 arg1, INT32 arg2)
{
	i3_prof_func();

	elapse_time = 0.f;
	g_check_slide = false;
	g_slide_mtd.invert = true;
	m_Slide.start(&g_slide_mtd, 0.2f);

	MEDAL_STATE * pData = (MEDAL_STATE *)arg1;
	UINT8 _type; // = pData->m_ui8MedalType;
	UINT16 _medalIdx = pData->m_ui16MedalIdx;

	if(pData->m_ui8MedalType == MEDAL_TYPE_NORMAL)
		_type = MEDAL_SET_TYPE_NORMAL;
	else
	{
		INT32 int32Type = CMedalManager::i()->getMedalSetTypeByIndex(pData->m_ui16MedalIdx);
		I3_BOUNDCHK_RETURN(int32Type, MEDAL_SET_TYPE_MAX);
		_type = static_cast<UINT8>(int32Type);
	}

	// 커버리티에 더 이상 시달리기 싫기 때문에, 조건으로 거르는 것과 동시에 ASSERT처리한다...(2016.07.05)
	// 앞으로도 I3ASSERT를 제대로 걸러내지 못하는 커버리티 때문에 웬간하면 인덱스 안맞으면 어떻게든 어써트와 함께 리턴탈출을 하도록 수정하겠다. (2016.07.05).수빈.
		
	MedalSetBase * pBase = CMedalManager::i()->getMedalSet(_type);
	if(pBase == nullptr)
		return;
	MEDAL_INFO * pMedal = pBase->getMedal(_medalIdx);
	if(pMedal == nullptr)
		return;

	const char * desc = CMedalManager::i()->getNotifyMedalDesc(_type, _medalIdx);

	UINT8 lv = pBase->getCurrentLevel(_medalIdx);
	i3::wstring wstr_medalDescStr;
	i3::stack_wstring wstr_medalCntStr;

	INT32 begin_cnt= pBase->getMedalProcessCnt(_medalIdx) - pBase->getLevelProcessCnt(pMedal, lv);
	INT32 dest_cnt = 0;
	if(lv == MEDAL_LEVEL_MAX)
	{
		dest_cnt = pBase->getLevelProcessCnt(pMedal, lv);
	}
	else
	{
		dest_cnt = pBase->getLevelProcessCnt(pMedal, lv + 1) - pBase->getLevelProcessCnt(pMedal, lv);
	}
	if(begin_cnt >= dest_cnt)
		begin_cnt = dest_cnt;
	if(begin_cnt < 0)
		begin_cnt = 0;

	i3::sprintf(wstr_medalDescStr, GAME_QUEST_RCSTRING(desc), dest_cnt);
	i3::sprintf(wstr_medalCntStr, L"\n%d/%d", begin_cnt, dest_cnt);
	i3::generic_string_cat(wstr_medalDescStr, wstr_medalCntStr);

	m_StaticText->SetText(wstr_medalDescStr);
}

void UIHudFloatingBoard::slide_proc()
{
	if(g_slide_mtd.invert == true)
	{
		g_check_slide = true;
	}
	else
	{
		g_check_slide = false;
	}
}

void UIHudFloatingBoard::OnRevive( i3RenderContext * pCtx)
{
	 m_Slide.move_target();
}