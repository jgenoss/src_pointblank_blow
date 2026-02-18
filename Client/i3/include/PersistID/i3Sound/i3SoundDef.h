//-----------------------------------------------------------------------------
// File: i3SoundDef.h
//
// Desc: Sound Engine Define for State and Include File 
//		
// Date: 2005/11/09		by: raja
//-----------------------------------------------------------------------------
#ifndef I3SOUNDDEF_H
#define I3SOUNDDEF_H

#include "i3SoundConfig.h"

#if defined( I3SND_DSOUND )
	#include <mmreg.h>
	#include <dsound.h>
#endif

#if (defined( I3_WINDOWS) || defined( I3_WINCE)) && defined( I3_DLL)
	#if defined( I3_SOUND_BUILD)
		#define I3_EXPORT_SOUND		__declspec( dllexport)
		#define I3_EXTERN_SOUND		extern
	#else
		#define I3_EXPORT_SOUND		__declspec( dllimport)
		#define I3_EXTERN_SOUND		__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_SOUND)
		#define	I3_EXPORT_SOUND
		#define I3_EXTERN_SOUND		extern
	#endif
#endif

//SOUND CONTEXT를 위한 속성
#define 	I3SND_CMN_MASK_ALL				0xFFFFFFFF		//MASK_ALL
#define		I3SND_CMN_MASK_FORMAT			0x0000000F		//MASK_FORMAT
#define		I3SND_CMN_MASK_OUTCHANNEL		0x000000F0		//MASK_CHANNEL

#define		I3SND_CMN_FORMAT_WAV			0x00000001		//Standard PCM/Uncompressed
#define		I3SND_CMN_FORMAT_VAG			0x00000002		//Sony adpcm(vag) 
#define		I3SND_CMN_FORMAT_I3SND			0x00000003		//I3SND
#define		I3SND_CMN_FORMAT_MP3			0x00000004		//MP3
#define		I3SND_CMN_FORMAT_OGG			0x00000005		//Ogg
#define		I3SND_CMN_FORMAT_UNKNOWN		0x00000006

//Speaker Type
#define		I3SND_CMN_OUTCHANNEL_MONO		0x00000010		//Mono Speaker
#define		I3SND_CMN_OUTCHANNEL_2DP		0x00000020		//Stereo Speaker
#define		I3SND_CMN_OUTCHANNEL_3DP		0x00000030		//
#define		I3SND_CMN_OUTCHANNEL_51			0x00000040

//Hardware Acceleration Enable
#define		I3SND_CMN_USEACCELERATION		0x10000000

//Sound Effect
#define		I3SND_CMN_FX_CHORUS				0x00010000
#define		I3SND_CMN_FX_COMPRESSOR			0x00020000
#define		I3SND_CMN_FX_DISTORTION			0x00040000
#define		I3SND_CMN_FX_ECHO				0x00080000
#define		I3SND_CMN_FX_FLANGER			0x00100000
#define		I3SND_CMN_FX_GARGLE				0x00200000
#define		I3SND_CMN_FX_PARAMEQ			0x00400000
#define		I3SND_CMN_FX_REVERB				0x00800000
#define		I3SND_CMN_FX_COUNT				0x00000008


#define		I3SND_CMN_ENDIAN_LITTLE			0x00000000
#define		I3SND_CMN_ENDIAN_BIG			0x00000001
#define		I3SND_ENDIAN					I3SND_CMN_ENDIAN_LITTLE

#define		I3SND_CMN_SIGNTYPE_UNSIGNED		0x00000000
#define		I3SND_CMN_SIGNTYPE_SIGNED		0x00000001
#define		I3SND_SIGN_TYPE					I3SND_CMN_SIGNTYPE_SIGNED

//For mask
#define		I3SND_RT_MASK_3DMODE			0x0F000000		//MASK_3DMODE
#define		I3SND_RT_MASK_STREAM			0x00F00000		//MASK_STREAM
#define		I3SND_PROP_MASK_TYPE			0x000F0000		//MASK_SOUNDTYPE
#define		I3SND_PROP_MASK_PRIORITY		0x0000F000		//MASK_PRIORITYTYPE
#define		I3SND_RT_MASK_LOOPMODE			0x000000F0		//MASK_LOOPMODE
#define		I3SND_RT_MASK_PLAYSTATE			0x0000000F		//MASK_PLAYSTATE


	//음원의 종류
#define		I3SND_PROP_TYPE_BGM				0x00010000		//BGM은 스트리밍을 합니다.
#define		I3SND_PROP_TYPE_VOICE			0x00020000		
#define		I3SND_PROP_TYPE_EFFECT			0x00040000
#define		I3SND_PROP_TYPE_COUNT			0x00000003

	//동일 음원의 우선순위 
#define		I3SND_PROP_PRIORITY_DISTANCE	0x00000000
#define		I3SND_PROP_PRIORITY_PLAYTIME	0x00001000		//같은 음원중에서의 우선순위 판별

	//BUFFER MODE
#define		I3SND_RT_BUFFMODE_BUFFER		0x00000000		//streamming, buffermode판별
#define		I3SND_RT_BUFFMODE_STREAM		0x00100000

	//LOOP MODE
#define		I3SND_RT_LOOPMODE_ONESHOT		0x00000000		//loop mode판별
#define		I3SND_RT_LOOPMODE_LOOPING		0x00000010		//loop mode판별

	//3DMODE
#define		I3SND_RT_3DMODE_DISABLE			0x01000000		//POSITIONAL SOUND 안함
#define		I3SND_RT_3DMODE_CAMERA_SPACE	0x02000000		//카메라 좌표계 사용
#define		I3SND_RT_3DMODE_WORLD_SPACE		0x03000000		//월드 좌표계 사용

	//USED FLAG
#define		I3SND_RT_PLAYSTATE_USED			0x10000000		//현재 스테이트를 참조하여 쓰고잇는놈이 있음

//Play State
#if defined( I3SND_DSOUND)
#define		I3SND_RT_REQ_NONE				0x00000000
#define		I3SND_RT_REQ_STOP				0x00000001
#define		I3SND_RT_REQ_PLAY				0x00000002
#define		I3SND_RT_REQ_PAUSE				0x00000004
#define		I3SND_RT_REQ_RESUME				0x00000008
#define		I3SND_RT_REQ_PITCH				0x00000010
#define		I3SND_RT_REQ_VOLUME				0x00000020
#define		I3SND_RT_REQ_POSITION			0x00000040
#define		I3SND_RT_REQ_REPLAY				0x00000080
#endif

#define		I3SND_RT_PLAYSTATE_FREE			0x00000000
#define		I3SND_RT_PLAYSTATE_READY		0x00000001
#define		I3SND_RT_PLAYSTATE_PLAY			0x00000002		//플레이중
#define		I3SND_RT_PLAYSTATE_PAUSE		0x00000004		//일시 정지중
#define		I3SND_RT_PLAYSTATE_MUTE			0x00000008

#define		I3SND_3D_POSITIONAL		//POSITION SOUND를 할것이냐 말것이냐

enum	I3SND_ARGTYPE
{
	I3SND_ARGTYPE_CONST = 0,
	I3SND_ARGTYPE_LINEAR,
	I3SND_ARGTYPE_EXP,
	I3SND_ARGTYPE_EXP2,
};

#if defined( I3SND_DSOUND)
	#define I3SND_STANDARD_PITCH DSBFREQUENCY_ORIGINAL

	#define	I3SND_WAV_CHANNELS					2
	#define I3SND_WAV_SAMPLERATE				22050
	#define I3SND_WAV_BITSPERSAMPLE				16
	#define I3SND_WAV_BLOCKALIGN				4

	#define I3SND_DEFAULT_STREAMBUF_SIZE		22050

#elif defined( I3G_PSP)
	#define I3SND_STANDARD_PITCH 0x1000
#else
	#define I3SND_STANDARD_PITCH 0
#endif

#if defined(I3G_PSP)
///////////////////////////////////////////////////////////////////////////////
// Define for Atrac/vag
///////////////////////////////////////////////////////////////////////////////
//only vag
#define		I3SND_SAMPLE_SIZE			256					// 1sample의 길이
#define		I3SND_SAMPLE_SIDE			2					// 

//only atrac
//#define USE_HOST0
//////////////////////////////////////////////////////////////////////////////
#define I3SND_USE_ATRAC3_PLUS				//at3plus를 사용함

//////////////////////////////////////////////////////////////////////////////
#ifdef I3SND_USE_ATRAC3_PLUS
	#define I3SND_MAX_SAMPLE			(2048)	//1 sample의 max size.
#else
	#define I3SND_MAX_SAMPLE            (1024)	//1 sample의 max size.
#endif //USE_ATRAC3_PLUS

//////////////////////////////////////////////////////////////////////////////
#define I3SND_INPUTBUF_SIZE				(128 * 1024)
#define I3SND_WRITEBUF_SIZE				(3   * I3SND_MAX_SAMPLE)

//////////////////////////////////////////////////////////////////////////////
#ifdef USE_HOST0
	#define I3SND_READABLE_SIZE		I3SND_MAX_SAMPLE
#else
	#define I3SND_READABLE_SIZE		I3SND_INPUTBUF_SIZE
#endif// USE_HOST0


//////////////////////////////////////////////////////////////////////////////
#define I3SND_OUTPUT_SAMPLE			I3SND_MAX_SAMPLE
#define I3SND_WAV_VOLUME_MAX		SCE_WAVE_AUDIO_VOLUME_MAX
//////////////////////////////////////////////////////////////////////////////
enum I3SND_WAVOUT_CHANNEL
{
	I3SND_WAVOUT_CHANNEL_AUDIO	= 0,
	I3SND_WAVOUT_CHANNEL_BGM	= 1,
	I3SND_WAVOUT_CHANNEL_EFFECT = 2,
	I3SND_WAVOUT_CHANNEL_MAX,
};
#endif // I3G_PSP


enum I3SND_FX_REVERB_PRESET
{
	I3SND_FX_REVERB_OFF	= 0,
	I3SND_FX_REVERB_GENERIC,
	I3SND_FX_REVERB_PADDEDCELL,
	I3SND_FX_REVERB_ROOM,
	I3SND_FX_REVERB_BATHROOM,
	I3SND_FX_REVERB_LIVINGROOM,
	I3SND_FX_REVERB_STONEROOM,
	I3SND_FX_REVERB_AUDITORIUM,
	I3SND_FX_REVERB_CONCERTHALL,
	I3SND_FX_REVERB_CAVE,
	I3SND_FX_REVERB_ARENA,
	I3SND_FX_REVERB_CARPETEDHALLWAY,
	I3SND_FX_REVERB_HALLWAY,
	I3SND_FX_REVERB_FORESET,
	I3SND_FX_REVERB_CITY,
	I3SND_FX_REVERB_MOUNTAINS,
	I3SND_FX_REVERB_UNDERWATER,

	I3SND_FX_REVERB_COUNT,
};

typedef struct tag_i3SoundInfoHeader
{
	UINT8				m_nID[4];
	UINT32				m_nFormat;
	UINT32				m_nChannels;
	UINT32				m_nSampleRate;
	UINT32				m_nBitsPerSample;
	UINT32				m_nAvgBytesPerSec;
	UINT32				m_nBlockAlign;
	UINT32				m_nDataSize;
}I3SND_INFOHEADER;

#if defined( I3SND_DSOUND)
#define ERRCHECK(a)
#elif defined( I3SND_FMOD)
#define ERRCHECK(a)		if( (a) != FMOD_OK) { I3TRACE( "[FMOD] %s\n", i3SoundFMOD::getErrorMsg( (a))); }
#endif

#if defined( I3SND_FMOD)
typedef struct ALIGN1 _tagi3FMODTriangles
{
	INT32			_nVertexCount;
	REAL32			_rDirectOcclusion;		//Reverb가 적용되지 않은 Occlusion의 볼륨
	REAL32			_rReverbOcclusion;		//Reverb가 적용된 Occlusion의 볼륨
	UINT8			_bDoubleSided;			//트라이앵글이 양방향 모두 적용되는지의 여부
	UINT8			_pad[3];

	PERSIST_VEC3D * _pvVertexArray;
}I3SND_FMOD_POLYGON;
#endif

#endif // I3SOUNDDEF_H

