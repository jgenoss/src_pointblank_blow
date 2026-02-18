/**
 * \file	UI\UIFloatChat.h
 *
 * Declares the user interface float chat class.
 */


#if !defined( __UI_FLOAT_CHAT_H__)
#define __UI_FLOAT_CHAT_H__

#include "UIFloatingBase.h"

#include "../UserLocationInfo.h"
#include "../CommunityContext.h"

#include "UITopMenu.h"
#include "UIFlowEditBox.h"

#define MAX_CHATBOXBUFF		MAX_CHAT_BUFFER * (MAX_STRING_COUNT + MAX_STRING_COUNT + 10)

enum CHAT_AREA
{
	CHAT_AREA_LOBBY = 0,
	CHAT_AREA_READYROOM,
	CHAT_AREA_CLAN_MATCH,
	CHAT_AREA_CLAN_MEMBER,

	CHAT_AREA_COUNT,
};

enum CHAT_TYPE
{
	CHAT_TYPE_NONE = -1,
	CHAT_TYPE_ALL,		// 전체		CHATTING_TYPE_ALL | CHATTING_TYPE_LOBBY | CHATTING_TYPE_MATCH | CHATTING_TYPE_CLAN_MEMBER_PAGE
	CHAT_TYPE_TEAM,		// 팀		CHATTING_TYPE_TEAM
	CHAT_TYPE_CLAN,		// 클랜 챗	CHATTING_TYPE_CLAN
	CHAT_TYPE_WHISPER,	// 귓말		CHATTING_TYPE_WHISPER, CHATTING_TYPE_REPLY

	CHAT_TYPE_MAX
};

class UIFloatChat : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatChat, UIFloatingBase);

private:
	
protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIFloatChat();
	virtual ~UIFloatChat();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;

	virtual	bool	OnKey_Enter( void) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd() override;

	virtual bool	OnExitStart(void) override;

	virtual bool	OnEnterMouse( i3UIControl * pControl) override;

	virtual void	OnMovePhase( UIPHASE phase) override;
	virtual void	OnMoveSubPhase( UISUBPHASE phase) override;

	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	//--------------------------------------------------------------------------//
	//								Chatting									//
	//--------------------------------------------------------------------------//

	void			ClickMegaPhoneOpenButton();
private:
	i3UIFrameWnd *	m_pChatMainBox;
	i3UIFrameWnd *	m_pSubChatMain;

	i3UIEditBox *	m_pTextBox;
	i3UIControl *	m_pChatList;
	i3UIEditBox *	m_pChatBox;

	i3UIScrollBar * m_pChatScrollBar;
	
	i3UIEditBox   *  m_pMegaPhoneMain_EditBox;
	i3UIButton	  *  m_pMegaPhone_Button;
	i3UIEditBox   *  m_pMegaPhone_EditBox;
	i3UIFrameWnd  *  m_pParentMegPhoneMain;

	bool			m_bNotice;

	INT32 			m_nChatType;
	INT32 			m_nOldChatType;

	CHAT_AREA		m_ChatArea;

	bool			m_bOnceTeamChat;	// 1회성 팀채팅 변수

	static bool		m_bUseHistory;

	char			m_lastWhisperUserNick[ NET_NICK_NAME_SIZE];

	bool			m_bWhisperMode;	///<< 귓말중

	INT32			m_LanguageCount;
	INT32			m_CurrentLanguage;
	HKL				m_LanguageList[6];

	COutGameQACommandSet * m_pOutGameQACommandSet;

	bool			m_bMegaPhoneOpen;

protected:
	/** \brief Input 언어의 갯수와 리스트를 설정한다. */
	void			_SearchLanguage( void);

	/** \brief Input Layout에서 첫번째 언어로 설정한다. */
	void			_ResetLanguage( void);

	// Chatting
	void 			_UpdateMacroChat(void);
	void			_SendMacroChat(INT32 nIdx);

	void 			_UpdateLang(void);
	void 			_UpdateParsingType( EDIT_STATE state);
	void			_UpdateWhisperBackupBuffer(void);
	
	void			_UpdateChatType( void);
	void			_ChangeChatType(INT32 nChatType, const i3::rc_wstring& wstrMsg, I3COLOR * pColor);

	void			_CopyChatBufferToBox( i3::wstring& inout_wstr, INT32 bufferIdx);
	void			_CopyMegaPhoneBufferToBox( i3::wstring& inout_wstr, INT32 bufferIdx);

	void			_ClearChatBox(void);

	void			_UpdateWhisperChat(const i3::rc_wstring& wstrTarget);
	void			_ClearWhisperChatBox(const i3::rc_wstring& wstrTarget);
	void			_ClearMegaPhoneChatBox(const i3::rc_wstring& wstrTarget);
	bool			_SetWhisperChatToSend(i3::stack_wstring& out_only_chat, i3::stack_wstring& out_colored_chat, const i3::wliteral_range& wImeBufRng);
	void			_SendWhisperChatting(const i3::rc_wstring& wstrTarget, const i3::wliteral_range& wMsgRng, const i3::wliteral_range& wImeBufRng, bool bReply);
	void			_SendMegaPhoneChatting(const i3::wliteral_range& wMsgRng);

	void			_SetChatColorFromType(void);						// 채팅 타입별 채팅 Color 설정
	CHAT_SYS_TYPE	_UpdateChatBox(const i3::wliteral_range& wImeBufRng);			// ChatBox Update (return Type CHAT_SYS_TYPE)

	void			_SendChattingMsg(i3::wstring& inout_wstrImeBuf, CHAT_SYS_TYPE eChatType);
	bool			_CheckGMCommand(const i3::wliteral_range& wImeBufRng);

	bool			_CheckMedalCommand(const i3::wliteral_range& wImeBufRng);

	void			_ChatHistory(EDIT_STATE state);

	void			_ToWhisperChatMessage(i3::wstring& out, const i3::rc_wstring& target, const i3::wliteral_range& msg = L"");
	void			_GetOnlyChatMessage(i3::stack_wstring& out,	const i3::wliteral_range& wfullMsgRng);

	void			_SetMegaPhoneMainFrame(bool bEnable);

public:
	void			OnWhisper( INT32 slotIdx);
	void			OnWhisper( const i3::rc_wstring& wstrNick);

	/** \brief Chatting Buffer의 내용을 출력한다. */
	void 			UpdateChatBuffer(void);
	void			UpdateMegaPhoneBuffer(void);

	void			SetFocus_ChatEditBox();	// 채팅창의 포커스를 켜는 외부에서 켜는 함수..

private:
	typedef void	(UIFloatChat::*call_func)(INT32,const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Change_ReadyroomInfo( INT32 arg, const i3::user_data&);
};

#endif
