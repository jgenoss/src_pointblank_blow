#ifndef	__CRASH_MANAGER__
#define __CRASH_MANAGER__

enum CRASH_STATE
{
	//여기에 크래쉬 종류 추가해 주시고 쓰시면 됩니다.
	CRASH_NORMAL = 0,
	CRASH_NULL_RESOURCE,
	CRASH_STATE_END
};

struct CrashInfo
{
	i3::vector<i3::wstring>		CrashMessageArray;
	CRASH_STATE					CrashState;

	CrashInfo()
	{
		CrashState = CRASH_NORMAL;
	}

	CrashInfo(CRASH_STATE state, i3::rc_wstring& wstring)
	{
		CrashMessageArray.push_back(wstring);
		CrashState = state;
	}
};

class I3_EXPORT_BASE i3CrashManager : public i3::shared_ginst<i3CrashManager>
{

public:
	i3CrashManager() {}
	~i3CrashManager() {}

private:
	CrashInfo		m_CrashInfo;
public:
	CrashInfo*		GetCrashInfo();
	void			SetCrashInfo(CRASH_STATE state, i3::wstring& wstring);
	void			AddCrashMassage(i3::wstring& wstring);
};

#endif
