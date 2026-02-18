#if !defined( __UI_TOOL_TIP_SHOP_H__)
#define __UI_TOOL_TIP_SHOP_H__

//
// 파일이 2종류이기 때문에, 2개로 나누어 살펴보는게 좋겠지만,
// 샵페이즈가 아니면 쓰지 않을 것이므로, 객체생성의 위치는 샵페이즈로 둔다.
// 따라서, 부모는 샵페이즈가 됨..
// 샵페이즈의 로드/언로드에 종속시켜두고 관리..
//
#include "UI/UIToolTipBase.h"
#include "UI_V11/UIPhaseShop_V11.h"

class UIImgSetCtrl;

class UIToolTipShop : public UIToolTipBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIToolTipShop, UIToolTipBase);
	
public:
	UIToolTipShop();
	virtual ~UIToolTipShop();

	virtual void	OnCreate(i3GameNode* pParent);


	void			EnableCurrentSlotToolTip(INT32 slotidx, SHOP_GOODS_PACK* pack);
	void			DisableToolTip(INT32 slotidx, SHOP_GOODS_PACK* pack);	//화면에서 제거..
	void			DisableToolTip_Forced();
	
	bool			IsToolTipDisable() const { return m_pEnteredPack == NULL ; }

protected:	

	class UIPhaseShop_V11*	GetPhaseShop() const;
	virtual const char*		GetImageSetName() const = 0;

private:
		
	virtual void	OnLoadAllScenes();
	virtual void	OnUnloadAllScenes();

	virtual void	SetMoreInfo(SHOP_GOODS_PACK* pack) = 0;

	void			_Makeup_Pos_Array();
	void			SetMainInfo(SHOP_GOODS_PACK* pack);

	SHOP_GOODS_PACK*		m_pEnteredPack;
	class UIImgSetCtrl*		m_pImageSet;

	VEC3D					m_pos_by_slot[TAPSHOP_ITEM_SLOT];
	REAL32					m_interval_tooltip;
};


class UIToolTipShop_Weapon : public UIToolTipShop
{
	I3_CLASS_DEFINE( UIToolTipShop_Weapon, UIToolTipShop);
	typedef UIToolTipShop base;
public:
	UIToolTipShop_Weapon();
private:
	virtual void OnCreate(i3GameNode* pParent);
	virtual void SetMoreInfo(SHOP_GOODS_PACK* pack);
	virtual const char* GetImageSetName() const;

};

class UIToolTipShop_NonWeapon : public UIToolTipShop
{
	I3_CLASS_DEFINE( UIToolTipShop_NonWeapon, UIToolTipShop);
	typedef UIToolTipShop base;
public:
	UIToolTipShop_NonWeapon();
	virtual	~UIToolTipShop_NonWeapon();
private:
	i3UIFrameWnd *		m_pInfoCtrl[2];
	i3UIStaticText *	m_pInfoCtrl2;
	VEC2D				m_pOriginalsize[3];

	virtual void OnCreate(i3GameNode* pParent);
	virtual void SetMoreInfo(SHOP_GOODS_PACK* pack);
	virtual const char* GetImageSetName() const;
	virtual void	_InitializeAtLoad( i3UIScene * pScene);

};

















#endif