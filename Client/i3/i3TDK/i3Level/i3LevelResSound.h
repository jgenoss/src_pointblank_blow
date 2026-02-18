#if !defined( __I3LV_RES_SOUND_H_ )
#define __I3LV_RES_SOUND_H_

#include "i3LevelRes.h"

class I3_EXPORT_TDK i3LevelResSound : public i3LevelRes
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResSound, i3LevelRes);

protected:
	UINT32	m_Format = I3SND_CMN_FORMAT_WAV;
	UINT32	m_nChannels = 1;
	UINT32	m_nSampleRate = 22050;
	UINT32	m_nBitsPerSample = 16;
	UINT32	m_nDataSize = 0;

	UINT32	m_nBlockAlign = 2;
	UINT32	m_nAvgBytesPerSec = 44100;
	REAL32	m_Duration = 0.0f;

	i3Sound	*	m_pSound = nullptr;

public:
	i3LevelResSound(void);
	virtual ~i3LevelResSound(void);

	UINT32	getFormat( void)			{	return m_Format;		}
	UINT32	getChannels( void)			{	return m_nChannels;		}
	UINT32	getSampleRate( void)		{	return m_nSampleRate;	}
	UINT32	getBitsPerSample( void)		{	return m_nBitsPerSample;}
	UINT32	getDataSize( void)			{	return m_nDataSize;		}

	UINT32	getBlockAlign( void)		{	return m_nBlockAlign;		}
	UINT32	getAvgBytesPerSec( void)	{	return m_nAvgBytesPerSec;	}
	REAL32	getDuration( void)			{	return m_Duration;			}

protected:
	void	_UpdateInfo( i3Sound * pSnd);

public:
	virtual bool		OnValidateBrief( bool bForce) override;
	virtual bool		OnValidate( i3LevelScene * pScene, bool bForce) override;
	virtual i3GameRes *	CreateGameRes(void) override;

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif	//