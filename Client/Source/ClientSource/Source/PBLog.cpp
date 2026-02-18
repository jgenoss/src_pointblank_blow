#include "pch.h"

#if 0

#include "PBLog.h"
#include <time.h>

I3_CLASS_INSTANCE( CPBLog);//, i3ElementBase);

CPBLog::CPBLog()
{
	m_pFile = nullptr;
	m_ePLT = PLT_MEOMORY;
	m_szFilename[0] = 0;
}

CPBLog::~CPBLog()
{
	while(!m_strQueue.empty()) {
		m_strQueue.pop();
	}
}

void CPBLog::SetFileName(LPSTR szFilename)
{
	i3::safe_string_copy( m_szFilename, szFilename, MAX_PATH );
}



void CPBLog::Log( LPSTR fmt, ... )
{
	char	buff[1024];
	std::string strBuffer;

	vsprintf_s( buff, fmt, (char*)(&fmt+1) );
	
	if(m_ePLT == PLT_MEOMORY) 
	{
		if(m_strQueue.size() > 22048) {
		// if(m_strQueue.size() > 20) {
			m_strQueue.pop();
		}
		strBuffer = buff;
		m_strQueue.push(strBuffer);
	}
	else if(m_ePLT == PLT_DEBUG_WINDOW) 
	{
		I3PRINTF(buff);
	}
}

void CPBLog::WriteToFile()
{
	std::string strBuffer;

	m_pFile = fopen( m_szFilename, "a" );

	I3ASSERT(m_pFile != nullptr);

	while(!m_strQueue.empty()) {
		strBuffer = m_strQueue.front();
		fprintf( m_pFile, "%s", strBuffer.c_str() );
		m_strQueue.pop();
	}
	
	fclose( m_pFile );
}
#endif