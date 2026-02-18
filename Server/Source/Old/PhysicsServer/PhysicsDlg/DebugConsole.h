// DebugConsole.h
//
// YouJong Ha
//	Last update : 2010-10-04 (yyyy:mm:dd)
//	
// Description:
//	console window Áö¿ø
//
//
// Dependency: 
//		
//	
#ifndef _DebugConsole_H
#define _DebugConsole_H

#ifdef YJ_DEBUG_CONSOLE
void startConsoleWin(int width=80, int height=25, char* fname = NULL);
int console_printf(const char *fmt, ...);
#endif 

#endif