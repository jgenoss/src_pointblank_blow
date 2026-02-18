#include "pch.h"
#include "UISkillSlot.h"

#include "UIMainFrame.h"
#include "UITabProfileAbility.h"

namespace
{
	class SkillSlot_Callback_None : public SkillSlot_Callback
	{
	public:
		virtual void	OnLoadSlot( INT32 slot_idx, i3UIScene* slot_scn ) {}				// 로드된 경우, 초기화할 호출 위치..
		virtual void	OnEnableSlot( INT32 slot_idx, i3UIScene* slot_scn) {}			// enable호출된 경우 업데이트할 정보를 채울 함수..
	} g_callback_none;
}

UISkillSlot::UISkillSlot() : m_Parent(NULL), m_Callback(&g_callback_none), m_Row(0)
{
}

UISkillSlot::~UISkillSlot()
{
}

void UISkillSlot::OnCreate(UIBase* parent, INT32 row)
{
	m_Parent = parent;
	m_Row = row;
}

void UISkillSlot::RegisterSlotScene( const char *szSlotScene, const LuaFuncReg* Lua_Func, bool bLoaded)
{
	for (INT32 i = 0 ; i < m_Row ; ++i )	//현재구조는 세로로 쭉 뿌리는 구조이기 때문에..
	{
		m_Parent->AddScene(szSlotScene, Lua_Func, bLoaded, !bLoaded);					// 슬롯을 한꺼번에 로딩할때 현재 문제점은 일단 씬리소스파일과 루아파일이 중복로딩되는것 아닌가 하는 점..
	}
}