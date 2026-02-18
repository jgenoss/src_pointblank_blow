#if !defined( __I3_ACTION_PLAY_ANIM_H)
#define __I3_ACTION_PLAY_ANIM_H

#include "i3GameResDefine.h"

class I3_EXPORT_FRAMEWORK i3ActionPlayAnim : public i3Action
{
	I3_EXPORT_CLASS_DEFINE( i3ActionPlayAnim, i3Action);
protected:
	i3Animation *			m_pAnim = nullptr;
	I3_ANIM_PLAY_MODE		m_PlayMode = I3_ANIM_PLAY_MODE_CLAMP;

	// Volatile members

public:
	virtual ~i3ActionPlayAnim(void);

	i3Animation *		getAnim(void)					{ return m_pAnim; }
	void				setAnim( i3Animation * pAnim)	{ I3_REF_CHANGE( m_pAnim, pAnim); }

	I3_ANIM_PLAY_MODE	getPlayMode(void)				{ return m_PlayMode; }
	void				setPlayMode( I3_ANIM_PLAY_MODE mode) { m_PlayMode = mode;  }

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
