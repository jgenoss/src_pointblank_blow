#if !defined( _I3_GAMERES_WORLD_H__)
#define	_I3_GAMERES_WORLD_H__

#include "i3GameResSceneGraph.h"
#include "i3World.h"

class I3_EXPORT_FRAMEWORK i3GameResWorld : public i3GameResSceneGraph
{
	I3_EXPORT_CLASS_DEFINE( i3GameResWorld, i3GameResSceneGraph);
protected:
	i3World *		m_pWorld = nullptr;

public:
	i3GameResWorld(void);
	virtual ~i3GameResWorld(void);

	virtual void	Bind( i3Framework * pFramework, i3GameObjBase * pObj, bool bInstancing, bool bClone) override;

	i3World *		getWorld(void)				{ return m_pWorld; }
	void			setWorld( i3World * pWorld)	{ I3_REF_CHANGE( m_pWorld, pWorld); }

public:
	virtual void	Destroy( void) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif