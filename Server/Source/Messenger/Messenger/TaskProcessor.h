#ifndef _TaskProcessor_h
#define _TaskProcessor_h

#include "../../CommonStructure/ShareServer/GameMessegerServer/ShareDB2MessServerProtocolStructure.h"

class CPacketLocker;
class CGateway;
class CUserFinder; 

class CTaskProcessor: public i3Thread
{
	I3_CLASS_DEFINE(CTaskProcessor);
private	:
	BOOL		m_bIsRunning;
	UINT8	*	m_pui8RecvBuffer;		//추후 메모리 포인터 받는 것으로 수정한다. 
	UINT8	*	m_pui8SendBuffer;		//추후 메모리 포인터 받는 것으로 수정한다. 

	UINT32		m_ui32CheckUserIdx;
	UINT32		m_ui32DataSize;
	UINT32		m_ui32ReadedLength;

	CUserFinder		* m_pUserFinder; 
	CPacketLocker	* m_pPacketLocker;
	CGateway		* m_pGateway;
	
	char			m_strLogBuffer[MAX_STRING_COUNT];

	INT32			m_i32LogIdx;

	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
#ifdef USE_CHANGE_NICK	
	// Change Nick
	Local_CRingBuffer	* m_ppI_ChangeNick;

	UINT8	m_ui8CurrentChangeNickCount;
	INT32	m_i32DelayWoringChangeNickCount;
	

#endif
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
#ifdef USE_CHANGE_RANK	
	// Change Rank	
	Local_CRingBuffer	* m_ppI_ChangeRank;

	UINT8	m_ui8CurrentChangeRankCount;
	INT32	m_i32DelayWoringChangeRankCount;


#endif
	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------
	// Clan Group Note
	Local_CRingBuffer	* m_ppI_ClanGroupNote;

	UINT32	m_ui32CurrentGroupCount;
	INT32	m_i32DelayWoringGroupCount;	

	//-----------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------

private	: 
	BOOL _AddUser(UINT32 ui32ServerIdx, UINT32 ui32SessionIdx, INT64 i64UID, INT32 i32Rank, TTCHAR * pstrNickName); 
	BOOL _RemoveUser(UINT32 ui32ServerIdx, INT64 i64UID); 

	void _DoTaskForGameDataBase(UINT32 ui32idxBuffer);
	void _WorkForFriendList(void);
	void _WorkForFriendControl(void);
	void _WorkForBlockList(void);
	void _WorkForBlockControl(void);
	void _WorkForNoteList(void);
	void _WorkForNoteCommand(void);


#ifdef USE_CHANGE_NICK
	BOOL	_InChangeNick(OGame2Mess_Change_NickName_Req* pReq);
	INT32	_GetInChangeNick()
	{
		INT32 i32Rv = 0;
		i32Rv += m_ppI_ChangeNick->GetBufferCount();
		return i32Rv;
	}

	OGame2Mess_Change_NickName_Req *	_GetOutChangeNickName(void);

	INT32	_InWorkForChangeNick(void); 	
	void	_OutWorkForChangeNick(void);	// Send Game Server	

#endif
#ifdef USE_CHANGE_RANK
	BOOL	_InChangeRank(OGame2Mess_Change_Rank_Req stGame2Mess_Change_Rank_Req);
	INT32	_GetInChangeRank()
	{
		INT32 i32Rv = 0;
		i32Rv += m_ppI_ChangeRank->GetBufferCount();
		return i32Rv;
	}

	OGame2Mess_Change_Rank_Req *	_GetOutChangeRank(void);

	INT32	_InWorkForChangeRank(void); 	
	void	_OutWorkForChangeRank(void);	// Send Game Server

#endif
	BOOL	_InClanGroupNoteAddClan(INT8 i8Type, OGame2Mess_Clan_Group_Note_Send_Req stGame2Mess_Clan_Group_Note_Send_Req);
	INT32	_GetInClanGroupNoteCount()
	{
		INT32 i32Rv = 0;
		i32Rv += m_ppI_ClanGroupNote->GetBufferCount();
		return i32Rv;
	}

	OGame2Mess_Clan_Group_Note_Send_Req *	_GetOutClanGroupNoteCommand(void);
	
	INT32	_InWorkForClanGroupNoteCommand(void); 
	void	_OutWorkForClanGroupNoteCommand(void);	// Send Game Server

	//void _AccountKick(UINT8* pPacket);	

	BOOL _FindTatooWord(TTCHAR *pstrName);		// 금지어 검사(스페이스)

	BOOL _CheckBlock( INT64 i64SenderUID, INT64 i64RecverUID );	// 차단 여부 체크

	BOOL _CheckMyFriendByNick(  USER_NODE* pNode, INT64 i64FriendUID  );	// 자신의 친구인지 체크( UID )

public	:
	CTaskProcessor();
	virtual ~CTaskProcessor();

	BOOL Create(UINT32 ui32DataSize,CPacketLocker* pPacketLocker,CGateway* pGateway);
	void OnDestroy(void); 
	virtual UINT32 OnRunning(void* pUserData);
	BOOL SendPacketMessage(UINT32 ui32idx,i3NetworkPacket* pPacket);

	void _ProcessFriendInfo(INT64 i64UID);

	INT32 GetUserCount(void); 

};

extern CTaskProcessor* g_pTaskProcessor;

#endif //_TaskProcessor_h
