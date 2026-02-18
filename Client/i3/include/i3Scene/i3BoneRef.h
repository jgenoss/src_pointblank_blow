#if !defined( __I3_BONE_REF_H)
#define __I3_BONE_REF_H

#include "i3Transform.h"
#include "i3Body.h"

/** \brief
	\desc leaf class */

class I3_EXPORT_SCENE i3BoneRef : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3BoneRef, i3Node);

protected:
	INT32					m_BoneIndex = -1;

	virtual void		_GetWrappingBound( I3_GETBOUND_INFO * pInfo) override;

public:
	i3BoneRef(void);

	INT32					getBoneIndex(void)				{ return m_BoneIndex; }
	void					setBoneIndex( INT32 idx)		{ m_BoneIndex = idx; }

	virtual void			OnUpdate( i3SceneTracer * pTracer) override;
	virtual void			PreTrace(i3SceneTracer * pTracer) override;

	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void			OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;

	virtual void			OnInstancing( I3_ONINSTANCING_INFO * pInfo) override;
};

#endif
