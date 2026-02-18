#ifndef __PCCAFEINFO_H__
#define __PCCAFEINFO_H__

#ifdef I3_DEBUG
	#define PCCAFE_CHARGE_CHECK_TIME_SECOND		10		// 정량이 생겼는지 확인할때, 확인 간격 시간(초)
#else
	#define PCCAFE_CHARGE_CHECK_TIME_SECOND		180		// 정량이 생겼는지 확인할때, 확인 간격 시간(초)
#endif
#define PCCAFE_LOGIN_TIMEOUT				30		// 로그인 요청후 로그인 결과가 타임아웃 확인값(초)

// 상태다이어그램: https://docs.google.com/a/zepetto.com/drawings/d/1qFS8lZ5LN6iPRrcrBGmGdiON2PpEpAwHriTmaKNTSWI/edit?usp=sharing
enum PCCAFE_WORK_STATE
{
	PCCAFE_WORK_STATE_NONE = 0,			// 없음
	PCCAFE_WORK_STATE_LOGIN,			// 웹젠서버에 로그인 하려고 하는 상태. 이 다음 상태는 _LOGIN_REQ이다.
	PCCAFE_WORK_STATE_LOGIN_REQ,		// 웹젠서버에 로그인 요청한 상태. 성공시 _NONE상태로 가고. 실패시 _LOGIN상태로 간다.
	PCCAFE_WORK_STATE_CHARGE_CHECK,		// 웹젠서버에 정량이 생겼는지 확인하는 상태. 정량이 생긴경우 _LOGIN상태로 간다.
	PCCAFE_WORK_STATE_REMOVE_RIGHT,		// 웹젠서버와 연결이 단절되어 게임서버에 PC방 혜택을 제거하려고 하는 상태. 게임서버에 패킷 전송후 _NONE 상태로 간다.
};

class CPCCafeInfo
{
private:
	UINT32				m_ui32PCCafeGUID;		// 웹젠서버에서 주는 PC방 UID, 비PC방일 경우 0
	INT32				m_i32BillingGUID;		// 로그인 성공시 웹젠에서 주는 계정 UID( PB UID와 다름 ) : 로그아웃시 사용
	INT8				m_i8WorkState;			// enum PCCAFE_WORK_STATUS
	UINT32				m_ui32ChargeCheckTime;	// CHARGE_CHECK 상태일때 3분 체크를 하기 위한 시간변수
	UINT32				m_ui32StateTime;		// 상태가 설정되었을때의 시간

public:
	CPCCafeInfo(void);
	~CPCCafeInfo(void);

	void				SetPCCafeGUID( UINT32 ui32PCCafeGUID )	{ m_ui32PCCafeGUID = ui32PCCafeGUID;	}
	void				SetBillingUID( UINT32 ui32BillingUID )	{ m_i32BillingGUID = ui32BillingUID;	}
	void				SetState( PCCAFE_WORK_STATE eState );
	void				SetChargeCheckTime();
	PCCAFE_WORK_STATE	GetState()								{ return (PCCAFE_WORK_STATE)m_i8WorkState; }
	UINT32				GetPCCafeGUID()							{ return m_ui32PCCafeGUID; }
	UINT32				GetPCCafeBillingGUID()					{ return m_i32BillingGUID; }
	BOOL				IsChargeCheckTime();
	BOOL				CheckTimeOut();
};

#endif

