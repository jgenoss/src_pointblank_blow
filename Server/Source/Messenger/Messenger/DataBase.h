#ifndef _DATABASE_H__
#define _DATABASE_H__

#include "../../CommonStructure/ShareServer/GameMessegerServer/ShareDB2MessServerProtocolStructure.h"

#define DB_BUFFER_FRIEND_LIST_COUNT		5000
#define DB_BUFFER_BLOCK_LIST_COUNT		5000
#define DB_BUFFER_COUNT					100

// TODO : 테스트 후 10배로 늘릴 것
#define UPDATE_NICK_BUFFER_COUNT		1000
#define UPDATE_RANK_BUFFER_COUNT		5000
#define CLAN_GROUP_NOTE_BUFFER_COUNT	300

// Delay of Working
#define DELAY_WORKING					100

#define	BLOCK_CONTROL_ADD		0
#define	BLOCK_CONTROL_DELETE	1
 
#define NOTE_COMMAND_TYPE_ADD_NORMAL	NT_NORMAL
#define NOTE_COMMAND_TYPE_ADD_NOTICE	NT_NOTICE
#define NOTE_COMMAND_TYPE_ADD_CLAN		NT_CLAN
#define NOTE_COMMAND_TYPE_ADD_CLANASK	NT_CLAN_ASK
#define NOTE_COMMAND_TYPE_DEL			14
#define NOTE_COMMAND_TYPE_READ			15
#define NOTE_COMMAND_TYPE_ADD_GIFT		NT_GIFT

 							
#define SP_RV_USP_TCMNBK_BLOCK_DELETE_ERROR_DELETE_FAIL			3301
#define SP_RV_USP_TCMNBK_BLOCK_ADD_ERROR_INSERT_FAIL			3501

// Log Type

#define LOG_DB_IDX			m_pi32LogIdx[i32ThreadIdx]

//==========================================================================================================================================================================

class CDataBase :	public i3ElementBase
{
	I3_CLASS_DEFINE( CDataBase );
protected	:	
	CDBConfig*				m_apMessDBConfig[DB_THREAD_COUNT];
	CDBConfig*				m_apUserDBConfig[DB_THREAD_COUNT];

	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	CADODatabase			**	m_ppADOdb_MESS;
	CADODatabase			**	m_ppADOdb_USER;

	HANDLE					*	m_pHThread;
	INT32					*	m_pi32ThreadIdx;
	INT32					*	m_pi32LogIdx;
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	// Friend
	Local_CRingBuffer	**	m_ppI_FriendList;
	Local_CRingBuffer	**	m_ppO_FriendList;

	Local_CRingBuffer	**	m_ppI_FriendControl;
	Local_CRingBuffer	**	m_ppO_FriendControl;

	INT32	m_i32OutFriendListThreadIdx;
	INT32	m_i32OutFriendControlThreadIdx;


	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	// Block
#if defined( USE_BLOCK_LIST )
	Local_CRingBuffer	**	m_ppI_BlockList;
	Local_CRingBuffer	**	m_ppO_BlockList;

	Local_CRingBuffer	**	m_ppI_BlockControl;
	Local_CRingBuffer	**	m_ppO_BlockControl;

	INT32	m_i32OutBlockListThreadIdx;
	INT32	m_i32OutBlockControlThreadIdx;
#endif

	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	//Note 
	Local_CRingBuffer	** m_ppI_NoteList; 
	Local_CRingBuffer	** m_ppO_NoteList;

	Local_CRingBuffer	** m_ppI_NoteCommand;
	Local_CRingBuffer	** m_ppO_NoteCommand;

	INT32	m_i32PopIdxNoteList;
	INT32	m_i32PopIdxNoteCommand;
	
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
#ifdef USE_CHANGE_NICK	
	// Change Nick
	Local_CRingBuffer	** m_ppI_ChangeNick;
	Local_CRingBuffer	** m_ppO_ChangeNick;

	INT32	m_i32OutChangeNickThreadIdx;
#endif
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
#ifdef USE_CHANGE_RANK	
	// Change Rank	
	Local_CRingBuffer	** m_ppI_ChangeRank;
	Local_CRingBuffer	** m_ppO_ChangeRank;

	INT32	m_i32OutChangeRankThreadIdx;	
#endif
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	// Clan Group Note
	Local_CRingBuffer	** m_ppI_ClanGroupNote;
	Local_CRingBuffer	** m_ppO_ClanGroupNote;

	INT32	m_i32PopIdxClanGroupNoteCommand;
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
#ifdef USE_MESS_FIND	
	//User Search
	Local_CRingBuffer	** m_ppI_UserInfo; 	
	Local_CRingBuffer	** m_ppO_UserInfo;

	INT32	m_iPopIdxUserInfo; 
#endif

public:
	CDataBase(void);
	~CDataBase(void);

	BOOL	OnCreate(void); 
	void	OnDestroy(void);
	void	OnWorking(INT32 i32ThreadIdx);
	
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
#ifdef USE_MESS_FIND
	//User Search
	BOOL	InUserSearch(INT64 i64UID,  TTCHAR * pstrNick);
	INT32	Working_UserSearch(INT32 i32ThreadIdx);
	INT32	GetOutUserCount(void)
	{
		INT32 iCount = 0;
		for(INT32 i = 0 ; i < DB_THREAD_COUNT; i++)	iCount += m_ppO_UserInfo[i]->GetBufferCount();
		return iCount;
	}
	OUT_USER_INFO_SEARCH * GetOutUserSearch(void); 
	void	OutUserSearchIdx(void)		{ m_ppO_UserInfo[m_iPopIdxUserInfo]->PopIdx(); }
#endif
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	// Friend
	BOOL	InFriendList(IN_FRIEND_LIST* pIn);
	BOOL	InFriendControl(IN_FRIEND_CONTROL* pIn);
	INT32	Working_Friend(INT32 i32ThreadIdx); 

	INT32	GetOutFriendListCount(void)
	{
		INT32 iCount = 0;
		for(INT32 i = 0 ; i < DB_THREAD_COUNT; i++)	iCount += m_ppO_FriendList[i]->GetBufferCount();
		return iCount;
	}
	void				GetOutFriendListIdx(void);
	OUT_FRIEND_LIST*	GetOutFriendListData(void);

	INT32	GetOutFriendControlCount(void)
	{
		INT32 iCount = 0;
		for(INT32 i = 0 ; i < DB_THREAD_COUNT; i++)	iCount += m_ppO_FriendControl[i]->GetBufferCount();
		return iCount;
	}
	void				GetOutFriendControlIdx(void);
	OUT_FRIEND_CONTROL* GetOutFriendControlData(void);


	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	// Block
#if defined( USE_BLOCK_LIST )
	BOOL	InBlockList(IN_BLOCK_LIST* pIn);
	BOOL	InBlockControl(IN_BLOCK_CONTROL* pIn);
	INT32	Working_Block(INT32 i32ThreadIdx);

	INT32	GetOutBlockListCount(void)
	{
		INT32 i32Count = 0;
		for(INT32 i = 0 ; i < DB_THREAD_COUNT; i++)	i32Count += m_ppO_BlockList[i]->GetBufferCount();
		return i32Count;
	}
	void				GetOutBlockListIdx(void);
	OUT_BLOCK_LIST*	GetOutBlockListData(void);

	INT32	GetOutBlockControlCount(void)
	{
		INT32 i32Count = 0;
		for(INT32 i = 0 ; i < DB_THREAD_COUNT; i++)	i32Count += m_ppO_BlockControl[i]->GetBufferCount();
		return i32Count;
	}
	void				GetOutBlockControlIdx(void);
	OUT_BLOCK_CONTROL* GetOutBlockControlData(void);
#endif

#ifdef USE_CHANGE_NICK
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	// Change Nick
	BOOL	InChageNick(IN_CHANGE_NICK* pIn);
	INT32	WorkingChangeNick(INT32 i32ThreadIdx);

	INT32	GetInChangeNickCount()
	{
		INT32 i32Rv = 0;
		for(INT32 i = 0; i < DB_THREAD_COUNT; i++) i32Rv += m_ppI_ChangeNick[i]->GetBufferCount();
		return i32Rv;
	}
	INT32	GetOutChangeNickCount(void)
	{
		INT32 i32Count = 0;
		for(INT32 i = 0 ; i < DB_THREAD_COUNT; i++)	i32Count += m_ppO_ChangeNick[i]->GetBufferCount();
		return i32Count;
	}
	void				GetOutChangeNickIdx(void);
	OUT_CHANGE_NICK*	GetOutChangeNickData(void);
#endif
#ifdef USE_CHANGE_RANK
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	// Change Rank
	BOOL	InChangeRank(IN_CHANGE_RANK* pIn);
	INT32	WorkingChangeRank(INT32 i32ThreadIdx);

	INT32	GetInChangeRankCount() 
	{
		INT32 i32Rv = 0;
		for(INT32 i = 0; i < DB_THREAD_COUNT; i++) i32Rv += m_ppI_ChangeRank[i]->GetBufferCount();
		return i32Rv;
	}
	INT32	GetOutChangeRankCount(void)
	{
		INT32 i32Count = 0;
		for(INT32 i = 0 ; i < DB_THREAD_COUNT; i++)	i32Count += m_ppO_ChangeRank[i]->GetBufferCount();
		return i32Count;
	}
	void				GetOutChangeRankIdx(void);
	OUT_CHANGE_RANK*	GetOutChangeRankData(void);
#endif
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	//Note 
	BOOL	InNoteList(INT64 i64UID);
	BOOL	InNoteCommand(INT8 i8Type, INT64 i64UID, UINT8 ui8Count, INT32 * pi32List);
	BOOL	InNoteCommandAddClan(INT8 i8Type, OGame2Mess_Clan_Note_Send_Req stGame2Mess_Clan_Note_Send_Req);
	BOOL	InNoteCommandAdd(INT8 i8Type, TTCHAR * pstrNickSend, OGame2Mess_Note_Send_Req stGame2Mess_Note_Send_Req);
	BOOL	InNoteCommandAddFindUID(INT8 i8Type, TTCHAR * pstrNickSend, OGame2Mess_Note_Send_Find_UID_Req stGame2Mess_Note_Send_Find_UID_Req);
	BOOL	InNoteCommandAddGiftOff(INT8 i8Type, OGame2Mess_Note_Send_Gift_Notice_Req* stGame2Mess_Note_Send_Gift_Notice_Req);
	//BOOL	InNoteCommandAdd(INT8 i8Type, UINT32 iUID, char * pstrNickSend, char * pstrNickRecv, char * pstrMessage, UINT8 ui8MessSize);
	//BOOL	InNoteCommandAddClan(INT8 i8Type, UINT32 iUID, UINT32 ui32ReceiverUID, char * pstrClanName, char * pstrMessage, UINT8 ui8ClanSize, UINT8 ui8MessSize, UINT32 ui32AppendingData);

	INT32	GetOutNoteListCount(void)
	{
		INT32 i32Rv = 0; 
		for(INT32 i = 0; i < DB_THREAD_COUNT; i++) i32Rv += m_ppO_NoteList[i]->GetBufferCount();
		return i32Rv; 
	}

	INT32	GetOutNoteCommandCount(void)
	{
		INT32 i32Rv = 0;
		for(INT32 i = 0; i < DB_THREAD_COUNT; i++) i32Rv += m_ppO_NoteCommand[i]->GetBufferCount();
		return i32Rv;
	}

	OUT_NOTE_LIST	 *	GetOutNoteList(void);
	OUT_NOTE_COMMAND *	GetOutNoteCommand(void);

	void	OutNoteListIdx(void)				{ m_ppO_NoteList[m_i32PopIdxNoteList]->PopIdx(); }
	void	OutNoteCommandIdx(void)				{ m_ppO_NoteCommand[m_i32PopIdxNoteCommand]->PopIdx(); }
	INT32	Working_Note(INT32 i32ThreadIdx); 	


	UINT8 GetRemainDay(INT32 i32deleteDate);

	// MakePacket
	BOOL	MakeNoteNode(NOTE_NODE*	pNoteNode,	
						 INT32		i32id,
						 INT64		i64suid,
						 UINT8		ui8type,
						 UINT8		ui8state,
						 UINT8		ui8remainDay,
						 INT32		i32appendingData,
						 UINT8		ui8senderSize,
						 UINT8		ui8messageSize,
						 TTCHAR *	pstrSender,
						 TTCHAR *	pstrMessage,
						 void *		pUser);

	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	// Clan Group Note
	BOOL	InClanGroupNoteAddClan(INT8 i8Type, UINT32 ui32Count, OGame2Mess_Clan_Group_Note_Send_Req stGame2Mess_Clan_Group_Note_Send_Req);
	INT32	GetInClanGroupNoteCommandCount()
	{
		INT32 i32Rv = 0;
		for(INT32 i = 0; i < DB_THREAD_COUNT; i++) i32Rv += m_ppI_ClanGroupNote[i]->GetBufferCount();
		return i32Rv;
	}
	INT32	GetOutClanGroupNoteCommandCount(void)
	{
		INT32 i32Rv = 0;
		for(INT32 i = 0; i < DB_THREAD_COUNT; i++) i32Rv += m_ppO_ClanGroupNote[i]->GetBufferCount();
		return i32Rv;
	}

	OUT_NOTE_COMMAND *	GetOutClanGroupNoteCommand(void);

	void	OutClanGroupNoteCommandIdx(void) { m_ppO_ClanGroupNote[m_i32PopIdxClanGroupNoteCommand]->PopIdx(); }
	INT32	Working_Clan_Group_Note(INT32 i32ThreadIdx);

	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
protected	: 

	void	ErrorDBMess( wchar_t* wstrSql, INT32 i32ThreadIdx);
	void	ErrorDBUser( wchar_t* wstrSql, INT32 i32ThreadIdx);

	BOOL	_GetNickByReceiverUID(const INT32 i32ThreadIdx,  const INT64 i64ReceiverUID);
	BOOL	_GetUIDByNick(const INT32 iThreadCount, const TTCHAR* pstrNickName, INT64* pi64outUID);
	INT32	_GetNoteDaySendCount(INT32 i32ThreadIdx, INT64 i64UID);
	INT32	_GetNoteCount(INT32 i32ThreadIdx, INT64 i64UID, UINT32* pui32ListIdx);
	BOOL	_GetNoteList(INT32 i32ThreadIdx, INT64 i64UID, OUT_NOTE_LIST * pOutList);
	
	INT32	_DeleteNote(const INT32 i32ThreadIdx, const INT32 i32ListIdx, OUT_NOTE_INFO * pOutNoteInfo);

	INT32	_DeleteNoteTemp(const INT32 i32ThreadIdx, const INT32 i32TableIDX, const INT32 i32ListIdx, OUT_NOTE_INFO * pOutNoteInfo);

	INT32	_NoteCheckBlock( INT32 i32ThreadIdx, INT64 i64SenderUID, INT64 i64ReceiverUID, BOOL* pbIsBlock );	// 쪽지 보내려는 사람이 받는 사람으로부터 차단 당했는지 체크.
};

extern CDataBase * g_pDataBase;

#endif 
