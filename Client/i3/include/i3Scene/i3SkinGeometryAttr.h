#if !defined( __I3_SKIN_GEOMETRY_ATTR_H)
#define __I3_SKIN_GEOMETRY_ATTR_H

#include "i3Base.h"
#include "i3Gfx.h"
#include "i3GeometryAttr.h"
#include "i3SkinBuilder.h"

class I3_EXPORT_SCENE i3SkinGeometryAttr : public i3GeometryAttr
{
	I3_EXPORT_CLASS_DEFINE( i3SkinGeometryAttr, i3GeometryAttr);

protected:
	INT32					m_SkinGroupCount = 0;
	i3MemoryBuffer *		m_pSkinGroup = nullptr;

public:
	virtual ~i3SkinGeometryAttr(void);

	INT32					getSkinGroupCount(void)				{ return m_SkinGroupCount; }
	void					SetSkinGroupCount( INT32 cnt);

	i3MemoryBuffer *		getSkinGroupBuffer(void)			{ return m_pSkinGroup; }
	I3_DRAW_GROUP *	getSkinGroup( INT32 idx);

	virtual void	OptimizeForSkin(void) override;

	void					Setup( i3GeometryAttr * pSrcAttr);

	// for i3RenderAttr
	virtual void	Apply( i3RenderContext * pContext) override;
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	//virtual BOOL	IsSame( i3RenderAttr * pAttr);
	// for i3PersistantElement
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void	Dump(void) override;
	#endif
};

#endif
