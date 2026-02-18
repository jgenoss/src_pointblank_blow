// AnimOpt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "i3Base/string/ext/remove_blank_from_tail.h"
#include "i3Base/string/ext/remove_blank_from_head.h"
#include "i3Base/string/ext/generic_string_ncat.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/remove_slash_filename.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/ext/remove_end_slash.h"
#include "i3Base/string/algorithm/to_lower.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "AnimOpt.h"

/////////////////////////////////////////////////////////////////////////////
//	실행하기 위해서는 WorkFolder거 설정되어 있는 WorkDir.ini파일이 필요합니다.
//	파일이 없을 경우 메모장으로 생성이 가능하며 안에는 WorkFolder가 문자열로 지정되어 있어야 합니다.
//	WorkDir.ini에 설정된 WorkFolder는 AnimPack할때 Animation의 SetName으로 지정됩니다.
//
//	- 2008.08.08 komet
/////////////////////////////////////////////////////////////////////////////

//
//  실행 중 커맨드창에서 창을 x로 닫으면, 릭이 많이 나고 크래시가 일어나는데, 이건 정상적인 종료행위가 아니니까..
//  너무 놀라지 말것....정상적인 종료를 위해 엔터를 치고 넘긴다...(2012.09.10.수빈)
//  주 릭 원인은 매니저가 소거되지 않은 것인데, 우선 i3System::TerminateSys()를 추가해본다..

// GUI for verify, 2013-12-04, chunjong.song
// AnimPackviewer, trunk media비교용 GUI
#include "gui/AnimOptGUI.h"

i3::vector<ANIMPACK_INFO*> g_AnimList;

#pragma message("*****************************************************************")
#pragma message("*")
#pragma message("VS2008로 빌드후 배포시에는 반드시 Release로 빌드해서 배포하세요.")
#pragma message("*")
#pragma message("*****************************************************************")

char szWorkFolder[MAX_PATH] = ""; 
char szInputPath[MAX_PATH] = "";
HINSTANCE g_moduleInstance = NULL;


INT32 AnimCount(void)
{
	return (INT32)g_AnimList.size();
}

BOOL AddAnim(char * pszPath, bool bRemoveTrans, bool b16Bits)
{
	i3Animation * pAnim = i3Animation::LoadFromFile( pszPath);
	if( pAnim)
	{
		char szRelPath[ MAX_PATH];

	
		// 이름을 지정...
		// 항상 Working Folder를 기준으로 한, Relative Path가
		// 이름으로 지정되어야 한다.
		i3::make_relative_path( szWorkFolder, pszPath, szRelPath );
		i3::make_unix_path( szRelPath);
		pAnim->SetName( szRelPath);

		ANIMPACK_INFO * pInfo = (ANIMPACK_INFO*) i3MemAlloc( sizeof( ANIMPACK_INFO));

		i3mem::FillZero( pInfo, sizeof( ANIMPACK_INFO));

		pInfo->_pAnim = pAnim;

		if( i3::contain_string( pszPath, "1pv") >= 0)
			b16Bits = false;

		pInfo->_b16Bits = b16Bits;
		pInfo->_bRemoveTrans = bRemoveTrans;

		//	파일 경로 리스트 
		i3::string_ncopy_nullpad( pInfo->_szPath, pszPath, MAX_PATH);

		g_AnimList.push_back( pInfo);

		return TRUE;
	}

	return FALSE;
}

ANIMPACK_INFO * getAnimInfo( INT32 idx)
{
	return g_AnimList[ idx];
}

i3Animation * GetAnim(INT32 idx)
{
	return (i3Animation*) getAnimInfo(idx)->_pAnim;
}

void ReleaseAnim(void)
{
	for(INT32 i=0; i<AnimCount(); i++)
	{
		ANIMPACK_INFO* pAnimPackInfo = getAnimInfo(i);
		i3Animation * pAnim = pAnimPackInfo->_pAnim; 
		pAnim->Release();
		i3MemFree(pAnimPackInfo);
	}

	g_AnimList.clear();
}

BOOL FindAnim( char * pszRelativePath, BOOL IncludeSubDir, bool bRemoveTrans, bool b16Bits)
{
	HANDLE hFind;
	WIN32_FIND_DATA wfd;
	BOOL bFind = TRUE;
	char path[MAX_PATH] = "";
	char szFullPath[MAX_PATH] = "";
	char szTemp[MAX_PATH] = "";

	//	full path
	i3::safe_string_copy( szFullPath, szWorkFolder, MAX_PATH );
	i3::generic_string_cat( szFullPath, "\\");
	i3::generic_string_cat( szFullPath, pszRelativePath);

	i3::safe_string_copy( path, szFullPath, MAX_PATH );
	i3::generic_string_cat( path, "\\*.*");

	hFind = FindFirstFileEx(path, FindExInfoStandard, &wfd, FindExSearchNameMatch, NULL, 0);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		printf( pszRelativePath);
		printf( "\n지정된 폴더가 이상합니다. 정확한 상대 경로인지 확인해 주세요. 진행을 종료합니다.\n");

		return TRUE;
	}	

	//	collect all files in all folder with subfolder to list
	while( bFind)
	{	
		if ((IncludeSubDir == TRUE) &&
			(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			(wfd.cFileName[0] != '.')
			)
		{
			sprintf(path, "%s\\%s", pszRelativePath, wfd.cFileName);

			//	Recursive to subfolder
			if( !FindAnim( path, IncludeSubDir, bRemoveTrans, b16Bits))
			{
				FindClose( hFind);

				return FALSE;
			}
		}
		else
		{
			char szExt[64] = "";
			char fname[MAX_PATH] = "";
			sprintf(fname, "%s", wfd.cFileName);
			fname[MAX_PATH-1] = 0; 

		//	i3String::SplitFileExt( fname, szExt, 64);
			i3::extract_fileext(fname, szExt);

			if( i3::generic_is_iequal( szExt, "i3a") )
			{
				i3::safe_string_copy( path, szFullPath, MAX_PATH );
				i3::generic_string_cat( path, "\\");
				i3::generic_string_cat( path, fname);

				// Add an animation file to out list
				if( AddAnim(path, bRemoveTrans, b16Bits))
				{
					i3::make_relative_path( szWorkFolder, szFullPath, path );
					i3::generic_string_cat( path, "\\");
					i3::generic_string_cat(path, fname);

					sprintf(szTemp, "Detected : %s\n",  path);
					printf(szTemp);
				}
			}
		}

		bFind = FindNextFile(hFind, &wfd);
	}		
	FindClose( hFind);

	return TRUE;
}

BOOL _CheckI3CHRatPath( const char * pszFindPath)
{
	WIN32_FIND_DATA wfd;
	char szFindFolder[ MAX_PATH] = {0,};
	char szExt[ 16] = {0,};

	BOOL bCheck = TRUE;

	i3::safe_string_copy( szFindFolder, pszFindPath, MAX_PATH );
	i3::generic_string_cat( szFindFolder, "\\*.*");

	HANDLE hCheckFile = ::FindFirstFile( szFindFolder, &wfd);

	if( hCheckFile == INVALID_HANDLE_VALUE)
	{
		bCheck = FALSE;
	}

	while( bCheck)
	{
//		i3String::SplitFileExt( wfd.cFileName, szExt, 16);
		i3::extract_fileext(wfd.cFileName, szExt);
		i3::remove_end_slash( szExt);
		if( i3::generic_is_iequal( szExt, "I3CHR")  )
		{
			bCheck = TRUE;
			break;
		}

		if( bCheck )
		{
			if( FindNextFile( hCheckFile, &wfd) == FALSE)
			{
				bCheck = FALSE;
				break;
			}
		}
	}

	FindClose( hCheckFile);

	return bCheck;
}


void ShowHelp()
{
	printf("\n");
	printf("\n=====================================================");
	printf("\n= AnimOpt.exe [인자1] [인자2] [인자3]               =");
	printf("\n=====================================================");
	printf("\n");
	printf("\n");
	printf("\n필수 파일 : AnimOptWork.ini");
	printf("\n생성되는 파일 : Opt된 *.i3a 과 *.i3AnimPack");
	printf("\n");
	printf("\n인자에 지정된 폴더(서브 포함)의 모든 애니메이션 파일(i3a)을 Opt하고 Pack파일을 생성하는 툴입니다.");
	printf("\n인자 추가시 Opt나 Pack 생성을 스킵할 수 있습니다.");
	printf("\n");
	printf("\n----------------------------------------------------------------------------------------");
	printf("\n");
	printf("\n[인자] -gui : i3AnimPack파일 내용을 보고, 검색해보고, 미디어와 검증할수있는 GUI툴 모드로 실행합니다.");
	printf("\n[인자] 애니메이션이 있는 폴더. 상대 경로입니다. (안에 모든 서브 폴더도 포함됩니다.)");
	printf("\n[인자] -NoOpt : Opt를 하지 않습니다.");
	printf("\n[인자] -NoPack : Pack파일을 생성하지 않습니다.");
	printf("\n");
	printf("\n 예1: F:\\Project\\PointBlank\\Media\\Weapon\\K-2폴더에 존재하는 모든 i3a파일을 Opt하고 Pack파일을 생성한다.");
	printf("\n    AnimOpt F:\\Project\\PointBlank\\Media\\Weapon\\K-2");
	printf("\n");
	printf("\n 예2: F:\\Project\\PointBlank\\Media\\Weapon\\K-2폴더에 존재하는 모든 i3a파일에 대한 Pack파일만 생성한다.");
    printf("\n AnimOpt -NoOpt F:\\Project\\PointBlank\\Media\\Weapon\\K-2");
	printf("\n");
	printf("\n 예2: F:\\Project\\PointBlank\\Media\\Weapon\\K-2폴더에 존재하는 모든 i3a파일을 Opt만 한다.");
    printf("\n AnimOpt -NoPack F:\\Project\\PointBlank\\Media\\Weapon\\K-2");
	printf("\n");
	printf("\n----------------------------------------------------------------------------------------");
	printf("\n반드시 Working folder가 설정된 AnimOptWork.ini파일이 실행파일 같은 폴더내에 존재해야 합니다.");
	printf("\n만약 파일이 없다면 메모장 등으로 아스키 텍스트 형식으로 ini 파일을 생성해 주세요.");
	printf("\nWorking folder는 절대 경로여야 합니다. ");
	printf("\n");
	printf("\nWorking folder 설정 예: F:\\Project\\PointBlank\\Media");
	printf("\n");
	printf("\n----------------------------------------------------------------------------------------");
	printf("\nnote1. 실행파일의 인자 순서는 상관없습니다.");
	printf("\nnote2. 애니메이션 폴더는 절대 경로가 아닌 상대 경로여야 합니다. (절대 경로시 에러가 납니다) ");
	printf("\nnote3. 실행후 콘솔 창이 사라지면 작업후 정상 종료 된것이며, 에러 메시지와 함꼐 콘솔 창이 ");
	printf("\n남아있다면 정상 실행에 실패한 것입니다.");
	printf("\n");
	printf("\n");
	printf("\n=====================================================");
	printf("\n= AnimOpt.exe [인자1] [인자2] [인자3]               =");
	printf("\n=====================================================");
	printf("\n 0. AnimOpt -gui : i3AnimPack파일 내용을 보고, 검색해보고, 미디어와 검증할수있는 GUI툴 모드로 실행합니다.");
	printf("\n 1. AnimOpt F:\\Project\\PointBlank\\Media\\Weapon\\K-2");
	printf("\n 2. AnimOpt -NoOpt F:\\Project\\PointBlank\\Media\\Weapon\\K-2");
	printf("\n 3. AnimOpt -NoPack F:\\Project\\PointBlank\\Media\\Weapon\\K-2");
	printf("\n 4. AnimOpt Script_Chara_REBEL_DFOX\\Chara_REBEL_DFOX_Assault.flt");
	printf("\n");

	getchar();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//	Main
///////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
	struct RAII_i3TerminateSys
	{
		~RAII_i3TerminateSys()
		{
			i3System::TerminateSys();
		}
	};
}

static void HideConsoleWindow()
{
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	HWND console = FindWindow("ConsoleWindowClass",path);

	if(IsWindow(console))
		ShowWindow(console,SW_HIDE); // hides the window
}

BOOL WINAPI ConsoleHandler(DWORD ctrl_type) { return TRUE;  }		// 크래시를 막기 위한 장치..

int _tmain(int argc, _TCHAR* argv[])
{

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

//	new int[625];

	::SetConsoleCtrlHandler(ConsoleHandler, TRUE);

	RAII_i3TerminateSys i3_terminate_system;			// RAII

	BOOL bNeedOpt = TRUE;
	BOOL bNeedPack = TRUE;
	BOOL bIncSubDir = TRUE;
	bool b16Bits = false;
	bool bRemoveTrans = false;		///< Translation Key를 삭제한다. (Bone Animation시 Rotation만 되는 것을 가정으로)
	char path[MAX_PATH] = "";
	char szExt[ 16];
	char szSaveFolder[ MAX_PATH] = {0,};
	char szSaveFileName[ MAX_PATH] = {0,};

	char temp[ MAX_PATH];

	i3MemoryInit();

	i3BaseRegisterMetas();
	i3SceneRegisterMetas();

	i3Error::Init();
	i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
	i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
	i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_CALLBACK | I3ERR_CHN_DEBUG);
	i3Error::SetChannelName( I3LOG_FATAL, "AnimOpt.log");
	i3Error::SetChannelName( I3LOG_WARN, "AnimOpt.log");

	if(argc == 1)
	{
		ShowHelp();
		return 0;
	}

	if(argc > 1 && argv[1] && !stricmp(argv[1], "-gui"))
	{
		HideConsoleWindow();
		AnimOptGUI_Run();

		return 0;
	}

	//	초기화 파일 읽기
	i3FileStream initFile;

	if( initFile.isFile( "WorkDir.ini") == FALSE)
	{
		printf("[ERROR] 작업을 더이상 진행할 수 없습니다 !!!!!!!!!!!!!!!\n");
		printf("툴이 실행되는 폴더내에 설정 파일(WorkDir.ini)이 존재하지 않습니다.\n");
		printf("Working folder가 설정된 WorkDir.ini파일이 실행파일 폴더내에 존재해야 합니다.\n");
		printf("WorkDir.ini파일에 work directory를 설정해 주세요.\n");
		printf("* 설정 예: F:\\Project\\PointBlank\\Media \n");			

		getchar(); 
		return 0;
	}
	else
	{
		initFile.Open( "WorkDir.ini", STREAM_READ | STREAM_SHAREREAD);

		initFile.ReadLine(szWorkFolder, MAX_PATH);

		WIN32_FIND_DATA wfd;
		if(INVALID_HANDLE_VALUE == FindFirstFile(szWorkFolder, &wfd))
		{
			printf("[ERROR] 지정된 WorkFolder가 올바르지 않습니다. 다시 설정해 주세요.\n");
			printf("WorkFolder : %s\n", szWorkFolder);

			getchar(); 
			return 0;
		}

		i3::remove_end_slash(szWorkFolder);
	}

	/*
	{
	i3AnimationPackFile file;

	file.LoadFromFile( "D:\\LastWarfare\\Project\\Media\\Chara\\Leopard\\Leopard.i3AnimPack");
	}
	*/

	//	get parameters
	for(INT32 i=1; i<argc; i++)
	{
		char * arg = argv[i];

		if( arg)
		{
			if(i3::generic_is_iequal("-NoOpt", arg) )
			{
				bNeedOpt = FALSE;
			}
			else if(i3::generic_is_iequal("-NoPack", arg) )
			{
				bNeedPack = FALSE;
			}
			else if ( i3::generic_is_iequal("-NoSubDir", arg) )
			{
				bIncSubDir = FALSE;
			}
			else if( i3::generic_is_iequal( "-16Bits", arg) )
			{
				b16Bits = false;
			}
			else if( i3::generic_is_iequal( "-RemoveTrans", arg) )
			{
				bRemoveTrans = true;
			}
			else
			{
				i3::string_ncopy_nullpad(szInputPath, arg, 256);
				i3::remove_blank_from_tail(szInputPath);
				i3::remove_end_slash(szInputPath);
			}
		}
	}

	INT32 iInputPathLen = i3::generic_string_size(szInputPath);

	if( iInputPathLen == 0)
	{
		printf("\n[ERROR] 지정된 폴더가 없습니다. 작업을 중단합니다!!!!!!!!!!!!!!!\n");

		getchar(); 
		return 0;
	}

//	i3String::SplitFileExt( szInputPath, szExt, 16);
	i3::extract_fileext(szInputPath, szExt);

	// flt 파일에 있는 경로를 Optimize합니다.
	if( i3::generic_is_iequal( szExt, "flt"))
	{
		char packingPath[ 512] = {0,};

	//	i3String::SplitFileName( szInputPath, szSaveFileName, FALSE);
		i3::extract_filetitle(szInputPath, szSaveFileName);

		i3FileStream fltFile;
		fltFile.Open( szInputPath, STREAM_READ | STREAM_SHAREREAD);

		//while( true )
		for (;;)
		{
			BOOL bIncSubDir2 = TRUE;

			b16Bits = true;
			bRemoveTrans = true;

			fltFile.ReadLine( packingPath, 512);
			
			INT32 iPackingLen = i3::generic_string_size( packingPath);
			if( iPackingLen <= 0)
			{
				break;
			}

			i3::to_lower(packingPath);
			INT32 pos;

			pos = i3::contain_string( packingPath, "-nosubdir");
			if( pos >= 0)
			{
				char * p=  strstr(packingPath, "-nosubdir");

				{
					i3::string_ncopy_nullpad( temp, &p[9], sizeof( temp));
					i3::string_ncopy_nullpad( p, temp, sizeof( temp));

					//i3String::RemoveBlankFromTail(packingPath);
					bIncSubDir2 = FALSE;
				}
			}

			pos = i3::contain_string( packingPath, "-no16bits");

			if( pos >= 0)
			{
				char * p=  strstr(packingPath, "-no16bits");
				
				i3::string_ncopy_nullpad( temp, &p[9], sizeof( temp));
				i3::string_ncopy_nullpad( p, temp, sizeof( temp));
				b16Bits = false;
			}

			pos = i3::contain_string( packingPath, "-16bits");

			if( pos >= 0)
			{
				char * p=  strstr(packingPath, "-16bits");
				
				i3::string_ncopy_nullpad( temp, &p[7], sizeof( temp));
				i3::string_ncopy_nullpad( p, temp, sizeof( temp));
				b16Bits = false;
			}

			pos = i3::contain_string( packingPath, "-noremovetrans");
			if( pos >= 0)
			{
				char * p = strstr( packingPath, "-noremovetrans");
				i3::string_ncopy_nullpad( temp, &p[14], sizeof( temp));
				i3::string_ncopy_nullpad( p, temp, sizeof( temp));
				bRemoveTrans = false;
			}

			pos = i3::contain_string( packingPath, "-removetrans");
			if( pos >= 0)
			{
				char * p = strstr( packingPath, "-removetrans");
				i3::string_ncopy_nullpad( temp, &p[12], sizeof( temp));
				i3::string_ncopy_nullpad( p, temp, sizeof( temp));
				bRemoveTrans = true;
			}
			
			i3::remove_blank_from_head( packingPath);

			if( FindAnim( packingPath, bIncSubDir2, bRemoveTrans, b16Bits) == FALSE)
			{
				printf( "[Error] 무엇일까요.........................................\n");
				break;
			}

			BOOL bFind = FALSE;
			char szCheckingFolder[ 512] = {0,};
			INT32 iCnt = 0;

			i3::safe_string_copy( szCheckingFolder, szWorkFolder, 512 );
			i3::generic_string_cat( szCheckingFolder, "\\");

			INT32 iWorkingFolderNameLen = i3::generic_string_size( szWorkFolder);

			// I3CHR 파일의 위치에 생성 파일을 save하기 위해 path를 찾는 듯....(누가 작업했는지 모름;;)
			while( !bFind)
			{
				if( packingPath[ iCnt ] == '\\' || packingPath[ iCnt ] == 0)
				{
					if( _CheckI3CHRatPath( (const char*) szCheckingFolder) )
						bFind = TRUE;
				}

				// NULL일 경우 빠져나가도록...여기서 못나가면 위의 조건에 만족하지 않을 경우 계속 while 돌면서 메모리 버퍼를 침범하게 된다;;;
				if( packingPath[ iCnt ] == 0)	break;	

				if( !bFind)
				{
					szCheckingFolder[ iWorkingFolderNameLen + 1 + iCnt] = packingPath[ iCnt];
					iCnt++;
				}
			}

			if( bFind && iCnt > 0 )
			{	//	I3CHR 파일을 찾을 경우 I3CHR 파일 위치를 저장 위치로 지정.
				i3::safe_string_copy( szSaveFolder, szWorkFolder, MAX_PATH );
				i3::generic_string_cat( szSaveFolder, "\\");
			//	i3String::InsertStr( szSaveFolder, packingPath, i3::generic_string_size( szSaveFolder), iCnt);
				i3::generic_string_ncat(szSaveFolder, packingPath, iCnt);
			}
			else
			{
				// I3CHR 파일 위치를 못찾을 경우 로딩한 flt 파일 위치를 저장 위치로 지정.
				i3::safe_string_copy( szSaveFolder, szInputPath, MAX_PATH );
			//	i3String::RemoveFileName( szSaveFolder);
				i3::remove_slash_filename(szSaveFolder);
			}
		}

		fltFile.Close();
	}
	else
	{
		//	build anim list in the directory
		if( !FindAnim( szInputPath, bIncSubDir, bRemoveTrans, b16Bits)) 
		{
			printf("\n[ERROR] 해당 파일들을 찾는데 실패했습니다. 작업을 중단합니다!!!!!!!!!!!\n");

			//	release
			ReleaseAnim();	

			getchar(); 
			return 0;
		}

		i3::safe_string_copy( szSaveFolder, szWorkFolder, MAX_PATH );
		i3::generic_string_cat( szSaveFolder, "\\");
		i3::generic_string_cat( szSaveFolder, szInputPath);
		i3::safe_string_copy( szSaveFileName, szInputPath, MAX_PATH );
	}

	if( AnimCount() == 0 )
	{
		printf( "\n 저장할 애니메이션이 없습니다.");
		return 0;
	}

	//	optimize
	if( bNeedOpt)
	{
		i3OptAnimation opt;

		for(INT32 i=0; i<AnimCount(); i++)
		{
			i3Animation * pAnim = GetAnim(i);

			opt.OptimizeAnim( pAnim, getAnimInfo( i)->_bRemoveTrans, getAnimInfo( i)->_b16Bits);

			/*char * pszPath = (char*) g_PathList.GetItem( i);

			i3String::MakeRelativePath( szWorkFolder, pszPath, path);

			if(pAnim->SaveToFile( pszPath))
			{
				printf("[%s] Optimize되어 저장되었습니다.\n", path);
			}
			else
			{
				printf("[ERROR] [%s] 저장에 실패했습니다.!!!!!!!!!!!!!!!!\n", path);
			}*/
		}
	}

	//	pack
	if( bNeedPack && (AnimCount() > 0))
	{		
		i3AnimationPackFile pack;

		char fname[MAX_PATH] = "";
				
		i3::safe_string_copy( temp, szSaveFileName, MAX_PATH );
		i3::generic_string_cat( temp, ".i3AnimPack"); 
//		i3String::SplitFileName(temp, fname, TRUE);
		i3::extract_filename(temp, fname);

		//	full path
		i3::safe_string_copy( path, szSaveFolder, MAX_PATH );
		i3::generic_string_cat( path, "\\");
		i3::generic_string_cat( path, fname);

		i3::vector<i3Animation*> animList;

		for(INT32 i = 0; i < AnimCount(); i++)
		{
			animList.push_back( GetAnim( i));
		}
		

		if( pack.SaveToFile(path, animList) > 0)
		{
			printf("\n[%s] PACK이 생성되었습니다.\n", path);
		}
		else
		{
			printf("\n[ERROR] [%s] PACK이 실패하였습니다. !!!!!!!!!!!!!!!!!!!!!!!\n", path);
		}
	}

	printf("\n작업 폴더 : %s\n", szWorkFolder);
	printf("애니메이션 폴더 : %s\n", szInputPath);
	printf("파일수 : %d\n", AnimCount());
	printf("모든 작업의 수행이 종료되었습니다.\n");

	//	release
	ReleaseAnim();	

	return 0;
}


HINSTANCE GetAnimOptModuleInstance() 
{ 
	if(!g_moduleInstance)
	{
		::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCSTR)GetAnimOptModuleInstance, (HMODULE*)&g_moduleInstance);
	}

	return g_moduleInstance;
}
