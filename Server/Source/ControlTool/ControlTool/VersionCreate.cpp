#pragma warning(disable:4996)

#include "pch.h"
#include "VersionCreate.h"


CVersionCreate::CVersionCreate()
{
	ZeroMemory(m_MD5output, sizeof(WCHAR) * MD5_SIZE);

	m_FileList		= NULL;
	m_ConfigFile	= NULL;
	
	ZeroMemory(m_strFilePath, MAX_PATH);
	ZeroMemory(m_strRootPath, MAX_PATH);	//수정
	
	m_i32CurFileCount = 0;

	m_bCreate = FALSE;
}


CVersionCreate::~CVersionCreate()
{
	if(m_ConfigFile != NULL)
		fclose(m_ConfigFile);


	if(m_FileList != NULL)
	{
		delete[] m_FileList;
		m_FileList = NULL;
	}

	m_bCreate = FALSE;
	
}






BOOL CVersionCreate::CheckVersion_onlyfile(WCHAR* Input)
{
	if(Input == NULL)
		return FALSE;

	if(m_MD5output[0] == Input[0])
	{
		INT32 i32Cnt = 1;
		for(INT32 idx = 1; idx != MD5_SIZE; idx++)
		{
			if(m_MD5output[idx] == Input[idx])
				i32Cnt++;
		}

		if(i32Cnt == MD5_SIZE)
			return TRUE;
	}
	
	return FALSE;
}







INT64	CVersionCreate::getFileSize(const TCHAR *filename)
{
	struct _stati64 statbuf;
		
	if ( _wstati64(filename, &statbuf) ) return -1; // 파일 정보 얻기: 에러 있으면 -1 을 반환

	return statbuf.st_size;                        // 파일 크기 반환
}

void	CVersionCreate::GetCount(TCHAR* pDirectory, INT32& pi32Count)
{
	WIN32_FIND_DATA wfd;							//파일 정보 구조체
	HANDLE FileSearch = NULL;						//파일 정보를 열어볼 HANDLE
	TCHAR strFolderPath[MAX_PATH] = _T("");			//파일의 위치한 디렉토리 저장 버퍼
	
	_stprintf_s(strFolderPath, _T("%s\\%s"), pDirectory,  _T("\\*.*"));

	//폴더내 파일 검색 시작
	FileSearch = FindFirstFile(strFolderPath, &wfd);

	if(FileSearch == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if(strcmp((char*)wfd.cFileName, ".") && strcmp((char*)wfd.cFileName, ".."))
		{
			if((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				TCHAR Subfolder[MAX_PATH] = {0, };
				TCHAR tempPath[MAX_PATH] = {0, };
				INT32 idx = 0;
				while(TRUE)
				{
					tempPath[idx] = wfd.cFileName[idx];
					idx++;

					if(wfd.cFileName[idx] == '\0')
					{
						tempPath[idx] = '\0';
						break;
					}

				}
				_stprintf_s(Subfolder, _T("%s\\%s"), pDirectory, tempPath);
				GetCount(Subfolder, pi32Count);
			}
			else
				pi32Count++;
		}
	}
	while(FindNextFile(FileSearch, &wfd));
	
	//검색이 완료되면 HANDLE을 반환
	FindClose(FileSearch);

	return;
}







void	CVersionCreate::ClearString( TCHAR* strBuf)
{
	INT32 i = 0;
	while(strBuf[i] != 0)
	{
		strBuf[i] = 0;
		i++;
	}

	return;
}








WCHAR*	CVersionCreate::MD5Encrypt(TCHAR* pFileName, INT32 i32Current)
{
	UINT64 i64FileSize			= 0;			//파일크기. 크기가 비대해 질것을 대비
	char MD5_Digest[MD5_SIZE]	= {0, };
	char cMD5_Data[MD5_SIZE]	= {0, };
	WCHAR MD5_Data[MD5_SIZE]		= {0, };
	
	//파일 사이즈
	i64FileSize = getFileSize(pFileName);	

	md5_file(pFileName, (unsigned char*)MD5_Digest);
	MD5::Gen(MD5_Digest, MD5_SIZE, cMD5_Data);

	//생성된 MD5 마지막에 추가.
	cMD5_Data[MD5_SIZE - 1] = '\0';

	//char -> WCHAR
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cMD5_Data, MD5_SIZE, MD5_Data, MD5_SIZE);

	//MD5셋팅후 복사
	if(i32Current != -1)
	{
		memcpy(m_FileList[i32Current].MD5output, MD5_Data, MD5_SIZE);
		return NULL;
	}
	else
	{
		WCHAR MD5Value[MD5_SIZE]		= {0, };
		memcpy(MD5Value, MD5_Data, MD5_SIZE);
		return MD5Value;
	}

	return NULL;
}










WCHAR* CVersionCreate::GetMD5ValueFromPath( TCHAR* input)
{
	return MD5Encrypt(input, -1);
}








void	CVersionCreate::SetFileVersionList(TCHAR* pDirectory, INT32& Count)
{
	WIN32_FIND_DATA wfd;							//파일정보 구조체
	HANDLE FileSearch = NULL;						//파일정보를 열어볼 HANDLE
	TCHAR strFileName[MAX_PATH] = _T("");			//파일이름 저장할 버퍼
	TCHAR strFilePath[MAX_PATH] = _T("");			//파일위치 저장할 버퍼					- 파일 Open
	TCHAR strFileFullPath[MAX_PATH] = _T("");		//파일위치 MD5생성용 버퍼
	TCHAR strFindFolderPath[MAX_PATH];				//파일이 위치란 Directory 저장 버퍼		- 파일 검색
	
	_stprintf_s( strFindFolderPath, _T("%s\\%s"), pDirectory,  _T("\\*.*") );

	//파일 검색
	FileSearch = FindFirstFile( strFindFolderPath, &wfd );

	if(FileSearch == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if(strcmp((char*)wfd.cFileName, ".") && strcmp((char*)wfd.cFileName, ".."))
		{
			//폴더 제외
			if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				TCHAR Subfolder[MAX_PATH] = {0, };
				TCHAR tempPath[MAX_PATH] = {0, };
				INT32 idx = 0;
				while(TRUE)
				{
					tempPath[idx] = wfd.cFileName[idx];
					idx++;

					if(wfd.cFileName[idx] == '\0')
					{
						tempPath[idx] = '\0';
						break;
					}

				}
				_stprintf_s( Subfolder, _T("%s\\%s"), pDirectory, tempPath);
				SetFileVersionList(Subfolder, Count);
			}
			else
			{
				//파일 이름을 가져온다
				INT32 i = 0;				
				while(wfd.cFileName[i] != 0)
				{
					strFileName[i] = wfd.cFileName[i];
					i++;
				}

				_stprintf_s(strFileFullPath, _T("%s\\%s"), pDirectory, strFileName);

				SetFilePath(strFileFullPath, strFilePath);
				
				memcpy(m_FileList[Count].FileName, strFilePath, MAX_PATH);

				//파일이름으로 MD5 버젼 생성
				MD5Encrypt(strFileFullPath, Count);

				//파일 전송여부 초기화
				m_FileList[Count].SetChange = TRUE;

				//파일리스트 카운트 증가
				Count++;
				
				//사용했던 버퍼 초기화
				ClearString(strFilePath);
				ClearString(strFileName);			
			}
		}
	}
	while(FindNextFile(FileSearch, &wfd));
	
	//사용한 HANDLE 반납
	FindClose(FileSearch);
}










BOOL	CVersionCreate::Create(char* strPath, char* strRootPath)
{
	INT32 i32Cur = 0;
	
	memcpy(m_strFilePath, strPath, MAX_PATH);
	memcpy(m_strRootPath, strRootPath, MAX_PATH);	//수정

	//해당 Path내에 몇개에 파일이 있는지 검사
	GetCount(m_strFilePath, m_i32CurFileCount);

	//파일 갯수 만큼 동적으로 크기 할당
	m_FileList = new FILELIST[m_i32CurFileCount];


	//버젼을 생성 셋팅
	SetFileVersionList(m_strFilePath, i32Cur);

	m_bCreate = TRUE;

	return m_bCreate;
}








BOOL	CVersionCreate::CheckVersion(char* UpdateData)
{
	if(UpdateData == NULL)
		return FALSE;

	for(INT32 i32Cur = 0; i32Cur != m_i32CurFileCount; i32Cur++)
	{
		if(m_FileList[i32Cur].MD5output[0] == UpdateData[0])
		{
			INT32 i32Cnt = 1;
			for(INT32 idx = 1; idx != MD5_SIZE; idx++)
			{
				if(m_FileList[i32Cur].MD5output[idx] == UpdateData[idx])
					i32Cnt++;
			}

			if(i32Cnt == MD5_SIZE)
				return TRUE;
		}
	}

	return FALSE;
}








//받은 파일리스트 메모리를 기존 파일리스트에 할당하기 때문에 파라미터로 받은 파일리스트의 메모리를 삭제 하면 안됨
//리턴값은 파일리스트와 같은 크기의 메모리에 해당 파일을 받을것인이 여부를 true / false 로 셋팅함
BOOL	CVersionCreate::CheckVersion(FILELIST* UpdateDataList, INT32 i32ListSize)
{
	if(UpdateDataList == NULL)
		return FALSE;

	//전송받은 FileList Loop
	for(INT32 i32Cur = 0; i32Cur != i32ListSize; i32Cur++)
	{
		INT32 i32DataCur = 0;
		BOOL bSetFind = FALSE;

		//기존의 파일 List Loop
		while(TRUE)
		{
			//첫 부분이 같은지 확인.
			if(UpdateDataList[i32Cur].MD5output[0] == m_FileList[i32DataCur].MD5output[0])
			{
				INT32 i32Cnt = 1;

				while(TRUE)
				{
					//하나라도 다른 부분이 있으면 다른 파일이라 판단
					if(UpdateDataList[i32Cur].MD5output[i32Cnt] != m_FileList[i32DataCur].MD5output[i32Cnt])
					{
						i32DataCur++;
						break;
					}

					i32Cnt++;

					//같은파일
					if((i32Cnt + 1) == MD5_SIZE)
					{
						UpdateDataList[i32Cur].SetChange = FALSE;
						bSetFind = TRUE;
						break;
					}
				}
			}
			//다르면 다음 파일로.
			else
				i32DataCur++;

			//리스트의 끝 확인 || 같은파일을 찾음
			if( i32DataCur == m_i32CurFileCount  || bSetFind)
				break;
		}
	}
	
	//현재 파일리스트의 메모리 해제
	delete[] m_FileList;

	m_FileList = new FILELIST[i32ListSize];

	//새로운 파일리스트의 메모리 주소를 할당
	memcpy( m_FileList, UpdateDataList, sizeof(FILELIST) * i32ListSize );
	//m_FileList = UpdateDataList;
	//사이즈 변경
	m_i32CurFileCount = i32ListSize;

	m_ui16UpdateFilePos = 0;

	return TRUE;
}









void CVersionCreate::SetFilePath( TCHAR* FullPah, TCHAR* FilePath)
{
	INT32 CurCnt = 0;
	while(TRUE)
	{
		if(FullPah[CurCnt] == m_strRootPath[CurCnt])
			CurCnt++;
		else
			break;
								
	}
	
	INT32 j = 0;
	while(TRUE)
	{
		if(FullPah[CurCnt] == '\0')
		{
			FilePath[j] = '\0';
			break;
		}
		
		FilePath[j] = FullPah[CurCnt];
		j++;
		CurCnt++;
	}

	return;
}




//FILELIST* CVersionCreate::GetNextUpdateFile()
//{
//	//더이상 Update할 파일이 없을 경우 NULL 리턴
//	if( m_ui16UpdateFilePos >= m_i32CurFileCount ) return NULL;
//
//	for( INT32 i = m_ui16UpdateFilePos ; i < m_i32CurFileCount ; i++ )
//	{
//		if( TRUE == m_FileList[i].SetChange )
//		{
//			m_ui16UpdateFilePos++;
//			return &m_FileList[i];
//		}
//
//		m_ui16UpdateFilePos++;
//	}
//
//	//나머지 List를 검사하여 업데이트할 파일이 없는경우 NULL 리턴
//
//	return NULL;
//}
//
//void AttachFileList(FILELIST* &Des, UINT16& _sizeDes, FILELIST* Src, UINT16 _sizeSrc )
//{
//	FILELIST* NewFileList = new FILELIST[_sizeDes + _sizeSrc];
//
//	S2Memory::Copy(NewFileList, Des, sizeof(FILELIST) * _sizeDes );
//	S2Memory::Copy(NewFileList + _sizeDes, Src, sizeof(FILELIST) * _sizeSrc );
//
//	SAFE_DELETE(Des);
//	Des = NewFileList;
//	_sizeDes += _sizeSrc;
//}

