#include "StdAfx.h"
#include "ShellChangeWatcher.h"

#include "WndProcThunk.h"
#include "i3Base/itl/range/algorithm/replace.h"

// 원본 참고소스는 ChangeNotifyWatcher.zip으로 마이크로소프트 개발홈페이지의 예문소스이다.


UINT I3_WM_SHELL_CHANGE_NOTIFY = ::RegisterWindowMessage(_T("I3_WM_SHELL_CHANGE_NOTIFY"));



HRESULT  CreateAbsPidlFromPath(const wchar_t* wstrPath, PIDLIST_ABSOLUTE& pidl)
{
	LPSHELLFOLDER pDesktopFolder;
	HRESULT hr = SHGetDesktopFolder(&pDesktopFolder);
	if (FAILED(hr))
		return hr;

	// 일단 특별이 문제가 되기 전까지는 const_cast로 쓴다.
	hr = pDesktopFolder->ParseDisplayName(NULL, NULL, const_cast<wchar_t*>(wstrPath), NULL, &pidl, NULL);
	pDesktopFolder->Release();
	return hr;
}

HRESULT AcquireMalloc(LPMALLOC& pMalloc)
{
	return SHGetMalloc(&pMalloc);
}

void    ReleaseMalloc(LPMALLOC& pMalloc)
{
	if (pMalloc) pMalloc->Release();
	pMalloc = NULL;
}

void FreePidl(LPMALLOC& pMalloc, LPITEMIDLIST& pidl)
{
	if (pidl) pMalloc->Free(pidl);
	pidl = NULL;
}

void FreePidl(LPITEMIDLIST& pidl)
{
	if (pidl) ::CoTaskMemFree(pidl);	
	pidl = NULL;
}

bool GetPathFromAbsPidl(PCIDLIST_ABSOLUTE pidl, wchar_t* outstrPath_MAXPATH)
{
	return ::SHGetPathFromIDListW(pidl, outstrPath_MAXPATH) != FALSE;
}

bool GetPathFromAbsPidl(PCIDLIST_ABSOLUTE pidl, char* outstrPath_MAXPATH)
{
	return ::SHGetPathFromIDListA(pidl, outstrPath_MAXPATH) != FALSE;
}

bool GetDisplayNameFromAbsPidl(PCIDLIST_ABSOLUTE pidl, wchar_t* outstrPath_MAXPATH)
{
	SHFILEINFO sfi;

	if (SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
	{
		i3::generic_string_copy(outstrPath_MAXPATH, sfi.szDisplayName);
		return true;
	}

	return false;
}



ShellChangeCallback g_empty_callback;


ShellChangeWatcher::ShellChangeWatcher() : m_ulRegister(0), m_SHCNE_event(SHCNE_ALLEVENTS & ~SHCNE_GLOBALEVENTS ), m_recursive(TRUE), m_hwnd(0), m_oldProc(0), m_callback(&g_empty_callback)
{
	m_thunk.Init(&ShellChangeWatcher::StaticWndProc, this);
}

ShellChangeWatcher::~ShellChangeWatcher()
{
	Stop();
}

HRESULT ShellChangeWatcher::Start(HWND hwnd, const wchar_t* wstrPath)
{
	PIDLIST_ABSOLUTE pidl;

	i3::stack_wstring wstrPath2 = wstrPath;

	i3::range::replace(wstrPath2, L'/', L'\\');

	HRESULT hr = CreateAbsPidlFromPath(wstrPath2.c_str(), pidl);
	if (FAILED(hr)) return hr;

	hr = ShellChangeWatcher::Start(hwnd, pidl);

	FreePidl(pidl);

	return hr;
}

HRESULT ShellChangeWatcher::Start(HWND hwnd, PIDLIST_ABSOLUTE pidl)
{
	Stop();				// 중지동작이 선행..

	SHChangeNotifyEntry const entries[] = { pidl, m_recursive };

	int const nSources = SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery;

	m_ulRegister = SHChangeNotifyRegister(hwnd, nSources, m_SHCNE_event, I3_WM_SHELL_CHANGE_NOTIFY, ARRAYSIZE(entries), entries);

	if (m_ulRegister == 0)
		return E_FAIL;

	m_oldProc = (WNDPROC)::SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR) m_thunk.GetWNDPROC()  );
	m_hwnd = hwnd;

	return S_OK;
}

LRESULT CALLBACK ShellChangeWatcher::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ShellChangeWatcher* pThis = reinterpret_cast<ShellChangeWatcher*>(hwnd);
	return pThis->MemberWndProc(msg, wParam, lParam);
}

// #define SHCNE_GLOBALEVENTS        0x0C0581E0L // Events that dont match pidls first	(첫번째 pidl 인수를 쓰지 않는 이벤트 마스크인듯?)
// #define SHCNE_DISKEVENTS          0x0002381FL // 디스크 전용 이벤트 마스크인듯....(마지막 4비트가 모두채워진걸 보아서, 파일접근되는것 모두 포함..)
// #define SHCNE_EXTENDED_EVENT      0x04000000L // 확장 이벤트.. ( SHCNEE_~ 로 시작하는 이벤트 아이디가 별도로 활용된다. (현재 제외)
/*
#define SHCNE_RENAMEITEM          0x00000001L
#define SHCNE_CREATE              0x00000002L
#define SHCNE_DELETE              0x00000004L
#define SHCNE_MKDIR               0x00000008L
#define SHCNE_RMDIR               0x00000010L
#define SHCNE_MEDIAINSERTED       0x00000020L
#define SHCNE_MEDIAREMOVED        0x00000040L
#define SHCNE_DRIVEREMOVED        0x00000080L
#define SHCNE_DRIVEADD            0x00000100L
#define SHCNE_NETSHARE            0x00000200L
#define SHCNE_NETUNSHARE          0x00000400L
#define SHCNE_ATTRIBUTES          0x00000800L
#define SHCNE_UPDATEDIR           0x00001000L
#define SHCNE_UPDATEITEM          0x00002000L
#define SHCNE_SERVERDISCONNECT    0x00004000L
#define SHCNE_UPDATEIMAGE         0x00008000L
#define SHCNE_DRIVEADDGUI         0x00010000L
#define SHCNE_RENAMEFOLDER        0x00020000L
#define SHCNE_FREESPACE           0x00040000L				
*/

LRESULT ShellChangeWatcher::MemberWndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == I3_WM_SHELL_CHANGE_NOTIFY )
	{
		long lEvent;
		PIDLIST_ABSOLUTE *rgpidl;
		HANDLE hNotifyLock = SHChangeNotification_Lock((HANDLE)wParam, (DWORD)lParam, &rgpidl, &lEvent);

		if (hNotifyLock)
		{
//			TCHAR szPath1[MAX_PATH], szPath2[MAX_PATH];
//			GetPathFromAbsPidl(rgpidl[0], szPath1);
//			GetPathFromAbsPidl(rgpidl[1], szPath2);			// 일단 테스트용도로 문자열 확인.. ( UI 데이터 변경에는 pidl,path중 어느것을 쓸지 생각해봐야함)

			bool bIsItemNotificationEvent =
				!(lEvent & ( SHCNE_UPDATEIMAGE | SHCNE_ASSOCCHANGED | SHCNE_EXTENDED_EVENT | SHCNE_FREESPACE | SHCNE_DRIVEADDGUI | SHCNE_SERVERDISCONNECT));

			if (bIsItemNotificationEvent)
			{

				long lEventCase = lEvent & SHCNE_DISKEVENTS;

				switch( lEventCase )
				{
				case SHCNE_RENAMEITEM:
					m_callback->OnRename_File(rgpidl[0], rgpidl[1]);	
					break;

				case SHCNE_CREATE:
					m_callback->OnCreate_File(rgpidl[0]);					
					break;
				case SHCNE_DELETE:
					m_callback->OnDelete_File(rgpidl[0]);
					break;
				case SHCNE_MKDIR:
					m_callback->OnCreate_Directory(rgpidl[0]);				
					break;
				case SHCNE_RMDIR:
					m_callback->OnDelete_Directory(rgpidl[0]);				
					break;
				case SHCNE_RENAMEFOLDER:
					m_callback->OnRename_Directory(rgpidl[0], rgpidl[1]);	
					break;
					//
				case SHCNE_UPDATEDIR:
					m_callback->OnUpdate_Directory(rgpidl[0]);				
					break;
				case SHCNE_UPDATEITEM:
					m_callback->OnUpdate_File(rgpidl[0]);	
					break;		// 아이콘오버레이업데이트의 경우, 해당 어플리케이션 구현에 의해서만 적용됨.
				default:
					break;
				}		
				m_callback->OnGeneral(lEvent, rgpidl[0], rgpidl[1]);
			}

			SHChangeNotification_Unlock(hNotifyLock);
		}

	}

	return ::CallWindowProc(m_oldProc, m_hwnd, msg, wParam, lParam);
}


void	ShellChangeWatcher::Stop()
{
	if (m_ulRegister)
	{
		SHChangeNotifyDeregister(m_ulRegister);
		m_ulRegister = 0;
	}

	if (m_hwnd)
	{
		::SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_oldProc);
		m_hwnd = 0;
	}
}


