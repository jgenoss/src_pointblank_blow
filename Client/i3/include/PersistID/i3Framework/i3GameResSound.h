#if !defined( __I3_GAME_RES_SOUND_H)
#define __I3_GAME_RES_SOUND_H

#include "i3GameRes.h"

class I3_EXPORT_FRAMEWORK i3GameResSound : public i3GameRes
{
	I3_CLASS_DEFINE( i3GameResSound);

protected:
	i3SoundPlayInfo *	m_pSound;

public:
	i3GameResSound(void);
	virtual ~i3GameResSound(void);

	i3SoundPlayInfo *	getSound(void)		{ return m_pSound; }
	void				setSound( i3SoundPlayInfo * pSnd)
	{
		I3_REF_CHANGE( m_pSound, pSnd);
	}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual UINT32	SaveToFile( i3Stream * pStream);
	virtual UINT32	LoadFromFile( i3Stream * pStream, INT32 nVersion);
};

#endif
