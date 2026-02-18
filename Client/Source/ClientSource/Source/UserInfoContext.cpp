#include "pch.h"
#include "UserInfoContext.h"

#include "TimeUtil.h"
#include "ui/Batmanager.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"


UserInfoContext::UserInfoContext() : m_InvenServerTime (0), m_iAge(0), 	m_ui32LoginCnt(0), 	m_ReceivedMyInfo(false), 
m_PCCafePoint(0), m_PCCafeExp(0), m_bEnable_GMObserver(false), m_gmObserverViewFlag(0), 
m_bInitialRankUpFlag(false), m_bGeneralRankUpFlag(false), m_bViewHUDNick(true), m_nMyUsurpationIdx(255), m_LastGameWeapon(0), m_nMaxMultiWeapon(1)
, m_bReceivedUserInfo(false), m_bCreateNickName(false)
{
	m_MyUICharaInfo = new UI_CHARA_INFO;
	m_MyUserInfoInvItemData = new USER_INFO_INVITEM_DATA; 
	m_MyUserInfoBasic = new USER_INFO_BASIC;
	m_MyUserInfoRecord = new USER_INFO_RECORD; 
	m_MyUserInfoEquipmentWeapon = new USER_INFO_EQUIPMENT_WEAPON;

	MEMDUMP_NEW( m_MyUserInfoInvItemData, sizeof(USER_INFO_INVITEM_DATA));
	MEMDUMP_NEW( m_MyUserInfoBasic, sizeof(USER_INFO_BASIC));
	MEMDUMP_NEW( m_MyUserInfoRecord, sizeof(USER_INFO_RECORD));
	MEMDUMP_NEW( m_MyUserInfoEquipmentWeapon, sizeof(USER_INFO_EQUIPMENT_WEAPON));
	
	m_MyUICharaInfo->Reset();
	m_MyUserInfoInvItemData->Reset();
	m_MyUserInfoBasic->Reset();
	m_MyUserInfoRecord->Reset();
	m_MyUserInfoEquipmentWeapon->Reset();


#if defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG) || defined( I3_RELEASE_PROFILE)//주석으로 되어 있는 디버그
	////	디버그용 필요함...지우지 마셈 :)
	ITEM_TYPE	Class[6] = {};
	Class[0] = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)g_DebugWeaponSet[0][0]);
	Class[1] = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)g_DebugWeaponSet[1][0]);
	Class[2] = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)g_DebugWeaponSet[2][0]);
	Class[3] = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)g_DebugWeaponSet[3][0]);
	Class[4] = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)g_DebugWeaponSet[4][0]);
	Class[5] = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)g_DebugWeaponSet[5][0]);

	SetMyUICharaInfo()->setWeaponItemID( WEAPON_SLOT_PRIMARY, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[0], g_DebugWeaponSet[0][0], g_DebugWeaponSet[0][1]));		// (g_DebugWeaponSet[0][0] << 8) | g_DebugWeaponSet[0][1];
	SetMyUICharaInfo()->setWeaponItemID( WEAPON_SLOT_SECONDARY, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[1], g_DebugWeaponSet[1][0], g_DebugWeaponSet[1][1]));		//(1 << 16) | (g_DebugWeaponSet[1][0] << 8) | g_DebugWeaponSet[1][1];
	SetMyUICharaInfo()->setWeaponItemID( WEAPON_SLOT_MELEE, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[2], g_DebugWeaponSet[2][0], g_DebugWeaponSet[2][1]));		//(2 << 16) | (g_DebugWeaponSet[2][0] << 8) | g_DebugWeaponSet[2][1];
	SetMyUICharaInfo()->setWeaponItemID( WEAPON_SLOT_THROWING1, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[3], g_DebugWeaponSet[3][0], g_DebugWeaponSet[3][1]));		//(3 << 16) | (g_DebugWeaponSet[3][0] << 8) | g_DebugWeaponSet[3][1];
	SetMyUICharaInfo()->setWeaponItemID( WEAPON_SLOT_THROWING2, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[4], g_DebugWeaponSet[4][0], g_DebugWeaponSet[4][1]));		//(4 << 16) | (g_DebugWeaponSet[4][0] << 8) | g_DebugWeaponSet[4][1];
	SetMyUICharaInfo()->setWeaponItemID( WEAPON_SLOT_MISSION, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[5], g_DebugWeaponSet[5][0], g_DebugWeaponSet[5][1]));		//(4 << 16) | (g_DebugWeaponSet[4][0] << 8) | g_DebugWeaponSet[4][1];
	
#endif

	m_eAuthLevel = AUTH_LEVEL_NONE;
	m_eSpecialAbility = SA_NONE;

	m_rankUpRank = 0;
	m_rankUpPoint = 0;

	receive_user_info_daily = false;
	m_nMyRespawnCnt =0;

	m_ui8NationalCode = 0;

	resetRankUpItemList();

	m_enableTsReward = false;
	m_enableEventReward = false;
	receive_daily_gift_gauge = false;

	m_bEnableNewRepair		= false;
	m_bDefaultItemChanged	= false;

	m_ContentsFlag.Reset();

	m_GeneralRank_ItemInfo.Reset();
	m_bGeneralRank_MsgBoxFlag = false;
	m_bGeneralRank_RecvDataFlag = false;

	m_showTsRewardMsg = false;
}

UserInfoContext::~UserInfoContext()
{
	I3_SAFE_DELETE( m_MyUserInfoRecord);
	I3_SAFE_DELETE( m_MyUserInfoBasic);
	I3_SAFE_DELETE( m_MyUserInfoInvItemData);
	I3_SAFE_DELETE( m_MyUICharaInfo);
	I3_SAFE_DELETE( m_MyUserInfoEquipmentWeapon);
}

void UserInfoContext::ResetForSingleMode( void)
{
	m_eSpecialAbility = SA_CHEAT;
}

void UserInfoContext::SetMyUserInfoBasic( const USER_INFO_BASIC & UserInfoBasic )
{
	*m_MyUserInfoBasic = UserInfoBasic;
	GlobalFunc::EncryptValue(m_MyUserInfoBasic, sizeof(*m_MyUserInfoBasic), ENC_SEED_USERINFOBASIC);
}

void UserInfoContext::GetMyUserInfoBasic( USER_INFO_BASIC * UserInfoBasic ) const
{
	*UserInfoBasic = *m_MyUserInfoBasic;
	GlobalFunc::DecryptValue(UserInfoBasic, sizeof(*m_MyUserInfoBasic),  ENC_SEED_USERINFOBASIC);
}

void UserInfoContext::SetMyUserInfoInvItemData( const USER_INFO_INVITEM_DATA & Data )
{
	*m_MyUserInfoInvItemData = Data;
	GlobalFunc::EncryptValue(m_MyUserInfoInvItemData, sizeof(*m_MyUserInfoInvItemData), ENC_SEED_USERINFOINVEN);
}

void UserInfoContext::GetMyUserInfoInvItemData( USER_INFO_INVITEM_DATA * pData ) const
{
	*pData = *m_MyUserInfoInvItemData;
	GlobalFunc::DecryptValue(pData, sizeof(*m_MyUserInfoInvItemData), ENC_SEED_USERINFOINVEN);
}

void UserInfoContext::SetMyUserInfoEquipmentWeapon( const USER_INFO_EQUIPMENT_WEAPON & InfoEquipWeapon )
{
	*m_MyUserInfoEquipmentWeapon = InfoEquipWeapon;
	GlobalFunc::EncryptValue(m_MyUserInfoEquipmentWeapon, sizeof(*m_MyUserInfoEquipmentWeapon), ENC_SEED_USERINFOWEAPON);

	SetMyUICharaInfo()->setWeaponItemID( WEAPON_SLOT_PRIMARY,		InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_PRIMARY ], -1, __FILE__, __LINE__ );
	SetMyUICharaInfo()->setWeaponItemID( WEAPON_SLOT_SECONDARY,	InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_SECONDARY ], -1, __FILE__, __LINE__ );
	SetMyUICharaInfo()->setWeaponItemID( WEAPON_SLOT_MELEE,		InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_MELEE ], -1, __FILE__, __LINE__ );
	SetMyUICharaInfo()->setWeaponItemID( WEAPON_SLOT_THROWING1,	InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_THROWING1 ], -1, __FILE__, __LINE__ );
	SetMyUICharaInfo()->setWeaponItemID( WEAPON_SLOT_THROWING2,	InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_THROWING2 ], -1, __FILE__, __LINE__ );
}

void UserInfoContext::GetMyUserInfoEquipmentWeapon( USER_INFO_EQUIPMENT_WEAPON & InfoEquipWeapon ) const
{
	InfoEquipWeapon = *m_MyUserInfoEquipmentWeapon;
	GlobalFunc::DecryptValue(&InfoEquipWeapon, sizeof(InfoEquipWeapon), ENC_SEED_USERINFOWEAPON);
}

UINT32 UserInfoContext::GetInventorySecTime( UINT32 ExpireTime ) const
{
	return TimeUtil::CalcTimeDifference(m_InvenServerTime, ExpireTime);
}

bool	UserInfoContext::IsAdultUser(void) const
{
	return (g_pConfig->m_bNetworking ? g_pConfigEx->GetMisc().AdultAge <= m_iAge : true);
}


/// 아 이 함수 최악이네...
i3::rc_wstring UserInfoContext::GetMyNickName() const
{
	USER_INFO_BASIC MyUserInfo;
	UserInfoContext::GetMyUserInfoBasic(&MyUserInfo);

	i3::rc_wstring wstrMyNickName = MyUserInfo.m_strNick;

	return wstrMyNickName;
}


UINT32 UserInfoContext::GetMyRank() const
{
	USER_INFO_BASIC MyUserInfo;
	UserInfoContext::GetMyUserInfoBasic(&MyUserInfo);
	return MyUserInfo.m_ui32Rank;
}

UINT32 UserInfoContext::GetMyClanState() const
{
	USER_INFO_BASIC MyUserInfo;
	UserInfoContext::GetMyUserInfoBasic(&MyUserInfo);
	return MyUserInfo.m_ui32ClanState;
}

UINT32 UserInfoContext::GetMyPoint() const
{
	USER_INFO_BASIC MyUserInfo;
	UserInfoContext::GetMyUserInfoBasic(&MyUserInfo);
	return MyUserInfo.m_ui32Point;
}

UINT32 UserInfoContext::GetMyCash() const
{
	USER_INFO_BASIC MyUserInfo;
	UserInfoContext::GetMyUserInfoBasic(&MyUserInfo);

	// 한국일 경우
	// _cash는 _NCCoin + _HappyCoin 값으로 들어있다
	return MyUserInfo.m_ui32Cash;
}


void UserInfoContext::SetMyNickName( const i3::wliteral_range& wstrName )
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::GetMyUserInfoBasic(&MyUserInfoBasic);

	i3::safe_string_copy(MyUserInfoBasic.m_strNick, wstrName, NET_NICK_NAME_SIZE);

	UserInfoContext::SetMyUserInfoBasic(MyUserInfoBasic);
}

void	UserInfoContext::SetMyNickName(const char* szName)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::GetMyUserInfoBasic(&MyUserInfoBasic);

	i3::mb_to_utf16(szName, MyUserInfoBasic.m_strNick, NET_NICK_NAME_SIZE);

	UserInfoContext::SetMyUserInfoBasic(MyUserInfoBasic);
}


void UserInfoContext::SetMyPoint( UINT32 val )
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::GetMyUserInfoBasic(&MyUserInfoBasic);
	MyUserInfoBasic.m_ui32Point = val;
	UserInfoContext::SetMyUserInfoBasic(MyUserInfoBasic);
}

void UserInfoContext::SetMyCash( UINT32 val )
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::GetMyUserInfoBasic(&MyUserInfoBasic);
	MyUserInfoBasic.m_ui32Cash = val;
	UserInfoContext::SetMyUserInfoBasic(MyUserInfoBasic);
}

void UserInfoContext::SetMyNCSACoin(UINT32 ui32NCCoin,UINT32 ui32HappyCoin)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::GetMyUserInfoBasic(&MyUserInfoBasic);
	MyUserInfoBasic.m_ui32NCCoin = ui32NCCoin;
	MyUserInfoBasic.m_ui32HappyCoin = ui32HappyCoin;
	UserInfoContext::SetMyUserInfoBasic(MyUserInfoBasic);
}

void UserInfoContext::GetMyNCSACoin(UINT32 * ui32NCCoin,UINT32 * ui32HappyCoin)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::GetMyUserInfoBasic(&MyUserInfoBasic);
	*ui32NCCoin = MyUserInfoBasic.m_ui32NCCoin;
	*ui32HappyCoin = MyUserInfoBasic.m_ui32HappyCoin;
	UserInfoContext::SetMyUserInfoBasic(MyUserInfoBasic);
}



bool	UserInfoContext::IsMyNickName(const i3::wliteral_range& Name) const		
{
	return i3::generic_is_iequal(Name, GetMyNickName());		// GetMyNickName함수 퍼포먼스 문제..(2014.07.11.수빈)
}

bool	UserInfoContext::HaveNickName() const							
{
	return 0 < GetMyNickName().length();
		//i3::generic_string_size(GetMyNickName().c_str()); 
}


UINT32 UserInfoContext::GetRealRank() const
{
	return GetMyRank();
}

INT32	UserInfoContext::GetMaxExpByRank( INT32 nRank )
{	
	if( 0 <= nRank && nRank < RANK_NEWBIE )
	{
		if (g_pEnvSet->IsV2Version())
			return g_ExpValueV2[nRank];
		else
			return g_ExpValueV1[nRank];
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  "invalid rank type.... CGameContext::GetExpMaxByRank" );
		return 0;
	}
}

bool UserInfoContext::IsGM_Observer() const
{
	if( BaTMaN::use_batman )
	{
		if(BaTMaN::gm_observer_mode == true) return true;
	}
	
	return ( m_bEnable_GMObserver && CheckAuthLevel() );
}

DWORD UserInfoContext::GetGMObserverViewFlag() const
{
	if(!IsGM_Observer())
		return 0;

	return m_gmObserverViewFlag;
}
 
void	UserInfoContext::SetDailyGiftGaugeArray(UINT32 *src )
{
	::memcpy(m_ui32DailyGiftGauge, src, sizeof(UINT32) * DAILY_GIFT_TYPE_MAX );
	receive_daily_gift_gauge = true;
}

void	UserInfoContext::SetBillingURL(const i3::rc_wstring& wstrBillingURL)
{
	m_wstrBillingURL = wstrBillingURL;	
}

bool	UserInfoContext::IsChargeCashEnable(void)
{
	if( IsContentsEndabled(CONTENTS_CASH_RECHARGE) == false)
		return false;

	if( i3::generic_string_size( GetBillingURL() ) <= 0 )
		return false;

	return true;
}

bool UserInfoContext::IsAprilFoolsDay()
{
	if(CInvenList::i()->FindCashItem(CASHITEM_ITEMID_DISGUISE_RANK_APRIL_FOOL_CLIENT))
		return true;

	return false;
}

void UserInfoContext::SetGeneralRankInfo( INVEN_BUFFER* buf)
{
	if(buf != nullptr)
	{
		if (buf[0]._ui32ItemID == 0)
		{
			// 장성급베레모. 유저가 강등당해서 베레모를 삭제해야할 경우 ItemID가 0으로 들어오기 때문에
			// 이를 강제로 삭제하기 위해서는 ItemID가 필요합니다. 문제시 말씀해주세요. gyungju.kim
			buf[0]._ui32ItemID = 701147;
		}
		m_GeneralRank_ItemInfo._ui32ItemID = buf[0]._ui32ItemID;
		m_GeneralRank_ItemInfo._TItemWareDBIdx = buf[0]._TItemWareDBIdx;
		m_bGeneralRank_MsgBoxFlag = true;
	}
	else
		m_bGeneralRank_MsgBoxFlag = false;
}