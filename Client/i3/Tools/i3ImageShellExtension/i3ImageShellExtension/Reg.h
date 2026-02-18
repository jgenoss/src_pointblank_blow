
#pragma once

#include <windows.h>

HRESULT RegisterInprocServer(PCWSTR pszModule, const CLSID& clsid, PCWSTR pszFriendlyName, PCWSTR pszThreadModel);
HRESULT UnregisterInprocServer(const CLSID& clsid);
HRESULT RegisterShellExtThumbnailHandler(PCWSTR pszFileType, const CLSID& clsid);
HRESULT UnregisterShellExtThumbnailHandler(PCWSTR pszFileType);