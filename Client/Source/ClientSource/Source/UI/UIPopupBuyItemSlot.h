#if !defined( __UI_POPUP_BUYITEM_slot_H__)
#define __UI_POPUP_BUYITEM_slot_H__

#include "iSlot.h"

class UIPopupBuyItem;

class ShopPopupBuyItemSlot : public UI::iSlot
{
	typedef ShopPopupBuyItemSlot me;

	friend class UIPopupBuyItem;

public:
	ShopPopupBuyItemSlot();
	~ShopPopupBuyItemSlot() {}

	void SetParent(UIPopupBuyItem *parent)	{ mParent = parent;	}

private:
	UIPopupBuyItem *mParent;
	bool is_buyable_chara;
	
	int	 m_nRow;
	int  m_nCol;
	int  m_nSlot;

public:
	void setRow(int Row) { m_nRow = Row; }
	void setCol(int Col) { m_nCol = Col; }

	void clicked();
	void clicked_combo(INT32 tab);

public:
	void update_list();
	INT32 get_now_slot()const	{	return iSlot::get_now_slot();	}

public:
	void OnCreate( i3GameNode * pParent);

	void OnLoadAllScenes();
	void OnUnloadAllScenes();

	void OnUpdate( REAL32 rDeltaSeconds);
	void OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code);

protected:
	virtual void cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
};

#endif	

