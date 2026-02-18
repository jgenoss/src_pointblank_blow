#pragma once

#include "ClanMatchDefine.h"

//#define DEFAULT_CLAN_POINT		1000.0



//enum RECORD_TYPE
//{
//	RECORD_TOTAL		= 0,	// 전체 누적 기록
//	RECORD_PRE_SEASON	,		// 지난 시즌 기록
//	RECORD_CUR_SEASON	,		// 현재 시즌 기록
//
//	RECORD_TYPE_COUNT	,
//};






//class CRecord
//{
//public:
//	void				InitRecord();
//
////	void				SetRecord(CLAN_RECORD* pRecord);
//	void				SetRecord(INT32 i32MatchCnt, INT32 i32WinCnt, INT32 i32LoseCnt);
//	void				GetRecord(CLAN_RECORD* pClanRecord);
//
//	CRating&			GetClanRating()							{ return m_ClanRating; }
//	void				SetClanRating(CRating ClanRating)		{ m_ClanRating = ClanRating; }
//
//
//public:
//	CRecord();
//	virtual ~CRecord();
//
//
//private:
//	INT32		m_i32Match;			// 전
//	INT32		m_i32Wins;			// 승
//	INT32		m_i32Defeats;		// 패
//
//	CRating		m_ClanRating;		// 클랜 포인트
//};
//
//
//
//class CClanRecord
//{
//public:
//	void				InitRecord(RECORD_TYPE eRecordType);
////	void				SetRecord(RECORD_TYPE eRecordType, CLAN_RECORD* pRecord);
//
//	bool				GetClanRecord(RECORD_TYPE eRecordType, CLAN_RECORD* pClanRecord);
////	void				SetClanRecord(RECORD_TYPE eRecordType, CLAN_RECORD* pClanRecord);
//	void				SetClanRecord(RECORD_TYPE eRecordType, INT32 i32MatchCnt, INT32 i32WinCnt, INT32 i32LoseCnt);
//	void				IncClanRecord(CLAN_RECORD* pMatchResult);
//
//	CRating&			GetClanRating(RECORD_TYPE eRecordType);
//	void				SetClanRating(RECORD_TYPE eRecordType, CRating& Rating);
//
//
//public:
//	CClanRecord();
//	virtual ~CClanRecord();
//
//
//private:
//	CRecord				m_ClanRecord[RECORD_TYPE_COUNT];
//};
