#ifndef _CServerIPList_h
#define _CServerIPList_h

class CServerIPList: public i3ElementBase
{
	I3_CLASS_DEFINE( CServerIPList, i3ElementBase);
public:
	CServerIPList();
	virtual ~CServerIPList();

	void InitIPString(void);
	bool Load(void);
	INT32 GetCount(void);
	const char* GetIP(INT32 idxIP);
	const char* GetCurrentIP(void);
	INT32 GetCurrentIPIdx(void) { return m_listCurrentIP; }
	void SetCurrentIPIdx(INT32 idx) { g_pEnvSet->SetDefaultNation(idx); m_listCurrentIP = idx;	}
	
	char* GetCompareIP() { return m_ServerCompare;};

private:
	void _RemoveInvalidIP(void);

private:
	i3::vector<i3::rc_string>	m_listIPString;	
	INT32						m_listCurrentIP;

public:
	char	m_strUseServerSet[MAX_STRING_COUNT * 2];
	char	m_ServerCompare[MAX_STRING_COUNT * 2];
};

#endif //_CServerIPList_h
