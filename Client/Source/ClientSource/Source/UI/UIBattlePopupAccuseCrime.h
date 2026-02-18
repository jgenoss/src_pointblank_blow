#pragma once

#include "UIBattlePopupBase.h"

enum CRIME_UI_TYPE;

class UIBattlePopupAccuseCrime : public UIBattlePopupBase
{
	I3_CLASS_DEFINE( UIBattlePopupAccuseCrime, UIBattlePopupBase);

public:
	void clicked(int idx1, int idx2);

public:
	UIBattlePopupAccuseCrime();
	virtual ~UIBattlePopupAccuseCrime();

	virtual void OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 CRIME_UI_TYPE
	*/
	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override;
	virtual void OnLoadAllScenes() override;
	virtual void OnEntranceEnd() override;

	virtual void OnExitEnd( void) override;
	virtual bool OnKey_Escape()  override { return click_cancel(); }
	virtual bool OnKey_Enter( void) override;

public:
	const i3::rc_wstring& GetSelected_NickName() const;// { return m_UserList[m_Selected_Index].second; }
	FORCEREMOVE_CAUSE GetSelected_Cause() const { return m_Reason; }

private:
	void check_ok_btn();

	bool click_ok();
	bool click_cancel();

private:
	size_t get_user_list_size() const;
	const i3::pair<INT32, i3::rc_wstring>& get_selected_user_info() const;

private:
	//INT32 : slot 인덱스, i3::rc_string : 닉네임
	typedef i3::vector< i3::pair<INT32, i3::rc_wstring> > UserList;
	UserList m_LeftUserList, m_RightUserList;

	CRIME_UI_TYPE m_UiType;
	FORCEREMOVE_CAUSE	m_Reason;
	INT32 m_Vote_Type;
	INT32 m_Selected_Index;
};
