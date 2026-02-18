#pragma once

struct DesigSet;
class CUserDesigInfo;
struct TempUserDesigInfo;
class DesigAttr;

namespace DesigUtil
{
	bool						LoadDesignationPef( const i3::rc_string& strFileName );

	void						ClearDesigItemInfosFromServer();
	void						UpdateDesigItemInfosFromServer(INT32 begShopItemIdx, INT32 currShopItemCount );

	// 아래 2개는 상점정보패킷에 기인한 맵 정보함수들임..
	const i3::vector<UINT32>*	FindItemIDsFromDesigType( UINT8 desigSetType);
	const i3::vector<UINT8>*	FindDesigTypesFromItemID( UINT32 itemID);

	// Find쪽은 SAFE하게 처리 되고..
	const DesigSet*				FindDesigSetByType(INT32 desigSetType);
	const DesigSet*				FindDesigSetByUI( INT32 idx);

	// Get쪽은 범위 체크를 하지 않는다..
	const DesigSet*				GetDesigSetByType(INT32 desigSetType);
	const DesigSet*				GetDesigSetByUI( INT32 idx);

	INT32						GetDesigSetCountByUI();
	INT32						GetDesigSetCountByType();

	INT32						GetDesigAttrCount();
	const DesigAttr*			FindDesigAttr(INT32 desigID);
	
	// 구매 가능 여부만... (획득 호칭으로 결정됨)
	bool	CheckPurchasable_MyItemByDesig(UINT32 itemID, const i3::vector<UINT8>** outNeedDesigSetType = nullptr);
	bool	CheckPurchasable_ItemByDesig( UINT32 itemID, const CUserDesigInfo* userDesigInfo, const i3::vector<UINT8>** outNeedDesigSetType = nullptr);
	
	// 장착 가능 여부를 따짐... ( 현재 슬롯의 장착 호칭으로 결정됨)
	bool	CheckWearable_MyItemByDesig(UINT32 itemID, const i3::vector<UINT8>** outNeedDesigSetType = nullptr);
	bool	CheckWearable_ItemByDesig( UINT32 itemID, const CUserDesigInfo* userDesigInfo, const i3::vector<UINT8>** outNeedDesigSetType = nullptr);
	bool	IsItemLimitedByDesigSlot(T_ItemID itemID);

	void	AcquireGainedDesigSetsForUI( i3::vector<bool>& outGainedForUI, size_t numUIControl);

	// 서버 리시브시 꼽아 넣는 함수들...
	void Update_Recv_PROTOCOL_BASE_GET_USER_INFO_ACK(UINT64 myGainedDesigFlag, 
		const UINT8 (&desigSetTypes)[MAX_EQUIP_USER_TITLE], INT32 limitCount);
	void Update_Recv_PROTOCOL_BASE_USER_TITLE_CHANGE_ACK(INT32 limitCount);
	void Update_Recv_PROTOCOL_BASE_USER_TITLE_EQUIP_ACK();
	void Update_Recv_PROTOCOL_BASE_USER_TITLE_RELEASE_ACK();
	void Update_Recv_PROTOCOL_BASE_USER_TITLE_INFO_ACK(bool IsMyInfo, UINT64 GainedDesigFlag, 
		const UINT8 (&desigSetTypes)[MAX_EQUIP_USER_TITLE], INT32 limitCount);

	// 서버 리시브 직후 EVENT 처리에 꼽아넣는 함수들...
	void Update_Recv_EVENT_DESIGNATION_GET(i3::rc_wstring& outMessage);

	// 서버 전송시 꼽아 넣는 함수들..
	void						Update_Send_EVENT_DESIGNATION_GET(UINT8 gaindSetType); 
	void						Update_Send_EVENT_DESIGNATION_CHANGE(UINT8 slot, UINT8 desigSetType); 
	void						Update_Send_EVENT_DESIGNATION_RELEASE(UINT8 slot, UINT8& outDesigSetType); 

	//
	TempUserDesigInfo*			GetTempUserDesigInfo();
	CUserDesigInfo*				GetMyUserDesigInfo();
	//

	void EnableDesigApplyInfo(UINT8 desigSetType, DesigApplyInfo* inoutInfo);


}
