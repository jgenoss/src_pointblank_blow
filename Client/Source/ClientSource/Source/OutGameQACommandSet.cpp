#include "pch.h"
#include "GlobalVariables.h"

#include "UI/UIMainFrame.h"
#include "OutGameQACommandSet.h"
#include "UserInfoContext.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/atoi.h"

//1초에 30번까지 패킷 전송이 가능함.
#define TIME_SPACE_PER_PACKET 0.033

I3_CLASS_INSTANCE( COutGameQACommandSet);

COutGameQACommandSet::~COutGameQACommandSet()
{
	resultBuffer.clear();
}

void COutGameQACommandSet::SetEventForCommand(COMMAND_TYPE commandType, const void * pArg, const void * pArg1, const void * pArg2, LPCSTR szFile, int nLine)
{
	switch (commandType)
	{
	case COMMAND_RANDOMBOX:
		GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH, pArg);
		break;
	default:
		break;
	}
}

void COutGameQACommandSet::ProcessCommand(REAL32 rDeltaSeconds)
{
	if( !UserInfoContext::i()->CheckSpecialAbility(SA_CHEAT) ) return;

	//ProcessRandomBoxCommand(rDeltaSeconds);	콘솔창 전용으로 변경.
}

void COutGameQACommandSet::ProcessRandomBoxCommand(REAL32 rDeltaSeconds)
{
	if(m_bIsRandomBoxTestEnabled)
	{
		if(m_iterationCount <= 0)
		{
			m_iterationCount = 0;
			m_timeBuffer += rDeltaSeconds;
			if(m_timeBuffer > 0.5)
			{
				m_bIsRandomBoxTestEnabled = false;
				SetCSVLog();
			}
		}
		else
		{
			m_timeBuffer += rDeltaSeconds;
			if(m_timeBuffer >= TIME_SPACE_PER_PACKET)
			{
				m_timeBuffer = 0.0;
				SetEventForCommand(COMMAND_RANDOMBOX, &m_itemDBIdx);

				m_iterationCount--;
			}
		}
	}
}

bool COutGameQACommandSet::CheckRandomBoxCommand(const i3::wliteral_range& wImeBufRng)
{
	// QA Command가 아니라 GM command에서 사용으로 변경했습니다. 나중에 QA Command를 별개로 사용할 일이 있으면 치트키 내용 변경해서 다시 풀어줍니다.
	return false;

#if 0
	if( !UserInfoContext::i()->CheckSpecialAbility(SA_CHEAT) ) return false;
	if ( i3::generic_string_size(wImeBufRng) <= 0 || i3::generic_string_size(wImeBufRng) >= MAX_PATH )
		return false;

	wchar_t command[512];
	i3::string_ncopy_nullpad(command, wImeBufRng, 512);
	i3::to_upper(command);

	wchar_t * token = wcstok(command, L" ");
	wchar_t itemID[10] = L"";
	wchar_t iterCount[6] = L"";

	/*
	랜덤박스 커맨드 형식 : /randombox ItemID IterationCounts
	아이템 아이디는 7~9자리이다.
	IterationCounts는 최대 1000번까지 가능하게.
	*/

	if(i3::contain_string_head(command, L"/RANDOMBOX"))
	{
		token = wcstok(nullptr, L" ");
		if ( token == nullptr )
		{
			return false;
		}
		i3::generic_string_ncopy(itemID, token, 9);

		token = wcstok(nullptr, L" ");
		if ( token == nullptr )
		{
			return false;
		}
		i3::generic_string_ncopy(iterCount, token, 5);

		m_itemID = _wtol(itemID);
		I3ASSERT( m_itemID > 1800001 && m_itemID < 180099900);

		if(m_itemID <= 1800001 || m_itemID > 180099900)
			return false;
		
		UINT32 itemCount = 0;
		UINT32 listItemCount = CInvenList::i()->GetItemCount(INVEN::DB_CASHITEM, 0);
		for( UINT32 i = 0; i < listItemCount; ++i)
		{//
			const INVEN_BUFFER* info = CInvenList::i()->GetItem( INVEN::DB_CASHITEM, i, 0);
			if( info->_ui32ItemID== m_itemID)
			{
				itemCount = info->_ui32ItemArg;
			}
		}

		if(itemCount > 0)
		{
			m_iterationCount = (UINT32)i3::atoi(iterCount);
			I3ASSERT( m_iterationCount > 0 && m_iterationCount < 10000);

			//소지 아이템 갯수가 반복 횟수보다 적다면, 반복 횟수를 총 갯수로 정해준다.
			if(itemCount < m_iterationCount)
			{
				m_iterationCount = itemCount;
			}

			m_itemDBIdx = CInvenList::i()->FindWareDBIdx_ByItemID(m_itemID);
			I3ASSERT( m_itemDBIdx >= 0); 

			if(m_itemDBIdx < 0)
				return false;

			m_bIsRandomBoxTestEnabled = true;
		}
		else
		{
			g_pFramework->getChatBox()->PutSystemChatting(L"{col:255,120,0,255}There is no item in Inventory.{/col}");
			return false;
		}
	} 
	else
	{
		return false;
	}
	return true;
#endif
}

void COutGameQACommandSet::AddResultToBuffer(UINT32 itemID, const i3::rc_wstring& itemName)
{
	if(itemID != 0 && !itemName.empty() )
	{
		ItemInfo info;
		info.m_itemID = itemID;
		info.m_itemName = itemName;

		resultBuffer.push_back(info);
	}
}

void COutGameQACommandSet::SetCSVLog()
{
	char filename[MAX_PATH] = "";
	char nation[MAX_PATH] = "";
	char header[30] = "count,ItemID,ItemName\n"; 

	const char* gameName = g_pInterface->GetTitle();
	i3::generic_string_copy(nation, GlobalFunc::GetLocaleFolderName(GetLanguageType()));

	SYSTEMTIME		SystemTime;
	//Cal Local Time
	::GetLocalTime( &SystemTime );	
	INT32 Year		=	SystemTime.wYear;
	INT32 Month		=	SystemTime.wMonth; 
	INT32 Day		=	SystemTime.wDay; 	
	INT32 Hour		=	SystemTime.wHour; 
	INT32 Minute	=	SystemTime.wMinute; 
	INT32 Seconds   =   SystemTime.wSecond;

	char buffer[MAX_PATH];
	char driveLetter[3];
	::GetModuleFileName( NULL, buffer, MAX_PATH );
	_splitpath(buffer, driveLetter, nullptr, nullptr, nullptr);
	
	sprintf(filename, "%s/%s.%s.%d-%d-%d-%d.%d.%d.csv", driveLetter, gameName, nation, Year, Month, Day, Hour, Minute, Seconds);

	FILE * fp = fopen(filename,"a+");

	if(fp != nullptr)
	{
		fwrite(header, sizeof(char), strlen(header), fp); // header
		
		UINT32 num = 0;
		while(num < resultBuffer.size())
		{
			//char temp[MAX_PATH];
			i3::string strTemp;
			i3::sprintf(strTemp, "%d,%d,\"%s\"\n", num, resultBuffer[num].m_itemID, resultBuffer[num].m_itemName);
			fwrite(&strTemp[0], sizeof(char), strTemp.size(), fp);
			num++;
		}

		resultBuffer.clear();

		fclose(fp);
	}
	
	g_pFramework->GetUIMainframe()->InputDisableMainUI(false);
	g_pFramework->getChatBox()->PutSystemChatting(L"{col:255,120,0,255}RandomBox Test Finished!{/col}");
}


