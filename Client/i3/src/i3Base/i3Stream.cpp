#include "i3CommonType.h"
#include "i3Memory.h"
#include "i3String.h"
#include "i3Stream.h"
#include "i3Debug.h"
#include "i3Macro.h"
#include "i3Performance.h"
#include "i3ErrorHandler.h"
#include "i3PackFileSystem.h"
#include "i3Base/string/ext/generic_string_size.h"

#include <stdio.h>
#include <stdarg.h>

#define UNITSZ		(65536 * 2)

#ifdef I3_PSP
	#define PSP_HDD_NAME "host0:"
	#define PSP_UMD_NAME "disc0:/PSP_GAME/USRDIR/"
	static char s_szDiskPrefix[64] = PSP_HDD_NAME;
#endif //I3_PSP

I3_CLASS_INSTANCE( i3Stream);


UINT32 i3Stream::CopyTo( i3Stream *Stream, UINT32 Length)
{
	char *Buf;
	UINT32 Accm, Rc, ReadBytes;

	Buf = (char *)i3MemAlloc( UNITSZ);  
	if( Buf == nullptr)return STREAM_ERR;

	Accm = 0;

	while( Accm < Length) 
	{
		if((Accm+UNITSZ) < Length)
			ReadBytes = UNITSZ;
		else
			ReadBytes = Length - Accm;

		if( ReadBytes > 0)
		{
			Rc = Read( Buf, ReadBytes);
			if( Rc == STREAM_ERR) 
			{
				i3MemFree( Buf );
				return STREAM_ERR;
			}
			if( Stream->Write( Buf, ReadBytes) == STREAM_ERR)
			{
				i3MemFree( Buf);
				return STREAM_ERR;
			}

			Accm += Rc;
		}
	}

	i3MemFree( Buf );

	return Accm;
}

UINT32 i3Stream::CopyFrom( i3Stream *Stream, UINT32 Length)
{
	char *Buf;
	UINT32 Accm, Rc, ReadBytes;

	//INT32 midx; 
	//midx = i3MemAlloc( UNITSZ);
	Buf = (char *) i3MemAlloc( UNITSZ);
	if( Buf == nullptr)return STREAM_ERR;

	Accm = 0;

	while( Accm < Length) 
	{
		if((Accm+UNITSZ) < Length)
			ReadBytes = UNITSZ;
		else
			ReadBytes = Length - Accm;

		if( ReadBytes > 0) {
			Rc = Stream->Read( Buf, ReadBytes);
			if( Rc == STREAM_ERR) 
			{
				i3MemFree( Buf );
				return STREAM_ERR;
			}

			if( Write( Buf, ReadBytes) == STREAM_ERR)
			{
				i3MemFree( Buf);
				return STREAM_ERR;
			}

			Accm += Rc;
		}
	}

	i3MemFree( Buf );

	return Accm;
}

UINT32 i3Stream::ReadLine( char * pStr, INT32 BufLength )
{
	INT32 count;
	UINT32 rc;

	I3ASSERT( BufLength != 0 );

	count = 0;

	do
	{
		rc = Read( pStr, sizeof( char));
		if( rc == STREAM_ERR)
		{
			switch( GetLastError())
			{
				case STREAM_EOS :
					goto ExitPart;

				default:
					count = STREAM_ERR;
					goto ExitPart;

			}
		}

		if( *pStr == '\r')
			continue;

		count++;

		if(*pStr == '\n')
		{
			goto ExitPart;
		}

		if( BufLength > 0 )
		{
			if( count < BufLength )
			{
				pStr++;
			}
		}
		else
		{
			pStr++;
		}
	} while( rc != STREAM_ERR);
	
ExitPart:
	*pStr = 0;

	return count;
}

UINT32 i3Stream::ReadLineW( WCHAR16 * pStr, INT32 BufLength)
{
	INT32 count;
	UINT32 rc;

	I3ASSERT( BufLength != 0 );

	count = 0;

	do
	{
		rc = Read( pStr, sizeof( WCHAR16));
		if( rc == STREAM_ERR)
		{
			switch( GetLastError())
			{
				case STREAM_EOS :
					goto ExitPart;

				default:
					count = STREAM_ERR;
					goto ExitPart;

			}
		}
		
		if( *pStr == 0xfeff ||	//	UTF-16(little endian) 헤더 구분.
			*pStr == 0xfffe)	//	UTF-16(big endian) 헤더 구분.
			continue;	

		if( *pStr == L'\r')
			continue;

		count++;

		if( *pStr == L'\n')
		{
			goto ExitPart;
		}

		if( BufLength > 0 )
		{
			if( count < BufLength )
			{
				pStr++;
			}
		}
		else
		{
			pStr++;
		}
	} while( rc != STREAM_ERR);
	
ExitPart:
	*pStr = 0;

	return count;
}

UINT32 i3Stream::WriteLine( const char * pStr)
{
	INT32 Length;

	Length = i3::generic_string_size( pStr);

	return Write( pStr, Length);
}

UINT32 i3Stream::WriteLineW( const WCHAR16 * pStr)
{
	INT32 Length;

	Length = i3::generic_string_size( pStr);

	return Write( pStr, sizeof( WCHAR16) * Length);
}

UINT32 i3Stream::Printf( char *szBuf, const char *szFmt, ... )
{
	va_list marker;
	va_start( marker, szFmt );
	vsprintf( szBuf, szFmt, marker);

	return WriteLine( szBuf );
}
