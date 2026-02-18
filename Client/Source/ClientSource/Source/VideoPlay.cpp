#include "pch.h"
#include "VideoPlay.h"

I3_CLASS_INSTANCE( CVideoPlay);//, i3ElementBase);

void CALLBACK	OnExternalRender(float tm)
{
	g_pFramework->GetVideoPlayer()->RenderVideo();
}


CVideoPlay::CVideoPlay(void)
{
	bvl = NULL;
	bvd = NULL;
	bvt = NULL;
	CreateLibrary();
}

CVideoPlay::~CVideoPlay(void)
{
	Destroy();
}

void CVideoPlay::Destroy()
{
	g_pViewer->setExternalRender(NULL);
	bvl->Destroy();

	I3_SAFE_DELETE(bvl);
	if(bvd)
	{
		bvd->Close();
		I3_SAFE_DELETE( bvd);
	}
	//	delete bvd;
	if(bvt)
	{
		//bvt->Close();
		I3_SAFE_DELETE( bvt);
	}
}

bool CVideoPlay::CreateLibrary()
{
	if(bvl == NULL)
	{
		bvl = new CBandiVideoLibrary();
		bvd = new CBandiVideoDevice_DX9();
		bvt = new CBandiVideoTexture_DX9((CBandiVideoDevice_DX9*)bvd);

		MEMDUMP_NEW( bvl, sizeof(CBandiVideoLibrary));
		MEMDUMP_NEW( bvd, sizeof(CBandiVideoDevice_DX9));
		MEMDUMP_NEW( bvt, sizeof(CBandiVideoTexture_DX9));
		
		g_pViewer->setExternalRender(OnExternalRender);
	}
	return true;
}

void CVideoPlay::OpenFile(const char *videoFile)
{
	// File Open시 객체 생성 및 라이브 러리 로드
	if( bvl->IsCreated() == false)
	{
		// 추후에 사용시에는 라이센스 구매해야되는 용도로 Assert 처리 합니다.
		I3ASSERT(FALSE);

		if(FAILED(bvl->Create(BANDIVIDEO_DLL_FILE_NAME,NULL,NULL)) )
			return;

		//// 라이센스 인증 (2013.04.24 기준, 라이센스 구매 안됨)
		//if( FAILED(bvl->Verify("ZEPETTO_POINTBLANK_20110812", "bb76cf9a") )
		//	return;
	}

	if(FAILED(bvl->Open(videoFile, FALSE)))
		return;

	if(FAILED(bvl->GetVideoInfo(m_VideoInfo)))
	{
		DestroyWindow(g_hWnd);
		return;
	}

	if(!bvd/* || FAILED(bvd->Open(g_hWnd))*/)
	{
		DestroyWindow(g_hWnd);
		I3_SAFE_DELETE(bvd);
		return;
	}

	if(FAILED(bvt->Open(m_VideoInfo.width, m_VideoInfo.height)))
	{
		DestroyWindow(g_hWnd);
		I3_SAFE_DELETE(bvd);
		I3_SAFE_DELETE(bvt);
	}
}

void CVideoPlay::PlayVideo()
{
	bvl->Play();
}

void CVideoPlay::PauseVideo()
{
	bvl->Pause(TRUE);
}

void CVideoPlay::StopVideo()
{
	bvl->Stop();
}

void CVideoPlay::CloseVideo()
{
	bvl->Close();

	// 파일 종료시, 객체 및 라이브러리 해제
	bvl->Destroy();
}

void CVideoPlay::RenderVideo()
{
	if(bvl == NULL || bvl->IsCreated() == false)
		return;

	BVL_STATUS status;
	bvl->GetStatus(status);
	if(status == BVL_STATUS_PLAYEND)
	{
		bvl->Seek(0, BVL_SEEK_TIME);
		bvl->Play();
	}

	if(bvl->IsNextFrame())
	{
		INT pitch;
		BYTE* buf = bvt->Lock(pitch);
		if(buf)
		{
			BVL_FRAME frame;
			frame.frame_buf = buf;
			frame.frame_buf_size = m_VideoInfo.height*pitch;
			frame.pitch = pitch;
			frame.width = m_VideoInfo.width;
			frame.height = m_VideoInfo.height;
			frame.pixel_format = BVLPF_X8R8G8B8;

			bvl->GetFrame(frame, TRUE);
			bvt->Unlock();

			// Draw frame
			bvt->Draw(0, 0, m_VideoInfo.width, m_VideoInfo.height);

		}
	}
}

void CVideoPlay::LostDevice( void)
{
	//Destroy();
}

void CVideoPlay::Revive( void)
{
	//CreateLibrary();
}

