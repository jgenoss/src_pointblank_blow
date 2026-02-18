#ifndef _GENERAL_RANK_UP_H__
#define _GENERAL_RANK_UP_H__

#include "S2RingBuffer.h"

#define MIN_LIMIT_RANK					RANK_47
#define MAX_LIMIT_RANK					RANK_51

enum GENERAL_RANK_STATE
{
	GENERAL_RANK_STATE_WAIT,
	GENERAL_RANK_STATE_GET_DB,
	GENERAL_RANK_STATE_READ,
	GENERAL_RANK_STATE_CHECK_RANK,
	GENERAL_RANK_STATE_READ_DEMOTION,
	GENERAL_RANK_STATE_WRITE_DEMOTION_RANK,
	GENERAL_RANK_STATE_WRITE_BRIGADIER_RANK,
	GENERAL_RANK_STATE_WRITE,
};

struct GENERAL_RANK_DATA
{
	T_UID					m_TUID;								// 유저의 고유번호
	UINT32					m_ui32Exp;							// 경험치
	UINT32					m_ui32Rank;							// 계급
	UINT32					m_ui32ChangeRank;					// 변경 될 계급
};

class CGeneralRankUP :	public i3Thread
{
	I3_CLASS_DEFINE( CGeneralRankUP );

private :
	BOOL					m_bIsRunning;
	
	CDBConfig*				m_pUserDBConfig;
	CADODatabase *			m_pDB;

	INT32					m_i32GeneralRankDataCount;
	GENERAL_RANK_DATA*		m_pGeneralRankData;

	INT32					m_i32UpdateGeneralRankIdx;

	GENERAL_RANK_STATE		m_eRunState;
	
	UINT32					m_ui32TotalRankCount;
	UINT32 					m_ui32Count[ GENERAL_RANK_COUNT ];			// 소장 계급 제한 인원수
	UINT32 					m_ui32Rank[ GENERAL_RANK_COUNT ];			// 소장 계급 제한 인원수
	UINT32 					m_ui32Exp[ GENERAL_RANK_COUNT ];			// 소장 계급 제한 인원수

	INT32					m_i32StartHour;
	INT32					m_i32RunDay;
	INT32					m_i32WriteIdx;

	S2RingBuffer*		m_pOutBuffer;

	UINT32					m_ExpValue[ DEF_RANK_COUNT ]; 

private :

	BOOL					_GetGeneralRankUser();
	void					_ReadGeneralRankUser( BOOL& bContinue );

	BOOL					_GetRankData();								// 장성 계급의 상위 인원 정보 획득
	BOOL					_CheckGeneral();
	BOOL					_UpdateRankUP( BOOL& bContinue );			// 상위 계급 인원 설정
	BOOL					_UpdateBrigadierRank();
	BOOL					_GetDemotionGeneralUser();
	BOOL					_WriteGeneralDemotion( BOOL& bContinue );

public:

	CGeneralRankUP(void);
	~CGeneralRankUP(void);

	BOOL					OnCreate();	
	void					OnDestroy(void);

	virtual UINT32			OnRunning( void * pUserData );

	void					ErrorDBRankUP( wchar_t *wstrLog );

	void					SetStartHour(INT32 starthour);

	inline INT32			GetOutBufferCount()		
	{ 
		if( NULL == m_pOutBuffer )
			return NULL;

		return m_pOutBuffer->GetBufferCount();
	}

	inline GENERAL_RANK_RESULT*	GetOutBuffer()			{ return (GENERAL_RANK_RESULT*)m_pOutBuffer->Pop(); }
	inline void					GetOutBufferIdx()		{ return m_pOutBuffer->PopIdx(); }

};

#endif //_GENERAL_RANK_UP_H__