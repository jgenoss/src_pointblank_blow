#pragma once


// static object는 초기화부터 5초 동안 보내고 반복하지 않는다
// animation, controlled, mission object는 1초에 한번씩 보낸다. 
class CStageInfoTimer
{
public:
	BOOL	IsInitialized()
	{
		return m_bInitialized;
	}
	
	
	void	ResetTimer( REAL32 r32GameTime )	// network available 상태(유효데이터 도착)가 되면 호출
	{
		m_fStageInfoSentTime_Static	= r32GameTime;
		m_fStageInfoSentTime_Other	= r32GameTime;
		m_bInitialized = TRUE;
	}
	
	UINT32	SelectTimer( REAL32 r32GameTime ) // 어떤 정보를 보낼지 선택하는 함수
	{
		UINT32 ui32State = 0;

		if( m_bInitialized )
		{
			if( r32GameTime - m_fStageInfoSentTime_Static  < 5.0f ) // n초동안 계속 보내야 한다(1회).
			{
				ui32State |= ( 1 << SEND_HEAD_TYPE_OBJ_STATIC );
			}

			if( r32GameTime - m_fStageInfoSentTime_Other > 1.0f ) // 1초에 한번씩만 보내도록 합니다(계속).
			{
				ui32State |= ( 1UL << SEND_HEAD_TYPE_CHAR );
				ui32State |= ( 1UL << SEND_HEAD_TYPE_OBJ_ANIM );
				ui32State |= ( 1UL << SEND_HEAD_TYPE_OBJ_CONTROLED	);
				ui32State |= ( 1UL << SEND_HEAD_TYPE_MISSION  );

				if( 0 < g_pConfig->GetCheckLevel(HACK_TYPE_USURPATION) )
				{
					ui32State |= ( 1UL << SEND_HEAD_TYPE_OBJ_MOVE );
				}
				m_fStageInfoSentTime_Other = r32GameTime;

				//I3TRACE("SIGNAL:%f\n", fGameTime );
			}
		}

		return ui32State;
	}

	void	Init()	
	{
		m_fStageInfoSentTime_Static	= 0.0f;
		m_fStageInfoSentTime_Other	= 0.0f;
		m_bInitialized = FALSE;
	}

public:
	CStageInfoTimer(void);
	~CStageInfoTimer(void);

private:
	// 난입시 모든 유저에게 stageinfo static을 조금씩 보냄
	// 심플하게 처리하기 위해 인덱스/카운터 방식 대신 시간으로 결정.
	REAL32					m_fStageInfoSentTime_Static;
	REAL32					m_fStageInfoSentTime_Other;
	BOOL					m_bInitialized;				// stageinfo를 보낼 수 있을 때
};
