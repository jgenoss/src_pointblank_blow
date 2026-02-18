#include "stdafx.h"
#include "stdio.h"
#include "i3SoundStreamDx.h"

#if defined( I3SND_DSOUND)

#include "i3SoundStreamDx.h"

I3_CLASS_INSTANCE( i3SoundStreamDx);

#if !defined( USE_I3_FILESTREAM)
	size_t ovCB_read(void *buf,unsigned int a,unsigned int b,void * fp) 
	{
		return fread(buf,a,b,(FILE *)fp);
	}

	int ovCB_close(void * fp) 
	{
		return fclose((FILE *)fp);
	}

	long ovCB_tell(void *fp) 
	{
		return ftell((FILE *)fp);
	}

	int ovCB_seek(void *fp,__int64 a,int b) 
	{
		return fseek((FILE *)fp,(long)a,b);
	}
#else
	size_t ovCB_read( void * buf, unsigned int a, unsigned int b, void * fp)
	{
		i3FileStream * pStream = (i3FileStream*)fp;

		UINT32 Rc = pStream->Read( buf, a * b);
		if( Rc == STREAM_ERR)
		{
			if( i3FileStream::STREAM_EOS == pStream->GetLastError())
			{
				return 0;
			}
			else
			{
				I3_CHKIO( Rc);
			}
		}

		return Rc;
	}

	int ovCB_close( void * fp)
	{
		i3FileStream * pStream = (i3FileStream *)fp;

		bool bClose = pStream->Close();
		if( bClose)
			return 0;
		else
			return 1;
	}


	int ovCB_seek( void * fp, __int64 a, int b)
	{
		i3FileStream * pStream = (i3FileStream *)fp;

		UINT32 n = STREAM_BEGIN;
		if( b == SEEK_SET)
		{
			n = STREAM_BEGIN;
		}
		else if( b == SEEK_CUR)
		{
			n = STREAM_CURRENT;
		}
		else
		{
			n = STREAM_END;
		}

		UINT32 Rc = pStream->SetPosition( (INT32)a, n);
		if( Rc == STREAM_ERR)
		{
			return Rc;
		}

		return 0;
	}

	long ovCB_tell( void * fp)
	{
		i3FileStream * pStream = ( i3FileStream*)fp;

		return pStream->GetPosition();
	}

#endif

i3SoundStreamDx::i3SoundStreamDx()
{
	m_pFile				= nullptr;

	#if defined( USE_I3_FILESTREAM)
	m_pFileHandle		= nullptr;
	#endif

	m_nBufferSize		= 0;	//버퍼 사이즈
	m_nGapBetweenCursor = 0;	//스트림 읽기 커서와 플레이커서간의 최소거리
	m_nReadOnce			= 0;	//한번에 읽는 최대 샘플수
	
	m_nPlaySize			= 0;	//현재까지 플레이한 샘플수
	m_nReadSize			= 0;	//현재까지 읽어들인 샘플수
	m_nTempPlaySize		= 0;	//스트림이 한바퀴 돌때 이전 루프에 플레이되지 않은 샘플수

	m_nPlayPos			= 0;	//현재 플레이 커서
	m_nOldPlayPos		= 0;	//이전 플레이 커서
	m_nWritePos			= 0;	//현재 읽기 커서

	i3mem::FillZero( &m_pos, sizeof( position_set));
	m_next_pos			= nullptr;
	m_CurSection		= 0;

	m_lpDSBuffer		= nullptr;
}

i3SoundStreamDx::~i3SoundStreamDx()
{
	#if defined( USE_I3_FILESTREAM)
	I3_SAFE_RELEASE( m_pFileHandle);
	#endif
}

bool i3SoundStreamDx::Create( const char * pszFileName, WCHAR32 nBufferSize, WCHAR32 nGap, WCHAR32 nReadOnce)
{
	if( g_pDS == nullptr)
		return false;

	if( pszFileName == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "StreamFileName is nullptr");
		return false;
	}

	m_nBufferSize		= nBufferSize;	//버퍼 사이즈
	m_nGapBetweenCursor	= nGap;			//스트림 읽기 커서와 플레이커서간의 최소거리
	m_nReadOnce			= nReadOnce;	//한번에 읽는 최대 샘플수

	m_nOldPlayPos = m_nWritePos = m_nPlayPos = m_nTempPlaySize = 0;
	m_pos.index = m_pos.start_pos = m_pos.loop_start_pos = m_pos.option = 0;
	m_pos.next	= nullptr;

	#if defined( USE_I3_FILESTREAM)
		I3_SAFE_RELEASE( m_pFileHandle);

		m_pFileHandle = i3FileStream::new_object();

		if( !m_pFileHandle->Open( pszFileName, STREAM_READ | STREAM_SHAREREAD))
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could't Open File %s", pszFileName);
			return false;
		}
	#else
		m_pFile = fopen( pszFileName, "rb+");
		if( m_pFile < 0)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could't Open File %s", pszFileName);
			return false;
		}
	#endif

	i3SoundStream::Create( pszFileName, nBufferSize, nGap, nReadOnce);

	return true;
}

void i3SoundStreamDx::ResetStream()
{
	m_nPlaySize = m_nReadSize = m_nTempPlaySize = 0;
	m_nOldPlayPos = m_nWritePos = m_nPlayPos = 0;
	m_pos.index = m_pos.start_pos = m_pos.loop_start_pos = m_pos.option = 0;
	m_pos.next	= nullptr;

	MoveFromFile( 0);

	m_pos.size = (UINT32)ov_pcm_total( &m_vf, -1);
	ReadFile( m_nWritePos, min( m_nBufferSize, m_pos.size), 0, &m_nReadSize);
	m_nWritePos = (m_nWritePos + m_nReadSize) % m_nBufferSize;
}

void i3SoundStreamDx::OnCreate()
{
	char cc[8];

	#if !defined( USE_I3_FILESTREAM)
		if( fread( cc, sizeof( char) * 4, 1, m_pFile) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL, "Could't Read File");
			return;
		}
	#else
		UINT32 Rc = m_pFileHandle->Read( cc, 4);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL, "Could't Read File");
			return;
		}
	#endif
	
	UINT32				nPersistFormat	= I3SND_CMN_FORMAT_UNKNOWN;
	UINT32				nChannels		= 2;
	UINT32				nSampleRate		= 44100;
	UINT32				nBitsPerSample	= 16;
	UINT32				nDataSize		= 0;
	INT32				nRealBufSize	= 0;	//스트림에서 사용될 버퍼의 실제 바이트수( 멤버는 계산용 단위수)

	if( i3String::NCompare( cc, "OggS", 4) == 0)
	{
		nPersistFormat = I3SND_CMN_FORMAT_OGG;

		#if defined( USE_I3_FILESTREAM)
		m_pFileHandle->SetPosition( 0, STREAM_BEGIN);
		#else
		fseek( m_pFile, -4, SEEK_CUR);
		#endif

		ov_callbacks ovCB;

		ovCB.read_func = ovCB_read;
		ovCB.close_func = ovCB_close;
		ovCB.seek_func = ovCB_seek;
		ovCB.tell_func = ovCB_tell;

		#if defined( USE_I3_FILESTREAM)
		if( ov_open_callbacks( (void*)m_pFileHandle, &m_vf,nullptr,0,ovCB) < 0)
		{
			I3TRACE("Couldn't Open File: \n");
			return ;
		}
		#else
		if( ov_open_callbacks( (void*)m_pFile, &m_vf,nullptr,0,ovCB) < 0)
		{
			I3TRACE("Couldn't Open File: \n");
			return ;
		}
		#endif

		if(( m_vi = ov_info( &m_vf, -1)) == nullptr)
		{
			I3TRACE("Couldn't Read OggInfo: \n");
			return ;
		}

		nChannels		= m_vi->channels;
		nSampleRate		= m_vi->rate;
		nBitsPerSample	= 16;
		nDataSize		= (UINT32)ov_pcm_total( &m_vf, -1);
		nRealBufSize	= m_nBufferSize * (nBitsPerSample>>3) * nChannels;
	}
	else if( i3String::NCompare(cc, "RIFF", 4) == 0)
	{
		nPersistFormat = I3SND_CMN_FORMAT_WAV;

		#if !defined( USE_I3_FILESTREAM)
		if( fread( cc, sizeof( char) * 8, 1, m_pFile) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL, "Could't Read File");
			return;
		}
		#else
		if( m_pFileHandle->Read( cc, sizeof( char) * 8) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL, "Could't Read File");
			return;
		}
		#endif

		if( i3String::NCompare( cc, "WAVEfmt ", 8) != 0)
		{
			I3TRACE("Not a WAV File\n");
			return;
		}

		//Get Wave Buffer Data Info
	}
	else if( i3String::NCompare( cc, "VAGp", 4) == 0)
	{
		nPersistFormat = I3SND_CMN_FORMAT_VAG;

		//Get Vag Buffer Data Info
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL, "Not a SoundFile");
		return ;
	}

	i3SoundDx::Create( nPersistFormat,
					nChannels,
					nSampleRate,
					nBitsPerSample,
					nRealBufSize);

	//버퍼크기와 실제 사운드의 크기가 스트림에서는 다르므로 버퍼사이즈만큼 생성하고 실제사이즈를 나중에 따로 ;;;
	m_pos.next		= nullptr;
	m_pos.size		= nDataSize;

	ReadFile( m_nWritePos, min( m_nBufferSize, nDataSize), 0, &m_nReadSize);
	m_nWritePos = (m_nWritePos + m_nReadSize) % m_nBufferSize;
}

UINT32 i3SoundStreamDx::_ReadOgg( WCHAR32 pos, WCHAR32 size, WCHAR32 flag, WCHAR32 * nReadSize)
{
	HRESULT	hr;
	char	*pt1 = nullptr, *pt2 = nullptr;
	WCHAR32	size_byte = 0, pos_byte = 0, size1 = 0, size2 = 0;
	WCHAR32	count = 0;

	size_byte	= size * m_nBlockAlign;
	pos_byte	= pos * m_nBlockAlign;

	if (( hr = m_lpDSBuffer->Lock( pos_byte, size_byte, (void**)&pt1, (WCHAR32*)&size1,
					(void**)&pt2, (WCHAR32*)&size2, 0)) == DSERR_BUFFERLOST)
	{
		I3TRACE("Lock Failed: %X\n", hr);
	}
	else
	{
		WCHAR32 loop = 0, limit = 512, sz = size1;
		long loop_temp = 0;
		char *ptx = pt1;
		char temp_v[512];

		while (ptx)
		{
			loop = 0;

			while (loop < sz)
			{
				if (sz - loop < limit) limit = sz - loop;
				else limit = 512;
				loop_temp = ov_read (&m_vf, temp_v, limit, I3SND_ENDIAN, m_nBitsPerSample / 8, I3SND_SIGN_TYPE, &m_CurSection);

				if (loop_temp > 0)
				{
					i3mem::Copy(ptx+loop, temp_v, loop_temp);
					loop += loop_temp;

				}
				else if (loop_temp == 0)
					break;

			}

			count += loop;

			if (ptx == pt1) 
			{
				ptx = pt2;
				sz = size2;
			}
			else break;

		}

		count /= m_nBlockAlign;
	}

	if (nReadSize != nullptr) *nReadSize = count;
	if (FAILED(m_lpDSBuffer->Unlock((void*)pt1, size1, (void*)pt2, size2)))
	{
		I3TRACE("Unlock Failed\n");
		return STREAM_ERR;
	}

	return count;
}

I3SND_BUFINFO	*	i3SoundStreamDx::GetFreeBuffer( UINT32 nCreateFlag)
{
	I3SND_BUFINFO * pBuf = i3SoundDx::GetFreeBuffer( nCreateFlag);

	m_lpDSBuffer = pBuf->lpBuffer;

	return pBuf;
}

void i3SoundStreamDx::FillBuffer( LPDIRECTSOUNDBUFFER pBuffer)
{
	HRESULT hr;
	char * pDst;
	INT32	nLockedSize = 0;

	if( FAILED(hr = pBuffer->Lock( 0, m_nDataSize, (LPVOID*)&pDst, (LPDWORD)&nLockedSize, nullptr, nullptr, 0L )))
	{
		I3PRINTLOG(I3LOG_FATAL,  "pBuffer->Lock(): hr : %X", hr );
	}

	i3mem::FillZero( pDst, nLockedSize);

	pBuffer->Unlock( pDst, nLockedSize, nullptr, 0L );
}
	
UINT32 i3SoundStreamDx::ReadFile( WCHAR32 pos, WCHAR32 size, WCHAR32 flag, WCHAR32 * nReadSize)
{
	if( m_lpDSBuffer == nullptr)	return STREAM_ERR;

	if( GetPersistFormat() == I3SND_CMN_FORMAT_OGG)
	{
		return _ReadOgg( pos, size, flag, nReadSize);
	}
	else
	{
		return 0;
	}
}

UINT32 i3SoundStreamDx::_MoveFilePosOgg( WCHAR32 position)
{
	if( ov_seekable( &m_vf) != 0)
	{
		ov_pcm_seek( &m_vf, position);
	}
	else
	{
		return STREAM_ERR;
	}

	return 0;
}

UINT32 i3SoundStreamDx::MoveFromFile( WCHAR32 position)
{
	if( m_lpDSBuffer == nullptr)	return STREAM_ERR;

	if( GetPersistFormat() == I3SND_CMN_FORMAT_OGG)
	{
		return _MoveFilePosOgg( position);
	}
	//OTHERS
	else
	{
	
	}

	return 0;
}

UINT32 i3SoundStreamDx::_SetCurrentStreamPosOgg(position_set *p)
{
	if (p)
	{
		i3mem::Copy( &m_pos, p, sizeof( position_set));
		m_next_pos = m_pos.next;
	}
	else
	{
		I3TRACE("CANT'T SET STREAM POSITION\n");
		return STREAM_ERR;
	}

	return 0;
}

UINT32 i3SoundStreamDx::SetCurrentStreamPosition(position_set *p)
{
	if( GetPersistFormat() == I3SND_CMN_FORMAT_OGG)
	{
		return _SetCurrentStreamPosOgg( p);
	}

	return 0;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
	struct ALIGN1 STREAM_SOUND
	{
		WCHAR32 m_nBufferSize = 0;
		WCHAR32 m_nGapBetweenCursor = 0;
		WCHAR32 m_nReadOnce = 0;
		INT32	nFileNameCount = 0;
		char  * szFileName = nullptr;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3SoundStreamDx::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Result = 0, Rc = 0;
	pack::STREAM_SOUND data;
	i3Stream	* pStream = pResFile->GetStream();

	// Named Element
	Result = i3SoundStream::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	i3mem::FillZero( &data, sizeof( data));

	data.m_nBufferSize			= m_nBufferSize;	//버퍼 사이즈
	data.m_nGapBetweenCursor	= m_nGapBetweenCursor;			//스트림 읽기 커서와 플레이커서간의 최소거리
	data.m_nReadOnce			= m_nReadOnce;	//한번에 읽는 최대 샘플수
	data.nFileNameCount = i3::generic_string_size( GetNameString());
	data.szFileName		= (char*)GetName();

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.nFileNameCount > 0)
	{
		Rc = pStream->Write( (char*)GetName(), data.nFileNameCount);
		I3_CHKIO( Rc);
		Result += Rc;
	}
	
	return Result;
}

UINT32	i3SoundStreamDx::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Result = 0, Rc = 0;
	pack::STREAM_SOUND data;
	i3Stream	* pStream = pResFile->GetStream();

	// Named Element
	Result = i3SoundStream::OnLoad( pResFile);
	I3_CHKIO( Result);

	if( IsLoaded() )
	{
		I3TRACE( "WARNING : [%s] Multiple Load found!!!!!!!!!!!!!!!!!!!!\n",  __FUNCTION__  );
		return 0;
	}

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.nFileNameCount > 0)
	{
		char szFileName[MAX_PATH];
		Rc = pStream->Read( szFileName, data.nFileNameCount);
		I3_CHKIO( Rc);

		Result += Rc;
		
		Create( szFileName, data.m_nBufferSize, data.m_nGapBetweenCursor, data.m_nReadOnce);
	}

	return Result;
}

#endif
