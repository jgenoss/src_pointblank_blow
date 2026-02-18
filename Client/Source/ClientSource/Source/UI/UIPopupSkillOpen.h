#pragma once

#include "UIPopupBase.h"

enum ITEM_QUERY_STYLE;
namespace inner
{
	void Reuse(void * pThis, INT32 nParam);
}

class UIPopupSkillOpen : public UIPopupBase
{
	I3_CLASS_DEFINE(UIPopupSkillOpen, UIPopupBase);

	friend void inner::Reuse(void * pThis, INT32 nParam);

public:
	UIPopupSkillOpen();
	virtual ~UIPopupSkillOpen();

public:
	virtual void	OnCreate( i3GameNode * pParent) override;

	/*
	pArg2의 타입에 따라 pArg1의 casting이 달라집니다.
	param[in] pArg1 ITEM_INFO* or T_ItemID*
	param[in] pArg2 ITEM_QUERY_STYLE*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;

	virtual bool	OnKey_Enter() override;
	virtual bool	OnKey_Escape(void) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;

public:
	void clicked(int idx);
	void SetCharaIdx(const i3::wliteral_range& nickname);

private:
	void SetInfo();
	void getCharaString(i3::stack_wstring& out);

	void use_checkitem();
	void use_item();
	void use_masterticket();

private:
	enum
	{
		OK = 0,
		CANCEL,
	};

private:
	class UIImgSetCtrl* m_pImageCtrl;
	ITEM_QUERY_STYLE item_query_style;

	INT32 chara_idx;
	T_ItemDBIdx m_item_db_idx;
	T_ItemID m_item_id;
};