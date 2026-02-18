
#ifndef __PB_LOG_H
#define __PB_LOG_H

#if 0

#include <queue>
#include <string>

enum PB_LOG_TYPE
{
	PLT_MEOMORY,
	PLT_DEBUG_WINDOW
};

class CPBLog : public i3ElementBase 
{
	I3_CLASS_DEFINE( CPBLog , i3ElementBase);
private:
	char			m_szFilename[MAX_PATH];
	FILE*			m_pFile;
	std::queue<std::string> m_strQueue;
	PB_LOG_TYPE m_ePLT;

public:
	CPBLog();
	virtual ~CPBLog();

	void		SetFileName(LPSTR szFilename);
	void		Log( LPSTR fmt, ... );


	#if defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)
		inline void CPBLog::Log(REAL32 rTime, LPSTR fmt, ... )
		{
			char buff[1024];
			char szTemp[1024];

			vsprintf_s( buff, fmt, (char *)(&fmt+1) );
			sprintf_s(szTemp, "%f %s", rTime, buff);
			
			Log(szTemp);
		}
	#else
		inline void CPBLog::Log(REAL32 rTime, LPSTR fmt, ... )
		{
		}
	#endif

	void		WriteToFile();
};

extern  CPBLog*		g_pLog;

#endif

#endif // __PB_LOG_H

