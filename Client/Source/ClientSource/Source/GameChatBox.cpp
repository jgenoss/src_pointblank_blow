#include "pch.h"
#include "GameChatBox.h"
#include "GlobalVariables.h"
#include "CommunityContext.h"
#include "GameGUI.h"

#include "UI/UIMainFrame.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIHUDIngameChat.h"

#include "i3Base/string/ext/generic_string_ncopy.h"
#include "i3Base/string/stack_string.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/remove_blank_from_tail.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "i3Base/RemoveColorString.h"

#include "UserInfoContext.h"

//I3_CLASS_INSTANCE( CGameChatBox, i3GameObjBase);
I3_CLASS_INSTANCE( CGameChatBox);

CGameChatBox::CGameChatBox(void) :	m_ChatBufferTypeList(MAX_CHAT_BUFFER), 
									m_ChatBufferList(MAX_CHAT_BUFFER), 
									m_ChatBackupBufferList(MAX_CHAT_BUFFER), 
									m_WhisperBufferList(5),
									m_MegaPhoneBufferList(MAX_MEGAPHONE_BUFFER),
									m_HistoryBufferList(MAX_CHAT_HISTORY_COUNT), 
									m_HistoryChatModeList(MAX_CHAT_HISTORY_COUNT)
{

	m_pSceneNode = i3Node::new_object();	

	m_nCurrentBufferIndex = 0;
	m_nBackUpBufferIndex = 0;
	m_nDeleteBufferIndex = 0;
	m_nCurrentWhisperBufferIndex = 0;
	m_nCurrentMegaPhoneBufferIndex = 0;
	m_nWhisperBufferIndex = 0;
	m_nMegaPhoneBufferIndex = 0;
	m_nBufferCount = 0;
	m_nBackUpBufferCount = 0;
	m_nWhisperBufferCount = 0;
	m_nMegaPhoneBufferCount = 0;
	m_fBackUpBufferRatio = 0.0f;

	m_bWhisperEnable = false;
	m_bRingBufferFull = false;

	m_nCurrentHistoryBufferIndex = 0;
	m_nHistoryBufferIndex = 0;
	m_nHistoryBufferCount = 0;
	m_nHistoryBufferOffset = 0;

}

CGameChatBox::~CGameChatBox(void)
{

}

void CGameChatBox::ResetChatBuffer()
{
	INT32 i;

	m_nCurrentBufferIndex = 0;
	m_nBackUpBufferIndex = 0;
	m_nDeleteBufferIndex = 0;
	m_nBufferCount = 0;
	m_nBackUpBufferCount = 0;
	m_fBackUpBufferRatio = 0.0f;
	m_bRingBufferFull = false;

	for(i=0; i<MAX_CHAT_BUFFER; i++)
	{
		m_ChatBufferList[i].clear();
		m_ChatBackupBufferList[i].clear();
	}

	// UI에 이벤트를 보낸다.
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	if( pUIFrame != nullptr)
	{
		pUIFrame->UpdateChatBuffer();
	}

	if(g_pFramework->GetStageBattle())
	{
		UIBattleFrame::i()->UpdateChatBuffer();
	}

}

void CGameChatBox::ResetWhisperBuffer(void)
{
	INT32 i = 0;

	m_nCurrentWhisperBufferIndex = 0;
	m_nWhisperBufferIndex = 0;
	m_nWhisperBufferCount = 0;

	for(i=0; i<5; i++)
	{
		m_WhisperBufferList[i].clear();
	}
}

void CGameChatBox::ResetMegaPhoneBuffer(void)
{
	INT32 i = 0;

	m_nCurrentMegaPhoneBufferIndex = 0;
	m_nMegaPhoneBufferIndex = 0;
	m_nMegaPhoneBufferCount = 0;

	for(i=0; i<MAX_MEGAPHONE_BUFFER; i++)
	{
		m_MegaPhoneBufferList[i].clear();
	}
}	


const i3::rc_wstring& CGameChatBox::GetBufferText( INT32 index) const
{
	// 현재 반환될 index가 Buffer의 크기를 넘어선다면, nullptr을 반환합니다.
	if( index < 0 || index >= MAX_CHAT_BUFFER)
	{
		return m_emptyString;
	}

	return m_ChatBufferList[index];				// 또 고쳐야 함. (13.02.15.수빈)
}

const i3::rc_wstring& CGameChatBox::GetBackUpBufferText( INT32 index) const
{
	// 현재 반환될 index가 Buffer의 크기를 넘어선다면, nullptr을 반환합니다.
	if( index < 0 || index >= MAX_CHAT_BUFFER)
	{
		return m_emptyString;
	}

	return m_ChatBackupBufferList[index];		
}

const i3::rc_wstring& CGameChatBox::GetWhisperBufferText( INT32 index) const
{
	return m_WhisperBufferList[index];
}

const i3::rc_wstring& CGameChatBox::GetMegaPhoneBufferText( INT32 index) const
{
	return m_MegaPhoneBufferList[index];
}

void CGameChatBox::GetHistoryBufferText( INT32 index, i3::rc_wstring& out_str, INT32 * pMode)
{
	if( index == 1)		// 위쪽 키를 눌렀을 경우
	{
		m_nHistoryBufferOffset++;

		if( m_nHistoryBufferOffset <= m_nHistoryBufferCount)
		{	
			if( m_nHistoryBufferOffset != 1)
			{
				m_nCurrentHistoryBufferIndex--;
				if( m_nCurrentHistoryBufferIndex < 0) m_nCurrentHistoryBufferIndex = m_nHistoryBufferCount - 1;
			}

			out_str = m_HistoryBufferList[m_nCurrentHistoryBufferIndex];

			*pMode = m_HistoryChatModeList[ m_nCurrentHistoryBufferIndex ];		//	m_szHistoryChatMode[m_nCurrentHistoryBufferIndex];
		}
		else
		{
			m_nHistoryBufferOffset = m_nHistoryBufferCount;					
			
			out_str = m_HistoryBufferList[m_nCurrentHistoryBufferIndex];

			*pMode = m_HistoryChatModeList[m_nCurrentHistoryBufferIndex];
		}
	}
	else				// 아래쪽 키를 눌렀을 경우
	{
		m_nHistoryBufferOffset--;

		if( m_nHistoryBufferOffset > 0)
		{			
			m_nCurrentHistoryBufferIndex++;

			if( m_nCurrentHistoryBufferIndex >= m_nHistoryBufferCount) m_nCurrentHistoryBufferIndex = 0;

			out_str =  m_HistoryBufferList[m_nCurrentHistoryBufferIndex];

			*pMode = m_HistoryChatModeList[m_nCurrentHistoryBufferIndex];
		}
		else
		{
			m_nHistoryBufferOffset = 0;

			out_str.clear();
		}
	}
}

void CGameChatBox::ResetHistoryBufferIndex(void)
{
	if( m_nHistoryBufferIndex - 1 > 0)
	{
		m_nCurrentHistoryBufferIndex = m_nHistoryBufferIndex - 1;
	}
	else
	{
		if( m_nHistoryBufferIndex < m_nHistoryBufferCount)
		{
			m_nCurrentHistoryBufferIndex = 0;
		}
		else
		{
			m_nCurrentHistoryBufferIndex = m_nHistoryBufferCount - 1;
		}
	}

	m_nHistoryBufferOffset = 0;
}

//
// 이 루틴은 정말 많이 고쳐야 한다...왜냐하면 원하는 것은 검색결과로 첫인덱스가 0인것을 찾는것인데
// 0에 없다면 한참을 찾아 해매기 때문이다... 0에 없다면 즉시 리턴할수 있도록 해야한다..(13.02.19.수빈)
// 함수가 잘못 쓰여진 것으로 보인다.... 그냥 단순 비교함수로 바꾸는게 낫다..
// 
// ParsingText가 많이 호출된다면 ( 잘 모름) .... GAME_STRING쓰지말고 미리 인덱스 보관하는게 좋다...
//

CHAT_SYS_TYPE CGameChatBox::ParsingText( const i3::wliteral_range& wTextRng) 
{
	CHAT_SYS_TYPE ret = CHAT_SYS_TYPE_NONE;

	if( i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_TEAM3"))  ||
		i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_TEAM4"))  ||
		i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_TEAM2"))  ||
		i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_TEAM1")) ) 
	{
		// 일회성 팀 챗팅!!	
		ret = CHAT_SYS_TYPE_ONETIME_TEAM;/*/T *//*/t *//*/ㅅ *//*/팀 */
	}
	else if( i3::generic_is_equal( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_ERASE3"))  ||
			 i3::generic_is_equal( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_ERASE4"))  ||
			 i3::generic_is_equal( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_ERASE2"))  ||
			 i3::generic_is_equal( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_ERASE1")) ) 
	{
		// 채팅창 클리어!!	
		ret = CHAT_SYS_TYPE_CLEAR;/*/C*//*/c*//*/ㅊ*//*/지우기*/
	}
	else if( i3::generic_is_equal( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_HELP"))  ||
			 i3::generic_is_equal( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_HELP2")) )
	{
		// 채팅 도움말 표시!!	
		ret = CHAT_SYS_TYPE_HELP;/*/? *//*/?*/
	}
	else if( i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_WHISPER3")) ||
		i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_WHISPER4"))||
		i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_WHISPER2")) ||
		i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_WHISPER1")) ) 
	{
		// 귓속말!!			
		if( FindTarget( wTextRng))
		{
			if(	!m_wstrChatTarget.empty() )	
			{
				ret = CHAT_SYS_TYPE_WHISPER;/*/W *//*/w *//*/ㅈ *//*/귓 */
			}
		}
	}
	else if( i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_REPLY3"))  ||
			 i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_REPLY4"))  ||
			 i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_REPLY2"))  ||
			 i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_REPLY1")) ) 
	{
		FindTarget( wTextRng );/*/R *//*/r *//*/ㄱ *//*/답 */

		// 답변!!	
		ret = CHAT_SYS_TYPE_REPLY;
	}
	else if( i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_MEGAPHONE1"))  ||
			 i3::contain_string_head( wTextRng, GAME_RCSTRING("STR_TBL_CHAT_HOTKEY_MEGAPHONE2")) )
	{
		ret = CHAT_SYS_TYPE_MEGAPHONE;
	}
	
	return ret;
}


//
// 기존 FindTarget함수의 while문자열 처리부는 첫 공백에서부터 타겟닉네임을 얻기 시작하고, 두번째 공백에서 닉네임 확정뒤 나머지 문자열을
// 귓말 텍스트로 넣는 것임..(13.02.15.수빈)
//
// 이부분을 일반화하면, 먼저 첫공백을 찾고, 그 위치에서 다시 두번째 공백을 찾으면 된다.. ( 따라서 이부분을 신규루틴으로 대체한다..)
// 

bool CGameChatBox::FindTarget( const i3::wliteral_range& wTextRng)
{
	m_wstrHistoryText.clear();

	i3::wliteral_range::const_iterator it_target = i3::find(wTextRng.begin(), wTextRng.end(), L' '); 
	
	if ( it_target == wTextRng.end() )
	{
		m_wstrChatTarget.clear();
		return false;
	}
	++it_target;
	i3::wliteral_range::const_iterator it_text = i3::find( it_target, wTextRng.end(), ' ');
	
	m_wstrChatTarget.assign(it_target, it_text);	

	if ( it_text == wTextRng.end() )
		return false;
	
	++it_text;
	m_wstrHistoryText.assign(it_text, wTextRng.end() );

	return true;
}

void CGameChatBox::SetChatReplyTarget(const i3::rc_wstring& wstrTarget)
{
	m_wstrChatReplyTarget = wstrTarget; 
}

void CGameChatBox::SetHelpText(void)
{
	PutSystemChatting(GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HELP1"));	//"{col:80,255,60,255}F2: 전체 채팅 모드로 변경{/col}"
	PutSystemChatting(GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HELP2"));	//"{col:80,255,60,255}F3: 팀 채팅 모드로 변경{/col}"
	PutSystemChatting(GAME_RCSTRING("STR_TBL_CHAT_HELP_CHANGE_CLANMODE"));/*{col:255,120,0,255}F4: 클랜 채팅 모드로 변경{/col}*/
	PutSystemChatting(GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HELP3"));	//"{col:80,255,60,255}/w, /귓: 귓속말{/col}"
	PutSystemChatting(GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HELP4"));	//"{col:80,255,60,255}/r, /답: 답변{/col}"
	PutSystemChatting(GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HELP5"));	//"{col:80,255,60,255}/t, /팀: 1회성 팀 채팅 입력{/col}"
	PutSystemChatting(GAME_RCSTRING("STBL_IDX_CHATMESSAGE_HELP6"));	//"{col:80,255,60,255}/c, /지우기: 채팅 내용 지우기{/col}"
}

void CGameChatBox::PutUserChatting( const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng,  const bool bIsGM, UINT16 nType, UINT8 NickColor)
{
//	char szTemp[MAX_STRING_COUNT + MAX_STRING_COUNT + 10] = {0,};

	i3::stack_wstring wstrTemp;

	i3::stack_wstring wstrNickColor;
	i3::stack_wstring wstrStringColor;
	//bool isGM = false;
	
	// revision 61128
	i3::wstring wstrClearString = wStringRng;
//	char szResult[256] = {0,};
	bool bcolor = ::RemoveColorString(wStringRng, wstrClearString);		// ::RemoveColorSting 의 매개변수로는 i3::string 이 들어갈 수 있음
//	i3::safe_string_copy(szResult,szClearString,sizeof(szResult));		// 그러나 i3String::Concat 의 매개변수는 char * 이므로 부득이하게 이러한 루틴으로 작성하게 되었음
																		// i3String 의 char * 와 i3::string 호환 작업이 이루어지면 RemoveColorString 한줄로 작성 가능함
	if( bcolor == false)
		wstrClearString = wStringRng;

	//if (i3::generic_is_niequal(wstrNick , GAME_RCSTRING("STR_TBL_CHAT_GM_TEXT") , 2) )
	//{
	//	isGM = true; 
	//}
	if( CCommunityContext::i()->CheckBlockUser(wstrNick) )
	{
		return;
	}

	//채팅 범위를 정한다.
	if( bIsGM == false)
	{
		// 커뮤니티 옵션의 설정에 따라 채팅내용을 업데이트할지 결정합니다.
		switch( g_pEnvSet->m_nChatType)
		{
		// 전체차단
		case 1:
			// 모든 채팅메세지를 무시하고 리턴합니다.
			return;
			break;

		// 아군허용
		case 2:
			if( nType != CHATTING_TYPE_TEAM && nType != CHATTING_TYPE_CLAN)
			{
				return;
			}
			break;
		// 전체허용
		default:
			break;
		}
	}

	//채팅 색상을 정한다.
	if( bIsGM )
	{
		wstrNickColor = L"247,200,60,255";/*GM*/

		switch( nType)
		{
		case CHATTING_TYPE_ALL:		wstrStringColor = L"255,255,255,255";		break;
		case CHATTING_TYPE_TEAM:	wstrStringColor = L"98, 180, 253, 255";	break;
		case CHATTING_TYPE_CLAN:	wstrStringColor = L"80, 255, 60, 255";	break;
		case CHATTING_TYPE_LOBBY:	wstrStringColor = L"255,255,255,255";		break;
		default:					wstrStringColor = L"255,255,255,255";		break;
		}
	}
	else if( nType == CHATTING_TYPE_CLAN )
	{
		wstrNickColor = L"80, 255, 60, 255";
		wstrStringColor = L"80, 255, 60, 255";
	}
	else if( nType == CHATTING_TYPE_TEAM )
	{
		wstrNickColor = L"98, 180, 253, 255";
		wstrStringColor = L"98, 180, 253, 255";
	}
	else if( nType == CHATTING_TYPE_ALL 
		|| nType == CHATTING_TYPE_LOBBY 
		|| nType == CHATTING_TYPE_CLAN_MEMBER_PAGE  )
	{
		const I3COLOR* clr_nick = GetColorCashItem(NickColor);
		const I3COLOR* clr_string = GetColorCashItem(0);
		
		i3::sprintf(wstrNickColor, L"%d,%d,%d,%d", clr_nick->r, clr_nick->g, clr_nick->b, clr_nick->a);
		i3::sprintf(wstrStringColor, L"%d,%d,%d,%d", clr_string->r, clr_string->g, clr_string->b, clr_string->a);
	}
	else if( nType == CHATTING_TYPE_RADIO )	//라디오 메세지
	{
		const I3COLOR* clr_nick = GetColorCashItem(NickColor);
		const I3COLOR* clr_string = GetColorCashItem(0);
		i3::sprintf(wstrNickColor, L"%d,%d,%d,%d", clr_nick->r, clr_nick->g, clr_nick->b, clr_nick->a);
		i3::sprintf(wstrStringColor, L"%d,%d,%d,%d", clr_string->r, clr_string->g, clr_string->b, clr_string->a);

		wstrClearString = wStringRng;
	}
	else
	{
		const I3COLOR* clr_nick_string = GetColorCashItem(0);

		i3::sprintf(wstrNickColor, L"%d,%d,%d,%d", clr_nick_string->r, clr_nick_string->g, clr_nick_string->b, clr_nick_string->a);
		wstrStringColor = wstrNickColor;
	}

	// Nick Name이 있다면 값을 입력합니다.
	if( !wstrNick.empty() )
	{
		if( i3::generic_is_equal( wstrNickColor,wstrStringColor) )
		{
			wstrTemp = L"{col:";
			i3::generic_string_cat( wstrTemp, wstrNickColor);
			i3::generic_string_cat( wstrTemp, L"}[");
			i3::generic_string_cat( wstrTemp, wstrNick);
			i3::generic_string_cat( wstrTemp, L"] ");
			i3::generic_string_cat( wstrTemp, wstrClearString);
			i3::generic_string_cat( wstrTemp, L"{/col}");
		}
		else
		{
			wstrTemp = L"{col:";

			i3::generic_string_cat( wstrTemp, wstrNickColor);
			i3::generic_string_cat( wstrTemp, L"}[");
			i3::generic_string_cat( wstrTemp, wstrNick); 
			i3::generic_string_cat( wstrTemp, L"] {/col}");

			i3::generic_string_cat( wstrTemp, L"{col:");
			i3::generic_string_cat( wstrTemp, wstrStringColor);
			i3::generic_string_cat( wstrTemp, L"}");
			i3::generic_string_cat( wstrTemp, wstrClearString);
			i3::generic_string_cat( wstrTemp, L"{/col}");
		}
	}
	else
	{
		wstrTemp = wstrClearString;
	}

	// 내가 입력한 채팅이라면, 채팅타입에 따라 출력하는게 아니라, 모두 출력해야함. 그렇기때문에 무조건 출력되는 nType = 0으로 변경.
	_PutStringBuffer(wstrTemp, nType);
}

void CGameChatBox::PutUserWhisper( const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng, const bool bIsGM)
{
	i3::wstring wstrClearString;
	//i3::string szResult;
	::RemoveColorString(wStringRng, wstrClearString);					// 채팅 패킷 받을 시 컬러 키워드 삭제 루틴 변경 ( 2013_0628 유영정 )
	//i3::safe_string_copy(szResult,szClearString,sizeof(szResult));

	i3::stack_wstring strTemp;

	const wchar_t* litNickColor = L"237, 86, 250, 255";
	const wchar_t* litStringColor = litNickColor;	// "237, 86, 250, 255";

	if( UserInfoContext::i()->CheckAuthLevel())
	{
		litNickColor = L"255,128,64,255";  /*GM*/
		litStringColor = L"237, 86, 250, 255";
	}

	// 커뮤니티 옵션의 설정에 따라 귓속말을 업데이트할지 결정합니다.
	switch( g_pEnvSet->m_nWhisperType)
	{
	// 전체차단
	case 1:
		// 모든 귓속말을 무시하고 리턴합니다.
		return;
		break;

	// 친구&클랜허용
	case 2:
		// 작업해야합니다~!
		break;
	// 전체허용
	default:
		break;
	}

	// Nick Name이 있다면 값을 입력합니다.

	if( litNickColor == litStringColor )			// 리터럴로 그냥 포인터 비교로 끝낸다...(13.02.15.수빈)
	{
		i3::sprintf(strTemp, L"{col:%s}[%s] %s{/col}", litNickColor, wstrNick, wstrClearString);
	}
	else
	{
		i3::sprintf(strTemp, L"{col:%s}[%s]{/col} {col:%s}%s{/col}", litNickColor, wstrNick, litStringColor, wstrClearString);
	}

	if( m_bWhisperEnable)	_PutStringBuffer(strTemp, (UINT16)CHATTING_TYPE_WHISPER);
	else					_PutStringWhisperBuffer(strTemp);

}

void CGameChatBox::PutUserMegaPhone( const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng)
{
	i3::wstring wstrClearString;
	::RemoveColorString(wStringRng, wstrClearString);

	i3::stack_wstring strTemp;

	const wchar_t* litNickColor = L"253, 139, 248, 255";
	
	i3::sprintf(strTemp, L"{col:%s}[%s] %s{/col}", litNickColor, wstrNick, wstrClearString);
	
	_PutStringMegaPhoneBuffer(strTemp);
}

void CGameChatBox::PutSystemChatting(const i3::rc_wstring& str, UINT16 nType)
{
	// ! praptor - 색을 바꾸거나 특수한 꾸밈을 하기위해서는 제어문자를 삽입하고,
	// 출력하는 부분 - 이를테면 ReadyRoom에서 제어문자를 찾아내어 출력하여야한다.
	// 현재 일반 체팅과는 닉네임이 제외되었다는 점 이외엔 차이점이 없다.

	_PutStringBuffer(str, nType);
}

void CGameChatBox::PutHistory(  const i3::rc_wstring& str, INT32 ChatMode)
{
		
	if( str.empty() ) return;

	I3TRACE( "History Buffer TextInput: Idx = %d\n", m_nHistoryBufferIndex);

	// 실제 사용되는 Buffer에 값을 넣습니다.
	m_HistoryBufferList[m_nHistoryBufferIndex] = str;
	m_HistoryChatModeList[ m_nHistoryBufferIndex ] = UINT16(ChatMode);


	// Buffer Count를 증가시킵니다.
	if( m_nHistoryBufferCount < MAX_CHAT_HISTORY_COUNT)
	{
		m_nHistoryBufferCount++;
	}

	// Buffer Index를 증가시킵니다.
	m_nHistoryBufferIndex++;
	if( m_nHistoryBufferIndex >= MAX_CHAT_HISTORY_COUNT)
	{
		m_nHistoryBufferIndex = 0;				
	}

	m_nCurrentHistoryBufferIndex = m_nHistoryBufferIndex;
}

void CGameChatBox::_PutStringBuffer(const i3::rc_wstring& str, UINT16 nType)
{
	if( str.empty() )	return;

	//I3TRACE( "TextInput: Idx = %d\n", m_nCurrentBufferIndex);

	// Buffer의 Type
	m_ChatBufferTypeList[m_nCurrentBufferIndex] = nType;

	// 실제 사용되는 Buffer에 값을 넣습니다.
	m_ChatBufferList[ m_nCurrentBufferIndex ] = str;
	// Backup용 Buffer에 값을 넣습니다.
	m_ChatBackupBufferList[ m_nCurrentBufferIndex ] = str;
	
	// Buffer Count를 증가시킵니다.
	m_nBufferCount++;
	m_nBackUpBufferCount++;

	// Buffer의 현재 Index Count를 증가시킵니다.
	if( m_nCurrentBufferIndex == m_nBackUpBufferIndex)
	{
		// 버퍼위치가 가장 밑(m_nCurrentBufferIndex)이라면, 자동 스크롤 되어야 함.
		m_nCurrentBufferIndex++;
		m_nBackUpBufferIndex++;
	}
	else
	{
		// 버퍼위치가 가장 밑(m_nCurrentBufferIndex)이 아니라면, 자동 스크롤 되지 않음.
		m_nCurrentBufferIndex++;

		// 버퍼가 가득차서 순환되고 있을경우, 화면에 보이는 백버퍼 인덱스도 변경되어야 합니다.
		if( m_bRingBufferFull) m_nBackUpBufferIndex++;
	}

	// 만약 Buffer의 Index가 Buffer의 크기를 넘어선다면, Index를 다시 초기치로 설정합니다.(순환되도록!!)
	if(m_nCurrentBufferIndex >= MAX_CHAT_BUFFER)
	{
		m_bRingBufferFull = true;
		m_nCurrentBufferIndex = 0;		
	}

	if(m_nBackUpBufferIndex >= MAX_CHAT_BUFFER)
	{
		m_nBackUpBufferIndex = 0;		
	}

	if(m_nBackUpBufferCount >= MAX_CHAT_BUFFER)
	{
		m_nBackUpBufferCount = MAX_CHAT_BUFFER;
	}

	// UI에 이벤트를 보낸다.
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	if( pUIFrame )
	{
		pUIFrame->UpdateChatBuffer();
	}

	CStageBattle*pStageBattle = g_pFramework->GetStageBattle();
	if(pStageBattle)
	{
		if( UIBattleFrame::i() != nullptr)
			UIBattleFrame::i()->UpdateChatBuffer();
	}
}

void CGameChatBox::_PutStringWhisperBuffer(const i3::rc_wstring& str)
{

	if( str.empty() ) return;

	I3TRACE( "BackUp TextInput: Idx = %d\n", m_nWhisperBufferIndex);

	// 실제 사용되는 Buffer에 값을 넣습니다.
	m_WhisperBufferList[m_nWhisperBufferIndex] = str;

	// Buffer Count를 증가시킵니다.
	if( m_nWhisperBufferCount < 5)
	{
		m_nWhisperBufferCount++;
	}
	else
	{
		m_nCurrentWhisperBufferIndex++;
		if( m_nCurrentWhisperBufferIndex >= 5)
		{
			m_nCurrentWhisperBufferIndex = 0;
		}
	}

	// Buffer Index를 증가시킵니다.
	m_nWhisperBufferIndex++;
	if( m_nWhisperBufferIndex >= 5)
	{
		m_nWhisperBufferIndex = 0;		
	}
}

void CGameChatBox::_PutStringMegaPhoneBuffer(const i3::rc_wstring& str)
{

	if( str.empty() ) return;

	I3TRACE( "BackUp TextInput: Idx = %d\n", m_nWhisperBufferIndex);

	// 실제 사용되는 Buffer에 값을 넣습니다.
	m_MegaPhoneBufferList[m_nMegaPhoneBufferIndex] = str;

	// Buffer Count를 증가시킵니다.
	if( m_nMegaPhoneBufferCount < MAX_MEGAPHONE_BUFFER)
	{
		m_nMegaPhoneBufferCount++;
	}
	else
	{
		m_nCurrentMegaPhoneBufferIndex++;
		if( m_nCurrentMegaPhoneBufferIndex >= MAX_MEGAPHONE_BUFFER)
		{
			m_nCurrentMegaPhoneBufferIndex = 0;
		}
	}

	// Buffer Index를 증가시킵니다.
	m_nMegaPhoneBufferIndex++;
	if( m_nMegaPhoneBufferIndex >= MAX_MEGAPHONE_BUFFER)
	{
		m_nMegaPhoneBufferIndex = 0;		
	}

	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	if( pUIFrame )
	{
		pUIFrame->UpdateMegaPhoneBuffer();
	}
}


void CGameChatBox::CalcRatio( INT32 nExceptionValue)
{	
	INT32 nCount = 0;
	INT32 nStartIdx = m_nCurrentBufferIndex;
	INT32 nEndIdx = m_nBackUpBufferIndex;

	while( nStartIdx != nEndIdx)
	{
		nCount++;

		nStartIdx--;
		if( nStartIdx < 0) nStartIdx = MAX_CHAT_BUFFER-1;
	}

	if( nCount > 0)
	{
		if( (m_nBackUpBufferCount - nExceptionValue) > 0)
		{
			m_fBackUpBufferRatio = ((REAL32)nCount) / (REAL32)(m_nBackUpBufferCount - nExceptionValue);
		}
		else
		{
			m_fBackUpBufferRatio = 0.0f;
		}
	}
	else
	{
		m_fBackUpBufferRatio = 0.0f;
	}
}

void CGameChatBox::DeleteBufferIndex()
{
	m_ChatBufferList[ m_nDeleteBufferIndex ].clear();
	
	//I3TRACE( "TextInput Delete: Idx = %d\n", m_nDeleteBufferIndex);

	m_nDeleteBufferIndex++;

	if(m_nDeleteBufferIndex >= MAX_CHAT_BUFFER)
	{
		m_nDeleteBufferIndex = 0;
	}

	m_nBufferCount--;
	if( m_nBufferCount < 0) m_nBufferCount = MAX_CHAT_BUFFER-1;
}

void CGameChatBox::ResetBackupBuffer(void)
{
	m_nBackUpBufferIndex = m_nCurrentBufferIndex;
	m_fBackUpBufferRatio = 0.0f;
}

void CGameChatBox::IncreaseBackupBuffer( INT32 nExceptionValue)
{
	if( m_nBackUpBufferIndex != m_nCurrentBufferIndex)
	{
		m_nBackUpBufferIndex++;
		if( m_nBackUpBufferIndex >= MAX_CHAT_BUFFER)
		{
			m_nBackUpBufferIndex = 0;
		}
	}

	CalcRatio(nExceptionValue);
}

void CGameChatBox::DecreaseBackupBuffer( INT32 nExceptionValue)
{
	INT32 i = 0;
	INT32 nStopIdx = m_nCurrentBufferIndex;

	INT32 nLimit = m_nBackUpBufferCount - nExceptionValue;
	if( nLimit < 0) return;

	for( i=0; i<nLimit; i++)
	{
		nStopIdx--;
		if( nStopIdx < 0) nStopIdx = MAX_CHAT_BUFFER-1;
	}

	if( m_nBackUpBufferIndex !=  nStopIdx)
	{
		m_nBackUpBufferIndex--;
		if( m_nBackUpBufferIndex < 0)
		{
			m_nBackUpBufferIndex = MAX_CHAT_BUFFER-1;
		}
	}

	CalcRatio( nExceptionValue);

	I3TRACE("Decrese Index: %d\n", m_nBackUpBufferIndex);
}

void CGameChatBox::CalcBackupBufferByRatio( REAL32 fVal, INT32 nExceptionValue)
{
	// 스크롤할 분량이 되지 않으면 처리하지 않는다
	if (nExceptionValue >= m_nBackUpBufferCount)
	{
		return;
	}

	INT32 nResultIdx = 0;
	INT32 nIdxArray[MAX_CHAT_BUFFER] = {0,};
	INT32 nStartIdx = m_nCurrentBufferIndex;

	m_fBackUpBufferRatio = 1.0f - fVal;
	fVal = 1.0f - fVal;

	for(INT32 i = 0; i < m_nBackUpBufferCount; i++)
	{
		nIdxArray[i] = nStartIdx;
		nStartIdx--;

		if (nStartIdx < 0)
		{
			nStartIdx = MAX_CHAT_BUFFER-1;
		}
	}

	INT32 nTemp = m_nBackUpBufferCount - nExceptionValue;

	if (nTemp < 0)
	{
		nTemp = MAX_CHAT_BUFFER + nTemp;
	}

	nResultIdx = (INT32)(nTemp * fVal);

	m_nBackUpBufferIndex = nIdxArray[nResultIdx];
}

void CGameChatBox::SetEnable( bool bTrue, REAL32 tm)
{
	i3GameNode::SetEnable( bTrue, tm);
}


void CGameChatBox::OnUpdate( REAL32 rDeltaSeconds )
{
	i3GameObjBase::OnUpdate( rDeltaSeconds );
}
