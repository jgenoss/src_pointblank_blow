
#include "i3TdkAgent.h"


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Windows 헤더에서 거의 사용되지 않는 내용을 제외시킵니다.
#endif

// 아래 지정된 플랫폼보다 우선하는 플랫폼을 대상으로 하는 경우 다음 정의를 수정하십시오.
// 다른 플랫폼에 사용되는 해당 값의 최신 정보는 MSDN을 참조하십시오.
#ifndef WINVER				// Windows 95 및 Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
#define WINVER 0x0501		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_WINNT		// Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_WINNT 0x0501		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_WINDOWS 0x0410 // Windows Me 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_IE			// IE 4.0 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_IE 0x0700	// IE 5.0 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소
#include <afxext.h>         // MFC 익스텐션
#include <afxdisp.h>        // MFC 자동화 클래스

#include <afxdtctl.h>		// Internet Explorer 4 공용 컨트롤에 대한 MFC 지원
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 공용 컨트롤에 대한 MFC 지원
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>
#include "../i3TDK/Resource.h"

#include "i3Base.h"
#include "i3Gfx.h"
#include "i3Math.h"
#include "i3Scene.h"
#include "i3SceneUtil.h"
#include "i3Framework.h"
#include "i3Opt.h"

#if defined( I3_WINDOWS) && defined(I3_DLL)
	#if defined( I3_TDK_BUILD)
		#define I3_EXPORT_TDK		__declspec( dllexport)
		#define I3_EXTERN_TDK		extern
	#else
		#define I3_EXPORT_TDK		__declspec( dllimport)
		#define I3_EXTERN_TDK		__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_TDK)
		#define	I3_EXPORT_TDK
		#define I3_EXTERN_TDK		extern
	#endif
#endif

#define		CHECKIO( Rc)				\
	if( Rc == STREAM_ERR)\
	{\
		i3Log( __FUNCDNAME__, "Line:%d", __LINE__);\
		return STREAM_ERR;\
	}\
	Result += Rc;


I3_EXTERN_TDK	HINSTANCE				g_hInstTDK;


#include "../i3TDK/i3TDKMeta.h"

#include "../i3TDK/i3TDKOptSpecs.h"
#include "../i3TDK/i3TDKOptSpecMipmapGen.h"
#include "../i3TDK/i3TDKSpec_ShapeSet.h"


static bool							gs_i3TDK_Init = false;
static HMODULE						gs_hostDllModule = NULL;
static AFX_EXTENSION_MODULE			gs_afx_ext_module;
static CWinApp*						gs_theApp = NULL;


//
// Init & Finalize for MFC Extension DLL
//

bool i3TDK_Init(void* hModule)
{
	if(gs_i3TDK_Init)
		return true;

	gs_hostDllModule = (HMODULE)hModule;

	//
	//  http://msdn.microsoft.com/en-us/library/1btd5ea3.aspx
	//

	// Extension DLL one-time initialization
    if(AfxInitExtensionModule(gs_afx_ext_module, gs_hostDllModule) == 0)
        return false;

	// Insert this DLL into the resource chain
    new CDynLinkLibrary(gs_afx_ext_module);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	gs_theApp = new CWinApp; // CObject초기화를 위하여
	gs_theApp->InitInstance();

	gs_theApp->m_hInstance = gs_afx_ext_module.hModule;

	i3TDKRegisterMetas();

	gs_i3TDK_Init = true;

	return true;
} 

void i3TDK_Finalize()
{
	if(!gs_i3TDK_Init)
		return;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(gs_theApp)
	{
		gs_theApp->ExitInstance();

		delete gs_theApp;
		gs_theApp = NULL;
	}

//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	AfxTermExtensionModule(gs_afx_ext_module);

	gs_i3TDK_Init = false;
}
 

//
// 
//

bool i3TDK_SceneOptimizer_DoModalOption(i3SceneOptimizer* optimizer)
{
	if(!optimizer)
		return false;

	// 2013.8.19기준 i3SceneOptimizer계열엔 CopyTo 미구현
	bool rst = false;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	rst = i3TDK::PopupOptimizerSpecDialog(optimizer, NULL);

	return rst;	
}
