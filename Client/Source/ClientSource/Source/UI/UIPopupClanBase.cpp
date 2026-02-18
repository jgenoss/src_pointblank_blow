#include "pch.h"
#include "UIPopupClanBase.h"

#include "UIUtil.h"
#include "i3Base/string/ext/contain_char.h"

I3_CLASS_INSTANCE( UIPopupClanBase);//, UIPopupBase);

UIPopupClanBase::UIPopupClanBase(void)
{

}

UIPopupClanBase::~UIPopupClanBase(void)
{

}

void UIPopupClanBase::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);
	if( m_StringFilter == nullptr)
	{
		m_StringFilter.reset(i3::make_pscoped<CStrFilter>());
		m_StringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
	}

	if( m_StringFilter2 == nullptr)
	{
		m_StringFilter2.reset(i3::make_pscoped<CStrFilter>()); 
		m_StringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);
	}
}

bool UIPopupClanBase::_CheckNickNameOverLap( const i3::wliteral_range& wTextRng) const
{
	if( i3::generic_string_size(wTextRng) <= 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_NAME"));	//	"클랜명을 적어주세요"
		return false;
	}

	{
		bool GoodNickName = true;

		INT32 idxBlank = i3::contain_char(wTextRng, L' ');

		if( -1 != idxBlank )
		{
			I3PRINTLOG(I3LOG_DEFALUT, "NickName Contained Blank");
			GoodNickName = false;
		}
		else if( false == m_StringFilter2->IsFilterNickOK( wTextRng ) )
		{
			I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected IsFilter");
			GoodNickName = false;
		}
		else if( false == m_StringFilter->IsFilterMatchOK( wTextRng ) )
		{
			I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected IsFilterSame");
			GoodNickName = false;
		}
		else if( false == g_pFramework->IsFilterNickOK( wTextRng ) )
		{
			I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected IsFilter");
			GoodNickName = false;
		}
		else if( false == g_pFramework->CheckValidCharacterByLocale( wTextRng ) )
		{
			I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected CheckValidCharacterByLocale");
			GoodNickName = false;
		}

		if( !GoodNickName )
		{
			//	입력 할 수 없는 단어가 포함되어 있습니다"
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));

			return false;
		}
	}

	return true;
}

bool	UIPopupClanBase::_CheckClanAddressOverLap(const i3::wliteral_range& wTextRng) const
{
#ifdef CLAN_ADDRESS_ENABLE
	if( i3::generic_string_size(wTextRng) <= 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));	//	"클랜 아지트 주소를 적어주세요."

		return false;
	}

	{
		INT32 idxBlank = i3::contain_char(wTextRng, L' ');

		if( -1 != idxBlank || 
			false == m_pStringFilter2->IsFilter( wTextRng ) || 
			false == m_pStringFilter->IsFilterSame( wTextRng ) || 
			false == g_pFramework->IsFilter( wTextRng ) ) 
		{
			//	"입력 할 수 없는 단어가 포함되어 있습니다."
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
			return false;
		}

		//	check step 5
		{
			CStringFilter* pStringFilter = CStringFilter::new_object();
			pStringFilter->CreateFilter(STR_FILTER_URL);

			if( pStringFilter->IsFilterSame( wTextRng ) == false ) {
				//	"입력 할 수 없는 단어가 포함되어 있습니다."
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
				I3_SAFE_RELEASE( pStringFilter );
				return false;
			}

			I3_SAFE_RELEASE( pStringFilter );
		}
	}

	return true;

#else
	// m_bCheckedAddress = true;

	return true;

#endif
}

bool	UIPopupClanBase::_CheckClanAddressValidate(const i3::wliteral_range& wTextRng) const
{
#ifdef CLAN_ADDRESS_ENABLE
	if( wTextRng[0] == 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_PLEASE_INPUT_AZIT"));	//	"클랜 아지트 주소를 적어주세요."
		return false;
	}

	const wchar_t* wszText  = wTextRng.c_str();

	while(*pszText)
	{
		//	클랜 주소는 영문 소문자와 숫자로만 입력 가능하므로 일일이 체크한다. (0 ~ 9 && a ~ z)
		if( (*pszText >= '0' && *pszText <= '9') || (*pszText >= 'a' && *pszText <= 'z')) 
		{ 
			pszText++;

			continue;
		}
		else
		{
			//	"클랜 아지트 URL은\n영문 소문자와 숫자만 사용 가능합니다."
			//	"아지트 주소 오류"
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_ONLY_ENGLISH_OR_NUMERIC"), 
				&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_ERROR_AZIT"));

			return false;
		}
	}	

	return true;
#endif
	
	return true;
}

void UIPopupClanBase::UseName( i3::stack_wstring& wstrSaveChar, const i3::rc_wstring& wstrName)
{
	i3::sprintf(wstrSaveChar, L"%s\n\n%s", wstrName , GAME_RCSTRING("STBL_IDX_CLAN_CAN_USE"));
}

bool UIPopupClanBase::_OnPopupCheckOverlapping( i3::stack_wstring& wstrSaveChar, const i3::rc_wstring& wstrName, bool bDuplicate)
{
	if( bDuplicate)
	{
		// "은/는 이미 사용중이거나 사용할수 없는 이름입니다. \n 다른 이름을 사용해 주십시오."
		i3::sprintf(wstrSaveChar, L"%s\n\n%s",  wstrName, GAME_RCSTRING("STBL_IDX_CLAN_ALREADY_USED_NAME"));
		return false;
	}
	else
	{
		// "은/는 사용하실 수 있습니다."
		UseName(wstrSaveChar, wstrName);
		return true;
	}
}

// 
bool UIPopupClanBase::_OnPopupCheckOverlappingAddress(  i3::stack_wstring& wstrSaveChar, const i3::rc_wstring& wstrName, bool bDuplicate)
{
	if( bDuplicate)
	{
		//	"은/는 이미 사용중이거나 사용할수 없는 주소입니다.\n 다른 주소를 선택해 주세요."
		i3::sprintf(wstrSaveChar, L"%s\n\n%s", wstrName , GAME_RCSTRING("STBL_IDX_CLAN_ALREADY_USED_NAME"));
		return false;
	}
	else
	{
		//	"은/는 사용하실 수 있습니다."
		UseName(wstrSaveChar, wstrName);
		return true;
	}
}
/*
bool	UIPopupClanBase::_OnEventDuplicateName( const char * pszName, INT32 Arg)
{
	char szTemp[MAX_PATH] = ""; 

	// 클랜 생성화면
	if( Arg == 0)		//	클랜명이 중복되지 않는다
	{	
		//	"은/는 사용하실 수 있습니다."
		sprintf_s( szTemp, "\n {col:255, 255, 255, 255} %s {/col} \n \n %s", pszName, GAME_RCSTRING("STBL_IDX_CLAN_CAN_USE"));
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  szTemp);
		return true;
	}
	else		//	클랜명이 중복된다.
	{
		_OnPopupCheckOverlapping(szTemp, pszName, true);
		return false;
	}		
}

bool	UIPopupClanBase::_OnEventDuplicateAddress( const char * pszAddress, INT32 Arg)
{
	char szTemp[MAX_PATH] = ""; 

	// 클랜 생성화면
	if( Arg == 0)
	{
		//	"은/는 사용하실 수 있습니다."
		sprintf_s( szTemp, "\n {col:255, 255, 255, 255} %s {/col} \n \n %s" , 
			pszAddress, GAME_RCSTRING("STBL_IDX_CLAN_CAN_USE"));
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  szTemp);
		return true;
	}
	else
	{			
		_OnPopupCheckOverlappingAddress(szTemp, pszAddress, true);
		return false;
	}
}
*/