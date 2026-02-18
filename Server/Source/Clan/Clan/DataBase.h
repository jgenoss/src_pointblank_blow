#ifndef _DATABASE_H__
#define _DATABASE_H__


#include "RingBuffer.h"
#include "DataBase_Script.h"


class i3ODBC; 
class CADODatabase; 
class CRankingManager;

typedef struct _DataQueryInfo
{
	CADODatabase	*	_pDB;
	wchar_t				_pQuery[SQL_STRING_COUNT];
}DB_QUERY_INFO;

// 멤버 기본 정보
/*-----------------------------------------------------------------------
Name : CDataBase
2007 03 06 : 버그확인 : 버그없음 : Query 버퍼를 모두 512로 변경

-----------------------------------------------------------------------*/
class CDataBase :	public i3ElementBase
{
	I3_CLASS_DEFINE( CDataBase );

	CDBConfig*			m_pClanDBConfig;

	// 추가
	DB_QUERY_INFO*		m_pTaskDB;
	INT32*				m_pi32ThreadIdx;
	HANDLE*				m_phThread;
	BOOL*				m_pbRunning;

	NSM_CRingBuffer**	m_ppRingInDB;
	NSM_CRingBuffer**	m_ppRingOutDB;

private:
	INT32*				m_pi32LogDataBaseIdx;

public:
	CDataBase(void);
	~CDataBase(void);
	
	NSM_CRingBuffer*	GetRingOutDB( INT32 i32Idx )			{	return m_ppRingOutDB[ i32Idx ];	}

	BOOL			OnCreate(); 
	void			OnDestroy(void);

	INT32			InsertWorking( UINT32 ui32ClanDBIdx, void* pData );
	INT32			InsertWorkingUID( INT64 i64UID, void* pData );

	void			OnWorking( INT32 i32TheadIdx );
	BOOL			OnWorkingClan( INT32 i32TheadIdx, void* pData );

	INT32			LoadClanDetailData(CLAN_DETAIL_INFO_SERVER* pDetail, i3BinListEx* pList);

	INT32			LoadSeasonRanking(INT32 i32SeasonIdx, DB_SORT_TYPE  eSortType, CLAN_BASIC_INFO* pClanInfo );

	INT32			LoadClanItem( );
	
	INT32			LoadMemberData();
	INT32			LoadRequestData();
	INT32			LoadClanWarResultData();

	INT32			ClanClose( UINT32 i32UserDBIdx, UINT32 ui32ClanDBIdx );
	INT32			ClanSecession( UINT32 i32UserDBIdx, UINT32 ui32ClanDBIdx );
	
	BOOL			_UserLogin(				INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );

	BOOL			_ClanCreate(			INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			_ClanDestory(			INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );

	BOOL			ClanRequestJoin(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanRequestAccept(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanRequestCancel(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanRequestDenial(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	
	BOOL			ClanMemberSecession(	INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanMemberDeportatio(	INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );

	BOOL			ClanInvite(				INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanInviteAccept(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );

	BOOL			ClanCommissionMaster(	INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanCommissionStaff(	INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanCommissionRegular(	INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );

	BOOL			ClanGetMemberInfo(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanGetRequestInfo(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );

	BOOL			ClanReplaceNotice(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanReplaceIntro(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanReplaceClanName(	INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanReplaceMark(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanReplacePersonMax(	INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanReplaceManagement(	INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );

	BOOL			ClanReplaceMemberNick(	INT32 i32TheadIdx, RING_IN_DB* pInDB );
	BOOL			ClanReplaceMemberNickColor(	INT32 i32TheadIdx, RING_IN_DB* pInDB );
	BOOL			ClanReplaceMemberRank(	INT32 i32TheadIdx, RING_IN_DB* pInDB );

	BOOL			ClanCheckName(			INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanCheckAzit(			INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );

	BOOL			ClanItemPeriod(			INT32 i32TheadIdx, RING_IN_DB* pInDB );
	BOOL			ClanItemResetRecord(	INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanItemResetPoint(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );
	BOOL			ClanItemDelete(			INT32 i32TheadIdx, RING_IN_DB* pInDB );
	
	BOOL			ClanSetActiveState(		INT32 i32TheadIdx, RING_IN_DB* pInDB );

	// Battle Server
	BOOL			ClanBattleResult(		INT32 i32TheadIdx, RING_IN_DB* pInDB );
	BOOL			ClanBattleResultUser(	INT32 i32TheadIdx, RING_IN_DB* pInDB );

	BOOL			ClanMemberOrder(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );

	void			ErrorDB(		INT32 i32TheadIdx, INT32 i32LogNo = 0 );

	UINT32			GetAddAuthTime(UINT32 ui32Date, UINT32 ui32Sec);
	BOOL			ClanFindUserInfo(		INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB );


	BOOL			UpdateClanMatchRecord(INT32 i32ThreadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB);
	BOOL			UpdateMercenaryMatchRecord(INT32 i32TheadIdx, RING_IN_DB* pInDB);
	BOOL			UpdateMercenaryDisconnect(INT32 i32ThreadIdx, RING_IN_DB* pInDB);

	BOOL			LoadMercenary(INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB);
	BOOL			LoadMercenaryPenaltyTime(INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB);
};

extern CDataBase * g_pDataBase;

#endif 

