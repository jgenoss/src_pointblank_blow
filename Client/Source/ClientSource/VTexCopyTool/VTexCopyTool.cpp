// VTexCopyTool.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "i3Base/string/system/traverse_file.h"
#include "i3Base/string/system/copy_file.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/extract_filename.h"

//
//  콘솔창에서 응용프로그램을 버튼으로 종료시에는 전역변수 내부소멸자와 doexit함수호출 순서가 바뀐다..
//  이로 인해서, 라이브러리에서 정해진 소거규칙이 깨지게 되며, 또한 메인함수의 리턴도 실행되지 않기 때문에
//  말그대로 비정상 종료로 간주해야될것으로 보인다..
//

BOOL WINAPI ConsoleHandler(DWORD ctrl_type) { return TRUE;  }		// 크래시를 막기 위한 장치..

bool BuildProcess(const char* szUIPath, const char* szVTexName, const char* media_path, const char* qa_path, const char* live_path);


int _tmain(int argc, _TCHAR* argv[])
{
	::SetConsoleCtrlHandler(ConsoleHandler, TRUE);

	--argc;	++argv;
	
	TCHAR input_char;

	if ( argc < 3 )
	{
		TCHAR moduleName[MAX_PATH];
		::GetModuleFileName( GetModuleHandle(NULL), moduleName, MAX_PATH );
		
		i3::stack_string moduleFileNameOnly;
		i3::extract_filename(moduleName, moduleFileNameOnly);

		printf_s("%s [1st arg] [2nd arg] [3rd arg] {4th arg} \n", moduleFileNameOnly.c_str());
		printf_s("\n");

		printf_s("1st arg : 0이면 V1 , 1이면 V1.1 이후 버전입니다.\n");
		printf_s("2nd arg : \"MEDIA 경로\"\n");
		printf_s("3rd arg : \"QA 경로\"\n");
		printf_s("4th arg : \"LIVE 경로\" (생략 가능) \n");

		scanf_s("%c", &input_char);
		return 0;
	}
	
	int   iMediaVer = ( ::atoi(argv[0]) != 0 ) ? 1 : 0;

	const TCHAR* media_path  = argv[1];
	const TCHAR* qa_path = argv[2];
	const TCHAR* live_path = (argc > 3) ? argv[3] : NULL;
	
	const char* aszUIPath[2] = { "UI_Temp", "UIRe" };
	const char* aszVTexName[2] = { "PBRe", "PointBlankRe" };

	const char* szUIPath = aszUIPath[iMediaVer];
	const char* szVTexName = aszVTexName[iMediaVer];

	printf_s("UI SubPath = \"%s\"\n", szUIPath);
	printf_s("Virtual Texture Name = \"%s\"\n", szVTexName);
		
	printf_s("\n");

	printf_s("MEDIA PATH = \"%s\"\n", media_path);
	printf_s("QA PATH = \"%s\"\n", qa_path);

	if (live_path)
		printf_s("Live PATH = \"%s\"\n", live_path);
	
	printf_s("\n");

	bool bSuccess = BuildProcess(szUIPath, szVTexName, media_path, qa_path, live_path);

	if (bSuccess)
	{
		printf_s("작업이 잘 끝났습니다.\n");
	}
	else
	{
		printf_s("작업이 중단 되었습니다.\n");
	}
	
	scanf_s("%c", &input_char);
	
	return 0;
}

#define MAX_UI_SPRITE_COUNT 9600		// 16bit 지원 그래픽카드에서는 10900개까지 가능
const char g_szClassName[] = "myWindowClass";


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}


bool BuildProcess(const char* szUIPath, const char* szVTexName, const char* media_path, const char* qa_path, const char* live_path)
{
	i3MemoryInit();
	
	i3Error::Init();
	i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
	i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
	i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );
	i3Error::SetChannelName( I3LOG_NOTICE, "VTexCopyTool.log");
	i3Error::SetChannelName( I3LOG_FATAL, "VTexCopyTool.log");
	i3Error::SetChannelName( I3LOG_WARN, "VTexCopyTool.log");

	i3FrameworkRegisterMetas();

	printf_s("Media 경로의 Image/WeaponShape폴더와 i3UIL파일을 QA 경로로 복사 작업.\n");

	struct filecallback_media : i3::traverse_file_callback_mb
	{
		i3::vector<i3::string>& filename_list;
		const char*				szUIPath;
		filecallback_media(const char* szUIPath, i3::vector<i3::string>& filename_list) : szUIPath(szUIPath), filename_list(filename_list) {}

		virtual void on_file(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, 
			const i3::rc_string& subdir_path)
		{
			i3::string filename = szUIPath;
			filename += "/Image/WeaponShape/";

			if ( !subdir_path.empty() )
			{
				filename +=	subdir_path;
				filename += '/';
			}

			filename += fd.cFileName;
			filename_list.push_back(filename);
		}

		virtual bool on_dir(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, const i3::rc_string& subdir_path)
		{
			return true;
		}
	};

	i3::vector<i3::string> filename_list;

	{
		filecallback_media cb_media(szUIPath, filename_list);
		i3::string AbsMediaImageFolder = media_path;
		AbsMediaImageFolder += '/';
		AbsMediaImageFolder += szUIPath;
		AbsMediaImageFolder += "/Image/WeaponShape";
		i3::traverse_file( AbsMediaImageFolder, &cb_media);
	}

	if ( filename_list.empty() )
	{
		printf_s("Media 경로 Image/WeaponShape폴더를 찾을 수 없거나 파일이 없어서 종료.\n");
		return false;
	}
	
	const size_t num_filename = filename_list.size();

	for (size_t i = 0 ; i < num_filename ; ++i)
	{
		i3::copy_file(media_path, filename_list[i], qa_path);

		printf_s("copied : \"%s\" \n", filename_list[i].c_str());
	}

	printf_s("Image 폴더 복사 작업 완료.\n");


	i3::string strVTexTitle = szUIPath;
	strVTexTitle += '/';
	strVTexTitle += szVTexName;

	i3::string stri3UilPath = strVTexTitle;
	stri3UilPath += ".i3Uil";

	i3::string stri3Subset = strVTexTitle;
	stri3Subset += ".i3Subset";

	// media_path나 qa_path가 제대로 존재하지 않은 경우의 디렉토리 생성 버그가 아직 남아있음..
	bool bi3UILExist =i3::copy_file(media_path, stri3UilPath, qa_path);

	printf_s("copied : \"%s\" \n", stri3UilPath.c_str() );

//	bi3UILExist &= i3::copy_file(media_path, stri3Subset, qa_path);

//	printf_s("copied : \"%s\" \n", stri3Subset.c_str() );

	if (bi3UILExist == false)
	{
		printf_s("Media 경로 %s를 찾을 수 없어서 종료.\n", stri3UilPath.c_str() );
		return false;
	}
	
	char pszOldWindowTitle[MAX_PATH];
	GetConsoleTitle(pszOldWindowTitle, MAX_PATH);

	HWND hwndFound=FindWindow(NULL, pszOldWindowTitle);

	HMODULE module = GetModuleHandle(NULL);

	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = module;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wc);

	HWND hWnd= CreateWindowEx(
		0,
		g_szClassName,
		"",
		WS_POPUP ,
		CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
		hwndFound, NULL, module, NULL);


	::ShowWindow(hWnd, SW_HIDE);
	::UpdateWindow(hWnd);
	
	i3UI::setToolDevMode(true);			// 이것 없으면 파일 저장이 잘 안되는게 많음..(i3VTexSub파일 생성 실패)

	i3Viewer*    pViewer    = i3Viewer::new_object();

	i3VideoInfo video;
	video.m_Width					= 1;
	video.m_Height					= 1;
	video.m_bFullScreen				= FALSE;
	video.m_bVSync					= TRUE;
	video.m_MultiSampling			= 0;
	video.m_MultiSamplingQuality	= 0;
	video.m_RenderTargetFormat = I3G_IMAGE_FORMAT_RGBX_8888;

	pViewer->setFixedResolution( true);
	pViewer->Create(hWnd, &video);

	i3Framework* pFramework = i3Framework::new_object();

	pFramework->Create(pViewer);

	i3ResourceFile::SetWorkingFolder(const_cast<char*>(qa_path));
	i3UIManager * pUI = i3UI::getManager();

	{

		i3::string qa_uil_path = qa_path;
		qa_uil_path += '/';
		qa_uil_path += stri3UilPath;

		bool res = pUI->OpenTemplateLibrary( qa_uil_path.c_str() );
	
		printf_s("i3UIL파일 엔진에서 읽기 완료.\n");

		I3ASSERT(res == true);
	}

	i3ResourceManager * pResManager = pFramework->GetResourceManager();

	printf_s("QA경로에서 신규 등록 텍스쳐를 뒤져봅니다.\n");
	
	bool bNewTexRegExist = false;

	for (size_t i = 0 ; i < num_filename ; ++i)
	{
		INT32 idx = i3UI::getVirtualTexture()->FindRefTexture( filename_list[i].c_str() );

		if (idx == -1)			// 등록 안된 텍스쳐는 등록시킴..
		{
			i3ResourceObject * pResObj = pResManager->LoadResource( filename_list[i].c_str(), 0);		//2.0

			i3Texture * pTexture = NULL;

			if( i3::kind_of<i3GameResTexture*>(pResObj)) //->IsTypeOf( i3GameResTexture::static_meta()))
			{
				pTexture = ((i3GameResTexture*)pResObj)->getTexture();
			}
			
			if (pTexture)
			{
				INT32 x, y;
				if ( i3UI::getVirtualTexture()->PlaceTexture(pTexture, &x, &y) )
				{
					i3UI::getVirtualTexture()->Flush();
					
					printf_s("\"%s\" file 자동 등록...\n", filename_list[i].c_str() );
					bNewTexRegExist = true;
				}
				pTexture->Release();
			}
		}

	}

	if (bNewTexRegExist == false)
	{
		printf_s("새로 등록된 텍스쳐는 없는 것 같습니다.\n");
	}

	printf_s("QA경로에서 Virtual 텍스쳐 Adjust작업 시작.\n");

	if( i3UI::getVirtualTexture() != NULL)
		i3UI::getVirtualTexture()->DeleteWrongTexture();
	
	i3UI::AdjustVirtualTexture();			// 가장 시간 많이 걸리는 함수..

	printf_s("Virtual 텍스쳐 Adjust작업 완료.\n");

	i3UILibrary * pUILib = i3UI::getUILibrary();

	I3ASSERT(pUILib);

	{   // i3UIL파일 재저장...
		i3XMLFile File;
		char conv[MAX_PATH], szFull[MAX_PATH];

		if (pUI->GetLocale()->length() > 0)
		{
			i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), pUILib->GetName(), conv );
			i3::snprintf( szFull, MAX_PATH -1, "%s/%s/%s", i3ResourceFile::GetWorkingFolder(), pUI->GetLocaleUIFolder()->c_str(), conv);
		}
		else
		{
			i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), pUILib->GetName(), conv);
			i3::snprintf( szFull, MAX_PATH -1, "%s/%s", i3ResourceFile::GetWorkingFolder(), conv);
		}

		File.setKeyObject( pUILib);
		if( File.Save(szFull))
		{
			I3TRACE("Template Library Saved\n");
#ifdef I3_DEBUG
			pUILib->setEdited( false);
#endif
			printf_s("QA경로의 i3UIL파일 재저장 완료. \n");
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN,"Couldn't save Template Library to %s\n", szFull);
		}
				
	}	

	pFramework->Release();
	pViewer->Release();

	i3System::TerminateSys();

	::DestroyWindow(hWnd);

	if (live_path)			// 라이브 경로 주어진 경우...복사해야함..
	{
		printf_s("QA경로 버추얼텍스쳐 파일들을 Live경로로 복사 작업 시작.\n");

		struct filecallback_qa : i3::traverse_file_callback_mb
		{
			i3::vector<i3::string>& filename_list;
			const char*				szUIPath;

			filecallback_qa(const char*	szUIPath, i3::vector<i3::string>& filename_list) : szUIPath(szUIPath), filename_list(filename_list) {}

			virtual void on_file(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, 
				const i3::rc_string& subdir_path)
			{
				i3::string filename = szUIPath; 
				filename += '/';
				filename += fd.cFileName;
				filename_list.push_back(filename);
			}

			virtual bool on_dir(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, const i3::rc_string& subdir_path)
			{
				return false;
			}
		};
		
		i3::vector<i3::string>	vTexSubFileList;

		{
			filecallback_qa cb_qa(szUIPath, vTexSubFileList);
			i3::string AbsQAUIReFolder;
			
			i3::sprintf(AbsQAUIReFolder, "%s/%s/WeaponShape*.i3VTexSub", qa_path, szUIPath);
			
			i3::traverse_file( AbsQAUIReFolder, &cb_qa, false);
		}

		const size_t num_filename = vTexSubFileList.size();

		for (size_t i = 0 ; i < num_filename ; ++i)
		{
			if ( i3::copy_file(qa_path, vTexSubFileList[i], live_path) )
			{
				printf_s("copied : \"%s\" \n", vTexSubFileList[i].c_str());
			}
		}
		


		i3::string stri3RefTex = strVTexTitle;
		i3::string stri3VTex    = strVTexTitle;

		stri3RefTex += ".i3RefTex";
		stri3VTex   += ".i3VTex";

		i3::copy_file(qa_path, stri3UilPath, live_path);
		printf_s("copied : \"%s\" \n", stri3UilPath.c_str());
		i3::copy_file(qa_path, stri3Subset, live_path);
		printf_s("copied : \"%s\" \n", stri3Subset.c_str());
		i3::copy_file(qa_path, stri3RefTex, live_path);
		printf_s("copied : \"%s\" \n", stri3RefTex.c_str());
		i3::copy_file(qa_path, stri3VTex, live_path);
		printf_s("copied : \"%s\" \n", stri3VTex.c_str());
		
		printf_s("버추얼텍스쳐 파일들 복사 작업 완료.\n");
	}

	return true;
}
