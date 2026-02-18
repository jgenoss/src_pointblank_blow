#include "i3Base.h"
#include "i3Gfx.h"
#include "i3Math.h"
#include "i3Scene.h"
#include "i3Framework.h"

i3Viewer *			g_pViewer;

char szFileName[256] = "";

SCE_MODULE_INFO( i3Viewer, 0, 1, 0);

// 메인 쓰레드
int MathThread(
	unsigned int _ArgNum, // 인자 크기
	void* _ArgData // 인자 데이터 포인터
)
{
	I3TRACE( "File name : %s\n", szFileName );

	i3MemoryInit();
	i3Edram::Create(500); 

	g_pViewer = i3Viewer::NewObject();
	I3ASSERT( g_pViewer != NULL );

	if( g_pViewer->Create( (void *) NULL, 0, 0, TRUE) == FALSE)
	{
		I3TRACE( "Could not create a Render Context."); 
		return -1;
	}

	g_pViewer->EnableDebugControl();

	I3TRACE( "load start\n");
	if( g_pViewer->LoadI3Scene( szFileName ) == FALSE )
	{
		I3TRACE( "Could not read I3 file.\n");
		return -1;
	}
	I3TRACE( "load end\n");

	while( 1) 
	{
		g_pViewer->Render();
	}

	return 0;
}

int sce_newlib_heap_kb_size = 21 * 1024; // 40MB

// 메인 함수
int main( int argc, char * argv[])
{
	if( argc != 2 )
	{
		I3TRACE( "ERROR : File name is not specified.\n" );
#ifdef I3_PSP
		printf( "usage : i3Viewer.prx [File]\n" );
#endif
		return -1;
	}
	else
	{
		strcpy( szFileName, argv[1] );
		//I3TRACE( "File name : %s\n", szFileName );
	}

	SceUID main_thread = 0;

	main_thread = sceKernelCreateThread( "main", MathThread,
		SCE_KERNEL_USER_LOWEST_PRIORITY, 256*100,
		SCE_KERNEL_TH_USE_VFPU, NULL );

	sceKernelStartThread( main_thread, 0, NULL );

	sceKernelSleepThread();

	return 0;
}
