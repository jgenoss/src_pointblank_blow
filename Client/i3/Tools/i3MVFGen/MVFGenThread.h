#if !defined( __MVF_GEN_THREAD_H)
#define __MVF_GEN_THREAD_H

#include "MVFNode.h"

typedef void (*_LOGPROC)( INT32 type, MVFNode * pNode, char * pszMsg, void * pUserData);

#define		THM_PROGRESS		(WM_USER + 5)
#define		THM_FILE			(WM_USER + 3)
#define		THM_END				(WM_USER + 4)

class MVFGenThread : public i3Thread
{
	I3_CLASS_DEFINE( MVFGenThread);
protected:
	char			m_szFile[MAX_PATH];
	_LOGPROC		m_pLogProc;
	void *			m_pUserData;
	MVFNode *		m_pRoot;
	i3FileStream *	m_pStream;
	HWND			m_hwnd;
	UINT32			m_AlignUnit;
	i3StringList	m_StrList;
	INT32 *			m_pStringOffsetTable;
	char *			m_pStrTable;

	I3_MVF_HEADER	m_Header;
	I3_MVF_ENTRY *	m_pEntry;
	INT32			m_EntryIdx;

	void		ReportInfo( MVFNode * pNode, char * pszMsg, ...);
	void		ReportWarn( MVFNode * pNode, char * pszMsg, ...);
	void		ReportErr( MVFNode * pNode, char * pszMsg, ...);

	static void _Process( char * pszFull, MVFNode * pNode, MVFGenThread * pThread);
	void		EntryUpdateProc( MVFNode * pNode, I3_MVF_ENTRY * pEntry);
	
	void		Align(void);
public:
	MVFGenThread(void);
	virtual ~MVFGenThread(void);

	void		SetMVFFileName( char * pszFile)		{ i3String::Copy( m_szFile, pszFile); }
	void		SetLogProc( _LOGPROC pProc)			{ m_pLogProc = pProc; }

	void		SetUserData( void * pUserData)		{ m_pUserData = pUserData; }

	void		SetHWND( HWND hwnd)					{ m_hwnd = hwnd; }

	void		SetRoot( MVFNode * pRoot)			{ m_pRoot = pRoot; }

	void		SetAlignUnit( UINT32 unit)			{ m_AlignUnit = unit; }

	virtual UINT32	OnRunning( void * pUserData);
};

#endif
