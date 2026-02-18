
#ifndef _PBCONSOLELOG_H_
#define _PBCONSOLELOG_H_

#include <string>
#include <deque>
#include <stack>


class PBCSLogItem
{
public:
	class LogTextAttr
	{
	public:
		enum EFMT_FLAG
		{
			EFMTF_PREFIX_LINEFEED			= 0x00000001,  // 로그텍스트 앞쪽에 개행 \n을 넣어줍니다.
			EFMTF_SUFFIX_LINEFEED			= 0x00000002,  // 로그텍스트 뒤쪽에 개행 \n
		};

		DWORD						m_fmtFlag;
		BYTE						m_color[4];

		void Reset();
	};

	explicit PBCSLogItem(const wchar_t* str);
	explicit PBCSLogItem(LogTextAttr* attr, const wchar_t* str);

	const wchar_t* GetRawStr() { return m_text.empty() ? L"  " : m_text.c_str(); }
	const wchar_t* GetFormattedStr(wchar_t* buf, INT32 siz);

	static const LogTextAttr& GetDefaultAttr();

	SYSTEMTIME						m_time;
	LogTextAttr						m_attr;
	std::wstring					m_text;
};


class PBConsoleLog
{
public:
	enum 
	{ 
		LOG_ROW_MAX				= 1000,
		LOG_ROW_EXCEED_TOKEEP	= 50,   // 삭제시작 : 현재로우 > LOG_ROW_MAX + LOG_ROW_EXCEED_TOKEEP
	};

	enum EDIRTYFLAG
	{
		EDF_DIRTY_LOGTEXT				= 0x00000001,	
	};

	PBConsoleLog();
	virtual ~PBConsoleLog();

		// LogText Attribute Start
	void ResetLogAttrib();
	void SaveAttrib();
	void RestoreAttrib();

	void SetLogColorAttr(BYTE r=0xff,BYTE g=0xff, BYTE b=0xff);
	void SetLogLineFeedAttr(bool suffixLineFeed=true, bool prefixLineFeed=false);
		// LogText Attribute End

	void Reset();

	void AddLog(BYTE r, BYTE g, BYTE b, const wchar_t* fmt, ...);
	void AddLog(const wchar_t* fmt, ...);
	void AddLineFeed();

	INT32 GetCount();
	const wchar_t* GetFullLogText(DWORD* rstTextRev= nullptr);
	DWORD GetFullLogTextRev() { return m_textRev; }
	const wchar_t* GetLogText(INT32 index);

	void SetDirty(DWORD flag) { m_dirtyFlag |= flag; }
	void ClearDirty();

	bool SaveLogToFile(const wchar_t* path);

	static void StripColorTag(wchar_t* inout, INT32 bufSiz);

protected:
	void MergeLogIntoText();
	void AddLogItem(PBCSLogItem* item);

private:
	DWORD											m_dirtyFlag;

	typedef std::deque<PBCSLogItem*>				LogQueue;
	LogQueue										m_logQ;

	std::wstring									m_text;
	DWORD											m_textRev;

	PBCSLogItem::LogTextAttr						m_logTextAttr;
	std::stack<PBCSLogItem::LogTextAttr>			m_logTextAttrStack;
};



#endif
