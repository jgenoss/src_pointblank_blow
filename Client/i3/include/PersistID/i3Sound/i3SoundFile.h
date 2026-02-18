#ifndef I3_SOUND_FILE_H__
#define	I3_SOUND_FILE_H__

#include "i3Sound.h"

class I3_EXPORT_SOUND i3SoundFile :	public i3ElementBase
{
	I3_CLASS_DEFINE( i3SoundFile );
public:
	i3SoundFile(void);
	~i3SoundFile(void);

	UINT32 m_ReadBytes;

public:
	UINT32	GetReadBytes(){ return m_ReadBytes; }
public:

	i3Sound * Load( const char * pszFileName);
	i3Sound * LoadWav( const char * pszFileName );
	i3Sound * LoadVag( const char * pszFileName );
	i3Sound * Loadi3Snd( const char * pszFileName, i3Sound * pSound = NULL);

	UINT32	  Load( const char * pszFileName, i3Sound * pSound);
	UINT32	  Load( i3Stream * pStream, i3Sound * pSound);

	UINT32	  Save( i3Stream * pStream, i3Sound * pSound);
	UINT32	  Save( const char * pszFileName , i3Sound * pSound );
	UINT32	  SaveWav( const char * pszFileName , i3Sound * pSound );
	UINT32	  SaveVag( const char * pszFileName , i3Sound * pSound );
	UINT32	  Savei3Snd( const char * pszFileName , i3Sound * pSound );

	UINT32	  LoadWav( i3Stream * pStream, i3Sound * pSound );
	UINT32	  SaveWav( i3Stream * pStream, i3Sound * pSound );

	UINT32	  LoadVag( i3Stream * pStream, i3Sound * pSound );
	UINT32	  SaveVag( i3Stream * pStream, i3Sound * pSound );

	UINT32	  Loadi3Snd( i3Stream * pStream, i3Sound * pSound );
	UINT32	  Savei3Snd( i3Stream * pStream, i3Sound * pSound );
};

#endif
