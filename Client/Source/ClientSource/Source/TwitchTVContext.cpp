#include "pch.h"
#include <float.h>
#include "TwitchTVContext.h"

#include "ConfigEx.h"

#include "../UI/UIUtil.h"
#include "../UI/UIMainFrame.h"
#include "../UI/UIClanFunction.h"

#include "i3Base/string/ext/utf16_to_utf8.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/smart_ptr/detail/spinlock.h"

#define		TWITCH_FILE_NAME_ASCII			".\\Twitch.dat"

TwitchTVContext::TwitchTVContext() : m_ui8TwitchState(0), m_bWaitingForCallback(false), m_fServerBitrate(0.f),
									 m_nFrameIndex(0), m_nVideoIndex(1), m_nResoultionIndex(3), 
									 m_bMike(true), m_bSpeaker(true), m_nMikeVolume(50), m_nSpeakerVolume(50),
									 m_CaptureTextureWidth(0), m_CaptureTextureHeight(0), m_CaptureGet(0), m_CapturePut(0),
									 m_LastCaptureTime(0)
{
	m_bMsgBox = false;
	m_hTwitchShopFile = 0;
	m_strTwitchMD5Key[NET_MD5_KEY_SIZE] = 0;

	m_LastTaskResult = TTV_EC_SUCCESS;
	
	m_CaptureTexture = nullptr;
	m_CaptureSurface = nullptr;
	for (int i = 0; i < NUM_CAPTURE_SURFACES; ++i)
	{
		m_CaptureQuery[i]  = nullptr;
		m_ResizeSurface[i] = nullptr;
	}

	if( true == OpenTwitchFile() )
	{
		LoadTwitchFile();
	}else
	{
		m_bFastTwitchSreamingStop = true;
	}
}

TwitchTVContext::~TwitchTVContext()
{
	if(isTwitchState(TWITCHTV_STREAMING))
		TTVStreamStop();

	if(isTwitchState(TWITCHTV_GET_INGESTLIST))
		TTV_FreeIngestList(&m_IngestList);

	if(isTwitchState(TWITCHTV_INIT))
		TTV_Shutdown();

	for( i3::vector<TWFRAME_INFO*>::iterator fit = m_FrameList.begin() ; fit != m_FrameList.end() ; fit++ )
	{
		delete (*fit);
	}
	m_FrameList.clear();

	for(i3::vector<TWVIDEO_INFO*>::iterator vit = m_VideoList.begin() ; vit != m_VideoList.end() ; vit++ )
	{
		delete (*vit);
	}
	m_VideoList.clear();

	for(i3::vector<TWRESOULTION_INFO*>::iterator vit = m_ResoultionLsit.begin() ; vit != m_ResoultionLsit.end() ; vit++ )
	{
		delete (*vit);
	}
	m_ResoultionLsit.clear();

	I3_SAFE_RELEASE_NODBG(m_CaptureTexture);
	I3_SAFE_RELEASE_NODBG(m_CaptureSurface);
	for (int i = 0; i < NUM_CAPTURE_SURFACES; ++i)
	{
		I3_SAFE_RELEASE_NODBG(m_CaptureQuery[i]);
		I3_SAFE_RELEASE_NODBG(m_ResizeSurface[i]);
	}
}

void TwitchTVContext::Init()
{
	SetBroadTitle(GAME_STRING("STR_POPUP_TWITCH_BROADCAST_NAME_INPUT"));
	SetMikeEnable(false);
	SetSpeakerEnable(true);
	SetFrame(0);
	SetVideoGrade(0);
	SetMikeVolume(0);
	SetSpeakerVolume(50);
	Init_FrameInfo();
	Init_VideoInfo();
	Init_ResultionInfo();
}

void TwitchTVContext::Init_FrameInfo()
{
	if(m_FrameList.size() != 0)
		return;

	//일단 하드 코딩
	INT32 default_frame[] = { 30, 40, 50, 60 };
	i3::wstring default_frame_string[] =
	{
		GAME_STRING("STR_POPUP_TWITCH_BROADCAST_FRAME_1"),
		GAME_STRING("STR_POPUP_TWITCH_BROADCAST_FRAME_2"),
		GAME_STRING("STR_POPUP_TWITCH_BROADCAST_FRAME_3"),
		GAME_STRING("STR_POPUP_TWITCH_BROADCAST_FRAME_4")
	};
	INT32 size = sizeof(default_frame)/sizeof(INT32);

	m_FrameList.clear();

	for(INT32 i = 0 ; i < size ; i++)
	{
		TWFRAME_INFO * pNew = new TWFRAME_INFO;
		pNew->m_frame = default_frame[i];
		pNew->m_sframe = default_frame_string[i];
		m_FrameList.push_back( pNew );
	}
}

void TwitchTVContext::Init_VideoInfo()
{
	if(m_VideoList.size() != 0)
		return;

	i3::wstring default_video[] = 
	{ 
		GAME_STRING("STR_POPUP_TWITCH_BROADCAST_CPU_LOW"),
		GAME_STRING("STR_POPUP_TWITCH_BROADCAST_CPU_MEDIUM"),
		GAME_STRING("STR_POPUP_TWITCH_BROADCAST_CPU_HIGH")
	};
	INT32 size = sizeof(default_video)/sizeof(i3::wstring);

	m_VideoList.clear();

	for(INT32 i = 0 ; i < size ; i++)
	{
		TWVIDEO_INFO * pNew =new TWVIDEO_INFO;
		pNew->m_video = default_video[i];

		m_VideoList.push_back( pNew );
	}
}

void TwitchTVContext::Init_ResultionInfo()
{
	if(m_ResoultionLsit.size() != 0)
		return;

	//일단 하드 코딩
	INT32 width[]  = { 320, 640, 960, 1280, 1280, 1920 };
	INT32 height[] = { 240, 360, 540, 720,  800,  1200 };
	INT32 bitreate[] = { TTV_MIN_BITRATE, 640, 1500, 2500, 2700, TTV_MAX_BITRATE };
	INT32 size = sizeof(width)/sizeof(INT32);

	m_ResoultionLsit.clear();
	for(INT32 i = 0 ; i < size ; i++)
	{
		TWRESOULTION_INFO * pNew =new TWRESOULTION_INFO;
		pNew->m_width = width[i];
		pNew->m_height = height[i];
		pNew->m_recomendbitrate = bitreate[i];

		m_ResoultionLsit.push_back( pNew );
	}
}

bool TwitchTVContext::FindTwitchFile()
{
	WIN32_FIND_DATA		ffResult;
	HANDLE              hFind;
	bool				bRv			= false;

	char strFileName[MAX_PATH];

	i3::safe_string_copy(strFileName, TWITCH_FILE_NAME_ASCII, MAX_PATH);

	hFind = FindFirstFile( (LPCSTR)strFileName, &ffResult);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		BOOL bContinue = TRUE;

		while( bContinue )
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

bool TwitchTVContext::OpenTwitchFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hTwitchShopFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hTwitchShopFile );
		m_hTwitchShopFile = INVALID_HANDLE_VALUE;
	}

	i3::safe_string_copy( wstrFileName, TWITCH_FILE_NAME_ASCII, MAX_PATH);

	if(FindTwitchFile() == true)
	{
		m_hTwitchShopFile = ::CreateFile(wstrFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

		if( INVALID_HANDLE_VALUE == m_hTwitchShopFile )
		{			
			::CloseHandle( m_hTwitchShopFile );
			m_hTwitchShopFile = INVALID_HANDLE_VALUE; 
			return false; 
		}
	}
	else	return false;

	return true;
}

void TwitchTVContext::LoadTwitchFile(void)
{
	INT32 i32ReadLength = 0;

	::ReadFile(m_hTwitchShopFile, &m_bFastTwitchSreamingStop, sizeof(BOOL),	(LPDWORD)&i32ReadLength, NULL);
	
	// MD5Key 생성
	MakeTwitchMD5Key();
	if( INVALID_HANDLE_VALUE == m_hTwitchShopFile )
	{			
		::CloseHandle( m_hTwitchShopFile );
		m_hTwitchShopFile = INVALID_HANDLE_VALUE; 
	}

	return;
}

bool TwitchTVContext::CreateTwitchFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hTwitchShopFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hTwitchShopFile );
		m_hTwitchShopFile = INVALID_HANDLE_VALUE;
	}

	i3::safe_string_copy(wstrFileName, TWITCH_FILE_NAME_ASCII, MAX_PATH);

	m_hTwitchShopFile = ::CreateFile(wstrFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == m_hTwitchShopFile )
	{			
		::CloseHandle( m_hTwitchShopFile );
		m_hTwitchShopFile = INVALID_HANDLE_VALUE; 
		return false; 
	}

	::SetFilePointer(m_hTwitchShopFile, 0, NULL, FILE_BEGIN);

	return true;
}

bool TwitchTVContext::SaveTwitchFile()
{
	DeleteTwitchFile();

	INT32			   i32ReadLength = 0;
	if( CreateTwitchFile() == false)	return false;


	::WriteFile(m_hTwitchShopFile, &m_bFastTwitchSreamingStop,	sizeof(BOOL), (LPDWORD)&i32ReadLength, NULL);
		
	if( INVALID_HANDLE_VALUE == m_hTwitchShopFile )
	{			
		::CloseHandle( m_hTwitchShopFile );
		m_hTwitchShopFile = INVALID_HANDLE_VALUE; 
	}

	MakeTwitchMD5Key();
 
	return true;
}

bool TwitchTVContext::DeleteTwitchFile()
{
	char	wstrFileName[MAX_PATH];

	if( m_hTwitchShopFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hTwitchShopFile );
		m_hTwitchShopFile = INVALID_HANDLE_VALUE;
	}

	i3::safe_string_copy(wstrFileName, TWITCH_FILE_NAME_ASCII, MAX_PATH);

	if( !DeleteFile(wstrFileName) ) return false;

	return true;
}

void TwitchTVContext::MakeTwitchMD5Key(void)
{
	unsigned char md5Key[NET_MD5_HASH_SIZE] = {};
	UINT128 ui128Temp;

	i3MD5::Getmd5_file(TWITCH_FILE_NAME_ASCII, md5Key);
	memcpy(&ui128Temp, md5Key, sizeof(char)*NET_MD5_HASH_SIZE);
	i3MD5::GetMD5String(m_strTwitchMD5Key, ui128Temp, sizeof(m_strTwitchMD5Key));

	return;
}

i3::wstring TwitchTVContext::GetFrameInfoString()
{
	ucf::ComboString cs;

	INT32 Count = m_FrameList.size();
	i3::wstring conv;

	for( INT32 i = 0; i < Count; i++)
	{	
		TWFRAME_INFO * pInfo = m_FrameList[i];
		cs<<pInfo->m_sframe;
	}

	return i3::wstring(cs);
}

INT32 TwitchTVContext::GetFrameInfoToIndex(INT32 index)
{
	INT32 size = m_FrameList.size();

	if( !(index == 0 && index >= size) )
		return m_FrameList[index]->m_frame;

	return -1;
}

i3::wstring TwitchTVContext::GetVideoInfoString()
{
	ucf::ComboString cs;

	INT32 Count = m_VideoList.size();

	for( INT32 i = 0; i < Count; i++)
	{	
		TWVIDEO_INFO * pInfo = m_VideoList[i];
		cs<<pInfo->m_video;
	}

	return i3::wstring(cs);
}

i3::wstring TwitchTVContext::GetVideoInfoToIndex(INT32 index)
{
	INT32 size = m_VideoList.size();

	if( !(index == 0 && index >= size) )
		return m_VideoList[index]->m_video;

	return L"NULL";
}

i3::wstring TwitchTVContext::GetResoultionInfoString()
{
	ucf::ComboString cs;

	INT32 Count = m_ResoultionLsit.size();

	i3::wstring temp;
	for( INT32 i = 0; i < Count; i++)
	{	
		TWRESOULTION_INFO * pInfo = m_ResoultionLsit[i];
		i3::sprintf( temp, L"%d x %d", pInfo->m_width, pInfo->m_height );
		cs<<temp;
	}

	return i3::wstring(cs);
}

i3::wstring TwitchTVContext::GetResoultionInfoToIndex(INT32 index)
{
	INT32 size = m_ResoultionLsit.size();

	i3::wstring temp;
	if( !(index == 0 && index >= size) )
	{
		TWRESOULTION_INFO * pInfo = m_ResoultionLsit[index];
		i3::sprintf( temp, L"%d x %d", pInfo->m_width, pInfo->m_height );
		return temp;
	}

	return L"NULL";
}
//=============================================================//

namespace
{
	void TestBufferUnlockCallback (const uint8_t* /*buffer*/, void* /*userData*/)
	{
		//테스트용 Callback이라 내용이 없습니다.
	}


	void IngestListCallback(TTV_ErrorCode result, void* /*userData*/)
	{
		if (TTV_SUCCEEDED(result))
		{
			TwitchTVContext::i()->AddTwitchState(TWITCHTV_GET_INGESTLIST);
			I3ASSERT("트위치 티비 서버 정보 받아오기 성공");
		}
		else
		{
			I3ASSERT("트위치 티비 서버 정보 받아오기 실패");
		}
		TwitchTVContext::i()->SetLastTaskResult(result);
		TwitchTVContext::i()->SetWaitForCallback(false);
	}

	void LoginCallback(TTV_ErrorCode result, void* /*userData*/)
	{
		if (TTV_SUCCEEDED(result))
		{
			TwitchTVContext::i()->AddTwitchState(TWITCHTV_LOGIN);
			I3ASSERT("트위치 티비 로그인 성공");
		}
		else
		{
			I3ASSERT("트위치 티비 로그인 실패");
		}
		TwitchTVContext::i()->SetLastTaskResult(result);
		TwitchTVContext::i()->SetWaitForCallback(false);
	}

	void AuthDoneCallback(TTV_ErrorCode result, void* /*userData*/)
	{
		if (TTV_SUCCEEDED(result))
		{
			TwitchTVContext::i()->AddTwitchState(TWITCHTV_GET_AUTH);
			I3ASSERT("트위치 티비 인증 성공");
		}
		else
		{
			I3ASSERT("트위치 티비 인증 실패");
		}
		TwitchTVContext::i()->SetLastTaskResult(result);
		TwitchTVContext::i()->SetWaitForCallback(false);
	}

	void StatsCallback(TTV_StatType type, uint64_t value)
	{
		switch (type)
		{
		case TTV_ST_RTMPSTATE:
			TwitchTVContext::i()->SetRTMPState(value);
			break;
		case TTV_ST_RTMPDATASENT:
			TwitchTVContext::i()->SetTotalSent(value);
			break;
		}
	}

	void UserInfoDoneCallback(TTV_ErrorCode result, void* /*userData*/)
	{
		if (TTV_SUCCEEDED(result))
		{
			TwitchTVContext::i()->AddTwitchState(TWITCHTV_GET_USERINFO);
			I3ASSERT("트위치 티비 유저정보 받기 성공");
		}
		else
		{
			I3ASSERT("트위치 티비 유저정보 받기 실패");
		}
	}


	void StreamInfoDoneCallback(TTV_ErrorCode result, void* /*userData*/)
	{
		if (TTV_SUCCEEDED(result))
		{
			TwitchTVContext::i()->AddTwitchState(TWITCHTV_GET_STREAMINFO);
			I3ASSERT("트위치 티비 방송정보 받기 성공");
		}
		else
		{
			I3ASSERT("트위치 티비 유저정보 받기 실패");
		}
	}
	
	void FrameUnlockCallback(const uint8_t* buffer, void* /*userData*/)
	{
		unsigned char* p = const_cast<unsigned char*>(buffer);

		// Put back on the free list
		TwitchTVContext::i()->GetFreeBufferList().push_back(p);
		TwitchTVContext::i()->SetWaitForCallback(true);
	}
}

//=============================================================//
//=============================================================//

TTV_ErrorCode TwitchTVContext::TwitchTVAuthenticate(TTV_AuthToken* authToken, const i3::wstring& id, const i3::wstring& pw)
{
	SetWaitForCallback(true);

	i3::stack_string cid, cpw;
	i3::utf16_to_utf8(id, cid);
	i3::utf16_to_utf8(pw, cpw);
	
	TTV_AuthParams authParams;
	authParams.size = sizeof(TTV_AuthParams);
	authParams.userName = cid.c_str();
	authParams.password = cpw.c_str();
	authParams.clientSecret = "lvprhsdedfopm3h98s07gr3cz0gvy7d";

	TTV_ErrorCode ret = TTV_RequestAuthToken(&authParams, TTV_RequestAuthToken_Broadcast, AuthDoneCallback, NULL, authToken);
	
	while(m_bWaitingForCallback && TTV_SUCCEEDED(ret))
	{
		ret = TTV_PollTasks();
	}

	return m_LastTaskResult;
}

TTV_ErrorCode TwitchTVContext::TwitchTVLogin(TTV_AuthToken* authToken, TTV_ChannelInfo* channelInfo)
{
	SetWaitForCallback(true);

	TTV_ErrorCode ret = TTV_Login(authToken, LoginCallback, nullptr, channelInfo);

	while(m_bWaitingForCallback && TTV_SUCCEEDED(ret))
	{
		ret = TTV_PollTasks();
	}

	return m_LastTaskResult;
}

TTV_ErrorCode TwitchTVContext::TwitchTVGetIngetList(const TTV_AuthToken* authToken, TTV_IngestList* ingestList)
{
	SetWaitForCallback(true);

	TTV_ErrorCode ret = TTV_GetIngestServers(authToken, IngestListCallback, 0, ingestList);

	while(m_bWaitingForCallback && TTV_SUCCEEDED(ret))
	{
		ret = TTV_PollTasks();
	}

	return m_LastTaskResult;
}

TTV_ErrorCode TwitchTVContext::TwitchTVFoundIngestServer()
{
	TTV_RegisterStatsCallback(StatsCallback);

	const uint width = 1280;
	const uint height = 720;

	TTV_VideoParams Test_videoParams;
	::memset(&Test_videoParams, 0, sizeof(TTV_VideoParams));
	Test_videoParams.size = sizeof(TTV_VideoParams);
	Test_videoParams.targetFps = TTV_MAX_FPS;
	Test_videoParams.maxKbps = TTV_MAX_BITRATE;
	Test_videoParams.outputWidth = width;
	Test_videoParams.outputHeight = height;
	Test_videoParams.pixelFormat = TTV_PF_BGRA;

	TTV_GetDefaultParams(&Test_videoParams);

	std::vector<uint32_t> whiteBuffer(width*height, 0xFFFFFFFF);
	std::vector<uint32_t> blackBuffer(width*height, 0x000000FF);

	TTV_AudioParams Test_audioParams = {sizeof(TTV_AudioParams)};
	Test_audioParams.audioEnabled = false;
	Test_audioParams.enableMicCapture = false;
	Test_audioParams.enablePlaybackCapture = false;
	Test_audioParams.enablePassthroughAudio = false;

	float bestConectionTime = FLT_MAX; 
	uint  bestServerIndex = 0;
	TTV_ErrorCode ret = TTV_EC_SUCCESS;
	
	
	i3::stack_string select_server[4]; 
	i3::utf16_to_utf8( g_pConfigEx->GetTwitchTV().Server0.c_str(), select_server[0] );
	i3::utf16_to_utf8( g_pConfigEx->GetTwitchTV().Server1.c_str(), select_server[1] );
	i3::utf16_to_utf8( g_pConfigEx->GetTwitchTV().Server2.c_str(), select_server[2] );
	i3::utf16_to_utf8( g_pConfigEx->GetTwitchTV().Server3.c_str(), select_server[3] );

	{
		//가장 빠른 접속 속도를 제공하는 서버 고르기.
		for (uint i = 0U ; TTV_SUCCEEDED(ret) && i < m_IngestList.ingestCount; i++ )
		{
			bool check = false;
			i3::stack_string check_server_name(m_IngestList.ingestList[i].serverName);
			for(int index_select = 0 ; index_select < 4 ; ++index_select)
			{
				size_t found = check_server_name.find(select_server[index_select]);
				if( found!=i3::stack_string::npos )
					check = true;
			}

			if(!check)
				continue;

			float startTime = (float)timeGetTime();			//밀리세컨드로 계산.
			ret = TTV_Start(&Test_videoParams, &Test_audioParams, &m_IngestList.ingestList[i], 0, NULL, NULL);
			float elapsedTime = ((float)timeGetTime()) - startTime;

			if(bestConectionTime > elapsedTime)
			{
				bestConectionTime = elapsedTime;
				bestServerIndex = i;
			}

			if (TTV_SUCCEEDED(ret))
			{
				ret = TTV_Stop(NULL, NULL);
			}

			TTV_PollStats();
		}
	}

	{
		//선택한 서버의 비트레이트 구하기.
		float testDuration    = 5.0f;
		float startTime = (float)timeGetTime() * 0.001f;	//초 단위로 계산.
		float elapsedTime = 0.f;
		bool twiddle = true;
		uint64_t lastTotalSent = GetTotalSent();

		SetRTMPState(0);
		SetTotalSent(0);

		ret = TTV_Start(&Test_videoParams, &Test_audioParams, &m_IngestList.ingestList[bestServerIndex], 0, NULL, NULL);
		do 
		{
			TTV_SubmitVideoFrame(reinterpret_cast<uint8_t*>(twiddle ? &whiteBuffer[0] : &blackBuffer[0]), TestBufferUnlockCallback, 0);
			twiddle = !twiddle;

			TTV_PollStats();
			elapsedTime =  ((float)timeGetTime()*0.001f) - startTime;

			if (lastTotalSent != GetTotalSent() )
			{
				float elapsedMilliseconds = elapsedTime * 1000.0f; //밀리세컨드로 계산.
				m_fServerBitrate = static_cast<float>(GetTotalSent() * 8) / static_cast<float>(elapsedMilliseconds);
				lastTotalSent = GetTotalSent();
			}

		} while (elapsedTime < testDuration);

		if (TTV_SUCCEEDED(ret))
		{
			ret = TTV_Stop(NULL, NULL);
		}

		TTV_PollStats();
	}

	//계산된것들 적용 및 정리.
	if( bestServerIndex < m_IngestList.ingestCount )
	{
		m_IngestServer = m_IngestList.ingestList[bestServerIndex];
	}

	AddTwitchState(TWITCHTV_GET_INGESTSERVER);
	TTV_RemoveStatsCallback(StatsCallback);

	return ret;
}

TTV_ErrorCode TwitchTVContext::TwitchTVStreamingStart()
{
	TTV_ErrorCode ret = TwitchTVChangeOption(true);
	if ( TTV_FAILED(ret) )
	{
		return ret;
	}

	AddTwitchState(TWITCHTV_STREAMING);
	
	TWRESOULTION_INFO * pResoultionInfo = m_ResoultionLsit[m_nResoultionIndex];
	for (unsigned int i=0; i< 3 ; ++i)
	{
		unsigned char* pBuffer = new unsigned char[(pResoultionInfo->m_width*pResoultionInfo->m_height)*4+1];
		m_FreeBufferList.push_back(pBuffer);
	}
	
	return ret;
}

TTV_ErrorCode TwitchTVContext::TwitchTVStreamingStop()
{
	TTV_ErrorCode ret = TTV_EC_UNKNOWN_ERROR;

	if (!isBroadCasting())
	{
		return ret;
	}

	ret = TTV_Stop(NULL, NULL);
	if ( TTV_FAILED(ret) )
	{
		return ret;
	}
	RemoveTwitchState(TWITCHTV_STREAMING);

	for (unsigned int i=0; i<m_FreeBufferList.size(); ++i)
	{
		delete [] m_FreeBufferList[i];
	}
	m_FreeBufferList.clear();

	I3_SAFE_RELEASE_NODBG(m_CaptureTexture);
	I3_SAFE_RELEASE_NODBG(m_CaptureSurface);

	for (int i = 0; i < NUM_CAPTURE_SURFACES; ++i)
	{
		I3_SAFE_RELEASE_NODBG(m_CaptureQuery[i]);
		I3_SAFE_RELEASE_NODBG(m_ResizeSurface[i]);
	}

	return ret;
}

TTV_ErrorCode TwitchTVContext::TwitchTVSubmitFrame(unsigned char* pBgraFrame)
{
	TTV_ErrorCode ret = TTV_EC_UNKNOWN_ERROR;

	if(!isBroadCasting())
	{
		return ret;
	}

	ret = TTV_SubmitVideoFrame(pBgraFrame, FrameUnlockCallback, 0);
	if ( TTV_FAILED(ret) )
	{
	}

	return ret;
}

TTV_ErrorCode TwitchTVContext::TwitchTVChangeOption(bool IsNeedRestart)
{
	TTV_ErrorCode ret = TTV_EC_SUCCESS;
	
	if( IsNeedRestart )
	{
		if( TwitchTVContext::i()->isBroadCasting() )
		{
			if( !TwitchTVContext::i()->TTVStreamStop() )
				return ret;
		}

		TWRESOULTION_INFO * pResoultionInfo = m_ResoultionLsit[m_nResoultionIndex];
		TWFRAME_INFO * pFrameInfo = m_FrameList[m_nFrameIndex];

		//비트레이트 설정
		UINT bitrate = 0;
		if( pResoultionInfo->m_recomendbitrate < m_fServerBitrate )
			bitrate = (UINT) m_fServerBitrate;
		else
			bitrate = (UINT) pResoultionInfo->m_recomendbitrate;

		//인코딩 Cpu 사용량 설정.
		TTV_EncodingCpuUsage encCpu = TTV_ECU_MEDIUM;
		switch(m_nVideoIndex)
		{
		case 0:
			encCpu = TTV_ECU_LOW;
			break;
		case 1:
			encCpu = TTV_ECU_MEDIUM;
			break;
		case 2:
			encCpu = TTV_ECU_HIGH;
			break;
		}

		//방송 비디오 설정.
		TTV_VideoParams videoParams  = {sizeof(TTV_VideoParams)};
		videoParams.pixelFormat		 = TTV_PF_BGRA;
		videoParams.outputWidth		 = pResoultionInfo->m_width;
		videoParams.outputHeight	 = pResoultionInfo->m_height;
		videoParams.targetFps		 = pFrameInfo->m_frame;
		videoParams.encodingCpuUsage = encCpu;
		videoParams.maxKbps			 = bitrate;

		//방송 오디오 설정.
		TTV_AudioParams audioParams;
		audioParams.size = sizeof(TTV_AudioParams);
		audioParams.audioEnabled = m_bSpeaker;
		audioParams.enableMicCapture = m_bMike;
		audioParams.enablePlaybackCapture = true;
		audioParams.enablePassthroughAudio = false;

		ret = TTV_Start(&videoParams, &audioParams, &m_IngestServer, 0, NULL, NULL);
		if ( TTV_FAILED(ret) )
		{
			RemoveTwitchState(TWITCHTV_STREAMING);
			return ret;
		}
	}

	i3::wstring wgameTitle = GAME_STRING("STR_TWITCH_HOMEPAGE_NAME");
	i3::stack_string gameTitle;
	i3::stack_string broadTitle;
	i3::utf16_to_mb(m_sBroadTitle, broadTitle);
	i3::utf16_to_mb(wgameTitle, gameTitle);

	//방송 송출 설정, 방송 제목, 게임 제목
	TTV_StreamInfoForSetting streaminfoParams = {sizeof(TTV_StreamInfoForSetting)};
	i3::generic_string_copy(streaminfoParams.streamTitle,broadTitle.c_str());
	i3::generic_string_copy(streaminfoParams.gameName,gameTitle.c_str());

	ret = TTV_SetStreamInfo(&m_AuthToken, m_Channelnfo.name, &streaminfoParams, NULL, NULL);
	if ( TTV_FAILED(ret) )
	{
		I3ASSERT("트위치 티비 방송 설정 성공");
		return ret;
	}

	ret = TTV_SetVolume(TTV_PLAYBACK_DEVICE, m_bSpeaker?((float)m_nSpeakerVolume/100.0f):0.0f );
	if ( TTV_FAILED(ret) )
	{
		I3ASSERT("트위치 티비 사운드 볼륨 성공");
		return ret;
	}

	ret = TTV_SetVolume(TTV_RECORDER_DEVICE, m_bMike?((float)m_nMikeVolume/100.0f):0.0f );
	if ( TTV_FAILED(ret) )
	{
		I3ASSERT("트위치 티비 마이크 볼륨 성공");
		return ret;
	}

	return ret;
}

bool TwitchTVContext::CaptureFrame(int captureWidth, int captureHeight, unsigned char*& outBgraFrame )
{
	IDirect3DDevice9 * pD3DDevice = (IDirect3DDevice9 *) g_pViewer->GetRenderContext()->GetNativeDevice();
	IDirect3DSurface9* pMainRenderTargetSurface = g_pViewer->GetRenderContext()->GetRenderTarget(0)->GetColorSurface();

	if (pMainRenderTargetSurface == nullptr || 
		captureWidth <= 0 || 
		captureHeight <= 0 ||
		captureWidth % 16 != 0 ||
		captureHeight % 16 != 0)
	{
		return false;
	}

	D3DSURFACE_DESC srcDesc;
	pMainRenderTargetSurface->GetDesc(&srcDesc);

	if (m_CaptureTexture == nullptr ||
		captureWidth != m_CaptureTextureWidth || 
		captureHeight != m_CaptureTextureHeight)
	{
		I3_SAFE_RELEASE_NODBG(m_CaptureTexture);
		I3_SAFE_RELEASE_NODBG(m_CaptureSurface);

		for (int i = 0; i < NUM_CAPTURE_SURFACES; ++i)
		{
			I3_SAFE_RELEASE_NODBG(m_CaptureQuery[i]);
			I3_SAFE_RELEASE_NODBG(m_ResizeSurface[i]);
		}

		if ( FAILED(pD3DDevice->CreateTexture(captureWidth, captureHeight, 1, D3DUSAGE_RENDERTARGET, srcDesc.Format, D3DPOOL_DEFAULT, &m_CaptureTexture, NULL)) )
		{
			return false;
		}

		if ( FAILED(m_CaptureTexture->GetSurfaceLevel(0, &m_CaptureSurface)) )
		{
			return false;
		}

		for (int i = 0; i < NUM_CAPTURE_SURFACES; ++i)
		{
			if ( FAILED(pD3DDevice->CreateOffscreenPlainSurface(captureWidth, captureHeight, srcDesc.Format, D3DPOOL_SYSTEMMEM, &m_ResizeSurface[i], NULL)) )
			{
				return false;
			}
		}

		m_CaptureGet = 0;
		m_CapturePut = 0;

		m_CaptureTextureWidth = captureWidth;
		m_CaptureTextureHeight = captureHeight;
	}

	if (m_CapturePut - m_CaptureGet < NUM_CAPTURE_SURFACES)
	{
		float captureAspect = (float)captureHeight / (float)captureWidth;
		float srcAspect = (float)srcDesc.Height / (float)srcDesc.Width;
		RECT destRect;

		if (captureAspect >= srcAspect)
		{
			float scale = (float)captureWidth / (float)srcDesc.Width;

			destRect.left = 0;
			destRect.right = captureWidth-1;
			destRect.top = (LONG)( ((float)captureHeight - (float)srcDesc.Height*scale) / 2 );
			destRect.bottom = (LONG)( ((float)captureHeight + (float)srcDesc.Height*scale) / 2 );
		}
		else
		{
			float scale = (float)captureHeight / (float)srcDesc.Height;

			destRect.top = 0;
			destRect.bottom = captureHeight-1;
			destRect.left = (LONG)( ((float)captureWidth - (float)srcDesc.Width*scale) / 2 );
			destRect.right = (LONG)( ((float)captureWidth + (float)srcDesc.Width*scale) / 2 );
		}

		int idx = m_CapturePut % NUM_CAPTURE_SURFACES;

		if ( FAILED(pD3DDevice->StretchRect(pMainRenderTargetSurface, NULL, m_CaptureSurface, &destRect, D3DTEXF_LINEAR)) )
		{
			return false;
		}

		if ( FAILED(pD3DDevice->GetRenderTargetData(m_CaptureSurface, m_ResizeSurface[idx])) )
		{
			return false;
		}

		// Create a query that will indicate when the GetRenderTargetData call has finished
		if (m_CaptureQuery[idx] == nullptr)
		{
			pD3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &m_CaptureQuery[idx]);
		}

		// Schedule the query
		m_CaptureQuery[idx]->Issue( D3DISSUE_END );

		++m_CapturePut;
	}

	// Get the latest capture
	{
		int idx = m_CaptureGet % NUM_CAPTURE_SURFACES;
		if (m_CaptureGet != m_CapturePut && 
			m_CaptureQuery[idx] && 
			m_CaptureQuery[idx]->GetData(NULL, 0, 0) == S_OK)
		{
			D3DLOCKED_RECT locked;
			memset( &locked, 0, sizeof(locked) );

			// Attempt to lock the surface to obtain the pixel data
			HRESULT ret = m_ResizeSurface[idx]->LockRect(&locked, NULL, D3DLOCK_READONLY);
			if ( SUCCEEDED(ret) )
			{
				// Create the output buffer
				int bufferSize = captureWidth*captureHeight*4;

				// grab the free buffer from the streaming pool
				outBgraFrame = GetNextFreeBuffer();
				if (!outBgraFrame)
				{
					m_ResizeSurface[idx]->UnlockRect();
					return false;
				}

				memcpy(outBgraFrame, locked.pBits, bufferSize);

				// Unlock the surface
				m_ResizeSurface[idx]->UnlockRect();
				++m_CaptureGet;
				return true;
			}
		}
	}

	return false;
}

//=============================================================//
//=============================================================//

bool TwitchTVContext::TTVLogin(const i3::wstring& id, const i3::wstring& pw)
{
	TTV_ErrorCode ret = TTV_EC_SUCCESS;
	
	if( !isTwitchState(TWITCHTV_INIT) )
	{
		TTV_Init(NULL,"psrw936c0p117d6atj3gmhqkrh91r5u",L"");
		if(TTV_SUCCEEDED(ret))
		{
			AddTwitchState(TWITCHTV_INIT);
		}
		else
		{
			I3ASSERT("트위치 티비 SDK 초기화 실패");
			return false;
		}
	}

	ret = TwitchTVAuthenticate(&m_AuthToken, id, pw);
	if(!TTV_SUCCEEDED(ret))
	{
		I3ASSERT("트위치 티비 인증 실패");
		return false;
	}
	m_Channelnfo.size = sizeof(m_Channelnfo);

	ret = TwitchTVLogin(&m_AuthToken, &m_Channelnfo);
	if(!TTV_SUCCEEDED(ret))
	{
		I3ASSERT("트위치 티비 로그인 실패");
		return false;
	}

	ret = TwitchTVGetIngetList(&m_AuthToken, &m_IngestList);
	if(!TTV_SUCCEEDED(ret))
	{
		I3ASSERT("트위치 티비 서버 정보 받아오기 실패");
		return false;
	}

	Init();
	
	return true;
}

bool TwitchTVContext::TTVStreamStart()
{
	TTV_ErrorCode ret = TwitchTVFoundIngestServer();
	if(!TTV_SUCCEEDED(ret))
	{
		I3ASSERT("트위치 티비 최적 서버 선택 실패");
		return false;
	}

	ret = TwitchTVStreamingStart();
	if(!TTV_SUCCEEDED(ret))
	{
		I3ASSERT("트위치 티비 방송 시작 실패");
		return false;
	}
	
	SetFastSreamingStop(false);
	SaveTwitchFile();

	m_bWaitingForCallback = true;
	m_LastCaptureTime = 0;
	return true;
}

bool TwitchTVContext::TTVStreamStop()
{
	TTV_ErrorCode ret = TwitchTVStreamingStop();
	if(!TTV_SUCCEEDED(ret))
	{
		I3ASSERT("트위치 티비 방송 종료 실패");
		return false;
	}
	
	SetFastSreamingStop(true);
	SaveTwitchFile();

	return true;
}

//=============================================================//

namespace
{
	unsigned __int64 GetSystemClockFrequency()
	{	
		static unsigned __int64 frequency = 0;
		if (frequency == 0)
		{		
			QueryPerformanceFrequency( reinterpret_cast<LARGE_INTEGER*>(&frequency) );		
		}

		return frequency;
	}

	unsigned __int64 GetSystemClockTime()
	{
		unsigned __int64 counter;
		QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>(&counter) );
		return counter;
	}

	unsigned __int64 SystemTimeToMs(unsigned __int64 sysTime)
	{
		return sysTime * 1000 / GetSystemClockFrequency();
	}

	unsigned __int64 GetSystemTimeMs()
	{
		return SystemTimeToMs( GetSystemClockTime() );
	}
}

//=============================================================//

bool TwitchTVContext::TTVStreaming()
{
	if(!isTwitchLogin() || !isBroadCasting())
		return false;

	TWFRAME_INFO * pFrameInfo = m_FrameList[m_nFrameIndex];
	TWRESOULTION_INFO * pResoultionInfo = m_ResoultionLsit[m_nResoultionIndex];

	unsigned __int64 curTime = GetSystemTimeMs();

	unsigned __int64 captureDelta = curTime - m_LastCaptureTime;
	bool isTimeForNextCapture = (captureDelta / 1000.0f) >= (1.0f / pFrameInfo->m_frame);

	if(isBroadCasting() && isTimeForNextCapture)
	{
		unsigned char* pBgraFrame = nullptr;
		bool gotFrame = false;

		gotFrame = CaptureFrame(pResoultionInfo->m_width,pResoultionInfo->m_height,pBgraFrame);

		if(gotFrame)
		{
			TwitchTVSubmitFrame(pBgraFrame);
		}
	}

	TTV_PollTasks();

	return true;
}

bool TwitchTVContext::TTVOptionChange( bool IsNeedRestart )
{
	TTV_ErrorCode ret = TwitchTVChangeOption( IsNeedRestart );
	if(!TTV_SUCCEEDED(ret))
	{
		I3ASSERT("트위치 티비 옵션 변경 실패");
		return false;
	}

	return true;
}

void TwitchTVContext::CbFastStopStreaming(void* pThis, INT32 nParam)
{
	I3ASSERT( pThis != nullptr);
	if( nParam == MBR_OK)
	{
		g_pFramework->GetUIMainframe()->OpenPopup(UPW_TWITCHLOGIN);
	}
	TwitchTVContext::i()->SetFastSreamingStop(true);
	TwitchTVContext::i()->SetMsgBox(true);
	TwitchTVContext::i()->SaveTwitchFile();
}