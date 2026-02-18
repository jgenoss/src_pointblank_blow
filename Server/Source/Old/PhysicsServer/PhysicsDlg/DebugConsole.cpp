// DebugConsole.cpp
//	
// Description:
//	debug consol module http://www.codeguru.com/cpp/v-s/debug/article.php/c1249/
//	
//
// Dependency: 
//		
//		
#include "pdh.h"
#include <stdio.h>
#include <stdlib.h>
#include "DebugConsole.h"


FILE* __fStdOut = NULL;
HANDLE __hStdOut = NULL;


// width and height is the size of console window, if you specify fname, 
// the output will also be writton to this file. The file pointer is automatically closed 
// when you close the app

void startConsoleWin(int width, int height, char* fname)
{

	AllocConsole();
	SetConsoleTitle("Debug Window");
	__hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	COORD co = {width,height};
	SetConsoleScreenBufferSize(__hStdOut, co);

	if(fname)
		__fStdOut = fopen(fname, "w");

}

// Use wprintf like TRACE0, TRACE1, ... (The arguments are the same as printf)
int console_printf(const char *fmt, ...)
{

	char s[2048];
	va_list argptr;
	int cnt;

	va_start(argptr, fmt);
	cnt = vsprintf(s, fmt, argptr);
	va_end(argptr);

	DWORD cCharsWritten;

	if(__hStdOut)
		WriteConsole(__hStdOut, s, strlen(s), &cCharsWritten, NULL);

	if(__fStdOut)
		fprintf(__fStdOut, s);

	return(cnt);

}
