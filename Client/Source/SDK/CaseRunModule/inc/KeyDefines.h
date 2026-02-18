#pragma once

namespace CaseRun
{
	int		getKeyCount(void);
	int		getKeyIndex(const wchar_t * pszKey);
	const wchar_t * getKeyName(int idx);

	int		getWindowsVKey(const wchar_t * pszKey);
	int		getKeyScanCode(const wchar_t * pszKey);

#if defined( WIN32)
	void	Win32_SetKeyPress(int vk, bool bPress);
#endif
}