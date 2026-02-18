#if !defined( I3_OPTIMIZER_H_)
#define I3_OPTIMIZER_H_

#include "stdafx.h"

#include "i3Base.h"
#include "i3Framework.h"
#include "i3Scene.h"
#include "i3Gfx.h"
#include "i3Opt.h"

#include "i3OptimizerDefine.h"


enum _tagOutputFileType
{
	OUTPUT_FILE_NO = 0,
	OUTPUT_FILE_NEW,
	OUTPUT_FILE_OVER,
};

typedef struct _tagOptimizerInfo 
{	
	BOOL			m_bOutputInfo;		//	진행중 정보 표시 여부

	INT32			m_nOutputFile;
	BOOL			m_bIgnoreRun;		//	실패해도 계속 진행 여부 (default : FALSE)

} OptimizerInfo;


BOOL		MainInit(void);
void		Cleanup(void);
void		SetDefaultOptimize(void);

//
i3Node *	_openMeshFile( const char * pszFileName);
BOOL		_saveMeshFile( const char * pszFileName);
void		_preRead( i3IniParser * pParser);
BOOL		_processFile( const char * pszPath);
BOOL		_runOpt( i3IniParser * pParser, const char * pszFileName = NULL);
void		_printHelp(void);
BOOL		_checkIgnoreRun( void);

//	Create Window
LRESULT		CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
ATOM		MyRegisterClass(HINSTANCE hInstance);
BOOL		InitInstance(HINSTANCE hInstance, int nCmdShow);

#endif

