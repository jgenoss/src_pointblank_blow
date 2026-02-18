#include "pch.h"
#include "GameMaster.h"

#include "LobbyContext.h"
#include "ObserverBattle.h"
#include "UserInfoContext.h"
#include "BattleSlotContext.h"
#include "GameStateMgr.h"

#include "StageBattle/UserContext.h"

#include "i3Base/string/ext/atoi.h"

#include "i3Base/string/ext/utf16_to_mb.h"

CGameMaster::CGameMaster()
{
	m_bIsPaused = false;
};

CGameMaster::~CGameMaster()
{

};


bool CGameMaster::SetGMCommand(const i3::wliteral_range& wCommandRng )
{
	if ( ! UserInfoContext::i()->CheckAuthLevel() ) return false;

	if ( i3::generic_string_size(wCommandRng) <= 0)
		return false;

	GM_COMMAND_TYPE	CommandType = GetCommandType(wCommandRng);
	
	INT32 Arg = GetCommandTypeArg(CommandType, wCommandRng );

	switch (CommandType)
	{
	case GM_COMMAND_USER_KICK:
		{
			INT8 UserIndex = (INT8)Arg;
			if ( (0 <= UserIndex) && (SLOT_MAX_COUNT > UserIndex) )
				GameEventSender::i()->SetEvent(EVENT_KICK_USER, &UserIndex);
		}
		return true;
	case GM_COMMAND_USER_EXIT:
		{
			INT8 UserIndex = (INT8)Arg;
			if ( (0 <= UserIndex) && (SLOT_MAX_COUNT > UserIndex) )
				GameEventSender::i()->SetEvent(EVENT_EXIT_USER, &UserIndex);
		}
		return true;
	case GM_COMMAND_USER_BLOCK:
		{
			INT8 UserIndex = (INT8)Arg;

			INT32 i32CommentPtr = 0;
			INT32 i32CommentLength = GetCommandArgLength(2, wCommandRng, &i32CommentPtr);

			i32CommentLength = MIN( i32CommentLength, GM_COMMENT_SIZE );

			i3::const_wchar_range comment_rng( wCommandRng.begin() + i32CommentPtr , wCommandRng.begin() + i32CommentPtr + i32CommentLength );

			TTCHAR szComment[GM_COMMENT_SIZE+1];

			i3::safe_string_copy(szComment, comment_rng, GM_COMMENT_SIZE);
			
			szComment[i32CommentLength] = 0;

			if ( (0 <= UserIndex) && (SLOT_MAX_COUNT > UserIndex) )
				GameEventSender::i()->SetEvent(EVENT_BLOCK_USER, &UserIndex, szComment);
		}
		return true;
	case GM_COMMAND_ROOM_DESTROY:
		{
			GameEventSender::i()->SetEvent(EVENT_DESTROY_ROOM);
		}
		return true;
	}

	return false;
}

bool CGameMaster::SetGMLobbyCommand(const i3::wliteral_range& wCommandRng )
{
	if ( ! UserInfoContext::i()->CheckAuthLevel() ) return false;

	if ( i3::generic_string_size(wCommandRng) <= 0)
		return false;

	GM_COMMAND_TYPE	CommandType = GetCommandType(wCommandRng);	

	switch (CommandType)
	{
	case GM_COMMAND_CHEAT_RANDOMBOX:
		{
			INT32 i32Args[2];
			if ( GetCommandTypeArgList( CommandType, wCommandRng, i32Args ) )
			{
				T_ItemDBIdx TWareDBIdx	= CInvenList::i()->FindWareDBIdx_ByItemID(i32Args[0]); 
				INT32 i32LoopCount		= i32Args[1];

				bool bWrongLoopCount	= ( ( i32LoopCount < GM_CHEAT_RANDOMBOX_MIN ) || ( GM_CHEAT_RANDOMBOX_MAX < i32LoopCount ) );
				bool bWrongItemID		= ( TWareDBIdx == INVALID_WAREDB_INDEX );
				i3::rc_wstring wstrMsg;

				if ( bWrongLoopCount )
				{
					i3::sprintf( wstrMsg, L"[Cheat RandomBox] Invalid Loop Count : %d", i32LoopCount );
					g_pFramework->getChatBox()->PutSystemChatting( wstrMsg );
				}
				else if ( bWrongItemID )
				{
					i3::sprintf(wstrMsg, L"[Cheat RandomBox] Invalid Item ID : %d", i32Args[0]);
					g_pFramework->getChatBox()->PutSystemChatting(wstrMsg);
				}
				else
					GameEventSender::i()->SetEvent(EVENT_GM_CHEATKEY_RANDOMBOX, &TWareDBIdx, &i32LoopCount);
			}
			else
				g_pFramework->getChatBox()->PutSystemChatting(L"[Cheat RandomBox] Invalid Command");
		}
		return true;
	case GM_COMMAND_USER_EXIT:
		{	
			i3::rc_wstring wstrNickName;

			INT32 SessionIndex = GetCommandTypeSessionIdx(CommandType, wCommandRng, wstrNickName);
			// 종료시킬 유저의 SessionIdx, 닉네임 
			GameEventSender::i()->SetEvent(EVENT_LOBBY_EXIT_USER, &SessionIndex, wstrNickName.c_str() );
		}
		return true;
	}

	return false;
}


GM_COMMAND_TYPE CGameMaster::GetCommandType(const i3::wliteral_range& wCommandRng)
{

	if ( i3::icontain_string_head( wCommandRng, COMMAND_USER_KICK)  )
		return GM_COMMAND_USER_KICK;
	else if( i3::icontain_string_head( wCommandRng, COMMAND_USER_EXIT)  )
		return GM_COMMAND_USER_EXIT;
	else if( UserInfoContext::i()->CheckAuthLevel(AUTH_LEVEL_GM) )
	{// revision 44121
		if( i3::icontain_string_head( wCommandRng, COMMAND_USER_BLOCK)  )
			return GM_COMMAND_USER_BLOCK;
		else if( i3::icontain_string_head( wCommandRng, COMMAND_ROOM_DESTROY)  )
			return GM_COMMAND_ROOM_DESTROY;
		else if (i3::icontain_string_head( wCommandRng, COMMAND_CHEAT_RANDOMBOX ) )
			return GM_COMMAND_CHEAT_RANDOMBOX;
	}

	return GM_COMMAND_UNKNOWN;
}


INT32 CGameMaster::GetCommandTypeArg(GM_COMMAND_TYPE Type, const i3::wliteral_range& wCommandRng )
{
	switch(Type)
	{
	case GM_COMMAND_USER_KICK:
		{
			INT32 ScoreIdx  = -1;
			// 닉네임으로 찾기, 파라메터의 자리수가 2보다 크면

			i3::const_wchar_range wArgRange = wCommandRng;
			wArgRange.advance_begin(i3::generic_string_size(COMMAND_USER_KICK) + 1);

			if(i3::generic_string_size(wArgRange) > 2 ) 
			{
				INT32 * pArgStart = nullptr;
				if(GetCommandArgLength(2, wCommandRng, pArgStart) != 0)
				{
					//2번째 Argument가 존재할 경우
					//e.g) /Kick blue 1
					
//					const char * pArg = (CommandString + i3::generic_string_size(COMMAND_USER_KICK) + 1);

					if(i3::icontain_string(wArgRange, COMMAND_ARG_TEAM_BLUE) != -1)
					{
						//ScoreIdx는 -1 빼주어야 배열에서 찾는게 가능.
						i3::const_wchar_range wNumberRange = wArgRange;
						wNumberRange.advance_begin(i3::generic_string_size(COMMAND_ARG_TEAM_BLUE) + 1);

						ScoreIdx = i3::atoi(wNumberRange);
						if(ScoreIdx > 0)
						{
							return UserContext::i()->GetIndexedRankderIdx(TEAM_BLUE, ScoreIdx - 1);
						}
					}
					else
					{
						i3::const_wchar_range wNumberRange = wArgRange;
						wNumberRange.advance_begin(i3::generic_string_size(COMMAND_ARG_TEAM_RED) + 1);

						ScoreIdx = i3::atoi(wNumberRange);
						if(ScoreIdx > 0)
						{
							return UserContext::i()->GetIndexedRankderIdx(TEAM_RED, ScoreIdx - 1);
						}
					}
				}
				else
				{
					//1번째 Argument만 존재할 경우
					//e.g) /Kick Nickname
					for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
					{
						if( i3::icontain_string(BattleSlotContext::i()->getNickForSlot(i), wArgRange ) != -1) 
						{
							return i;
						}
					}
				}

				return -1;

			}
			else 
			{
				INT32 UserIndex = i3::atoi(wArgRange);
				return UserIndex;
			}

		}
		break;
	case GM_COMMAND_USER_EXIT:
		{
			i3::const_wchar_range wArgRange = wCommandRng;
			wArgRange.advance_begin( i3::generic_string_size(COMMAND_USER_KICK) + 1 );

			if(i3::generic_string_size(wArgRange) > 2) 
			{
				for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
				{
					if(i3::contain_string(BattleSlotContext::i()->getNickForSlot(i), wArgRange ) != -1) 
					{
						return i;
					}
				}
				return -1;

			}
			else 
			{
				INT32 UserIndex = i3::atoi(wArgRange);
				return UserIndex;
			}
		}
		break;
	case GM_COMMAND_USER_BLOCK:
		{
			// 닉네임으로 찾기, 파라메터의 자리수가 2보다 크면
			INT32 i32NickLength = GetCommandArgLength(1, wCommandRng);
			
			i3::const_wchar_range wArgRng = wCommandRng;
			wArgRng.advance_begin(i3::generic_string_size(COMMAND_USER_BLOCK) + 1);

			if( i32NickLength > 2) 
			{
				for(INT32 i=0; i<SLOT_MAX_COUNT; i++)
				{
					if(i3::icontain_string(BattleSlotContext::i()->getNickForSlot(i), wArgRng) != -1) 
					{
						return i;
					}
				}
				return -1;

			}
			else {
				INT32 UserIndex = i3::atoi(wArgRng);
				return UserIndex;
			}
		}
		break;
	}

	return -1;
}


bool CGameMaster::GetCommandTypeArgList(GM_COMMAND_TYPE Type, const i3::wliteral_range& wCommandRng, INT32 * pOutArgArr)
{
	switch (Type)
	{
	case GM_COMMAND_CHEAT_RANDOMBOX:
		{
			i3::const_wchar_range wArgRng = wCommandRng;
			wArgRng.advance_begin(i3::generic_string_size(COMMAND_CHEAT_RANDOMBOX) + 1);

			INT32 * pOut = pOutArgArr;
			wchar_t wstrArgForParsing[10];
			{
				INT32 * pArgStart = nullptr;
				INT32 i32Arg1Length = GetCommandArgLength(1, wCommandRng, pArgStart) + 1;
				if (i32Arg1Length < 2)
					return false;

				i3mem::Copy(wstrArgForParsing, wArgRng.begin(), sizeof(wchar_t) * i32Arg1Length);
				pOut[0] = i3::atoi(wstrArgForParsing);

				i3::const_wchar_range wArg2Rng = wArgRng;
				wArg2Rng.advance_begin( (i32Arg1Length ) );

				//i3mem::Copy(strArgForParsing, wArg2Rng.begin(), i32Arg2Length);
				pOut[1] = i3::atoi(wArg2Rng);
			}
			//else
			//	return false;
		}
		return true;
	default :
		break;
	}
	return false;
}

INT32 CGameMaster::GetCommandTypeSessionIdx(GM_COMMAND_TYPE Type, const i3::wliteral_range& wCommandRng, i3::rc_wstring& out_wstrNickName )
{
	switch(Type)
	{
	case GM_COMMAND_USER_EXIT:
		{
			INT32 i32NickLength = GetCommandArgLength(1, wCommandRng);
			i32NickLength = MIN( i32NickLength, NET_NICK_NAME_SIZE - 1 );	// 입력받은 닉네임 길이 체크

			i3::const_wchar_range wArgRng = wCommandRng;
			wArgRng.advance_begin(i3::generic_string_size(COMMAND_USER_EXIT) + 1);

			i3::safe_string_copy(out_wstrNickName, wArgRng, NET_NICK_NAME_SIZE);

			INT32 i32SessionIdx = LobbyContext::i()->getUserSessionIdxByUserNickName( out_wstrNickName );
			
//			USER_INFO_BASIC		MyUserInfoBasic;
//			UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

			return i32SessionIdx;
		}
		break;
	}

	return -1;
}


// Command에서 Arg의 길이를 구해온다 (ArgIndex 0: Command, ArgIndex 1: Arg1 ...)
INT32 CGameMaster::GetCommandArgLength(INT8 ArgIndex, const i3::wliteral_range& wCommandRng, INT32 * ArgStartPtr)
{
	INT32 Length = 0;
	INT32 RefIndex = 0;

	for( UINT8 i = 0; i < ArgIndex; i ++ )
	{
		// RefIndex가 너무 커지면 오류로 판단
		if( RefIndex > GM_COMMENT_SIZE*2 )
			return 0;

		// RefIndex
		while( wCommandRng[RefIndex] != L' ' &&
			   wCommandRng[RefIndex] != L'\t' ) // space, horizontal Tab
		{
			if( wCommandRng[RefIndex] == 0 )
			{
				// 요청한 Arg가 없다.
				return 0;
			}
			RefIndex += 1;
		}
		RefIndex += 1;
	}
	if( ArgStartPtr != nullptr )
		*ArgStartPtr = RefIndex;

	// Length
	while( wCommandRng[RefIndex+Length] != L' ' &&
		   wCommandRng[RefIndex+Length] != L'\t' &&
		   wCommandRng[RefIndex+Length] != L'\0' )
	{
		// 한 Arg의 길이가 너무 길면 오류로 판단
		if( Length > GM_COMMENT_SIZE )
			return 0;

		Length += 1;
	}

	return Length;
}

void CGameMaster::SetPause(bool bPause)	
{ 
	if( gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE )return;
	m_bIsPaused = bPause; 
}