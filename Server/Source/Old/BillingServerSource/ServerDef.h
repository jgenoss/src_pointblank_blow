#ifndef __SERVER_DEF_H__
#define	__SERVER_DEF_H__

#include "ModuleLogFile.h"
#include "System.h"
#include "ServerDefine.h"


//////////////////////////////////////////////////////////////////////////
//±Û·Î¹ú º¯¼ö 
extern char			g_pCCurrentPath[SERVER_STRING_COUNT]; 
extern INT32		g_i32StartServer;
extern UINT32		g_ui32StartTime;
extern INT32		g_i32MainLogThreadIdx;

//±Û·Î¹ú ÇÔ¼ö 
void				g_SetCurrentPath( char* pPath );
INT32				g_InitServer( void );
INT32				g_StartServer( void );
INT32				g_UpdateServer( void );
void				g_DestroyServer( void );

#endif
