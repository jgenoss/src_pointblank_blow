#if !defined( __POINT_BLANK_GAME_INTERFACE_H)
#define __POINT_BLANK_GAME_INTERFACE_H

class i3GameObjPropertyDialog;

class GameInterface : public i3GameInterface
{
protected:
	void					ShaderVersionControl(void);

public:
		// 게임의 이름 (파일명)
	virtual const char *		GetTitle(void) override {		return GAME_TITLE_NAME_A;	}

	// 사용자 출력용 게임 이름
	virtual const char *		GetLongTitle(void) override {		return GAME_TITLE_NAME_A;	}

	// Version
	// High Word : Major version
	// Low Word  : Minor version
	virtual UINT32				GetVersion(void) override {		return 0x00010000 | 0x00000000;		}
	virtual void				GetVideoConfig( i3VideoInfo * pVideo) override;
	virtual bool				GetMenuKeyEnable(void) override {		return false; }
	virtual UINT32				GetInputDeviceMask(void) override {		return I3I_DEVICE_MASK_MOUSE | I3I_DEVICE_MASK_KEYBOARD; }

	// 초기 메모리 점유량
	// 0 : System Default
	virtual UINT32				GetInitialMemSize(void)	override {		return 0;	}

	virtual bool				GetInitialWindowVisible(void) override {	return false; }

	// 목표 FPS
	virtual UINT32				GetTargetFPS(void) override {		return 60; }

	virtual bool				GetMultipleInstanceEnable(void) override {	return false; 	}

	void						InitPhysXValues( void);

	// Initialize Process Instance
	//
	// Win32 Platform :
	//		pInstHandle : HINSTANCE
	//		pScrHandle	: HWND
	//
	virtual bool				OnInitInstance( void * pInstHandle, void * pScrHandle) override;
	virtual void				OnExitInstance( void) override;
	virtual void				OnRegisterMeta(void) override;

	virtual i3Framework *		CreateFramework( i3Viewer * pViewer) override;
	virtual bool				CreatePreFramework(HWND hwnd) override;
	virtual bool				OnInitFramework( i3Framework * pFramework, const char * pszCmdLine) override;

	virtual bool				OnDrive( i3Viewer * pViewer) override;

	virtual UINT32				GetViewportStyle(void) override;
	virtual UINT32				GetIcon(void) override;

	virtual void				RegisterScript( lua_State * L) override;

#if defined( I3_WINDOWS) && defined( USE_EDITDLG)//프로퍼티 쿼리 디버그
	virtual INT32				OnQueryPropertyDlgCount(void) override;
	virtual void				OnQueryPropertyDlg( HINSTANCE hInst, INT32 idx, i3ClassMeta ** ppMeta, i3GameObjPropertyDialog ** ppDlg) override;
#endif

private:
	void	_ParseCmdLineForNetwork( const char * pszCmdLine);

};

#ifndef USE_I3EXEC
GameInterface * i3CreateGameInterface(const char * pszRunPath);
UINT32 i3Geti3EngineVersion();
#endif

#endif
