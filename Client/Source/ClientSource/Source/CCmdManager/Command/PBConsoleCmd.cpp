#include "pch.h"
#include "PBConsoleCmd.h"

void PBConsoleCmd :: setLoopCount(UINT32 Count)
{
	m_LoopCount = Count;
}
UINT32 PBConsoleCmd :: getLoopCount()
{
	return m_LoopCount;
}

void PBConsoleCmd :: setLogContext(string str)
{
	m_LogContext = str;
}
void PBConsoleCmd :: addLogContext(string str)
{
	m_LogContext += str;
}
string PBConsoleCmd :: getLogContext( )
{
	return m_LogContext;
}
void PBConsoleCmd :: clearLogContext( )
{
	m_LogContext = "";
}
bool PBConsoleCmd :: SaveLogFile( char Path[] , bool AddWrite )
{
	FILE* LogFile = nullptr;
	if(AddWrite)
		LogFile = fopen(Path, "a+");
	else
		LogFile = fopen(Path, "w");
	if(!LogFile)
		return false;

	fprintf(LogFile,"%s",m_LogContext.c_str());

	fclose(LogFile);
	return true;
}