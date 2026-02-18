#include "pch.h"
#include "StepInventory.h"
#include "StageReady.h"
#include "GlobalVariables.h"
#include "StageGui/Popup/PopupExtension.h"
#include "StageGui/Popup/PopupUseItem.h"
#include "StageGui/Popup/PopupDeleteItem.h"
#include "GameCharaDef.h"
#include "Shop.h"

#include "CharaAbilityManager.h"

struct ShopSlot
{
	INT32 _ItemID;
	INT32 _team;
	INT32 _term;
	BOOL _possible;
};


I3_CLASS_INSTANCE(CStepInventory, CStepBase);

CStepInventory::CStepInventory()
{
	m_pInvenGui = NULL;

	m_pInvenWin = NULL;
	m_pWeaponWin = NULL;
	m_pCharacterWin = NULL;
	m_pInvenCaption = NULL;
	m_pWeaponCaption = NULL;
	m_pCharacterCaption = NULL;

	i3mem::FillZero(m_weaponEquip, sizeof(m_weaponEquip));

	m_pWeaponSect = NULL;
	m_pCharacterSect = NULL;
	m_pWeaponTab = NULL;
	m_pCharacterTab  = NULL;
	m_pLeave = NULL;
	m_pAllWeapon = NULL;
	m_pPrimary = NULL;
	m_pSecondary = NULL;
	m_pMelee = NULL;
	m_pThrowing = NULL;
	m_pItem = NULL;
	m_pWeaponScrollList = NULL;
	m_pCharaScrollList = NULL;

	i3mem::FillZero( m_weaponSlot, sizeof( m_weaponSlot));

	m_pWeaponInfo = NULL;
	m_pWeaponImage = NULL;
	m_pWeaponInfoKey = NULL;
	m_pWeaponInfoValue = NULL;
	m_pWeaponInfoTerm = NULL;
	m_pWeaponInfoExtension = NULL;
	m_pDetailWeapon = NULL;
	m_pEquipWeapon = NULL;
	m_pAuthWeapon = NULL;
	m_pRepairWeapon	= NULL;

	i3mem::FillZero(m_weaponInfo, sizeof(m_weaponInfo));

	m_pBulletKey = NULL;
	m_pBulletValue = NULL;

	m_pRedTypeSlot = NULL;
	m_pRedTypeKey = NULL;
	m_pRedTypeDummy = NULL;
	m_pDinoTypeSlot = NULL;
	m_pDinoTypeKey = NULL;
	m_pDinoTypeDummy = NULL;
	m_pBlueTypeSlot = NULL;
	m_pBlueTypeKey = NULL;
	m_pBlueTypeDummy = NULL;
	m_pHeadSlot = NULL;
	m_pHeadKey = NULL;
	m_pHeadDummy = NULL;
	m_pAllCharacter = NULL;
	m_pTypeCharacter = NULL;
	m_pHeadCharacter = NULL;

	i3mem::FillZero( m_characterSlot, sizeof( m_characterSlot));

	m_pCharacterInfo = NULL;
	m_pCharacterInfoDummy = NULL;
	m_pCharacterInfoValue = NULL;
	m_pCharacterTermKey = NULL;
	m_pCharacterTermValue = NULL;
	m_pCharacterDetail = NULL;
	m_pCharacterEquip = NULL;
	m_pCharacterRemove = NULL;
	m_pCharacterAuth = NULL;
	m_pCharacterTypeCombo = NULL;
	m_pCharacterTypeButton = NULL;
	m_pCharacterTypePopup = NULL;
	m_pCharacterDescription = NULL;

	m_nWeaponUsageType = WEAPON_SLOT_UNKNOWN;
	m_idxSelectedWeaponSlot = INVALID_INDEX;
	m_idxWeaponStartSlot = 0;
	m_bWeaponChanged = FALSE;

	m_nCharacterClassType = CHARACTER_CLASS_ALL;
	m_idxSelectedCharacterSlot = INVALID_INDEX;
	m_idxDescriptionCharacterID = INVALID_INDEX;
	m_idxDescriptionCharacterTerm = INVALID_INDEX;
	m_idxDescriptionCharacterTeam = INVALID_INDEX;
	m_idxCharacterStartSlot = 0;
	m_bCharacterChanged = FALSE;

	m_nCharacterFilter = CHARACTER_TEAM_RB;

	m_pItemScrollList = NULL;
	m_pItemSect = NULL;
	m_pItemTab = NULL;

	m_nItemUsageType = ITEM_TYPE_UNKNOWN;
	m_idxSelectedItemSlot = INVALID_INDEX;
	m_idxDescriptionItemID = INVALID_INDEX;
	m_idxItemStartSlot = 0;

	i3mem::FillZero( m_itemSlot, sizeof( m_itemSlot));

	m_pItemGroupAll = NULL;
	m_pItemGroupMaintenance = NULL;
	m_pItemGroupExpendable = NULL;

	m_pItemDetail = NULL;
	m_pItemDetailDummy = NULL;
	m_pItemDetailValue = NULL;
	m_pItemDelete = NULL;
	m_pItemDetailUse = NULL;
	m_pItemDetailDescription = NULL;

	m_authID = 0;
	m_bWaitAuthStart = FALSE;

	//CharaAbilityManager::Get()->SetStepInventory( this );

	m_pBeretCharacter = NULL;
	m_pBeretDummy = NULL;
	m_pBeretKey = NULL;
	m_pBeretSlot = NULL;
	m_pRangeKey = NULL;
	m_pRangeValue = NULL;
}

CStepInventory::~CStepInventory()
{
	if( m_ShopSlotList.GetCount() > 0)
	{
		ShopSlot * pSlotArray = (ShopSlot*) m_ShopSlotList.GetFirst();
		I3MEM_SAFE_FREE( pSlotArray);
		m_ShopSlotList.Clear();
	}
}

void CStepInventory::OnPreGuiNotify(void)
{
	CStepBase::OnPreGuiNotify();

	// 온마우스 된 웨폰 아이템을 찾아서 원상태로 돌린다
	for(UINT32 i = 0; i < MAX_SLOT_FOR_INVEN_WEAPON; i++)
	{
		if (m_weaponSlot[i]._pBg->GetShapeEnable(GUI_SHAPE_OFFSET + 1))
		{
			m_weaponSlot[i]._pBg->SetShapeEnable(GUI_SHAPE_OFFSET + 1, FALSE);
			m_weaponSlot[i]._pBg->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
		}
	}
}

void CStepInventory::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	//	Weapon
	case GCI_IVS_B_WEAPON_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnWeaponTab(true);
		break;
	case GCI_IVS_B_CHARACTER_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCharacterTab();
		break;
	case GCI_IVS_B_LEAVE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnLeave();
		break;
	case GCI_IVS_B_ALL_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnAllWeapon();
		break;
	case GCI_IVS_B_PRIMARY:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnPrimaryWeapon();
		break;
	case GCI_IVS_B_SECONDARY:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSecondaryWeapon();
		break;
	case GCI_IVS_B_MELEE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnMeleeWeapon();
		break;
	case GCI_IVS_B_THROWING:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnThrowingWeapon();
		break;
	case GCI_IVS_B_ITEM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnItemWeapon();
		break;
	case GCI_IVS_L_WEAPON:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnChangeWeaponScroll();
		break;
	case GCI_IVS_S_WEAPON_SLOT_1:	// fall through
	case GCI_IVS_S_WEAPON_SLOT_2:	// fall through
	case GCI_IVS_S_WEAPON_SLOT_3:	// fall through
	case GCI_IVS_S_WEAPON_SLOT_4:	// fall through
	case GCI_IVS_S_WEAPON_SLOT_5:	// fall through
	case GCI_IVS_S_WEAPON_SLOT_6:	// fall through
	case GCI_IVS_S_WEAPON_SLOT_7:	// fall through
	case GCI_IVS_S_WEAPON_SLOT_8:
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnFocusWeapon(pNotify->m_nID - GCI_IVS_S_WEAPON_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) OnSelectWeapon(pNotify->m_nID - GCI_IVS_S_WEAPON_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent) OnEquipWeapon(pNotify->m_nID - GCI_IVS_S_WEAPON_SLOT_1);
		break;
	case GCI_IVS_B_DETAIL_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnDetailWeapon();
		break;
	case GCI_IVS_B_EQUIP_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnEquipWeapon();
		break;
	case GCI_IVS_B_AUTH_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnAuthWeapon();
		break;
	case GCI_IVS_B_REPAIR_WEAPON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnRepairWeapon();
		break;
	//	Character
	case GCI_IVS_L_CHARACTER:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnChangeCharacterScroll();
		break;
	// 의미 없는 함수 막음.
	//case GCI_IVS_S_HEAD:
	//	if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) OnHeadSlot();
	//	break;
	//case GCI_IVS_S_RED_TYPE:
	//	if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) OnRedSlot();
	//	break;
	//case GCI_IVS_S_BLUE_TYPE:
	//	if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) OnBlueSlot();
	//	break;
	case GCI_IVS_B_ALL_CHARACTER:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnAllCharacter();
		break;
	case GCI_IVS_B_Type:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCharacterType();
		break;
	case GCI_IVS_B_HEAD:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCharacterHead();
		break;
	case GCI_IVS_B_BERET:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCharacterBeret();
		break;
	case GCI_IVS_B_CHARACTER_SLOT_1:	// fall through
	case GCI_IVS_B_CHARACTER_SLOT_2:	// fall through
	case GCI_IVS_B_CHARACTER_SLOT_3:	// fall through
	case GCI_IVS_B_CHARACTER_SLOT_4:	// fall through
	case GCI_IVS_B_CHARACTER_SLOT_5:	// fall through
	case GCI_IVS_B_CHARACTER_SLOT_6:	// fall through
	case GCI_IVS_B_CHARACTER_SLOT_7:	// fall through
	case GCI_IVS_B_CHARACTER_SLOT_8:	// fall through
	case GCI_IVS_B_CHARACTER_SLOT_9:
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnFocusCharacter(pNotify->m_nID - GCI_IVS_B_CHARACTER_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) OnSelectCharacter(pNotify->m_nID - GCI_IVS_B_CHARACTER_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent) OnEquipCharacter(pNotify->m_nID - GCI_IVS_B_CHARACTER_SLOT_1);
		break;
	case GCI_IVS_B_CHARACTER_DETAIL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnDetailCharacter();
		break;
	case GCI_IVS_B_CHARACTER_AUTH:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnAuthCharacter();
		break;
	case GCI_IVS_B_CHARACTER_EQUIP:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnEquipCharacter();
		break;
	case GCI_IVS_B_CHARACTER_REMOVE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnRemoveCharacter();
		break;
	case GCI_IVS_B_TYPE_BUTTON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnTypeButton();
		break;
	case GCI_IVS_L_TYPE_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnTypePopup(pNotify->m_nPriParam);
		break;
	//	Item
	case GCI_IVS_B_ITEM_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnItemTab();
		break;
	case GCI_IVS_L_ITEM:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnChangeItemScroll();
		break;
	case GCI_IVS_S_ITEM_SLOT_1:	// fall through
	case GCI_IVS_S_ITEM_SLOT_2:	// fall through
	case GCI_IVS_S_ITEM_SLOT_3:	// fall through
	case GCI_IVS_S_ITEM_SLOT_4:	// fall through
	case GCI_IVS_S_ITEM_SLOT_5:	// fall through
	case GCI_IVS_S_ITEM_SLOT_6:	// fall through
	case GCI_IVS_S_ITEM_SLOT_7:	// fall through
	case GCI_IVS_S_ITEM_SLOT_8:	// fall through
	case GCI_IVS_S_ITEM_SLOT_9:
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnFocusItem(pNotify->m_nID - GCI_IVS_S_ITEM_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) OnSelectItem(pNotify->m_nID - GCI_IVS_S_ITEM_SLOT_1);
		if (I3GUI_STATIC_NOTIFY_DOUBLECLICK == pNotify->m_nEvent) OnAuthCashItem();
		break;
	case GCI_IVS_B_ITEM_USE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnAuthCashItem();
		break;
	case GCI_IVS_B_ITEM_DELETE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnDeleteItem();
		break;
	case GCI_IVS_B_ITEM_ALL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnItemGroupAll();
		break;
	case GCI_IVS_B_ITEM_MAINTENANCE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnItemGroupMaintenance();
		break;
	case GCI_IVS_B_ITEM_EXPENDABLE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnItemGroupExpendable();
		break;
	}
}

BOOL CStepInventory::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	char strName[MAX_STRING_COUNT] = "";

	LinkControlEx(plist, "Inventory", m_pInvenGui, -1);

	LinkControlEx(plist, "ivEquipmentWin", m_pWeaponWin, -1);
	LinkControlEx(plist, "ivCharacterWin", m_pCharacterWin, -1);
	LinkControlEx(plist, "ivInvenWin", m_pInvenWin, -1);
	LinkControlEx(plist, "ivEquipmentCaption", m_pWeaponCaption, -1);
	LinkControlEx(plist, "ivCharacterCaption", m_pCharacterCaption, -1);
	LinkControlEx(plist, "ivInvenCaption", m_pInvenCaption, -1);

	WeaponEquip* pWeapon;

	pWeapon = m_weaponEquip + WEU_PRIMARY;
	LinkControlEx(plist, "ivPrimarySlot", pWeapon->_pImage, GCI_IVS_S_PRIMARY);
	LinkControlEx(plist, "ivPrimaryKey", pWeapon->_pKey, -1);
	LinkControlEx(plist, "ivPrimaryValue", pWeapon->_pValue, -1);

	pWeapon  = m_weaponEquip + WEU_SECONDARY;
	LinkControlEx(plist, "ivSecondarySlot", pWeapon->_pImage, GCI_IVS_S_SECONDARY);
	LinkControlEx(plist, "ivSecondaryKey", pWeapon->_pKey, -1);
	LinkControlEx(plist, "ivSecondaryValue", pWeapon->_pValue, -1);

	pWeapon  = m_weaponEquip + WEU_MELEE;
	LinkControlEx(plist, "ivMeleeSlot", pWeapon->_pImage, GCI_IVS_S_MELEE);
	LinkControlEx(plist, "ivMeleeKey", pWeapon->_pKey, -1);
	LinkControlEx(plist, "ivMeleeValue", pWeapon->_pValue, -1);

	pWeapon  = m_weaponEquip + WEU_THROWING;
	LinkControlEx(plist, "ivThrowingSlot", pWeapon->_pImage, GCI_IVS_S_THROWING);
	LinkControlEx(plist, "ivThrowingKey", pWeapon->_pKey, -1);
	LinkControlEx(plist, "ivThrowingValue", pWeapon->_pValue, -1);

	pWeapon  = m_weaponEquip + WEU_ITEM;
	LinkControlEx(plist, "ivItemSlot", pWeapon->_pImage, GCI_IVS_S_ITEM);
	LinkControlEx(plist, "ivItemKey", pWeapon->_pKey, -1);
	LinkControlEx(plist, "ivItemValue", pWeapon->_pValue, -1);

	LinkControlEx(plist, "ivWeapon", m_pWeaponSect, -1);
	LinkControlEx(plist, "ivCharacter", m_pCharacterSect, -1);
	LinkControlEx(plist, "ivInvenWeaponTab", m_pWeaponTab, GCI_IVS_B_WEAPON_TAB);
	LinkControlEx(plist, "ivInvenCharaTab", m_pCharacterTab, GCI_IVS_B_CHARACTER_TAB);
	LinkControlEx(plist, "ivInvenLeave", m_pLeave, GCI_IVS_B_LEAVE);

	LinkControlEx(plist, "ivAllWeapon", m_pAllWeapon, GCI_IVS_B_ALL_WEAPON);
	LinkControlEx(plist, "ivPrimaryWeapon", m_pPrimary, GCI_IVS_B_PRIMARY);
	LinkControlEx(plist, "ivSecondaryWeapon", m_pSecondary, GCI_IVS_B_SECONDARY);
	LinkControlEx(plist, "ivMeleeWeapon", m_pMelee, GCI_IVS_B_MELEE);
	LinkControlEx(plist, "ivThrowingWeapon", m_pThrowing, GCI_IVS_B_THROWING);
	LinkControlEx(plist, "ivItemWeapon", m_pItem, GCI_IVS_B_ITEM);

	LinkControlEx(plist, "ivWeaponList", m_pWeaponScrollList, GCI_IVS_L_WEAPON);
	LinkControlEx(plist, "ivCharacterList", m_pCharaScrollList, GCI_IVS_L_CHARACTER);

	for(UINT32 i = 0; i < MAX_SLOT_FOR_INVEN_WEAPON; i++)
	{
		sprintf_s(strName, "ivWeaponSlot%d", i + 1);
		LinkControlEx(plist, strName, m_weaponSlot[i]._pBg, -1);

		sprintf_s(strName, "ivWeaponSlotValue%d", i + 1);
		LinkControlEx(plist, strName, m_weaponSlot[i]._pValue, -1);

		sprintf_s(strName, "ivWeaponSlotKey%d", i + 1);
		LinkControlEx(plist, strName, m_weaponSlot[i]._pKey, -1);

		sprintf_s(strName, "ivWeaponSlotDummy%d", i + 1);
		LinkControlEx(plist, strName, m_weaponSlot[i]._pImage, i);

		sprintf_s(strName, "ivWeaponSlotExtension%d", i + 1);
		LinkControlEx(plist, strName, m_weaponSlot[i]._pExtension, -1);

		sprintf_s(strName, "ivWeaponSlotUsed%d", i + 1);
		LinkControlEx(plist, strName, m_weaponSlot[i]._pUsed, -1);

		sprintf_s(strName, "ivWeaponSlotTerm%d", i + 1);
		LinkControlEx(plist, strName, m_weaponSlot[i]._pTerm, -1);

		sprintf_s(strName, "ivWeaponSlotSaleMark%d", i + 1);
		LinkControlEx(plist, strName, m_weaponSlot[i]._pSaleMark, -1);

		sprintf_s(strName, "ivWeaponSlotFilter%d", i + 1);
		LinkControlEx(plist, strName, m_weaponSlot[i]._pFilter, GCI_IVS_S_WEAPON_SLOT_1 + i);

		sprintf_s(strName, "ivWeaponSlotDurabilityBar%d", i + 1);
		LinkControlEx(plist, strName, m_weaponSlot[i]._pDurabilityBar, -1);

		sprintf_s(strName, "ivWeaponSlotDurabilityGauge%d", i + 1);
		LinkControlEx(plist, strName, m_weaponSlot[i]._pDurabilityGauge, -1);
	}

	LinkControlEx(plist, "ivInvenWeaponInfoImage", m_pWeaponInfo, -1);
	LinkControlEx(plist, "ivWeaponDummy", m_pWeaponImage, -1);
	LinkControlEx(plist, "ivWeaponInfoExtension", m_pWeaponInfoExtension, -1);
	LinkControlEx(plist, "ivWeaponKey", m_pWeaponInfoKey, -1);
	LinkControlEx(plist, "ivWeaponValue", m_pWeaponInfoValue, -1);
	LinkControlEx(plist, "ivTerm", m_pWeaponInfoTerm, -1);

	LinkControlEx(plist, "ivInvenWeaponInfoDetail", m_pDetailWeapon, GCI_IVS_B_DETAIL_WEAPON);
	LinkControlEx(plist, "ivInvenWeaponInfoUse", m_pEquipWeapon, GCI_IVS_B_EQUIP_WEAPON);
	LinkControlEx(plist, "ivInvenWeaponInfoAuth", m_pAuthWeapon, GCI_IVS_B_AUTH_WEAPON);
	LinkControlEx(plist, "ivInvenWeaponRepair", m_pRepairWeapon, GCI_IVS_B_REPAIR_WEAPON);

	WeaponInfo* pWeaponInfo;

	pWeaponInfo = m_weaponInfo + WEAPON_INFO_DAMAGE;
	LinkControlEx(plist, "ivDamageKey", pWeaponInfo->_pKey, -1);
	LinkControlEx(plist, "ivDamageValue", pWeaponInfo->_pValue, -1);
	LinkControlEx(plist, "ivDamageGage", pWeaponInfo->_pGage, -1);

	pWeaponInfo = m_weaponInfo + WEAPON_INFO_RAPID;
	LinkControlEx(plist, "ivRapidKey", pWeaponInfo->_pKey, -1);
	LinkControlEx(plist, "ivRapidValue", pWeaponInfo->_pValue, -1);
	LinkControlEx(plist, "ivRapidGage", pWeaponInfo->_pGage, -1);

	pWeaponInfo = m_weaponInfo + WEAPON_INFO_HIT_RATE;
	LinkControlEx(plist, "ivHitRateKey", pWeaponInfo->_pKey, -1);
	LinkControlEx(plist, "ivHitRateValue", pWeaponInfo->_pValue, -1);
	LinkControlEx(plist, "ivHitRateGage", pWeaponInfo->_pGage, -1);

	pWeaponInfo = m_weaponInfo + WEAPON_INFO_VERT_RECOIL;
	LinkControlEx(plist, "ivVertRecoilKey", pWeaponInfo->_pKey, -1);
	LinkControlEx(plist, "ivVertRecoilValue", pWeaponInfo->_pValue, -1);
	LinkControlEx(plist, "ivVertRecoilGage", pWeaponInfo->_pGage, -1);

	pWeaponInfo = m_weaponInfo + WEAPON_INFO_HORZ_RECOIL;
	LinkControlEx(plist, "ivHorzRecoilKey", pWeaponInfo->_pKey, -1);
	LinkControlEx(plist, "ivHorzRecoilValue", pWeaponInfo->_pValue, -1);
	LinkControlEx(plist, "ivHorzRecoilGage", pWeaponInfo->_pGage, -1);

	pWeaponInfo = m_weaponInfo + WEAPON_INFO_SPEED;
	LinkControlEx(plist, "ivSpeedKey", pWeaponInfo->_pKey, -1);
	LinkControlEx(plist, "ivSpeedValue", pWeaponInfo->_pValue, -1);
	LinkControlEx(plist, "ivSpeedGage", pWeaponInfo->_pGage, -1);

	LinkControlEx(plist, "ivBulletKey", m_pBulletKey, -1);
	LinkControlEx(plist, "ivBulletValue", m_pBulletValue, -1);

	LinkControlEx(plist, "ivRangeKey", m_pRangeKey, -1);
	LinkControlEx(plist, "ivRangeValue", m_pRangeValue, -1);


	LinkControlEx(plist, "ivRedTypeSlot", m_pRedTypeSlot, GCI_IVS_S_RED_TYPE);
	LinkControlEx(plist, "ivRedTypeKey", m_pRedTypeKey, -1);
	LinkControlEx(plist, "ivRedTypeDummy", m_pRedTypeDummy, -1);
	LinkControlEx(plist, "ivDinoTypeSlot", m_pDinoTypeSlot, GCI_IVS_S_RED_TYPE);
	LinkControlEx(plist, "ivDinoTypeKey", m_pDinoTypeKey, -1);
	LinkControlEx(plist, "ivDinoTypeDummy", m_pDinoTypeDummy, -1);
	LinkControlEx(plist, "ivBlueTypeSlot", m_pBlueTypeSlot, GCI_IVS_S_BLUE_TYPE);
	LinkControlEx(plist, "ivBlueTypeKey", m_pBlueTypeKey, -1);
	LinkControlEx(plist, "ivBlueTypeDummy", m_pBlueTypeDummy, -1);
	LinkControlEx(plist, "ivHeadSlot", m_pHeadSlot, GCI_IVS_S_HEAD);
	LinkControlEx(plist, "ivHeadKey", m_pHeadKey, -1);
	LinkControlEx(plist, "ivHeadDummy", m_pHeadDummy, -1);
	LinkControlEx(plist, "ivBeretSlot", m_pBeretSlot, -1);
	LinkControlEx(plist, "ivBeretKey", m_pBeretKey, -1);
	LinkControlEx(plist, "ivBeretDummy", m_pBeretDummy, -1);
	LinkControlEx(plist, "ivAllCharacter", m_pAllCharacter, GCI_IVS_B_ALL_CHARACTER);
	LinkControlEx(plist, "ivTypeCharacter", m_pTypeCharacter, GCI_IVS_B_Type);
	LinkControlEx(plist, "ivHeadCharacter", m_pHeadCharacter, GCI_IVS_B_HEAD);
	LinkControlEx(plist, "ivBeretCharacter", m_pBeretCharacter, GCI_IVS_B_BERET);

	for(UINT32 i = 0; i < MAX_SLOT_FOR_INVEN_CHARACTER; i++)
	{
		sprintf_s(strName, "ivCharacterSlot%d", i + 1);
		LinkControlEx(plist, strName, m_characterSlot[i]._pSlot, -1);

		sprintf_s(strName, "ivCharacterDummy%d", i + 1);
		LinkControlEx(plist, strName, m_characterSlot[i]._pImage, -1);

		sprintf_s(strName, "ivCharacterValue%d", i + 1);
		LinkControlEx(plist, strName, m_characterSlot[i]._pValue, -1);

		sprintf_s(strName, "ivCharacterUsed%d", i + 1);
		LinkControlEx(plist, strName, m_characterSlot[i]._pUsed, -1);

		sprintf_s(strName, "ivCharacterTerm%d", i + 1);
		LinkControlEx(plist, strName, m_characterSlot[i]._pTerm, -1);

		sprintf_s(strName, "ivCharacterSaleMark%d", i + 1);
		LinkControlEx(plist, strName, m_characterSlot[i]._pSaleMark, -1);

		sprintf_s(strName, "ivCharacterFilter%d", i + 1);
		LinkControlEx(plist, strName, m_characterSlot[i]._pFilter, GCI_IVS_B_CHARACTER_SLOT_1 + i);
	}

	LinkControlEx(plist, "ivCharacterInfo", m_pCharacterInfo, -1);
	LinkControlEx(plist, "ivCharacterInfoDummy", m_pCharacterInfoDummy, -1);
	LinkControlEx(plist, "ivCharacterInfoValue", m_pCharacterInfoValue, -1);
	LinkControlEx(plist, "ivCharacterTermKey", m_pCharacterTermKey, -1);
	LinkControlEx(plist, "ivCharacterTermValue", m_pCharacterTermValue, -1);
	LinkControlEx(plist, "ivCharacterDetail", m_pCharacterDetail, GCI_IVS_B_CHARACTER_DETAIL);
	LinkControlEx(plist, "ivCharacterEquip", m_pCharacterEquip, GCI_IVS_B_CHARACTER_EQUIP);
	LinkControlEx(plist, "ivCharacterAuth", m_pCharacterAuth, GCI_IVS_B_CHARACTER_AUTH);
	LinkControlEx(plist, "ivCharacterRemove", m_pCharacterRemove, GCI_IVS_B_CHARACTER_REMOVE);
	LinkControlEx(plist, "ivCharacterTypeCombo", m_pCharacterTypeCombo, -1);
	LinkControlEx(plist, "ivCharacterTypeButton", m_pCharacterTypeButton, GCI_IVS_B_TYPE_BUTTON);
	LinkControlEx(plist, "ivCharacterTypePopup", m_pCharacterTypePopup, GCI_IVS_L_TYPE_POPUP);
	LinkControlEx(plist, "ivCharacterDescription", m_pCharacterDescription, -1);

	//	Item
	{
		LinkControlEx(plist, "ivInvenItemTab", m_pItemTab, GCI_IVS_B_ITEM_TAB);
		LinkControlEx(plist, "ivItemSlotList", m_pItemScrollList, GCI_IVS_L_ITEM);
		LinkControlEx(plist, "ivItemSect", m_pItemSect, -1);

		LinkControlEx(plist, "ivItemGroupAll", m_pItemGroupAll, GCI_IVS_B_ITEM_ALL);
		LinkControlEx(plist, "ivItemGroupMaintenance", m_pItemGroupMaintenance, GCI_IVS_B_ITEM_MAINTENANCE);
		LinkControlEx(plist, "ivItemGroupExpendable", m_pItemGroupExpendable, GCI_IVS_B_ITEM_EXPENDABLE);

		for(UINT32 i = 0; i < MAX_SLOT_FOR_INVEN_ITEM; i++)
		{
			sprintf_s(strName, "ivItemSlot%d", i + 1);
			LinkControlEx(plist, strName, m_itemSlot[i]._pSlot, -1);

			sprintf_s(strName, "ivItemSlotDummy%d", i + 1);
			LinkControlEx(plist, strName, m_itemSlot[i]._pImage, -1);

			sprintf_s(strName, "ivItemSlotValue%d", i + 1);
			LinkControlEx(plist, strName, m_itemSlot[i]._pValue, -1);

			sprintf_s(strName, "ivItemSlotUsed%d", i + 1);
			LinkControlEx(plist, strName, m_itemSlot[i]._pUsed, -1);

			sprintf_s(strName, "ivItemSlotTerm%d", i + 1);
			LinkControlEx(plist, strName, m_itemSlot[i]._pTerm, -1);

			sprintf_s(strName, "ivItemSlotSaleMark%d", i + 1);
			LinkControlEx(plist, strName, m_itemSlot[i]._pSaleMark, -1);

			sprintf_s(strName, "ivItemSlotFilter%d", i + 1);
			LinkControlEx(plist, strName, m_itemSlot[i]._pFilter, GCI_IVS_S_ITEM_SLOT_1 + i);
		}

		LinkControlEx(plist, "ivItemDetailInfo", m_pItemDetail, -1);
		LinkControlEx(plist, "ivItemDetailInfoDummy", m_pItemDetailDummy, -1);
		LinkControlEx(plist, "ivItemDetailInfoValue", m_pItemDetailValue, -1);
		LinkControlEx(plist, "ivItemDelete", m_pItemDelete, GCI_IVS_B_ITEM_DELETE);
		LinkControlEx(plist, "ivItemDetailUse", m_pItemDetailUse, GCI_IVS_B_ITEM_USE);
		LinkControlEx(plist, "ivItemDetailDescription", m_pItemDetailDescription, -1);
	}

#if ! defined(USE_REPAIR_WEAPON)
	m_pRepairWeapon->SetEnable(FALSE);
#endif

	return FALSE;
}

void CStepInventory::OnInitControl(void)
{
	m_pInvenGui->SetEnable(FALSE);

	_InitEquipWin();
	_InitCharaWin();
	_InitInvenWin();

	CStepBase::OnInitControl();
}

void CStepInventory::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CStepBase::OnUpdate(rDeltaSeconds);
/*
	if (m_bWaitAuthStart)
	{
		// CbAuthStart에서 해야되는 일이지만, 팝업이 종료될때 자동으로
		// guiInput을 풀어놓는다. 따라서, 팝업 종료후 input을 막는다.
		//i3GuiRoot::setGuiDisableInput(TRUE);
		m_bWaitAuthStart = FALSE;
	}
*/
	if( m_pWeaponSect->isEnable())
	{
		_WeaponWinUpdate();
		_WeaponListUpdate();
		_WeaponDescriptionUpdate();
	}

	if(m_pCharacterSect->isEnable())
	{
		_CharacterWinUpdate();
		_CharacterListUpdate();
		_CharacterDescriptionUpdate();
	}

	if(m_pItemSect->isEnable())
	{
		_ItemListUpdate();
		_ItemDescriptionUpdate();
	}
}

void CStepInventory::OnExitKey(void)
{
	OnLeave();
}

void CStepInventory::OnWheelUpdate(INT32 scrollby)
{
	if (0 < scrollby)
	{
		for(INT32 i = 0; i < scrollby; i++)
		{
			if( m_pWeaponWin->isEnable())
				m_pWeaponScrollList->MovePrev();

			if( m_pCharacterWin->isEnable())
				m_pCharaScrollList->MovePrev();

			if( m_pItemSect->isEnable())
				m_pItemScrollList->MovePrev();
		}
	}
	else
	{
		for(INT32 i = 0; i > scrollby; i--)
		{
			if( m_pWeaponWin->isEnable())
				m_pWeaponScrollList->MoveNext();

			if( m_pCharacterWin->isEnable())
				m_pCharaScrollList->MoveNext();

			if( m_pItemSect->isEnable())
				m_pItemScrollList->MoveNext();
		}
	}

	if( m_pWeaponWin->isEnable())
		m_idxWeaponStartSlot = m_pWeaponScrollList->GetCurrentTop() * SLOT_HORI_COUNT_WEAPON;

	if( m_pCharacterWin->isEnable())
		m_idxCharacterStartSlot = m_pCharaScrollList->GetCurrentTop() * SLOT_HORI_COUNT_CHARA;

	if( m_pItemSect->isEnable())
		m_idxItemStartSlot = m_pItemScrollList->GetCurrentTop() * SLOT_HORI_COUNT_ITEM;

	m_OldidxStartSlot = m_idxWeaponStartSlot;
}

void CStepInventory::SetInputDisable(BOOL bDisable)
{
	m_pInvenGui->setInputDisable(bDisable);
}

BOOL CStepInventory::GetInputDisable(void)
{
	return m_pInvenGui->getInputDisable();
}

void CStepInventory::OnStart(void)
{
	CStepBase::OnStart();

	// 닉네임 설정 전이라면, 시작하지 않는다
	if( ! g_pGameContext->HaveNickName() )
	{
		OnLeave();
		return;
	}

	//	인벤 리스트를 작성한다.
	_BuildCharacterList();

	// Default View Tab
	OnWeaponTab();

	m_authID = 0;

	SetPushedStateButton(MBT_INVENTORY, TRUE);

	CStageReady * pParent = ((CStageReady*)m_pParent);
	I3ASSERT( pParent->IsTypeOf( CStageReady::GetClassMeta()));
}

void CStepInventory::OnEnd(void)
{
	CStepBase::OnEnd();

	SetPushedStateButton(MBT_INVENTORY, FALSE);

	if( m_ShopSlotList.GetCount() > 0)
	{
		ShopSlot * pSlotArray = (ShopSlot*) m_ShopSlotList.GetFirst();
		I3MEM_SAFE_FREE( pSlotArray);
		m_ShopSlotList.Clear();
	}
}

void CStepInventory::OnInitRun(void)
{
	CStepBase::OnInitRun();

	MoveCamera(CL_WEAPON);
}

BOOL CStepInventory::OnSliding(RT_REAL32 rDeltaSeconds)
{
	if (FALSE == CStepBase::OnSliding(rDeltaSeconds))
	{
		return FALSE;
	}

	// 시간에 따른 기준위치 지정
	m_pInvenGui->setPositionY(CStepBase::GetSlideY());


	// 퇴장 시, 비활성화. 그외 활성화
	BOOL enableWindow = TRUE;

	if (SS_OUT == CStepBase::GetSlideState())
	{
		enableWindow = FALSE;
	}

	m_pInvenGui->SetEnable(enableWindow);

	return TRUE;
}

void CStepInventory::OnLeave(void)
{
	CStageReady * pParent = ((CStageReady*)m_pParent);
	I3ASSERT( pParent->IsTypeOf( CStageReady::GetClassMeta()));

	if( g_pFramework->GetClanContext()->InBattleTeam())
	{
		if( g_pFramework->GetClanContext()->GetInClanBattleLobby())
			pParent->ChangeStep(STEP_CLAN_BATTLE_LOBBY);		//	클랜전 로비 예외 처리
		else
			pParent->ChangeStep(STEP_NULL);
	}
	else
	{
		pParent->ChangeStep(STEP_NULL);
	}

	m_authID = 0;
}

void CStepInventory::OnGameEvent(INT32 event,INT32 arg)
{
	//	이벤트 결과를 받아 처리한다.
	switch( event )
	{
	case EVENT_AUTH_DELETE:		// 아이템 삭제
		{
			if( EV_SUCCESSED(arg))
			{
				//STRING_POPUP( GAME_STRING("STR_INVEN_DELETE_ITEM_SUCCESS"));	//	"아이템이 삭제되었습니다."
				
				//	삭제 되었으므로 리스트를 refrash 한다.
				_ChangeItemUsageType(m_nItemUsageType);
			}
			else
			{
				STRING_POPUP( GAME_STRING("STR_INVEN_DELETE_ITEM_FAIL"));	//	"네트워크 문제로\n아이템의 삭제를 실패하였습니다."
			}
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//	Weapon
///////////////////////////////////////////////////////////////////////////////////////////////////////
void CStepInventory::OnWeaponTab(bool ResetVestige/* = false*/)
{
	if( ResetVestige )
		_ResetVestige();

	m_pWeaponTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pCharacterTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pItemTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_pWeaponSect->SetEnable(TRUE);
	m_pCharacterSect->SetEnable(FALSE);
	m_pItemSect->SetEnable(FALSE);
	m_pWeaponWin->SetEnable(TRUE);
	m_pCharacterWin->SetEnable(FALSE);

	switch( _GetFirstGroup() )
	{
	case WEAPON_SLOT_UNKNOWN:	OnAllWeapon();	break;
	case WEAPON_SLOT_PRIMARY:	OnPrimaryWeapon();	break;
	case WEAPON_SLOT_SECONDARY:	OnSecondaryWeapon();	break;
	case WEAPON_SLOT_MELEE:		OnMeleeWeapon();	break;
	case WEAPON_SLOT_THROWING1:	OnThrowingWeapon();	break;
	case WEAPON_SLOT_THROWING2:	OnItemWeapon();	break;
	}
	
	m_idxSelectedWeaponSlot = _GetFirstSelectItem();
	m_idxWeaponStartSlot = _GetFirstStartItem();
	m_pWeaponScrollList->SetCurValue(0.0f);
	_ApplyScroll();
}

void CStepInventory::OnAllWeapon(void)
{
	m_pAllWeapon->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	_ChangeWeaponUsage(WEAPON_SLOT_UNKNOWN);
}

void CStepInventory::OnPrimaryWeapon(void)
{
	m_pPrimary->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	_ChangeWeaponUsage(WEAPON_SLOT_PRIMARY);
}

void CStepInventory::OnSecondaryWeapon(void)
{
	m_pSecondary->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	_ChangeWeaponUsage(ITEM_TYPE_SECONDARY);
}

void CStepInventory::OnMeleeWeapon(void)
{
	m_pMelee->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	_ChangeWeaponUsage(ITEM_TYPE_MELEE);
}

void CStepInventory::OnThrowingWeapon(void)
{
	m_pThrowing->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	_ChangeWeaponUsage(ITEM_TYPE_THROWING1);
}

void CStepInventory::OnItemWeapon(void)
{
	m_pItem->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	_ChangeWeaponUsage(ITEM_TYPE_THROWING2);
}

void CStepInventory::OnChangeWeaponScroll(void)
{
	m_idxWeaponStartSlot = m_pWeaponScrollList->GetCurrentTop() * SLOT_HORI_COUNT_WEAPON;
	m_OldidxStartSlot = m_idxWeaponStartSlot;
}

void CStepInventory::OnFocusWeapon(UINT32 idxItem)
{
	if(MAX_SLOT_FOR_INVEN_WEAPON > idxItem)
	{
		if (FALSE == m_weaponSlot[idxItem]._pBg->GetShapeEnable(GUI_SHAPE_OFFSET + 3))
		{
			m_weaponSlot[idxItem]._pBg->SetShapeEnable(GUI_SHAPE_OFFSET, FALSE);
			m_weaponSlot[idxItem]._pBg->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
		}
	}
}

void CStepInventory::OnSelectWeapon(UINT32 idxItem)
{
	if(MAX_SLOT_FOR_INVEN_WEAPON > idxItem)
	{
		m_idxSelectedWeaponSlot = idxItem + m_idxWeaponStartSlot;

		m_OldidxStartSlot = m_idxWeaponStartSlot;
		m_OldidxTotalSlot = m_idxSelectedWeaponSlot;

		INT32 ItemID = g_pFramework->GetWeaponID(m_idxSelectedWeaponSlot, m_nWeaponUsageType);
		WEAPON_CLASS_TYPE weaponClassType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(ItemID);
		INT32 weaponNum = GET_ITEM_FLAG_NUMBER(ItemID);

		// 무기 이미지
		if ( NULL == g_pWeaponInfoDataBase->getWeaponInfo( weaponClassType, weaponNum) )
		{
			I3FATAL("invalid item select : %d", ItemID);
			m_idxSelectedWeaponSlot = INVALID_INDEX;
		}

		PlaySound(GUI_SND_ITEM_SELECTING);
	}
}

void CStepInventory::OnEquipWeapon(UINT32 idxItem)
{
	if(MAX_SLOT_FOR_INVEN_WEAPON > idxItem)
	{
		m_idxSelectedWeaponSlot = idxItem + m_idxWeaponStartSlot;
	}

	_WeaponDescriptionUpdate();

	if (m_pAuthWeapon->isEnable())
	{
		OnAuthWeapon();
	}
	else if (m_pEquipWeapon->isEnable())
	{
		OnEquipWeapon();
	}
}

void CStepInventory::OnRepairWeapon(void)
{
	// 슬롯 인덱스 체크
	I3ASSERT( m_idxSelectedWeaponSlot != INVALID_INDEX );
	if ( m_idxSelectedWeaponSlot == INVALID_INDEX )
		return;

	// 유효한 무기인지 확인
	INT32 WeaponItemID = g_pFramework->GetWeaponID(m_idxSelectedWeaponSlot, m_nWeaponUsageType);
	I3ASSERT( WeaponItemID != 0 );
	if ( WeaponItemID == 0 )
		return;

	// CTabWeapon::OnRepair 코드와 중복되는 것이 많습니다.
	// 전반적으로 inventory와 shop의 코드는 중복되는 것이 많습니다.
#if !defined( ENABLE_BUY_USER_TITLE_WEAPON)
	char szNeedUserTitle[MAX_STRING_COUNT] = {};
	if( ! CheckItemByDesignation( WeaponItemID, g_pGameContext->GetTitleInfo(), szNeedUserTitle))
	{
		char szMessage[1024] = "";
		/*해당 아이템은 %s 호칭을\n획득하여야 수리할 수 있습니다.\n(호칭 획득 후 수리 가능)*/
		sprintf_s( szMessage, GAME_STRING("STR_SHOP_REQUIRE_DESIGNATION_TO_BUY"), szNeedUserTitle);

		STRING_POPUP( szMessage);
		return;
	}
#endif

	((CStageReady*)m_pParent)->TogglePopup(POPUP_REPAIR, (void*)WeaponItemID);
}

void CStepInventory::EnterInventory(void)
{
	_ResetVestige();
}

void CStepInventory::LeaveInventory(void)
{
	INT32 uChangedID = 0;

	if( m_bCharacterChanged )
	{
		uChangedID |= 0x1;
		m_bCharacterChanged = FALSE;
	}

	if( m_bWeaponChanged )
	{
		uChangedID |= 0x2;
		m_bWeaponChanged = FALSE;
	}

	g_pGameContext->SetEventOnce(EVENT_LEAVE_INVENTORY, &uChangedID);
}

void CStepInventory::AuthStartSuccess(INT32 arg)
{
	//	인증후 장비
	INVEN_ITEM_TYPE	InvenItemType = g_pFramework->GetInvenList()->GetInvenItemType(m_authID);
	switch(InvenItemType)
	{
	//	무기는 인증후 장비
	case INVEN_ITEM_WEAPON:
		{
			if (EV_SUCCESSED(arg))
			{
				OnEquipWeapon();
			}
			else
			{
				STRING_POPUP(GAME_STRING("STR_INVEN_FAILED_AUTH"));/*인증에 실패하였습니다. 잠시 후 다시 시도해주세요.*/
			}
		}
		break;

	//	캐릭터는 인증후 장비
	case INVEN_ITEM_CHARA:
		{
			if (EV_SUCCESSED(arg))
			{
				OnEquipCharacter();
			}
			else
			{
				STRING_POPUP(GAME_STRING("STR_INVEN_FAILED_AUTH"));/*인증에 실패하였습니다. 잠시 후 다시 시도해주세요.*/
			}
		}
		break;

	//	아이템 인증후 결과 (아이템 사용 결과는 CPopupUseItem::OnGameEvent에서 따로 처리합니다.)
	case INVEN_ITEM_CASHITEM:
		{
			//USE_ITEM_STYLE style = USE_ITEM_STYLE_RESULT;

			//((CStageReady*) m_pParent)->TogglePopup(POPUP_USE_ITEM, &m_authItemID, &style);
		}
		break;

	default:			/* Do nothing */		break;
	}
}

void CStepInventory::CbAuthStart(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		CStepInventory* inven = (CStepInventory*)pThis;
		g_pGameContext->SetEvent(EVENT_ITEM_AUTH, &inven->m_authID);

		//inven->m_bWaitAuthStart = TRUE;

		i3GuiRoot::setGuiDisableInput(TRUE);
	}
}

void CStepInventory::OnDetailWeapon(void)
{
	if (INVALID_INDEX == m_idxSelectedWeaponSlot)
		return;	// 선택 무기 없으면 무시

	_SetupVestige();

	// 웨폰플레그를 익스텐션팝업에 전달한다
	INT32 WeaponID = g_pFramework->GetWeaponID(m_idxSelectedWeaponSlot, m_nWeaponUsageType);

	CPopupExtension::SetWeaponFlag(WeaponID);
	((CStageReady*)m_pParent)->TogglePopup(POPUP_EXTENSION);
}

void CStepInventory::OnEquipWeapon(void)
{
	// 선택되어 정보가 보여지는 무기가 없으면 처리하지 않는다
	if (INVALID_INDEX == m_idxSelectedWeaponSlot)
		return;

	// 무기 정보 불러오기
	INT32 nWeaponID = g_pFramework->GetWeaponID(m_idxSelectedWeaponSlot, m_nWeaponUsageType);

	UINT8	Type = 0;

	CShop * pShop = g_pGameContext->GetShop();
	AUTH_TYPE authType = pShop->GetAuthType(nWeaponID);

	if ( ! g_pFramework->GetWeaponTypeByID(nWeaponID, &Type, NULL) )
		return;
	if (Type == ITEM_ATTR_TYPE_BUY && AUTH_SEC == authType)
	{
		return;
	}


#if !defined( ENABLE_EQUIP_USER_TITLE_WEAPON)
	char szNeedUserTitle[1024] = "";

	//	호칭 무기인지 체크
	if( IsItemByDesignation( nWeaponID)) 
	{
		if( ! CheckItemByDesignation( nWeaponID, g_pGameContext->GetTitleInfo(), szNeedUserTitle))
		{
			char szMessage[1024] = "";
			sprintf_s( szMessage, GAME_STRING("STR_INVEN_REQUIRE_DESIGNATION"), szNeedUserTitle);/*해당 아이템은 %s 호칭을\n획득하여야 장비할 수 있습니다.\n(호칭 획득 후 장비 가능)*/
			STRING_POPUP( szMessage);

			m_bWeaponChanged = FALSE;
		}
		else
		{
			m_bWeaponChanged = TRUE;
		}
	}
	else
#endif
	{
		m_bWeaponChanged = TRUE;
	}

	if( m_bWeaponChanged)
	{
		//	장착 사운드
		if( g_pFramework->EquipWeaponToMyInven(nWeaponID, TRUE))
		{
			PlaySound(GUI_SND_ITEM_EQUIP);
			i3GuiRoot::setGuiDisableInput(FALSE);
		}
		GlobalFunc::PB_TRACE("OnEquipWeapon nWeaponID %d", nWeaponID);
		
	}
}

void CStepInventory::OnAuthWeapon(void)
{
	if (INVALID_INDEX == m_idxSelectedWeaponSlot)	return;

	INT32 WeaponID = g_pFramework->GetWeaponID(m_idxSelectedWeaponSlot, m_nWeaponUsageType);


	INT64 authIdx = g_pFramework->GetWeaponWareDBIdx(WeaponID);
	if (INVALID_WAREDB_INDEX == authIdx) return;

	m_authID = authIdx;

	STRING_POPUP_Q(GAME_STRING("STR_INVEN_STARTING_AUTH"), MAKE_CALLBACK(CbAuthStart), this);/*사용을 시작하지 않은 아이템입니다.\n장비 시부터 남은 기간만큼 사용이 가능합니다.\n장비하시겠습니까?*/
}

void CStepInventory::_InitEquipWin(void)
{
	INIT_WIN_CAPTION(m_pWeaponCaption, GAME_STRING("STR_INVEN_USED_EQUIP"));/*사용장비*/

	for(UINT32 i = WEU_START; i < WEU_MAX; i++)
	{
		m_weaponEquip[i]._pImage->SetEnable(TRUE);

		m_weaponEquip[i]._pKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponEquip[i]._pValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponEquip[i]._pKey->SetTextColor(GetColor(GCT_STATIC));
		m_weaponEquip[i]._pValue->SetTextColor(GetColor(GCT_STATIC));
	}

	m_weaponEquip[WEU_PRIMARY]._pKey->SetText(GAME_STRING("STR_INVEN_PRIMARY_WEAPON"));/*주무기*/
	m_weaponEquip[WEU_SECONDARY]._pKey->SetText(GAME_STRING("STR_INVEN_SECONDARY_WEAPON"));/*보조무기*/
	m_weaponEquip[WEU_MELEE]._pKey->SetText(GAME_STRING("STR_INVEN_MELEE_WEAPON"));/*근접무기*/
	m_weaponEquip[WEU_THROWING]._pKey->SetText(GAME_STRING("STR_INVEN_THROWING_WEAPON"));/*투척무기*/
	m_weaponEquip[WEU_ITEM]._pKey->SetText(GAME_STRING("STR_INVEN_SPECIAL_WEAPON"));/*특수무기*/
}

void CStepInventory::_InitInvenWin(void)
{
	_InitInvenCommon();
	_InitInvenWeaponTab();
	_InitInvenCharaTab();
	_InitInvenItemTab();
}

void CStepInventory::_InitInvenCommon(void)
{
	INIT_WIN_CAPTION(m_pInvenCaption, GAME_STRING("STR_INVEN_INVENTORY"));/*인벤토리*/

	INIT_TAB_EX(m_pWeaponTab);
	INIT_TAB_EX(m_pCharacterTab);
	INIT_TAB_EX(m_pItemTab);

	m_pWeaponTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	INIT_BUTTON_EX(m_pLeave);
}

void CStepInventory::_InitInvenWeaponTab(void)
{
	INIT_CAPTION_BUTTON(m_pAllWeapon, GAME_STRING("STR_INVEN_VIEW_ALL"));/*모두보기*/
	INIT_CAPTION_BUTTON(m_pPrimary, GAME_STRING("STR_INVEN_PRIMARY_WEAPON"));/*주무기*/
	INIT_CAPTION_BUTTON(m_pSecondary, GAME_STRING("STR_INVEN_SECONDARY_WEAPON"));/*보조무기*/
	INIT_CAPTION_BUTTON(m_pMelee, GAME_STRING("STR_INVEN_MELEE_WEAPON"));/*근접무기*/
	INIT_CAPTION_BUTTON(m_pThrowing, GAME_STRING("STR_INVEN_THROWING_WEAPON"));/*투척무기*/
	INIT_CAPTION_BUTTON(m_pItem, GAME_STRING("STR_INVEN_SPECIAL_WEAPON"));/*특수무기*/

	ACTIVATE_BUTTON(m_pAllWeapon);
	ACTIVATE_BUTTON(m_pPrimary);
	ACTIVATE_BUTTON(m_pSecondary);
	ACTIVATE_BUTTON(m_pMelee);
	ACTIVATE_BUTTON(m_pItem);

	m_pAllWeapon->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_pWeaponScrollList->CreateTextEx(4,  GetDefaultFontName());
	m_pWeaponScrollList->SetSelColor(0, 0, 0, 0);

	for(UINT32 i = 0; i < MAX_SLOT_FOR_INVEN_WEAPON; i++)
	{
		m_weaponSlot[i]._pBg->SetAllShapeEnable(FALSE);
		m_weaponSlot[i]._pBg->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
		m_weaponSlot[i]._pKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponSlot[i]._pKey->SetTextColor(GetColor(GCT_DEFAULT));
		m_weaponSlot[i]._pValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponSlot[i]._pValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_weaponSlot[i]._pValue->SetTextSpace(0, -3);
		m_weaponSlot[i]._pImage->SetEnable(FALSE);
		m_weaponSlot[i]._pExtension->SetEnable(FALSE);
		m_weaponSlot[i]._pUsed->SetEnable(FALSE);
		m_weaponSlot[i]._pTerm->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponSlot[i]._pTerm->SetTextColor(GetColor(GCT_DEFAULT));
		m_weaponSlot[i]._pUsed->SetAllShapeEnable(FALSE);
		m_weaponSlot[i]._pSaleMark->SetAllShapeEnable(FALSE);

		I3COLOR	* pGaugeColor = (I3COLOR*)&g_pConfigEx->GetGUI().DurabilityBarColorInSlot;
		m_weaponSlot[i]._pDurabilityGauge->SetColor(I3GUI_CONTROL_STATE_NORMAL, pGaugeColor);
	}

	m_pWeaponImage->SetEnable(FALSE);
	m_pWeaponInfoKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pWeaponInfoValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pWeaponInfoTerm->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pWeaponInfoKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pWeaponInfoValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pWeaponInfoTerm->SetTextColor(GetColor(GCT_DEFAULT));

	INIT_CAPTION_BUTTON(m_pDetailWeapon, GAME_STRING("STR_INVEN_DETAIL_INFO"));	//상세정보
	INIT_CAPTION_BUTTON(m_pEquipWeapon, GAME_STRING("STR_INVEN_DO_EQUIP"))
	INIT_CAPTION_BUTTON(m_pAuthWeapon, GAME_STRING("STR_INVEN_DO_EQUIP"));	//장비하기
	INIT_CAPTION_BUTTON(m_pRepairWeapon, GAME_STRING("STR_INVEN_DO_REPAIR"));	//수리

	ACTIVATE_BUTTON(m_pDetailWeapon);
	DEACTIVATE_BUTTON(m_pEquipWeapon);
	DEACTIVATE_BUTTON(m_pAuthWeapon);
	DEACTIVATE_BUTTON(m_pRepairWeapon);

	for(UINT32 i = 0; i < MAX_WEAPON_INFO; i++)
	{
#if defined (LOCALE_THAILAND) || defined(GUI_ADJUST_RUSSIA)
		m_weaponInfo[i]._pKey->ReCreateTextEx( GetDefaultFontName() , 32, GAME_FONT_SIZE_ANY_TEXT, FALSE, 0, 0, TRUE, FONT_SHADOW_QUALITY_LOW);
#endif

		m_weaponInfo[i]._pKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponInfo[i]._pValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_weaponInfo[i]._pKey->SetTextColor(GetColor(GCT_DEFAULT));
		m_weaponInfo[i]._pValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_weaponInfo[i]._pGage->SetEnable(FALSE);
	}

#if defined (LOCALE_THAILAND) || defined(GUI_ADJUST_RUSSIA)
	m_pBulletKey->ReCreateTextEx( GetDefaultFontName() , 32, GAME_FONT_SIZE_ANY_TEXT, FALSE, 0, 0, TRUE, FONT_SHADOW_QUALITY_LOW);
	m_pRangeKey->ReCreateTextEx( GetDefaultFontName() , 32, GAME_FONT_SIZE_ANY_TEXT, FALSE, 0, 0, TRUE, FONT_SHADOW_QUALITY_LOW);
#endif


	m_pBulletKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pBulletValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pBulletKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pBulletValue->SetTextColor(GetColor(GCT_DEFAULT));

	m_pRangeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRangeValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRangeKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pRangeValue->SetTextColor(GetColor(GCT_DEFAULT));

	m_weaponInfo[WEAPON_INFO_DAMAGE]._pKey->SetText(GAME_STRING("STR_INVEN_DAMAGE"));						//데미지
	m_weaponInfo[WEAPON_INFO_RAPID]._pKey->SetText(GAME_STRING("STR_INVEN_RAPID_RATE"));					//연사력
	m_weaponInfo[WEAPON_INFO_HIT_RATE]._pKey->SetText(GAME_STRING("STR_INVEN_HIT_RATE"));					//명중률
	m_weaponInfo[WEAPON_INFO_VERT_RECOIL]._pKey->SetText(GAME_STRING("STR_INVEN_WEAPON_VERT_RECOIL"));		//수직반동
	m_weaponInfo[WEAPON_INFO_HORZ_RECOIL]._pKey->SetText(GAME_STRING("STR_INVEN_WEAPON_HORZ_RECOIL"));		//수평반동
	m_weaponInfo[WEAPON_INFO_SPEED]._pKey->SetText(GAME_STRING("STR_INVEN_WEAPON_SPEED"));					//이동속도

	m_pBulletKey->SetText(GAME_STRING("STR_INVEN_BULLETS"));				//장탄수
	m_pRangeKey->SetText(GAME_STRING("STR_INVEN_WEAPON_RANGE"));			//사정거리
}

void CStepInventory::_WeaponListUpdate(void)
{
	char szWeaponName[ MAX_STRING_COUNT];
	char szString[MAX_STRING_COUNT] = "";

	// 목록을 모두 지운다
	m_pWeaponScrollList->Clear();

	for(UINT32 i = 0; i < MAX_SLOT_FOR_INVEN_WEAPON; i++)
	{
		m_weaponSlot[i]._pBg->SetShapeEnable(GUI_SHAPE_OFFSET + 2, FALSE);
		m_weaponSlot[i]._pBg->setInputDisable(TRUE);
		m_weaponSlot[i]._pBg->SetEnable(FALSE);

		m_weaponSlot[i]._pKey->SetEnable(FALSE);
		m_weaponSlot[i]._pValue->SetEnable(FALSE);
		m_weaponSlot[i]._pImage->SetEnable(FALSE);
		m_weaponSlot[i]._pExtension->SetEnable(FALSE);
		m_weaponSlot[i]._pUsed->SetEnable(FALSE);
		m_weaponSlot[i]._pTerm->SetEnable(FALSE);
		m_weaponSlot[i]._pSaleMark->SetEnable(FALSE);
		m_weaponSlot[i]._pUsed->SetAllShapeEnable(FALSE);
		m_weaponSlot[i]._pDurabilityBar->SetEnable(FALSE);
	}

	// 소유 무기를 불러온다
	INT32 nCount = g_pFramework->GetWeaponCount(m_nWeaponUsageType);
	INT32 TotalSlotCount = (nCount + 1) / 2;
	for(INT32 i = 0; i < TotalSlotCount ; i ++)
	{
		m_pWeaponScrollList->AddElement("");
	}

	for(INT32 i = 0; i < MAX_SLOT_FOR_INVEN_WEAPON; i++)
	{
		INT32 CurrentSlotIndex = i + m_idxWeaponStartSlot;
		// 슬롯이 비어있으면 끝
		if (CurrentSlotIndex >= nCount)
			break;

		// 슬록 활성화
		m_weaponSlot[i]._pBg->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
		m_weaponSlot[i]._pBg->setInputDisable(FALSE);
		m_weaponSlot[i]._pBg->SetEnable(TRUE);

		if (m_idxSelectedWeaponSlot == CurrentSlotIndex)
		{
			m_weaponSlot[i]._pBg->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
		}

		// 무기가 있는 슬롯이면 표시
		INT32 ItemID = g_pFramework->GetWeaponID(CurrentSlotIndex, m_nWeaponUsageType);
		WEAPON_CLASS_TYPE weaponClassType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(ItemID);
		INT32 weaponNum = GET_ITEM_FLAG_NUMBER(ItemID);

		// KEYWORD : WEAPON_MINIAXE	일반 나이프에 다른 무기를 들고 있는 경우입니다. 해당 무기가 표시되도록 세팅해줘야 합니다.
		GUI_WEAPON_VALID( weaponClassType, weaponNum);

		if(g_pFramework->SetWeaponSelectMark(m_weaponSlot[i]._pImage, (UINT8) weaponClassType, (UINT8) weaponNum) )
		{
			m_weaponSlot[i]._pImage->SetEnable(TRUE);
		}

		UINT8 itemAttr;
		UINT32 itemArg;

		CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( weaponClassType, weaponNum);

		if (g_pFramework->GetWeaponType(CurrentSlotIndex, m_nWeaponUsageType, &itemAttr, &itemArg))
		{
			UINT32 ItemID = g_pFramework->GetWeaponID(CurrentSlotIndex, m_nWeaponUsageType);
			CShop * pShop = g_pGameContext->GetShop();
			AUTH_TYPE authType = pShop->GetAuthType(ItemID);

			//
			// 슬롯 우하단에 장비중, 미사용, 사용중을 표시한다.
			//
			// 현재 장착중인 아이템인지..
			USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
			g_pGameContext->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);
			if (g_pWeaponInfoDataBase->IsUsedWeapon( &InfoEquipWeapon, ItemID))
			{
				m_weaponSlot[i]._pUsed->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
				m_weaponSlot[i]._pUsed->SetEnable(TRUE);
			}
			//else if ( ITEM_ATTR_TYPE_BASIC != itemAttr )
			else if ( (ITEM_ATTR_TYPE_BUY == itemAttr) || (ITEM_ATTR_TYPE_AUTH == itemAttr ))
			{
				//	구매완료 (구입만 하고 인증하지 않은 상태)
				// 기간제 일경우에만 미사용 이미지를 표시한다.
				// 개수제는 아무런 표시를 하지 않는다.
				if (ITEM_ATTR_TYPE_BUY == itemAttr && AUTH_SEC == authType)
				{
					m_weaponSlot[i]._pUsed->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
					m_weaponSlot[i]._pUsed->SetEnable(TRUE);
				}
			}

			// 상품 판매 마크
			UINT8 nSaleMark = GetItemSaleMarkIndex(itemAttr);
			SetItemSaleMark( m_weaponSlot[i]._pSaleMark, nSaleMark);

#if defined(USE_REPAIR_WEAPON)
			// 수리 시스템이 들어가면 개수 정보를 보이지 않습니다.
			// 기간정보만 표시합니다.
			if ( authType == AUTH_SEC )
#else
			if ( authType != AUTH_UNKNWON )
#endif
			{
				//
				// 슬롯 좌하단에 보유개수 또는 남은기간을 표시한다.
				//
				BOOL Expendable = (authType == AUTH_COUNT) ? TRUE : FALSE;
				char termText[MAX_STRING_COUNT] = {};
				_GetInvenUseDay(Expendable, itemAttr, itemArg, termText, MAX_STRING_COUNT);

				m_weaponSlot[i]._pTerm->SetText(termText);
				m_weaponSlot[i]._pTerm->SetEnable(TRUE);
			}

#if defined (USE_REPAIR_WEAPON)
			CShopContext * pShopContext = g_pFramework->GetShopContext();
			BOOL	DurabilityWeapon = pShopContext->IsDurabilityItem(ItemID);
			if ( DurabilityWeapon )
			{
				m_weaponSlot[i]._pDurabilityBar->SetEnable(TRUE);

				UINT32	Current = 0, Max = 0;
				if ( pShopContext->GetDurability(ItemID, Current, Max) )
				{
					I3ASSERT_ONCE( Max != 0 );
					if ( Max > 0 )
					{
						REAL32	fRate	= (static_cast<REAL32>(Current) / static_cast<REAL32>(Max)) * 100.0f;
						UINT32	nRate	= static_cast<UINT32>(MINMAX( 0, static_cast<INT32>(fRate), 100));			// 캐스팅수정(11.11.07.수빈)
						_ShowGaugeBarStatic(m_weaponSlot[i]._pDurabilityGauge, nRate);
					}
				}
			}
#endif
		}

		if(pInfo)
		{
			// 무기 이름
			pInfo->GetWeaponInfoName( szWeaponName);

			m_weaponSlot[i]._pValue->SetText( szWeaponName);
			m_weaponSlot[i]._pValue->SetEnable(TRUE);

			//	Endurance zero weapon name is red color
			if( GetEnduranceZero(itemAttr, itemArg) )
			{
				m_weaponSlot[i]._pValue->SetTextColor( GetColor(GCT_RED));
			}
			else
			{
				m_weaponSlot[i]._pValue->SetTextColor( GetColor(GCT_DEFAULT));
			}

			// 카테고리
			g_pWeaponInfoDataBase->GetWeaponCategoryEx(pInfo, szString, MAX_STRING_COUNT);
			m_weaponSlot[i]._pKey->SetText(szString);
			m_weaponSlot[i]._pKey->SetEnable(TRUE);
		}
		else
		{
			i3String::Format(szWeaponName, MAX_STRING_COUNT, _T("%s:%d"), UNKNOWN_TEXT, ItemID);
			m_weaponSlot[i]._pValue->SetText(UNKNOWN_TEXT);
			m_weaponSlot[i]._pValue->SetEnable(TRUE);
			m_weaponSlot[i]._pKey->SetText(UNKNOWN_TEXT);
			m_weaponSlot[i]._pKey->SetEnable(TRUE);
		}
	}
}

void CStepInventory::_GetInvenUseDay(BOOL IsCountItem, UINT8 Type, UINT32 Arg, char * pOut, const size_t sizeOutMax)
{
	I3ASSERT( pOut != NULL);

	switch(Type)
	{
	case ITEM_ATTR_TYPE_BUY: // 구매 완료
		{
			if ( IsCountItem != TRUE )
			{
				UINT32 term = ConvertSecToUseDay(Arg);

				if( term <= 1)
				{	// 1일 미만이면 시간으로 표시
					UINT32 hour = ConvertSecToUseHour(Arg);
					//	남은기간 : %d시간
					sprintf_s(pOut, sizeOutMax, "%s : {col:255,255,255,255}%d%s{/col}", GAME_STRING("STBL_IDX_ITEM_REMAIN_TERM"), hour, GAME_STRING("STR_UI_HOUR"));
				}
				else
				{
					//	남은기간 : %d일
					sprintf_s(pOut, sizeOutMax, "%s : {col:255,255,255,255}%d%s{/col}", GAME_STRING("STBL_IDX_ITEM_REMAIN_TERM"), term, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));
				}
			}
			else
			{
				sprintf_s(pOut, sizeOutMax, "%s : {col:255,255,255,255}%d%s{/col}", GAME_STRING("STBL_IDX_ITEM_REMAIN_COUNT"), Arg, GAME_STRING("STBL_IDX_ITEM_COUNT_UNIT"));
			}
		}
		break;
	case ITEM_ATTR_TYPE_AUTH: // 사용중		/* Fall through */
		{
			// 기간제라면.

			UINT32 sec = 0;
			UINT32 term = 0;

			if( Arg > 0)
			{
				sec = g_pGameContext->GetInventorySecTime(Arg);

				if( 0 > (INT32)sec ) sec = 0;

				//	김성백 실장님 부탁으로 코드 추가합니다. 기간 보정 - komet
				if( sec >= ITEM_TERM_ADJUST_VALUE)
					sec -= ITEM_TERM_ADJUST_VALUE;

				term = ConvertSecToUseDay(sec);
			}

			if (1 < term)
			{
				//	남은기간 : %d일
				sprintf_s(pOut, sizeOutMax, "%s : {col:255,255,255,255}%d%s{/col}", GAME_STRING("STBL_IDX_ITEM_REMAIN_TERM"), term, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));
			}
			else if( term == 0)
			{
				// 시간이 다된 경우
				//	남은기간 : 0시간
				sprintf_s(pOut, sizeOutMax, "%s : {col:210,0,0,255}0%s{/col}", GAME_STRING("STBL_IDX_ITEM_REMAIN_TERM"), GAME_STRING("STR_UI_HOUR"));
			}
			else if( sec > 0)
			{
				UINT32 hour = ConvertSecToUseHour(sec);
				if( hour <= 1)
				{
					// 남은기간 : 1시간 미만
					sprintf_s(pOut, sizeOutMax, "%s : {col:210,0,0,255}1%s %s{/col}", GAME_STRING("STBL_IDX_ITEM_REMAIN_TERM"),
						GAME_STRING("STR_UI_HOUR"), GAME_STRING("STBL_IDX_ITEM_LESS_THEN"));
				}
				else
				{
					// 남은기간 : %d시간
					sprintf_s(pOut, sizeOutMax, "%s : {col:210,0,0,255}%d%s{/col}", GAME_STRING("STBL_IDX_ITEM_REMAIN_TERM"),
						hour, GAME_STRING("STR_UI_HOUR") );
				}

				////	남은기간 : 1일 미만
				//sprintf_s(pOut, sizeOutMax, "%s : {col:210,0,0,255}1%s %s{/col}", GAME_STRING("STBL_IDX_ITEM_REMAIN_TERM"),
				//	GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"), GAME_STRING("STBL_IDX_ITEM_LESS_THEN"));
			}
		}
		break;

	default:
		{
#if defined (ENABLE_INVEN_DISPLAY_OBT)
			//	남은기간 : OBT 한정
			sprintf_s(pOut, sizeOutMax, "%s : {col:255,255,255,255}%s{/col}", GAME_STRING("STBL_IDX_ITEM_REMAIN_TERM"), GAME_STRING("STBL_IDX_ITEM_UNLIMITED"));
#else
			i3String::Copy(pOut, "", sizeOutMax);
#endif
		}
		/* Do nothing */
		break;
	}
}


void CStepInventory::_SetAuthButton(UINT32 ItemID, UINT8 ItemAttr, i3GuiButton * pEquipButton, i3GuiButton * pAuthButton)
{
	I3ASSERT(pEquipButton != NULL && pAuthButton != NULL);
	if (pEquipButton == NULL || pAuthButton == NULL)
		return;

	// 이벤트 아이템일 경우에는 ItemID를 Shop에서 얻을 수 없기 때문에
	// 바로 장비버튼을 활성화 처리합니다.
	if ( ITEM_ATTR_TYPE_AUTH < ItemAttr )
	{
		pEquipButton->SetEnable(TRUE);
		ACTIVATE_BUTTON(pEquipButton);
		return;
	}

	// 개수제 무기일 경우에 예외처리를 합니다.
	// 개수제 무기는 인증하기를 하지 않고 바로 장착합니다.

	AUTH_TYPE AuthType = g_pGameContext->GetShop()->GetAuthType(ItemID);
	I3ASSERT(AuthType == AUTH_SEC || AuthType == AUTH_COUNT);
	if ( ITEM_ATTR_TYPE_BUY == ItemAttr && AuthType == AUTH_COUNT )
	{
		pEquipButton->SetEnable(TRUE);
		ACTIVATE_BUTTON(pEquipButton);
	}
	else if (ITEM_ATTR_TYPE_BUY == ItemAttr )
	{
		pAuthButton->SetEnable(TRUE);
		ACTIVATE_BUTTON(pAuthButton);
	}
	else
	{
		pEquipButton->SetEnable(TRUE);
		ACTIVATE_BUTTON(pEquipButton);
	}
}

void CStepInventory::_WeaponDescriptionUpdate(void)
{
	char szWeaponName[ MAX_STRING_COUNT];
	char szString[MAX_STRING_COUNT] = "";
	UINT8 itemAttr = 0;
	UINT32 itemArg = 0;

	// 인포 초기화
	m_pWeaponInfo->SetEnable(TRUE);
	m_pWeaponImage->SetEnable(FALSE);
	m_pWeaponInfoKey->SetEnable(FALSE);
	m_pWeaponInfoValue->SetEnable(FALSE);
	m_pWeaponInfoTerm->SetEnable(FALSE);
	m_pWeaponInfoExtension->SetEnable(FALSE);

	m_weaponInfo[WEAPON_INFO_DAMAGE]._pGage->SetEnable(FALSE);
	m_weaponInfo[WEAPON_INFO_DAMAGE]._pValue->SetText("-");
	m_weaponInfo[WEAPON_INFO_RAPID]._pGage->SetEnable(FALSE);
	m_weaponInfo[WEAPON_INFO_RAPID]._pValue->SetText("-");
	m_weaponInfo[WEAPON_INFO_HIT_RATE]._pGage->SetEnable(FALSE);
	m_weaponInfo[WEAPON_INFO_HIT_RATE]._pValue->SetText("-");
	m_weaponInfo[WEAPON_INFO_VERT_RECOIL]._pGage->SetEnable(FALSE);
	m_weaponInfo[WEAPON_INFO_VERT_RECOIL]._pValue->SetText("-");
	m_weaponInfo[WEAPON_INFO_HORZ_RECOIL]._pGage->SetEnable(FALSE);
	m_weaponInfo[WEAPON_INFO_HORZ_RECOIL]._pValue->SetText("-");
	m_weaponInfo[WEAPON_INFO_SPEED]._pGage->SetEnable(FALSE);
	m_weaponInfo[WEAPON_INFO_SPEED]._pValue->SetText("-");
	m_pBulletValue->SetEnable(FALSE);
	m_pRangeValue->SetEnable(FALSE);

	m_pDetailWeapon->SetEnable(FALSE);
	m_pAuthWeapon->SetEnable(FALSE);
	m_pEquipWeapon->SetEnable(FALSE);
	m_pRepairWeapon->SetEnable(FALSE);

	// 장비된 주무기를 찾지 못하면 처리하지 않음
	if (INVALID_INDEX == m_idxSelectedWeaponSlot)
	{
		return;
	}

	if (FALSE == g_pFramework->GetWeaponType(m_idxSelectedWeaponSlot, m_nWeaponUsageType, &itemAttr, &itemArg))
	{
		return;
	}

	INT32 ItemID = g_pFramework->GetWeaponID(m_idxSelectedWeaponSlot, m_nWeaponUsageType);

	//	기간이 만료된지 체크합니다.
	BOOL bBroken = GetEnduranceZero(itemAttr, itemArg);

	//	장비 버튼 활성화/비활성화
	{
		USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
		g_pGameContext->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);
		INT32 nDescriptionWeaponID = g_pFramework->GetWeaponID(m_idxSelectedWeaponSlot, m_nWeaponUsageType);

		//	이미 장비한 무기
		if( InfoEquipWeapon._noprim == (UINT32) nDescriptionWeaponID ||
			InfoEquipWeapon._nosub == (UINT32) nDescriptionWeaponID ||
			InfoEquipWeapon._nomelee == (UINT32) nDescriptionWeaponID ||
			InfoEquipWeapon._nothrow == (UINT32) nDescriptionWeaponID ||
			InfoEquipWeapon._noitem == (UINT32) nDescriptionWeaponID)
		{
			m_pEquipWeapon->SetEnable(TRUE);

			DEACTIVATE_BUTTON(m_pEquipWeapon);
		}
		else
		{
			_SetAuthButton(nDescriptionWeaponID, itemAttr, m_pEquipWeapon, m_pAuthWeapon);
		}
	}

	WEAPON_CLASS_TYPE weaponClassType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(ItemID);
	INT32 weaponNum = GET_ITEM_FLAG_NUMBER(ItemID);

	// KEYWORD : WEAPON_MINIAXE	일반 나이프에 다른 무기를 들고 있는 경우입니다. 해당 무기가 표시되도록 세팅해줘야 합니다.
	GUI_WEAPON_VALID( weaponClassType, weaponNum);


#if ! defined (USE_REPAIR_WEAPON) || defined (I3_DEBUG)
	CShop * pShop = g_pGameContext->GetShop();
	AUTH_TYPE authType = pShop->GetAuthType(ItemID);
	if ( authType != AUTH_UNKNWON )
	{
		char termText[MAX_STRING_COUNT] = {0, };
		BOOL Expendable = (authType == AUTH_COUNT) ? TRUE : FALSE;
		_GetInvenUseDay(Expendable, itemAttr, itemArg, termText, MAX_STRING_COUNT);

		m_pWeaponInfoTerm->SetText(termText);
		m_pWeaponInfoTerm->SetEnable(TRUE);
	}
#endif

	if(g_pFramework->SetWeaponSelectMark(m_pWeaponImage, (UINT8) weaponClassType, (UINT8) weaponNum) )
	{
		m_pWeaponImage->SetEnable(TRUE);
	}

	// 무기 이미지
	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( weaponClassType, weaponNum);


	// 무기 이름
	if ( pInfo )
		pInfo->GetWeaponInfoName( szWeaponName);
	else
		i3String::Format( szWeaponName, MAX_STRING_COUNT, _T("invalid weapon: %d"), ItemID);

	m_pWeaponInfoValue->SetText( szWeaponName);
	m_pWeaponInfoValue->SetEnable(TRUE);

	if ( ! pInfo )
		return;

	//	Endurance zero weapon name is red color
	if( GetEnduranceZero(itemAttr, itemArg) )
	{
		m_pWeaponInfoValue->SetTextColor( GetColor(GCT_RED));
	}
	else
	{
		m_pWeaponInfoValue->SetTextColor( GetColor(GCT_DEFAULT));
	}

	// 카테고리
#if	!defined( LOCALE_NORTHAMERICA )
	g_pWeaponInfoDataBase->GetWeaponCategoryEx(pInfo, szString, MAX_STRING_COUNT);

	m_pWeaponInfoKey->SetText(szString);
	m_pWeaponInfoKey->SetEnable(TRUE);
#endif
	// 익스텐션
//	if (g_pWeaponInfoDataBase->IsExtensionWeapon(pInfo))
//	{
//		m_pWeaponInfoExtension->SetEnable(TRUE);
//	}

	// 상세정보
	if (g_pWeaponInfoDataBase->BeDescriptionWeapon(pInfo))
	{
		m_pDetailWeapon->SetEnable(TRUE);
	}

	// 데미지
	char strBuf[MAX_STRING_COUNT];
	INT32 nVal = 0;
	INT32 nGage = 0;

	nVal = pInfo->GetPercentage(WEAPON_INFO_DAMAGE);

#if defined( USE_WEAPON_ENDURANCE)
	if( bBroken)	//	망가진 무기
	{
		nVal = CGameWeaponManager::WeaponEnduranceZeroDamage(nVal);
	}
#endif

	if( 0 <= nVal )
	{
		nGage = MINMAX( 0, nVal, 100);
		sprintf_s(strBuf, "%d", nVal);
		_ShowWeaponGage(WEAPON_INFO_DAMAGE, nGage, bBroken);
		m_weaponInfo[WEAPON_INFO_DAMAGE]._pValue->SetText(strBuf);
	}
	else
	{
		m_weaponInfo[WEAPON_INFO_DAMAGE]._pValue->SetText("-");
	}

	// 연사력
	nVal = pInfo->GetPercentage(WEAPON_INFO_RAPID);

	if( 0 <= nVal)
	{
		nGage = MINMAX( 0, nVal, 100);
		sprintf_s(strBuf, "%d", nVal);
		_ShowWeaponGage(WEAPON_INFO_RAPID, nGage);
		m_weaponInfo[WEAPON_INFO_RAPID]._pValue->SetText(strBuf);
	}
	else
	{
		m_weaponInfo[WEAPON_INFO_RAPID]._pValue->SetText("-");
	}

	// 명중률

	nVal = pInfo->GetPercentage(WEAPON_INFO_HIT_RATE);

	if( 0 <= nVal)
	{
		nGage = MINMAX( 0, nVal, 100);
		sprintf_s(strBuf, "%d", nVal);
		_ShowWeaponGage(WEAPON_INFO_HIT_RATE, nGage);
		m_weaponInfo[WEAPON_INFO_HIT_RATE]._pValue->SetText(strBuf);
	}
	else
	{
		m_weaponInfo[WEAPON_INFO_HIT_RATE]._pValue->SetText("-");
	}

	// 수직반동
	nVal = pInfo->GetPercentage(WEAPON_INFO_VERT_RECOIL);

#if defined( USE_WEAPON_ENDURANCE)
	if( bBroken)	//	망가진 무기
	{
		nVal = CGameWeaponManager::WeaponEnduranceZeroVertRecoil(nVal);
	}
#endif

	if( 0 <= nVal )
	{
		nGage = MINMAX( 0, nVal, 100);
		sprintf_s(strBuf, "%d", nVal);
		_ShowWeaponGage(WEAPON_INFO_VERT_RECOIL, nGage, bBroken);
		m_weaponInfo[WEAPON_INFO_VERT_RECOIL]._pValue->SetText(strBuf);
	}
	else
	{
		m_weaponInfo[WEAPON_INFO_VERT_RECOIL]._pValue->SetText("-");
	}

	// 수평반동
	nVal = pInfo->GetPercentage(WEAPON_INFO_HORZ_RECOIL);

#if defined( USE_WEAPON_ENDURANCE)
	if( bBroken)	//	망가진 무기
	{
		nVal = CGameWeaponManager::WeaponEnduranceZeroHorzRecoil(nVal);
	}
#endif

	if( 0 <= nVal )
	{
		nGage = MINMAX( 0, nVal, 100);
		sprintf_s(strBuf, "%d", nVal);
		_ShowWeaponGage(WEAPON_INFO_HORZ_RECOIL, nGage, bBroken);
		m_weaponInfo[WEAPON_INFO_HORZ_RECOIL]._pValue->SetText(strBuf);
	}
	else
	{
		m_weaponInfo[WEAPON_INFO_HORZ_RECOIL]._pValue->SetText("-");
	}

	// 이동속도
	nVal = pInfo->GetPercentage(WEAPON_INFO_SPEED);

	if( 0 <= nVal )
	{
		nGage = MINMAX( 0, nVal, 100);
		sprintf_s(strBuf, "%d", nVal);
		_ShowWeaponGage(WEAPON_INFO_SPEED, nGage);
		m_weaponInfo[WEAPON_INFO_SPEED]._pValue->SetText(strBuf);
	}
	else
	{
		m_weaponInfo[WEAPON_INFO_SPEED]._pValue->SetText("-");
	}

	// 장탄수
	{
		pInfo->MakeBulletCountString(strBuf, MAX_STRING_COUNT);
		m_pBulletValue->SetText(strBuf);
		m_pBulletValue->SetEnable(TRUE);
	}

	// 사정거리
	{
		pInfo->MakeWeaponRangeString(strBuf, MAX_STRING_COUNT);
		m_pRangeValue->SetText(strBuf);
		m_pRangeValue->SetEnable(TRUE);
	}

	// 내구도 표시
#if defined (USE_REPAIR_WEAPON)
	{
		CShopContext * pShopContext = g_pFramework->GetShopContext();
		UINT32 Current, Max;
		UINT32	Gage	= 0;
		BOOL	Broken	= FALSE;


		BOOL	DurabilityWeapon = pShopContext->IsDurabilityItem(ItemID);
		if ( DurabilityWeapon && pShopContext->GetDurability(ItemID, Current, Max) )
		{
			I3ASSERT( Max != 0 );
			if ( Max > 0 )
			{
				REAL32	Rate	= (static_cast<REAL32>(Current) / static_cast<REAL32>(Max)) * 100.0f;
				Gage			= MINMAX( 0, static_cast<INT32>(Rate), 100);		// UINT32 --> INT32 로 변경. (11.11.07.수빈)
			}
			Broken = (Current != 0) ? FALSE : TRUE;

			if ( DurabilityWeapon == TRUE && Current < Max )
			{
				ACTIVATE_BUTTON(m_pRepairWeapon);
			}
			else
			{
				DEACTIVATE_BUTTON(m_pRepairWeapon);
			}

		}
	}
#endif
}

void CStepInventory::_ChangeWeaponUsage(INT32 nWeaponUsage)
{
	if( m_nWeaponUsageType == nWeaponUsage) return;

	i3GuiButton* pButton;

	switch( m_nWeaponUsageType )
	{
	case WEAPON_SLOT_UNKNOWN:			pButton = m_pAllWeapon;		break;
	case WEAPON_SLOT_PRIMARY:		pButton = m_pPrimary;		break;
	case WEAPON_SLOT_SECONDARY:	pButton = m_pSecondary;		break;
	case WEAPON_SLOT_MELEE:		pButton = m_pMelee;			break;
	case WEAPON_SLOT_THROWING1:		pButton = m_pThrowing;		break;
	case WEAPON_SLOT_THROWING2:			pButton = m_pItem;			break;
	default:
		return;
	}

	pButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_nWeaponUsageType = nWeaponUsage;
	m_OldWeaponUsageType = (WEAPON_SLOT_TYPE)m_nWeaponUsageType;

	m_idxSelectedWeaponSlot = INVALID_INDEX;
	m_idxWeaponStartSlot = 0;
	m_pWeaponScrollList->SetCurValue(0.0f);
}

void CStepInventory::_WeaponWinUpdate(void)
{
	WEAPON_CLASS_TYPE type;
	INT32 number;
	char szWeaponName[ MAX_STRING_COUNT];
	CWeaponInfo* pWeaponInfo = NULL;

	// 초도보급 전 처리 금지. 닉네임 유무로 확인한다
	if ( ! g_pGameContext->HaveNickName() )
	{
		return;
	}


	// Red/Blue 캐릭터가 없으면 처리하지 않는다
	if (FALSE == g_pGameContext->IsMyEquipChara())
	{
		return;
	}

	// 장비품을 불러온다
	USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
	g_pGameContext->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

	// 무기 장비창이 활성화되있다면 갱신
	if (m_pWeaponWin->isEnable())
	{
		g_pFramework->SetWeaponSelectMark(m_weaponEquip[WEU_PRIMARY]._pImage,	(UINT8)GET_ITEM_FLAG_CLASS(InfoEquipWeapon._noprim),	(UINT8)GET_ITEM_FLAG_NUMBER(InfoEquipWeapon._noprim));
		g_pFramework->SetWeaponSelectMark(m_weaponEquip[WEU_SECONDARY]._pImage, (UINT8)GET_ITEM_FLAG_CLASS(InfoEquipWeapon._nosub),	(UINT8)GET_ITEM_FLAG_NUMBER(InfoEquipWeapon._nosub));

		type = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(InfoEquipWeapon._nomelee);
		number = GET_ITEM_FLAG_NUMBER(InfoEquipWeapon._nomelee);

		// KEYWORD : WEAPON_MINIAXE	일반 나이프에 다른 무기를 들고 있는 경우입니다. 해당 무기가 표시되도록 세팅해줘야 합니다.
		GUI_WEAPON_VALID( type, number);

		g_pFramework->SetWeaponSelectMark(m_weaponEquip[WEU_MELEE]._pImage,		(UINT8) type,	(UINT8)number);
		g_pFramework->SetWeaponSelectMark(m_weaponEquip[WEU_THROWING]._pImage,	(UINT8)GET_ITEM_FLAG_CLASS(InfoEquipWeapon._nothrow),	(UINT8)GET_ITEM_FLAG_NUMBER(InfoEquipWeapon._nothrow));
		g_pFramework->SetWeaponSelectMark(m_weaponEquip[WEU_ITEM]._pImage,		(UINT8)GET_ITEM_FLAG_CLASS(InfoEquipWeapon._noitem),	(UINT8)GET_ITEM_FLAG_NUMBER(InfoEquipWeapon._noitem));

		// 무기 이름
		// 주무기
		type = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(InfoEquipWeapon._noprim);
		number = GET_ITEM_FLAG_NUMBER(InfoEquipWeapon._noprim);
		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(type, number);
		if (pWeaponInfo)
		{
			pWeaponInfo->GetWeaponInfoName( szWeaponName);
			m_weaponEquip[WEU_PRIMARY]._pValue->SetText( szWeaponName);
		}

		// 보조무기
		type = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(InfoEquipWeapon._nosub);
		number = GET_ITEM_FLAG_NUMBER(InfoEquipWeapon._nosub);
		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(type, number);
		if (pWeaponInfo)
		{
			pWeaponInfo->GetWeaponInfoName( szWeaponName);
			m_weaponEquip[WEU_SECONDARY]._pValue->SetText( szWeaponName);
		}

		// 근접무기
		type = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(InfoEquipWeapon._nomelee);
		number = GET_ITEM_FLAG_NUMBER(InfoEquipWeapon._nomelee);

		// KEYWORD : WEAPON_MINIAXE	일반 나이프에 다른 무기를 들고 있는 경우입니다. 해당 무기가 표시되도록 세팅해줘야 합니다.
		GUI_WEAPON_VALID( type, number);

		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(type, number);
		if (pWeaponInfo)
		{
			pWeaponInfo->GetWeaponInfoName( szWeaponName);
			m_weaponEquip[WEU_MELEE]._pValue->SetText( szWeaponName);
		}

		// 투척무기
		type = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(InfoEquipWeapon._nothrow);
		number = GET_ITEM_FLAG_NUMBER(InfoEquipWeapon._nothrow);
		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(type, number);
		if (pWeaponInfo)
		{
			pWeaponInfo->GetWeaponInfoName( szWeaponName);
			m_weaponEquip[WEU_THROWING]._pValue->SetText( szWeaponName);
		}

		// 아이템
		type = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(InfoEquipWeapon._noitem);
		number = GET_ITEM_FLAG_NUMBER(InfoEquipWeapon._noitem);
		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(type, number);
		if (pWeaponInfo)
		{
			pWeaponInfo->GetWeaponInfoName( szWeaponName);
			m_weaponEquip[WEU_ITEM]._pValue->SetText( szWeaponName);
		}
	}
}

BOOL CStepInventory::_IsDefaultSupplyWeapon(INT32 nClassType,INT32 nWeaponNumber)
{
	BOOL bDefaultSupply = FALSE;

	switch(nClassType)
	{
	case WEAPON_CLASS_KNIFE:
		if (nWeaponNumber)
		{
		}
		break;
	case WEAPON_CLASS_HANDGUN:
		if (nWeaponNumber)
		{
		}
		break;
	case WEAPON_CLASS_ASSAULT:
		if (nWeaponNumber)
		{
		}
		break;
	case WEAPON_CLASS_THROWING1:
		if (nWeaponNumber)
		{
		}
		break;
	}

	return bDefaultSupply;
}

void CStepInventory::_ShowWeaponGage(UINT32 nInfoType,UINT32 nRate, BOOL bBroken /*=FALSE*/)
{
	if( nRate > 0)
	{
		i3GuiImage * pGageImage = m_weaponInfo[nInfoType]._pGage->GetControlData()->GetShape()->getImage(GUI_SHAPE_OFFSET);

		REAL32 fWidth = pGageImage->getOriginalWidth() * g_pGUIRoot->getGuiResolutionWidth();
		REAL32 fHeight = pGageImage->getOriginalHeight() * g_pGUIRoot->getGuiResolutionHeight();

		//	망가진 무기는 빨간색 게이지로 표시
		// EXPORT_AVALIABLE_CODE_VALUE
		if( bBroken)
		{
			m_weaponInfo[nInfoType]._pGage->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255, 0, 0, 255);
		}
		else
		{
			m_weaponInfo[nInfoType]._pGage->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 255);
		}

		m_weaponInfo[nInfoType]._pGage->SetEnable( TRUE);

		//	게이지
		pGageImage->setWidth( ((REAL32)nRate / 100.0f) * fWidth);
		pGageImage->setHeight( fHeight);

		FLUSH_CONTROL(m_weaponInfo[nInfoType]._pGage);
	}
	else
	{
		m_weaponInfo[nInfoType]._pGage->SetEnable( FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	Character
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CStepInventory::OnCharacterTab(void)
{
	_ResetVestige();

	m_pWeaponTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pCharacterTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pItemTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	for(UINT32 i = 0; i < MAX_WEAPON_INFO; i++)
	{
		m_weaponInfo[i]._pGage->SetEnable(FALSE);
	}

	m_pWeaponSect->SetEnable(FALSE);
	m_pCharacterSect->SetEnable(TRUE);
	m_pItemSect->SetEnable(FALSE);

	m_pWeaponWin->SetEnable(FALSE);
	m_pCharacterWin->SetEnable(TRUE);

	OnAllCharacter();

	OnTypePopup(0);
	m_pCharacterTypePopup->SetSelectedElement(0, FALSE);
}

void CStepInventory::OnChangeCharacterScroll(void)
{
	m_idxCharacterStartSlot = m_pCharaScrollList->GetCurrentTop() * SLOT_HORI_COUNT_CHARA;
}


void CStepInventory::OnFocusCharacter(UINT32 idxSlot)
{
	if(MAX_SLOT_FOR_INVEN_CHARACTER > idxSlot)
	{
		if (FALSE == m_characterSlot[idxSlot]._pSlot->GetShapeEnable(GUI_SHAPE_OFFSET + 3))
		{
			m_characterSlot[idxSlot]._pSlot->SetShapeEnable(GUI_SHAPE_OFFSET, FALSE);
			m_characterSlot[idxSlot]._pSlot->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
		}
	}
}

void CStepInventory::OnSelectCharacter(UINT32 idxSlot)
{
	if(MAX_SLOT_FOR_INVEN_CHARACTER > idxSlot)
	{
		PlaySound(GUI_SND_ITEM_SELECTING);

		m_idxSelectedCharacterSlot = idxSlot + m_idxCharacterStartSlot;

		I3_BOUNDCHK( idxSlot, MAX_SLOT_FOR_INVEN_CHARACTER);

		m_idxDescriptionCharacterID = m_characterSlot[idxSlot].nCharaID;
		m_idxDescriptionCharacterTerm = m_characterSlot[idxSlot].nUseTerm;
		m_idxDescriptionCharacterTeam = m_characterSlot[idxSlot].nTeam;

		//	캐릭터 일경우 장비하기 버튼을 변경하기로..
		switch( GET_ITEM_FLAG_CLASS( m_idxDescriptionCharacterID) )
		{
		case CHARACTER_CLASS_CHARA_TYPE :
		case CHARACTER_CLASS_CHARA_DINO :
			{
				m_pCharacterAuth->SetCaption(GAME_STRING("STR_INVEN_DO_CHANGE"));/*변경하기*/
				m_pCharacterEquip->SetCaption(GAME_STRING("STR_INVEN_DO_CHANGE"));/*변경하기*/
			}
			break;

		default:
			{
				m_pCharacterAuth->SetCaption(GAME_STRING("STR_INVEN_DO_EQUIP"));/*장비하기*/
				m_pCharacterEquip->SetCaption(GAME_STRING("STR_INVEN_DO_EQUIP"));/*장비하기*/
			}
			break;
		}
	}
}

void CStepInventory::OnEquipCharacter(UINT32 idxSlot)
{
	if(MAX_SLOT_FOR_INVEN_CHARACTER > idxSlot)
	{
		m_idxSelectedCharacterSlot = idxSlot + m_idxCharacterStartSlot;

		m_idxDescriptionCharacterID = m_characterSlot[idxSlot].nCharaID;
		m_idxDescriptionCharacterTerm = m_characterSlot[idxSlot].nUseTerm;
		m_idxDescriptionCharacterTeam = m_characterSlot[idxSlot].nTeam;
	}

	_CharacterDescriptionUpdate();

	if (m_pCharacterAuth->isEnable())
	{
		OnAuthCharacter();	//	인증하기
	}
	else if (m_pCharacterEquip->isEnable())
	{
		OnEquipCharacter();	//	장비하기
	}
	else if (m_pCharacterRemove->isEnable())
	{
		OnRemoveCharacter();//	해제하기
	}
}

void CStepInventory::OnAllCharacter(void)
{
	m_pAllCharacter->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	_ChangeCharacterClassType(CHARACTER_CLASS_ALL);
}

void CStepInventory::OnCharacterType(void)
{
	m_pTypeCharacter->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	_ChangeCharacterClassType(CHARACTER_CLASS_CHARA_TYPE);
}

void CStepInventory::OnCharacterHead(void)
{
	m_pHeadCharacter->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	_ChangeCharacterClassType(CHARACTER_CLASS_HEADGEAR);
}

void CStepInventory::OnCharacterBeret(void)
{
	m_pBeretCharacter->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	_ChangeCharacterClassType(CHARACTER_CLASS_ITEM);
}

void CStepInventory::OnDetailCharacter(void)
{
}

void CStepInventory::OnAuthCharacter(void)
{
	if (INVALID_INDEX == m_idxSelectedCharacterSlot)	return;

	INT32 slot = m_idxSelectedCharacterSlot - m_idxCharacterStartSlot;
	I3_BOUNDCHK(slot, MAX_SLOT_FOR_INVEN_CHARACTER);

	INT32 CharaID = m_characterSlot[slot].nCharaID;
	INT64 authIdx = g_pFramework->GetCharacterWareDBIdx(CharaID);
	if (INVALID_WAREDB_INDEX == authIdx) return;

	m_authID = authIdx;

	STRING_POPUP_Q(GAME_STRING("STR_INVEN_STARTING_AUTH"), MAKE_CALLBACK(CbAuthStart), this);/*사용을 시작하지 않은 아이템입니다.\n장비 시부터 남은 기간만큼 사용이 가능합니다.\n장비하시겠습니까?*/
}

void CStepInventory::OnTypeButton(void)
{
	ExclusionPopup(m_pCharacterTypePopup);
}

void CStepInventory::OnTypePopup(INT32 idxItem)
{
	switch( idxItem)
	{
	case 0:		m_nCharacterFilter = CHARACTER_TEAM_RB;			break;
	case 1:		m_nCharacterFilter = CHARACTER_TEAM_RED;		break;
	case 2:		m_nCharacterFilter = CHARACTER_TEAM_BLUE;		break;
	default:	m_nCharacterFilter = CHARACTER_TEAM_RB;			break;
	}

	const char * pszText = m_pCharacterTypePopup->GetText(idxItem);
	m_pCharacterTypeCombo->SetText((char *) pszText);

	m_idxSelectedCharacterSlot = INVALID_INDEX;
	m_idxCharacterStartSlot = 0;
	m_idxDescriptionCharacterID = -1;
	m_idxDescriptionCharacterTerm = -1;
	m_idxDescriptionCharacterTeam = -1;
	m_pCharaScrollList->SetCurValue(0.0f);

	//	인벤 리스트를 작성한다.
	_BuildCharacterList( m_nCharacterClassType);
}

void CStepInventory::OnEquipCharacter(void)
{
	if(g_pFramework == NULL)
		return;
	// 선택되어 정보가 보여지는 캐릭터가 없으면 처리하지 않는다
	if (-1 == m_idxDescriptionCharacterID)
	{
		return;
	}

	char szUserTitle[256] = "";
	INT32 slot = m_idxSelectedCharacterSlot - m_idxCharacterStartSlot;
	I3_BOUNDCHK(slot, MAX_SLOT_FOR_INVEN_CHARACTER);

	// 정보 불러오기
	INT32 ItemID		= m_characterSlot[slot].nCharaID;
	INT32 nTeam			= m_characterSlot[slot].nTeam;
	CHARACTER_CLASS_TYPE ItemUsage		= (CHARACTER_CLASS_TYPE)GET_ITEM_FLAG_CLASS(ItemID);


	UINT8 Type= 0;
	if ( ! g_pFramework->GetCharacterType(ItemID, &Type, NULL) )
		return;

	CShop * pShop = g_pGameContext->GetShop();
	AUTH_TYPE authType = pShop->GetAuthType(ItemID);

	if (Type == ITEM_ATTR_TYPE_BUY && AUTH_SEC == authType)
	{
		return;
	}


	if( !IsPossibleEquipUserTitleCharacter(ItemID, g_pGameContext->GetTitleInfo(), szUserTitle))
	{
		char szMessage[512] = "";
		sprintf_s( szMessage, GAME_STRING("STR_INVEN_ERQUIRE_DESIGNATION"), szUserTitle);/*[%s] 호칭을 장착하고 있지 않아 \n 선택하신 베레모를 장비할 수 없습니다.*/

		STRING_POPUP(szMessage);
		return;
	}

	m_bCharacterChanged = TRUE;

	switch(GET_ITEM_FLAG_CLASS(ItemID))
	{
	case CHARACTER_CLASS_CHARA_TYPE	:
		break;

	case CHARACTER_CLASS_ITEM	:
		// 베레모를 착용할 때 특정 애니메이션을 출력한다.
		if( g_pFramework->GetCurrentStage()->IsExactTypeOf( CStageReady::GetClassMeta()) )
		{
			CStageReady * pStage = (CStageReady *) g_pFramework->GetCurrentStage();
			pStage->PlayCeremonyCharacter();
		}
		break;
	}

	if( g_pFramework->EquipCharacterToMyInven(ItemID, ItemUsage, (TEAM_TYPE) nTeam))
	{
		PlaySound(GUI_SND_ITEM_EQUIP);
	}
}

void CStepInventory::OnRemoveCharacter(void)
{

	// 선택되어 정보가 보여지는 캐릭터가 없으면 처리하지 않는다
	if (-1 == m_idxDescriptionCharacterID)
	{
		return;
	}

	INT32 slot = m_idxSelectedCharacterSlot - m_idxCharacterStartSlot;
	I3_BOUNDCHK(slot, MAX_SLOT_FOR_INVEN_CHARACTER);

	// 정보 불러오기
	CHARACTER_CLASS_TYPE	RemoveItemUsage	= (CHARACTER_CLASS_TYPE)GET_ITEM_FLAG_CLASS(m_characterSlot[slot].nCharaID);
	INT32	nTeam	= m_characterSlot[slot].nTeam;


	m_bCharacterChanged = TRUE;

	if( g_pFramework->EquipCharacterToMyInven(NULL, RemoveItemUsage, (TEAM_TYPE) nTeam))
	{
		PlaySound(GUI_SND_ITEM_EQUIP);
	}
}

void CStepInventory::_InitCharaWin(void)
{
	m_pCharacterWin->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pCharacterCaption, GAME_STRING("STR_INVEN_USED_CHARA"));/*사용캐릭터*/

	m_pRedTypeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pDinoTypeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pBlueTypeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pHeadKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pBeretKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRedTypeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pDinoTypeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pBlueTypeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pHeadKey->SetTextColor(GetColor(GCT_STATIC));
	m_pBeretKey->SetTextColor(GetColor(GCT_STATIC));
	m_pRedTypeKey->SetText(GAME_STRING("STR_INVEN_RED_CHARA"));/*레드팀 캐릭터*/
	m_pBlueTypeKey->SetText(GAME_STRING("STR_INVEN_BLUE_CHARA"));/*블루팀 캐릭터*/
	m_pHeadKey->SetText(GAME_STRING("STR_INVEN_HEAD"));/*머리*/
	m_pBeretKey->SetText(GAME_STRING("STR_INVEN_BERET"));/*베레모*/
	m_pDinoTypeKey->SetText(GAME_STRING("STBL_IDX_SHOP_FILTER_DINO"));/*공룡캐릭터*/

	m_pRedTypeKey->setInputDisable(TRUE);
	m_pRedTypeDummy->setInputDisable(TRUE);
	m_pDinoTypeKey->setInputDisable(TRUE);
	m_pDinoTypeDummy->setInputDisable(TRUE);
	m_pBlueTypeKey->setInputDisable(TRUE);
	m_pBlueTypeDummy->setInputDisable(TRUE);
	m_pHeadKey->setInputDisable(TRUE);
	m_pHeadDummy->setInputDisable(TRUE);
	m_pBeretKey->setInputDisable(TRUE);
	m_pBeretDummy->setInputDisable(TRUE);
}

void CStepInventory::_InitInvenCharaTab(void)
{
	m_pCharacterSect->SetEnable(FALSE);

	INIT_CAPTION_BUTTON(m_pAllCharacter, GAME_STRING("STR_INVEN_VIEW_ALL"));/*모두보기*/
	INIT_CAPTION_BUTTON(m_pTypeCharacter, GAME_STRING("STR_INVEN_CHRACTER"));/*캐릭터*/
	INIT_CAPTION_BUTTON(m_pHeadCharacter, GAME_STRING("STR_INVEN_HEAD"));/*머리*/
	INIT_CAPTION_BUTTON(m_pBeretCharacter, GAME_STRING("STR_INVEN_BERET"));/*베레모*/

	ACTIVATE_BUTTON(m_pAllCharacter);
	ACTIVATE_BUTTON(m_pTypeCharacter);
	ACTIVATE_BUTTON(m_pHeadCharacter);
	ACTIVATE_BUTTON(m_pBeretCharacter);

	m_pAllCharacter->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_pCharaScrollList->CreateTextEx(3,  GetDefaultFontName());
	m_pCharaScrollList->SetSelColor(0, 0, 0, 0);

	m_pCharacterInfo->SetAllShapeEnable(FALSE);
	m_pCharacterInfo->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
	m_pCharacterInfoValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCharacterTermKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCharacterTermValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCharacterInfoValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCharacterTermKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCharacterTermValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCharacterTermKey->SetText(GAME_STRING("STR_INVEN_REMAIN_TERM"));/*남은기간*/

	m_pCharacterDescription->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pCharacterDescription->SetTextColor(GetColor(GCT_DEFAULT));
	//m_pCharacterDescription->SetTextSpace(0, 2);
	m_pCharacterDescription->SetTextAutoWrap(TRUE);

	INIT_CAPTION_BUTTON(m_pCharacterDetail, GAME_STRING("STR_INVEN_DETAIL_INFO"));/*상세정보*/
	INIT_CAPTION_BUTTON(m_pCharacterEquip, GAME_STRING("STR_INVEN_DO_EQUIP"));/*장비하기*/
	INIT_CAPTION_BUTTON(m_pCharacterAuth, GAME_STRING("STR_INVEN_DO_EQUIP"));/*장비하기*/
	INIT_CAPTION_BUTTON(m_pCharacterRemove, GAME_STRING("STR_INVEN_REMOVE"));/*해제하기*/

	DEACTIVATE_BUTTON(m_pCharacterDetail);
	DEACTIVATE_BUTTON(m_pCharacterEquip);
	DEACTIVATE_BUTTON(m_pCharacterAuth);

	m_pCharacterTypeCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCharacterTypeCombo->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCharacterTypeCombo->SetText(GAME_STRING("STR_INVEN_VIEW_TOTAL"));/*전체보기*/
	INIT_COMBO_EX(m_pCharacterTypeButton);

	m_nCharacterFilter = CHARACTER_TEAM_RB;

	m_pCharacterTypePopup->CreateTextEx(3,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pCharacterTypePopup->SetSelColor(100, 100, 100, 100);
	m_pCharacterTypePopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCharacterTypePopup->SetExclusiveEnableControl(TRUE);
	m_pCharacterTypePopup->AddElement(GAME_STRING("STR_INVEN_VIEW_TOTAL"));/*전체보기*/
	m_pCharacterTypePopup->AddElement(GAME_STRING("STR_INVEN_RED_ONLY"));/*레드팀 전용*/
	m_pCharacterTypePopup->AddElement(GAME_STRING("STR_INVEN_BLUE_ONLY"));/*블루팀 전용*/
	m_pCharacterTypePopup->SetEnable(FALSE);

	for(UINT32 i = 0; i < MAX_SLOT_FOR_INVEN_CHARACTER; i++)
	{
		INIT_BUTTON_EX(m_characterSlot[i]._pSlot);

		m_characterSlot[i]._pValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_characterSlot[i]._pValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_characterSlot[i]._pValue->SetTextSpace(0, -3);
		m_characterSlot[i]._pUsed->SetEnable(FALSE);
		m_characterSlot[i]._pUsed->SetAllShapeEnable(FALSE);
		m_characterSlot[i]._pSaleMark->SetEnable(FALSE);
		m_characterSlot[i]._pSaleMark->SetAllShapeEnable(FALSE);
		m_characterSlot[i]._pTerm->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_characterSlot[i]._pTerm->SetTextColor(GetColor(GCT_DEFAULT));

		m_characterSlot[i]._pImage->setInputDisable(TRUE);
		m_characterSlot[i]._pValue->setInputDisable(TRUE);
		m_characterSlot[i]._pUsed->setInputDisable(TRUE);
		m_characterSlot[i]._pTerm->setInputDisable(TRUE);
		m_characterSlot[i]._pSaleMark->setInputDisable(TRUE);
	}
}

void CStepInventory::_BuildCharacterList(CHARACTER_CLASS_TYPE type)
{
	// 소유 캐릭터를 불러온다
	//INT32 charaMaxCount = g_pFramework->GetCharacterCount(type);			//	인덱스 갯수
	INT32 charaMaxSlotCount = g_pFramework->GetCharacterSlotCount(type);	//	슬롯 갯수

	if( m_ShopSlotList.GetCount() > 0)
	{
		ShopSlot * pSlotArray = (ShopSlot*) m_ShopSlotList.GetFirst();
		I3MEM_SAFE_FREE( pSlotArray);
		m_ShopSlotList.Clear();
	}

	//	상점 슬롯 갯수가 실제 인벤하고 차이가 나면 상점 슬롯을 다시 작성
	if( charaMaxSlotCount <= 0)
		return;

	INT32 slotCount = 0;

	INT32 size = sizeof(ShopSlot) * charaMaxSlotCount;
	ShopSlot * pNewSlotArray = (ShopSlot*) i3MemAlloc( size);
	i3mem::FillZero( pNewSlotArray, size);

	for(INT32 i=0; i<charaMaxSlotCount; i++)
	{
		// 소유하고 있는 것을 가져온다.
		INT32 ID = g_pFramework->GetCharacterID(i, type);
		INT32 Term = g_pFramework->GetCharacterTerm(i, type);

		//	현재는 기획상 캐릭터만 해당
		if( GET_ITEM_FLAG_CLASS(ID) == CHARACTER_CLASS_CHARA_TYPE)
		{
			//	현재는 기획상 공용 캐릭터는 서버에서 인덱스 하나로 넘어오지만 실제로 인벤에서는 2개 슬롯으로 보여야 하며 따로 장착되어야 한다.
			//	그러므로 하나의 인덱스는 하나의 인벤 슬롯이라는 명제가 성립되지 않는다.
			//	그래서 공용 캐릭터만 좀 복잡하지만 2개의 슬롯으로 예외처리함. 2008.9.19 komet
			if( GET_ITEM_FLAG_CLASS( ID) == CHARACTER_TEAM_RB)
			{
				//	공용 캐릭터를 레드로 분리
				if( TESTBIT(CHARACTER_TEAM_RED, m_nCharacterFilter))
				{
					ShopSlot * pSlot = pNewSlotArray + slotCount;

					pSlot->_ItemID = ID;
					pSlot->_term = Term;
					pSlot->_team = CHARACTER_TEAM_RED;
					pSlot->_possible = 1;

					m_ShopSlotList.Add( (void*) pSlot);

					slotCount++;
				}

				//	공용 캐릭터를 블루로 분리
				if( TESTBIT(CHARACTER_TEAM_BLUE, m_nCharacterFilter))
				{
					ShopSlot * pSlot = pNewSlotArray + slotCount;

					pSlot->_ItemID = ID;
					pSlot->_term = Term;
					pSlot->_team = CHARACTER_TEAM_BLUE;
					pSlot->_possible = 1;

					m_ShopSlotList.Add( (void*) pSlot);

					slotCount++;
				}

				continue;	//	예외 처리 완료
			}
		}

		//	하나의 인덱스는 하나의 인벤 슬롯일 경우
		if( TESTBIT( GET_ITEM_FLAG_CLASS(ID), m_nCharacterFilter))
		{
			ShopSlot * pSlot = pNewSlotArray + slotCount;

			pSlot->_ItemID = ID;
			pSlot->_term = Term;
			pSlot->_team = GET_ITEM_FLAG_CLASS( ID);

			//	베레모만 장비가능 여부
			if( GET_ITEM_FLAG_CLASS( ID) == CHARACTER_CLASS_ITEM)
				pSlot->_possible = IsPossibleEquipUserTitleCharacter( ID, g_pGameContext->GetTitleInfo());
			else
				pSlot->_possible = 1;

			m_ShopSlotList.Add( (void*) pSlot);

			slotCount++;
		}
	}
}

void CStepInventory::_CharacterListUpdate(void)
{
	// 목록을 모두 지운다
	m_pCharaScrollList->Clear();

	INT32 max = m_ShopSlotList.GetCount();

	//	상품 리스트 슬롯 출력
	for(INT32 i=0; i<MAX_SLOT_FOR_INVEN_CHARACTER; i++)
	{
		INT32 nCurIndex = i + m_idxCharacterStartSlot;

		if( nCurIndex >= max)
		{
			m_characterSlot[i]._pSlot->SetShapeEnable(GUI_SHAPE_OFFSET + 2, FALSE);
			m_characterSlot[i]._pSlot->setInputDisable(TRUE);
			m_characterSlot[i]._pSlot->SetEnable(FALSE);

			m_characterSlot[i]._pImage->SetEnable(FALSE);
			m_characterSlot[i]._pValue->SetEnable(FALSE);
			m_characterSlot[i]._pUsed->SetEnable(FALSE);
			m_characterSlot[i]._pUsed->SetAllShapeEnable(FALSE);
			m_characterSlot[i]._pSaleMark->SetEnable(FALSE);
			m_characterSlot[i]._pSaleMark->SetAllShapeEnable(FALSE);
			m_characterSlot[i]._pTerm->SetEnable(FALSE);

			m_characterSlot[i].nTeam = -1;
			m_characterSlot[i].nUseTerm = -1;
		}
		else
		{
			I3_BOUNDCHK( nCurIndex, max);

			ShopSlot * pSlot = (ShopSlot*) m_ShopSlotList.GetItem( nCurIndex);
			BOOL	Selected = (m_idxSelectedCharacterSlot == i + m_idxCharacterStartSlot);

			_SetToCharacterSlot(i, Selected, pSlot->_ItemID, pSlot->_team, pSlot->_term);

			//	베레모만
			if( GET_ITEM_FLAG_CLASS( pSlot->_ItemID) == CHARACTER_CLASS_ITEM)
			{
				if( pSlot->_possible == FALSE)
				{
					m_characterSlot[i]._pTerm->SetEnable(TRUE);
					m_characterSlot[i]._pTerm->SetText(GAME_STRING("STR_INVEN_CANNOT_EQUIP"));/*{col:210,0,0,255}장비 불가{/col}*/
				}
			}
		}
	}

	for(INT32 i = 0; i < (max + 2) / 3; i ++)
	{
		m_pCharaScrollList->AddElement("");
	}
}

void CStepInventory::_SetToCharacterSlot(INT32 slotNum, BOOL Selected, INT32 charaID, INT32 charaTeam, INT32 charaTerm)
{
	I3_BOUNDCHK( slotNum, MAX_SLOT_FOR_INVEN_CHARACTER);

	CharacterSlot	& Slot = m_characterSlot[slotNum];

	g_pFramework->SetCharacterShape(Slot._pImage, charaID);


	Slot.nTeam = charaTeam;
	Slot.nCharaID = charaID;
	Slot.nUseTerm = charaTerm;

	Slot._pImage->SetEnable(TRUE);
	Slot._pUsed->SetAllShapeEnable(FALSE);
	Slot._pUsed->SetEnable(FALSE);
	Slot._pSaleMark->SetAllShapeEnable(FALSE);
	Slot._pSaleMark->SetEnable(FALSE);
	Slot._pTerm->SetEnable(FALSE);

	// 슬록 활성화
	Slot._pSlot->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
	Slot._pSlot->setInputDisable(FALSE);
	Slot._pSlot->SetEnable(TRUE);

	//	선택 슬롯
	if (Selected)
	{
		Slot._pSlot->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
	}

	// 이름
	char szString[MAX_STRING_COUNT] = "";
	g_pCharaInfoDataBase->GetCharacterName(charaID, szString, sizeof(szString));
	Slot._pValue->SetText(szString);
	Slot._pValue->SetEnable(TRUE);

	// 사용여부
	UINT8 Type = 0;
	UINT32 Arg = 0;
	if ( ! g_pFramework->GetCharacterType(charaID, &Type, &Arg) )
		return;

	CShop * pShop = g_pGameContext->GetShop();
	AUTH_TYPE authType = pShop->GetAuthType(charaID);

	USER_INFO_EQUIPMENT_CHARA	InfoEquipChara;
	g_pGameContext->GetMyUserInfoEquipmentChara(InfoEquipChara);

	//// 1.5 Check CharaTeam (메서드 내 nTeam이 불필요하여 주석처리)
	if (g_pCharaInfoDataBase->IsUsedCharacter(&InfoEquipChara, charaID/*, charaTeam*/))
	{
		Slot._pUsed->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);
		Slot._pUsed->SetEnable(TRUE);
	}
	else if ( (ITEM_ATTR_TYPE_BUY == Type) || (ITEM_ATTR_TYPE_AUTH == Type ))
	{
		//	구매완료
		if (ITEM_ATTR_TYPE_BUY == Type && authType == AUTH_SEC)
		{
			Slot._pUsed->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
			Slot._pUsed->SetEnable(TRUE);
		}
	}

	// 상품 판매 마크
	UINT8 nSaleMark = GetItemSaleMarkIndex(Type);
	SetItemSaleMark( Slot._pSaleMark, nSaleMark);

	if ( authType != AUTH_UNKNWON )
	{
		char termText[MAX_STRING_COUNT] = "";
		BOOL Expendable = (authType == AUTH_COUNT) ? TRUE : FALSE;
		_GetInvenUseDay(Expendable, Type, Arg, termText, MAX_STRING_COUNT);

		Slot._pTerm->SetText(termText);
		Slot._pTerm->SetEnable(TRUE);
	}
}

void CStepInventory::_CharacterDescriptionUpdate(void)
{

	// 초기화
	m_pCharacterInfo->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
	m_pCharacterInfo->SetEnable(TRUE);
	m_pCharacterInfoDummy->SetEnable(FALSE);
	m_pCharacterInfoValue->SetEnable(FALSE);
	m_pCharacterTermKey->SetEnable(FALSE);
	m_pCharacterTermValue->SetEnable(FALSE);
	m_pCharacterDetail->SetEnable(FALSE);
	m_pCharacterDescription->SetEnable(FALSE);

	m_pCharacterAuth->SetEnable(FALSE);
	m_pCharacterEquip->SetEnable(FALSE);
	m_pCharacterRemove->SetEnable(FALSE);

	// 찾지 못하면 처리하지 않음
	if (-1 == m_idxDescriptionCharacterID)
	{
		return;
	}

	INT32 charaID = m_idxDescriptionCharacterID;
	INT32 charaTeam = m_idxDescriptionCharacterTeam;

	UINT8 itemAttr;
	UINT32 itemArg;

	if (FALSE == g_pFramework->GetCharacterType(charaID, &itemAttr, &itemArg))
	{
		return;
	}

	//	장비 버튼 활성화/비활성화
	{
		USER_INFO_EQUIPMENT_CHARA	InfoEquipChara;
		g_pGameContext->GetMyUserInfoEquipmentChara(InfoEquipChara);
		UINT32 nCheckCharaflag = 0;

		//	이미 장비한 무기
		if( charaTeam & CHARACTER_TEAM_RED)
		{
			nCheckCharaflag = InfoEquipChara.GetRedNum();
		}
		else if( charaTeam & CHARACTER_TEAM_BLUE)
		{
			nCheckCharaflag = InfoEquipChara.GetBlueNum();
		}

		if( nCheckCharaflag == (UINT32) charaID ||
			InfoEquipChara.GetHairNum() == (UINT32) charaID)
		{
			m_pCharacterEquip->SetEnable(TRUE);

			DEACTIVATE_BUTTON(m_pCharacterEquip);
		}
		else if( InfoEquipChara.GetItemNum() == (UINT32) charaID)		//	베레모일 경우
		{
			m_pCharacterRemove->SetEnable(TRUE);

			ACTIVATE_BUTTON(m_pCharacterRemove);	//	현재 베레모일 경우 해제가 가능하도록
		}
		else
		{
			_SetAuthButton(charaID, itemAttr, m_pCharacterEquip, m_pCharacterAuth);
		}
	}

	char termText[MAX_STRING_COUNT] = {0, };

	CShop * pShop = g_pGameContext->GetShop();
	AUTH_TYPE authType = pShop->GetAuthType(charaID);
	if ( authType != AUTH_UNKNWON )
	{
		BOOL Expendable = (authType == AUTH_COUNT) ? TRUE : FALSE;
		_GetInvenUseDay(Expendable, itemAttr, itemArg, termText, MAX_STRING_COUNT);

		m_pCharacterTermKey->SetText(termText);
		m_pCharacterTermKey->SetEnable(TRUE);
	}
	// 이미지
	if (g_pFramework->SetCharacterShape(m_pCharacterInfoDummy, charaID))
	{
		m_pCharacterInfo->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
		m_pCharacterInfoDummy->SetEnable(TRUE);
	}

	// 이름
	char szString[2048] = "";
	g_pCharaInfoDataBase->GetCharacterName(charaID, szString, sizeof(szString));
	m_pCharacterInfoValue->SetText(szString);
	m_pCharacterInfoValue->SetEnable(TRUE);

	// 설명
	g_pCharaInfoDataBase->GetDescription(charaID, szString, 2048);
	m_pCharacterDescription->SetText(szString);
	m_pCharacterDescription->SetEnable(TRUE);
}

void CStepInventory::_ChangeCharacterClassType(CHARACTER_CLASS_TYPE type)
{
	if( m_nCharacterClassType == type) return;

	i3GuiButton* pButton = NULL;

	switch(m_nCharacterClassType)
	{
	case CHARACTER_CLASS_ALL			:	pButton = m_pAllCharacter;		break;
	case CHARACTER_CLASS_CHARA_TYPE		:	pButton = m_pTypeCharacter;		break;
	case CHARACTER_CLASS_HEADGEAR			:
	case CHARACTER_CLASS_FACEGEAR			:
	case CHARACTER_CLASS_EVENT			:	pButton = m_pHeadCharacter;		break;
	case CHARACTER_CLASS_ITEM			:	pButton = m_pBeretCharacter;	break;
	case CHARACTER_CLASS_CHARA_DINO		:	pButton = m_pTypeCharacter;		break;
	default:
		return;
	}

	pButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_nCharacterClassType = type;
	m_idxSelectedCharacterSlot = INVALID_INDEX;
	m_idxCharacterStartSlot = 0;

	m_idxDescriptionCharacterID = -1;
	m_idxDescriptionCharacterTerm = -1;
	m_idxDescriptionCharacterTeam = -1;

	m_pCharaScrollList->SetCurValue(0.0f);

	//	인벤 리스트를 작성한다.
	_BuildCharacterList( m_nCharacterClassType);
}

void CStepInventory::_CharacterWinUpdate(void)
{
	// 초도보급 전 처리 금지. 닉네임 유무로 확인한다
	if ( ! g_pGameContext->HaveNickName() )
	{
		return;
	}

	// Red/Blue 캐릭터가 없으면 처리하지 않는다
	if (FALSE == g_pGameContext->IsMyEquipChara())
	{
		return;
	}

	// 캐릭터 장비창이 활성화되있다면 갱신
	if (m_pCharacterWin->isEnable())
	{
		USER_INFO_EQUIPMENT_CHARA	InfoEquipChara;
		g_pGameContext->GetMyUserInfoEquipmentChara(InfoEquipChara);

		//	Red
		if( GET_ITEM_FLAG_CLASS(InfoEquipChara.GetRedNum()) == CHARACTER_CLASS_CHARA_TYPE)
		{
			// 이름
			g_pFramework->SetCharacterShape(m_pRedTypeDummy, InfoEquipChara.GetRedNum());
		}

		//	Blue
		if( GET_ITEM_FLAG_CLASS(InfoEquipChara.GetBlueNum()) == CHARACTER_CLASS_CHARA_TYPE)
		{
			// 이름
			g_pFramework->SetCharacterShape(m_pBlueTypeDummy, InfoEquipChara.GetBlueNum());
		}

		// 캐릭터
		g_pFramework->SetCharacterShape(m_pHeadDummy, InfoEquipChara.GetHairNum());

		// 베레모
		g_pFramework->SetCharacterShape(m_pBeretDummy, InfoEquipChara.GetItemNum());

		// 이름
		g_pFramework->SetCharacterShape(m_pDinoTypeDummy, InfoEquipChara.GetDinoNum());

		bool enablDino = IsAvailableStageMode(STAGE_TYPE_ESCAPE);

		m_pDinoTypeKey->SetEnable( enablDino );
		m_pDinoTypeDummy->SetEnable( enablDino );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	Item
void CStepInventory::_InitInvenItemTab(void)
{
	m_pItemSect->SetEnable(FALSE);

	INIT_CAPTION_BUTTON(m_pItemGroupAll, GAME_STRING("STR_INVEN_VIEW_ALL"));/*모두보기*/
	INIT_CAPTION_BUTTON(m_pItemGroupMaintenance, GAME_STRING("STR_INVEN_MAINTENANCE_ITEM"));/*지속성*/
	INIT_CAPTION_BUTTON(m_pItemGroupExpendable, GAME_STRING("STR_INVEN_EXPENDABLES"));/*소모성*/

	ACTIVATE_BUTTON(m_pItemGroupAll);
	ACTIVATE_BUTTON(m_pItemGroupMaintenance);
	ACTIVATE_BUTTON(m_pItemGroupExpendable);

	m_pItemGroupAll->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_pItemScrollList->CreateTextEx(3,  GetDefaultFontName());
	m_pItemScrollList->SetSelColor(0, 0, 0, 0);
	m_pItemScrollList->SetCurValue(0.0f);

	INIT_CAPTION_BUTTON(m_pItemDetailUse, GAME_STRING("STR_INVEN_DO_USE"));/*사용하기*/
	ACTIVATE_BUTTON(m_pItemDetailUse);
#ifdef ENABLE_INVEN_ITEM_DELETE
	INIT_CAPTION_BUTTON(m_pItemDelete, GAME_STRING("STR_INVEN_DELETE"));	//	삭제하기
	ACTIVATE_BUTTON(m_pItemDelete);
#endif
	

	for(UINT32 i = 0; i < MAX_SLOT_FOR_INVEN_ITEM; i++)
	{
		INIT_BUTTON_EX(m_itemSlot[i]._pSlot);

		m_itemSlot[i]._pSlot->SetEnable(FALSE);

		m_itemSlot[i]._pImage->setInputDisable(TRUE);

		m_itemSlot[i]._pValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_itemSlot[i]._pValue->SetTextColor(GetColor(GCT_DEFAULT));
		m_itemSlot[i]._pValue->SetTextSpace(0, -3);
		m_itemSlot[i]._pValue->setInputDisable(TRUE);

		m_itemSlot[i]._pUsed->SetEnable(FALSE);
		m_itemSlot[i]._pUsed->SetAllShapeEnable(FALSE);
		m_itemSlot[i]._pUsed->setInputDisable(TRUE);

		m_itemSlot[i]._pSaleMark->SetEnable(FALSE);
		m_itemSlot[i]._pSaleMark->SetAllShapeEnable(FALSE);
		m_itemSlot[i]._pSaleMark->setInputDisable(TRUE);

		m_itemSlot[i]._pTerm->ReCreateTextEx( GetDefaultFontName(), 32, 9, FALSE, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_NORMAL);
		m_itemSlot[i]._pTerm->getTextNodeDX()->SetShadowColor(255, 255, 255, 255);

		m_itemSlot[i]._pTerm->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_itemSlot[i]._pTerm->SetTextColor(GetColor(GCT_DEFAULT));
		m_itemSlot[i]._pTerm->setInputDisable(TRUE);
	}

	m_pItemDetail->SetAllShapeEnable(FALSE);
	m_pItemDetail->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);
	m_pItemDetailValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pItemDetailValue->SetTextColor(GetColor(GCT_DEFAULT));

	m_pItemDetailDescription->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pItemDetailDescription->SetTextColor(GetColor(GCT_DEFAULT));
	m_pItemDetailDescription->SetTextAutoWrap(TRUE);
}

void CStepInventory::_ClearItemSlot(INT32 nSlot)
{
	I3_BOUNDCHK( nSlot, MAX_SLOT_FOR_INVEN_ITEM);

	m_itemSlot[nSlot]._pSlot->SetShapeEnable(GUI_SHAPE_OFFSET + 2, FALSE);
	m_itemSlot[nSlot]._pSlot->setInputDisable(TRUE);
	m_itemSlot[nSlot]._pSlot->SetEnable(FALSE);

	m_itemSlot[nSlot]._pImage->SetEnable(FALSE);
	m_itemSlot[nSlot]._pValue->SetEnable(FALSE);
	m_itemSlot[nSlot]._pUsed->SetEnable(FALSE);
	m_itemSlot[nSlot]._pUsed->SetAllShapeEnable(FALSE);
	m_itemSlot[nSlot]._pSaleMark->SetEnable(FALSE);
	m_itemSlot[nSlot]._pSaleMark->SetAllShapeEnable(FALSE);
	m_itemSlot[nSlot]._pTerm->SetEnable(FALSE);
}

void CStepInventory::_ItemListUpdate(void)
{
	CInvenList * pInvenList = g_pFramework->GetInvenList();
	I3ASSERT( pInvenList != NULL);

	INT32 itemMaxCount = pInvenList->GetCashItemCount(m_nItemUsageType);

	GameGUI::Reset(m_pItemScrollList, GameGUI::CalcuScrollCount(itemMaxCount, 3));


	for(INT32 i=0; i<MAX_SLOT_FOR_INVEN_ITEM; i++)
	{
		INT32 nCurIndex = i + m_idxItemStartSlot;

		if( nCurIndex >= itemMaxCount)
		{
			//	가진 갯수를 넘을시 나머지 슬롯은 없앤다.
			_ClearItemSlot(i);
			continue;
		}

		// 소유하고 있는 것을 가져온다.
		INT32 itemID = pInvenList->GetCashItemID(nCurIndex, m_nItemUsageType);
		INT32 itemTerm = pInvenList->GetCashItemTerm(nCurIndex, m_nItemUsageType);

		//임시 코드 삭제해도 무방
		//ItemAbilityAttribute ret = CharaAbilityManager::Get()->GetItemAbilityAttribute( itemID );


		_SetToItemSlot(i, m_idxSelectedItemSlot, m_idxItemStartSlot, itemID, itemTerm);
	}

}

void CStepInventory::_SetToItemSlot(INT32 slotNum, INT32 selectedSlotNum, INT32 startSlotNum, INT32 ItemID, INT32 term)
{
	I3_BOUNDCHK( slotNum, MAX_SLOT_FOR_INVEN_ITEM);

	CashItemSlot &	ItemSlot = m_itemSlot[slotNum];


	//	선택 슬롯
	if (selectedSlotNum == slotNum + startSlotNum)
	{
		ItemSlot._pSlot->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
	}

	// 슬록 활성화
	ItemSlot._pSlot->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
	ItemSlot._pSlot->setInputDisable(FALSE);
	ItemSlot._pSlot->SetEnable(TRUE);

	// 아이템 이름	
	char Name[MAX_STRING_COUNT] = {};
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(ItemID);
	if(!pInfo)
	{
		i3String::Format(Name, MAX_STRING_COUNT, 
			"INVALID\nItem:%d\nNum:%d", ItemID);
		ItemSlot._pValue->SetText(Name);
		ItemSlot._pValue->SetEnable(TRUE);
		return;
	}

	//	아이템 이미지
	ItemSlot._pImage->SetEnable(TRUE);
	g_pFramework->SetCashItemShape(ItemSlot._pImage, ItemID);

#ifndef NC_BUILD
		bool noName = false;
#endif

	if ( (GET_ITEM_FLAG_TYPE(ItemID) != ITEM_TYPE_POINT) && 
		(GET_ITEM_FLAG_TYPE(ItemID) == ITEM_TYPE_MAINTENANCE) && 
		(GET_ITEM_FLAG_NUMBER(ItemID) > 0) )
	{
		// 인증하지 않은 기간제 아이템은 Number에 몇일짜리 아이템인지 저장되어 있다.
		char OrigName[MAX_STRING_COUNT] = {};
		g_pShopItemInfoDataBase->GetItemName(ItemID, OrigName, MAX_STRING_COUNT);

		INT32 nTerm = GET_ITEM_FLAG_NUMBER(ItemID);

		i3String::Format(Name, MAX_STRING_COUNT, 
			GAME_STRING("STR_INVEN_CASH_ITEM_TERM"), OrigName, nTerm);	/*%s %d일권*/
	}
	else
	{
		g_pShopItemInfoDataBase->GetItemName(ItemID, Name, MAX_STRING_COUNT);
	}

	if( i3String::Length(Name) == 0)
	{
		i3String::Format(Name, MAX_STRING_COUNT, "NO NAME\nItem:%d\nGroup:%d\nUsage:%d", 
			ItemID, pInfo->GetGroupIndex(), pInfo->GetType() );
#ifndef NC_BUILD
		noName = true;
#endif
	}

#ifndef NC_BUILD
		ItemSlot._pValue->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE );
		ItemSlot._pSaleMark->SetEnable( TRUE );
		ItemSlot._pImage->SetEnable( TRUE );

		if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LCTRL ) )
		{
			if( !noName )
			{
				i3String::Format( Name, sizeof(Name), 
					"%s\nItem:%d\nGroup:%d\nUsage:%d", Name,
					ItemID, pInfo->GetGroupIndex(), pInfo->GetType() );
			}
			ItemSlot._pValue->SetTextAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP );
			ItemSlot._pSaleMark->SetEnable( FALSE );
			ItemSlot._pImage->SetEnable( FALSE );
		}
#endif

	ItemSlot._pValue->SetText(Name);
	ItemSlot._pValue->SetEnable(TRUE);

	// 사용여부
	UINT8	Type	= 0;
	UINT32	Arg		= 0;
	if (g_pFramework->GetCashItemTypeByID( ItemID, &Type, &Arg))
	{
		CShop * pShop = g_pGameContext->GetShop();
		char termText[MAX_STRING_COUNT] = "";

		ItemSlot._pUsed->SetEnable(FALSE);
		ItemSlot._pUsed->SetAllShapeEnable(FALSE);

		AUTH_TYPE authType = pShop->GetAuthType(ItemID);

		// 사용중 또는 미사용중 마크 표시처리.
		// 구매완료 (미사용)
		// 개수제가 아닐경우에만 (미사용)이라는 태그이미지가 나오게 합니다.
		if (ITEM_ATTR_TYPE_BUY == Type && AUTH_COUNT != authType)
		{
			ItemSlot._pUsed->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
			ItemSlot._pUsed->SetEnable(TRUE);
		}
		//	사용중
		else if (ITEM_ATTR_TYPE_AUTH == Type)
		{
			ItemSlot._pUsed->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
			ItemSlot._pUsed->SetEnable(TRUE);
		}


		// 상품 판매 마크
		UINT8 nSaleMark = GetItemSaleMarkIndex(Type);
		SetItemSaleMark( ItemSlot._pSaleMark, nSaleMark);

		if ( authType != AUTH_UNKNWON )
		{
			BOOL IsCountItem = (authType == AUTH_COUNT) ? TRUE : FALSE;
			_GetInvenUseDay(IsCountItem, Type, Arg, termText, MAX_STRING_COUNT);
		}

		ItemSlot._pTerm->SetText(termText);
		ItemSlot._pTerm->SetEnable(TRUE);
	}

}

void CStepInventory::_ItemDescriptionUpdate(void)
{
	// 초기화
	m_pItemDetail->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
	m_pItemDetail->SetEnable(TRUE);

	m_pItemDetailDummy->SetEnable(FALSE);
	m_pItemDetailValue->SetEnable(FALSE);
	//m_pCharacterTermKey->SetEnable(FALSE);
	//m_pCharacterTermValue->SetEnable(FALSE);
	m_pItemDetailUse->SetEnable(FALSE);
	m_pItemDelete->SetEnable(FALSE);
	m_pItemDetailDescription->SetEnable(FALSE);

	// 찾지 못하면 처리하지 않음
	if (-1 == m_idxDescriptionItemID)
	{
		return;
	}

	INT32 ItemID = m_idxDescriptionItemID;

	//	장비 버튼 활성화/비활성화
	UINT8	Type	= 0;
	UINT32	Arg		= 0;
	g_pFramework->GetCashItemTypeByID( ItemID, &Type, &Arg);
	if ( Type == ITEM_ATTR_TYPE_AUTH)	// 사용중
	{
		DEACTIVATE_BUTTON(m_pItemDetailUse);		// 아이템을 이미 사용중이므로 사용하기 버튼 비활성화
#if defined (ENABLE_INVEN_ITEM_DELETE)
		ACTIVATE_BUTTON(m_pItemDelete);
		// 아이템 삭제하기 버튼
		INT32 itemUsage = GET_ITEM_FLAG_TYPE(ItemID);
		INT32 itemType = GET_ITEM_FLAG_CLASS(ItemID);
		switch( itemUsage)
		{
			//	지속성
			//	인벤토리에 있는 사용 가능한 갯수 아이템인 아닌 현재 사용중인 아이템이다.
		case ITEM_TYPE_MAINTENANCE:
			{
				switch( itemType)
				{
				case CASHITEM_GROUP_XP_PLUS_110:
				case CASHITEM_GROUP_XP_PLUS_130:
				case CASHITEM_GROUP_XP_PLUS_150:
				case CASHITEM_GROUP_PT_PLUS_130:
				case CASHITEM_GROUP_COLOR_CLAN:
				case CASHITEM_GROUP_COLOR_NICK:
				case CASHITEM_GROUP_SHORT_RESPAWN:
				case CASHITEM_GROUP_AMMO_UP:
				case CASHITEM_GROUP_DISGUISE_RANK:
				case CASHITEM_GROUP_DISGUISE_NICK:
				case CASHITEM_GROUP_FREE_MOVE:
				case CASHITEM_GROUP_CLAN_XP_PLUS_150:
				case CASHITEM_GROUP_CUSTOM_CROSSHAIR:
				case CASHITEM_GROUP_COLOR_CROSSHAIR:
				case CASHITEM_GROUP_COLOR_CHATTING:
				case CASHITEM_GROUP_SHORT_RESPAWN_50:
				case CASHITEM_GROUP_SHORT_RESPAWN_20:
				case CASHITEM_GROUP_BULLET_PROOF_VEST:
				case CASHITEM_GROUP_BULLET_PROOF_VEST_PLUS:
				case CASHITEM_GROUP_BULLET_PROOF_VEST_METAL:
				case CASHITEM_GROUP_MEGA_HP5:
				case CASHITEM_GROUP_MEGA_HP10:
					{
						ACTIVATE_BUTTON(m_pItemDelete);
					}
					break;
				}
			}
			break;
		}
#endif
	}
	else
	{
		ACTIVATE_BUTTON(m_pItemDetailUse);
#if defined (ENABLE_INVEN_ITEM_DELETE)
		DEACTIVATE_BUTTON(m_pItemDelete);
#endif
	}

	// 설명과 이름
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(ItemID);
	if(pInfo)
	{
		char Name[MAX_STRING_COUNT] = {};
		g_pShopItemInfoDataBase->GetItemName(ItemID, Name, MAX_STRING_COUNT);
		m_pItemDetailValue->SetText(Name);

		m_pItemDetailDescription->SetText(pInfo->GetDescription());


		// 이미지
		if (g_pFramework->SetCashItemShape(m_pItemDetailDummy, ItemID))
		{
			m_pItemDetail->SetShapeEnable(GUI_SHAPE_OFFSET + 3, FALSE);
			m_pItemDetailDummy->SetEnable(TRUE);
		}
	}
	else
	{
		m_pItemDetailValue->SetText(INVALID_TEXT);
		m_pItemDetailDescription->SetText(INVALID_TEXT);
	}

	m_pItemDetailValue->SetEnable(TRUE);
	m_pItemDetailDescription->SetEnable(TRUE);

}

void CStepInventory::_ChangeItemUsageType(ITEM_TYPE nType)
{
	m_nItemUsageType = nType;
	m_idxSelectedItemSlot = INVALID_INDEX;
	m_idxDescriptionItemID = -1;
	m_idxItemStartSlot = 0;
	m_pItemScrollList->SetCurValue(0.0f);

	for(UINT32 i = 0; i < MAX_SLOT_FOR_INVEN_ITEM; i++)
	{
		m_itemSlot[i]._pSlot->SetShapeEnable(GUI_SHAPE_OFFSET + 2, FALSE);
		m_itemSlot[i]._pSlot->setInputDisable(TRUE);
		m_itemSlot[i]._pSlot->SetEnable(FALSE);

		m_itemSlot[i]._pImage->SetEnable(FALSE);
		m_itemSlot[i]._pValue->SetEnable(FALSE);
		m_itemSlot[i]._pUsed->SetEnable(FALSE);
		m_itemSlot[i]._pUsed->SetAllShapeEnable(FALSE);
		m_itemSlot[i]._pSaleMark->SetEnable(FALSE);
		m_itemSlot[i]._pSaleMark->SetAllShapeEnable(FALSE);
		m_itemSlot[i]._pTerm->SetEnable(FALSE);
	}
}

void CStepInventory::OnItemTab(void)
{
	if ( ! g_pConfigEx->GetShop().Item.Enable )
	{
		STRING_POPUP(GAME_STRING("STR_INVEN_PREPARE_PLAN"));/*차후 지원될 예정입니다.*/
		return;
	}

	_ResetVestige();

	m_pWeaponTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pCharacterTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pItemTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_pWeaponSect->SetEnable(FALSE);
	m_pCharacterSect->SetEnable(FALSE);
	m_pItemSect->SetEnable(TRUE);

	m_pWeaponWin->SetEnable(FALSE);
	m_pCharacterWin->SetEnable(FALSE);

	OnItemGroupAll();

	m_idxSelectedItemSlot = INVALID_INDEX;
	m_idxDescriptionItemID = -1;
	m_idxItemStartSlot = 0;
	m_pItemScrollList->SetCurValue(0.0f);
}

void CStepInventory::OnItemGroupAll(void)
{
	m_pItemGroupAll->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pItemGroupMaintenance->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pItemGroupExpendable->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	_ChangeItemUsageType(ITEM_TYPE_UNKNOWN);
}

void CStepInventory::OnItemGroupMaintenance(void)
{
	m_pItemGroupAll->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pItemGroupMaintenance->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pItemGroupExpendable->setControlState(I3GUI_CONTROL_STATE_NORMAL);


	_ChangeItemUsageType(ITEM_TYPE_MAINTENANCE);
}

void CStepInventory::OnItemGroupExpendable(void)
{
	m_pItemGroupAll->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pItemGroupMaintenance->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pItemGroupExpendable->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	_ChangeItemUsageType(ITEM_TYPE_EXPENDABLE);
}

void CStepInventory::OnChangeItemScroll(void)
{
	m_idxItemStartSlot = m_pItemScrollList->GetCurrentTop() * SLOT_HORI_COUNT_ITEM;
}

void CStepInventory::OnFocusItem(UINT32 slot)
{
	if(MAX_SLOT_FOR_INVEN_ITEM > slot)
	{
		if (FALSE == m_itemSlot[slot]._pSlot->GetShapeEnable(GUI_SHAPE_OFFSET + 3))
		{
			m_itemSlot[slot]._pSlot->SetShapeEnable(GUI_SHAPE_OFFSET, FALSE);
			m_itemSlot[slot]._pSlot->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
		}
	}
}

void CStepInventory::OnSelectItem(UINT32 slot)
{
	if(MAX_SLOT_FOR_INVEN_ITEM > slot)
	{
		PlaySound(GUI_SND_ITEM_SELECTING);

		m_idxSelectedItemSlot = slot + m_idxItemStartSlot;
		m_idxDescriptionItemID = g_pFramework->GetInvenList()->GetCashItemID(m_idxSelectedItemSlot, m_nItemUsageType);
	}
}

void CStepInventory::OnAuthCashItem(void)
{
	//	사용할 아이템
	char temp[MAX_STRING_COUNT] = "";
	INT32 ItemID = m_idxDescriptionItemID;		//	사용할 아이템
	INT32 itemUsage = GET_ITEM_FLAG_TYPE( ItemID);
	INT32 itemType = GET_ITEM_FLAG_CLASS( ItemID);
	BOOL bMaster = g_pFramework->GetClanContext()->IsMaster();
	CLAN_DETAIL_INFO * pClanInfo = g_pGameContext->getMyClanDetailInfo();

#if defined(CHANGE_ITEM_USE_CHECK)
	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(ItemID);
	
	if( pInfo == NULL)
	{
		return;	//	Goods.pef에 등록되지 않은 아이템
	}

	if ( pInfo->GetPurchaseAuthority() != PURCHASE_AUTH_ANYONE )
	{
		if ( pInfo->GetPurchaseAuthority() == PURCHASE_AUTH_CLAN_MASTER && !bMaster )
		{
			const char * Msg = pInfo->GetCertificationAuthorityDesc();	// 사용할 수 없는 메시지
			CAPTION_POPUP(Msg, GAME_STRING("STR_INVEN_DO_NOT_USE"));	//아이템 사용 불가
			return;
		}
	}

	if ( pInfo->GetUsePosition() != USE_POS_ANYWHERE )
	{
		const char * Msg = pInfo->GetUsePositionDesc();				// 사용할 수 있는 위치 메시지
		CAPTION_POPUP(Msg, GAME_STRING("STR_INVEN_DO_NOT_USE"));	//아이템 사용 불가
		return;
	}
#else
	//	먼저 클랜 마스터용 아이템인지 여부 판단.
	if( CheckCashItemOnlyClanMaster( ItemID) && !bMaster)
	{
		CAPTION_POPUP(GAME_STRING("STR_INVEN_ONLY_MASTER"),
					GAME_STRING("STR_INVEN_DO_NOT_USE"));/*아이템 사용 불가*//*클랜 마스터만\n 사용할 수 있는 아이템입니다.*/
		return;
	}

	//	클랜 관리 페이지용 아이템인지 여부 판단.
	if( CheckCashItemOnlyClanManagementStep( ItemID))
	{
		CAPTION_POPUP(GAME_STRING("STR_INVEN_ONLY_CLAN_MANAGEMENT"),
					GAME_STRING("STR_INVEN_DO_NOT_USE"));/*아이템 사용 불가*//*클랜 관리 페이지에서만\n 사용할 수 있는 아이템입니다.*/
		return;
	}

	//	인게임에서 사용하는 캐시 아이템인지 여부 판단.
	if( CheckCashItemOnlyInGame( ItemID))
	{
		CAPTION_POPUP(GAME_STRING("STR_INVEN_ONLY_USE_IN_GAME"),
					GAME_STRING("STR_INVEN_DO_NOT_USE"));/*게임중에만 사용할 수 있는 아이템입니다.*//*아이템 사용 불가*/
		return;
	}
#endif

	//	아이템 별로 사용시 예외처리
	if( itemUsage == ITEM_TYPE_MAINTENANCE)			//	지속성
	{
	}
	else if( itemUsage == ITEM_TYPE_EXPENDABLE)		//	소모성
	{
		switch( itemType)
		{
		case CASHITEM_GROUP_CLAN_PERSON_PLUS_50:
			{
				//	클랜원 증가 아이템은 최대 회원수를 넘을시 사용 불가
				if( pClanInfo->_maxPerson >= MAX_MEMBER_COUNT)
				{
					sprintf_s( temp, GAME_STRING("STR_INVEN_LIMITED_CLAN_MEMBER"), MAX_MEMBER_COUNT);/*아이템을 사용할 수 없습니다.\n \n 클랜 최대 인원수는\n {col:255,153,51,255}%d{/col}명으로 제한되어 있습니다.*/
					CAPTION_POPUP( temp, GAME_STRING("STR_INVEN_DO_NOT_USE"));/*아이템 사용 불가*/

					return;
				}
			}
			break;
		case CASHITEM_GROUP_CHANGE_NICK:		//	닉네임 변경
			{
				INT32 itemDisguiseNick = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_NICK, CASHITEM_DAY_NO);

				//	임시 닉네임 사용중에는 사용 불가.
				if(g_pFramework->IsUsingCashItem( itemDisguiseNick))
				{
					CAPTION_POPUP(GAME_STRING("STR_POPUP_Q_MESSAGE_MODIFY_NICK_WITHOUT_DISUISE_NICK"),
								  GAME_STRING("STR_INVEN_DO_NOT_USE"));

					return;
				}
			}
			break;
		}
	}

	//	아이템 사용창 띄운다.
	OpenCashItem( ItemID);
}

void CStepInventory::CbUseCashItem( void * pThis, INT32 nParam)
{
	if( MBR_OK == nParam)
	{
		CStepInventory * pInven = ((CStepInventory*) pThis);

		pInven->OpenCashItem( pInven->m_idxDescriptionItemID);
	}
}

void CStepInventory::OpenCashItem(INT32 itemID)
{
	//	아이템 사용창 띄운다.
	m_authID = g_pFramework->GetInvenList()->GetCashItemWareDBIdx( itemID);
	if (INVALID_WAREDB_INDEX == m_authID) return;

	//	아이템을 사용할지 질문
	((CStageReady*)m_pParent)->TogglePopup(POPUP_USE_ITEM, &itemID);
}

void CStepInventory::OnDeleteItem(void)
{
	//	삭제할 아이템
	INT32 itemID = m_idxDescriptionItemID;		//	사용할 아이템
	//	아이템 사용창 띄운다.
	m_authID = g_pFramework->GetInvenList()->GetCashItemWareDBIdx( itemID);
	if (INVALID_WAREDB_INDEX == m_authID) return;

	//	아이템을 사용할지 질문
	((CStageReady*)m_pParent)->TogglePopup(POPUP_DELETE_ITEM, &itemID);
}

void CStepInventory::CbDeleteCashItem( void * pThis, INT32 nParam)
{
	if( MBR_OK == nParam)
	{
		CStepInventory * pInven = ((CStepInventory*) pThis);

		INT64 authID = g_pFramework->GetInvenList()->GetCashItemWareDBIdx( pInven->m_idxDescriptionItemID);
		if (INVALID_WAREDB_INDEX == authID) return;

		g_pGameContext->SetEvent(EVENT_AUTH_DELETE, &authID);
	}
}

void CStepInventory::_ApplyScroll()
{
	if( m_bVestigeDuty )
	{
		INT32 Index = 0;
		while( Index < m_OldidxStartSlot )
		{
			m_pWeaponScrollList->MoveNext();
			Index = m_pWeaponScrollList->GetCurrentTop() * SLOT_HORI_COUNT_WEAPON;
		}
	}
}