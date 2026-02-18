#pragma once

#include "UIHUD_Domination_Base.h"
#include "../ProgressBar.h"

namespace DominationUI_UserState
{
	struct slot_info
	{
		enum { EMPTY, ME, OTHER};

		i3::rc_wstring	_wstr_name;
		UINT32		_score; 
		INT32		_max_hp;
		INT32		_cur_hp;
		INT32		_state;

		slot_info() : _wstr_name(L"empty") { _state = EMPTY; _score = 0; _max_hp = 100; _cur_hp = 0;}
	};

	enum HUD_USER_STATE_EVENT
	{
		USER_STATE_E_INIT,
		USER_STATE_E_SYNC,

	};
};
namespace dus = DominationUI_UserState;

class CUIHudDomi_UserState : public UIHUDDominationBase
{
	I3_CLASS_DEFINE( CUIHudDomi_UserState, UIHUDDominationBase);
public:
	CUIHudDomi_UserState();
	~CUIHudDomi_UserState();

	virtual void OnCreate( i3GameNode* pParent) override;
	virtual void _InitializeAtLoad( i3UIScene * pScene) override;
	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool SetHudEvent( INT32 nEventNum, INT32 arg = 0) override;
private:
	void _set_slot_info( INT32 slot_idx, dus::slot_info& in);
	void _get_user_info( INT32 user_slot_idx, dus::slot_info& out);

	IntColorProgressBar m_ProgressBar[4];
};