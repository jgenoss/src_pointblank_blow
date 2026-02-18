#include "pch.h"
#include "DesigUtil.h"
#include "NetCharaInfo.h"
#include "DesigInfoMgr.h"
#include "DesigAttr.h"
#include "DesigSet.h"
#include "UserDesigInfo.h"
#include "DesigApplyInfo.h"
#include "../UI/UIShopFunction.h"


namespace DesigUtil
{
	bool LoadDesignationPef( const i3::rc_string& strFileName )
	{
		return DesigInfoMgr::i()->LoadRegistryFile(strFileName);
	}

	void ClearDesigItemInfosFromServer()
	{
		DesigInfoMgr::i()->ClearDesigItemInfosFromServer();
	}

	void UpdateDesigItemInfosFromServer(INT32 begShopItemIdx, INT32 currShopItemCount)
	{
		DesigInfoMgr::i()->UpdateDesigItemInfosFromServer( begShopItemIdx, currShopItemCount);
	}

	void EnableDesigApplyInfo(UINT8 desigSetType, DesigApplyInfo* inoutInfo)
	{
		if (desigSetType == 0)		// 0이면 슬롯이 없는 것으로 간주하면 될것 같다..
			return;

		if (desigSetType >= DesigInfoMgr::i()->GetDesigSetCountByType() )
		{
			//I3PRINTLOG(I3LOG_FATAL, "%d DesigSetType doesn't exist.", desigSetType);
			return;
		}
		const DesigSet* pSet = DesigInfoMgr::i()->GetDesigSetByType(desigSetType);
		if ( pSet == nullptr)
		{
			//I3PRINTLOG(I3LOG_FATAL, "%d DesigSetType doesn't exist.", desigSetType);
			return;
		}

		inoutInfo->desigWeapon.applyType = pSet->applyType; 

		for ( INT32 i = 0 ; i < DESIG_EFFECT_COUNT ; ++i )
		{
			INT32 iPlusIdx = pSet->plusEffect[i];
			INT32 iMinusIdx = pSet->minusEffect[i];

			const DesigAttr* IDInfo_plus = DesigUtil::FindDesigAttr(iPlusIdx);
			const DesigAttr* IDInfo_minus = DesigUtil::FindDesigAttr(iMinusIdx);

			if  (IDInfo_plus)
				IDInfo_plus->AddAttr(inoutInfo);
			if  (IDInfo_minus)
				IDInfo_minus->AddAttr(inoutInfo);
		}

	}


	const i3::vector<UINT32>*	FindItemIDsFromDesigType( UINT8 desigType)
	{
		return DesigInfoMgr::i()->FindItemIDsFromDesigType(desigType);
	}

	const i3::vector<UINT8>*	FindDesigTypesFromItemID( UINT32 itemID)
	{
		return DesigInfoMgr::i()->FindDesigTypesFromItemID(itemID);
	}
	
	// desigSetType에서 0은 쓰지 않는다..
	const DesigSet*				FindDesigSetByType(INT32 desigSetType)
	{
		if ( 0 < desigSetType && desigSetType < DesigInfoMgr::i()->GetDesigSetCountByType() )
			return DesigInfoMgr::i()->GetDesigSetByType(desigSetType);
		return nullptr;
	}

	const DesigSet*				FindDesigSetByUI( INT32 idx)
	{
		if ( -1 < idx && idx < DesigInfoMgr::i()->GetDesigSetCountByUI() )
			return DesigInfoMgr::i()->GetDesigSetByUI(idx);
		return nullptr;
	}

	const DesigSet*				GetDesigSetByType(INT32 desigSetType)
	{
		return DesigInfoMgr::i()->GetDesigSetByType(desigSetType);
	}

	const DesigSet*				GetDesigSetByUI( INT32 idx)
	{
		return DesigInfoMgr::i()->GetDesigSetByUI(idx);
	}


	INT32						GetDesigSetCountByUI() 
	{
		return DesigInfoMgr::i()->GetDesigSetCountByUI();
	}

	INT32						GetDesigSetCountByType() 
	{
		return DesigInfoMgr::i()->GetDesigSetCountByType();
	}


	TempUserDesigInfo*			GetTempUserDesigInfo()
	{
		return DesigInfoMgr::i()->GetTempUserDesigInfo();
	}
	
	CUserDesigInfo*				GetMyUserDesigInfo()
	{
		return DesigInfoMgr::i()->GetMyUserDesigInfo();
	}

	INT32						GetDesigAttrCount()
	{
		return DesigInfoMgr::i()->GetDesigAttrCount();
	}

	const DesigAttr*			FindDesigAttr(INT32 desigID)
	{
		if ( -1 < desigID && desigID < DesigInfoMgr::i()->GetDesigAttrCount() )
			return DesigInfoMgr::i()->GetDesigAttr(desigID);
		return nullptr;
	}

	void						Update_Send_EVENT_DESIGNATION_GET(UINT8 gaindSetType)
	{
		TempUserDesigInfo*  tempInfo =  DesigInfoMgr::i()->GetTempUserDesigInfo();
		tempInfo->gainedDesigSetType = gaindSetType;
	}

	void						Update_Send_EVENT_DESIGNATION_CHANGE(UINT8 slot, UINT8 desigSetType)
	{
		TempUserDesigInfo*  tempInfo =  DesigInfoMgr::i()->GetTempUserDesigInfo();
		tempInfo->uploadSlotIdx = slot;
		tempInfo->uploadDesigSetType = desigSetType;
	}

	void						Update_Send_EVENT_DESIGNATION_RELEASE(UINT8 slot, UINT8& outDesigSetType)
	{
		CUserDesigInfo* myInfo = DesigInfoMgr::i()->GetMyUserDesigInfo();
		outDesigSetType = myInfo->GetEquipDesig(slot);
		TempUserDesigInfo*  tempInfo =  DesigInfoMgr::i()->GetTempUserDesigInfo();
		tempInfo->releaseSlotIdx = slot;
	}

	void						Update_Recv_PROTOCOL_BASE_GET_USER_INFO_ACK(UINT64 myGainedDesigFlag, 
		const UINT8 (&desigSetTypes)[MAX_EQUIP_USER_TITLE], INT32 limitCount)
	{
		CUserDesigInfo* myInfo = DesigInfoMgr::i()->GetMyUserDesigInfo();

		myInfo->SetGainedDesigWithFlag(myGainedDesigFlag);
		myInfo->SetEquipDesig(desigSetTypes);
		myInfo->SetLimitCount(limitCount);
	}

	void Update_Recv_PROTOCOL_BASE_USER_TITLE_INFO_ACK(bool IsMyInfo, UINT64 GainedDesigFlag, 
		const UINT8 (&desigSetTypes)[MAX_EQUIP_USER_TITLE], INT32 limitCount)
	{
		CUserDesigInfo* info = (IsMyInfo) ? DesigInfoMgr::i()->GetMyUserDesigInfo() : DesigInfoMgr::i()->GetOtherUserDesigInfo();
						
		info->SetGainedDesigWithFlag(GainedDesigFlag);
		info->SetEquipDesig(desigSetTypes);
		info->SetLimitCount(limitCount);
	}

	void						Update_Recv_PROTOCOL_BASE_USER_TITLE_CHANGE_ACK(INT32 limitCount)
	{
		CUserDesigInfo* myInfo = DesigInfoMgr::i()->GetMyUserDesigInfo();
		myInfo->SetLimitCount(limitCount);
	}

	void						Update_Recv_PROTOCOL_BASE_USER_TITLE_RELEASE_ACK()
	{
		TempUserDesigInfo*  tempInfo =  DesigInfoMgr::i()->GetTempUserDesigInfo();
		CUserDesigInfo* myInfo = DesigInfoMgr::i()->GetMyUserDesigInfo();
		myInfo->SetEquipDesig(tempInfo->releaseSlotIdx, 0);
	}

	void						Update_Recv_PROTOCOL_BASE_USER_TITLE_EQUIP_ACK()
	{
		TempUserDesigInfo*  tempInfo =  DesigInfoMgr::i()->GetTempUserDesigInfo();
		DesigInfoMgr::i()->GetMyUserDesigInfo()->SetEquipDesig(tempInfo->uploadSlotIdx, tempInfo->uploadDesigSetType);
	}

	void	Update_Recv_EVENT_DESIGNATION_GET(i3::rc_wstring& outMessage)
	{
		CUserDesigInfo* myInfo = DesigInfoMgr::i()->GetMyUserDesigInfo();
		TempUserDesigInfo*  tempInfo =  DesigInfoMgr::i()->GetTempUserDesigInfo();
		myInfo->SetGainedDesig(tempInfo->gainedDesigSetType, true);

		const i3::vector<UINT32>* pvecItemID = 	FindItemIDsFromDesigType(tempInfo->gainedDesigSetType);
		
		if (pvecItemID && !pvecItemID->empty() )
		{
			const i3::vector<UINT32>& vecItemID = *pvecItemID;
			
			i3::stack_wstring wstrTemp = GAME_RCSTRING("STR_TBL_NETWORK_SUCESS_GET_DESIGNATION_WITH_PRIZE"); /*호칭 획득에 성공하여 보상이 지급되었습니다.*/
			wstrTemp += L'\n';
			
			i3::rc_wstring wstrShopItemName;
			
			const size_t num_itemID = vecItemID.size();
			bool  includeItem = false;

			for (size_t i = 0 ; i < num_itemID ; ++i)
			{
				usf::GetShopItemName( wstrShopItemName, vecItemID[i]);		// 상품 이름 존재시에...

				if ( !wstrShopItemName.empty() )
				{
					wstrTemp += wstrShopItemName;
					wstrTemp += L' ';
					includeItem = true;
				}
			}
			
			if (includeItem)
				wstrTemp[ wstrTemp.size()- 1 ] = L'\n';			// 마지막에 ' ' 대신 '\n'으로 변경..
			
			wstrTemp += GAME_RCSTRING("STR_POPUP_NOTICE_DESIGNATION_GET_WITH_ITEM");	/*(인벤토리에서 확인하실 수 있습니다.)*/
			outMessage = wstrTemp;
		}
		else
		{
			outMessage = GAME_RCSTRING("STR_TBL_NETWORK_SUCESS_GET_DESIGNATION"); /*호칭 획득에 성공하였습니다.*/
		}
	}

	bool	CheckPurchasable_MyItemByDesig(UINT32 itemID, const i3::vector<UINT8>** outNeedDesigSetType)
	{
		return CheckPurchasable_ItemByDesig(itemID, DesigInfoMgr::i()->GetMyUserDesigInfo(), outNeedDesigSetType);
	}

	bool	CheckPurchasable_ItemByDesig( UINT32 itemID, const CUserDesigInfo* userDesigInfo, const i3::vector<UINT8>** outNeedDesigSetType)
	{
		const i3::vector<UINT8>* pvecDesigTypes = DesigUtil::FindDesigTypesFromItemID(itemID);

		if (outNeedDesigSetType)
			*outNeedDesigSetType = pvecDesigTypes;

		if (pvecDesigTypes == nullptr || pvecDesigTypes->empty() )
			return true;										// 호칭 아이템 아니면 구매 가능..
		
		const i3::vector<UINT8>& vecDesigTypes = *pvecDesigTypes;

		const size_t numDesigOfItem = vecDesigTypes.size();

		for (size_t i = 0 ; i < numDesigOfItem ; ++i)
		{
			if ( userDesigInfo->IsGainedDesig( vecDesigTypes[i] ) )
				return true;
		}
		return false;
	}

	bool	CheckWearable_MyItemByDesig(UINT32 itemID, const i3::vector<UINT8>** outNeedDesigSetType )
	{
		return CheckWearable_ItemByDesig(itemID, DesigInfoMgr::i()->GetMyUserDesigInfo(), outNeedDesigSetType);
	}

	bool	CheckWearable_ItemByDesig( UINT32 itemID, const CUserDesigInfo* userDesigInfo, const i3::vector<UINT8>** outNeedDesigSetType)
	{
		bool isItemLimitedByDesigSlot = DesigUtil::IsItemLimitedByDesigSlot(itemID);
		
		if ( isItemLimitedByDesigSlot == false)		// pef에 호칭슬롯제한여부가 false라면 바로 리턴 가능..
		{
			if (outNeedDesigSetType == nullptr)	
				return true;
		}

		const i3::vector<UINT8>* pvecDesigTypes = DesigUtil::FindDesigTypesFromItemID(itemID);
		if (outNeedDesigSetType)
			*outNeedDesigSetType = pvecDesigTypes;
		
		if ( isItemLimitedByDesigSlot == false)
			return true;

		if (pvecDesigTypes == nullptr || pvecDesigTypes->empty() )
			return true;										// 호칭 아이템 아니면 장착 가능..

		const i3::vector<UINT8>& vecDesigTypes = *pvecDesigTypes;
		const size_t numDesigOfItem = vecDesigTypes.size();

		for (size_t i = 0; i <numDesigOfItem ; ++i)
		{
			if (userDesigInfo->FindDesigEquipSlot(vecDesigTypes[i]) != -1)
				return true;									// 슬롯에 들어있는 호칭이면 장착 가능..
		}
		
		return false;
	}

	void	AcquireGainedDesigSetsForUI( i3::vector<bool>& outGainedForUI, size_t numUIControl)
	{
		outGainedForUI.resize(numUIControl);
		
		const size_t numDesigUI = DesigInfoMgr::i()->GetDesigSetCountByUI();

		const size_t numGainedForUI = 
			outGainedForUI.size() < numDesigUI ? outGainedForUI.size() : numDesigUI;
		
		UINT8 desigSetTypes_MyInfo[MAX_USER_TITLE];
		DesigInfoMgr::i()->GetMyUserDesigInfo()->GetGainedDesig(desigSetTypes_MyInfo);

		for (size_t i = 0 ; i < numGainedForUI; ++i)
		{
			const DesigSet* desigSet = DesigInfoMgr::i()->GetDesigSetByUI(i);			
			
			if (desigSet->desigSetType > 0  && desigSet->desigSetType < MAX_USER_TITLE)
			{
				if (desigSetTypes_MyInfo[desigSet->desigSetType] == 1)
				{
					outGainedForUI[i] = true;	continue;
				}
			}
			outGainedForUI[i] = false;
		}
	}

	bool	IsItemLimitedByDesigSlot(T_ItemID itemID)
	{
		ITEM_TYPE itemClass = (ITEM_TYPE) GET_ITEM_TYPE(itemID);

		switch(itemClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
		{
			CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(itemID);
			if (pInfo == nullptr)
				return false;

			return pInfo->IsLimitedByDesigSlot();
		}
		break;

		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
		case ITEM_TYPE_FACEGEAR:
		case ITEM_TYPE_UPPER:
		case ITEM_TYPE_LOWER:
		case ITEM_TYPE_GLOVE:
		case ITEM_TYPE_BELT:
		case ITEM_TYPE_HOLSTER:
		case ITEM_TYPE_SKIN	:
		case ITEM_TYPE_BERET :
		{
			const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo(itemID);
			if (pInfo == nullptr)
				return false;
			return pInfo->IsLimitedByDesigSlot();
		}
		break;
		default:
		break;
		}
			
		return false;
	}


}
