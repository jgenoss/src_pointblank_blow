#if !defined( __I3_Level_WORLD_H)
#define __I3_Level_WORLD_H

#include "i3LevelElement3D.h"

class I3_EXPORT_TDK i3LevelWorld : public i3LevelElement3D
{
	I3_EXPORT_CLASS_DEFINE( i3LevelWorld, i3LevelElement3D);

protected:
	i3World *		m_pWorld = nullptr;

public:
	i3LevelWorld(void);

	virtual void		BindRes( i3LevelRes * pRes) override;
	virtual i3GameObj *	CreateGameInstance(void) override;

	i3World			*	getWorld( void)		{	return m_pWorld;}
	void				setWorld( i3World * pWorld)	{	I3_REF_CHANGE( m_pWorld, pWorld);}

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void		OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

	virtual void		OnInstantiate() override;
	virtual void		OnQueryCollisionNode( i3::vector<i3Node*>& List) override;

	virtual void		OnAttachScene( i3LevelScene * pScene ) override;
	virtual void		OnDetachScene( i3LevelScene * pScene ) override;
};

#endif