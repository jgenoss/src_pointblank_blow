#ifndef _UIPOPUP_QUICK_JOIN_H_
#define _UIPOPUP_QUICK_JOIN_H_

#include "../UIPopupBase.h"

namespace Qjoin
{
	class Slot;
}

class UIPopupQuickJoin : public UIPopupBase
{
	I3_CLASS_DEFINE(UIPopupQuickJoin, UIPopupBase);

public:
	UIPopupQuickJoin();
	virtual ~UIPopupQuickJoin();

	virtual void OnCreate(i3GameNode* parent) override;
	virtual void _InitializeAtLoad(i3UIScene* scn) override;
	virtual void OnLoadAllScenes() override;
	virtual void OnUpdate(REAL32 tm) override;
	virtual void OnEntranceEnd() override;
	virtual void ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;
	virtual bool OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;
	virtual bool OnCloseButton( void) override;

	void GetLeftTopPos(INT32& outLeft, INT32 & outTop);

	Qjoin::Slot* GetSlot() const { return m_slot; }

private:
	Qjoin::Slot* m_slot;
};

#endif