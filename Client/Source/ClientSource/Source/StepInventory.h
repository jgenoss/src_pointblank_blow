#ifndef _StepInventory_h
#define _StepInventory_h

#include "StepBase.h"
#include "StageDef.h"
#include "ShopDef.h"
#include "../../CommonSource/WeaponDefine.h"

#include <set>

class CStepInventory: public CStepBase
{
	I3_CLASS_DEFINE(CStepInventory);
public:
	CStepInventory();
	virtual ~CStepInventory();

	virtual void OnPreGuiNotify(void);	
	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnExitKey(void);
	virtual void OnWheelUpdate(INT32 scrollby);	
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual void OnStart(void);
	virtual void OnEnd(void);
	virtual void OnInitRun(void);

	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);
	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual void AuthStartSuccess(INT32 arg);

	void EnterInventory(void);
	void LeaveInventory(void);

	void OnLeave(void);

	//	Weapon
	void OnWeaponTab(bool ResetVestige = false);	
	void OnAllWeapon(void);
	void OnPrimaryWeapon(void);
	void OnSecondaryWeapon(void);
	void OnMeleeWeapon(void);
	void OnThrowingWeapon(void);
	void OnItemWeapon(void);
	void OnChangeWeaponScroll(void);
	void OnFocusWeapon(UINT32 idxItem);
	void OnSelectWeapon(UINT32 idxItem);
	void OnEquipWeapon(UINT32 idxItem);
	void OnAuthWeapon(void);
	void OnDetailWeapon(void);
	void OnEquipWeapon(void);
	void OnRepairWeapon(void);

	//	Character
	void OnCharacterTab(void);	
	void OnAllCharacter(void);
	void OnCharacterType(void);
	void OnCharacterHead(void);
	void OnCharacterBeret(void);
	void OnChangeCharacterScroll(void);
	void OnFocusCharacter(UINT32 idxSlot);
	void OnSelectCharacter(UINT32 idxSlot);
	void OnEquipCharacter(UINT32 idxSlot);
	void OnEquipCharacter(void);
	void OnRemoveCharacter(void);
	void OnDetailCharacter(void);
	void OnAuthCharacter(void);
	void OnTypeButton(void);
	void OnTypePopup(INT32 idxItem);

	//	Item
	void OnItemTab(void);	
	void OnItemGroupAll(void);
	void OnItemGroupMaintenance(void);
	void OnItemGroupExpendable(void);
	void OnChangeItemScroll(void);
	void OnFocusItem(UINT32 slot);
	void OnSelectItem(UINT32 slot);
	void OnAuthCashItem(void);
	void OpenCashItem(INT32 itemID);
	void OnDeleteItem(void);

	static void CbAuthStart(void* pThis,INT32 nParam);
	static void CbUseCashItem( void * pThis, INT32 nParam);
	static void CbDeleteCashItem( void * pThis, INT32 nParam);

private:
	
	void _InitInvenWin(void);
	void _InitInvenCommon(void);
	
	//	Weapon
	void _InitInvenWeaponTab(void);
	void _InitEquipWin(void);
	void _WeaponListUpdate(void);
	void _WeaponDescriptionUpdate(void);
	void _ShowWeaponGage(UINT32 nInfoType, UINT32 nRate, BOOL bBroken = FALSE);
	void _WeaponWinUpdate(void);
	void _EquipSelectedWeapon(void);
	void _ChangeWeaponUsage(INT32 nWeaponUsage);
	BOOL _IsDefaultSupplyWeapon(INT32 nClassType,INT32 nWeaponNumber);

	//	Character
	void _InitCharaWin(void);
	void _InitInvenCharaTab(void);
	void _CharacterListUpdate(void);
	void _BuildCharacterList(CHARACTER_CLASS_TYPE type = CHARACTER_CLASS_ALL);
	void _CharacterDescriptionUpdate(void);
	void _ChangeCharacterClassType(CHARACTER_CLASS_TYPE type);	
	void _CharacterWinUpdate(void);
	void _SetToCharacterSlot(INT32 slotNum, BOOL Selected, INT32 charaID, INT32 charaTeam, INT32 charaTerm);
	BOOL _IsUsedCharacter(INT32 ItemID, INT32 nTeam);	
	BOOL _IsDefaultSupplyCharacter(INT32 ItemID);	

	//	Item
	void _InitInvenItemTab(void);
	void _ItemListUpdate(void);
	void _ItemDescriptionUpdate(void);
	void _ChangeItemUsageType(ITEM_TYPE nType);
	void _SetToItemSlot(INT32 slotNum, INT32 selectedSlotNum, INT32 startSlotNum, INT32 ItemID, INT32 term);
	void _ClearItemSlot(INT32 nSlot);

	void _GetInvenUseDay(BOOL ExpendableItem, UINT8 itemAttr, UINT32 itemArg, char * pOut, const size_t sizeOutMax);

	// 아이템의 상태에 따라서 장비버튼 또는 인증버튼을 활성화한다. (*인증버튼도 UI상에는 '장비하기' 텍스트로 나온다)
	void _SetAuthButton(UINT32 ItemID, UINT8 ItemAttr, i3GuiButton * pEquipButton, i3GuiButton * pAuthButton);

	void _ApplyScroll();
private:	

	i3GuiStatic* m_pInvenGui;
	i3GuiStatic* m_pInvenWin;
	i3GuiEditBox* m_pInvenCaption;	
	i3GuiButton* m_pLeave;

	//	Weapon
	i3GuiList* m_pWeaponScrollList;

	INT32 m_nWeaponUsageType;
	INT32 m_idxSelectedWeaponSlot;
	INT32 m_idxWeaponStartSlot;
	BOOL m_bWeaponChanged;

	WeaponEquip m_weaponEquip[WEU_MAX];
	WeaponSlot m_weaponSlot[MAX_SLOT_FOR_INVEN_WEAPON];	
	WeaponInfo m_weaponInfo[MAX_WEAPON_INFO];

	i3GuiStatic* m_pWeaponWin;
	i3GuiEditBox* m_pWeaponCaption;
	i3GuiStatic* m_pWeaponSect;
	i3GuiButton* m_pWeaponTab;

	i3GuiButton* m_pAllWeapon;
	i3GuiButton* m_pPrimary;
	i3GuiButton* m_pSecondary;
	i3GuiButton* m_pMelee;
	i3GuiButton* m_pThrowing;
	i3GuiButton* m_pItem;

	i3GuiStatic* m_pWeaponInfo;
	i3GuiStatic* m_pWeaponImage;
	i3GuiEditBox* m_pWeaponInfoKey;
	i3GuiEditBox* m_pWeaponInfoValue;
	i3GuiEditBox* m_pWeaponInfoTerm;
	i3GuiStatic* m_pWeaponInfoExtension;
	i3GuiButton* m_pDetailWeapon;
	i3GuiButton* m_pEquipWeapon;
	i3GuiButton* m_pAuthWeapon;

	i3GuiButton* m_pRepairWeapon;
	
	i3GuiEditBox* m_pBulletKey;
	i3GuiEditBox* m_pBulletValue;

	i3GuiEditBox* m_pRangeKey;
	i3GuiEditBox* m_pRangeValue;

	//	Character
	i3GuiList* m_pCharaScrollList;

	CHARACTER_CLASS_TYPE m_nCharacterClassType;
	INT32 m_idxSelectedCharacterSlot;
	INT32 m_idxDescriptionCharacterID;
	INT32 m_idxDescriptionCharacterTerm;
	INT32 m_idxDescriptionCharacterTeam;
	INT32 m_idxCharacterStartSlot;
	INT32 m_nCharacterFilter;
	BOOL m_bCharacterChanged;

	CharacterSlot m_characterSlot[MAX_SLOT_FOR_INVEN_CHARACTER];

	i3GuiStatic* m_pCharacterWin;
	i3GuiEditBox* m_pCharacterCaption;
	i3GuiStatic* m_pCharacterSect;	
	i3GuiButton* m_pCharacterTab;

	i3GuiStatic* m_pRedTypeSlot;
	i3GuiEditBox* m_pRedTypeKey;
	i3GuiStatic* m_pRedTypeDummy;
	i3GuiStatic* m_pDinoTypeSlot;
	i3GuiEditBox* m_pDinoTypeKey;
	i3GuiStatic* m_pDinoTypeDummy;
	i3GuiStatic* m_pBlueTypeSlot;
	i3GuiEditBox* m_pBlueTypeKey;
	i3GuiStatic* m_pBlueTypeDummy;
	i3GuiStatic* m_pHeadSlot;
	i3GuiEditBox* m_pHeadKey;
	i3GuiStatic* m_pHeadDummy;
	i3GuiStatic* m_pBeretSlot;
	i3GuiEditBox* m_pBeretKey;
	i3GuiStatic* m_pBeretDummy;

	i3GuiButton* m_pAllCharacter;
	i3GuiButton* m_pTypeCharacter;
	i3GuiButton* m_pHeadCharacter;
	i3GuiButton* m_pBeretCharacter;
	i3GuiStatic* m_pCharacterInfo;
	i3GuiStatic* m_pCharacterInfoDummy;
	i3GuiEditBox* m_pCharacterInfoValue;
	i3GuiEditBox* m_pCharacterTermKey;
	i3GuiEditBox* m_pCharacterTermValue;
	i3GuiButton* m_pCharacterDetail;
	i3GuiButton* m_pCharacterEquip;
	i3GuiButton* m_pCharacterRemove;
	i3GuiButton* m_pCharacterAuth;
	i3GuiEditBox* m_pCharacterTypeCombo;
	i3GuiButton* m_pCharacterTypeButton;
	i3GuiPopupList* m_pCharacterTypePopup;	
	i3GuiEditBox* m_pCharacterDescription;

	i3List m_ShopSlotList;

	//	Item
	i3GuiList* m_pItemScrollList;

	ITEM_TYPE m_nItemUsageType;
	INT32 m_idxSelectedItemSlot;
	INT32 m_idxDescriptionItemID;
	INT32 m_idxItemStartSlot;

	CashItemSlot m_itemSlot[MAX_SLOT_FOR_INVEN_ITEM];

	i3GuiStatic* m_pItemSect;	
	i3GuiButton* m_pItemTab;

	i3GuiButton* m_pItemGroupAll;
	i3GuiButton* m_pItemGroupMaintenance;
	i3GuiButton* m_pItemGroupExpendable;

	i3GuiStatic* m_pItemDetail;
	i3GuiStatic* m_pItemDetailDummy;
	i3GuiEditBox* m_pItemDetailValue;
	i3GuiButton* m_pItemDelete;
	i3GuiButton* m_pItemDetailUse;
	i3GuiEditBox* m_pItemDetailDescription;

	INT64 m_authID;
	BOOL m_bWaitAuthStart;
};

#endif //_StepInventory_h
