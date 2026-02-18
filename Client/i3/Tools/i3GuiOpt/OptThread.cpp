#include "stdafx.h"
#include "OptThread.h"
#include "GlobalVar.h"

I3_CLASS_INSTANCE( COptThread); //, i3Thread);

COptThread::COptThread(void)
{
	m_hwndOwner = NULL;
	m_bExit = false;
}

COptThread::~COptThread(void)
{
}

void COptThread::REPORT( INT32 msg, WPARAM wParam, LPARAM lParam)
{
	PostMessage( m_hwndOwner, msg, wParam, lParam);
}

void COptThread::DoOptimize( const char * pszPath)
{
	i3SceneFile file;
	i3SceneGraphInfo * pSgInfo;
	INT32 i;

	if( file.Load( pszPath) == STREAM_ERR)
	{
		return;
	}

	pSgInfo = file.getSceneGraphInfo();
	if( pSgInfo == NULL)
	{
		return;
	}

	pSgInfo->SetName( pszPath);

	I3TRACE( "OPT Start ------ %s\n", pszPath);

	pSgInfo->AddRef();

	for( i = 0; i < g_pOptConfig->getCount(); i++)
	{
		i3SceneOptimizer * pOpt = g_pOptConfig->getOpt( i);

		pOpt->Optimize( pSgInfo);

		I3_REF_CHANGE( pSgInfo, pOpt->getSg());

		REPORT( WM_OPT_OPT, i + 1);
	}

	pSgInfo->Release();

	I3TRACE( "OPT End ------\n");
}
/*
namespace
{
	// 쓰레드 내부에 g_pEnvSet접근이 있어서 일단 다음과 같이 RAII처리한다.(2012.09.10.수빈)
	struct RAII_g_pEnvSet
	{
		RAII_g_pEnvSet() 
		{   
			g_pEnvSet->AddRef();
		}
		~RAII_g_pEnvSet() 
		{   
			g_pEnvSet->Release();
		}
	};

}
*/
UINT32 COptThread::OnRunning( void * pData)
{
//	RAII_g_pEnvSet _raii_;

	char szFull[MAX_PATH];
	const char * pszWorkDir;

	g_bProcessing = true;

	pszWorkDir = g_pEnvSet->getCurWorkDir();

	size_t i;

	for( i = 0; (i < g_FileList.size()) && (m_bExit == false); i++)
	{
		REPORT( WM_OPT_FILE, i);

		const char * pszTargetFile = g_FileList[i].c_str();

		if( i3System::isAbsolutePath( pszTargetFile))
			i3::snprintf( szFull, sizeof(szFull), "%s", g_FileList[i].c_str());
		else
		{
			if( pszWorkDir != NULL)
				i3::snprintf( szFull, sizeof(szFull), "%s/%s", pszWorkDir, g_FileList[i].c_str());
			else
				i3::snprintf( szFull, sizeof(szFull), "%s", g_FileList[i].c_str());
		}
			

		DoOptimize( szFull);
	}

	g_bProcessing = false;

	if( i == g_FileList.size())
	{
		REPORT( WM_OPT_END);
	}
	else
	{
		REPORT( WM_OPT_CANCEL);
	}

	return 0;
}
