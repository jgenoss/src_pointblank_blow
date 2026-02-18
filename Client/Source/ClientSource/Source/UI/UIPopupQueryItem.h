#pragma once

#include "UIPopupBase.h"

enum ITEM_QUERY_STYLE;
namespace inner
{
	void CbQueryUse(void * pThis, INT32 nParam);
}

class UIPopupQueryItem : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupQueryItem, UIPopupBase);

	friend void inner::CbQueryUse(void * pThis, INT32 nParam);

public:
	UIPopupQueryItem();
	virtual ~UIPopupQueryItem();

public:
	virtual void	OnCreate( i3GameNode * pParent) override;

	/*
	pArg2의 값에 따라 pArg1의 casting이 다르게 사용됩니다.
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

private:
	void setting_ui();

	void test_use();
	void use_item();
	void remove_item();

private:
	class UIImgSetCtrl* m_pImageCtrl;
	ITEM_QUERY_STYLE item_query_style;
	T_ItemDBIdx m_item_db_idx;
	T_ItemID m_item_id;
	
};
