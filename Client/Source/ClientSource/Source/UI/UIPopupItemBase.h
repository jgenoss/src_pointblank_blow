#if !defined( __UI_POPUP_ITEMBASE_H__)
#define __UI_POPUP_ITEMBASE_H__

#include "UIPopupBase.h"
#include "Useable_Item_Def.h"

class UIUseItem;

class UIPopupItemBase : public UIPopupBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIPopupItemBase, UIPopupBase);

public:
	UIPopupItemBase();
	virtual ~UIPopupItemBase();

	/*
	param[in] pArg1 INT32*
	param[in] pArg2 USE_ITEM_POPUP_TYPE*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;

protected:
	USE_ITEM_POPUP_TYPE m_Type;

	T_ItemID			m_ItemID;
	T_ItemDBIdx			m_AuthWareDBIndex;
	
protected:
	/** \brief EntranceStart시 호출되며 ItemID를 갖고 정보를 설정하게 됩니다.
		\desc UI에 정보를 설정하려면 이곳에 define하십시오.
			LuaStack count 1이 증가된 상태(ItemName을 설정함)
			*/
	virtual LuaState * _SetItemInfo( void);
};

#endif
