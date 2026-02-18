#include "pch.h"
#include "UIBattlePopupUserReport.h"

#include "UIBattleFrame.h"
#include "UIUtil.h"
#include "BattleHUD_Define.h"
#include "MinimapUtil.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/ext/format_string.h"

#include "../StageBattle/UserContext.h"
#include "../StageBattle/NetworkContext.h"
#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameLoginContext.h"
#include "GlobalInfoDef.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#include "ClientStageInfoUtil.h"

I3_CLASS_INSTANCE( i3ThreadReport );

i3ThreadReport::i3ThreadReport()
{
	::InitializeCriticalSection( &m_cs );

	m_ftp = i3NetworkFTP::new_object(); 

	m_bSetFTP = false;
}

i3ThreadReport::~i3ThreadReport()
{
	::DeleteCriticalSection( &m_cs );

	I3_SAFE_RELEASE( m_ftp );
}


void	i3ThreadReport::SetFTPServer( const std::string& ip, int nport )
{
	// 한번만 설정 한다.
	if( false == m_bSetFTP ) 
	{
		m_ip = ip;
		m_port = nport;
		m_bSetFTP = true;
	}
}


void	i3ThreadReport::SetUploadFile( const std::string& _dir, const std::string& _local, const std::string& _remote )
{	
	if( THREAD_STOPPED == GetState() ) 
	{
		m_cur.dir		= _dir;
		m_cur.local		= _local;
		m_cur.remote	= _remote;

		Terminate();
		Create( "i3ThreadReport" );
	}
	else
	{
		_Upload upload;

		upload.dir		= _dir;
		upload.local	= _local;
		upload.remote	= _remote;

		{
			_AutoCs _auto( &m_cs );
			m_que.push_back( upload );
		}
	}
}

UINT32	i3ThreadReport::OnRunning( void* )
{
	if( nullptr == m_ftp )
	{
		return 1;
	}

	bool bRes = m_ftp->Create( (char*)m_ip.c_str()
		, m_port
		, nullptr, nullptr
		, I3_FTP_FLAG_CHECKREMOTEDIR | I3_FTP_FLAG_CONNECTMAINTAIN );

	if( false == bRes )
	{
		return 0;
	}

	std::string szRemote, szLocal;

	while( true )
	{
		Sleep( 1 );

		// up load txt
		{
			szRemote	= m_cur.dir + m_cur.remote + REPORT_FILE_EXTENTION;
			szLocal		= m_cur.local + REPORT_FILE_EXTENTION;

			m_ftp->UploadFile( szLocal.c_str(), szRemote.c_str() );
			_DeleteFile( szLocal );
		}

		// up load image
		{
			szRemote	= m_cur.dir + m_cur.remote + REPORT_IMAGE_EXTENTION;
			szLocal		= m_cur.local + REPORT_IMAGE_EXTENTION;

			m_ftp->UploadFile( szLocal.c_str(), szRemote.c_str() );
			_DeleteFile( szLocal );
		}

		// set next
		{
			_AutoCs _auto( &m_cs );

			if( false == m_que.empty() )
			{
				_Upload upload = (*m_que.begin());

				m_cur.dir		= upload.dir;
				m_cur.remote	= upload.remote;
				m_cur.local		= upload.local;

				m_que.pop_front();
				continue;
			}
		}

		break;
	} 

	m_ftp->DisConnect();
	return 0;
}

void	i3ThreadReport::_DeleteFile( const std::string& file )
{
	if( false == FileUtil::is_exist_file( file.c_str() ) )
	{
		return;
	}

	int res = remove( file.c_str() );
	I3ASSERT(res != -1);
}



extern "C"
{
	int UIBattlePopupReport_IngameReport(LuaState* L)
	{
#ifdef USE_REPORT
		UIBattlePopupUserReport* popup = static_cast< UIBattlePopupUserReport* >( GameUI::GetBattlePopup( UBP_USER_REPORT ) );
		if( popup != nullptr)
			popup->IngameReportReq();
#endif
		return 0;
	}

};

LuaFuncReg UIBattlePopupReport_Glue[] = 
{
	{ "IngameReport", UIBattlePopupReport_IngameReport },
	{ nullptr, nullptr }
};



I3_CLASS_INSTANCE( UIBattlePopupUserReport );

UIBattlePopupUserReport::UIBattlePopupUserReport() : m_pTex( nullptr )
{
	m_UserList.reserve( (MAX_TEAM_USER * TEAM_COUNT) -1 );
	m_pTrheadReport = i3ThreadReport::new_object();	

	m_bReported = false;
}

UIBattlePopupUserReport::~UIBattlePopupUserReport()
{
	I3_SAFE_RELEASE( m_pTrheadReport );
	I3_SAFE_RELEASE( m_pTex );
}

void	UIBattlePopupUserReport::IngameReportReq()
{
	if( m_bReported )
	{
		DWORD dwSpend = timeGetTime() - m_dwPrevReportTime;

		if( dwSpend < 60000) // 1분 내에 신고 못하게 막음
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING( "STBL_IDX_REPORT_COOLTIME" ) );
			OnKey_Escape();
			return;
		}
	}

	m_bReported = true;
	m_dwPrevReportTime = timeGetTime();

#ifdef USE_REPORT
	GameEventSender::i()->SetEventOnce( EVENT_INGAME_REPORT );
#endif
}


void	UIBattlePopupUserReport::OnCreate( i3GameNode * pParent )
{
	UIBattlePopupBase::OnCreate( pParent );

	AddScene( "Scene/Popup/PBRe_PopUp_Report.i3UIs", UIBattlePopupReport_Glue );
}

bool	UIBattlePopupUserReport::OnEntranceStart( void * pArg1, void * pArg2 )
{
	// 임시 로컬 파일명
	{
		wchar_t szCurDir[ MAX_PATH ] = L"";
		::SHGetSpecialFolderPathW(NULL, szCurDir, CSIDL_PERSONAL, FALSE);

		//document 위치
		std::wstring doc_path = szCurDir;

		//document/PointBlank 위치
		std::wstring doc_pb_path = doc_path + L"\\" + GAME_TITLE_NAME_W;
		FileUtil::create_directory( doc_pb_path );

		i3::string szDate = FileUtil::string_by_date( "_" );
		i3::string szTime	= FileUtil::string_by_time( "_" );

		m_szTemp = _bstr_t( doc_pb_path.c_str() );

		m_szTemp += '\\';
		m_szTemp += i3::format_string( "report_%s_%s", szDate, szTime );	//	BOOST_FORMAT2( "report_%s_%s", szDate, szTime );

		m_szTempTXT = m_szTemp + REPORT_FILE_EXTENTION;
		m_szTempIMG = m_szTemp + REPORT_IMAGE_EXTENTION;
	}

	// 현재 화면 캡쳐
	{
		g_pFramework->SaveImageCapture( m_szTempIMG.c_str(), 40, BCAP_MODE_D3D9_SCALE );
		::SetFileAttributes( m_szTempIMG.c_str(), FILE_ATTRIBUTE_HIDDEN );

		LPDIRECT3DTEXTURE9 pTexDx9 = nullptr;
		HRESULT hr = S_OK;

		hr = ::D3DXCreateTextureFromFile( (LPDIRECT3DDEVICE9)g_pViewer->GetRenderContext()->GetNativeDevice()
			, m_szTempIMG.c_str()
			, &pTexDx9 );

		if( S_OK == hr )
		{
			m_pTex = i3TextureDX::new_object();
			m_pTex->SetDXTexture( pTexDx9 );
		}	
		else
		{
			I3PRINTLOG(I3LOG_NOTICE, "D3DXCreateTextureFromFile fail (0x%X)\n- %s -", hr, m_szTempIMG );
		}
	}

	for (size_t i=0; i<m_UserList.size(); i++)
	{
		I3_SAFE_DELETE( m_UserList[i]);
	}
	m_UserList.clear();

	if( false == UIBattlePopupBase::OnEntranceStart( pArg1, pArg2 ) )
	{
		return false;
	}

	_GatherUserInfo();	// 유저 정보 수집
	_SetUserInfo();		// 유저 정보 설정

	return false;
}


bool	UIBattlePopupUserReport::OnExitStart()
{	
	return UIBattlePopupBase::OnExitStart();
}


void	UIBattlePopupUserReport::_GatherUserInfo()
{	
	//for( int nTeam = 0; nTeam < TEAM_COUNT; ++nTeam )
	//{
	//	const TeamInfo& teamInfo = m_pBattleInfo->GetTeamInfo( (TEAM_TYPE) nTeam );

	//	for( int i = 0; i < MAX_TEAM_USER; ++i )
	//	{
	//		const UserInfo& userInfo = teamInfo.GetMember( i );

	//		if( true == userInfo.IsMyCharacter() ) // 자신은 신고 리스트 에서 제외 한다.
	//		{				
	//			continue;
	//		}

	//		if( false == userInfo.IsPlaying() || false == userInfo.IsInBattleField() )
	//		{
	//			continue;
	//		}

	//		m_UserList.push_back( userInfo );
	//	}
	//}

	INT32 slotIdx[TEAM_COUNT][MAX_TEAM_USER] = 
	{
		{0, 2, 4, 6, 8, 10, 12, 14}, // red
		{1, 3, 5, 7, 9, 11, 13, 15}  // blue
	};

	for (INT32 t=0; t<TEAM_COUNT; t++)
	{
		for (INT32 u=0; u<MAX_TEAM_USER; u++)
		{
			INT32 idx = slotIdx[t][u];

			if (BattleSlotContext::i()->getMySlotIdx() == idx) continue;
			if (UserContext::i()->IsPlayingSlot(idx)) continue;
			if (tu::IsInBattleField(idx)) continue;
			
			s_UserInfo * pInfo = new s_UserInfo(idx);
			MEMDUMP_NEW( pInfo, sizeof( s_UserInfo));
			m_UserList.push_back( pInfo);
		}
	}

	struct fn
	{
		bool operator()(const s_UserInfo* lhs, const s_UserInfo* rhs) const
		{
			return BattleSlotContext::i()->getNickForSlot(lhs->slotIdx) < BattleSlotContext::i()->getNickForSlot(rhs->slotIdx);
		}
	};
	std::sort( m_UserList.begin(), m_UserList.end(), fn() ) ;
}

bool	UIBattlePopupUserReport::OnKey_Enter()
{
	IngameReportReq();
	return true;
}

bool	UIBattlePopupUserReport::OnKey_Escape()
{
	_DeleteReportFile( m_szTempTXT );
	_DeleteReportFile( m_szTempIMG );

#ifdef USE_REPORT
	UIBattleFrame::i()->ClosePopup( UBP_USER_REPORT );
#endif
	return true;
}

void	UIBattlePopupUserReport::_InitializeAtLoad( i3UIScene* pScene )
{
	UIBattlePopupBase::_InitializeAtLoad( pScene );

	i3UIImageBoxEx* pImageBox = (i3UIImageBoxEx*) pScene->FindChildByName( "i3UIImageBoxEx");
	I3ASSERT( pImageBox != nullptr);

	if( nullptr != pImageBox && nullptr != m_pTex )
	{
		const REAL32 realW = pImageBox->getWidth();
		const REAL32 realH = pImageBox->getHeight();

		pImageBox->DeleteShapeAll();
		pImageBox->AddTexture( m_pTex );		
		pImageBox->CreateBaseSceneObjects();
		pImageBox->SetCurrentImage( 0 );
		pImageBox->SetShapeSize( realW, realH );
	}
}

void	UIBattlePopupUserReport::_ClearAtUnload( i3UIScene * pScene )
{
	I3_SAFE_RELEASE( m_pTex );
	UIBattlePopupBase::_ClearAtUnload( pScene );
}

void	UIBattlePopupUserReport::_CalculateRemoteFilePath()
{
	USER_INFO_BASIC userInfo;
	UserInfoContext::i()->GetMyUserInfoBasic( &userInfo );

	UINT32 serverTime = GameLoginContext::i()->GetReportServerTime();

	int nYear	= (2000 + (serverTime / 100000000));
	int nMon	= ((serverTime % 100000000) / 1000000);
	int nDay	= ((serverTime % 1000000) / 10000);
	int nHour	= ((serverTime % 10000) / 100);
	int nMin	= (serverTime % 100);

	m_szDate = i3::format_string( "%d_%d_%d", nYear, nMon, nDay );
	m_szTime = i3::format_string( "%d_%d", nHour, nMin );

	m_szDir		= m_szDate + "\\";

	i3::string strNick;		i3::utf16_to_mb(userInfo.m_strNick, strNick);
	i3::sprintf(m_szRemote, "report_%s_%s_%s", m_szDate, m_szTime, strNick);

//	m_szRemote	= BOOST_FORMAT3( "report_%s_%s_%s", m_szDate, m_szTime, userInfo._nick );

}

void	UIBattlePopupUserReport::_SetUserInfo()
{
	LuaState* L = _CallLuaFunction( "SetUserList" );

	if( nullptr == L )
	{
		return;
	}

	for each( const s_UserInfo* info in m_UserList )
	{
		i3Lua::PushStringUTF16To8( L, BattleSlotContext::i()->getNickForSlot(info->slotIdx) );
	}

	_EndLuaFunction( L, m_UserList.size() );
}

void	UIBattlePopupUserReport::_DeleteReportFile( const i3::string& path )
{
	if( true == path.empty() )
	{
		return;
	}

	unsigned int uRes = 0;

	if( true == FileUtil::is_exist_file( path.c_str() ) )
	{
		if( 0 != (uRes = remove( path.c_str() )) )
		{
			//I3PRINTLOG(I3LOG_NOTICE, "%s file remove fail (0x%X)", path.c_str(), uRes );
		}
	}
}

void	UIBattlePopupUserReport::ProcessGameEvent( INT32 _event, INT32 _arg , const i3::user_data& UserData)
{
#ifdef USE_REPORT
	switch( _event )
	{
	case	EVENT_INGAME_REPORT:
		{
			if( EV_SUCCESSED( _arg ) )
			{
				_CalculateRemoteFilePath();
				_SaveReportTXT();

				// file send to ftp
				if( m_pTrheadReport )
				{
					m_pTrheadReport->SetFTPServer( GameLoginContext::i()->GetReportIP(), GameLoginContext::i()->GetReportPort() );
					m_pTrheadReport->SetUploadFile( m_szDir, m_szTemp, m_szRemote );
				}

				CGameChatBox* pChatBox = g_pFramework->getChatBox();

				if( pChatBox )	// 파일 전송 성공 여부와 상관없이 성공 메시지를 채팅창에 띄움
					pChatBox->PutSystemChatting( GAME_RCSTRING("STR_TBL_REPORT_SUCCESS") );
			}
			else
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING( "STBL_IDX_REPORT_MAXCOUNT" ) );

				_DeleteReportFile( m_szTempTXT );
				_DeleteReportFile( m_szTempIMG );
			}

			UIBattleFrame::i()->ClosePopup( UBP_USER_REPORT );
		}
		break;
	}
#endif
}

void	UIBattlePopupUserReport::_SaveReportTXT()
{
	if( true == m_UserList.empty() )
	{
		return;
	}

	USER_INFO_BASIC userInfo;
	ROOM_INFO_BASIC roomInfo;

	i3::string szStage;	
	int nType, nSelUser;

	MyRoomInfoContext::i()->getMyRoomInfo( &roomInfo );
	UserInfoContext::i()->GetMyUserInfoBasic( &userInfo );

	LuaState* L = nullptr;

	{
		L = _CallLuaFunction( "GetSelectedReportType" );
		nType = _ReturnLuaInteger( L, 0 );
	}	

	{
		L = _CallLuaFunction( "GetSelectedUser" );
		nSelUser = _ReturnLuaInteger( L, 0 );
	}

	i3::rc_wstring stage_name = CStageInfoUtil::GetMapData(roomInfo._StageID)->getDisplayName();
	i3::utf16_to_mb(stage_name, szStage);

	if( 0 <= nSelUser && nSelUser < (int)m_UserList.size() )
	{
		FILE* fp;
		::fopen_s( &fp, m_szTempTXT.c_str(), "wt" );

		if( nullptr != fp )
		{
			int nServer		= g_pServerInfo->GetCurGameServerIndex() +1;
			int nChannel	= g_pServerInfo->GetCurChannelNumber() +1;
			int nRoom		= roomInfo._Idx + 1;

			::fprintf_s( fp, "%d\r\n", nType +1 );
			::fprintf_s( fp, "%s %s\r\n", m_szDate.c_str(), m_szTime.c_str() );

			char szNick[ MAX_STRING_COUNT ];
			i3::utf16_to_mb(userInfo.m_strNick, szNick, MAX_STRING_COUNT);

			::fprintf_s( fp, "%I64d %s\r\n", NetworkContext::i()->UID, szNick);

			i3::rc_wstring wstrNick = BattleSlotContext::i()->getNickForSlot(m_UserList[nSelUser]->slotIdx);
			i3::utf16_to_mb(wstrNick, szNick, MAX_STRING_COUNT);

			::fprintf_s( fp, "%s\r\n", szNick );
			::fprintf_s( fp, "%d - %d - %d\r\n", nServer, nChannel, nRoom );
			::fprintf_s( fp, "%s\r\n", szStage.c_str() );

			fclose( fp );
		}

		::SetFileAttributes( m_szTempTXT.c_str(), FILE_ATTRIBUTE_HIDDEN );
	}
}
