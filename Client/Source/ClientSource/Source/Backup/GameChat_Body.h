#ifndef __GAMECHATBODY_H__
#define __GAMECHATBODY_H__



enum CHAT_BODY_STATE
{
	CHAT_BODY_STATE_LOBBY = 0,
	CHAT_BODY_STATE_READYROOM,
	CHAT_BODY_STATE_CLAN_MATCH,
	CHAT_BODY_STATE_CLAN_MEMBER,

	CHAT_BODY_STATE_COUNT,
};

#define		CHAT_CHANNEL_BUTTON_COUNT		4

class CGameChatBody : public i3GameObjBase
{
	I3_CLASS_DEFINE( CGameChatBody, i3GameObjBase);

public:
	i3GuiList*			m_pChatList;				// 채팅 리스트
	i3GuiEditBox*		m_pChatCombo;				// 
	i3GuiButton*		m_pChatButton;
	i3GuiPopupList*		m_pChatPopup;
	i3GuiEditBox*		m_pChatBox;
	i3GuiStatic*		m_pChatLang;
	i3GuiEditBox*		m_pTextBox;
	i3GuiButton*		m_pChatChannelButton[CHAT_CHANNEL_BUTTON_COUNT];

protected:
	INT32 				m_nChatType;
	INT32 				m_nOldChatType;
	INT32 				m_nMaxChatLineCount;
	CHAT_BODY_STATE		m_ChatBodyState;
	BOOL				m_bOnceChatType;
	BOOL				m_bOnceChatTypeAll;
	UINT16				m_nCurrentChatBufferType;
	INT32				m_nCurrentSelectedChatType;

public:
	CGameChatBody(void);
	~CGameChatBody(void);

	//===========================================================================
	// 반드시 컨트롤들의 연결이 끝난후 실행되어야 합니다.
	void			InitChatBody( CHAT_BODY_STATE state, INT32 nMaxLineCount);
	//===========================================================================

	void			OnUpdate( REAL32 rDeltaSeconds, void* pPointer);
	void 			UpdateChatFocus(void);
	void 			UpdateChatType(void);
	void			SetEntranceStart(void * pPointer);
	void			OnExitEnd(void);
	void			OnWhisper( INT32 slotIdx);
	void			OnWhisper( const char * pszNick);
	void			OnWheelUpdate(INT32 scrollby);
	void			OnDragChatList(void);
	void			OnChatPopup(UINT32 idxItem);
	void			OnChatChannel(UINT32 idxItem);
	INT64			FindUserUIDByNick(char* nick);

protected:
	void 			_InitChat(CHAT_BODY_STATE state);	
	void 			_UpdateMacroChat(void);
	void 			_UpdateLang(void);
	void 			_UpdateParsingType(void);
	void 			_UpdateChatBuffer(void);
	void			_UpdateWhisperBackupBuffer(void);
	void			_UpdateChatChannelType(void);

};

#endif // __GAMECHATBODY_H__