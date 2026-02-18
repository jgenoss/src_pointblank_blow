#include "stdafx.h"
#include "ScanThread.h"
#include "FileInfo.h"

I3_CLASS_INSTANCE( ScanThread, i3Thread);

ScanThread::ScanThread(void)
{
	m_szResPath[0] = 0;
}

ScanThread::~ScanThread(void)
{
}

bool ScanThread::_Check( char * pszPath, char * pszName)
{
	INT32 i;
	Warn * pWarn;
	bool bResult;
	CFileInfo * pInfo = NULL;
	char szExt[64];
	i3ElementBase * pObj = NULL;
	i3StringList msgList;

	i3String::SplitFileExt( pszName, szExt, sizeof( szExt));

	if( i3String::Compare( szExt, "I3I") == 0)
	{
		i3ImageFile file;

		pObj = file.Load( pszPath);
		
		if( pObj == NULL)
		{
			I3TRACE( "%s 파일을 읽어들일 수 없습니다.", pszPath);
			return true;
		}

		pObj->AddRef();
	}
	else if( i3String::Compare( szExt, "I3S") == 0)
	{
		i3SceneFile file;

		if( file.Load( pszPath) == STREAM_ERR)
		{
			I3TRACE( "%s 파일을 읽을 수 없습니다.", pszPath);
			return true;
		}

		pObj = file.getKeyObject();

		pObj->AddRef();
	}

	if( pObj != NULL)
	{
		pInfo = NULL;

		for( i = 0; i < m_pWarnList->GetCount(); i++)
		{
			pWarn = (Warn *) m_pWarnList->Items[i];

			if( pWarn->isEnable() == false)
				continue;

			if( pWarn->IsInterested( pszPath, szExt) == false)
				continue;

			if( pWarn->Check( pszPath, szExt, pObj, &msgList) == true)
				continue;

			if( pInfo == NULL)
			{
				pInfo = CFileInfo::NewObjectRef();

				pInfo->setPath( pszPath);
			}

			pInfo->addWarn( pWarn, &msgList);
			msgList.Clear();
		}

		if( pInfo != NULL)
		{
			::SendMessage( m_hwnd, WM_SCAN_ADD, (WPARAM) pInfo, NULL);
			bResult = true;
		}

		pObj->Release();
	}

	return bResult;
}

BOOL ScanThread::_ResScanProc( int Level, LPCTSTR Path, WIN32_FIND_DATA * pInfo, LPVOID pUserData)
{
	ScanThread * pThread = (ScanThread *) pUserData;
	char szFull[ MAX_PATH];
	
	sprintf( szFull, "%s/%s", Path, pInfo->cFileName);

	SendMessage( pThread->m_hwnd, WM_SCAN_FILE, (WPARAM) szFull, 0);

	//pThread->_Check( szFull, pInfo->cFileName);
	
	return TRUE;
}

UINT32 ScanThread::OnRunning( void * pUserData)
{
	::SendMessage( m_hwnd, WM_SCAN_START, 0, 0);

	i3System::LookForFiles( m_szResPath, "*.*", (FINDFILEPROC) _ResScanProc, this, FFT_SUBDIR);

	::SendMessage( m_hwnd, WM_SCAN_END, 0, 0);
	return 0;
}
