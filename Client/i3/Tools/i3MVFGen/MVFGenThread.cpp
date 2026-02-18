#include "stdafx.h"
#include "MVFGenThread.h"

I3_CLASS_INSTANCE( MVFGenThread, i3Thread);

MVFGenThread::MVFGenThread(void)
{
	m_szFile[0] = 0;
	m_pLogProc = NULL;
	m_pUserData = NULL;
	m_pRoot = NULL;
	m_AlignUnit = 1;
}

MVFGenThread::~MVFGenThread(void)
{
}

char tempstr[4096];

void MVFGenThread::ReportInfo( MVFNode * pNode, char * pszMsg, ...)
{
	va_list marker;

	va_start( marker, pszMsg);

	vsprintf( tempstr, pszMsg, marker);

	if( m_pLogProc == NULL)
		return;

	m_pLogProc( 99, pNode, tempstr, m_pUserData);
}

void MVFGenThread::ReportWarn( MVFNode * pNode, char * pszMsg, ...)
{
	va_list marker;

	va_start( marker, pszMsg);

	vsprintf( tempstr, pszMsg, marker);

	if( m_pLogProc == NULL)
		return;

	m_pLogProc( 1, pNode, tempstr, m_pUserData);
}

void MVFGenThread::ReportErr( MVFNode * pNode, char * pszMsg, ...)
{
	va_list marker;

	va_start( marker, pszMsg);

	vsprintf( tempstr, pszMsg, marker);

	if( m_pLogProc == NULL)
		return;

	m_pLogProc( 0, pNode, tempstr, m_pUserData);
}

void MVFGenThread::Align(void)
{
	UINT32	pos, mod;

	pos = m_pStream->GetPosition();

	mod = pos % m_AlignUnit;
	if( mod > 0)
	{
		m_pStream->SetPosition( (m_AlignUnit - mod), STREAM_CURRENT);
	}
}

typedef struct _tagCountData
{
	UINT32	m_Count;
	i3StringList *	m_pStrList;
} COUNT_PROC_DATA;

static void _CountingProc( char * pszFull, MVFNode * pNode, void * pUserData)
{
	COUNT_PROC_DATA * pData = (COUNT_PROC_DATA *) pUserData;

	pData->m_Count++;

	pData->m_pStrList->Add( (char *) pNode->GetName());
}

UINT32 MVFGenThread::OnRunning( void * pUserData)
{
	BOOL Rv;
	UINT32 Rc;
	COUNT_PROC_DATA Data;

	m_pEntry = NULL;
	m_pStringOffsetTable = NULL;
	m_pStrTable = NULL;

	m_pStream = i3FileStream::NewObject();

	Rv = m_pStream->Create( m_szFile, STREAM_WRITE);
	if( Rv == FALSE)
	{
		ReportErr( NULL, "Could not create %s MVF file.", m_szFile);
		goto ExitPart;
	}

	// 전체 개수를 구한다.
	{
		Data.m_Count = 0;
		Data.m_pStrList = &m_StrList;

		m_StrList.Clear();
		m_StrList.SetOnceMode( TRUE);
		
		m_pRoot->Traverse( NULL, _CountingProc, &Data);

		PostMessage( m_hwnd, THM_PROGRESS, Data.m_Count, 0);

		ReportInfo( NULL, "Total Node Data.m_Count : %d", Data.m_Count);
		ReportInfo( NULL, "Align Unit : %d bytes", m_AlignUnit);
	}

	// Header
	{
		memset( &m_Header, 0, sizeof(m_Header));

		memcpy( m_Header.m_ID, I3MVF_ID, 4);
		m_Header.m_Version[0] = 1;
		m_Header.m_Version[1] = 0;
		m_Header.m_TotalEntryCount = Data.m_Count;
		m_Header.m_EntryOffset = sizeof( m_Header);
		m_Header.m_AlignUnit = m_AlignUnit;
		m_Header.m_StringTableSize = m_StrList.GetBufferSize();

		I3TRACE( "String Table Size : %d\n", m_Header.m_StringTableSize);

		Rc = m_pStream->Write( &m_Header, sizeof( m_Header));
		if( Rc == STREAM_ERR)
		{
			ReportErr( NULL, "파일 헤더를 저장할 수 없습니다. %s", m_szFile);
			goto ExitPart;
		}
	}

	// Entries
	{
		m_pEntry = new I3_MVF_ENTRY[ Data.m_Count];
		
		Rc = m_pStream->Write( m_pEntry, sizeof(I3_MVF_ENTRY) * Data.m_Count);
		if( Rc == STREAM_ERR)
		{
			ReportErr( NULL, "Entry 정보를 기록할 수 없습니다. %s", m_szFile);
			goto ExitPart;
		}

		ReportInfo( NULL, "Entry Size : %d bytes\n", sizeof(I3_MVF_ENTRY) * Data.m_Count);

		// String Table
		{
			m_pStrTable = new char[ m_Header.m_StringTableSize];

			m_pStringOffsetTable = new INT32[ Data.m_Count];

			m_StrList.CopyToBuffer( m_pStrTable, m_Header.m_StringTableSize, m_pStringOffsetTable);

			Rc = m_pStream->Write( m_pStrTable, m_Header.m_StringTableSize);
			if( Rc == STREAM_ERR)
			{
				ReportErr( NULL, "String Table을 저장할 수 없습니다. %s", m_szFile);
				goto ExitPart;
			}
		}

		ReportInfo( NULL, "String Table Size : %d bytes\n", m_Header.m_StringTableSize);

		Align();
	}

	m_pRoot->Traverse( NULL, (NODETRAVERSEPROC) _Process, this);

	m_Header.m_VolumeSize = m_pStream->GetPosition();

	// Entry 내용 갱신
	{
		m_EntryIdx = 1;			// Root Node를 위한 점유 조치
		EntryUpdateProc( m_pRoot, m_pEntry);
	}

	// Header Update
	{
		ReportInfo( NULL, "Volume Size : %d bytes\n", m_Header.m_VolumeSize);

		m_pStream->SetPosition( 0, STREAM_BEGIN);

		Rc = m_pStream->Write( &m_Header, sizeof( m_Header));
		if( Rc == STREAM_ERR)
		{
			ReportErr( NULL, "파일 헤더를 갱신할 수 없습니다. %s", m_szFile);
			goto ExitPart;
		}
	}

	// Entry Update
	{
		Rc = m_pStream->Write( m_pEntry, sizeof(I3_MVF_ENTRY) * Data.m_Count);
		if( Rc == STREAM_ERR)
		{
			ReportErr( NULL, "Entry 정보를 갱신할 수 없습니다. %s", m_szFile);
			goto ExitPart;
		}
	}

ExitPart:
	if( m_pStream != NULL)
	{
		m_pStream->Close();
		m_pStream->Release();
	}

	if( m_pEntry != NULL)
	{
		delete m_pEntry;
		m_pEntry = NULL;
	}

	if( m_pStringOffsetTable != NULL)
	{
		delete m_pStringOffsetTable;
		m_pStringOffsetTable = NULL;
	}

	if( m_pStrTable != NULL)
	{
		delete m_pStrTable;
		m_pStrTable = NULL;
	}

	PostMessage( m_hwnd, THM_END, 0, 0);

	return 0;
}

void MVFGenThread::_Process( char * pszFull, MVFNode * pNode, MVFGenThread * pThread)
{
	PostMessage( pThread->m_hwnd, THM_FILE, (WPARAM) pNode, 0);

	if( pNode->GetStyle() & I3MVF_ATTR_FOLDER)
	{
		// Folder

	}
	else
	{
		// File
		i3FileStream stream;
		BOOL Rv;

		//I3TRACE( "LOAD : %s\n", pszFull);
		Rv = stream.Open( pszFull, STREAM_READ);
		if( Rv == FALSE)
		{
			pThread->ReportErr( pNode, "파일을 읽을 수 없습니다.");
		}
		else
		{
			pNode->SetSize( stream.GetSize());
			pNode->SetOffset( pThread->m_pStream->GetPosition());

			stream.CopyTo( pThread->m_pStream, stream.GetSize());

			stream.Close();

			pThread->Align();
		}
	}
}

void MVFGenThread::EntryUpdateProc( MVFNode * pNode, I3_MVF_ENTRY * pEntry)
{
	INT32 i, entryidx, idx;
	MVFNode * pChild;

	pNode->GetEntry( pEntry);

	idx = m_StrList.IndexOf( (char *) pNode->GetName());
	I3ASSERT( idx != -1);
	pEntry->m_NameIndex = m_pStringOffsetTable[ idx];		// Name String Offset의 갱신

	//I3TRACE( "%s : %d\n", m_StrList.Items[idx], m_pStringOffsetTable[ idx]);

	if( pNode->GetStyle() & I3MVF_ATTR_FOLDER)
	{
		pEntry->dir.m_StartChildIndex = m_EntryIdx;
		pEntry->dir.m_ChildCount = pNode->GetChildCount();

		entryidx = m_EntryIdx;

		m_EntryIdx += pNode->GetChildCount();
		
		for( i = 0; i < pNode->GetChildCount(); i++)
		{
			pChild = pNode->GetChild( i);

			EntryUpdateProc( pChild, & m_pEntry[ entryidx]);
			entryidx++;
		}
	}
}