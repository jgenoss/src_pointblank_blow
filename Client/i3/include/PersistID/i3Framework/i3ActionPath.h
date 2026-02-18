#if !defined( __I3_ACTION_PATH_H)
#define __I3_ACTION_PATH_H

#include "i3GameResDefine.h"

class I3_EXPORT_FRAMEWORK i3ActionPath : public i3Action
{
	I3_CLASS_DEFINE( i3ActionPath);
protected:
	i3Animation *			m_pAnim;
	I3_ANIM_PLAY_MODE		m_PlayMode;

	// Volatile members

public:
	i3ActionPath(void);
	virtual ~i3ActionPath(void);

	i3Animation *		getAnim(void)					{ return m_pAnim; }
	void				setAnim( i3Animation * pAnim)	{ I3_REF_CHANGE( m_pAnim, pAnim); }

	I3_ANIM_PLAY_MODE	getPlayMode(void)				{ return m_PlayMode; }
	void				setPlayMode( I3_ANIM_PLAY_MODE mode)
	{
		m_PlayMode = mode; 
	}

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
