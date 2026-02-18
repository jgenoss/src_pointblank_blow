#ifndef _StepShop_Tab_h
#define _StepShop_Tab_h

#include "Shop.h"
#include "StepInventory.h"


class CStepShop;
class CTabBase;

// 상점 우하단에 몇일 또는 몇개와
// 그에 해당하는 가격이 나오는 리스트의 GUI 구조체
struct	GoodsPriceFrame
{
public:
	GoodsPriceFrame();

	void	EnableAll(BOOL bValue);
	INT32	SetGoodsRadio(UINT32 idx = 0);

	// 상품 가격을 표시하는 컨트롤
	i3GuiRadioButton*	pGoodsRadio[GOODS_MAX_IN_PACK];
	i3GuiEditBox*		pUseDayValue[GOODS_MAX_IN_PACK];
	i3GuiEditBox*		pUseDayStatic[GOODS_MAX_IN_PACK];
	i3GuiEditBox*		pSaleText[GOODS_MAX_IN_PACK];		// 영구구입 텍스트가 들어감.
															// 위 pUseDayValue, pUseDayStatic과 중복되며 이 두개를 삭제하여
															// 사용할 수 있으나 Old Source와 호환을 위하여 유지합니다.
															// New로 오면 이 2개를 삭제할 것 입니다.
	i3GuiEditBox*		pBuyPointValue[GOODS_MAX_IN_PACK];
	i3GuiEditBox*		pBuyPointStatic[GOODS_MAX_IN_PACK];	// 화폐단위 텍스트

	i3GuiStatic*		pCashSymbol[GOODS_MAX_IN_PACK];		// 한국 N코인 심볼 이미지

	// 가진돈, 남은돈을 표시하는 컨트롤
	i3GuiEditBox*		pHavePointStatic;
	i3GuiEditBox*		pHavePointValue;
	i3GuiEditBox*		pHavePointUnitStatic;
	i3GuiStatic*		pHaveCashSymbol;			// 한국 N코인 심볼 이미지

	i3GuiEditBox*		pRemainPointStatic;
	i3GuiEditBox*		pRemainPointValue;
	i3GuiEditBox*		pRemainPointUnitStatic;
	i3GuiStatic*		pRemainCashSymbol;			//	한국 N코인 심볼 이미지

	i3GuiEditBox*		pPointInfoStatic;

	i3GuiButton*		pBuy;
	i3GuiButton*		pGift;
};


class	CRepairFrame
{
public:
	CRepairFrame(CTabBase * pOwner);
	virtual	~CRepairFrame() {};

public:
	virtual	void	OnLinkControl(i3BinList * plist) =0;
	virtual	void	Init()	{};

	virtual	void	Update(RT_REAL32 rDeltaSec) =0;

	void	SetEnable(BOOL bValue)	{m_pFrame->SetEnable(bValue);}

protected:
	i3GuiStatic *	m_pFrame;
	i3GuiButton *	m_pDoRepair;

	i3GuiStatic *	m_pArrow;
	i3GuiEditBox *	m_pRepairText;
	i3GuiEditBox *	m_pRepairPrice;
	i3GuiEditBox *	m_pRepairPriceType;

	i3GuiEditBox *	m_pHaveMoneyText;
	i3GuiEditBox *	m_pHaveMoneyValue;
	i3GuiEditBox *	m_pHaveMoneyType;

	i3GuiEditBox *	m_pRemainMoneyText;
	i3GuiEditBox *	m_pRemainMoneyValue;
	i3GuiEditBox *	m_pRemainMoneyType;


	CTabBase *	m_pOwner;
};




class	CTabBase
{
public:
	CTabBase(INT32 SlotMaxCount, INT32 SlotHoriCount, CStepShop * pOwner = NULL);
	virtual	~CTabBase();


public:
	virtual	void	Init();
	virtual	BOOL	OnOpen();
	virtual	void	OnClose();
	virtual	void	OnUpdate(RT_REAL32 rDeltaSec)	{}
	virtual	void	OnFocus(UINT32 idxSlot)			{}
	virtual	BOOL	OnSelectSlot(UINT32 idxSlot, BOOL ForceSelect = FALSE);
	virtual	void	OnSelectGoodsRadio(UINT32 RadioIndex = 0);
	virtual	void	OnBuyGoodsSuccess()				{}
	virtual	void	OnPreGuiNotify()				{}
	virtual	void	OnLinkControl(i3BinList * plist) = 0;
	virtual	void	OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify) = 0;

	void	OnChangeScroll();
	void	OnWheelUpdate(INT32 ScrollBy);

	GoodsPriceFrame *	GetPriceFrame()	{return &m_PriceFrame;}
	UINT32				GetCurrentSelectedGoodsIndex()	{return m_SelectGoodsIndexInPack;}
	UINT32				GetSelectItemCurrentPriceType()	{return m_SelectItemCurrentPriceType;}

	SHOP_GOODS_PACK *	GetFocusGoodsPack()		{return m_pFocusedGoods;}

protected:
	static	UINT32 _GetCurrnetGoodsBuyFlag(UINT32 nGoodsBuyFlag);

	void	_UpdateGoodsFrame(RT_REAL32 rDeltaSeconds);
	virtual	BOOL	OnUpdateGoodsFrameText(SHOP_GOODS * pGoods, INT32 GoodsIndex);

protected:
	CStepShop *		m_pOwner;
	const INT32		m_SlotMaxCount;		// 슬롯에 표시되는 상품의 총 개수.
	const INT32		m_SlotHoriCount;	// 슬롯 가로 개수.

	i3GuiButton *	m_pTab;
	i3GuiStatic *	m_pSect;
	i3GuiList *		m_pScrollList;
	INT32			m_idxStartSlot;

	i3List			m_RefGoodsList;

	UINT32			m_SelectGoodsIndexInPack;	// 팩에서의 인덱스
	UINT32			m_SelectItemPriceType;
	UINT32			m_SelectItemCurrentPriceType;

	GoodsPriceFrame		m_PriceFrame;
	SHOP_GOODS_PACK *	m_pFocusedGoods;

protected:
	bool m_bVestigeDuty;
	WEAPON_SLOT_TYPE m_OldWeaponUsageType;
	INT32	m_OldidxStartSlot;
	INT32	m_OldidxTotalSlot;

public:
	virtual void _ResetVestige();
	virtual void _SetupVestige();
};







#endif