#include "pch.h"

#include "UIHudDomi_Skill.h"
#include "..\UIUtil.h"

using namespace ModeScript;

I3_CLASS_INSTANCE(CUIHudDomi_Skill);

CUIHudDomi_Skill::CUIHudDomi_Skill() : m_isVisibleWarningNoUse(false), m_isVisibleAllowTheUse(false), m_spMax(0), m_spCurr(0), m_frame_gauge(nullptr)
{
}

CUIHudDomi_Skill::~CUIHudDomi_Skill()
{
}

void CUIHudDomi_Skill::OnCreate(i3GameNode* parent)
{
	UIHUDDominationBase::OnCreate(parent);

	AddScene("Scene/Domination/PBRe_Domi_skill.i3UIs", nullptr, true, false);
}

void CUIHudDomi_Skill::_InitializeAtLoad(i3UIScene* scene)
{
	UIHUDBase::_InitializeAtLoad(scene);

	SetSPMax(10000);

	m_frame_gauge = (i3UIFrameWnd*)scene->FindChildByName( "main");
	I3ASSERT( m_frame_gauge != nullptr);
}

void CUIHudDomi_Skill::OnUpdate(REAL32 tm)
{
	UIHUDDominationBase::OnUpdate(tm);
}

void CUIHudDomi_Skill::WarningNoUse(const i3::wliteral_range& reason)
{
	if (m_isVisibleWarningNoUse) return;

	HideNotice();

	LuaState* L = _CallLuaFunction("WarningNoUse");
	i3Lua::PushStringUTF16To8(L, reason.c_str(), reason.size() );
	_EndLuaFunction(L, 1);

	m_isVisibleWarningNoUse = true;
}

void CUIHudDomi_Skill::WarningNoUse(const i3::wliteral_range& reason, const INT32 cost)
{
	if (m_isVisibleWarningNoUse) return;

	HideNotice();

	LuaState* L = _CallLuaFunction("WarningNoUseWithCostRequired");
	i3Lua::PushStringUTF16To8(L, reason.c_str(), reason.size() );
	i3Lua::PushInteger(L, cost);
	_EndLuaFunction(L, 2);

	m_isVisibleWarningNoUse = true;
}

void CUIHudDomi_Skill::WarningNoUse(const i3::wliteral_range& str1, const i3::wliteral_range& str2)
{
	if (m_isVisibleWarningNoUse) return;

	HideNotice();

	LuaState* L = _CallLuaFunction("WarningUsingTwoString");
	i3Lua::PushStringUTF16To8(L, str1.c_str(), str1.size());
	i3Lua::PushStringUTF16To8(L, str2.c_str(), str2.size());
	_EndLuaFunction(L, 2);

	m_isVisibleWarningNoUse = true;
}

void CUIHudDomi_Skill::AllowTheUse(const i3::wliteral_range& type, INT16 cost)
{
	if (m_isVisibleAllowTheUse) return;

	HideNotice();

	LuaState* L = _CallLuaFunction("AllowTheUse");
	i3Lua::PushStringUTF16To8(L, type.c_str(), type.size());
	i3Lua::PushInteger(L, cost);
	_EndLuaFunction(L, 2);

	m_isVisibleAllowTheUse = true;
}

void CUIHudDomi_Skill::HideNotice(bool force)
{
	if (m_isVisibleWarningNoUse || force)
	{
		LuaState* L = _CallLuaFunction("HideWarningNoUse");
		_EndLuaFunction(L, 0);
		m_isVisibleWarningNoUse = false;
	}

	if (m_isVisibleAllowTheUse || force)
	{
		LuaState* L = _CallLuaFunction("HideAllowTheUse");
		_EndLuaFunction(L, 0);
		m_isVisibleAllowTheUse = false;
	}
}

void CUIHudDomi_Skill::SetSPGauge(UINT16 sp)
{
	if (sp > m_spMax) 
	{
		sp = m_spMax;
	}

	LuaState* L = _CallLuaFunction("SetSPGauge");

	REAL32 curSP = static_cast<REAL32>(sp);
	m_spCurr = sp;
	REAL32 maxSP = static_cast<REAL32>(m_spMax);
	REAL32 ratio = curSP / maxSP;
	i3Lua::PushNumber(L, ratio);
	i3Lua::PushInteger(L, sp);

	_EndLuaFunction(L , 2);
}

void CUIHudDomi_Skill::SetSPMax(UINT16 max)
{
	LuaState* L = _CallLuaFunction("SetSPMax");

	i3Lua::PushInteger(L, max);

	_EndLuaFunction(L, 1);

	m_spMax = max;

	SetSPGauge(m_spCurr);
}

void CUIHudDomi_Skill::VisibleGauge( bool visible)
{
	if( m_frame_gauge)
		m_frame_gauge->SetVisible( visible);
}

