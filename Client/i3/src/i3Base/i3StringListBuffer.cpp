#include "i3CommonType.h"
#include "i3StringListBuffer.h"
#include "i3String.h"
#include "string/rc_string.h"

#include "i3ResourceFile.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

void CopyFromBufferToStringList(i3::vector<i3::rc_string>& out, const char *Buf, bool bClear)
{
	if( bClear)
		out.clear();

	const char *pStart = Buf;

	while( *Buf) 
	{
		switch( *Buf) 
		{
		case '\r':
			out.push_back(i3::rc_string(pStart, size_t(Buf - pStart)));

			pStart = Buf + 1;

			while(*pStart == '\n')
				pStart ++;

			Buf = pStart;
			break;

		default :
			Buf++;
			break;

		}// end switch()

	}// end while()

	if( pStart < Buf) 
	{
		out.push_back(i3::rc_string(pStart, size_t(Buf - pStart)));
	}
}


void CopyFromStringListToBuffer( const i3::vector<i3::rc_string>& in, char *Buf, INT32 Limit, INT32 * pOffsetTable, bool isCRLF)
{
	
	INT32 Accm = 0;
	char * pStart = Buf;
	
	const size_t num_list = in.size();

	for(size_t i = 0; i < num_list; ++i) 
	{
		INT32 Length = INT32(in[i].length());
		
		if((Accm + Length) >= Limit) 
		{
			Length -= (Limit - Accm + 1);

			if( Length > 0)
			{
				i3::string_ncopy_nullpad( Buf, in[i].c_str(), Length);

				if( pOffsetTable != nullptr)
					pOffsetTable[i] = Buf - pStart;
			}

			Accm = Limit - 1;
			Buf += Length;
			break;
		}
		else 
		{
			i3::copy(in[i].begin(), in[i].end(), Buf );

			if( pOffsetTable != nullptr)
				pOffsetTable[i] = Buf - pStart;

			if( i < num_list - 1) 
			{
				Buf[Length] = '\r';
				if( isCRLF ) 
				{
					Length ++;
					Buf[Length] = '\n';
				}
			}
		}// end if()

		Accm += Length + 1;
		Buf += Length + 1;

	}// end for()
	if(num_list > 0)
		*(Buf-1) = 0;
}

INT32 GetStringListBufferSize( const i3::vector<i3::rc_string>& StrList )
{
	INT32 Rv = 0;
	const size_t num = StrList.size();

	for(size_t i = 0; i < num; ++i) 
	{
		Rv += (INT32)StrList[i].length() + 2;
	}

	return Rv;
}


UINT32 SaveFromStringListToResourceFile(const i3::vector<i3::rc_string>& in, i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	INT32	size = GetStringListBufferSize(in);

	Result = pStream->Write( &size, sizeof( INT32));
	I3_CHKIO( Result);

	if( size > 0)
	{
		char * pszBuffer = (char *) i3MemAlloc( size);
		CopyFromStringListToBuffer(in, pszBuffer, size);

		Rc = pStream->Write( pszBuffer, size);
		I3_CHKIO( Rc);
		Result += Rc;

		I3MEM_SAFE_FREE( pszBuffer);
	}

	return Result;
}

UINT32 LoadFromResourceFileToStringList(i3::vector<i3::rc_string>& out, i3ResourceFile * pResFile)
{
	INT32		size;
	UINT32		Rc, Result	= 0;
	i3Stream *	pStream		= pResFile->GetStream();

	out.clear();

	Result = pStream->Read( &size, sizeof( INT32));
	I3_CHKIO( Result);

	if( size > 0)
	{
		char * pszBuffer = (char *) i3MemAlloc( size);
		Rc = pStream->Read( pszBuffer, size);
		I3_CHKIO( Rc);

		Result += Rc;

		CopyFromBufferToStringList(out, pszBuffer);

		I3MEM_SAFE_FREE( pszBuffer);
	}

	return Result;
}