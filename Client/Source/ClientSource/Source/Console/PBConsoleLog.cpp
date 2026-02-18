
#include "pch.h"
#include "PBConsoleLog.h"


//
// PBCSLogItem
//

const PBCSLogItem::LogTextAttr& PBCSLogItem::GetDefaultAttr()
{	
	static PBCSLogItem::LogTextAttr defaultAttr;
	static bool reset = false;

	if(!reset)
	{
		reset = true;
		defaultAttr.Reset();
	}

	return defaultAttr;
}

void PBCSLogItem::LogTextAttr::Reset()
{
	m_fmtFlag = EFMTF_SUFFIX_LINEFEED;
	m_color[0] = 0xff;	m_color[1] = 0xff;	m_color[2] = 0xff;	m_color[3] = 0xff;
}

PBCSLogItem::PBCSLogItem(const wchar_t* str)
{
	if(!str)
		str = L"  ";

	GetLocalTime(&m_time);

	m_text = str;
	m_attr = GetDefaultAttr();
}

PBCSLogItem::PBCSLogItem(LogTextAttr* attr, const wchar_t* str)
{
	if(!str)
		str = L" ";

	GetLocalTime(&m_time);

	m_text = str;
	m_attr = attr ? *attr : GetDefaultAttr();
}

const wchar_t* PBCSLogItem::GetFormattedStr(wchar_t* buf, INT32 siz)
{
	wchar_t* st = buf;

	if(m_attr.m_fmtFlag & PBCSLogItem::LogTextAttr::EFMTF_PREFIX_LINEFEED)
	{		
		*buf = L'\n';
		buf++; siz--;
	}

	swprintf_s(buf, siz, L"[%02d-%02d-%02d:%02d:%02d]{col:%d,%d,%d,%d} %s{/col}", 
		m_time.wMonth, m_time.wDay, m_time.wHour, m_time.wMinute, m_time.wSecond,
		m_attr.m_color[0], m_attr.m_color[1], m_attr.m_color[2], m_attr.m_color[3], 
		GetRawStr());

	if(m_attr.m_fmtFlag & PBCSLogItem::LogTextAttr::EFMTF_SUFFIX_LINEFEED)
	{
		while(*buf)
		{
			buf++; 
			siz--;
		}

		if(siz > 0)
		{
			*buf++ = L'\n';
			*buf   = 0;
		}
	}

	return st;
}


//
// PBConsoleLog
//

PBConsoleLog::PBConsoleLog()
{
	m_dirtyFlag = 0;
	m_textRev = 0;
	m_logTextAttr = PBCSLogItem::GetDefaultAttr();
}

PBConsoleLog::~PBConsoleLog()
{
	Reset();
}

void PBConsoleLog::ResetLogAttrib()
{
	m_logTextAttr.Reset();
	
	while(!m_logTextAttrStack.empty())
		m_logTextAttrStack.pop();
}

void PBConsoleLog::SaveAttrib()
{
	m_logTextAttrStack.push(m_logTextAttr);
}

void PBConsoleLog::RestoreAttrib()
{
	if(m_logTextAttrStack.empty())
		return;

	m_logTextAttr = m_logTextAttrStack.top();
	m_logTextAttrStack.pop();
}

void PBConsoleLog::SetLogColorAttr(BYTE r,BYTE g, BYTE b)
{
	m_logTextAttr.m_color[0] = r;
	m_logTextAttr.m_color[1] = g;
	m_logTextAttr.m_color[2] = b;
	m_logTextAttr.m_color[3] = 0xff;
}

void PBConsoleLog::SetLogLineFeedAttr(bool suffixLineFeed, bool prefixLineFeed)
{
	DWORD mask = PBCSLogItem::LogTextAttr::EFMTF_PREFIX_LINEFEED | 
				 PBCSLogItem::LogTextAttr::EFMTF_SUFFIX_LINEFEED;

	DWORD newFlag = 0;

	if(prefixLineFeed)
		newFlag |= PBCSLogItem::LogTextAttr::EFMTF_PREFIX_LINEFEED;

	if(suffixLineFeed)
		newFlag |= PBCSLogItem::LogTextAttr::EFMTF_SUFFIX_LINEFEED;

	m_logTextAttr.m_fmtFlag &= ~mask;
	m_logTextAttr.m_fmtFlag |= newFlag;
}

void PBConsoleLog::AddLog(const wchar_t* fmt, ...)
{
	wchar_t buf[1024] = { 0 };
	
	va_list arg; va_start(arg, fmt);

	vswprintf(buf, fmt, arg); va_end(arg);
	AddLogItem(new PBCSLogItem(&m_logTextAttr, buf));
}

void PBConsoleLog::AddLog(BYTE r, BYTE g, BYTE b, const wchar_t* fmt, ...)
{
	wchar_t buf[1024] = { 0 };
	
	va_list arg; va_start(arg, fmt);

	vswprintf(buf, fmt, arg); va_end(arg);
	PBCSLogItem* item = new PBCSLogItem(&m_logTextAttr, buf);

	item->m_attr.m_color[0] = r;
	item->m_attr.m_color[1] = g;
	item->m_attr.m_color[2] = b;

	AddLogItem(item);
}


void PBConsoleLog::AddLineFeed()
{
	AddLog(L" ");
}

const wchar_t* PBConsoleLog::GetFullLogText(DWORD* rstTextRev)
{
	ClearDirty();

	if(rstTextRev)
		*rstTextRev = m_textRev;

	if(m_text.empty())
		return L"  ";

	const wchar_t* rst = m_text.c_str();

	return rst ? rst : L"  ";
}

void PBConsoleLog::Reset()
{
	LogQueue::iterator x = m_logQ.begin();
	while(x != m_logQ.end())
	{
		delete *x;
		x++;
	}

	m_logQ.clear();
	m_text.clear();

	m_dirtyFlag = 0;
	m_textRev = 0;
}

INT32 PBConsoleLog::GetCount()
{
	return (INT32)m_logQ.size();
}

const wchar_t* PBConsoleLog::GetLogText(INT32 index)
{
	if(index >= (INT32)m_logQ.size())
		return L"  ";

	PBCSLogItem* item = m_logQ[index];

	return item->GetRawStr();
}

void PBConsoleLog::ClearDirty()
{
	if(!m_dirtyFlag)
		return;

	if(m_dirtyFlag & EDF_DIRTY_LOGTEXT)
		MergeLogIntoText();

	m_dirtyFlag = 0;
}

void PBConsoleLog::MergeLogIntoText()
{
	m_text.clear();

	LogQueue::iterator x = m_logQ.begin();

	wchar_t tmpBuf[1024];

	while(x != m_logQ.end())
	{
		PBCSLogItem* cur = (*x);
		m_text += cur->GetFormattedStr(tmpBuf, 1024);
		x++;
	}
}

void PBConsoleLog::AddLogItem(PBCSLogItem* item)
{
	if(!item)
		return;

	INT32 numLog = (INT32)m_logQ.size();

	if(numLog > (LOG_ROW_MAX+LOG_ROW_EXCEED_TOKEEP))
	{
		INT32 numPop = (numLog - LOG_ROW_MAX);

		for(INT32 i=0; i<numPop; i++)
			m_logQ.pop_front();
	}

	m_logQ.push_back(item);

	m_textRev++;
	if(m_textRev == 0)
		m_textRev++;

	SetDirty(EDF_DIRTY_LOGTEXT);
	
//	OutputDebugStringW(item->GetRawStr());
//	OutputDebugStringW(L"\n");
}

void PBConsoleLog::StripColorTag(wchar_t* inout, INT32 bufSiz)
{
	if(!inout)
		return;

	wchar_t* cur = inout;

	while(cur)
	{
		wchar_t* p = wcsstr(cur, L"{col:");
		if(!p)
			break;

		wchar_t* e = wcschr(p+5, L'}');

		if(e)
		{
			wchar_t* p2 = wcsstr(e+1, L"{/col}");

			if(p2)
			{
				wchar_t* src = p2+6; 
				cur = src;
				wchar_t* dst = p+5;
				
				while(*src)
					*dst++ = *src++;

				*dst = 0;
			}
			else
				break;
		}		
	}
}

bool PBConsoleLog::SaveLogToFile(const wchar_t* path)
{
	wchar_t row[2048] = {0};

	FILE* fp = _wfopen(path, L"wt");

	if(!fp)
		return false;

	for(size_t si=0; si<m_logQ.size(); si++)
	{
		wcscpy_s(row, 2048, m_logQ[si]->GetRawStr());
//		StripColorTag(row, 2048);

		SYSTEMTIME& st = m_logQ[si]->m_time;
		fwprintf(fp, L"[%02d-%02d-%02d:%02d:%02d] %s \n", 
			st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
			m_logQ[si]->GetRawStr());
	}

	fclose(fp);	

	return true;
}
