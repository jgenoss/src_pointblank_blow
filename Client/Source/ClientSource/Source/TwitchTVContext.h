#if !defined( __TWITCH_CONTEXT_H)
#define __TWITCH_CONTEXT_H

#include "twitchsdk.h" 

#ifdef  I3_DEBUG
#pragma comment(lib, "../lib/twitchsdk_32_Debug.lib")
#else
#pragma comment(lib, "../lib/twitchsdk_32_Release.lib")
#endif

#define MAX_FRAME_INDEX	4
#define MAX_VIDEO_INDEX 3
#define MAX_RESOULTION_INDEX 6
#define NUM_CAPTURE_SURFACES 4

#define TWITCHTV_INIT				0x01
#define TWITCHTV_GET_AUTH			0x02
#define TWITCHTV_GET_INGESTLIST		0x04
#define TWITCHTV_GET_USERINFO		0x08
#define TWITCHTV_GET_STREAMINFO		0x10
#define TWITCHTV_GET_INGESTSERVER	0x20
#define TWITCHTV_STREAMING			0x40
#define TWITCHTV_LOGIN				0x80

struct TWFRAME_INFO
{
	INT32			m_frame = 0;
	i3::wstring		m_sframe;
} ;

struct TWVIDEO_INFO
{
	i3::wstring		m_video;
} ;

struct TWRESOULTION_INFO
{
	INT32			m_width = 0;
	INT32			m_height = 0;
	INT32			m_recomendbitrate = 0;
} ;

class TwitchTVContext : public i3::shared_ginst<TwitchTVContext>
{
public:

	//Set,Get Function.
	bool	isTwitchLogin()	const								{ return isTwitchState(TWITCHTV_LOGIN); }
	bool	isBroadCasting() const								{ return isTwitchState(TWITCHTV_STREAMING); }
	bool	isMikeOn() const									{ return m_bMike; }
	void	SetMikeEnable(bool bmike)							{ m_bMike = bmike; }
	bool	isSpeakerOn() const									{ return m_bSpeaker; }
	void	SetSpeakerEnable(bool bspeaker)						{ m_bSpeaker = bspeaker; }
	bool	isMsgBox() const									{ return m_bMsgBox; }
	void	SetMsgBox(bool bMsg)								{ m_bMsgBox = bMsg; }
	bool	isFastStreamingStop()const							{ return m_bFastTwitchSreamingStop; }
	void	SetFastSreamingStop(bool bresult)					{ m_bFastTwitchSreamingStop = bresult; }

	void		 SetBroadTitle(const i3::wstring& stitle)		{ m_sBroadTitle = stitle; }
	i3::wstring	 GetBroadTitle() const							{ return m_sBroadTitle;   }

	void	SetFrame(INT32 nframe)								{ m_nFrameIndex = nframe; }
	INT32	GetFrame() const									{ return m_nFrameIndex;   }
	void	SetVideoGrade(INT32 nVideoGrade)					{ m_nVideoIndex = nVideoGrade; }
	INT32	GetVideoGrade()	const								{ return m_nVideoIndex;		   }
	void	SetMikeVolume(INT32 nMikeVolume)					{ m_nMikeVolume = nMikeVolume; }
	INT32	GetMikeVolume()	const								{ return m_nMikeVolume;		   }
	void	SetSpeakerVolume(INT32 nSpeakerVolume)				{ m_nSpeakerVolume = nSpeakerVolume; }
	INT32	GetSpeakerVolume() const							{ return m_nSpeakerVolume;			 }

	i3::wstring	GetFrameInfoString();
	INT32		GetFrameInfoToIndex(INT32 index); 
	i3::wstring GetVideoInfoString();
	i3::wstring GetVideoInfoToIndex(INT32 index);
	i3::wstring GetResoultionInfoString();
	i3::wstring GetResoultionInfoToIndex(INT32 index);

	void		SetLastTaskResult(TTV_ErrorCode result)				{ m_LastTaskResult = result; }
	void		SetWaitForCallback(bool result)						{ m_bWaitingForCallback = result; }
	
	void		AddTwitchState( UINT8 state )						{ m_ui8TwitchState |= state; }
	void		RemoveTwitchState(UINT8 state )						{ m_ui8TwitchState &= ~state; }
	bool		isTwitchState( UINT8 state ) const					{ return (m_ui8TwitchState & state) != 0; }	
	
	void		SetRTMPState( uint64_t state )						{ m_ui64RTMPState = state; }
	uint64_t	GetRTMPState()										{ return m_ui64RTMPState;  }
	void		SetTotalSent( uint64_t state )						{ m_ui64TotalSent = state; }
	uint64_t	GetTotalSent()										{ return m_ui64TotalSent;  }

	i3::vector<unsigned char*> GetFreeBufferList()					{ return m_FreeBufferList; }
	unsigned char* GetNextFreeBuffer()
	{
		unsigned char * pBuffer = m_FreeBufferList.back();
		if( m_bWaitingForCallback == false )
		{
			m_bWaitingForCallback = true;
			m_FreeBufferList.pop_back();
		}
		return pBuffer;
	}

public:
	TwitchTVContext();
	~TwitchTVContext();

	void Init();

	bool TTVLogin(const i3::wstring& id, const i3::wstring& pw);
	bool TTVStreamStart();
	bool TTVStreamStop();
	bool TTVStreaming();
	bool TTVOptionChange( bool IsNeedRestart );
	
	static void CbFastStopStreaming(void* pThis, INT32 nParam);
private:
	
	void Init_FrameInfo();
	void Init_VideoInfo();
	void Init_ResultionInfo();

	bool FindTwitchFile();
	bool OpenTwitchFile();
	void LoadTwitchFile(void);

	bool CreateTwitchFile();
	bool SaveTwitchFile();
	bool DeleteTwitchFile();
	void MakeTwitchMD5Key(void);
	char* GetTwitchMD5Key(void)
	{  
		// 실행중에 ShopFile이 변경될 수 도 있으니 다시한번 MD5Key를 생성합니다.
		MakeTwitchMD5Key(); 
		return m_strTwitchMD5Key; 
	};

	
private:
	uint64_t m_ui64TotalSent;						//트위치 poolstats용 변수
	uint64_t m_ui64RTMPState;						//트위치 poolstats용 변수

	UINT8		m_ui8TwitchState;					//트위치 진행 상태 마스크

	TTV_ErrorCode	m_LastTaskResult;				//트위치 티비 마지막 결과 값.
	bool	   m_bWaitingForCallback;				//트위치 티비 콜백 함수 진행 여부.

	i3::wstring	 m_sBroadTitle;						//트위치 티비 방송 제목
	INT32	m_nFrameIndex;							//트위치 티비 방송 선택 프레임
	INT32	m_nVideoIndex;							//트위치 티비 방송 선택 비드오퀄리티
	INT32	m_nResoultionIndex;						//트위치 티비 방송 선택 해상도
	bool	m_bMike;								//트위치 티비 마이크 on/off
	bool	m_bSpeaker;								//트위치 티비 스피커 on/off
	INT32	m_nMikeVolume;							//트위치 티비 마이크 볼륨
	INT32	m_nSpeakerVolume;						//트위치 티비 스피커 볼륨

	i3::vector<TWFRAME_INFO *> m_FrameList;				//트위치 티비 방송 프레임 설정 데이타
	i3::vector<TWVIDEO_INFO *> m_VideoList;				//트위치 티비 방송 비디오 품질 설정 데이타
	i3::vector<TWRESOULTION_INFO *> m_ResoultionLsit;	//트위치 티비 방송 해상도 설정 데이타

	TTV_AuthToken		m_AuthToken;				//트위치 티비 접속 토큰
	TTV_IngestList		m_IngestList;				//트위치 티비 접속 가능 서버 리스트
	TTV_UserInfo		m_UserInfo;					//트위치 티비 계정 정보
	TTV_StreamInfo		m_StreamInfo;				//트위치 티비 스트림 정보
	TTV_ChannelInfo     m_Channelnfo;				//트위치 티비 채널 정보
	TTV_IngestServer	m_IngestServer;				//트위치 티비 사용할 서버
	float				m_fServerBitrate;			//트위치 티비 사용할 서버 비트레이트

	i3::vector<unsigned char*> m_FreeBufferList;		//트위치 티비 유지해야되는 버퍼.
	
	IDirect3DTexture9* m_CaptureTexture;				
	IDirect3DSurface9* m_CaptureSurface;
	IDirect3DSurface9* m_ResizeSurface[NUM_CAPTURE_SURFACES];
	IDirect3DQuery9*   m_CaptureQuery[NUM_CAPTURE_SURFACES];		
	
	INT32 m_CaptureTextureWidth;
	INT32 m_CaptureTextureHeight;
	INT32 m_CaptureGet;
	INT32 m_CapturePut;

	uint64_t m_LastCaptureTime;
	
	bool    m_bMsgBox;
	bool	m_bFastTwitchSreamingStop;
	HANDLE	m_hTwitchShopFile;
	char	m_strTwitchMD5Key[NET_MD5_KEY_SIZE+1];
	
private:
	//트위치 작동 함수들..
	TTV_ErrorCode TwitchTVAuthenticate(TTV_AuthToken* authToken, const i3::wstring& id, const i3::wstring& pw);
	TTV_ErrorCode TwitchTVLogin(TTV_AuthToken* authToken, TTV_ChannelInfo* channelInfo);
	TTV_ErrorCode TwitchTVGetIngetList(const TTV_AuthToken* authToken, TTV_IngestList* ingestList);
	TTV_ErrorCode TwitchTVFoundIngestServer();
	TTV_ErrorCode TwitchTVStreamingStart();
	TTV_ErrorCode TwitchTVStreamingStop();
	TTV_ErrorCode TwitchTVSubmitFrame(unsigned char* pBgraFrame);
	TTV_ErrorCode TwitchTVChangeOption(bool IsNeedRestart );

	bool CaptureFrame(int captureWidth, int captureHeight, unsigned char*& outBgraFrame);
};

#endif