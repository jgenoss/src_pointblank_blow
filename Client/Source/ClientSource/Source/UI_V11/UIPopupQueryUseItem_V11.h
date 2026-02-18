#pragma once

#include "UI/UIPopupBase.h"

enum ITEM_QUERY_STYLE;
namespace inner_V11
{
	void CbQueryUse(void * pThis, INT32 nParam);
}

class UIPopupQueryUseItem_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupQueryUseItem_V11, UIPopupBase);

	friend void inner_V11::CbQueryUse(void * pThis, INT32 nParam);

public:
	UIPopupQueryUseItem_V11();
	virtual ~UIPopupQueryUseItem_V11();

public:
	virtual void	OnCreate( i3GameNode * pParent);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnLoadAllScenes();
	virtual void	OnUnloadAllScenes();

	virtual bool	OnKey_Enter(); 
	virtual bool	OnKey_Escape(void);
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData);

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
