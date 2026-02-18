#pragma once
#include "LogBase.h"

class CLogFile : public CLogBase
{
	i3::string			m_pszWorkBase;
	i3::string			m_pszName;
	INT32				m_nBeginDate;
	INT32				m_nEndDate;

	bool				m_bDediHack;
	bool				m_bIsDedi;
public:
	CLogFile();
	~CLogFile();

	
	virtual void		Init(i3::string sWorkBase, i3::string sLogName);

	void				SetSearchRange(INT32 nBeginDate, INT32 nEndDate);

	static BOOL			_FindProc(INT32 Level, char * pszPath, WIN32_FIND_DATA * pFileInfo, void * pUserData);
	void				ExtractLog();


	void		SetDediHack(bool hack) { m_bDediHack = hack; }
	bool		GetDediHack() { return m_bDediHack; }
};

