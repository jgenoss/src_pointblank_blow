#include "pch.h"
#include "UITabClanMember.h"

#include "UIUtil.h"

#include "../GameGUI.h"

//----------------------------------------------------------------------------------//
//								Lua Glue Function									//
//----------------------------------------------------------------------------------//
extern "C" {
}

LuaFuncReg UITabClanMember_Glue[] = {
	{ nullptr,					nullptr}
};

I3_CLASS_INSTANCE( UITabClanMember);//, UITabClan_Normal);

UITabClanMember::UITabClanMember()
{
}

UITabClanMember::~UITabClanMember()
{

}

void UITabClanMember::_SetClanMemberInfo( INT32 slot, CLAN_MEMBER_BASIC_INFO * pInfo)
{
	LuaState * L;

	if( pInfo != nullptr)
	{
		L = _CallLuaFunction( "SetMemberInfo");
		if( L != nullptr)
		{
			// 계급 툴팁
			i3::rc_wstring wstrRankToolTip = GetCharaRank( pInfo->_rank);	// 계급명
			
			// 닉네임 컬러
			const I3COLOR * pNickColor = GameUI::GetNickNameColor( pInfo->_color);

			//	멤버 직급
			i3::wstring wstrLevel;
			GetMemberLevelName2( pInfo->_level, wstrLevel);
			
			// 직급 컬러
			COLOR Color;

			switch(pInfo->_level)
			{
			case CLAN_MEMBER_LEVEL_MASTER:
				i3Color::Set(&Color, (UINT8)205, 205, 207, 255);
				break;
			case CLAN_MEMBER_LEVEL_STAFF:
				i3Color::Set(&Color, (UINT8)0,	255, 0, 255);
				break ;
			case CLAN_MEMBER_LEVEL_REGULAR:
			default:
				i3Color::Set(&Color, (UINT8)214, 186, 147, 255);
				break;
			}

			// 전적 정보
			i3::stack_wstring wstrRecord;

			i3::sprintf(wstrRecord , L"%d %s , %d %s", 
				pInfo->m_win,	GAME_RCSTRING("STBL_IDX_CLAN_WIN"),
				pInfo->m_lose,	GAME_RCSTRING("STBL_IDX_CLAN_LOSE"));

			i3Lua::PushInteger( L, slot);
			i3Lua::PushInteger( L, pInfo->_rank);		// 계급
			i3Lua::PushStringUTF16To8( L, wstrRankToolTip);	// 계급 tooltip
			i3Lua::PushStringUTF16To8( L, pInfo->_nick);	// 닉네임
			i3Lua::PushColor( L, pNickColor);			// 닉네임 컬러
			i3Lua::PushStringUTF16To8( L, wstrLevel);		// 직급
			i3Lua::PushColor( L, &Color);				// 직급 컬러
			i3Lua::PushStringUTF16To8(L , wstrRecord);		// 전적정보

			_EndLuaFunction( L, 14);
		}
	}
	else
	{
		L = _CallLuaFunction( "SetMemberInfo");
		if( L != nullptr)
		{
			COLOR Color;
			i3Color::Set(&Color, (UINT8)214, 186, 147, 255);

			i3Lua::PushInteger( L, slot);
			i3Lua::PushInteger( L, -1);			// 계급
			i3Lua::PushStringUTF16To8( L, L"");	// 계급 tooltip
			i3Lua::PushStringUTF16To8( L, L"");	// 닉네임
			i3Lua::PushColor( L, &Color);		// 닉네임 컬러
			i3Lua::PushStringUTF16To8( L, L"");	// 직급
			i3Lua::PushColor( L, &Color);		// 직급 컬러
			i3Lua::PushStringUTF16To8(L , L"");	// 전적정보
			_EndLuaFunction( L, 14);
		}
	}
}

void UITabClanMember::_BuildClanMemberList( void)
{
	INT32 nNewCount = g_pFramework->GetClanContext()->GetMyClanMemberCount(); 
	
	//	클랜원 정렬 리스트 재구성
	m_vecClanMemberList.clear();
	m_vecClanMemberList.reserve(nNewCount);

	for( INT32 idx = 0 ; idx < nNewCount ; ++idx )
	{
		const CLAN_MEMBER_BASIC_INFO * pSrcInfo = g_pFramework->GetClanContext()->GetMyClanMember(idx);

		if( pSrcInfo->_i64UID > 0)
		{
			CLAN_MEMBER_BASIC_INFO_PTR info_ptr = i3::make_shared<CLAN_MEMBER_BASIC_INFO>();
			CLAN_MEMBER_BASIC_INFO* pDestInfo = info_ptr.get();
			i3mem::Copy( pDestInfo, pSrcInfo, sizeof(CLAN_MEMBER_BASIC_INFO));
			m_vecClanMemberList.push_back(info_ptr);
		}
		
	}

	// 클랜 등급으로 결정
	struct cbSortClanMember
	{
		bool operator()(const CLAN_MEMBER_BASIC_INFO_PTR& lhs, const CLAN_MEMBER_BASIC_INFO_PTR& rhs) const
		{
			return lhs->_level < rhs->_level;
		}
	};

	i3::sort(m_vecClanMemberList.begin(), m_vecClanMemberList.end(), cbSortClanMember() );
}

void UITabClanMember::_UpdateClanMember( void)
{
	if( IsReadyToUpdate() == false)return;

	//	멤버 리스트 재구성
	_BuildClanMemberList();

	LuaState * L = _CallLuaFunction( "ClearList");

	UINT32 nCount = m_vecClanMemberList.size();

	if(nCount < 10)
		nCount = 10;

	if( L != nullptr)
	{
		i3Lua::PushInteger( L, nCount);
		_EndLuaFunction( L, 1);
	}

	for(size_t i = 0; i < nCount; i++)
	{
		if(i < m_vecClanMemberList.size())
		{
			CLAN_MEMBER_BASIC_INFO * pMember = m_vecClanMemberList[ i].get();

			if( pMember != nullptr)
			{
				if( pMember->_i64UID > 0)
					_SetClanMemberInfo( i, pMember);
				else
					_SetClanMemberInfo( i, nullptr);
			}
			else
				_SetClanMemberInfo( i, nullptr);
		}
		else
			_SetClanMemberInfo( i, nullptr);
	}	
}

/*virtual*/ void UITabClanMember::OnCreate( i3GameNode * pParent)
{
	UITabClan_Normal::OnCreate( pParent);

	// add scene
	AddScene("Scene/Clan/PointBlankRe_Clan_Memeber_Info.i3UIs", UITabClanMember_Glue);
}

/*virtual*/ void UITabClanMember::OnUpdate( REAL32 rDeltaSeconds)
{
	UITabClan_Normal::OnUpdate( rDeltaSeconds);

	//	클랜원 정보 업데이트
	if( g_pFramework->GetClanContext() != nullptr)
		g_pFramework->GetClanContext()->UpdateClanMember(rDeltaSeconds);
}

/*virtual*/ bool UITabClanMember::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UITabClan_Normal::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if( g_pEnvSet->IsBaseOnUIVersion_1_0() )
	{
		i3UIListView_Box* pBox = (i3UIListView_Box*)GetScene(0)->FindChildByName("ListView_ClanMember");
		pBox->getHeader()->SetHeaderFont("", 8 );
	}
	else if( g_pEnvSet->m_nUIVersion == UFV_1_1 )
	{
		i3UIListView_Box* pBox = (i3UIListView_Box*)GetScene(0)->FindChildByName("ListView_ClanMember");
		pBox->getHeader()->SetHeaderFont("", 9 );
	}

	g_pFramework->GetClanContext()->EnterClanMember();

	_UpdateClanMember();

	return true;
}

/*virtual*/ void UITabClanMember::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	UITabClan_Normal::ProcessGameEvent( event, arg, UserData);

	switch( event)
	{
		case EVENT_CLAN_MEMBER_LIST :	
			_UpdateClanMember();	
			break;
	}
}


