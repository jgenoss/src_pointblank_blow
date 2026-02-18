
#include "stdafx.h"


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Windows 헤더에서 거의 사용되지 않는 내용을 제외시킵니다.
#endif

// 아래 지정된 플랫폼보다 우선하는 플랫폼을 대상으로 하는 경우 다음 정의를 수정하십시오.
// 다른 플랫폼에 사용되는 해당 값의 최신 정보는 MSDN을 참조하십시오.
#ifndef WINVER				// Windows 95 및 Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
#define WINVER 0x0600		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_WINNT		// Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_WINNT 0x0600		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_WINDOWS 0x0410 // Windows Me 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_IE			// IE 4.0 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_IE 0x0700	// IE 5.0 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#include <windows.h>
#include <CommCtrl.h>
#include <ShellAPI.h>

#pragma comment (lib, "comctl32.lib")
#pragma comment (lib, "shell32.lib")
#pragma comment (lib, "gdi32.lib")

#include <vector>
#include <map>
#include <list>
#include <algorithm>

#include "AnimOpt.h"
#include "AnimOptGUI.h"

#pragma warning (disable :4503)


