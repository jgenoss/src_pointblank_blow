#include "stdafx.h"
#include "Memory.h"
#include <stdlib.h>

UINT32 s_ui32MemorySize;

void NSM_nsMemory::Create(void)
{
	s_ui32MemorySize = 0;
	return; 
}

void NSM_nsMemory::Destroy(void)
{
	return; 
}

void * NSM_nsMemory::Alloc(INT32 nBytes, const char * FileName ,INT32 Line)
{
	void * pRv = malloc( nBytes ); 
	s_ui32MemorySize += nBytes;
	return pRv; 
}

void * NSM_nsMemory::Realloc(void * pPointer, INT32 nBytes, const char * FileName, INT32 Line)
{
	void * pRv = realloc( pPointer, nBytes );
	return pRv; 
}

void NSM_nsMemory::Free(void * pPointer, const char * pszFile, INT32 line )
{
	free( pPointer ); 	
	return; 
}

