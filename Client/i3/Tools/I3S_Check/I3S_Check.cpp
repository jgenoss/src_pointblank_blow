// I3S_Check.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "I3S_Check.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "GlobalVar.h"
#include "Viewer.h"

bool g_bWaitKey = true;

// The one and only application object

CWinApp theApp;

using namespace std;

void ProcessCmd( int argc, TCHAR * argv[])
{
	INT32 i;

	for( i = 1; i < argc; i++)
	{
		char * pszArg = argv[i];

		if( (pszArg[0] == '/' ) || (pszArg[0] == '-'))
		{
			// Option
			switch( pszArg[1])
			{
				case 'H' :
				case 'h' :
					g_Checker.setCheckHiddenState( true);
					break;

				case 'W' :
				case 'w' :
					g_Checker.setWorkPath( &pszArg[2]);
					break;

				case 'O' :
					g_Checker.setOutputPath( &pszArg[2]);
					break;

				default :
					if( i3::generic_is_iequal( pszArg, "/Index") )
					{
						g_Checker.m_bCheckIndexedGeometry = false;
					}
					else if( i3::generic_is_iequal( pszArg, "/Tangent") )
					{
						g_Checker.m_bCheckTangentBasis = false;
					}
					else if( i3::generic_is_iequal( pszArg, "/TFormat") )
					{
						g_Checker.m_bCheckTextureFormat = false;
					}
					else if( i3::generic_is_iequal( pszArg, "/BBox") )
					{
						g_Checker.m_bCheckBoundBox = false;
					}
					else if( i3::generic_is_iequal( pszArg, "/BSphere") )
					{
						g_Checker.m_bCheckBoundSphere = false;
					}
					else if( i3::generic_is_iequal( pszArg, "/VBlend") )
					{
						g_Checker.m_bCheckVertexBlend = false;
					}
					else if( i3::generic_is_iequal( pszArg, "/TexSet") )
					{
						g_Checker.m_bCheckTexCoordSet = false;
					}
					else if( i3::generic_is_iequal( pszArg, "/VColor") )
					{
						g_Checker.m_bCheckVertexColor = false;
					}
					else if( i3::generic_is_iequal( pszArg, "/STx") )
					{
						g_Checker.m_bCheckStaticTransform = false;
					}
					else if( i3::generic_is_iequal( pszArg, "/NoWait") )
					{
						g_bWaitKey = false;
					}
					break;
			}
		}
		else
		{
			g_Checker.setTargetPath( pszArg);
		}
	}
}

void PrintUsage(void)
{
	printf( "\n  Usage : I3S_Check <Directory or File> [OPTIONS]\n");
	printf( "\n  [OPTIONS] : \n");
	printf( "   /H         : 모든 I3S를 검사함 (숨긴 파일 포함).\n");
	printf( "   /W<Path>   : 작업 디렉토리.\n");
	printf( "   /O<Path>   : 결과 파일명.\n");
	printf( "   /Index     : Non-Indexed Geometry 검사 안함.\n");
	printf( "   /Tangent   : Tangent Basis component 검사 안함.\n");
	printf( "   /TFormat   : Texture Format 확인 안함.\n");
	printf( "   /BBox      : BoundBox 확인 안함.\n");
	printf( "   /BSphere   : BoundSphere 확인 안함.\n");
	printf( "   /VBlend    : Vertex Blend Limit 확인 안함.\n");
	printf( "   /TexSet    : Texture Coordinates Set 확인 안함.\n");
	printf( "   /VColor    : Vertex Color 확인 안함.\n");
	printf( "   /STx       : Static Transform Node 확인 안함.\n");
	printf( "   /NoWait    : 처리가 끝난 후, 키 입력을 기다리지 않음.\n");
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	printf( "i3Engine Scene-graph file checker. version 1.0\n");
	printf( "Copyright 2008 Zepetto Inc.\n\n");

	if( argc < 2)
	{
		PrintUsage();
		return -1;
	}

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		i3MemoryInit();
		i3Error::Init();

		i3Error::SetChannelName( I3LOG_NOTICE, "I3S_Check.log" );
		i3Error::SetChannelName( I3LOG_FATAL, "I3S_Check.log");
		i3Error::SetChannelName( I3LOG_WARN, "I3S_Check.log");

        i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE | I3ERR_CHN_CALLBACK);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );

		i3BaseRegisterMetas();
		i3MathRegisterMetas();
		i3InputRegisterMetas();
		i3SoundRegisterMetas();
		i3GfxRegisterMetas();
		i3SceneRegisterMetas();
		i3FrameworkRegisterMetas();

		i3ResourceFile::setExternRefResolveCallback( NULL, NULL);

		InitViewer();

		ProcessCmd( argc, argv);

		nRetCode = g_Checker.Check();

		CloseViewer();
	}

	if( g_bWaitKey)
	{
		getchar();
	}

	return nRetCode;
}
