#pragma once

#include "MercenaryMgr.h"

//////////////////////////////////////////////////////////////////////////
// CTeamMember
class CTeamMember
{
private:
	INT32						m_i32Slot;	// ¸ÅÄ¡ÆÀ Slot Idx.

public:
	INT32						GetSlot(void)			{ return m_i32Slot; }
	void						SetSlot(INT32 i32Slot)	{ m_i32Slot = i32Slot; }

	virtual T_UID				GetUserID() = 0;
	virtual INT32				GetSessionIdx() = 0;

	// Mercenary
	virtual bool				IsMercenary()						{ return false; }
	virtual bool				GetSendMerRankUp()					{ return false; }
	virtual void				SetSendMerRankUp(bool bSend)		{ return ; }
	virtual UINT8				GetMerRank()						{ return 0; }
	virtual UINT8				GetMerClass()						{ return 0; }

	virtual void				JoinTeam(CLAN_DETAIL_INFO_SERVER* pOwnerClan, T_TeamID TTeamID ) {}
	virtual void				LeaveTeam(CLAN_DETAIL_INFO_SERVER* pOwnerClan )	{}

	virtual void				MakePacketMemeber(i3NetworkPacket* pPacket ) = 0;
	virtual	void				MakePacketMemberUID(i3NetworkPacket* pPacket ) = 0;
	virtual	void				MakePacketMatchMakingRecord(i3NetworkPacket* pPacket ) = 0;
	virtual INT32				GetClanKillDeathRate(void)	{ return 0; }
	virtual INT32				GetClanWinRate(void)		{ return 0; }

	void						SetKillDeathRate(INT32 i32KillDeathRate) { m_i32KillDeathRate = i32KillDeathRate; }
	INT32						GetKillDeathRate(void) { return m_i32KillDeathRate; }

	virtual CLAN_MEMBER_DETAIL_INFO* GetClanMember(void) { return NULL; }

	virtual BOOL				ChangeMemberInfo(CHANGE_INFO	eType, MATCH_MEMBER_INFO *pMemberInfo ) { return TRUE; }

public:	
	CTeamMember();
	virtual ~CTeamMember();	

private:
	INT32						m_i32KillDeathRate;
};



//////////////////////////////////////////////////////////////////////////
// CMemberClan
class CClanMember : public CTeamMember
{
public:
	virtual T_UID				GetUserID()			{ return m_pMember ? m_pMember->_i64UID : INVALID_IDX; }
	virtual INT32				GetSessionIdx()		{ return m_pMember ? m_pMember->_session : INVALID_IDX; }

	virtual void				JoinTeam(CLAN_DETAIL_INFO_SERVER* pOwnerClan, T_TeamID TTeamID );
	virtual void				LeaveTeam(CLAN_DETAIL_INFO_SERVER* pOwnerClan );

	virtual void				MakePacketMemeber(i3NetworkPacket* pPacket );
	virtual	void				MakePacketMemberUID(i3NetworkPacket* pPacket );
	virtual	void				MakePacketMatchMakingRecord(i3NetworkPacket* pPacket );


	// °³ÀÎÀÇ Å¬·£ Å³µ« ºñÀ²
	virtual INT32						GetClanKillDeathRate(void)	
	{ 
		if( 0 ==  m_pMember->m_kill + m_pMember->m_death ) return 0;
		return m_pMember->m_kill / ( m_pMember->m_kill + m_pMember->m_death ) ; 
	}
	
	// °³ÀÎÀÇ Å¬·£ ½Â·ü
	virtual INT32						GetClanWinRate(void)
	{ 
		if( 0 ==  m_pMember->m_win + m_pMember->m_lose ) return 0;
		return m_pMember->m_win / ( m_pMember->m_win + m_pMember->m_lose ) ; 
	}

	virtual CLAN_MEMBER_DETAIL_INFO* GetClanMember(void) { return m_pMember; }
	virtual BOOL				ChangeMemberInfo(CHANGE_INFO	eType, MATCH_MEMBER_INFO *pMemberInfo ) ;
	
public:
	CClanMember(CLAN_MEMBER_DETAIL_INFO* pClanMember);
	virtual ~CClanMember();


private:
	CLAN_MEMBER_DETAIL_INFO*	m_pMember;
	
};



//////////////////////////////////////////////////////////////////////////
// CMemberMer
//class CMercenary;

class CMercenaryMember : public CTeamMember
{

public:
	virtual T_UID				GetUserID()			{ return m_pMercenary ? m_pMercenary->GetUserID() : INVALID_IDX; }
	virtual INT32				GetSessionIdx()		{ return m_pMercenary ? m_pMercenary->GetSessionIdx() : INVALID_IDX; }

	virtual bool				IsMercenary()		{ return true; } 
	
	virtual bool				GetSendMerRankUp()				{ return m_pMercenary ? m_pMercenary->GetSendMerRankUp() : false ; }
	virtual void				SetSendMerRankUp(bool bSend)	{ if( m_pMercenary) m_pMercenary->SetSendMerRankUp( bSend ); }
	virtual UINT8				GetMerRank()					{ return (UINT8)(m_pMercenary ? m_pMercenary->GetMerRank() : 0) ; }
	virtual UINT8				GetMerClass()					{ return (UINT8)(m_pMercenary ? m_pMercenary->GetMerClass() : 0) ; }

	virtual void				MakePacketMemeber(i3NetworkPacket* pPacket );
	virtual	void				MakePacketMemberUID(i3NetworkPacket* pPacket );
	virtual	void				MakePacketMatchMakingRecord(i3NetworkPacket* pPacket );
	virtual void				LeaveTeam(CLAN_DETAIL_INFO_SERVER* pOwnerClan );
	virtual void				JoinTeam(CLAN_DETAIL_INFO_SERVER* pOwnerClan, T_TeamID TTeamID );
	virtual BOOL				ChangeMemberInfo(CHANGE_INFO	eType, MATCH_MEMBER_INFO *pMemberInfo ) ;
	
private:
	void						Initiailze();


public:
	CMercenaryMember(CMercenary* pMercenary);
	virtual ~CMercenaryMember();

		
private:
	CMercenary*					m_pMercenary;
};