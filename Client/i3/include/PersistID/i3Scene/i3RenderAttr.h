#if !defined( __I3_RENDER_ATTR_H)
#define __I3_RENDER_ATTR_H

#include "i3Base.h"
#include "i3Gfx.h"
#include "i3SceneDef.h"

class I3_EXPORT_SCENE i3RenderAttr : public i3PersistantElement
{
	I3_ABSTRACT_CLASS_DEFINE( i3RenderAttr);

protected:
	UINT32		m_AttrID;

public:
	i3RenderAttr(void);
	virtual ~i3RenderAttr(void);

	void		SetID( UINT32 id)				{ m_AttrID = id; }
	UINT32		GetID(void)					{ return m_AttrID; }

	virtual void Apply( i3RenderContext * pContext);
	virtual BOOL	IsSame( i3RenderAttr * pAttr1);

	#if defined( I3_DEBUG)
	virtual void Dump(void);
	#endif

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
