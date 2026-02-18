#if !defined( _I3SOUND_STREAM_DX_H__)
#define _I3SOUND_STREAM_DX_H__

#if defined( I3SND_DSOUND)
	#include "i3SoundStreamDx.h"
	#include "i3SoundDx.h"

//#define USE_I3_FILESTREAM

typedef struct tag_position_set
{
	UINT32						index;				//	포지션 정보의 인덱스 넘버
	UINT32						option;				//	옵션
	UINT32						start_pos;			//	시작 부분의 샘플 넘버
	UINT32						loop_start_pos;		//	루핑할 부분의 샘플 넘버
	UINT32						size;				//	전체 샘플 수
	struct tag_position_set		*next;				//	다음 포지션 정보의 주소

} position_set;

class I3_EXPORT_SOUND i3SoundStreamDx : public i3SoundStream
{
	I3_CLASS_DEFINE( i3SoundStreamDx);

protected:
	FILE				* m_pFile;

	#if defined( USE_I3_FILESTREAM)
	i3FileStream		* m_pFileHandle;
	#endif

	LPDIRECTSOUNDBUFFER m_lpDSBuffer;

	UINT32			_ReadOgg( WCHAR32 pos, WCHAR32 size, WCHAR32 flag, WCHAR32 * nReadSize);
	UINT32			_MoveFilePosOgg( WCHAR32 position);
	UINT32			_SetCurrentStreamPosOgg(position_set *p);

public:
	WCHAR32				m_nBufferSize;			//버퍼 사이즈
	WCHAR32				m_nGapBetweenCursor;	//스트림 읽기 커서와 플레이커서간의 최소거리
	WCHAR32				m_nReadOnce;			//한번에 읽는 최대 샘플수
	
	WCHAR32				m_nPlaySize;			//현재까지 플레이한 샘플수
	WCHAR32				m_nReadSize;			//현재까지 읽어들인 샘플수
	WCHAR32				m_nTempPlaySize;		//스트림이 한바퀴 돌때 이전 루프에 플레이되지 않은 샘플수

	WCHAR32				m_nPlayPos;				//현재 플레이 커서
	WCHAR32				m_nOldPlayPos;			//이전 플레이 커서
	WCHAR32				m_nWritePos;			//현재 읽기 커서
	position_set		m_pos;
	position_set	*	m_next_pos;

	INT32				m_CurSection;

	//if file is ovfile
	OggVorbis_File		m_vf;
	vorbis_info			*m_vi;

public:
	i3SoundStreamDx(void);
	virtual ~i3SoundStreamDx(void);

	virtual BOOL	Create( const char * pszFileName, WCHAR32 nBufferSize, WCHAR32 nGap, WCHAR32 nReadOnce);
	virtual void	OnCreate();
	
	UINT32			ReadFile( WCHAR32 pos, WCHAR32 size, WCHAR32 flag, WCHAR32 * nReadSize);
	
	void			ResetStream();
	UINT32			MoveFromFile( WCHAR32 position);
	UINT32			SetCurrentStreamPosition(position_set *p);

	virtual	I3SND_BUFINFO	*	GetFreeBuffer( UINT32 nCreateFlag);
	virtual 	void	FillBuffer( LPDIRECTSOUNDBUFFER pBuffer);

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif //

#endif