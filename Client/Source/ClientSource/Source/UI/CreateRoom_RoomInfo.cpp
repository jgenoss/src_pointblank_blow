#include "pch.h"
#include "CreateRoom_RoomInfo.h"

#include "i3Base/string/ext/remove_all_blank.h"

#include "MyRoomInfoContext.h"

#include "UIUtil.h"
#include "ClientStageInfoUtil.h"

//ROOM_INFO======================================================================================================

CreateRoom_RoomInfo::CreateRoom_RoomInfo(i3UIScene * pBase)
{
	CreateRoom_Base::CreateRoom_Base(pBase);

	m_pTitleBox = nullptr;
	m_pPwBox = nullptr;
	m_pPwCheckBox = nullptr;
}

CreateRoom_RoomInfo::~CreateRoom_RoomInfo()
{
	CreateRoom_Base::~CreateRoom_Base();
}

void		CreateRoom_RoomInfo::InitData(bool isCreateRoom)
{
	m_bisCreateRoom = isCreateRoom;

	m_pTitleBox = (i3UIEditBox*)m_pScene->FindChildByName("i3UIEditBox_RoomName");
	m_pPwBox = (i3UIEditBox*)m_pScene->FindChildByName("i3UIEditBox_Password");
	m_pPwCheckBox = (i3UICheckBox*)m_pScene->FindChildByName("i3UICheckBox_Password");

	if (m_pPwBox)	m_pPwBox->setUseCopyNPaste(false);

	if (m_bisCreateRoom)
	{
		ROOM_INFO_BASIC	roomInfo;
		MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);

		// 비밀번호 값 설정 - NULL종료 문자열이 아니다! 4byte 바이너리!
		char strPassword[NET_ROOM_PW + 1] = { 0, };
		i3::string_ncopy_nullpad(strPassword, MyRoomInfoContext::i()->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);

		i3::wliteral_range wstrRoomTitle = roomInfo._Title;
		RoomInfoUIUpdate(wstrRoomTitle, strPassword);
	}
	else
	{
		INT32 nCreateRoomRnd = i3Math::Rand() % FIXED_ROOMNAME_COUNT;
		char szTemp[MAX_STRING_COUNT] = { 0 };

		i3::wliteral_range wRoomRange;
		if (nCreateRoomRnd < 4)
		{
			i3::snprintf(szTemp, MAX_STRING_COUNT, "STBL_IDX_ROOMNAME_%d", nCreateRoomRnd + 1);	// 1 ~ 4
			const i3::rc_wstring& wstrRoomName = GAME_RCSTRING(szTemp);
			wRoomRange = wstrRoomName;
		}
		else
			wRoomRange = m_pTitleBox->getTextRange();

		RoomInfoUIUpdate(wRoomRange, "");
	}
}

bool		CreateRoom_RoomInfo::ConfimRoomInfo(ROOM_INFO_ALL & info)
{
	i3::stack_wstring wstrTitleName;

	if (!SetTitle(wstrTitleName))	return false;
	if (SetPassword())				info._Basic._InfoFlag |= ROOM_INFO_FLAG_PRIVATE_MODE;

	i3::safe_string_copy(info._Basic._Title, wstrTitleName, NET_ROOM_NAME_SIZE);

	return true;
}

void		CreateRoom_RoomInfo::ResetPasswordEditbox()
{
	if (m_pPwBox != nullptr)
	{
		if (m_pPwCheckBox->isSelected() == false)
			m_pPwBox->SetText(L"");
	}
}

void		CreateRoom_RoomInfo::RoomInfoUIUpdate(const i3::wliteral_range& wRoomTitleRng, char * szPassword)
{
	LuaState * L = _CallLuaFunction("SetDefaultInfo");
	if (L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wRoomTitleRng);
		i3Lua::PushString(L, szPassword);				// TODO : 비밀번호는 영문자로 일단 간주한다. UTF-8처리 (2014.07.23.수빈) 
		_EndLuaFunction(L, 2);
	}

	L = _CallLuaFunction("SetCheckPassword");
	if (L != nullptr)
	{
		//i3Lua::PushBoolean(L, '\0' != szPassword[0]);
		i3Lua::PushInteger(L, '\0' != szPassword[0]);
		_EndLuaFunction(L, 1);
	}
}

bool		CreateRoom_RoomInfo::SetTitle(i3::stack_wstring &wstrTitleName)
{
	//방제목
	LuaState * L = _CallLuaFunction("getTitleName");
	{
		const char * szTitleName = UILua::ReturnLuaString(L, 0);		// UTF8로 간주한다. (2014.07.14.수빈)
		i3::utf8_to_utf16(szTitleName, wstrTitleName);
	}
	i3::stack_wstring wstrTemp = wstrTitleName;
	i3::remove_all_blank(wstrTemp);
	if (i3::generic_string_size(wstrTemp) <= 0) {
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_LOBBY_PLEASE_WRITE_ROOM_NAME"));/*방 제목을 입력하여 주십시오*/
		return false;
	}

	// 방생성 이름이 기본 제공하는 제목이라면 필터링하지 않는다. 기본 방제에서 필터에 걸려 방 생성이 안되는 문제를 해결함. komet
	bool bNeedFilter = true;
	i3::stack_string strTemp2;
	for (INT32 i = 0; i<4; i++)
	{
		i3::sprintf(strTemp2, "STBL_IDX_ROOMNAME_%d", i + 1);	// 1 ~ 4
		if (i3::generic_is_iequal(GAME_RCSTRING(strTemp2), wstrTitleName))
		{
			bNeedFilter = false;	// 기본 제공하는 방제목이므로 필터할 필요없다.
			break;
		}
	}

	if (bNeedFilter)
	{
		if (false == g_pFramework->IsFilterOK(wstrTitleName) || i3::icontain_string(wstrTemp, L"{col:") >= 0)
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_LOBBY_WRONG_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다*/
			return false;
		}
	}

	return true;
}

bool		CreateRoom_RoomInfo::SetPassword()
{
	LuaState * L = _CallLuaFunction("getPassword");
	const char * pszPassword = UILua::ReturnLuaString(L, 0);

	if (pszPassword != nullptr)
	{
		INT32 Length = i3::generic_string_size(pszPassword);

		if (Length > 0 && Length <= sizeof(char) * NET_ROOM_PW)
		{
			if (!m_bisCreateRoom)
			{
				MyRoomInfoContext::i()->SetMyRoomPW(pszPassword, Length);
				return true;
			}
			else
			{
				if (i3::generic_is_iequal(MyRoomInfoContext::i()->getMyRoomPW(), pszPassword) == false)
				{
					//일단 현재 설정된 패스워드를 전부 Clear 후 재설정
					MyRoomInfoContext::i()->SetMyRoomPW(pszPassword, Length);
					GameEventSender::i()->SetEvent(EVENT_CHANGE_ROOM_PASSWORD_N);
					return true;
				}
			}
		}
		else if (Length == 0 && m_bisCreateRoom)
		{
			MyRoomInfoContext::i()->ClearMyRoomPW();
			GameEventSender::i()->SetEvent(EVENT_CHANGE_ROOM_PASSWORD_N);
			return false;
		}
	}

	return false;
}