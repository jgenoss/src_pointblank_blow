#if !defined( __UI_TOOL_TIP_ROULETTE_H__)
#define __UI_TOOL_TIP_ROULETTE_H__

#include "UIToolTipBase.h"
#include "UIImageSetForItem.h"
#include "UIPhaseShop.h"

enum ItemStatus;
class UIImageSetForItem;

class UIToolTipRoulette : public UIToolTipBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIToolTipRoulette, UIToolTipBase);
public:
	UIToolTipRoulette(void);
	virtual ~UIToolTipRoulette(void);

	virtual void	OnCreate(i3GameNode* pParent);
	
	void			EnableCurrentSlotToolTip(VEC2D pos, INT32 goodsid);
	void			DisableToolTip();	//화면에서 제거..
	void			DisableToolTip_Forced();
	bool			IsToolTipDisable() const { return m_pGoodId == 0; }
	void			Character_ByShopCategory(SHOP_GOODS* pGoods, 
		i3::string& outItemName, i3::string& outDesc, char* outCategory, i3::string& outStatus, char* outRequired);
	void			Item_ByShopCategory(SHOP_GOODS* pGoods, 
		i3::string& outItemName, i3::string& outDesc, char* outCategory, i3::string& outStatus, char* outRequired);
	void			Weapon_ByShopCategory(SHOP_GOODS* pGoods, 
		i3::string& outItemName, i3::string& outDesc, char* outCategory, i3::string& outStatus, char* outRequired);
	ItemStatus	CalcItemStatus(INT32 itemID);

protected:	

	virtual const char* GetImageSetName() const = 0;

private:
		
	virtual void	OnLoadAllScenes();
	virtual void	OnUnloadAllScenes();

	virtual void	SetMoreInfo(INT32 goodsid) = 0;


	void			SetMainInfo(INT32 goodsid);

	INT32					m_pGoodId;
	class UIImgSetCtrl*		m_pImageSet;

};

class UIToolTipRoulette_Weapon : public UIToolTipRoulette
{
	I3_CLASS_DEFINE( UIToolTipRoulette_Weapon, UIToolTipRoulette);
	typedef UIToolTipRoulette base;
public:
	UIToolTipRoulette_Weapon();
private:
	virtual void OnCreate(i3GameNode* pParent);
	virtual void SetMoreInfo(INT32 goodsid);
	virtual const char* GetImageSetName() const;

};


class UIToolTipRoulette_NonWeapon : public UIToolTipRoulette
{
	I3_CLASS_DEFINE( UIToolTipRoulette_NonWeapon, UIToolTipRoulette);
	typedef UIToolTipRoulette base;
public:
	UIToolTipRoulette_NonWeapon();
	virtual	~UIToolTipRoulette_NonWeapon();
private:
	i3UIFrameWnd *		m_pInfoCtrl[2];
	i3UIStaticText *	m_pInfoCtrl2;
	VEC2D				m_pOriginalsize[3];

	virtual void		OnCreate(i3GameNode* pParent);
	virtual void		SetMoreInfo(INT32 goodsid);
	virtual const char* GetImageSetName() const;
	virtual void		_InitializeAtLoad( i3UIScene * pScene);

};

#endif