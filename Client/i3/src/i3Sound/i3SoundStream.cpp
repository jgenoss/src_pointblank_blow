#include "stdafx.h"
#include "stdio.h"
#include "i3SoundStream.h"

I3_CLASS_INSTANCE( i3SoundStream);

bool i3SoundStream::Create( const char * pszFileName, WCHAR32 nBufferSize, WCHAR32 nGap, WCHAR32 nReadOnce)
{
	if( pszFileName == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "StreamFileName is nullptr");
		return false;
	}

	SetName( pszFileName);

	OnCreate();

	return true;
}


