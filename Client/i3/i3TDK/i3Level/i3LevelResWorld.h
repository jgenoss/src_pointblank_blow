#if !defined( __I3_LV_RES_WORLD_H)
#define __I3_LV_RES_WORLD_H

#include "i3LevelRes.h"
#include "i3LevelResSceneGraph.h"

class I3_EXPORT_TDK i3LevelResWorld : public i3LevelRes
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResWorld, i3LevelRes);

protected:

	i3World *		m_pWorld = nullptr;

public:
	i3LevelResWorld(void);
	virtual ~i3LevelResWorld(void);

	void			CreateEmptyWorld(void);

	i3World *		getWorld(void)					{ return m_pWorld; }
	void			setWorld( i3World * pWorld);

	void			SetWorld( i3LevelResSceneGraph * pSg);
	void			SetSky( i3LevelResSceneGraph * pSg);
	void			SetWorldCollision( i3LevelResSceneGraph * pSg);
	void			SetHitCollision( i3LevelResSceneGraph * pSg);
	void			SetLightVolumeCollision( i3LevelResSceneGraph * pSg);

	virtual i3LevelElement3D *	CreateInstance( i3LevelScene * pScene) override;
	virtual bool				OnValidateBrief( bool bForce) override;
	virtual bool				OnValidate( i3LevelScene * pScene, bool bForce) override;
	virtual bool				OnSerialize(void) override;

	virtual i3GameRes *			CreateGameRes(void) override;
	virtual UINT32				OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32				OnLoad( i3ResourceFile * pResFile) override;
};

#endif
