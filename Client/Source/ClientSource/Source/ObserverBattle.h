#ifndef __OBSERVER_BATTLE_H
#define __OBSERVER_BATTLE_H

#include "BattleHUD_Define.h"
#include "tinst.h"

namespace observer
{
	struct ViewState;

	class Context
	{
	public:
		static Context*	i() { return tinst<Context>(); }

		bool ChangeViewTarget(const INT32 idx);
		void SetViewState(ViewState* state);
		void OnUpdate(REAL32 tm);
		void Reset();
		INT32 GetCurrIdx() const;

		bool IsGMObserverHUDOff() { return m_bGMHUDOff; }
		void SetGMObserverHUDOff(bool flag) { m_bGMHUDOff = flag; }

	private:
		friend tinst_creator<Context>;
		Context();

		void _UpdateInput();
		void _UpdateInput_GMObserver(i3InputKeyboard* pKey, i3InputMouse* pMouse);
		void _UpdateInput_ViewChange(i3InputKeyboard* pKey, i3InputMouse* pMouse);

		INT32 m_currIdx;
		ViewState* m_currViewState;
		bool m_bGMHUDOff;
	};
	tinst<Context>;

	void SetIntrudeViewState();
	bool IsPreparedToObserveEnemy();
	bool IsPreparedToObserveEnemyHP();
	bool IsObserverState();
}

#endif