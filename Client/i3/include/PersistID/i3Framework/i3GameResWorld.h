#if !defined( _I3_GAMERES_WORLD_H__)
#define	_I3_GAMERES_WORLD_H__

#include "i3GameResSceneGraph.h"
#include "i3World.h"

class I3_EXPORT_FRAMEWORK i3GameResWorld : public i3GameResSceneGraph
{
	I3_CLASS_DEFINE( i3GameResWorld);
protected:
	i3World *		m_pWorld;

public:
	i3GameResWorld(void);
	virtual ~i3GameResWorld(void);

	virtual void	Bind( i3Framework * pFramework, i3GameObjBase * pObj, bool bInstancing, bool bClone);

	i3World *		getWorld(void)				{ return m_pWorld; }
	void			setWorld( i3World * pWorld)	{ I3_REF_CHANGE( m_pWorld, pWorld); }

public:
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif