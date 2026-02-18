#pragma once

#ifdef BUILDMODE_KOREA

#include <WBANetwork/WBANetwork.h>

#include "BillEventHandler.h"

class i3RingBuffer;

class CLibClientSession : public CBillEventHandler
{
public:
	CLibClientSession();
	virtual  ~CLibClientSession();

private:
	i3RingBuffer*		m_pBuffer;

public:
	void SetCallBackBuffer( i3RingBuffer* pBuffer )			{ m_pBuffer = pBuffer; }

protected:		
	// 빌링서버의 연결 성공 여부
	// (새로 연결되면 현재 차감이 필요한 모든 유저의 정보를 다시 로그인해야한다.)
	//		success	: 연결 성공 여부
	//		error	: 에러코드
	void OnNetConnect( bool bSuccess, DWORD dwError );
	
	// 빌링서버로 보낸 데이타 전송 여부
	//		size	: 전송된 패킷 사이즈
	void OnNetSend( INT32 i32Size );
	
	// 빌링서버와 세션이 끊겼을때 (빌링서버가 죽은것으로 처리 해야한다.)	
	//		error	: 에러코드
	void OnNetClose( DWORD dwError );

	// 유저의 결재 정보가 변경되었을 때 전달받는 콜백
	// 정상 로그인의 경우 : (0 == nResultCode) && (0 == nAccessCheck) && (0 != nDeductType)
	void OnUserStatus(	DWORD	dwAccountGUID,				// 계정 GUID
						long	dwBillingGUID,				// 빌링 유니크 아이디
						DWORD	RealEndDate,				// 정액 사용 종료 일시 (__time32_t) 
						DWORD	EndDate,					// 정액 실제 종료 일시
						REAL64	r64RestPoint,				// 정량 잔여 포인트
						REAL64	r64RestTime,				// 정량 잔연 시간(초)
						INT16	i16DeductType,				// 사용자 차감 유형
						INT16	i16AccessCheck,				// 접근 체크 결과
						INT16	i16ResultCode );			// 처리 결과

	// PC 방 멀티 유저 조회 결과 
	//		Result : 1 (가능), 2 (불가), -1 (PC방 정보 없음)
	void OnInquireMultiUser(long	AccountID,				// 계정 GUID
							long	RoomGUID,				// PC 방 GUID
							long	Result);				// 처리 결과

	// 특정 PC방의 게임 별 정량 보유 여부 조회 결과
	void OnInquirePCRoomPoint(	long	AccountGUID,  		// 계정 GUID
								long	RoomGUID,			// PC 방 GUID
								long	GameCode,			// 게임 코드
								long	ResultCode );		// 처리 결과

	// 빌링에 로그인한 유저 정보를 인증서버에 전송
	// 실제로 인증서버에 로그인 했는지 체크하도록 알려주는 인터페이스
	// 정량에 대해서만 로그인 후 약 3분에 한번씩 전송.
	// 상품이 변경되는 시점과, 로그아웃 되면 전송되지 않는다.
	void OnCheckLoginUser(	long	AccountID,				// 계정 GUID
							long	BillingGUID );			// 빌링 유니크 아이디(로그인 성공 시 OnUserStatus 함수를 통해 전달된 값)

	// PC방 퍼블리싱
	// PC방 IP 여부 확인 결과
	// PC방 IP일 경우 PC방 GUID를 반환한다.
	void OnCheckPCRoomIPAddr(	DWORD dwIPAddress,			// 클라이언트 아이피 주소
								DWORD dwAccountGUID,		// 계정 GUID
								DWORD dwRoomGUID,			// PC 방 GUID (확인되지 않으면 0)
								INT16 i16ResultCode,		// 처리 결과
								DWORD dwReserved1,
								DWORD dwReserved2,
								DWORD dwReserved3);

	// 정량 보유중인 PC방의 GUID와 게임 코드를 알림.
	// 정량 만료에 의해 자동 로그아웃 처리된 PC방에 한정하여 자동으로 정량 보유 여부를 확인하여
	// 정량 보유중일 경우에만 발생한다.
	void OnPCRoomDeductUpdate(	long RoomGUID,					// PC 방 GUID
								long GameCode );				// 게임 코드

private:
	void WriteLog(char* szMsg) {}
};

#endif