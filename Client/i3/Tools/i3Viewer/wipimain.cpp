#include "i3Base.h"
#include "i3Gfx.h"
#include "i3Math.h"
#include "i3Scene.h"
#include "i3Framework.h"
#include "i3Input.h"

i3Viewer *			g_pViewer = NULL;

//char szFileName[256] = "redCar32.I3S";
//char szFileName[256] = "xyz.I3S";					// Passed
//char szFileName[256] = "xyz_opt.I3S";
//char szFileName[256] = "teapot.I3S";				// Passed
//char szFileName[256] = "teapot_tristrip.I3S";		// Half Passed
//char szFileName[256] = "teapot_index.I3S";		// Passed
//char szFileName[256] = "ball.I3S";				// Passed
// char szFileName[256] = "vftest.I3S";				// Passed
char szFileName[256] = "WalkFront_IStrip.I3S";
//char szFileName[256] = "back_base.I3S";


// 메인 함수
int main( int argc, char * argv[])
{	
	i3MemoryInit();

	i3FrameworkRegisterMetas();

	/*
	{
		UINT32 i;
		REAL32 r1, r2, r3;
		FIXED32 f1, f2, f3;

		r1 = 2.0f;
		r2 = 3.0f;
		r3 = 0.0f;

		f1 = i3FXD::FromReal( 2.0f);
		f2 = i3FXD::FromReal( 3.0f);
		f3 = 0;

		START_PROFILE( 40);
		for( i = 0; i < 100000; i++)
		{
			r3 += r1 * r2;
		}
		END_PROFILE( 40);

		I3TRACE( "%d\n", (INT32)(r3 * 1000.0f));

		START_PROFILE( 41);
		for( i = 0; i < 100000; i++)
		{
			f3 += i3FXD::Mul( f1,  f2);
		}
		END_PROFILE( 41);

		I3TRACE( "%d\n", (INT32)( i3FXD::ToReal(f3) * 1000.0f));

		RESET_PROFILE();

		REPORT_PROFILE();
	}
	*/

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

	return 0;
}

void Render(void)
{
	g_pViewer->Render();
}


//////////////////////////////////////////////////////////////////////////

#if defined ( I3_WIPI)

extern "C"
{
	typedef enum
	{
		aaa,
	} _Unwind_Reason_Code;

	extern _Unwind_Reason_Code _Unwind_SjLj_Resume(struct _Unwind_Exception *exc);

	_Unwind_Reason_Code _Unwind_SjLj_Resume_or_Rethrow (struct _Unwind_Exception *exc)
	{
		return _Unwind_SjLj_Resume(exc);
	}

	void end(void)
	{
		//
	}

	MCTimer tm;

	void Timer_CB(MCTimer* ptm, void* parm)
	{
		Render();

		OEMC_knlSetTimerEx( &tm, 10, NULL, 1);
	}

	void handleCletEvent(int type, int param1, int param2)
	{
		i3InputKeyboard::UpdateCletEvent( type, param1, param2);

		switch( type )
		{
		case MV_KEY_PRESS_EVENT:	break;
		case MV_KEY_RELEASE_EVENT:	break;
		}
	}

	void startClet(int argc, char * argv[])
	{
		OEMC_gCOP_Initialize();

		main(0, NULL);

		MC_knlDefTimer(&tm, Timer_CB);
		OEMC_knlSetTimerEx( &tm, 10, NULL, 1);

		I3TRACE("[GIGA] startClet complete..-----------------------------------------------\n");
	}

	void pauseClet( void)
	{
	}

	void resumeClet( void)
	{
	}

	void destroyClet( void)
	{
		I3_SAFE_RELEASE( g_pViewer);
		I3TRACE( "[GIGA] destroyClet\n");

		i3ElementBase::DumpObjects();

		MC_knlUnsetTimer(&tm);		
		OEMC_gCOP_Terminate();
	}

	void paintClet(int x, int y, int w, int h)
	{
	}

}

#endif
