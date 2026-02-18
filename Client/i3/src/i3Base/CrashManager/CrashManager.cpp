#include "i3CommonType.h"
#include "i3Base/CrashManager/CrashManager.h"

CrashInfo* i3CrashManager::GetCrashInfo()
{
	return &m_CrashInfo;
}

void i3CrashManager::SetCrashInfo(CRASH_STATE state, i3::wstring& wstring)
{
	m_CrashInfo.CrashState = state;
	m_CrashInfo.CrashMessageArray.clear();
	m_CrashInfo.CrashMessageArray.push_back(wstring);
}

void	i3CrashManager::AddCrashMassage(i3::wstring& wstring)
{
	m_CrashInfo.CrashMessageArray.push_back(wstring);
}
