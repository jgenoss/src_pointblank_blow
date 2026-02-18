#pragma once

#include "UIHUDBase.h"
#include "UIFloatChat.h"

class UIHUDIngameChat : public UIHUDBase
{
	I3_CLASS_DEFINE(UIHUDIngameChat, UIHUDBase);

public:
	static UIHUDIngameChat* instance();

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIHUDIngameChat();
	virtual ~UIHUDIngameChat();

	virtual void	OnCreate( i3GameNode * pParent) override;
	
	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;
	virtual	bool	OnKey_Enter( void) override;

	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual	void OnLoadAllScenes() override;
	virtual void OnEntranceEnd() override;

	virtual bool	OnExitStart( void) override;

protected:
	i3InputKeyboard* m_keyboard_mgr;
	CGameChatBox* m_chatbox_mgr;

protected:
	i3UIFrameWnd *	m_pChatMainBox;
	i3UIEditBox*	m_pChatBox;

	i3UIFrameWnd *	m_pSubChatMain;
	i3UIEditBox*	m_pTextBox;
	i3UIImageBox* m_pTextBg;

protected:
	INT32 m_nChatType, m_nOldChatType;

	bool m_bOnceChatType, m_bOnceChatTypeAll;
	bool m_bUseHistory, m_bUseOneTimeTeamChat;

	char m_lastWhisperUserNick[ NET_NICK_NAME_SIZE];

	CHAT_TYPE		m_Tab[CHAT_TYPE_MAX];
	CHAT_TYPE		m_CurrentTab;

	bool			m_bEnableChat;
	bool			m_bChatScrollEnable;
	bool m_bNothingText_inChatBox;

private:
	void setting_language();
	bool enable_whisper_chat(const i3::rc_wstring & wstr_nick);

public:
	void re_position();

protected:
	// Chatting
	void 			_UpdateMacroChat(void);
	void 			_UpdateLang(void);
	void 			_UpdateParsingType( EDIT_STATE state);
	void			_UpdateWhisperBackupBuffer(void);
	void			_UpdateChatType( void);
	void			_CopyChatBufferToBox(i3::wstring& inout_wstr, INT32 bufferIdx);

public:
	/** \brief 업데이트 돕니다. */
	void			OnUpdate(REAL32 rDeltaSeconds);

	/** \brief Chatting View Channel을 선택합니다. */
	void			SelectChatChannel( INT32 idx);

	/** \brief Chat 입력 그룹을 설정합니다. */
	void			SetChatType( INT32 type);

	void			OnWhisper( INT32 slotIdx);
	void			OnWhisper( const i3::rc_wstring& wstrNick);

	/** \brief Chatting Buffer의 내용을 출력한다. */
	void 			UpdateChatBuffer(void);

	void			_ChatHistory(EDIT_STATE state);

	void			_ToWhisperChatMessage(i3::wstring& out, const i3::rc_wstring& target, const i3::wliteral_range& msg = L"");
	void			_GetOnlyChatMessage(i3::stack_wstring& out,	const i3::wliteral_range& wfullMsgRng);
	//--------------------------------------------------------------------------//
	//								User List									//
	//--------------------------------------------------------------------------//

	/**		Ingame ChatBox Update 컨트롤	*/
	void			UpdateHistoryChatBuffer(REAL32 rDeltaSeconds);

	/**		Ingame onupdate 컨트롤 */
	void			SetEnableChat(bool bchat);

	bool GetEnableChat()  const	{		return m_bEnableChat;	}
	bool GetEnableChatScroll() const {	return m_bChatScrollEnable; }

	void			SetEnableChatScroll( bool bFlag);
	bool			TestCommand(const i3::wliteral_range& wCmdRng);
	bool IsGhostMessage( void);

public:
	void PrintStartingMessage();
};
