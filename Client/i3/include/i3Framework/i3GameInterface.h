#if !defined( __I3_GAME_INTERFACE_H)
#define __I3_GAME_INTERFACE_H

#include "i3Framework.h"

#if defined( I3_WINDOWS)
#include "i3GameObjPropertyDialog.h"
#endif

typedef INT32 (* I3_INTERACT_PROC)( void * pBuffer);
typedef INT32 (* I3_INTERACT_PROC2)( void * pBuffer, void * pBuffer2);
typedef INT32 (* I3_INTERACT_PROC3)( void * pBuffer, void * pBuffer2, void * pBuffer3);
	
class I3_EXPORT_FRAMEWORK i3GameInterface
{
public:
	// 게임의 이름 (파일명)
	virtual const char *		GetTitle(void) = 0;

	// 사용자 출력용 게임 이름
	virtual const char *		GetLongTitle(void) = 0;

	// Version
	// High Word : Major version
	// Low Word  : Minor version
	virtual UINT32				GetVersion(void) = 0;			// HALF WORD( H, L)

	// 초기 메모리 점유량
	// 0 : System Default
	virtual UINT32				GetInitialMemSize(void) = 0;

	virtual void				GetVideoConfig( i3VideoInfo * pVideo) = 0;

	// 목표 FPS
	virtual UINT32				GetTargetFPS(void) = 0;

	virtual UINT32				GetInputDeviceMask(void) = 0;

	// 중복 실행되었는지 검사하는 기능을 활성화할 것인지 확인
	//
	virtual bool				GetMultipleInstanceEnable(void) = 0;

	virtual bool				GetMenuKeyEnable(void) = 0;
	virtual bool				GetInitialWindowVisible(void) = 0;


	// Initialize Process Instance
	//
	// Win32 Platform :
	//		pInstHandle : HINSTANCE
	//		pScrHandle	: HWND
	//
	virtual bool				OnInitInstance( void * pInstHandle, void * pScrHandle) = 0;
	virtual void				OnExitInstance(void) = 0;

	virtual void				OnRegisterMeta(void);

	// i3Framework Class Instance를 생성
	// 생성 이외의 작업에 대해선 처리하지 않아야 한다.
	// 이것은 단순히 i3Framework Class에 대한 것만 필요로 하는 Editor 등에서
	// 사용할 때, 실제 Game에서와 같은 초기화를 피할 수 있도록 Control하기 위함이다.
	//
	// 실질적인 초기화 작업은 OnInitFramework() 함수를 이용한다.
	virtual i3Framework *		CreateFramework( i3Viewer * pViewer) = 0;
	
	virtual bool				CreatePreFramework(HWND hwnd) = 0; 
	
	virtual bool				OnInitFramework( i3Framework * pFramework, const char * pszCmdLine) = 0;

	virtual bool				OnDrive( i3Viewer * pViewer);

	virtual void				RegisterScript( lua_State * L){};

#if defined( I3_WINDOWS)
	virtual UINT32				GetViewportStyle(void);
	virtual UINT32				GetIcon(void);

	#if defined( I3_DEBUG)
		virtual INT32			OnQueryPropertyDlgCount(void);
		virtual void			OnQueryPropertyDlg( HINSTANCE hInst, INT32 idx, i3ClassMeta ** ppMeta, i3GameObjPropertyDialog ** ppDlg);
	#endif
#endif

	I3_INTERACT_PROC			s_pInteractProc;
	I3_INTERACT_PROC2			s_pInteractProc2;
	I3_INTERACT_PROC3			s_pInteractProc3;

	void SetInteractProc(I3_INTERACT_PROC proc)					{	s_pInteractProc = proc;			}
	void SetInteractProc2(I3_INTERACT_PROC2 proc)				{	s_pInteractProc2 = proc;		}
	void SetInteractProc3(I3_INTERACT_PROC3 proc)				{	s_pInteractProc3 = proc;		}			

	INT32 CallInteractProc( void * pBuffer)											{ if( s_pInteractProc)	{ return s_pInteractProc( pBuffer); } else return -1;		}
	INT32 CallInteractProc2( void * pBuffer, void * pBuffer2)						{ if( s_pInteractProc2)	{ return s_pInteractProc2( pBuffer, pBuffer2); } else return -1;		}
	INT32 CallInteractProc3( void * pBuffer, void * pBuffer2, void * pBuffer3)		{ if( s_pInteractProc3)	{ return s_pInteractProc3( pBuffer, pBuffer2, pBuffer3); } else return -1;		}
};

//I3_EXTERN_FRAMEWORK
//i3GameInterface *	__stdcall i3CreateGameInterface( const char * pszRunPath);

typedef i3GameInterface * (__stdcall * I3_CREATE_GAME_INTERFACE_PROC)( const char * pszRunPath);
typedef UINT32 (__stdcall * I3_GETVERSION_PROC)(void);

#define		I3_CREATE_GAME_INTERFACE_PROC_NAME			i3CreateGameInterface
#define		I3_CREATE_GAME_INTERFACE_PROC_NAME_STR		"i3CreateGameInterface"

#define		I3_GETVERSION_PROC_NAME						i3Geti3EngineVersion
#define		I3_GETVERSION_PROC_NAME_STR					"i3Geti3EngineVersion"

#endif
