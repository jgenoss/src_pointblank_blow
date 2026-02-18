#ifndef __LOGFILE_H__
#define __LOGFILE_H__

class CLogFile : public i3ElementBase
{
	I3_CLASS_DEFINE( CLogFile ); 
private:
	i3Mutex				m_csWrite;		
	HANDLE				m_hFile;
	INT32				m_Hour; 
protected:
	BOOL FileOpen(char * strFileName);
public: 
	CLogFile(void); 
	virtual ~CLogFile(void); 

	BOOL OnCreate(char * strFileName); 
	void OnUpdate(void);			
	void OnDestroy(void); 
	void WriteLog( char * pLogString );
};

extern CLogFile * g_pLogFile;

#endif
