#if !defined( __I3_GAME_INTERFACE_VIEWER_H)
#define __I3_GAME_INTERFACE_VIEWER_H

class i3GameInterfaceViewer : public i3GameInterface
{
public:
	// 게임의 이름 (파일명)
	virtual const char *		GetTitle(void)				{		return "i3Viewer";	}

	// 사용자 출력용 게임 이름
	virtual const char *		GetLongTitle(void)			{		return "i3Engine Scene-graph Viewer";	}

	// Version
	// High Word : Major version
	// Low Word  : Minor version
	virtual UINT32				GetVersion(void)			{		return 0x00020000 | 0x00000000;		}

	// 기본 Window의 폭
	virtual UINT32				GetScreenWidth(void)		{		return 800;		}
	
	// 기본 Window의 높이
	virtual UINT32				GetScreenHeight(void)		{		return 600;		}
	virtual UINT32				GetScreenColorBits(void)	{		return 0; }
	virtual UINT32				GetScreenDepthBits(void)	{		return 0; }
	virtual UINT32				GetScreenStencilBits(void)	{		return 0; }
	virtual BOOL				GetFullScreenEnable(void)	{		return FALSE; }
	virtual bool				GetMenuKeyEnable(void)		{		return true; }
	virtual UINT32				GetInputDeviceMask(void)	{		return 0; }

	// 초기 메모리 점유량
	// 0 : System Default
	virtual UINT32				GetInitialMemSize(void)		{		return 0;	}
	virtual bool				GetInitialWindowVisible(void)	{	return true; }

	// 목표 FPS
	virtual UINT32				GetTargetFPS(void)			{		return 60; }

	// Initialize Process Instance
	//
	// Win32 Platform :
	//		pInstHandle : HINSTANCE
	//		pScrHandle	: HWND
	//
	virtual bool				OnInitInstance( void * pInstHandle, void * pScrHandle)
	{
		return true;
	}

	virtual void				OnExitInstance(void)			{; }

	virtual bool				GetMultipleInstanceEnable(void)
	{
		return true;
	}

	virtual i3Framework *		CreateFramework( i3Viewer * pViewer)
	{
		i3Framework * pFramework = i3ViewerFramework::NewObject();

		pFramework->Create( pViewer);

		return pFramework;
	}

	virtual bool				OnInitFramework( i3Framework * pFramework, const char * pszCmdLine);

};

#endif
