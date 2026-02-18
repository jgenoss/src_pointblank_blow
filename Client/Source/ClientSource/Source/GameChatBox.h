#ifndef __GAMECHATBOX_H__
#define __GAMECHATBOX_H__

#include "i3Base/itl/svector.h"

#define MAX_CHAT_BUFFER	100
#define MAX_MEGAPHONE_BUFFER 300

#define MAX_CHAT_HISTORY_COUNT	10

enum CHAT_SYS_TYPE
{
	CHAT_SYS_TYPE_NONE = 0,
	CHAT_SYS_TYPE_WHISPER,
	CHAT_SYS_TYPE_REPLY,
	CHAT_SYS_TYPE_ONETIME_TEAM,
	CHAT_SYS_TYPE_CLEAR,
	CHAT_SYS_TYPE_HELP,
	CHAT_SYS_TYPE_MEGAPHONE,

	CHAT_SYS_TYPE_COUNT,
};

//
// GAME_STRING의 결과값을 모두 rc_string으로 바꿀지도 모른다.. (이 경우 수정 규모가 좀 많이 크다..)
//

class CGameChatBox : public i3GameObjBase
{
	I3_CLASS_DEFINE( CGameChatBox, i3GameObjBase);

protected:
	// Buffer
	INT32			m_nCurrentBufferIndex;
	INT32			m_nBackUpBufferIndex;
	INT32			m_nDeleteBufferIndex;
	INT32			m_nCurrentWhisperBufferIndex;
	INT32			m_nWhisperBufferIndex;
	INT32			m_nCurrentMegaPhoneBufferIndex;
	INT32			m_nMegaPhoneBufferIndex;
	INT32			m_nBufferCount;
	INT32			m_nBackUpBufferCount;
	INT32			m_nWhisperBufferCount;
	INT32			m_nMegaPhoneBufferCount;
	REAL32			m_fBackUpBufferRatio;
																// 아무리 속도문제가있더라도 배열은 일단 보류..
	i3::svector<UINT16>				m_ChatBufferTypeList;		// MAX_CHAT_BUFFER까지 미리확보하고 배열처럼 사용.
	i3::svector<i3::rc_wstring>		m_ChatBufferList;			// MAX_STRING_COUNT + MAX_STRING_COUNT + 10 제약 제거..(특별한 제약 이유는 없어보임)
	i3::svector<i3::rc_wstring>		m_ChatBackupBufferList;		// MAX_STRING_COUNT + MAX_STRING_COUNT + 10 제약 제거..(특별한 제약 이유는 없어보임)
	i3::svector<i3::rc_wstring>		m_WhisperBufferList;		// 기존에 5로 잡혀있습니다. (생성자에서 처리)
	i3::svector<i3::rc_wstring>		m_MegaPhoneBufferList;		

	i3::rc_wstring						m_wstrChatTarget;			//
	i3::rc_wstring						m_wstrChatReplyTarget;		//

	i3::rc_wstring						m_wstrHistoryText;			// MAX_STRING_COUNT + MAX_STRING_COUNT + 10 제약 제거..(특별한 제약 이유는 없어보임)
	
	i3::rc_wstring					m_emptyString;

	bool			m_bWhisperEnable;
	bool			m_bRingBufferFull;

	// Chat History
	i3::svector<i3::rc_wstring>	m_HistoryBufferList;					// MAX_CHAT_HISTORY_COUNT크기를 가진다. (생성자에서 처리)
	i3::svector<UINT16>			m_HistoryChatModeList;					// 일관성문제로 UINT16로 변경한다. (m_ChatBufferTypeList와 같은 값이다..)

	INT32			m_nCurrentHistoryBufferIndex;
	INT32			m_nHistoryBufferIndex;
	INT32			m_nHistoryBufferCount;
	INT32			m_nHistoryBufferOffset;

public:
	CGameChatBox(void);
	~CGameChatBox(void);

	// Buffer를 초기화 합니다.
	void			ResetChatBuffer();
	void			ResetWhisperBuffer(void);
	void			ResetMegaPhoneBuffer(void);

	// 여태까지 입력된 Chatting Buffer의 내용을 index로 가져옵니다.
	const i3::rc_wstring&	GetBufferText( INT32 index) const;				
	const i3::rc_wstring&	GetBackUpBufferText( INT32 index) const;		
	const i3::rc_wstring&	GetWhisperBufferText( INT32 index) const;
	const i3::rc_wstring&	GetMegaPhoneBufferText( INT32 index) const;
	void			GetHistoryBufferText( INT32 index, i3::rc_wstring& out_str, INT32 * pMode);	
	void			ResetHistoryBufferIndex(void);
	UINT16			getBufferType( INT32 index)				{ return m_ChatBufferTypeList[index];	}
	
	CHAT_SYS_TYPE	ParsingText(const i3::wliteral_range& wTextRng);

	bool			FindTarget( const i3::wliteral_range& wTextRng);
	void			SetHelpText(void);
	
	const i3::rc_wstring&	GetChatTarget(void) const		{ return m_wstrChatTarget;			}	// 고치는게 불가능해보인다..(자꾸 가변인수나 void* 인수함수로 넘어간다.)
	void			SetChatTarget(const i3::rc_wstring& chat_target)	{	m_wstrChatTarget = chat_target; }			// 고치는게 불가능해보인다..(자꾸 가변인수나 void* 인수함수로 넘어간다.)

	const i3::rc_wstring&	GetChatReplyTarget(void) const	{ return m_wstrChatReplyTarget;	}		// 고치는게 불가능해보인다..(자꾸 가변인수나 void* 인수함수로 넘어간다.)
	const i3::rc_wstring&	GetHistoryText(void) const				{ return m_wstrHistoryText;		}		// 고치는게 불가능해보인다..(자꾸 가변인수나 void* 인수함수로 넘어간다.)
	INT32			GetBufferCount()						{ return m_nBufferCount;				}
	INT32			GetWhisperBufferCount()					{ return m_nWhisperBufferCount;			}
	INT32			GetMegaPhoneBufferCount()				{ return m_nMegaPhoneBufferCount;       }
	INT32			GetBackUpBufferCount()					{ return m_nBackUpBufferCount;			}
	INT32			GetCurrentBufferIndex()					{ return m_nCurrentBufferIndex;			}
	INT32			GetCurrentWhisperBufferIndex()			{ return m_nCurrentWhisperBufferIndex;	}
	INT32			GetCurrentMegaPhoneBufferIndex()		{ return m_nCurrentMegaPhoneBufferIndex;}
	INT32			GetMegaPhoneBufferIndex()				{ return m_nMegaPhoneBufferIndex;		}
	INT32			GetBackUpBufferIndex()					{ return m_nBackUpBufferIndex;			}
	REAL32			GetBackUpBufferRatio()					{ return m_fBackUpBufferRatio;			}
	void			SetChatReplyTarget(const i3::rc_wstring& wstrTarget);
	void			CalcRatio( INT32 nExceptionValue);
	void			DeleteBufferIndex(void);
	void			ResetBackupBuffer(void);
	void			IncreaseBackupBuffer( INT32 nExceptionValue = 0);
	void			DecreaseBackupBuffer( INT32 nExceptionValue = 0);
	void			CalcBackupBufferByRatio( REAL32 fVal, INT32 nExceptionValue = 0);

	//
	INT32			GetHistoryBufferCount()					{ return m_nHistoryBufferCount;			}
	INT32			GetHistoryBufferIndex()					{ return m_nCurrentHistoryBufferIndex;	}
	INT32			GetHistoryBufferOffset()				{ return m_nHistoryBufferOffset;		}
	void			SetHistoryBufferIndex(INT32 nIndex)		{ m_nCurrentHistoryBufferIndex = nIndex;}

	void			PutUserChatting(const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng, const bool bIsGM, UINT16 nType = 0, UINT8 NickColor = 0);
	void			PutUserWhisper(const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng, const bool bIsGM);	
	void			PutUserMegaPhone(const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng );
	void			PutSystemChatting(const i3::rc_wstring& str, UINT16 nType = 0);
	void			PutHistory( const i3::rc_wstring& str, INT32 ChatMode);			
	void			OnUpdate( REAL32 rDeltaSeconds );

	void			setWhisperEnable( bool bEnable)			{ m_bWhisperEnable = bEnable;			}
	bool			getWhisperEnable(void)					{ return m_bWhisperEnable;				}

	virtual	void	SetEnable( bool bTrue = true, REAL32 tm = 0.0f ) override;

private:
	void			_PutStringBuffer(const i3::rc_wstring& str, UINT16 nType = 0);
	void			_PutStringWhisperBuffer(const i3::rc_wstring& str);
	void			_PutStringMegaPhoneBuffer(const i3::rc_wstring& str);
};





#endif // __GAMECHATBOX_H__