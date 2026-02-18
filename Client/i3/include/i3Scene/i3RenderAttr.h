#if !defined( __I3_RENDER_ATTR_H)
#define __I3_RENDER_ATTR_H

#include "i3Base.h"
#include "i3Gfx.h"
#include "i3SceneDef.h"

class I3_EXPORT_SCENE i3RenderAttr : public i3PersistantElement
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3RenderAttr, i3PersistantElement);

protected:
	UINT32		m_AttrID = 0xFFFFFFFF;

public:
	void		SetID( UINT32 id)				{ m_AttrID = id; }
	UINT32		GetID(void)					{ return m_AttrID; }

	virtual void Apply(i3RenderContext * pContext) {}
	virtual bool	IsSame(i3RenderAttr * pAttr1) { return false; }

	#if defined( I3_DEBUG)
	virtual void Dump(void);
	#endif

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave(i3ResourceFile * pResFile) override { return 0; }
	virtual UINT32	OnLoad(i3ResourceFile * pResFile) override { return 0; }

	virtual void	OnLostDevice(bool bLostDevice) {}
	virtual void	OnRevive(i3RenderContext * pCtx) {}

#if defined( I3_DEBUG)
protected:
	void *			m_pOwner = nullptr;

public:
	void			setOwner( void * pOwner)		{ m_pOwner = pOwner; }
	void *			getOwner( void)					{ return m_pOwner; }
#endif
};

#endif
