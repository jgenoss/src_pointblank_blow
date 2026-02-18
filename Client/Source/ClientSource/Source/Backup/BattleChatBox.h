#ifndef _BattleChatBox_h
#define _BattleChatBox_h

#if 지워질_것들

#define GAME_CHAT_LINE_COUNT	8

enum HUD_SPRITE_CHAT_INPUT
{
	HUD_SPRITE_CHAT_INPUTBOX,
	HUD_SPRITE_CHAT_MODE,
	HUD_SPRITE_CHAT_TARGET_CHANGE_PREV,
	HUD_SPRITE_CHAT_TARGET_CHANGE_NEXT,

	HUD_SPRITE_CHAT_INPUT_COUNT,
};

enum SpriteType
{
	ST_HEAD = 0,
	ST_BODY,
	ST_TAIL,
	ST_UP_SPIN_N,
	ST_UP_SPIN_F,
	ST_DOWN_SPIN_N,
	ST_DOWN_SPIN_F,
	ST_THUMB,
	ST_MAX
};

enum CHAT_CHANNEL_TYPE
{
	CHAT_CHANNEL_TYPE_ALL = 0,
	CHAT_CHANNEL_TYPE_TEAM,
	CHAT_CHANNEL_TYPE_CLAN,
	CHAT_CHANNEL_TYPE_WHISPER,

	CHAT_CHANNEL_TYPE_COUNT,
};

class CBattleChatBox: public i3GameObjBase
{
	I3_CLASS_DEFINE(CBattleChatBox, i3GameObjBase);
public:
	CBattleChatBox();
	virtual ~CBattleChatBox();

	virtual BOOL Create(i3Node* pParentNode = NULL,BOOL bEnable = TRUE);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

	void SetEnableChatScroll(BOOL bFlag);
	BOOL GetEnableChatScroll(void);
	void SetChatEnable(BOOL bFlag);
	BOOL GetChatEnable(void);
	
	BOOL getEnableChat(void)	{ return m_bChatEnable;	}
	i3IME*	getIME(void)		{ return m_pIME;	}

	void UpdateWhisperBackupBuffer(void);

	bool	IsGhostMessage( void);

private:
	void _GetTextrueRect(UINT32 idxSprite,I3RECT* pRect);
	void _GetSpriteRect(UINT32 idxSprite,I3RECT* pRect);
	void _ProcessChatScroll(REAL32 rDeltaSeconds);
	void _ProcessChatBox(REAL32 rDeltaSeconds);
	void _ProcessChat(REAL32 rDeltaSeconds);
	void _ProcessChatBattleWhisper(char* nick,char* message, char* message2);
	void _ProcessMacro(REAL32 rDeltaSeconds);
	void _UpdateChatPos(void);
	void _UpdateChatCaret( REAL32 rDeltaSeconds);
	const char* _GetKeyboardValue(GAME_KEY_STRUCT *pGameKey);	
	void _SetChatChannelType(CHAT_CHANNEL_TYPE type);

private:
	i3Texture* m_pTexture;
	i3Sprite2D* m_pSpriteChatOutput;

	i3Sprite2D* m_pSpriteChat;
	i3Sprite2D*	m_pCaretSprite;
	i3Sprite2D* m_pSpriteChatChannel;
	i3TextNodeDX2* m_pTextBox;
	i3TextNodeDX2* m_pTextChat;
	i3TextNodeDX2* m_pTextTarget;	
	i3TextNodeDX2* m_pTextChatChannel[CHAT_CHANNEL_TYPE_COUNT];

	BOOL m_bChatEnableWithGUI;
	BOOL m_bChatScrollEnable;
	BOOL m_bChatEnable;

	i3IME* m_pIME;
	IME_MODE m_CurrentIMEMode;

	INT32 m_nChatType;
	INT32 m_nOldChatType;
	BOOL m_bOnceChatType;
	BOOL m_bOnceChatTypeAll;

	INT32 m_nChatBoxOffsetHeight;

	REAL32 m_fChatScrollTimeDelta;
	REAL32 m_CaretLocalTime;
	REAL32 m_CaretWidth;
	REAL32 m_CaretHeight;

	BOOL m_bCapturedMouse;
	UINT16 m_nCurrentChatBufferType;
};

#endif

#endif //_BattleChatBox_h
