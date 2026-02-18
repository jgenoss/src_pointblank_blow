#include "pch.h"
#include "GachaContext.h"
#include "ShopContext.h"
#include "UI/UIMainFrame.h"
#include "UI/UIShopFunction.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#define		GACHA_SHOP_FILE_NAME_ASCII			".\\GachaShop.dat"

GachaContext::GachaContext()
{
	if( true == OpenGachaShopFile() )
	{
		//LoadGachaShopFile();
		I3PRINTLOG(I3LOG_NOTICE, "Success OGSF " );
	}
	else
		I3PRINTLOG(I3LOG_NOTICE, "Fail OGSF " );  
}

GachaContext::~GachaContext()
{
	if( m_hGachaShopFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hGachaShopFile );
	}
}

bool GachaContext::PopGachaReward(GACHA_REAWARD& out) 
{ 
	if(m_gachaReward.empty())
	{
		out.m_rewardGoodsID = 0;
		out.m_result = GACHA_RESULT_NONE;
		return false;
	}
	out = m_gachaReward.front(); 
	m_gachaReward.pop(); 
	return true;
}

void GachaContext::ClearGachaRst()
{
	m_bisMyBuy = false;
	memset(&m_gachaRstHeader, 0, sizeof(m_gachaRstHeader));
	m_gachaRstData.clear();
	
	ClearGachaReward();
}

void GachaContext::SetJackpotAnnounceChat( )
{
	UINT32 itemID = 0;
	if(MAX_STRING_COUNT > i3::generic_string_size( m_wstrGachaLuckyUser))
	{
		itemID =  CShopContext::i()->ConvertGoodsID(m_GachaLuckyGoodsId);
		
		if(itemID == 0)
		{
#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
			{
				I3ASSERT(0 && "ERR! - failed to find item id for goodsid");
				I3TRACE("ERR! - failed to find item id for goodsid[%d]", m_GachaLuckyGoodsId);
			}
#endif
			return;
		}

		i3::rc_wstring wstr_ShopItemName;
		i3::rc_wstring wstr_result_text;

		usf::GetShopItemName(wstr_ShopItemName, itemID);

		i3::sprintf(wstr_result_text, GAME_RCSTRING("STR_SHOP_JACKPOT_ANNOUNCE2"),
			m_wstrGachaLuckyUser, wstr_ShopItemName );

		m_RecvLuckyItemText.push_back(wstr_result_text);
		m_RecvLuckyItemId.push_back(itemID);
	}

	if(!m_bisMyBuy)
	{
		ShowJackpotAnnounceChat(itemID);
	}
}

void GachaContext::ShowJackpotAnnounceChat( UINT32 itemID )
{
	if(itemID == 0)
		return;

	if(0 < m_RecvLuckyItemText.size())
	{
		i3::vector<i3::rc_wstring>::iterator Itor = m_RecvLuckyItemText.begin();
		i3::vector<UINT32> ::iterator Itor2 = m_RecvLuckyItemId.begin();

		while(Itor != m_RecvLuckyItemText.end())
		{
			if((*Itor2) == itemID)
			{
				g_pFramework->PutSystemChatting(*Itor);
				m_RecvLuckyItemText.erase(Itor);
				m_RecvLuckyItemId.erase(Itor2);
				break;
			}
			else
			{
				++Itor;
				++Itor2;
			}
		}
	}
}

bool GachaContext::OpenGachaShopFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hGachaShopFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hGachaShopFile );
		m_hGachaShopFile = INVALID_HANDLE_VALUE;
	}
	
	i3::safe_string_copy( wstrFileName, GACHA_SHOP_FILE_NAME_ASCII, MAX_PATH);
	
	if(FindGachaShopFile() == true)
	{
		m_hGachaShopFile = ::CreateFile(wstrFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

		if( INVALID_HANDLE_VALUE == m_hGachaShopFile )
		{			
			::CloseHandle( m_hGachaShopFile );
			m_hGachaShopFile = INVALID_HANDLE_VALUE; 
			return false; 
		}
	}
	else	return false;

	return true;
}

void		GachaContext::LoadGachaShopFile(void)
{
	INT32	i32ReadLength = 0;

	// Gacha Group Info
	for( INT32 k = 0; k < GACHA_GROUP_COUNT; ++k )
	{
		GACHA_GROUP_INFO stGroup;
		GACHA_GROUP	eGroup;
		::ReadFile(m_hGachaShopFile, &eGroup,					sizeof(GACHA_GROUP),	(LPDWORD)&i32ReadLength, NULL);
		if( eGroup > GACHA_GROUP_NONE && eGroup < GACHA_GROUP_COUNT )
		{
			stGroup.m_eGachaGroup = eGroup;

			//::ReadFile(m_hGachaShopFile, &stGroup.m_i32GloryCount,						sizeof(INT32),	(LPDWORD)&i32ReadLength, NULL); 
			::ReadFile(m_hGachaShopFile, &stGroup.m_ui32LuckyGoodsID,					sizeof(UINT32),	(LPDWORD)&i32ReadLength, NULL);

			::ReadFile(m_hGachaShopFile, &stGroup.m_i32WinItemCount,					sizeof(INT32),	(LPDWORD)&i32ReadLength, NULL);
			stGroup.m_i32WinItemCount = MIN( stGroup.m_i32WinItemCount, WIN_ITEM_COUNT_BY_TAB );
			::ReadFile(m_hGachaShopFile, stGroup.m_arWinItems,							sizeof(GACHA_ITEM_INFO)*stGroup.m_i32WinItemCount,	(LPDWORD)&i32ReadLength, NULL);

			::ReadFile(m_hGachaShopFile, &stGroup.m_i32RandomItemCnt,					sizeof(INT32),	(LPDWORD)&i32ReadLength, NULL);
			stGroup.m_i32RandomItemCnt = MIN( stGroup.m_i32RandomItemCnt, RANDOM_ITEM_COUNT_BY_TAB );
			::ReadFile(m_hGachaShopFile, stGroup.m_arRandomGoodsIDs,					sizeof(UINT32)*stGroup.m_i32RandomItemCnt,	(LPDWORD)&i32ReadLength, NULL);

			SetGachaData( eGroup, &stGroup);
			GameEventReceiver::i()->PushReceivedGameEvent( EVENT_ENTER_GACHA_INFO, (INT32)eGroup );
		}
	}
	 
	// MD5Key 생성
	MakeGachaShopMD5Key();

 
	if( INVALID_HANDLE_VALUE == m_hGachaShopFile )
	{			
		::CloseHandle( m_hGachaShopFile );
		m_hGachaShopFile = INVALID_HANDLE_VALUE; 
	}

	return;
}


bool GachaContext::FindGachaShopFile()
{
	WIN32_FIND_DATA		ffResult;
	HANDLE              hFind;

	bool				bRv			= false;
 
	char strFileName[MAX_PATH];
	
	i3::safe_string_copy(strFileName, GACHA_SHOP_FILE_NAME_ASCII, MAX_PATH);

	hFind = FindFirstFile( (LPCSTR)strFileName, &ffResult);
    
	if ( hFind !=INVALID_HANDLE_VALUE )
	{
		BOOL                bContinue	= true;

		while( bContinue)
		{    
			if( (ffResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				bRv = true;
			}
	              
			bContinue = ::FindNextFile( hFind, &ffResult);
		}

		::FindClose(hFind);
	}

	return bRv;
}



void GachaContext::MakeGachaShopMD5Key(void)
{
	// ShopFile로 MD5Key를 생성합니다.
	unsigned char md5Key[NET_MD5_HASH_SIZE] = {};
	UINT128 ui128Temp;
	
	i3MD5::Getmd5_file(GACHA_SHOP_FILE_NAME_ASCII, md5Key);
	memcpy(&ui128Temp, md5Key, sizeof(char)*NET_MD5_HASH_SIZE);
	i3MD5::GetMD5String(m_strGachaShopMD5Key, ui128Temp, sizeof(m_strGachaShopMD5Key) );

	return;
}


bool GachaContext::SaveGachaShopFile()
{
	// 파일이 있으면 지운다.
	DeleteGachaShopFile();

	INT32			   i32ReadLength = 0;
 
	if( CreateGachaShopFile() == false)	return false;
 
	/*for( INT32 i = 0 ; i < GACHA_NOTICE_COUNT; ++i )
	{
		::WriteFile(m_hGachaShopFile, m_szNotices[i],			sizeof(char)*GACHA_NOTICE_LENGTH ,	(LPDWORD)&i32ReadLength, NULL);
	}*/

	for( INT32 k = 0 ; k < GACHA_GROUP_COUNT; ++k )
	{
		::WriteFile(m_hGachaShopFile, &m_GachaReceived[k].m_eGachaGroup,				sizeof(GACHA_GROUP),	(LPDWORD)&i32ReadLength, NULL);
		if( m_GachaReceived[k].m_eGachaGroup > GACHA_GROUP_NONE && m_GachaReceived[k].m_eGachaGroup < GACHA_GROUP_COUNT )
		{
			//::WriteFile(m_hGachaShopFile, &m_GachaReceived[k].m_i32GloryCount,			sizeof(INT32),	(LPDWORD)&i32ReadLength, NULL);
			::WriteFile(m_hGachaShopFile, &m_GachaReceived[k].m_ui32LuckyGoodsID,		sizeof(UINT32),	(LPDWORD)&i32ReadLength, NULL);

			m_GachaReceived[k].m_i32WinItemCount = MIN( m_GachaReceived[k].m_i32WinItemCount, WIN_ITEM_COUNT_BY_TAB );
			::WriteFile(m_hGachaShopFile, &m_GachaReceived[k].m_i32WinItemCount,		sizeof(INT32),	(LPDWORD)&i32ReadLength, NULL); 
			::WriteFile(m_hGachaShopFile, m_GachaReceived[k].m_arWinItems,				sizeof(GACHA_ITEM_INFO)*m_GachaReceived[k].m_i32WinItemCount,	(LPDWORD)&i32ReadLength, NULL);

			m_GachaReceived[k].m_i32RandomItemCnt = MIN( m_GachaReceived[k].m_i32RandomItemCnt, RANDOM_ITEM_COUNT_BY_TAB );
			::WriteFile(m_hGachaShopFile, &m_GachaReceived[k].m_i32RandomItemCnt,		sizeof(INT32),	(LPDWORD)&i32ReadLength, NULL);
			::WriteFile(m_hGachaShopFile, m_GachaReceived[k].m_arRandomGoodsIDs,		sizeof(UINT32)*m_GachaReceived[k].m_i32RandomItemCnt,	(LPDWORD)&i32ReadLength, NULL);
		}
	}
	
	if( INVALID_HANDLE_VALUE == m_hGachaShopFile )
	{			
		::CloseHandle( m_hGachaShopFile );
		m_hGachaShopFile = INVALID_HANDLE_VALUE; 
	}

	// GachaShop.dat 파일에 대한 MD5Key를 생성한다.
	MakeGachaShopMD5Key();
 
	return true;
}

bool GachaContext::CreateGachaShopFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hGachaShopFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hGachaShopFile );
		m_hGachaShopFile = INVALID_HANDLE_VALUE;
	}
	
	i3::safe_string_copy(wstrFileName, GACHA_SHOP_FILE_NAME_ASCII, MAX_PATH);

	m_hGachaShopFile = ::CreateFile(wstrFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == m_hGachaShopFile )
	{			
		::CloseHandle( m_hGachaShopFile );
		m_hGachaShopFile = INVALID_HANDLE_VALUE; 
		return false; 
	}

	::SetFilePointer(m_hGachaShopFile, 0, NULL, FILE_BEGIN);

	return true;
}


bool GachaContext::DeleteGachaShopFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hGachaShopFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hGachaShopFile );
		m_hGachaShopFile = INVALID_HANDLE_VALUE;
	}

	i3::safe_string_copy(wstrFileName, GACHA_SHOP_FILE_NAME_ASCII, MAX_PATH);

	if( !DeleteFile(wstrFileName) ) return false;

	return true;
}


const GACHA_ITEM_INFO* GachaContext::GetGachaItemInfo(GACHA_GROUP grp, INT32 itemKey) const
{
	if(grp > GACHA_GROUP_NONE && grp < GACHA_GROUP_COUNT)
	{
		const GACHA_GROUP_INFO& ary = m_GachaReceived[grp];

		for(INT32 i=0; i<ary.m_i32WinItemCount; i++)
		{
			if(ary.m_arWinItems[i].m_i32ItemKey == itemKey)
				return &ary.m_arWinItems[i];
		}
	}

	return nullptr;
}

const i3::wstring GachaContext::GetGachaLuckyGoodsName() const
{
	i3::rc_wstring name;
	T_ItemID itemID = CShopContext::i()->ConvertGoodsID(m_GachaLuckyGoodsId);
	usf::GetShopItemName(name, itemID);

	return name;
}