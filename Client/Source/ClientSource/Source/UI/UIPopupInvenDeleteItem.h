#ifndef _UIPOPUP_INVEN_DELETE_ITEM_H_
#define _UIPOPUP_INVEN_DELETE_ITEM_H_

#include "UIPopupBase.h"

class UIPopupInvenDeleteItem : public UIPopupBase
{
	I3_CLASS_DEFINE(UIPopupInvenDeleteItem, UIPopupBase);

public:
	UIPopupInvenDeleteItem();
	virtual ~UIPopupInvenDeleteItem();

	virtual void OnCreate(i3GameNode* parent) override;
	virtual void _InitializeAtLoad(i3UIScene* scn) override;
	/*
	param[in] pArg1 const ITEM_INFO*
	*/
	virtual bool OnEntranceStart(void* arg1, void* arg2) override;
	virtual void ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;
	virtual bool OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;

	void		clicked(INT32);

private:
	T_ItemDBIdx m_item_db_idx;
	T_ItemID m_item_id;
};

#endif