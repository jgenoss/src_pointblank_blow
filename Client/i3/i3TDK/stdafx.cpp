// stdafx.cpp : source file that includes just the standard includes
// i3TDK.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


CString		ToMFCString(const char* sz)
{
	if (sz == nullptr) return CString();
#ifdef _UNICODE
	i3::stack_wstring wstr;	i3::mb_to_utf16(sz, wstr);
	return CString(wstr.c_str(), wstr.size());
#else
	return CString(sz);
#endif
}

CString		ToMFCString(const wchar_t* sz)
{
	if (sz == nullptr) return CString();
#ifdef _UNICODE
	return CString(sz);
#else
	i3::stack_string str;	i3::utf16_to_mb(sz, str);
	return CString(str.c_str(), str.size());
#endif
}

#ifdef _UNICODE
i3::wstring		ToTString(const char* sz)
{
	if (!sz) return i3::wstring();

	i3::wstring tmp;		i3::mb_to_utf16(sz, tmp);
	return tmp;
}

i3::wliteral_range		ToTString(const wchar_t* sz)
{
	if (!sz) return i3::wliteral_range();
	return i3::wliteral_range(sz);
}
#else
i3::literal_range		ToTString(const char* sz)
{
	if (!sz) return i3::literal_range();
	return i3::literal_range(sz);
}

i3::string		ToTString(const wchar_t* sz)
{
	if (!sz) return i3::string();
	i3::string tmp;	i3::utf16_to_mb(sz, tmp);
	return tmp;
}

#endif

#ifdef _UNICODE

LOGFONTW			ToTLOGFONT(LOGFONTA& lf)
{
	LOGFONTW lf2;
	lf2.lfHeight			= lf.lfHeight;	
	lf2.lfWidth				= lf.lfWidth;
	lf2.lfEscapement		= lf.lfEscapement;
	lf2.lfOrientation		= lf.lfOrientation;
	lf2.lfWeight			= lf.lfWeight;
	lf2.lfItalic			= lf.lfItalic;
	lf2.lfUnderline			= lf.lfUnderline;
	lf2.lfStrikeOut			= lf.lfStrikeOut;
	lf2.lfCharSet			= lf.lfCharSet;
	lf2.lfOutPrecision		= lf.lfOutPrecision;
	lf2.lfClipPrecision		= lf.lfClipPrecision;
	lf2.lfQuality			= lf.lfQuality;
	lf2.lfPitchAndFamily	= lf.lfPitchAndFamily;
	
	::memset(lf2.lfFaceName, 0, sizeof(lf2.lfFaceName) );
	i3::mb_to_utf16(lf.lfFaceName, lf2.lfFaceName, _countof(lf2.lfFaceName));
	
	return lf2;
}

LOGFONTW&		ToTLOGFONT(LOGFONTW& lf)
{
	return lf;
}

LOGFONTW&		ToLOGFONTW(LOGFONTW& lf)
{
	return lf;
}

LOGFONTA			ToLOGFONTA(LOGFONTW& lf)
{
	LOGFONTA lf2;
	lf2.lfHeight			= lf.lfHeight;	
	lf2.lfWidth				= lf.lfWidth;
	lf2.lfEscapement		= lf.lfEscapement;
	lf2.lfOrientation		= lf.lfOrientation;
	lf2.lfWeight			= lf.lfWeight;
	lf2.lfItalic			= lf.lfItalic;
	lf2.lfUnderline			= lf.lfUnderline;
	lf2.lfStrikeOut			= lf.lfStrikeOut;
	lf2.lfCharSet			= lf.lfCharSet;
	lf2.lfOutPrecision		= lf.lfOutPrecision;
	lf2.lfClipPrecision		= lf.lfClipPrecision;
	lf2.lfQuality			= lf.lfQuality;
	lf2.lfPitchAndFamily	= lf.lfPitchAndFamily;

	::memset(lf2.lfFaceName, 0, sizeof(lf2.lfFaceName) );
	i3::utf16_to_mb(lf.lfFaceName, lf2.lfFaceName, _countof(lf2.lfFaceName));
	return lf2;
}


#else

LOGFONTA&		ToTLOGFONT(LOGFONTA& lf)
{
	return lf;
}

LOGFONTA			ToTLOGFONT(LOGFONTW& lf)
{
	LOGFONTA lf2;
	lf2.lfHeight			= lf.lfHeight;	
	lf2.lfWidth				= lf.lfWidth;
	lf2.lfEscapement		= lf.lfEscapement;
	lf2.lfOrientation		= lf.lfOrientation;
	lf2.lfWeight			= lf.lfWeight;
	lf2.lfItalic			= lf.lfItalic;
	lf2.lfUnderline			= lf.lfUnderline;
	lf2.lfStrikeOut			= lf.lfStrikeOut;
	lf2.lfCharSet			= lf.lfCharSet;
	lf2.lfOutPrecision		= lf.lfOutPrecision;
	lf2.lfClipPrecision		= lf.lfClipPrecision;
	lf2.lfQuality			= lf.lfQuality;
	lf2.lfPitchAndFamily	= lf.lfPitchAndFamily;

	::memset(lf2.lfFaceName, 0, sizeof(lf2.lfFaceName) );
	i3::utf16_to_mb(lf.lfFaceName, lf2.lfFaceName, _countof(lf2.lfFaceName));
	return lf2;
}

LOGFONTW			ToLOGFONTW(LOGFONTA& lf)
{
	LOGFONTW lf2;
	lf2.lfHeight			= lf.lfHeight;	
	lf2.lfWidth				= lf.lfWidth;
	lf2.lfEscapement		= lf.lfEscapement;
	lf2.lfOrientation		= lf.lfOrientation;
	lf2.lfWeight			= lf.lfWeight;
	lf2.lfItalic			= lf.lfItalic;
	lf2.lfUnderline			= lf.lfUnderline;
	lf2.lfStrikeOut			= lf.lfStrikeOut;
	lf2.lfCharSet			= lf.lfCharSet;
	lf2.lfOutPrecision		= lf.lfOutPrecision;
	lf2.lfClipPrecision		= lf.lfClipPrecision;
	lf2.lfQuality			= lf.lfQuality;
	lf2.lfPitchAndFamily	= lf.lfPitchAndFamily;

	::memset(lf2.lfFaceName, 0, sizeof(lf2.lfFaceName) );
	i3::mb_to_utf16(lf.lfFaceName, lf2.lfFaceName, _countof(lf2.lfFaceName));

	return lf2;
}

LOGFONTA&		ToLOGFONTA(LOGFONTA& lf)
{
	return lf;
}


#endif


/*
namespace 
{
	i3::tls_ptr<i3::stack_string>	g_pStackString;
	i3::tls_ptr<i3::stack_wstring>	g_pStackWString;
}


const TCHAR*	ToTCHARPointer(const char* sz)
{
#ifdef _UNICODE
	if (sz == nullptr) return nullptr;
	i3::stack_wstring* pwstr = g_pStackWString.get();
	i3::mb_to_utf16(sz, *pwstr);
	return (*pwstr).c_str();
#else
	return sz;
#endif
}

const TCHAR*	ToTCHARPointer(const wchar_t* sz)
{
#ifdef _UNICODE
	return sz;
#else
	if (sz == nullptr) return nullptr;
	i3::stack_string* pstr = g_pStackString.get();
	i3::utf16_to_mb(sz, *pstr);
	return (*pstr).c_str();
#endif
}

const char*		ToCharPointer(const TCHAR* sz)
{
#ifdef _UNICODE
	if (sz == nullptr) return nullptr;
	i3::stack_string* pstr = g_pStackString.get();
	i3::utf16_to_mb(sz, *pstr);
	return (*pstr).c_str();
#else
	return sz;
#endif
}

const wchar_t*	ToWideCharPointer(const TCHAR* sz)
{
#ifdef _UNICODE
	return sz;
#else
	if (sz == nullptr) return nullptr;
	i3::stack_wstring* pwstr = g_pStackWString.get();
	i3::mb_to_utf16(sz, *pwstr);
	return (*pwstr).c_str();
#endif
}
*/
namespace
{
	HMODULE GetCurrentModule()
	{
		HMODULE hModule = nullptr;
		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)GetCurrentModule, &hModule) != 0)
		{
			return hModule;
		}
		else return nullptr;
	}

	struct ModuleInitializer
	{
		ModuleInitializer()
		{
			HMODULE hModule = GetCurrentModule();
			
			i3System::HoldDllModule(hModule);
		}

	} _ModuleInitializer_;

}

/*
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

*/
