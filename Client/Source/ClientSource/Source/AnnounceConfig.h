#ifndef _AnnounceConfig_h
#define _AnnounceConfig_h

#define MAX_ANNOUNCE_COUNT		6

struct AnnounceInfo
{
	char _name[MAX_STRING_COUNT] = { 0 };
	char _url[MAX_STRING_COUNT] = { 0 };
	char _path[MAX_PATH] = { 0 };
	BOOL _scroll = FALSE;
};

class CAnnounceConfig: public i3ElementBase
{
	I3_CLASS_DEFINE(CAnnounceConfig, i3ElementBase);
public:
	CAnnounceConfig();
	virtual ~CAnnounceConfig();

	bool Create(void);

	INT32 GetCount(void) const;
	const char* GetName(INT32 idx) const;
	const char* GetURL(INT32 idx) const;
	const char* GetPath(INT32 idx) const;
	bool IsScroll(INT32 idx) const;

private:
	INT32 m_announceCount;
	AnnounceInfo m_announceInfo[MAX_ANNOUNCE_COUNT];
};

#endif
