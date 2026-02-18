#ifndef __MAIN_STREAM_BUFFER_LOGIN_H__
#define __MAIN_STREAM_BUFFER_LOGIN_H__

class CHashTable;

class CMainStreamBuffer_Login
{
private:
	INT32		m_i32TotalBufferCount;			// Create시 생성한 버퍼수
	INT32		m_i32UseBufferCount;			// 사용중인 버퍼수

	INT32		m_i32BlankIdx;
	INT32		m_i32PushIdx;
	INT32*		m_pi32BufferIndex;

	CHashTable*	m_pUIDTable;

	MAIN_STREAM_BUFFER_LOGIN*	m_pBuffer;

	INT32		m_i32UpdateIdx;
	
	INT32		_GetNextBufferIdx();								// 빈버퍼 인덱스 구하기
	void		_SetBlankBuffer( INT32 i32BufferIdx );				// 빈버퍼로 만듬

	MAIN_STREAM_BUFFER_LOGIN*	_FindByUID( T_UID i64UID );			// UID로 버퍼 찾음

public:
	CMainStreamBuffer_Login();
	virtual ~CMainStreamBuffer_Login();

	INT32		CreateBuffer( INT32 i32Count );

	INT32		Push( UINT32 ui32ServerIdx, LOGIN_INPUT* pInput );	// 빈버퍼에 데이터 삽입

	BOOL		SetRecvResult( LOGIN_OUTPUT* pInput );				// 외부 연동 결과 버퍼에 반영 - 로그인워커
	BOOL		SetRecvResult( PCCAFE_OUTPUT* pInput );				// 외부 연동 결과 버퍼에 반영 - PC카페워커

	BOOL		UpdateBuffer();										// 처리 가능한 Buffer를 Trans로 전송, Timeout 선별
	BOOL		SendResultData( INT32 i32BufferIdx );				// 트랜스서버에 결과 보냄
};

#endif