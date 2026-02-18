#if !defined( __UI_POPUP_PCCAFE_H__)
#define __UI_POPUP_PCCAFE_H__

#include "UIPopupBase.h"

class UIPopupPCCafe : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupPCCafe, UIPopupBase);

	enum PCCAFE_ITEM_TYPE
	{
		TYPE_WEAPON = 0,
		TYPE_PARTS,
		TYPE_ITEM,
	};

	enum {	DEFAULT_PCCAFE_ITEM_COUNT = 5, };

protected:
	void			_ClearPCCafeItemList( INT32 i32Count);
	void			_SetPCCafeItemList( INVEN::DB_TYPE dbtype);
	void			_SetPCCafeItemName( INT32 idx, T_ItemID itemID);
	void			_GetPCCafeInvenList( i3::vector<INVEN_BUFFER*> & out, INVEN::DB_TYPE dbtype);

	void			_SetBonusEffect(void);
public:
	UIPopupPCCafe();
	virtual ~UIPopupPCCafe();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnEntranceEnd( void) override;

	virtual bool	OnKey_Enter( void) override;
	virtual bool	OnCloseButton( void) override;

public:
	void			ConfirmOK( void);
	void			ClickItemListBtn(INT32 type);
	void			ClickInventory(void);
};

#endif
