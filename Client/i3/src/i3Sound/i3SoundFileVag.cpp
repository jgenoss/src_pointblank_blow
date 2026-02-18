
#include "stdafx.h"
#include "i3SoundFile.h"
#include "i3SoundFileVag.h"
#include "i3SoundUtil.h"
#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

UINT32	  i3SoundFile::LoadVag( i3Stream * pStream, i3Sound * pSound )
{
	I3ASSERT( pStream != nullptr );
	I3ASSERT( pSound != nullptr );

	UINT32	Result = 0, Rc = 0;
	i3::pack::SOUND_VAG_HEADER	vagHeader;

	Result = pStream->Read( &vagHeader, sizeof( i3::pack::SOUND_VAG_HEADER ));
	I3_CHKIO( Result);

	if ( i3::generic_is_niequal(vagHeader.cFormat, "VAGp", 4) == false)
	{
		I3TRACE("i3SoundFile::LoadVag() : Not a Vag File\n");
		return false;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	INT32	nSampleRate			= nBNTOCC( vagHeader.nSampleperSec );
	INT32	nSampleLength		= nBNTOCC(  vagHeader.nSampleSize );
	INT32	nRuntimeDataLength	= I3SNDUtil_CalculateBufferSize( I3SND_CMN_FORMAT_VAG, pSound->GetFormat(), nSampleLength );
	if( !pSound->Create( I3SND_CMN_FORMAT_VAG, 1, nSampleRate, 16, nRuntimeDataLength))
	{
		I3TRACE("i3SoundFile::LoadVag()>>pSound->Create() Failed\n");
		return false;
	}

	char * pData = (char*)i3MemAlloc( nSampleLength);
	i3mem::FillZero( pData, nSampleLength);
	{
		Rc = pStream->Read( pData, nSampleLength);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	if( pSound->GetFormat() == I3SND_CMN_FORMAT_VAG )
	{
		pSound->SetData( pData);
	}
	else
	{
		char * pRuntimeData = (char *)i3MemAlloc( nRuntimeDataLength);
		i3mem::FillZero( pRuntimeData, nRuntimeDataLength);

		pSound->MakeRuntimeForm( pRuntimeData, pData, nSampleLength);

		pSound->SetData( pRuntimeData);

		I3MEM_SAFE_FREE_POINTER( pData);
	}

	return true;
}

UINT32	  i3SoundFile::SaveVag( i3Stream * pStream, i3Sound * pSound )
{
	UINT32 Result, Rc;
	//기본적으로 vagHeader에 들어가는 값들은 BigEndian으로 들어갑니다.
	//vagHeader에 쓰인값을 그대로 인자로 사용하지 않도록 주의해야합니다.
	i3::pack::SOUND_VAG_HEADER vagHeader;

	INT32			nSize;

	vagHeader.nVersion		= nBNTOCC( 0x20 );
	vagHeader.nReserved		= 0x00;
	vagHeader.nSampleperSec = nBNTOCC( pSound->GetSampleRate());				//SamplingFrequency 4

	nSize = I3SNDUtil_CalculateBufferSize( pSound->GetFormat(),I3SND_CMN_FORMAT_VAG, pSound->GetDataLength());
	vagHeader.nSampleSize	= nBNTOCC( nSize );

	char szFileName[MAX_PATH];
	i3::safe_string_copy( szFileName, ((i3FileStream*)pStream)->GetFileName(), MAX_PATH );
	i3::remove_ext( szFileName );							//name 16
	i3::safe_string_copy( vagHeader.cName, szFileName, 16 );
	
	Result = pStream->Write( &vagHeader, sizeof( vagHeader ));
	I3_CHKIO( Result);

	char * pSrcBuf = nullptr;

	//save Data
	if( pSound->GetFormat() == I3SND_CMN_FORMAT_VAG )
	{
		pSrcBuf = (char*)pSound->getData();
		
		Rc = pStream->Write( pSrcBuf, nSize );
		I3_CHKIO( Rc);
		Result += Rc;
	}
	else
	{
		if( pSound->GetPersistFormat() == I3SND_CMN_FORMAT_VAG )
		{
			pSrcBuf = (char*)pSound->getData();

			Rc = pStream->Write( pSrcBuf, nSize);
			I3_CHKIO( Rc);

			Result += Rc;
		}
		else
		{
			pSrcBuf			= (char*)pSound->getData();
			char *  pBuf	= (char*)i3MemAlloc( nSize );

			I3SNDUtil_ConvertSound( pSound->GetFormat(), pSound->GetDataLength(), pSrcBuf,
				I3SND_CMN_FORMAT_VAG, nSize, pBuf );

			Rc = pStream->Write( pBuf, nSize);
			I3_CHKIO( Rc);

			Result += Rc;

			i3MemFree( pBuf );
		}
	}

	return Result;
}
