#if !defined( __UI_IMAGESET_FOR_ITEM_H__)
#define __UI_IMAGESET_FOR_ITEM_H__

enum UI_IMAGESET_BIND_TYPE
{
	UI_IMAGESET_BIND_WEAPON = 0,
	UI_IMAGESET_BIND_CHARA,
	UI_IMAGESET_BIND_ITEM,
	UI_IMAGESET_BIND_NONWEAPON,		// CHARA + ITEM
};

enum UISLOT_CHARA
{
	UISLOT_CHARA_ALL = 0,
	UISLOT_CHARA_HEAD,
	UISLOT_CHARA_HEAD_01, // ButtonImageSet_Item_Head_01
	UISLOT_CHARA_BERET,
	UISLOT_CHARA_BERET_01,
	UISLOT_CHARA_MASK,
};

enum UITOOLTIP_NONWEAPON
{
	UTT_NONWEAPON_CHARA = 0,
	UTT_NONWEAPON_HEAD,
	UTT_NONWEAPON_HEAD_01, // ButtonImageSet_Item_Head_01
	UTT_NONWEAPON_BERET,
	UTT_NONWEAPON_BERET_01,
	UTT_NONWEAPON_MASK,
	UTT_NONWEAPON_CASHA,
};

class UIImageSetControl;

class UIImageSetForItem : public i3ElementBase
{
	I3_CLASS_DEFINE( UIImageSetForItem, i3ElementBase);

private:
	UIImageSetControl *	m_pImageCtrl;
	UI_IMAGESET_BIND_TYPE	m_Type;
	INT32					m_CashACount;

public:
	UIImageSetForItem();
	virtual ~UIImageSetForItem();

	void		Create( UI_IMAGESET_BIND_TYPE type = UI_IMAGESET_BIND_WEAPON);

	void		BindScene( i3UIScene * pScene, const char * pszBindName);

	void		BindScene( i3UIScene * pScene, const char * pszBindName, bool pResize);

	void		SetItemImage( INT32 ItemID);

	void		SetVisibleImage( bool bVisible);

	void		SetSize( REAL32 cx, REAL32 cy);

	void		SetSize( VEC2D * pVec);

	VEC2D *		GetSize();

	void		SetPos( REAL32 x, REAL32 y, REAL32 z = 0.0f);

};

#endif
