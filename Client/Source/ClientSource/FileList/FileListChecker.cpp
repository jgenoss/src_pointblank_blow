#include "stdafx.h"
#include "FileListChecker.h"

#include "MD5.h"





CFileListChecker::CFileListChecker()
{
	Clear();
}


bool	CFileListChecker::Begin(CFileList * pDestFileList, ListErrorFileInfo * pErrorList, FileProgressData * pProgData, bool IsFullCheck, bool IsNTFS)
{
	m_IsFullCheck		= IsFullCheck;
	m_IsNTFS			= IsNTFS;
	m_pDestFileList		= pDestFileList;
	m_pFileInfoList		= &m_pDestFileList->GetFileInfoList();
	m_DestFileList_iter =  m_pFileInfoList->begin();

	m_pErrorList		= pErrorList;
	m_pErrorList->clear();

	if ( pProgData )m_pProgData = pProgData;
	else			m_pProgData = &m_tempProgData;

	m_pProgData->Clear();
	m_pProgData->NumTotal	= (UINT)m_pFileInfoList->size();
	m_pProgData->ProgText	= _T("file check");

	TRACE0("CFileListChecker::Begin\n");
	return true;
}


bool	CFileListChecker::End()
{
	TRACE0("CFileListChecker::End\n");
	Clear();
	return true;
}


void	CFileListChecker::Clear()
{
	m_IsFullCheck	= false;
	m_IsNTFS		= false;
	m_pDestFileList	= NULL;
	m_pFileInfoList	= NULL;
	m_pErrorList	= NULL;
	m_pProgData		= NULL;
	m_tempProgData.Clear();
}


bool	CFileListChecker::Update()
{
	assert ( NULL != m_pDestFileList );
	assert ( NULL != m_pFileInfoList );
	assert ( NULL != m_pErrorList );

	if ( m_DestFileList_iter != m_pFileInfoList->end() )
	{
		// 현재 검사를 하는 파일이름을 저장합니다.
		CFileListFileInfo * pInfo = *m_DestFileList_iter;
		m_pProgData->FileName = pInfo->GetFileName();


		// false를 반환하면 이상이있는 파일로 보고
		// 에러리스트에 추가합니다.
		UINT32	Rv = false;
		if ( m_IsFullCheck )
		{
			Rv = pInfo->CheckByCommand(m_pDestFileList->GetDirPath(), m_IsNTFS, &m_pProgData->FileSize, &m_pProgData->FileProgSize);
		}
		else
		{
			Rv = pInfo->CheckByStart(m_pDestFileList->GetDirPath(), m_IsNTFS, &m_pProgData->FileSize, &m_pProgData->FileProgSize);
		}

		if ( Rv )
		{
			m_pErrorList->push_back(ErrorFileInfo(pInfo, Rv));
		}

		++m_pProgData->NumProgCount;
		++m_DestFileList_iter;
		return true;
	}

	return false;
}
