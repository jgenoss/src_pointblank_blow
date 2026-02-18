#ifndef __SERVERTCPSOCKET_H__
#define __SERVERTCPSOCKET_H__

#include <unordered_map>

//#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
//#define		NETLOG		I3PRINTLOG
//#else
//#define		NETLOG		__noop
//#endif

struct	ServerConnectionInfo
{
public:
	ServerConnectionInfo()
	{
		Reset();
	}

	void	Reset()
	{
		m_SessionIndex	= -1;
		m_GamePort		= 0;
		m_IsRealIp		= false;
		m_MyUdpInfo.Reset();
	}

	INT32			m_SessionIndex;
	UINT16			m_GamePort;

	//NetInfo
	bool			m_IsRealIp;
	NET_UDP_INFO	m_MyUdpInfo;
};


enum	SERVER_TYPE;
struct _FriendNode;

class ClientTCPSocket : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( ClientTCPSocket, i3NetworkClientSocket );

private:
	i3Timer			m_SendTimer;

	SERVER_TYPE				m_TargetServerType;
	ServerConnectionInfo	m_ConnectionInfo;
	bool					m_ServerEntered;

	bool			m_ConnectionSuccess;
	INT16			m_i16PacketRandSeed;

protected:
	REAL32			m_rLastSendTime; 
	bool			m_bShowMessage; 

public : 
	SOCKADDR_IN		m_InAddr;
	INT32	m_UsingType;	// 디버깅용

#if defined( I3_DEBUG)
	INT32	m_iID;		// 디버깅용 박명진

	//Log용 릴리즈에서도 패킷에 대한 정보를 남긴다. 
	UINT32	m_iSendPacketSize; 
	UINT32	m_iSendPacketCount; 
	UINT32	m_iRecvPacketSize; 
	UINT32	m_iRecvPacketCount;
#endif

protected:
	void 		_PacketParsingAuth_server(i3NetworkPacket  * pPacket); 
	void 		_PacketParsingTest(i3NetworkPacket  * pPacket); 
	void 		_PacketParsingServer_message(i3NetworkPacket  * pPacket); 
	bool 		_PacketParsingBattleServer(i3NetworkPacket	*pPacket);
	void 		_PacketParsingModeData(i3NetworkPacket	*pPacket);
	void 		_PacketParsingBase(i3NetworkPacket  * pPacket); 
	void 		_PacketParsingLobby(i3NetworkPacket  * pPacket); 
	void 		_PacketParsingInventory(i3NetworkPacket  * pPacket); 
	void 		_PacketParsingShop(i3NetworkPacket  * pPacket); 
	void 		_PacketParsingGacha(i3NetworkPacket  * pPacket); 
	void 		_PacketParsingRoom(i3NetworkPacket  * pPacket); 
	void 		_PacketParsingBattle(i3NetworkPacket  * pPacket); 
	void 		_PacketParsingClan(i3NetworkPacket  * pPacket); 
	void		_PacketParsingMedal(i3NetworkPacket  * pPacket );
	void		_PacketParsingQuickJoin(i3NetworkPacket * pPacket );
	void 		_PacketParsingRsIgs(i3NetworkPacket  * pPacket); 
	void		_PacketParsingCheat(i3NetworkPacket  * pPacket );
	void		_PacketParsingGMAuthority(i3NetworkPacket * pPacket);
	void 		_PacketParsingClanWar(i3NetworkPacket  * pPacket );
	
	void 		_ReadFriendInfoToNode(i3NetworkPacket* pPacket,struct FRIEND_NODE* node);
	void 		_ReadClanMemberInfoToNode(i3NetworkPacket* pPacket, struct CLAN_MEMBER_NODE* node);

	INT32		_GetEncryptKey();

	INT16		_ChangeEncriptSeed();
	void		_EncriptPacket( i3NetworkPacket* pOutPacket, i3NetworkPacket* pInPacket );

	bool		_SendTCPProfile( UINT16 ui16PacketSize, PROTOCOL TProtocol );
public :
	ClientTCPSocket(void);
	virtual ~ClientTCPSocket(void); 

	virtual bool OnCreate(char * Address, UINT16 PortNo);
	virtual void OnDestroy(void) override;
	virtual void DisConnect() override;

	void	ForceDestroy(void); 
	INT32	SendMessage( i3NetworkPacket * pPacket );
	INT32	SendMessage( S2MOPacketBase* pS2MOPacket );
	REAL32	GetLastSendTime(void)	{ return m_rLastSendTime; }	

	virtual void PacketParsing(i3NetworkPacket  * pPacket) override;

	SOCKADDR_IN GetAddr() { return m_InAddr; }

	//Debug용으로 사용합니다. 
#if defined( I3_DEBUG)
	void DumpSendMessage(UINT16 Protocol);
#endif

	INT32	SendHeartBitMessage();
	INT32	SendHeartBit2Message();
	void	SetTargetServerType(SERVER_TYPE ServerType)			{m_TargetServerType = ServerType;}
	const ServerConnectionInfo &	GetConnectionInfo()			{return m_ConnectionInfo;}
	void	SetConnectionUDPPort(UINT16 ui16Port)				{ m_ConnectionInfo.m_GamePort = ui16Port;}
	NET_UDP_INFO &	GetUDPInfo()								{return m_ConnectionInfo.m_MyUdpInfo;}

	void		SetEnteredServer(bool Entered)					{ m_ServerEntered = Entered; }
	bool		GetEnteredServer()								{ return m_ServerEntered;}
	
public:
	// Parser function map
	typedef void	(ClientTCPSocket::*netFunc)( i3NetworkPacket * pPacket);
	//
	void	RegisterParser( UINT32 protocol, netFunc func);

private:
	void		__Parse_UnknownProtocol( i3NetworkPacket * pPacket);
	void		__Parse_None( i3NetworkPacket * pPacket);

	//---------------------------------------------------------------------------------------------------//
	//							기본 Parser
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_Base;
	void		__Register_Base_Function( void);
	void		__Parse_Base_Connect( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetBasicUserInfo( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetSessionRecord( i3NetworkPacket * pPacket);
	void		__Parse_Base_RendezvousNAT_State( i3NetworkPacket * pPacket);
	void		__Parse_Base_Login( i3NetworkPacket * pPacket);
	void		__Parse_Base_LoginWait( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetOptionInfo( i3NetworkPacket * pPacket);
	//void		__Parse_Base_GetOptionMacroInfo( i3NetworkPacket * pPacket);
	void		__Parse_Base_SaveOptionInfo( i3NetworkPacket * pPacket);
	void		__Parse_Base_SaveOptionMacroInfo( i3NetworkPacket * pPacket);
	void		__Parse_Base_EnterPassword( i3NetworkPacket * pPacket);
	void		__Parse_Base_StartGameGuard( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetMyInfo_Record( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetMyInfo_Basic( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetMyInfo_All( i3NetworkPacket * pPacket);
	void		__Parse_Base_RankUp( i3NetworkPacket * pPacket);
	void		__Parse_Base_InitialRankUp( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetMyInfo_InventoryItem( i3NetworkPacket * pPacket);
	void		__Parse_Base_Announce_SupplyBox( i3NetworkPacket * pPacket);
	void		__Parse_Base_Message_SupplyBox( i3NetworkPacket * pPacket);
	void		__Parse_Base_Present_SupplyBox( i3NetworkPacket * pPacket);
	void		__Parse_Base_Logout( i3NetworkPacket * pPacket);
	void		__Parse_Base_ChatLimit(i3NetworkPacket * pPacket);

	void		__Parse_Base_HeartBit( i3NetworkPacket * pPacket);

	// 1.5용
	void		__Parse_Base_GetUserInfo( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetCharacterInfo( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetInventoryInfo( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetSystemInfo( i3NetworkPacket * pPacket);
	void		__Parse_Base_CreateCharacter( i3NetworkPacket * pPacket);
	void		__Parse_Base_DeleteCharacter( i3NetworkPacket * pPacket);
	void		__Parse_Base_ChangeCharacterEquipment( i3NetworkPacket * pPacket);
	void		__Parse_Base_ChangeCharaState( i3NetworkPacket * pPacket);
	void		__Parse_Base_LearnSkillSlot( i3NetworkPacket * pPacket);
	void		__Parse_Base_LearnSkillClass( i3NetworkPacket * pPacket);
	void		__Parse_Base_GameServerState( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetChannelList( i3NetworkPacket * pPacket);
	void		__Parse_Base_SelectChannel( i3NetworkPacket * pPacket);
	void		__Parse_Base_LeaveServer( i3NetworkPacket * pPacket);
	void		__Parse_Base_EnterServer( i3NetworkPacket * pPacket);
	void		__Parse_Base_ChangeCharacterName( i3NetworkPacket * pPacket);
	void		__Parse_Base_FindUserPosition( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetUserBasicInfo( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetUserDetailInfo( i3NetworkPacket * pPacket);
	void		__Parse_Base_GetUserSubtaskInfo( i3NetworkPacket * pPacket);
	void		__Parse_Base_InviteNormalUser( i3NetworkPacket * pPacket);
	void		__Parse_Base_RequestInviteNormalUser( i3NetworkPacket * pPacket);
	void		__Parse_Base_UserGuideComplete(	i3NetworkPacket * pPacket );
	void		__Parse_Base_Reward_Popup( i3NetworkPacket * pPacket);

	//void		__Parse_Base_Attendance_Info( i3NetworkPacket * pPacket);
	void		__Parse_Base_Attendance_Check( i3NetworkPacket * pPacket);
	void		__Parse_Base_Attendance_Clear_Item( i3NetworkPacket * pPacket);

	void		__Parse_Base_Equipment( i3NetworkPacket * pPacket);
	void		__Parse_Base_DailyRecord( i3NetworkPacket * pPacket);

	void		__Parse_Base_ChangePCCafeStatus(i3NetworkPacket * pPacket);
	void		__Parse_Base_ReceiveMegaPhone(i3NetworkPacket * pPacket);
	void		__Parse_Base_Notice(i3NetworkPacket* pPacket);
	void		__Parse_Base_URL_List(i3NetworkPacket* pPacket);

	// 호칭용!!
	void		__Parse_PROTOCOL_BASE_USER_TITLE_EQUIP_ACK(i3NetworkPacket* pPacket);
	void		__Parse_PROTOCOL_BASE_USER_TITLE_CHANGE_ACK(i3NetworkPacket* pPacket);
	void		__Parse_PROTOCOL_BASE_USER_TITLE_RELEASE_ACK(i3NetworkPacket* pPacket);
	void		__Parse_PROTOCOL_BASE_USER_TITLE_INFO_ACK(i3NetworkPacket* pPacket);

	// 미션카드용!!
	void		__Parse_PROTOCOL_BASE_QUEST_GET_ACK(i3NetworkPacket* pPacket);
	void		__Parse_PROTOCOL_BASE_QUEST_GET_ITEM_ACK(i3NetworkPacket* pPacket);
	void		__Parse_PROTOCOL_BASE_QUEST_GET_INFO_ACK(i3NetworkPacket* pPacket);
	void		__Parse_PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK(i3NetworkPacket* pPacket);
	void		__Parse_PROTOCOL_BASE_QUEST_CHANGE_ACK(i3NetworkPacket* pPacket);
	void		__Parse_PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK(i3NetworkPacket* pPacket);
	void		__Parse_PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK(i3NetworkPacket* pPacket);
	void		__Parse_PROTOCOL_BASE_MEDAL_GET_INFO_ACK(i3NetworkPacket* pPacket);
	void	    __Parse_Base_EventItem(i3NetworkPacket * pPacket);

	void		__Parse_Base_Get_MAP_RULELIST(i3NetworkPacket* pPacket);
	void		__Parse_Base_Get_MAP_MATCHINGLIST(i3NetworkPacket* pPacket);
	void		__Parse_Base_Get_BOOSTEVENT_INFO(i3NetworkPacket* pPacket);

	//---------------------------------------------------------------------------------------------------//
	//							Auth Parser
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_Auth;
	void		__Register_Auth_Function( void);
	void		__Parse_Auth_BlockUser( i3NetworkPacket * pPacket);
	void		__Parse_Auth_FriendUser( i3NetworkPacket * pPacket);
	void		__Parse_Auth_InviteFriend( i3NetworkPacket * pPacket);
	void		__Parse_Auth_RequestInvitedFriend( i3NetworkPacket * pPacket);
	void		__Parse_Auth_ChangeFriendInfo( i3NetworkPacket * pPacket);
	void		__Parse_Auth_AcceptFriend( i3NetworkPacket * pPacket);
	void		__Parse_Auth_AddFriend( i3NetworkPacket * pPacket);
	void		__Parse_Auth_DeleteFriend( i3NetworkPacket * pPacket);
	void		__Parse_Auth_SendWhisper( i3NetworkPacket * pPacket);
	void		__Parse_Auth_ReceiveWhisper( i3NetworkPacket * pPacket);
	void		__Parse_Auth_FindUser( i3NetworkPacket * pPacket);
	void		__Parse_Auth_ChangeNickname( i3NetworkPacket * pPacket);
	void		__Parse_MailBox_SendNote( i3NetworkPacket * pPacket);
	void		__Parse_MailBox_SendClanNote( i3NetworkPacket * pPacket);
	void		__Parse_MailBox_ReceiveNoteList( i3NetworkPacket * pPacket);
	void		__Parse_MailBox_CheckReadedNote( i3NetworkPacket * pPacket);
	void		__Parse_MailBox_DeleteNote( i3NetworkPacket * pPacket);
	void		__Parse_MailBox_ReceiveNote( i3NetworkPacket * pPacket);
	void		__Parse_Auth_FCMInfo( i3NetworkPacket * pPacket);
	void		__Parse_Auth_FCMState( i3NetworkPacket * pPacket);
	void		__Parse_Auth_AddBlockUser( i3NetworkPacket * pPacket);
	void		__Parse_Auth_DeleteBlockUser( i3NetworkPacket * pPacket);
	void		__Parse_Auth_AccountKick( i3NetworkPacket * pPacket);
	



	//---------------------------------------------------------------------------------------------------//
	//							Lobby에서 사용되는 Parsing function
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_Lobby;
	void		__Register_Lobby_Function( void);
	void		__Parse_Lobby_Enter( i3NetworkPacket * pPacket);
	void		__Parse_Lobby_Leave( i3NetworkPacket * pPacket);
	void		__Parse_Lobby_ReceiveRoomList( i3NetworkPacket * pPacket);
	void		__Parse_Lobby_ReceiveRoomInfo( i3NetworkPacket * pPacket);
	void		__Parse_Lobby_CreateRoom( i3NetworkPacket * pPacket);
	void		__Parse_Lobby_JoinRoom( i3NetworkPacket * pPacket);
	void		__Parse_Lobby_QuickJoinRoom( i3NetworkPacket * pPacket);
	void		__Parse_Lobby_CreateNickname( i3NetworkPacket * pPacket);
	void		__Parse_Lobby_ReceiveRoomAddableInfo( i3NetworkPacket * pPacket);
	void		__Parse_Lobby_WaitingEndAbusing( i3NetworkPacket * pPacket);
	void		__Parse_Lobby_Chat( i3NetworkPacket * pPacket);
	void		__Parse_Lobby_ViewUserItem( i3NetworkPacket * pPacket);
	void		__Parser_Lobby_ViewUserItemForUserProfile( i3NetworkPacket* pPacket);

	//---------------------------------------------------------------------------------------------------//
	//							Inventory에서 사용되는 Parsing function
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_Inventory;
	void		__Register_Inventory_Function( void);
	void		__Parse_Inventory_GetInfo( i3NetworkPacket * pPacket);
	void		__Parse_Inventory_Enter( i3NetworkPacket * pPacket);
	void		__Parse_Inventory_Leave( i3NetworkPacket * pPacket);

	//---------------------------------------------------------------------------------------------------//
	//							Shop에서 사용되는 Parsing function
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_Shop;
	void		__Register_Shop_Function( void);
	void		__Parse_Shop_Enter( i3NetworkPacket * pPacket);
	void		__Parse_Shop_Leave( i3NetworkPacket * pPacket);

	void		__Parse_Auth_Shop_BuyGoods( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_AuthItem( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_DeleteItem( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_RepairItem( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Lobby_RepairEquipment( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_Capsule( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_Jackpot( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_ItemList( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_GoodsList( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_RepairList( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_MatchingList( i3NetworkPacket * pPacket);
	void		__Parse_Auth_UseItem_CheckNick( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Get_PointAndCash( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Get_PointAndCashForKR( i3NetworkPacket * pPacket);
	void		__Parse_Auth_PlusPoint( i3NetworkPacket * pPacket);
	void		__Parse_Auth_CouponAwardItem( i3NetworkPacket * pPacket);
	void		__Parse_Auth_SaleCouponList( i3NetworkPacket * pPacket);
	void		__Parse_Shop_GetItemList( i3NetworkPacket * pPacket);

	void		__Parse_Auth_Shop_GiftShop_Enter( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_GiftShop_Leave( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_GetGiftList( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_AuthGift( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_SendGift( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_RecvGift( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_Use_GiftCoupon( i3NetworkPacket * pPacket);
	void		__Parse_Auth_Shop_Item_Change(i3NetworkPacket * pPacket); 

	//---------------------------------------------------------------------------------------------------//
	//							Gacha에서 사용되는 Parsing function
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_Gacha;
	void		__Register_Gacha_Function( void);
	void		__Parse_Gacha_Enter( i3NetworkPacket * pPacket);
	void		__Parse_Gacha_Leave( i3NetworkPacket * pPacket);
	void		__Parse_Gacha_ItemInfo( i3NetworkPacket * pPacket);
	void		__Parse_Gacha_State( i3NetworkPacket * pPacket);	
	void		__Parse_Gacha_Purchase( i3NetworkPacket * pPacket);
	void		__Parse_Gacha_Notice( i3NetworkPacket * pPacket);
	void		__Parse_Gacha_WinningUserAck( i3NetworkPacket * pPacket);
	void		__Parse_Gacha_Purchase_Count( i3NetworkPacket * pPacket);

	//---------------------------------------------------------------------------------------------------//
	//												QuickJoin
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc > m_NetFuncMap_QuickJoin;
	void		__Register_QuickJoin_Function( void);
	void		__Parse_QuickJoin_Start( i3NetworkPacket * pPacket);
	void		__Parse_QuickJoin_SecondBest( i3NetworkPacket * pPacket);

	//---------------------------------------------------------------------------------------------------//
	//												Medal
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_Medal;
	void		__Register_Medal_Function( void);
	void		__Parse_Medal_Notify_Info( i3NetworkPacket * pPacket );			// GetMyInfo 이후 알리미 정보만.
	void		__Parse_Medal_Normal_All_Info( i3NetworkPacket * pPacket );		// 메달 페이지 진입시 기본 메달 정보
	void		__Parse_Medal_Set_Cur_All_Info( i3NetworkPacket * pPacket );	// 메달 페이지 진입시 진행중인 세트 메달 정보
	void		__Parse_Medal_Set_Com_All_Info( i3NetworkPacket * pPacket );	// 메달 페이지 진입시 완료된 세트 메달 정보
	void		__Parse_Medal_Mission_Complete( i3NetworkPacket * pPacket );	// 메달 배틀중 미션 카운트 증가
	void		__Parse_Medal_Get_Reward( i3NetworkPacket * pPacket );			// 메달 보상받기 눌렀을시 응답 패킷
	void		__Parse_Medal_Notify_Register( i3NetworkPacket * pPacket );		// 메달 알리미 등록 응답 패킷
	void		__Parse_Medal_Refresh_Set( i3NetworkPacket * pPacket );			// 일일 메달 갱신(Day가 바뀔때)

	//---------------------------------------------------------------------------------------------------//
	//												Cheat
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_Cheat;
	void		__Register_Cheat_Function( void);
	void		__Parse_Cheat_Medal_Command( i3NetworkPacket * pPacket );
	void		__Parse_Cheat_Reduce_TSEvent( i3NetworkPacket * pPacket );
	void		__Parse_Cheat_Increase_Killcount( i3NetworkPacket * pPacket );
	void		__Parse_Cheat_Play_Solo( i3NetworkPacket * pPacket );
	void		__Parse_Cheat_ClanWarMatchingPoint( i3NetworkPacket * pPacket );
	void		__Parse_Cheat_ClanWarMatchResult( i3NetworkPacket * pPacket );
	void		__Parse_Cheat_RandomBoxAuthResult( i3NetworkPacket * pPacket );

	//---------------------------------------------------------------------------------------------------//
	//							Clan에서 사용되는 Parsing function
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_Clan;
	void		__Register_Clan_Function( void);
	void		__Parse_Clan_Enter( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Leave( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ContextInfo( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ClanList( i3NetworkPacket * pPacket);
	void		__Parse_Clan_MemberInfo( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChangeMember( i3NetworkPacket * pPacket);
	void		__Parse_Clan_AddMember( i3NetworkPacket * pPacket);
	void		__Parse_Clan_DeleteMember( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Create( i3NetworkPacket * pPacket);
	void		__Parse_Clan_DuplicateName( i3NetworkPacket * pPacket);
	void		__Parse_Clan_DuplicateAzitURL( i3NetworkPacket * pPacket);
	void		__Parse_Clan_DuplicateMark( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ReplaceMark( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChangePersonMax( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Management( i3NetworkPacket * pPacket);
	void		__Parse_Clan_RequestJoin( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Note( i3NetworkPacket * pPacket);
	void		__Parse_Clan_JoinAuthoriy( i3NetworkPacket * pPacket);
	void		__Parse_Clan_CancelRequest( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ListDetailInfo(i3NetworkPacket * pPacket);
	void		__Parse_Clan_DetailInfo( i3NetworkPacket * pPacket);
	void		__Parse_Clan_MemberContext( i3NetworkPacket * pPacket);
	void		__Parse_Clan_MemberList( i3NetworkPacket * pPacket);
	void		__Parse_Clan_RequestContext( i3NetworkPacket * pPacket);
	void		__Parse_Clan_RequestList( i3NetworkPacket * pPacket);
	void		__Parse_Clan_RequestInfo( i3NetworkPacket * pPacket);
	void		__Parse_Clan_AcceptRequest( i3NetworkPacket * pPacket);
	void		__Parse_Clan_DenineRequest( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Invite( i3NetworkPacket * pPacket);
	void		__Parse_Clan_AcceptInvite( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Session( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Commission_Master( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Commission_Staff( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Commission_Regular( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Deportation( i3NetworkPacket * pPacket);
	void		__Parse_Clan_AcceptRequestResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_DenialRequestResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_DeportationResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Commission_MasterResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Commission_StaffResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Commission_RegularResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChangeExpRank( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChangePerRank( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChangeMarkResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChangeManagementResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChangeName( i3NetworkPacket * pPacket);
	void		__Parse_Clan_InviteRoom( i3NetworkPacket * pPacket);
	void		__Parse_Clan_InviteRoomResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Close( i3NetworkPacket * pPacket);
	void		__Parse_Clan_Chat( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChatAtMemberPage( i3NetworkPacket * pPacket);
	void		__Parse_Clan_MemberDetailInfo( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChangeMemberBasicInfoResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChangeNotice( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChangeIntro( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChangeAcceptOption( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ResetRecordResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ResetPointResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_ChangeColorNameResult( i3NetworkPacket * pPacket);
	void		__Parse_Clan_CheckConditionForCreation( i3NetworkPacket * pPacket);
	void		__Parse_Clan_MatchResultContext( i3NetworkPacket * pPacket);
	void		__Parse_Clan_MatchResultList( i3NetworkPacket * pPacket);
	void		__Parse_Clan_MatchResultEmotion( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_MatchResult(i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_Result(i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_Replace_Clan_Name(i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_Replace_Clan_Mark(i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_Replace_Clan_Color_Name(i3NetworkPacket * pPacket);

	//---------------------------------------------------------------------------------------------------//
	//							Room에서 사용되는 Parsing function
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_Room;
	void		__Register_Room_Function( void);
	void		__Parse_Room_ReceiveAllUserSlotInfo( i3NetworkPacket * pPacket);
	void		__Parse_Room_ReceiveUserSlotInfo( i3NetworkPacket * pPacket);
	void		__Parse_Room_ChangeRoomInfo( i3NetworkPacket * pPacket);
	void		__Parse_Room_ChangeRoomOption( i3NetworkPacket * pPacket);
	void		__Parse_Room_Enter( i3NetworkPacket * pPacket);
	void		__Parse_Room_Leave( i3NetworkPacket * pPacket);
	void		__Parse_Room_SearchHost( i3NetworkPacket * pPacket);
	void		__Parse_Room_TeamBalance( i3NetworkPacket * pPacket);
	void		__Parse_Room_Unready4vs4( i3NetworkPacket * pPacket);
	void		__Parse_Room_RequestChangeHost( i3NetworkPacket * pPacket);
	void		__Parse_Room_ChangeHost( i3NetworkPacket * pPacket);
	void		__Parse_Room_ReceiveNickname( i3NetworkPacket * pPacket);
	void		__Parse_Room_ReceiveRank( i3NetworkPacket * pPacket);
	void		__Parse_Room_ReceiveNicknameColor( i3NetworkPacket * pPacket);
	void		__Parse_Room_ChangePassword( i3NetworkPacket * pPacket);
	void		__Parse_Room_ChangeSlot( i3NetworkPacket * pPacket);
	void		__Parse_Room_ReceiveLobbyUserList( i3NetworkPacket * pPacket);
	void		__Parse_Room_ReceiveInviteUserList(i3NetworkPacket * pPacket);
	void		__Parse_Room_ChangeTeam( i3NetworkPacket * pPacket);
	void		__Parse_Room_ChangeObserverSlot( i3NetworkPacket * pPacket);
	void		__Parse_Room_ChangeHiddenSlot( i3NetworkPacket * pPacket);
	void		__Parse_Room_Chat( i3NetworkPacket * pPacket);
	void		__Parse_Room_SelectMainCharacter( i3NetworkPacket * pPacket);
	void		__Parse_Room_SelectOtherUserChara( i3NetworkPacket * pPacket);
	void		__Parse_Room_LoadingStart( i3NetworkPacket * pPacket);
	void		__Parse_Room_GetUserEquipmentInfo( i3NetworkPacket * pPacket);
	void		__Parse_Room_GetUserItem( i3NetworkPacket * pPacket);
	void		__Parse_Room_GetColorMuzzleFlash(i3NetworkPacket * pPacket);
	void		__Parse_Room_DomiEnter( i3NetworkPacket * pPacket);
	void		__Parse_Room_UserInfo( i3NetworkPacket * pPacket);

	//---------------------------------------------------------------------------------------------------//
	//							인게임 전투에서 사용되는 Parsing function
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_Battle;
	void		__Resigter_Battle_Function( void);
	void		__Parse_Battle_ReadyBattle( i3NetworkPacket * pPacket);
	void		__Parse_Battle_StartGame( i3NetworkPacket * pPacket);
	void		__Parse_Battle_StartGameTrans( i3NetworkPacket * pPacket);

	/** \brief 난입시 battle중인 유저들의 캐릭터 정보를 받는다. */
	void		__Parse_Battle_ReceiveEntrancedUserInfo( i3NetworkPacket * pPacket);
	void		__Parse_Battle_ReceiveAnotherUserInfo( i3NetworkPacket * pPacket);
	void		__Parse_Battle_ReceiveAnotherEquipInfo( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Prestart( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Start( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Start_Count_Down( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Giveup( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Respawn( i3NetworkPacket * pPacket);
	void		__Parse_Battle_RespawnForAI( i3NetworkPacket * pPacket);

	void		__Parse_Battle_Domination_UserSkillUpgrade( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Domination_WavePreStart( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Domination_WaveInfo( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Domination_WaveReadyInfo( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Domination_WaveStart( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Domination_WaveEnd( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Domination_PrintMessage( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Domination_FinalScore( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Domination_AppearAcidItem( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Domination_MutantTrexHowlingSlow( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Domination_NaviMesh_or_AICollision( i3NetworkPacket * pPacket);

	void		__Parse_Battle_Death( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Score( i3NetworkPacket * pPacket);
	void		__Parse_Battle_End( i3NetworkPacket * pPacket);
	void		__Parse_Battle_ChangeHost( i3NetworkPacket * pPacket);
	void		__Parse_Battle_TimeSync( i3NetworkPacket * pPacket);
	void		__Parse_Battle_HoleCheck( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Timeout( i3NetworkPacket * pPacket);
	void		__Parse_Battle_SendPing( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Round_Prestart( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Round_Start( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Round_End( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Install_Bomb( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Uninstall_Bomb( i3NetworkPacket * pPacket);
	void		__Parse_Battle_ReceiveDestructionModeInfo( i3NetworkPacket * pPacket);
	void		__Parse_Battle_ReceiveDefenceModeInfo( i3NetworkPacket * pPacket);
	void		__Parse_Battle_ReceiveTouchdownModeInfo( i3NetworkPacket * pPacket);
	void		__Parse_Battle_TourchdownCount( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Record( i3NetworkPacket * pPacket);
	void		__Parse_Battle_CountdownForStart( i3NetworkPacket * pPacket);
	void		__Parse_Battle_ChangeDifficultyLevelForAI( i3NetworkPacket * pPacket);
	void		__Parse_Battle_Tutorial_Round_End( i3NetworkPacket * pPacket);
	void		__Parse_Battle_SuggestKickVote( i3NetworkPacket * pPacket);
	void		__Parse_Battle_StartKickVote( i3NetworkPacket * pPacket);
	void		__Parse_Battle_VoteKickVote( i3NetworkPacket * pPacket);
	void		__Parse_Battle_NotifyKickVote( i3NetworkPacket * pPacket);
	void		__Parse_Battle_CancelKickVote( i3NetworkPacket * pPacket);
	void		__Parse_Battle_KickVoteResult( i3NetworkPacket * pPacket);
	void		__Parse_Battle_BeKickedByVote( i3NetworkPacket * pPacket);
	void		__Parse_Battle_BlockTouchdown( i3NetworkPacket * pPacket);
	void		__Parse_Battle_NotifyHackingUser( i3NetworkPacket * pPacket);
	void		__Parse_Battle_EquipFromBattleShop( i3NetworkPacket * pPacket);
	void		__Parse_Battle_GMPause_Activate( i3NetworkPacket * pPacket);
	void		__Parse_Battle_GMPause_Deactivate( i3NetworkPacket * pPacket);
	void		__Parse_Battle_LoadInvenInfo( i3NetworkPacket * pPacket);
	void		__Parse_Battle_RoomScore( i3NetworkPacket * pPacket);
	void		__Parse_Battle_ScopeType(i3NetworkPacket * pPacket);

	//---------------------------------------------------------------------------------------------------//
	//							서버에서 일방적으로 전달하는 Message
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_ServerMessage;
	void		__Register_ServerMessage_Function( void);
	void		__Parse_ServerMessage_ConnectionSuccess( i3NetworkPacket * pPacket);
	void		__Parse_ServerMessage_KickPlayer( i3NetworkPacket * pPacket);
	void		__Parse_ServerMessage_Invited( i3NetworkPacket * pPacket);
	void		__Parse_ServerMessage_FriendInvited( i3NetworkPacket * pPacket);
	void		__Parse_ServerMessage_Announce( i3NetworkPacket * pPacket);
	void		__Parse_ServerMessage_Error( i3NetworkPacket * pPacket);
	void		__Parse_ServerMessage_DisconnectionSuccess( i3NetworkPacket * pPacket);
	void		__Parse_ServerMessage_GameguardError( i3NetworkPacket * pPacket);
	void		__Parse_ServerMessage_KickPlayerFromBattle( i3NetworkPacket * pPacket);
	void		__Parse_ServerMessage_ChangeInventory( i3NetworkPacket * pPacket);
	void		__Parse_ServerMessage_EventCoupon( i3NetworkPacket * pPacket);
	void		__Parse_ServerMessage_PCCafeChangedStatus(i3NetworkPacket * pPacket); // 실시간으로 피시카펫혜택이처리되기때문에


	//---------------------------------------------------------------------------------------------------//
	//							RS(1.0뽑기상점), IGS에서 사용하는 essage
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_RsIgs;
	void		__Register_RsIgs_Function( void);
	//void		__Parse_RouletteShop_Enter( i3NetworkPacket * pPacket);
	//void		__Parse_RouletteShop_Leave( i3NetworkPacket * pPacket);
	//void		__Parse_RouletteShop_ItemInfo( i3NetworkPacket * pPacket);
	//void		__Parse_RouletteShop_RouletteResult( i3NetworkPacket * pPacket);
	void		__Parse_FieldShop_Open( i3NetworkPacket * pPacket);
	void		__Parse_FieldShop_GetGoodsList( i3NetworkPacket * pPacket);

	//---------------------------------------------------------------------------------------------------//
	//							GM 권한 Popup에서 사용하는 Message
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_GMAuthority;
	void		__Register_GMAuthority_Function( void);
	void		__Parse_GMAuthority_OpenPopup( i3NetworkPacket * pPacket);
	void		__Parse_GMAuthority_ReceiveChat( i3NetworkPacket * pPacket);
	void		__Parse_GMAuthority_ClosePopup( i3NetworkPacket * pPacket);
	void		__Parse_GMAuthority_ApplyPenalty( i3NetworkPacket * pPacket);
	void		__Parse_GMAuthority_PenaltyNotice( i3NetworkPacket * pPacket);


	//---------------------------------------------------------------------------------------------------//
	//							Clan War에 사용되는 Parsing Function
	//---------------------------------------------------------------------------------------------------//
	i3::unordered_map< UINT32, netFunc >	m_NetFuncMap_ClanWar;
	void		__Register_ClanWar_Function( void);
	void		__Parse_ClanWar_PreSeasonRanking( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_AllMatchingInfo( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_MatchTeamList( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_CreateTeam( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_JoinTeam( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_LeaveTeam( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_ChangeOperation( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_ChangeMaxPerson( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_RegisterMatchMaking( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_RegisterCancelMatchMaking(i3NetworkPacket * pPacket);
	
	void		__Parse_ClanWar_TeamChatting( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_BattleTeamInfo( i3NetworkPacket * pPacket);

	void		__Parse_ClanWar_MercenaryList( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_RegisterMercenary( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_RemoveMercenary( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_InviteMercenarySender( i3NetworkPacket * pPacket);		// Server -> Match Team Leader
	void		__Parse_ClanWar_InviteMercenaryReceiver( i3NetworkPacket * pPacket);	// Server -> Mercenary
	void		__Parse_ClanWar_AcceptInvite( i3NetworkPacket * pPacket);	
	void		__Parse_ClanWar_DenialInvite( i3NetworkPacket * pPacket);
	void		__Parse_ClanWar_MerceDetailInfo( i3NetworkPacket * pPacket );
	void		__Parse_ClanWar_ForceMatchroomLeave( i3NetworkPacket *pPacket );

};
#endif
