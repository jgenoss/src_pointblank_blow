#include <windows.h>
#include <Guiddef.h>
#include <shlobj.h>                 // For SHChangeNotify
#include "ClassFactory.h"
#include "Reg.h"

// {9EDDE056-7440-4140-A5A4-11739CD13488}
static const GUID GUID_i3ImageShellExtension =
{ 0x9edde056, 0x7440, 0x4140, { 0xa5, 0xa4, 0x11, 0x73, 0x9c, 0xd1, 0x34, 0x88 } };


HINSTANCE   g_hInst = NULL;
long        g_cDllRef = 0;

bool APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Hold the instance of this DLL module, we will use it to get the 
		// path of the DLL to register the component.
		g_hInst = hModule;
		DisableThreadLibraryCalls(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return true;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
	HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

	if (IsEqualCLSID(GUID_i3ImageShellExtension, rclsid))
	{
		hr = E_OUTOFMEMORY;

		ClassFactory *pClassFactory = new ClassFactory();
		if (pClassFactory)
		{
			hr = pClassFactory->QueryInterface(riid, ppv);
			pClassFactory->Release();
		}
	}

	return hr;
}

STDAPI DllCanUnloadNow(void)
{
	return g_cDllRef > 0 ? S_FALSE : S_OK;
}

STDAPI DllRegisterServer(void)
{
	HRESULT hr;

	wchar_t szModule[MAX_PATH];
	if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

	// Register the component.
	hr = RegisterInprocServer(szModule, GUID_i3ImageShellExtension,
		L"CppShellExtThumbnailHandler.i3iThumbnailProvider Class",
		L"Apartment");
	if (SUCCEEDED(hr))
	{
		// Register the thumbnail handler. The thumbnail handler is associated
		// with the .recipe file class.
		hr = RegisterShellExtThumbnailHandler(L".i3i",
			GUID_i3ImageShellExtension);

		hr = RegisterShellExtThumbnailHandler(L".dds",
			GUID_i3ImageShellExtension);

		if (SUCCEEDED(hr))
		{
			// This tells the shell to invalidate the thumbnail cache. It is 
			// important because any .recipe files viewed before registering 
			// this handler would otherwise show cached blank thumbnails.
			SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
		}
	}

	return hr;
}

STDAPI DllUnregisterServer(void)
{
	HRESULT hr = S_OK;

	wchar_t szModule[MAX_PATH];
	if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

	// Unregister the component.
	hr = UnregisterInprocServer(GUID_i3ImageShellExtension);
	if (SUCCEEDED(hr))
	{
		// Unregister the thumbnail handler.
		hr = UnregisterShellExtThumbnailHandler(L".i3i");

		hr = UnregisterShellExtThumbnailHandler(L".dds");
	}

	return hr;
}





