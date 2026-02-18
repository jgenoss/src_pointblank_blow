#if !defined( __GAME_EVENT_HANDLER_H__)
#define __GAME_EVENT_HANDLER_H__

#include "GameDefine.h"

typedef unsigned int uint32;

struct GAMEEVENTINFO
{
	S2MOPacketBase*		m_S2MOPacket = nullptr;

	i3NetworkPacket		_SendPacket;
	const void *		_pArg = nullptr;
	const void *		_pArg1 = nullptr;
	const void *		_pArg2 = nullptr;
	LPCSTR				_szFile = nullptr;
	INT32				_nLine = 0;
};

struct QA_COMMAND_TELEPORT_DATA
{
	INT32 _i32SlotIdx = 0;
	REAL32 _ar32Pos[3] = { 0.0f };
};

class CGameEventHandler : public i3ElementBase
{
	I3_CLASS_DEFINE( CGameEventHandler, i3ElementBase);

private:
	/** \brief EventHandler Callback Function
		\return true 이면 send TCP*/
	typedef bool	(CGameEventHandler::*func)( GAMEEVENTINFO * pInfo);

	struct SEND_EVENT
	{
		func				m_fn = nullptr;
		S2MOPacketBase*		m_pS2MOPacket = nullptr;
	};

	REAL32		m_ReqMoneyTime = 0.0f;

public:
	CGameEventHandler();
	~CGameEventHandler();

	void		regSendEvent( GAME_EVENT event, func fn, S2MOPacketBase* pS2MOBase = nullptr );
	void		SendEvent( GAME_EVENT event, const void * pArg = nullptr, const void * pArg1 = nullptr, const void * pArg2 = nullptr, LPCSTR szFile = nullptr, int nLine = 0);

private:

	void		_CleanS2MO( i3::unordered_map< GAME_EVENT, SEND_EVENT >* pMap );

	//======================================================================================//
	//										Global Event									//
	//======================================================================================//
	i3::unordered_map< GAME_EVENT, SEND_EVENT >	m_eventMap_Base;
	const char *__getBaseEventName( GAME_EVENT evt) const;
	void		__register_Base( void);
	bool		__evtLogin( GAMEEVENTINFO * pInfo);
	bool		__evtLoginWebkey( GAMEEVENTINFO * pInfo);
	bool		__evtLogout( GAMEEVENTINFO * pInfo);
	bool		__evtDisconnect( GAMEEVENTINFO * pInfo);
	bool		__evtCheckRunParam( GAMEEVENTINFO * pInfo);
	bool		__evtRequestChannelList( GAMEEVENTINFO * pInfo);
	bool		__evtSelectChannel( GAMEEVENTINFO * pInfo);
	bool		__evtEnterLobby( GAMEEVENTINFO * pInfo);
	bool		__evtLeaveLobby( GAMEEVENTINFO * pInfo);
	bool		__evtEnterMyProfile( GAMEEVENTINFO * pInfo);
	bool		__evtLeaveMyProfile( GAMEEVENTINFO * pInfo);
	bool		__evtEnterShop( GAMEEVENTINFO * pInfo);
	bool		__evtLeaveShop( GAMEEVENTINFO * pInfo);
	bool		__evtConnectGatewayServer( GAMEEVENTINFO * pInfo);
	bool		__evtConnectGameServer( GAMEEVENTINFO * pInfo);
	bool		__evtChangeMainCharacterFromRoom( GAMEEVENTINFO * pInfo);
	bool		__evtGlobal_TimeoutHost( GAMEEVENTINFO * pInfo);
	bool		__evtGlobal_TimeoutClient( GAMEEVENTINFO * pInfo);
	bool		__evtGlobal_GameGuardAuth2( GAMEEVENTINFO * pInfo);
	bool		__evtGlobal_GameGuardAuth3( GAMEEVENTINFO * pInfo);
	bool		__evtGlobal_XTrapAuth( GAMEEVENTINFO * pInfo);
	bool		__evtGlobal_GetMyOption( GAMEEVENTINFO * pInfo);
	bool		__evtGlobal_GetUserInfo( GAMEEVENTINFO * pInfo);
	bool		__evtLeaveUser( GAMEEVENTINFO * pInfo);
	bool		__evtEnterUser( GAMEEVENTINFO * pInfo);
	bool		__evtEnterPassword( GAMEEVENTINFO * pInfo);
	bool		__evtSaveOption( GAMEEVENTINFO * pInfo);
	//bool		__evtSaveOptionMacro( GAMEEVENTINFO * pInfo);
	bool		__evtGlobal_Chat( GAMEEVENTINFO * pInfo);
	bool		__evtGlobal_EndQuest( GAMEEVENTINFO * pInfo);
	bool		__evtBuyQuestCardset( GAMEEVENTINFO * pInfo);
	bool		__evtDeleteQuestCardset( GAMEEVENTINFO * pInfo);
	//bool		__evtAttendanceInfo( GAMEEVENTINFO * pInfo);
	bool		__evtAttendanceCheck( GAMEEVENTINFO * pInfo);
	bool		__evtAttendanceClear( GAMEEVENTINFO * pInfo);
	bool		__evtQuest_ChangeActive( GAMEEVENTINFO * pInfo);
	bool		__evtGM_KickUserFromRoom( GAMEEVENTINFO * pInfo);
	bool		__evtGM_ExitUserFromRoom( GAMEEVENTINFO * pInfo);
	bool		__evtGM_BlockUserFromRoom( GAMEEVENTINFO * pInfo);
	bool		__evtGM_DestroyRoom( GAMEEVENTINFO * pInfo);
	bool		__evtGM_ExitUserFromLobby( GAMEEVENTINFO * pInfo);
	bool		__evtAuth_FCMInfo( GAMEEVENTINFO * pInfo);
	bool		__evtGuard_CheckStageCodename( GAMEEVENTINFO * pInfo);
	bool		__evtGuard_KickUser( GAMEEVENTINFO * pInfo);
	bool		__evtUser_DetailInfo( GAMEEVENTINFO * pInfo);
	bool		__evtGetUserInfo( GAMEEVENTINFO * pInfo);
	//bool		__evtGetCharaInfo( GAMEEVENTINFO * pInfo);
	bool		__evtGetInventoryInfo( GAMEEVENTINFO * pInfo);
	bool		__evtGetSystemInfo( GAMEEVENTINFO * pInfo);
	bool		__evtCreateCharacter( GAMEEVENTINFO * pInfo);
	bool		__evtChangeCharacterName( GAMEEVENTINFO * pInfo);
	bool		__evtChangeCharacterEquip( GAMEEVENTINFO * pInfo);
	bool		__evtSkill_Characterset( GAMEEVENTINFO * pInfo);
	bool		__evtSkill_Classset( GAMEEVENTINFO * pInfo);
	bool		__evtSkill_Learn( GAMEEVENTINFO * pInfo);
	bool		__evtDailyRecordInfo( GAMEEVENTINFO * pInfo);
	bool		__evtFindSession_UserBasicInfo( GAMEEVENTINFO * pInfo);
	bool		__evtFindSession_UserSubtaskInfo( GAMEEVENTINFO * pInfo);
	bool		__evtFindUID_UserBasicInfo( GAMEEVENTINFO * pInfo);
	bool		__evtFindNick_UserBasicInfo( GAMEEVENTINFO * pInfo);
	bool		__evtFindRoom_UserBasicInfo( GAMEEVENTINFO * pInfo);
	bool		__evtFindUID_UserDetailInfo( GAMEEVENTINFO * pInfo);
	bool		__evtUserGuide(GAMEEVENTINFO * pInfo);
	bool		__evtGetStage(GAMEEVENTINFO * pInfo);

	//클라이언트 비정상 종료시 로그
	//bool		__evtLog_AbnormalExit(GAMEEVENTINFO * pInfo);

	// 미션카드 --> 베이스에 넣음..
	bool		__evt_EVENT_QUEST_CHANGE_ACTIVE_IDX(GAMEEVENTINFO* pInfo);
	// 호칭 --> 베이스에 넣음..
	bool		__evt_EVENT_DESIGNATION_GET(GAMEEVENTINFO * pInfo); 
	bool		__evt_EVENT_DESIGNATION_CHANGE(GAMEEVENTINFO * pInfo); 
	bool		__evt_EVENT_DESIGNATION_RELEASE(GAMEEVENTINFO * pInfo); 

	//배너 url 갱신 요청
	bool		__evtRequestIngameURL(GAMEEVENTINFO * pInfo);

	//======================================================================================//
	//										GM Option										//
	//======================================================================================//
	bool		__evtGMAuthority( GAMEEVENTINFO * pInfo);
	bool		__evtSendMessage( GAMEEVENTINFO * pInfo);
	bool		__evtPenaltyApply( GAMEEVENTINFO * pInfo);
	bool		__evtGMAuthority_Popup_Close( GAMEEVENTINFO * pInfo);

	//////////////////////////////////////////////////////////////////////////////////////////

	// Medal __evt function
	bool		__evtMedal_Notify_Info( GAMEEVENTINFO * pInfo);
	bool		__evtMedal_Normal_Info( GAMEEVENTINFO * pInfo);
	bool		__evtMedal_Set_Cur_Info( GAMEEVENTINFO * pInfo);
	bool		__evtMedal_Set_Com_Info( GAMEEVENTINFO * pInfo);
	bool		__evtMedal_Get_Reward( GAMEEVENTINFO * pInfo);
	bool		__evtMedal_Notify_Register( GAMEEVENTINFO * pInfo);

	bool		__evtMedal_Command( GAMEEVENTINFO * pInfo);

	bool		__evtDeleteChara( GAMEEVENTINFO * pInfo);

	bool		__evtCheat_RandomBox( GAMEEVENTINFO * pInfo);
	//======================================================================================//
	//										QA Command										//
	//======================================================================================//
	bool		__evtQACommandIncreaseKillCount( GAMEEVENTINFO * pInfo);
	bool		__evtQACommandIncreasePlayTime( GAMEEVENTINFO * pInfo);
	bool		__evtQACommandEnableSoloPlay( GAMEEVENTINFO * pInfo);
	bool		__evtQACommandTeleportSomeone( GAMEEVENTINFO * pInfo);
	bool		__evtQACommandDamageToObject( GAMEEVENTINFO * pInfo);
	bool		__evtQACommandDetectHackOff( GAMEEVENTINFO * pInfo);
	bool		__evtQACommandTimerGMPause( GAMEEVENTINFO * pInfo);
	bool		__evtQACommandControlRoundTime( GAMEEVENTINFO * pInfo);
	bool		__evtQACommandClanWarMatchingPoint( GAMEEVENTINFO * pInfo);
	bool		__evtQACommandClanWarMatchResult( GAMEEVENTINFO * pInfo);

	//======================================================================================//
	//										Shop Event										//
	//======================================================================================//
	i3::unordered_map< GAME_EVENT, SEND_EVENT >	m_eventMap_Shop;
	const char * __getShopEventName( GAME_EVENT evt) const;
	void		__register_Shop( void);
	bool		__evtShop_AuthItem( GAMEEVENTINFO * pInfo);
	bool		__evtShop_AuthItemData( GAMEEVENTINFO * pInfo);
	bool		__evtShop_DeleteAuth( GAMEEVENTINFO * pInfo);
	bool		__evtShop_CheckNickForUseChangeNickItem( GAMEEVENTINFO * pInfo);
	bool		__evtShop_RequestGoodsList( GAMEEVENTINFO * pInfo);
	bool		__evtShop_BuyGoods( GAMEEVENTINFO * pInfo);
	bool		__evtShop_BuyCharGoods( GAMEEVENTINFO * pInfo);
	bool		__evtShop_BuyExtendGoods( GAMEEVENTINFO * pInfo);
	bool		__evtShop_RepairItem( GAMEEVENTINFO * pInfo);
	bool		__evtShop_GetRemainMoney( GAMEEVENTINFO * pInfo);
	bool		__evtShop_AwardCoupon( GAMEEVENTINFO * pInfo);
	bool		__evtShop_ChangeItemData(GAMEEVENTINFO * pInfo);

	bool		__evtGiftShop_Enter( GAMEEVENTINFO * pInfo);
	bool		__evtGiftShop_Leave( GAMEEVENTINFO * pInfo);
	bool		__evtGiftShop_Enter_Fail(GAMEEVENTINFO * pInfo);
	
	bool		__evtGiftShop_GetRecvList( GAMEEVENTINFO * pInfo);
	bool		__evtGiftShop_SendGoods( GAMEEVENTINFO * pInfo);
	bool		__evtGiftShop_AuthGoods( GAMEEVENTINFO * pInfo);

	//======================================================================================//
	//									Inventory Event										//
	//======================================================================================//
	i3::unordered_map< GAME_EVENT, SEND_EVENT > m_eventMap_Inven;
	const char *__getInventoryEventName( GAME_EVENT evt) const;
	void		__register_Inventory( void);
	bool		__evtInven_Enter( GAMEEVENTINFO * pInfo);
	bool		__evtInven_Leave( GAMEEVENTINFO * pInfo);
	bool		__evtInven_ChangeEquip( GAMEEVENTINFO * pInfo);

	//======================================================================================//
	//									Community Event										//
	//======================================================================================//
	i3::unordered_map< GAME_EVENT, SEND_EVENT >	m_eventMap_Community;
	const char *__getCommunityEventName( GAME_EVENT evt) const;
	void		__register_Community( void);
	bool		__evtComm_AddFriend( GAMEEVENTINFO * pInfo);
	bool		__evtComm_AcceptFriend( GAMEEVENTINFO * pInfo);
	bool		__evtComm_DeleteFriend( GAMEEVENTINFO * pInfo);
	bool		__evtComm_InviteFriend( GAMEEVENTINFO * pInfo);
	bool		__evtComm_InviteClan( GAMEEVENTINFO * pInfo);
	bool		__evtComm_AddBlock( GAMEEVENTINFO * pInfo);
	bool		__evtComm_DeleteBlock( GAMEEVENTINFO * pInfo);
	bool		__evtComm_SendWhisper( GAMEEVENTINFO * pInfo);
	bool		__evtComm_SendWhisperWithUID( GAMEEVENTINFO * pInfo);
	bool		__evtComm_SendMegaPhone( GAMEEVENTINFO * pInfo);
	bool		__evtComm_FindUser( GAMEEVENTINFO * pInfo);
	bool		__evtComm_SendNote( GAMEEVENTINFO * pInfo);
	bool		__evtComm_SendNoteWithUID( GAMEEVENTINFO * pInfo);
	bool		__evtComm_CheckReadedNote( GAMEEVENTINFO * pInfo);
	bool		__evtComm_DeleteNote( GAMEEVENTINFO * pInfo);
	bool		__evtComm_RecordDB( GAMEEVENTINFO * pInfo);
	bool		__evtComm_RecordSession( GAMEEVENTINFO * pInfo);
	bool		__evtComm_InviteNormalUser( GAMEEVENTINFO * pInfo);

	//======================================================================================//
	//										Lobby Event										//
	//======================================================================================//
	i3::unordered_map< GAME_EVENT, SEND_EVENT >	m_eventMap_Lobby;
	const char * __getLobbyEventName( GAME_EVENT evt) const;
	void		__register_Lobby( void);
	bool		__evtLobby_CreateNickname( GAMEEVENTINFO * pInfo);
	bool		__evtLobby_CreateRoom( GAMEEVENTINFO * pInfo);
	bool		__evtLobby_JoinRoom( GAMEEVENTINFO * pInfo);
	bool		__evtLobby_QuickJoinRoom( GAMEEVENTINFO * pInfo);
	bool		__evtLobby_RefreshRoomList( GAMEEVENTINFO * pInfo);
	bool		__evtLobby_DetailRoomInfo( GAMEEVENTINFO * pInfo);
	bool		__evtLobby_EndAbuse( GAMEEVENTINFO * pInfo);
	bool		__evtLobby_RepairEquipment( GAMEEVENTINFO * pInfo);

	//======================================================================================//
	//										Room Event										//
	//======================================================================================//
	i3::unordered_map< GAME_EVENT, SEND_EVENT >	m_eventMap_Room;
	const char *__getRoomEventName( GAME_EVENT evt) const;
	void		__register_Room( void);
	bool		__evtRoom_Ready( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_GetLobbyUser( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_GetInviteLobbyUser(GAMEEVENTINFO * pInfo);
	bool		__evtRoom_InviteLobbyUser( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_SearchHost( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_TransferHost( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_RequestChangeHost( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_TeamChange( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_ChangeOption( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_ChangeInfo( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_ChangeAllInfo( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_ChangePassword( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_ChangeTeam( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_ChangeObserverSlot( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_ChangeHiddenSlot( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_ChagneSlot( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_GetSlotInfo( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_GetUserInfo( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_EquipmentTempWeapon( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_GetRoomScore( GAMEEVENTINFO * pInfo);
	bool		__evtRoom_GetUserEquipment( GAMEEVENTINFO * pInfo);

	//======================================================================================//
	//										Clan Event										//
	//======================================================================================//
	i3::unordered_map< GAME_EVENT, SEND_EVENT >	m_eventMap_Clan;
	const char * __getClanEventName( GAME_EVENT evt) const;
	void		__register_Clan( void);
	bool		__evtClan_EnterPage( GAMEEVENTINFO * pInfo);
	bool		__evtClan_ListContext( GAMEEVENTINFO * pInfo);
	bool		__evtClan_LeavePage( GAMEEVENTINFO * pInfo);
	bool		__evtClan_Create( GAMEEVENTINFO * pInfo);
	bool		__evtClan_DuplicateName( GAMEEVENTINFO * pInfo);
	bool		__evtClan_DuplicateURL( GAMEEVENTINFO * pInfo);
	bool		__evtClan_DuplicateMark( GAMEEVENTINFO * pInfo);
	bool		__evtClan_ChangeMark( GAMEEVENTINFO * pInfo);
	bool		__evtClan_RequestJoin( GAMEEVENTINFO * pInfo);
	bool		__evtClan_CheckAuthorityForJoin( GAMEEVENTINFO * pInfo);
	bool		__evtClan_CancelRequestJoin( GAMEEVENTINFO * pInfo);
	bool		__evtClan_ListDetailInfo(GAMEEVENTINFO * pInfo);
	bool		__evtClan_DetailInfo( GAMEEVENTINFO * pInfo);
	bool		__evtClan_MemberContext( GAMEEVENTINFO * pInfo);
	bool		__evtClan_MemberList( GAMEEVENTINFO * pInfo);
	bool		__evtClan_RequestContext( GAMEEVENTINFO * pInfo);
	bool		__evtClan_RequestList( GAMEEVENTINFO * pInfo);
	bool		__evtClan_RequestInfo( GAMEEVENTINFO * pInfo);
	bool		__evtClan_AcceptRequest( GAMEEVENTINFO * pInfo);
	bool		__evtClan_DenialRequest( GAMEEVENTINFO * pInfo);
	bool		__evtClan_Secession( GAMEEVENTINFO * pInfo);
	bool		__evtClan_CommisionMaster( GAMEEVENTINFO * pInfo);
	bool		__evtClan_CommisionStaff( GAMEEVENTINFO * pInfo);
	bool		__evtClan_CommisionRegular( GAMEEVENTINFO * pInfo);
	bool		__evtClan_DismissalMember( GAMEEVENTINFO * pInfo);
	bool		__evtClan_Dissolution( GAMEEVENTINFO * pInfo);
	bool		__evtClan_ModifyNotice( GAMEEVENTINFO * pInfo);
	bool		__evtClan_ModifyGuide( GAMEEVENTINFO * pInfo);
	bool		__evtClan_ModifyAcceptOption( GAMEEVENTINFO * pInfo);
	bool		__evtClan_Menagement( GAMEEVENTINFO * pInfo);
	bool		__evtClan_List( GAMEEVENTINFO * pInfo);
	bool		__evtClan_CreationInfo( GAMEEVENTINFO * pInfo);
	bool		__evtClan_Invite( GAMEEVENTINFO * pInfo);
	bool		__evtClan_AcceptInvite( GAMEEVENTINFO * pInfo);
	bool		__evtClan_Note( GAMEEVENTINFO * pInfo);

	bool		__evtClan_DetailMemberInfo( GAMEEVENTINFO * pInfo);
	bool		__evtClan_MemberInfo( GAMEEVENTINFO * pInfo);

	bool		__evtClan_MatchResultContext( GAMEEVENTINFO * pInfo);
	bool		__evtClan_MatchResultList( GAMEEVENTINFO * pInfo);
	bool		__evtClan_MatchResultEmotion( GAMEEVENTINFO * pInfo);

	bool		__evtClanWar_PreSeasonRanking( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_GetTeamList( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_CreateTeam( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_JoinTeam( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_LeaveTeam( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_ChangeOperation( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_ChangeMaxPerson( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_RegisterMatchMaking( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_RegisterCancelMatchMaking(GAMEEVENTINFO * pInfo);

	bool		__evtClanWar_GetMercenaryList( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_RegisterMercenary( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_RemoveMercenary( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_SendInviteMercenary( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_AcceptInvite( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_DenialInvite( GAMEEVENTINFO * pInfo);
	bool		__evtClanWar_MercenaryDetail(GAMEEVENTINFO * pInfo );
	bool		__evtClanWar_Result(GAMEEVENTINFO * pInfo);


	//======================================================================================//
	//									Battle Event										//
	//======================================================================================//
	i3::unordered_map< GAME_EVENT, SEND_EVENT >	m_eventMap_Battle;
	const char * __getBattleEventName( GAME_EVENT evt) const;
	void		__register_Battle( void);
	bool		__evtBattle_SendPing( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_Prestart( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_HolepuchOK( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_Start( GAMEEVENTINFO * pInfo);

	bool		__evtBattle_StageJump_RoundStart( GAMEEVENTINFO * pInfo);

	bool		__evtBattle_Giveup( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_Respawn( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_RespawnAI( GAMEEVENTINFO * pInfo);

	//bool		__evtBattle_DebugCommand( GAMEEVENTINFO * pInfo);

	bool		__evtBattle_Death( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_RemainTime( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_Install( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_Uninstall( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_ChangeAILevel( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_Touchdown( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_DeathBlow( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_AttackDefenceObject( GAMEEVENTINFO * pInfo);		// for only defence mode
	bool		__evtVote_Start( GAMEEVENTINFO * pInfo);
	bool		__evtVote_Vote( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_TutorialRoundEnd( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_DestructionObject( GAMEEVENTINFO * pInfo);
	bool		__evtBattle_Heartbit( GAMEEVENTINFO * pInfo);
	bool		__evtFieldShop_GetGoodsList( GAMEEVENTINFO * pInfo);
	bool		__evtFieldShop_Open( GAMEEVENTINFO * pInfo);
	bool		__evtFieldShop_ApplyItem( GAMEEVENTINFO * pInfo);
	bool		__evtGMPause_Activate( GAMEEVENTINFO * pInfo);
	bool		__evtGMPause_Deactivate( GAMEEVENTINFO * pInfo);
	bool		__evtDomination_Skill(GAMEEVENTINFO * pInfo);

	bool		__evtDomination_CheatKey(GAMEEVENTINFO * pInfo);
	bool		__evtDomination_WaveReady(GAMEEVENTINFO * pInfo);
	bool		__evtDomination_WaveReadyCancel(GAMEEVENTINFO * pInfo);
	bool		__evtDomination_UseItem(GAMEEVENTINFO * pInfo);

#ifdef DOMI_NEW_FLOW
	bool		__evtBattle_JumpStageRound_LoadFinished(GAMEEVENTINFO * pInfo);
#endif
	bool		__evtBattle_Change_UserScopeType(GAMEEVENTINFO * pInfo);

	//======================================================================================//
	//										QuickJoin Event									//
	//======================================================================================//
	i3::unordered_map< GAME_EVENT, SEND_EVENT >	m_eventMap_QuickJoin;
	const char *__getQuickjoinEventName( GAME_EVENT evt) const;
	void		__register_QuickJoin( void);
	bool		__evtQuickJoin_Start_Request(GAMEEVENTINFO * pInfo);
	bool		__evtQuickJoin_SecondBest_Request(GAMEEVENTINFO * pInfo);



	//======================================================================================//
	//										Gacha											//
	//======================================================================================//
	i3::unordered_map< GAME_EVENT, SEND_EVENT >	m_NetFuncMap_Gacha;
	void		__register_Gacha( void);
	bool		__evtGacha_ItemInfo( GAMEEVENTINFO * pInfo);
	bool		__evtGacha_Enter( GAMEEVENTINFO * pInfo);
	bool		__evtGacha_Leave( GAMEEVENTINFO * pInfo);
	bool		__evtGacha_Start( GAMEEVENTINFO * pInfo);
	bool		__evtGacha_State_Req( GAMEEVENTINFO * pInfo);
	bool		__evtGacha_Purchase_count( GAMEEVENTINFO * pInfo);

};

#endif
