#pragma once

#include "UIPopupItemBase.h"

class UIPopupColorNickname : public UIPopupItemBase
{
	I3_CLASS_DEFINE(UIPopupColorNickname, UIPopupBase);

public:
	UIPopupColorNickname();
	virtual ~UIPopupColorNickname();

	virtual void OnCreate(i3GameNode* parent) override;
	virtual void _InitializeAtLoad(i3UIScene* scene) override;
	
	virtual bool OnEntranceStart(void* pArg1, void* pArg2) override;

	virtual bool OnEnterMouse(i3UIControl* pControl) override;
	virtual void OnLeaveMouse(i3UIControl* pControl) override;
	virtual bool OnClick(i3UIControl* pControl) override;

	virtual bool OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	virtual bool OnKey_Enter(void) override;

	virtual void ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	virtual bool OnOKButton( void) override;
	virtual bool OnCloseButton( void) override;

	void UpdateNickname();
	bool GetSelectedColor(UINT8* rstColorIndex, I3COLOR* rstColor=nullptr) const;

protected:
	LuaState* _SetItemInfo(void);

private:
	bool Commit(); // ¼­¹ö¿¡ ¹Ù²ïÄÃ·¯ÀÎµ¦½º¸¦ º¸³À´Ï´Ù.
	void SyncIMEUI();

	typedef void (UIPopupColorNickname::*call_func)(INT32, const i3::user_data&);
	void RegisterGameEventFunc( GAME_EVENT evt, call_func f);
	void ReceiveEvent_Item_Auth(INT32 result, const i3::user_data&);

	void _CreateSelectionbox(i3UIScene* scene);
	void _CreateColorboxes(i3UIScene* scene);
	bool _IsColorbox(i3UIControl* pControl) const;

private:
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	i3::vector<i3UIButton*>				m_colorboxes;
	i3UIImageBoxEx*						m_selectionbox;
	i3UIFrameWnd*						m_palette;
	i3UIButton*							m_selectedColorBtn;
	i3UIEditBox*						m_chatBox;
	i3::wstring							m_chatLogStrOnly;
};