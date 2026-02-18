#pragma once


// Key : GM 강화 기능 추가
/************************************************************************/
/* GM Chatting										                   */
/************************************************************************/

#define MAX_GM_CHAT_BUFFER		10
#define MAX_GM_CHATBOX_BUFF		MAX_GM_CHAT_BUFFER * (MAX_STRING_COUNT + MAX_STRING_COUNT + 10)

class CGMChatBuffer
{
private:
	i3::svector<i3::rc_wstring>	m_vChatBuffer;

	INT32						m_i32BufferCount;
	INT32						m_i32CurBufferIdx;
public:
	CGMChatBuffer(void);
	~CGMChatBuffer(void);

	void	PutChatting(const i3::rc_wstring & str);
	void	CopyChatBuffer(i3::wstring& Out);
	void	ResetBuffer(void);

	bool	isFirstChat(void)	{ return (m_i32BufferCount == 0); }
};
