#ifndef _VIDEOCAPTURE_H__
#define _VIDEOCAPTURE_H__

#include "bandicap.h"		// 동영상 녹화 - 양승천[2011.06.17]

enum HUD_MOVIE_CAPTURE_SYMBOL
{
	HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_ON = 0,
	HUD_MOVIE_CAPTURE_SYMBOL_LIGHT_OFF,

	HUD_MOVIE_CAPTURE_SYMBOL_CNT
};

class CVideoCapture : public i3ElementBase
{
	I3_CLASS_DEFINE(CVideoCapture, i3ElementBase);

private:
	CBandiCaptureLibrary *			m_pBandiCapture;		// 동영상 녹화 - 양승천[2011.06.17]

	//i3AttrSet*						m_pVCRoot;				// 녹화에 사용되는 AttrSet
	i3Texture  *					m_pHud3Texture;			// 동영상에 사용되는 HUD 포함 Texture;
	//i3Sprite2D *					m_pSpriteSymbol;		// 녹화중 표시 심볼
	//i3Sprite2D *					m_pSpriteString;		// 녹화중 표시 스트링

	REAL32							m_rMinCaptureTime;		// 영상 녹화 최소 시간 
	REAL32							m_fNotifyDelay;			// 영상 녹화중 통지 딜레이
	
	INT32							m_nCaptureType;			// 영상 프리셋

protected:
	bool CreateLibrary();									// 반디 라이브러리 생성
	
	void StartVideoCapture(CGameChatBox* pChatBox);
	void NotifyForVideoCapture(REAL32 rDelta);				// 영상 녹화 통보 UI

public:
	CVideoCapture(void);
	~CVideoCapture(void);

	void InitHud3Texture(void);												// 영상 표시 UI 초기화
	void SaveVideoCapture(void);											// 영상 녹화
	void SetVideoCaptureInfo(INT32 iIdx);									// 영상 해상도 설정 
	void SetVideoCapture(CGameChatBox* pChatBox);							// 영상 녹화 설정
	void StopVideoCapture(CGameChatBox* pChatBox, bool bUserStop = true);	// 영상 녹화 종료

	i3Texture* LoadHud3Texture(void);						// Ingame Hud3 return
	bool IsCapturing() const { if (m_pBandiCapture != nullptr) { return m_pBandiCapture->IsCapturing() ? true : false; } return false; }

	bool	CaptureImage( LPCSTR pszFilePath, INT nFileType, INT nQuality, INT nCaptureMode, bool bIncludeCursor );
	bool	CaptureImage( LPCWSTR pwszFilePath, INT nFileType, INT nQuality, INT nCaptureMode, bool bIncludeCursor );
};

#endif //_VIDEOCAPTURE_H__