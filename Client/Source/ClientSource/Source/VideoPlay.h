#ifndef _VIDEOPLAY_H__
#define _VIDEOPLAY_H__

#include "bandivid.h"
#include "BandiVideoLibrary.h"
#include "BandiVideoTexture_DX9.h"

typedef void (CALLBACK *CALLBACK_EXTERNAL_RENDER)(REAL32 tm);

class CVideoPlay : public i3ElementBase
{
	I3_CLASS_DEFINE(CVideoPlay, i3ElementBase);

private:
	CBandiVideoLibrary*		bvl;
	CBandiVideoDevice*		bvd;
	CBandiVideoTexture*		bvt;
	BVL_VIDEO_INFO			m_VideoInfo;

protected:
	bool CreateLibrary();									// 반디 라이브러리 생성
	void	Destroy();

public:
	CVideoPlay(void);
	~CVideoPlay(void);

	void OpenFile(const char *videoFile);
	void PlayVideo();
	void StopVideo();
	void CloseVideo();
	void PauseVideo();
	void RenderVideo();
	LPDIRECT3DTEXTURE9 getTexture()	{return ((CBandiVideoTexture_DX9 *)bvt)->getTexture(); }

	void	LostDevice( void);
	void	Revive( void);
};

#endif