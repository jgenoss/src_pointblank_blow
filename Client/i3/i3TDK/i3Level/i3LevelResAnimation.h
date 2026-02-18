#if !defined( __I3LV_RES_ANIMATION_H_)
#define __I3LV_RES_ANIMATION_H_

#include "i3LevelRes.h"

class I3_EXPORT_TDK i3LevelResAnimation : public i3LevelRes
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResAnimation, i3LevelRes);

protected:
	// Persistant members

	// Volatile members
	INT32						m_TrackCount = 0;
	REAL32					m_Duration = 0.0f;

	//
	i3Animation	*				m_pAnimation = nullptr;

public:
	i3LevelResAnimation(void);
	virtual ~i3LevelResAnimation( void);

	INT32			getTrackCount( void)		{	return m_TrackCount;}
	REAL32		getDuration( void)			{	return m_Duration;}

protected:
	void			_UpdateInfo( i3Animation * pAnim);

public:
	virtual bool		OnValidateBrief( bool bForce) override;
	virtual bool		OnValidate( i3LevelScene * pScene, bool bForce) override;
	virtual i3GameRes *	CreateGameRes(void) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif // #if !defined ( __I3LV_RES_ANIMATION_H_)