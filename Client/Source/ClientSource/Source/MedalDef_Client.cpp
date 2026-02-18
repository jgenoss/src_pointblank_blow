#include "pch.h"
#include "MedalDef_Client.h"
#include "SteamContext.h"


void MEDAL_INFO::setBattleCount(UINT16 num, bool inBattle)
{
	m_ui16BattleCount = num;
	if((m_ui16BattleCount == m_ui16Count[1] ||
		m_ui16BattleCount == m_ui16Count[2] ||
		m_ui16BattleCount == m_ui16Count[3]) && inBattle )
	{
		setNewMark(true);
		setNewMedal(true);
	}

	if (m_ui16BattleCount >= m_ui16Count[1])
	{
		if (SteamContext::i()->IsSteamUser())
		{
			char achKey[MAX_STRING_MEDAL_NAME_KEY_COUNT] = { 0, };
			i3::generic_string_copy(achKey, m_szNameKey);
			if (m_ui16BattleCount >= m_ui16Count[2])
			{
				if (m_ui16BattleCount >= m_ui16Count[3])
				{
					if (m_ui16BattleCount >= m_ui16Count[3])
					{
						i3::generic_string_cat(achKey, "_4");
					}
					else
						i3::generic_string_cat(achKey, "_3");
				}
				else
					i3::generic_string_cat(achKey, "_2");
			}
			else
				i3::generic_string_cat(achKey, "_1");

			SteamContext::i()->SetAchievement(achKey);
		}
		setAcquired(true);
	}
}

void MEDAL_INFO::setTotalMedalReward(UINT8 reward)
{
	if(reward & 0x01)
		setMedalReward(MEDAL_LEVEL_1, true);
	if(reward & 0x02)
		setMedalReward(MEDAL_LEVEL_2, true);
	if(reward & 0x04)
		setMedalReward(MEDAL_LEVEL_3, true);
	if(reward & 0x08)
		setMedalReward(MEDAL_LEVEL_4, true);
}

void MEDAL_INFO::setMedalReward(UINT8 lv, bool bAcq)
{
	m_bRewardAcq[lv-1] = bAcq;
}

void medal_util::browse_directory(const char* path, pf_direcory pf_d, pf_file pf_f)
{
	i3::string tmp_path(path);
	tmp_path.append( "\\*.*" ); //모든 파일

	WIN32_FIND_DATA wfd;
	HANDLE hSrch = ::FindFirstFile(tmp_path.c_str(), &wfd);
	if (hSrch == INVALID_HANDLE_VALUE) 
	{
		return;
	}

	do 
	{
		//자신/상위 폴더명은 제외
		if ( !((_tcscmp(wfd.cFileName, _T(".") ) == 0) ||
			_tcscmp(wfd.cFileName, _T("..") ) == 0) )
		{
			if( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) //디렉토리
			{
				tmp_path.assign( path );
				tmp_path.append( "\\" );
				tmp_path.append( wfd.cFileName );

				bool browse_dir = pf_d( tmp_path );
				if( browse_dir == true )
					browse_directory(tmp_path.c_str(), pf_d, pf_f);
			}
			else //파일
			{
				pf_f( path, wfd.cFileName );
			}
		}
	} while( ::FindNextFile( hSrch, &wfd) );

	::FindClose(hSrch);
}