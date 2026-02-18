#if !defined( __FILE_INFO_H)
#define __FILE_INFO_H

#include "Warn.h"

class WarnInfo
{
protected:
	Warn *			m_pWarn;
	i3StringList	m_MsgList;

public:
	WarnInfo(void);
	virtual ~WarnInfo(void);

	Warn *			getWarn(void)				{ return m_pWarn; }
	void			setWarn( Warn * pWarn)		{ m_pWarn = pWarn; }

	i3StringList *	getMsgList(void)			{ return &m_MsgList; }
	void			CopyMsgList( i3StringList * pMsgList);
};

class CFileInfo : public i3ElementBase
{
	I3_CLASS_DEFINE( CFileInfo);
protected:
	char		m_szPath[MAX_PATH];
	i3List		m_WarnList;

protected:
	void		_RemoveAllWarn(void);

public:
	CFileInfo(void);
	virtual ~CFileInfo(void);

	INT32		getWarnCount(void)				{ return m_WarnList.GetCount(); }
	void		addWarn( Warn * pWarn, i3StringList * pMsgList);

	char *		getPath(void)					{ return m_szPath; }
	void		setPath( char * pszPath)		{ i3String::Copy( m_szPath, pszPath); }
};

#endif
