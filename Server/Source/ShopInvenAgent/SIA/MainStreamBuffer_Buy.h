#ifndef __MAIN_STREAM_BUFFER_BUY_H__
#define __MAIN_STREAM_BUFFER_BUY_H__

class CMainStreamBuffer_Buy
{

private:
	UINT16						m_ui16TotalBufferCount;
	UINT16						m_ui16UseBufferCount;

	UINT16						m_ui16BlankIdx;
	UINT16						m_ui16PushIdx;
	UINT16*						m_pBufferIndex;

	MAIN_STREAM_BUFFER_BUY*		m_pBuffer;

	UINT16						_GetNextBufferIdx();
	void						_SetBlankBuffer( UINT16 ui16BufferIdx );	

	BOOL						_SendResultData( UINT16 ui16BufferIdx );

public:
	CMainStreamBuffer_Buy();
	virtual ~CMainStreamBuffer_Buy();

	BOOL						CreateBuffer( UINT32 ui32Count );

	//Push 함수 MAIN_STREAM_BUFFER_BUY 내 union에 묶여있는 구조체 종류만큼 함수 오버로딩으로 정의해 줍니다.
	INT32						Push(  UINT32 ui32ServerIdx, PACKET_BUY_GOODS_REQ* pBuyReq, NET_PACKET_BUFFER *pPacket );

	
	//외부 연동 결과 버퍼에 반영
	BOOL						SetRecvResult( BILLING_OUTPUT* pBilling_Out, BOOL bIsAsyncOutput );

	//처리 가능한 Buffer를 Trans로 전송, Timeout 선별, etc......
	BOOL						UpdateBuffer();

};

#endif