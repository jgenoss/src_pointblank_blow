#if !defined( __GAME_INTERFACE_H)
#define __GAME_INTERFACE_H

class GameInterface : public i3GameInterface
{
public:
		// 게임의 이름 (파일명)
	virtual const char *		GetTitle(void)				{		return "PointBlank";	}

	// 사용자 출력용 게임 이름
	virtual const char *		GetLongTitle(void)			{		return "PointBlank";	}

	// Version
	// High Word : Major version
	// Low Word  : Minor version
	virtual UINT32				GetVersion(void)			{		return 0x00010000 | 0x00000000;		}
	virtual void				GetVideoConfig( i3VideoInfo * pVideo);
	virtual bool				GetMenuKeyEnable(void)		{		return false; }
	virtual UINT32				GetInputDeviceMask(void)	{		return I3I_DEVICE_MASK_MOUSE | I3I_DEVICE_MASK_KEYBOARD; }

	// 초기 메모리 점유량
	// 0 : System Default
	virtual UINT32				GetInitialMemSize(void)		{		return 0;	}

	virtual bool				GetInitialWindowVisible(void)	{	return false; }

	// 목표 FPS
	virtual UINT32				GetTargetFPS(void)			{		return 60; }

	virtual bool				GetMultipleInstanceEnable(void)		{	return false; 	}

	// Initialize Process Instance
	//
	// Win32 Platform :
	//		pInstHandle : HINSTANCE
	//		pScrHandle	: HWND
	//
	virtual bool				OnInitInstance( void * pInstHandle, void * pScrHandle);
	virtual void				OnExitInstance( void);
	virtual void				OnRegisterMeta(void);

	virtual i3Framework *		CreateFramework( i3Viewer * pViewer);
	virtual bool				CreatePreFramework(HWND hwnd); 
	virtual bool				OnInitFramework( i3Framework * pFramework, const char * pszCmdLine);

	virtual bool				OnDrive( i3Viewer * pViewer);

	virtual UINT32				GetViewportStyle(void);
	virtual UINT32				GetIcon(void);

#if defined( I3_WINDOWS) && defined( I3_DEBUG)//프로퍼티 쿼리 디버그
	virtual INT32				OnQueryPropertyDlgCount(void);
	virtual void				OnQueryPropertyDlg( HINSTANCE hInst, INT32 idx, i3ClassMeta ** ppMeta, i3GameObjPropertyDialog ** ppDlg);
#endif

};

#endif
