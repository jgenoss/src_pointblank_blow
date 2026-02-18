#include "pch.h"
#include "GMChatBuffer.h"
#include "i3Base/string/ext/insert_string.h"

// Key : GM 강화 기능 추가
/************************************************************************/
/* GM Chating Box                                                       */
/************************************************************************/

CGMChatBuffer::CGMChatBuffer(void) : m_vChatBuffer(MAX_GM_CHAT_BUFFER),
m_i32BufferCount(0), m_i32CurBufferIdx(0)
{

}

CGMChatBuffer::~CGMChatBuffer(void)
{

}

void CGMChatBuffer::PutChatting(const i3::rc_wstring & str)
{
	m_vChatBuffer[m_i32CurBufferIdx] = str;

	// 최대 200개까지 버퍼에 저장
	if(m_i32BufferCount < MAX_GM_CHAT_BUFFER )
		++m_i32BufferCount;

	// 200개가 가득 차면 다시 0번부터 시작
	++m_i32CurBufferIdx;

	if(m_i32CurBufferIdx >= MAX_GM_CHAT_BUFFER )
		m_i32CurBufferIdx = 0;
}

void CGMChatBuffer::CopyChatBuffer(i3::wstring& Out)
{
	INT32 BufferIdx= 0;

	if( m_i32CurBufferIdx == 0)
		BufferIdx = MAX_GM_CHAT_BUFFER - 1;
	else
		BufferIdx = m_i32CurBufferIdx - 1;

	// Buffer를 돌면서 출력할 글자를 담습니다.

	INT32 nLoopCount = 0;
	i3::custom_stack_wstring<MAX_GM_CHATBOX_BUFF>::type	wstrTotalText;

	for( INT32 idx = BufferIdx; nLoopCount < m_i32BufferCount; --idx, ++nLoopCount)
	{
		if( idx < 0)
			idx = MAX_GM_CHAT_BUFFER - 1;

		i3::custom_stack_wstring<256>::type  strChatBoxText = m_vChatBuffer[idx];

		if( !strChatBoxText.empty() )
		{
			strChatBoxText += L'\n';
			i3::insert_string(wstrTotalText, 0, strChatBoxText);
		}

		// 채팅 로그에 출력되는 메세지를 100개로 제한
		if(nLoopCount < 0 || nLoopCount >= MAX_GM_CHAT_BUFFER)
			break;
	}

	Out = wstrTotalText;
}

void CGMChatBuffer::ResetBuffer(void)
{
	for(size_t idx = 0 ; idx < m_vChatBuffer.size() ; ++idx)
		m_vChatBuffer[idx].clear();

	m_i32BufferCount = 0;
	m_i32CurBufferIdx = 0;
}
