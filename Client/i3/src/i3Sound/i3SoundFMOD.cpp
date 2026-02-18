#include "stdafx.h"

#if defined( I3SND_FMOD)

#include "i3SoundFMOD.h"
#include "i3SoundGlobal.h"
#include "i3SoundUtil.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/algorithm/to_upper.h"

I3_CLASS_INSTANCE( i3SoundFMOD);

i3SoundFMOD::i3SoundFMOD(void)
{
	//Runtime Format
	m_nFormat			= I3SND_CMN_FORMAT_WAV;
}

i3SoundFMOD::~i3SoundFMOD(void)
{
	if( m_p3DSound != nullptr)
	{
		m_p3DSound->release();
		m_p3DSound = nullptr;
	}

	if( m_p2DSound != nullptr)
	{
		m_p2DSound->release();
		m_p2DSound = nullptr;
	}
}

REAL32 i3SoundFMOD::GetDuration()
{
	return (REAL32) m_nDataLength / m_nAvgBytesPerSec;
}

bool	i3SoundFMOD::Create( char * pszPath )
{
	void * pBuf;

	i3FileStream strm;

	// 생성 단계에선 2D인지 3D인지 모르기 때문에
	// 우선 메모리에 파일을 읽어만 둔다.
	if( strm.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not open %s", pszPath);
		return false;
	}

	m_nDataSize = strm.GetSize();

	I3ASSERT( m_nDataSize > 0);

	pBuf = i3MemAlign( m_nDataSize, 16);
	I3ASSERT( pBuf != nullptr);

	UINT32 rc = strm.Read( pBuf, m_nDataSize);
	if( rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not load %s file.", pszPath);
		I3MEM_SAFE_FREE( pBuf);
		strm.Close();
		return false;
	}

	SetData( (char *) pBuf);

	strm.Close();

	char szExt[16];

	i3::extract_fileext(pszPath, szExt);
	i3::to_upper( szExt);
	if( i3::generic_is_iequal( szExt, "WAV") )
	{
		//wave의 format에 관한 데이터를 읽어들이고 저장해놓는다.
		{
			WAVEFORMATEX wfx;
			MMIOINFO mmioInfo;
			i3mem::FillZero( &mmioInfo, sizeof(mmioInfo) );
			mmioInfo.fccIOProc = FOURCC_MEM;
			mmioInfo.cchBuffer = m_nDataSize;
			mmioInfo.pchBuffer = (CHAR*) pBuf;

			HMMIO hmmio = mmioOpen( nullptr, &mmioInfo, MMIO_ALLOCBUF | MMIO_READ );

			rc = I3SNDUtil_LoadWaveFileFormat( nullptr, &wfx, hmmio);
			if( rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN,  "%s 파일정보를 읽어올수 없습니다.", pszPath);
				return false;
			}

			mmioClose( hmmio, 0);

			{
				m_nPersistFormat	= wfx.wFormatTag;

				m_nChannels			= wfx.nChannels;
				m_nSampleRate		= wfx.nSamplesPerSec;
				m_nBitsPerSample	= wfx.wBitsPerSample;
				m_nDataLength		= rc;

				m_nBlockAlign		= wfx.nBlockAlign;
				m_nAvgBytesPerSec	= wfx.nAvgBytesPerSec;;
			}
		}
	}

	return true;
}

FMOD::Sound * i3SoundFMOD::Get3DSound( bool bEnableCreate, bool bHeadRelative)
{
	if( m_p3DSound == nullptr)
	{
		I3ASSERT( getData() != nullptr);

		FMOD_CREATESOUNDEXINFO info;
		//FMOD_MODE mode = (FMOD_MODE)( FMOD_DEFAULT | FMOD_3D | FMOD_OPENMEMORY);
		//FMOD_MODE mode = (FMOD_MODE)( FMOD_DEFAULT | FMOD_3D | FMOD_OPENMEMORY | FMOD_3D_LINEARROLLOFF);
		FMOD_MODE mode = (FMOD_MODE)( FMOD_DEFAULT | FMOD_3D | FMOD_OPENMEMORY | FMOD_3D_LOGROLLOFF | FMOD_LOOP_NORMAL);

		mode = mode | FMOD_SOFTWARE;

		i3mem::FillZero( &info, sizeof(info));
		info.cbsize = sizeof(info);
		info.length = m_nDataSize;

		if( bHeadRelative)
			mode = (FMOD_MODE)((UINT32) mode | FMOD_3D_HEADRELATIVE);

		/*
		if( m_nPersistFormat != WAVE_FORMAT_PCM)
		{
			//mode = (FMOD_MODE)((UINT32)mode & ~FMOD_HARDWARE);
			//mode = (FMOD_MODE)((UINT32)mode & ~FMOD_OPENMEMORY_POINT);
			mode = (FMOD_MODE)((UINT32)mode | FMOD_CREATECOMPRESSEDSAMPLE | FMOD_OPENMEMORY_POINT);
		}*/

		// 아직 3D Sound가 재생된 적이 없기 때문에 생성한다.
		FMOD_RESULT rv = g_pFMODSystem->createSound( getData(), mode, &info, &m_p3DSound);
		ERRCHECK( rv);
	}

	return m_p3DSound;
}

FMOD::Sound * i3SoundFMOD::Get2DSound( bool bEnableCreate)
{
	if( m_p2DSound == nullptr)
	{
		I3ASSERT( getData() != nullptr);

		FMOD_CREATESOUNDEXINFO info;
		FMOD_MODE mode = (FMOD_MODE)( FMOD_DEFAULT | FMOD_2D | FMOD_OPENMEMORY | FMOD_LOOP_NORMAL);

		mode = mode | FMOD_SOFTWARE;

		i3mem::FillZero( &info, sizeof(info));
		info.cbsize = sizeof(info);
		info.length = m_nDataSize;

		/*
		if( m_nPersistFormat != WAVE_FORMAT_PCM)
		{
			//mode = (FMOD_MODE)((UINT32)mode & ~FMOD_HARDWARE);
			//mode = (FMOD_MODE)((UINT32)mode & ~FMOD_OPENMEMORY_POINT);
			mode = (FMOD_MODE)((UINT32)mode | FMOD_CREATECOMPRESSEDSAMPLE | FMOD_OPENMEMORY);
		}*/

		// 아직 3D Sound가 재생된 적이 없기 때문에 생성한다.
		FMOD_RESULT rv = g_pFMODSystem->createSound( getData(), mode, &info, &m_p2DSound);
		ERRCHECK( rv);
	}

	return m_p2DSound;
}

const char * i3SoundFMOD::getErrorMsg( FMOD_RESULT code)
{
	static const char s_szMsg[][64] =
	{
		"FMOD_OK",
		"FMOD_ERR_ALREADYLOCKED",
		"FMOD_ERR_BADCOMMAND",
		"FMOD_ERR_CDDA_DRIVERS",
		"FMOD_ERR_CDDA_INIT",
		"FMOD_ERR_CDDA_INVALID_DEVICE",
		"FMOD_ERR_CDDA_NOAUDIO",
		"FMOD_ERR_CDDA_NODEVICES",
		"FMOD_ERR_CDDA_NODISC",
		"FMOD_ERR_CDDA_READ",
		"FMOD_ERR_CHANNEL_ALLOC",
		"FMOD_ERR_CHANNEL_STOLEN",
		"FMOD_ERR_COM",
		"FMOD_ERR_DMA",
		"FMOD_ERR_DSP_CONNECTION",
		"FMOD_ERR_DSP_FORMAT",
		"FMOD_ERR_DSP_NOTFOUND",
		"FMOD_ERR_DSP_RUNNING",
		"FMOD_ERR_DSP_TOOMANYCONNECTIONS",
		"FMOD_ERR_FILE_BAD",
		"FMOD_ERR_FILE_COULDNOTSEEK",
		"FMOD_ERR_FILE_DISKEJECTED",
		"FMOD_ERR_FILE_EOF",
		"FMOD_ERR_FILE_NOTFOUND",
		"FMOD_ERR_FILE_UNWANTED",
		"FMOD_ERR_FORMAT",
		"FMOD_ERR_HTTP",
		"FMOD_ERR_HTTP_ACCESS",
		"FMOD_ERR_HTTP_PROXY_AUTH",
		"FMOD_ERR_HTTP_SERVER_ERROR",
		"FMOD_ERR_HTTP_TIMEOUT",
		"FMOD_ERR_INITIALIZATION",
		"FMOD_ERR_INITIALIZED",
		"FMOD_ERR_INTERNAL",
		"FMOD_ERR_INVALID_ADDRESS",
		"FMOD_ERR_INVALID_FLOAT",
		"FMOD_ERR_INVALID_HANDLE",
		"FMOD_ERR_INVALID_PARAM",
		"FMOD_ERR_INVALID_SPEAKER",
		"FMOD_ERR_INVALID_VECTOR",
		"FMOD_ERR_IRX",
		"FMOD_ERR_MAXAUDIBLE",
		"FMOD_ERR_MEMORY",
		"FMOD_ERR_MEMORY_IOP",
		"FMOD_ERR_MEMORY_SRAM",
		"FMOD_ERR_MEMORY_CANTPOINT",
		"FMOD_ERR_NEEDS2D",
		"FMOD_ERR_NEEDS3D",
		"FMOD_ERR_NEEDSHARDWARE",
		"FMOD_ERR_NEEDSSOFTWARE",
		"FMOD_ERR_NET_CONNECT",
		"FMOD_ERR_NET_SOCKET_ERROR",
		"FMOD_ERR_NET_URL",
		"FMOD_ERR_NET_WOULD_BLOCK",
		"FMOD_ERR_NOTREADY",
		"FMOD_ERR_OUTPUT_ALLOCATED",
		"FMOD_ERR_OUTPUT_CREATEBUFFER",
		"FMOD_ERR_OUTPUT_DRIVERCALL",
		"FMOD_ERR_OUTPUT_FORMAT",
		"FMOD_ERR_OUTPUT_INIT",
		"FMOD_ERR_OUTPUT_NOHARDWARE",
		"FMOD_ERR_OUTPUT_NOSOFTWARE",
		"FMOD_ERR_PAN",
		"FMOD_ERR_PLUGIN",
		"FMOD_ERR_PLUGIN_MISSING",
		"FMOD_ERR_PLUGIN_RESOURCE",
		"FMOD_ERR_PLUGIN_INSTANCES",
		"FMOD_ERR_RECORD",
		"FMOD_ERR_REVERB_INSTANCE",
		"FMOD_ERR_SUBSOUNDS",
		"FMOD_ERR_SUBSOUND_ALLOCATED",
		"FMOD_ERR_TAGNOTFOUND",
		"FMOD_ERR_TOOMANYCHANNELS",
		"FMOD_ERR_UNIMPLEMENTED",
		"FMOD_ERR_UNINITIALIZED",
		"FMOD_ERR_UNSUPPORTED",
		"FMOD_ERR_UPDATE",
		"FMOD_ERR_VERSION",
		"FMOD_ERR_EVENT_FAILED",
		"FMOD_ERR_EVENT_INTERNAL",
		"FMOD_ERR_EVENT_INFOONLY",
		"FMOD_ERR_EVENT_MAXSTREAMS",
		"FMOD_ERR_EVENT_MISMATCH",
		"FMOD_ERR_EVENT_NAMECONFLICT",
		"FMOD_ERR_EVENT_NOTFOUND",
	};

	return s_szMsg[ code];
}

#endif