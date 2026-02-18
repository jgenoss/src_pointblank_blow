// i3PackTool.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "i3PackTool.h"
#include "MainFrm.h"
#include "GlobalVar.h"
#include "SingleFile.h"
#include "ScriptCmd.h"

#include "i3Base/string/ext/remove_blank_from_head.h"
#include "i3Base/string/ext/remove_all_char.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/compare/generic_compare.h"
#include "i3Base/string/ext/extract_directoryname.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3PackToolApp

BEGIN_MESSAGE_MAP(Ci3PackToolApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()


// Ci3PackToolApp 생성

Ci3PackToolApp::Ci3PackToolApp()
{
	m_bNoWindow = false;
}


// 유일한 Ci3PackToolApp 개체입니다.

Ci3PackToolApp theApp;

// Ci3PackToolApp 초기화


char *	Ci3PackToolApp::ParsePath(char * pszCmd, char * pszPath)
{
	// 앞 공백 제거
	while (*pszCmd == ' ' || *pszCmd == '\t' )
		++pszCmd;


	int		idx = 0;
	if ( *pszCmd == '"' )
	{
		// 쌍따옴표로 감싸져 있을 경우입니다..
		pszCmd++;
		while ( *pszCmd && *pszCmd != '"' )
		{
			pszPath[idx] = *pszCmd;
			++idx;
			++pszCmd;
		}
	}
	else
	{
		while ( *pszCmd && *pszCmd != ' ' && *pszCmd != '\t' && *pszCmd != '\n' && *pszCmd != '\r' )
		{
			pszPath[idx] = *pszCmd;
			++idx;
			++pszCmd;
		}
	}
	pszPath[idx] = NULL;


	// 유효하지 않은 경로이거나 폴더가 아니면 문자열을 초기화합니다.
	DWORD Rv = GetFileAttributes(pszPath);
	if ( INVALID_FILE_ATTRIBUTES == Rv || !(FILE_ATTRIBUTE_DIRECTORY & Rv) )
		pszPath[0] = NULL;

	return pszCmd;
}

char * Ci3PackToolApp::ParseException( char * pszCmd, i3::vector<i3::rc_string>& ExceptionList)
{
	while( *pszCmd == ' ' || *pszCmd == '\t' || *pszCmd == '"')
		pszCmd++;

	while( *pszCmd && *pszCmd != ' ' && *pszCmd != '\t' && *pszCmd != '\n' && *pszCmd != '\r' )
	{
		INT32 cnt = 0;
		char szFile[MAX_PATH] = "";

		while( *pszCmd && *pszCmd != ' ' && *pszCmd != '\t' && *pszCmd != '\n' && *pszCmd != '\r' && *pszCmd != ';' && *pszCmd != '"')
		{			
			szFile[ cnt++] = *pszCmd;

			pszCmd++;
		}

		szFile[ cnt] = 0;

		// (;) 기호로 개별 구분
		if( cnt > 0)
		{
			ExceptionList.push_back(i3::rc_string(szFile, cnt) );			
		}	

		// 공백이면 끝낸다.
		if( *pszCmd == ' ' || *pszCmd == 0) 
			return pszCmd;
		else
			pszCmd++;
	}

	return pszCmd;
}

void Ci3PackToolApp::PrintUsage(void)
{
	PRINTF( "i3PackTool version 1.0    copyright Zepetto Inc.\n");
	PRINTF( "\n");
	PRINTF( "Usage : i3PackTool <Option> <ScriptFile>\n");
	PRINTF( "\n");
	PRINTF( "Option :\n");
	PRINTF(	"    /? : help\n");
	PRINTF( "    /S : Silence mode\n");
	PRINTF( "	 /X : Exit program after processing.\n");
	PRINTF( "	 /W : Override working directory.\n");
#if 0
	PRINTF( "	 /O : Set output directory.\n");
#endif
}

void Ci3PackToolApp::ParseCmd( CMainFrame * pFrame)
{
	char szScript[MAX_PATH];
	INT32 idx = 0;
	bool	bInStr = false;
//	bool	bFirst = true;

	// "/W" 옵션으로 워킹폴더 Override시에 레지스트리에 있는것을
	// 덮어 씌우지 않도록 원래 패스는 저장하고 마지막에 복구시킵니다.
	char	szOverrideWorkPath[MAX_PATH] = {};
	char	szOrigWorkPath[MAX_PATH] = {};

	// "/E" 옵션으로 제외파일 Override시에 레지스트리에 있는것을
	// 덮어 씌우지 않도록 원래 옵션을 저장하고 마지막에 복구시킵니다.
	i3::vector<i3::rc_string> cOverrideExceptionFile;
	i3::vector<i3::rc_string> cOriginalExceptionFile;

	char	szOutputPath[MAX_PATH] = {};

	char	_szCmdLine[1024] = {};
	strcpy(_szCmdLine, GetCommandLine());
//	i3String::RemoveAllChar(_szCmdLine, '"');
	i3::remove_all_char(_szCmdLine, '"');

	char * pszCmdLn = _szCmdLine;

	while( *pszCmdLn)
	{
		switch( *pszCmdLn)
		{
		case '/' :
			//case '-' :
			pszCmdLn ++;
			switch( *(pszCmdLn))
			{
			case '?' :
				PrintUsage();
				m_bNoWindow = true;
				break;

			case 'S' :
			case 's' :	
				g_bVerbose = false;
				break;

			// No run window
			case 'X' :
			case 'x' :
				m_bNoWindow = true;
				break;

			// Working Directory
			case 'W' :
			case 'w' :
				{
					++pszCmdLn;
					pszCmdLn = ParsePath(pszCmdLn, szOverrideWorkPath);
					if ( szOverrideWorkPath[0] != NULL )
					{
						PRINTF("Override Working Directory [%s]\n", szOverrideWorkPath);
					}
					else
					{
						PRINTF("Invalid Working Directory [%s]\n", szOverrideWorkPath);
					}
				}
				break;

			// Exception file 
			case 'E' :
			case 'e' :
				{
					++pszCmdLn;

					cOverrideExceptionFile.clear();
					pszCmdLn = ParseException(pszCmdLn, cOverrideExceptionFile);					
				}
				break;
			case 'o' :
			case 'O' :
				{
					++pszCmdLn;
					pszCmdLn = ParsePath(pszCmdLn, szOutputPath);
					if ( szOutputPath[0] != NULL )
					{
						PRINTF("Set Output Directory [%s]\n", szOutputPath);
					}
					else
					{
						PRINTF("invalid Output Directory [%s]\n", szOutputPath);
					}
				}
				break;
			}

			idx = 0;
			break;

		case '"' :
			if( bInStr)
			{
				bInStr = false;
				szScript[idx] = 0;
			}
			else
			{
				bInStr = true;
				idx = 0;
			}
			break;

		case ' ' :
			idx = 0;
			break;

		default :
			szScript[idx] = *pszCmdLn;
			idx++;
			break;

		}

		pszCmdLn ++;
	}

	if ( szOverrideWorkPath[0] != NULL )
	{
		strcpy(szOrigWorkPath, g_pOption->m_szWorkPath);
		strcpy(g_pOption->m_szWorkPath, szOverrideWorkPath);
	}

	if( cOverrideExceptionFile.size() > 0)
	{
		// backup
		cOriginalExceptionFile.clear();
		for( size_t i = 0; i < g_pOption->m_ExceptList.size(); i++)
			cOriginalExceptionFile.push_back( g_pOption->m_ExceptList[i]);

		// new
		g_pOption->m_ExceptList.clear();
		for( size_t i = 0; i < cOverrideExceptionFile.size(); i++)
			g_pOption->m_ExceptList.push_back( cOverrideExceptionFile[i]);
	}

	szScript[idx] = 0;

	if( szScript[0] != 0)
	{
		if( ScriptCmd::ParseScript( szScript, m_CmdList) == false)
		{
			I3PRINTLOG(I3LOG_FATAL, "%s Script 파일을 처리할 수 없습니다.", szScript);
			return;
		}
	}

	for( size_t i = 0; i < m_CmdList.size(); i++)
	{
		ScriptCmd * pCmd = m_CmdList[i];

		switch( pCmd->getCode())
		{
		case ScriptCmd::CMD_ADD :
			{
				CString	Path;
				Path.Format("%s/%s", g_pOption->m_szWorkPath, pCmd->getCmdParam());

				BOOL	IncludeSubDir = !i3::generic_is_iequal(pCmd->getCmdParam(1), "-nosub");

				PRINTF( "Adding %s ...", Path);
				pFrame->Add( Path, IncludeSubDir);
			}
			break;

		case ScriptCmd::CMD_REMOVE :
			{
				CString	Path;
				Path.Format("%s/%s", g_pOption->m_szWorkPath, pCmd->getCmdParam());

				const BOOL IncludeSubDir = !i3::generic_is_iequal(pCmd->getCmdParam(1), "-nosub");

				PRINTF( "Removing %s ...", Path);
				pFrame->Remove( Path, IncludeSubDir);
			}
			break;

		case ScriptCmd::CMD_ENCRIPT :
			{
				CString	Path;
				Path.Format("%s/%s", g_pOption->m_szWorkPath, pCmd->getCmdParam());

				PRINTF( "Encripting %s ...", Path);
				pFrame->EncFile( Path);
			}
			break;

		case ScriptCmd::CMD_SAVE :
			{
#if 0		// 명령 /o를 적용시 pack파일 결과물이 달라져서 막습니다.
				// 문제점 확인 후 수정하겠습니다.
				char szSavePath[MAX_PATH] = {};
				if (szOutputPath[0] != NULL)
				{
					i3::snprintf(szSavePath, MAX_PATH, "%s/%s", szOutputPath, pCmd->getCmdParam());
					PRINTF( "Saving %s ...", szSavePath);
					pFrame->SavePack( szSavePath);
				}
				else
#endif
				{
					PRINTF( "Saving %s ...", pCmd->getCmdParam());
					pFrame->SavePack( pCmd->getCmdParam());
				}
			}
			break;

		case ScriptCmd::CMD_CHECKSUM :
			PRINTF( "Setting checksum code %s ...", pCmd->getCmdParam());
			pFrame->SetChecksum( pCmd->getCmdParam());
			break;
		}

		PRINTF( "Done\n");

		delete pCmd;
	}

	// Override한 작업폴더를 복구합니다.
	if ( szOverrideWorkPath[0] != NULL )
		strcpy(g_pOption->m_szWorkPath, szOrigWorkPath);

	if( cOriginalExceptionFile.size() > 0)
	{
		// new
		g_pOption->m_ExceptList.clear();
		for( size_t i = 0; i < cOriginalExceptionFile.size(); i++)
			g_pOption->m_ExceptList.push_back( cOriginalExceptionFile[i]);
	}

	m_CmdList.clear();
}

BOOL Ci3PackToolApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다.
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.
	InitCommonControls();

	CWinApp::InitInstance();

//	_CrtSetBreakAlloc(690);

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	SetRegistryKey(_T("Zepetto"));

	{
		i3MemoryInit();
		i3BaseRegisterMetas();
		i3TDKRegisterMetas();

//		SingleFile::RegisterMeta();

		AttachConsole( ATTACH_PARENT_PROCESS);
	}

	{
		g_pOption = new COptionInfo;
	}

	// 주 창을 만들기 위해 이 코드에서는 새 프레임 창 개체를
	// 만든 다음 이를 응용 프로그램의 주 창 개체로 설정합니다.
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 프레임을 만들어 리소스와 함께 로드합니다.
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	ParseCmd( pFrame);

	if( m_bNoWindow)
	{
		delete pFrame;
		return FALSE;
	}

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	// SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	return TRUE;
}


int Ci3PackToolApp::ExitInstance()
{
	if( g_pOption != NULL)
	{
		delete g_pOption;
		g_pOption = NULL;
	}

	i3System::TerminateSys();		// 이게 없으면 릭..(2012.09.18.수빈)

	return CWinApp::ExitInstance();
}

void		Ci3PackToolApp::SetBatCmd(char* pszPath)
{
	i3FileStream file;
	char szLine[512];
	INT32 line;

	if( file.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == FALSE)
	{
		I3PRINTLOG(I3LOG_FATAL, "%s 파일을 열 수 없습니다.", pszPath);
	}

	line = 1;

	while( file.ReadLine( szLine, sizeof(szLine) - 1) > 0)
	{
		i3::remove_blank_from_head( szLine);
		CString temp = pszPath;
		CString X = szLine;
		CString name;

		if( (szLine[0] == 0) || (szLine[0] == '#' ) || (X.ReverseFind('%') == -1))
		{
			// Comment
			continue;
		}

		name = X.Right( X.GetLength() - X.ReverseFind('%')-2);
		temp.Format("%s\\%s", temp.Mid(0, temp.ReverseFind('\\')), name);
		SetScriptCmd(temp.GetBuffer());
		line++;
	}

	file.Close();
}

void Ci3PackToolApp::Export_i3RegXMLToPEF(const char* pszPath)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	char szFileName[MAX_PATH];
	i3::extract_filetitle(pszPath, szFileName);

	if (i3::generic_compare(szFileName, "Script") == 0 ||
		i3::generic_compare(szFileName, "Script_Locale") == 0)
	{
		i3::string regFile = g_pOption->m_szWorkPath;
		regFile += "\\config\\*.i3RegXML";

		CFileFind	FindCtx;

		BOOL IsFind = FindCtx.FindFile(regFile.c_str());

		i3::vector<i3::string> fileList;

		if (i3::generic_compare(szFileName, "Script_Locale") == 0)
		{
			while (IsFind)
			{
				IsFind = FindCtx.FindNextFile();

				if (FindCtx.IsDots())	continue;
				if (FindCtx.IsSystem())continue;

				if (i3::generic_compare(FindCtx.GetFileTitle(), "LocaleValue") == 0)
					fileList.push_back(i3::string(FindCtx.GetFilePath()));
			}
		}
		else
		{
			while (IsFind)
			{
				IsFind = FindCtx.FindNextFile();

				if (FindCtx.IsDots())	continue;
				if (FindCtx.IsSystem())continue;

				if (!(i3::generic_compare(FindCtx.GetFileTitle(), "LocaleValue") == 0))
					fileList.push_back(i3::string(FindCtx.GetFilePath()));
			}
		}
		
		for (INT32 i = 0; i < (INT32)fileList.size(); i++)
		{
			i3RegistrySet reg;
			if (reg.LoadRegistrySet(fileList[i].c_str(), REGISTRY_FILE_XML_BY_UTF8) != STREAM_ERR)
			{
				char curDirName[MAX_PATH], curTitleName[MAX_PATH];
				i3::extract_directoryname(fileList[i].c_str(), curDirName);
				i3::extract_filetitle(fileList[i].c_str(), curTitleName);

				i3::string exportPef = curDirName;
				exportPef += "\\";
				exportPef += curTitleName;
				exportPef += ".Pef";

				reg.SaveRegistrySet(exportPef.c_str(), REGISTRY_FILE_BINARY);
			}

			INT32 num = int((((float)i + 1) / ((float)fileList.size() + 1)) * 100.f);
			pFrame->m_PanelTreeScript.m_progressCtrl.SetPos(num);
			pFrame->UpdateData(false);
		}
	}
}
void		Ci3PackToolApp::SetScriptCmd(char* szScript)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	//script나 script_local을 팩할때.i3RegXML을 .pef로 export한다
	Export_i3RegXMLToPEF(szScript);

	if(g_pPack == NULL)
		pFrame->NewPack();
	else
	{
		pFrame->ClosePack();
		pFrame->NewPack();
	}

	if( szScript[0] != 0)
	{
		if( ScriptCmd::ParseScript( szScript, m_CmdList) == false)
		{
			I3PRINTLOG(I3LOG_FATAL, "%s Script 파일을 처리할 수 없습니다.", szScript);
			return;
		}
	}

	for( size_t i = 0; i < m_CmdList.size(); i++)
	{
		ScriptCmd * pCmd = m_CmdList[i];

		switch( pCmd->getCode())
		{
		case ScriptCmd::CMD_ADD :
			{
				CString	Path;
				Path.Format("%s/%s", g_pOption->m_szWorkPath, pCmd->getCmdParam());

				BOOL	IncludeSubDir = !i3::generic_is_iequal(pCmd->getCmdParam(1), "-nosub");

				PRINTF( "Adding %s ...", Path);
				pFrame->Add( Path, IncludeSubDir);
			}
			break;

		case ScriptCmd::CMD_REMOVE :
			{
				CString	Path;
				Path.Format("%s/%s", g_pOption->m_szWorkPath, pCmd->getCmdParam());

				PRINTF( "Removing %s ...", Path);

				const BOOL includeSubDirOK = !i3::generic_is_iequal(pCmd->getCmdParam(1), "-nosub");
				pFrame->Remove( Path, includeSubDirOK);
			}
			break;

		case ScriptCmd::CMD_ENCRIPT :
			{
				CString	Path;
				Path.Format("%s/%s", g_pOption->m_szWorkPath, pCmd->getCmdParam());

				PRINTF( "Encripting %s ...", Path);
				pFrame->EncFile( Path);
			}
			break;

		case ScriptCmd::CMD_SAVE :
			{
#if 0		// 명령 /o를 적용시 pack파일 결과물이 달라져서 막습니다.
				// 문제점 확인 후 수정하겠습니다.
				char szSavePath[MAX_PATH] = {};
				if (szOutputPath[0] != NULL)
				{
					i3::snprintf(szSavePath, MAX_PATH, "%s/%s", szOutputPath, pCmd->getCmdParam());
					PRINTF( "Saving %s ...", szSavePath);
					pFrame->SavePack( szSavePath);
				}
				else
#endif
				{
					CString	Path;
					Path.Format("%s\\pack\\%s", g_pOption->m_szLivePath,pCmd->getCmdParam());
					PRINTF( "Saving %s ...", pCmd->getCmdParam());
					pFrame->SavePack( pCmd->getCmdParam(),Path.GetBuffer());
				}
			}
			break;

		case ScriptCmd::CMD_CHECKSUM :
			PRINTF( "Setting checksum code %s ...", pCmd->getCmdParam());
			CString	Path;
			Path.Format("%s\\pack\\%s", g_pOption->m_szLivePath,pCmd->getCmdParam());
			pFrame->SetChecksum( pCmd->getCmdParam(), Path.GetBuffer());
			break;
		}
		PRINTF( "Done\n");

		delete pCmd;
	}
	m_CmdList.clear();
	if(g_pPack != NULL)
		pFrame->ClosePack();
}



// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

	// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void Ci3PackToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// Ci3PackToolApp 메시지 처리기
