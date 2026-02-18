#include "stdafx.h"
#include "LogFile.h"
#include "MainFrm.h"

CLogFile::CLogFile()
{
	m_nBeginDate = m_nEndDate = 0;
	m_bIsDedi = m_bDediHack = false;
}


CLogFile::~CLogFile()
{
}

typedef struct _tagWin32FindInfo
{
	char*		m_pszWorkBase;
	char*		m_pszName;


} WIN32_FIND_INFO;
i3::vector<i3::string> g_InputTemp;

BOOL CLogFile::_FindProc(INT32 Level, char * pszPath, WIN32_FIND_DATA * pFileInfo, void * pUserData)
{
	char szExt[32];
	i3::extract_fileext(pFileInfo->cFileName, szExt);

	if (i3::generic_is_equal("txt", szExt) )
	{
		g_InputTemp.push_back(pFileInfo->cFileName);
	}

	return TRUE;
}

void CLogFile::Init(i3::string sWorkBase, i3::string sLogName)
{
	i3::string workBase = sWorkBase;
	workBase += "\\";
	workBase += sLogName;

	m_pszWorkBase = workBase;
	m_pszName = sLogName;

	if (i3::generic_compare(sLogName, "Dedi") == 0)
		m_bIsDedi = true;
	else
		m_bIsDedi = false;
}

void CLogFile::SetSearchRange(INT32 nBeginDate, INT32 nEndDate)
{
	m_nBeginDate = nBeginDate*100;
	m_nEndDate = nEndDate*100;
}

void CLogFile::ExtractLog()
{
	i3::string sBuff;
	char szTitle[128];

	WIN32_FIND_INFO info;
	g_InputTemp.clear();
	
	// Dedi는 두 개로 나누려고 합니다. 하하
	i3::string FindFile = "*.*";
	
	if (true == m_bIsDedi)
	{
		if (true == m_bDediHack)
		{
			FindFile = "Hack*.*";
		}
		else
		{
			FindFile = "Dedi*.*";
		}
	}
	
	i3System::LookForFiles((char*)m_pszWorkBase.c_str(), FindFile.c_str(), (FINDFILEPROC)_FindProc, &info);
	INT32 nOldData = 0;

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->SetPrograssRange(0, g_InputTemp.size());

	for (INT32 i = 0; i < g_InputTemp.size(); i++)
	{
		pMainFrm->SetPrograssBar(i);
		i3::extract_filetitle(g_InputTemp[i], szTitle);

		//하이픈 지우기
		i3::string _FileName = szTitle;

		//숫자를 제외한 모든 문자열 삭제
		for(i3::string::iterator it = _FileName.begin(); it != _FileName.end(); )
		{
			if (*it >= 48 && *it <= 57)	it++;
			else						it = _FileName.erase(it);
		}

		//실제 날자가아닌 끝에 두자리 제거
		//_FileName.erase(8);

		INT32 nDate = i3::atoi(_FileName);

		if (m_nBeginDate <= nDate && m_nEndDate >= nDate)
		{
			char szPath[MAX_PATH];
			i3::snprintf(szPath, sizeof(szPath), "%s\\%s", m_pszWorkBase.c_str(), g_InputTemp[i].c_str());
			i3FileStream file;
			if (file.Open(szPath, STREAM_READ) == FALSE)
			{
				I3PRINTLOG(I3LOG_WARN, "%s 파일을 열 수 없습니다.");
			}

			INT32 size = file.GetSize();

			bool bUnicode;

			//if (nOldData != nDate)
			{
				i3::snprintf(szPath, sizeof(szPath), "\n\n////////////////////////%s////////////////////////\n", _FileName);
				nOldData = nDate;
				sBuff += szPath;
			}

			if (size > 2)
			{
				char* pCheckData = new char[2];
				file.Read(pCheckData, 2);
				if (pCheckData[0] < 0xff && pCheckData[1] == 0)
					bUnicode = true;
				else
					bUnicode = false;

				I3_SAFE_DELETE(pCheckData);

				if (!bUnicode)
				{
					char* pBuf = new char[size];
					ZeroMemory(pBuf, sizeof(char) * size);
					file.Read(pBuf, size);

					sBuff += pBuf;
					I3_SAFE_DELETE(pBuf);
				}
				else
				{
					wchar_t* pBuf = new wchar_t[size];
					ZeroMemory(pBuf, sizeof(wchar_t) * size);
					file.Read(pBuf, size);

					char *temp = new char[size];
					ZeroMemory(temp, sizeof(char) * size);

					int nRet = WideCharToMultiByte(CP_ACP, 0, pBuf, -1, temp, size, NULL, NULL);

					sBuff += temp;
					I3_SAFE_DELETE(temp);
					I3_SAFE_DELETE(pBuf);
				}
			}
			
			file.Close();
		}
	}

	m_inout.push_back(sBuff);
}