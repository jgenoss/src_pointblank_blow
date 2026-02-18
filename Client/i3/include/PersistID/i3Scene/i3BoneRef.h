#if !defined( __I3_BONE_REF_H)
#define __I3_BONE_REF_H

#include "i3BoneMatrixListAttr.h"
#include "i3Transform.h"
#include "i3Body.h"

class I3_EXPORT_SCENE i3BoneRef : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3BoneRef);

protected:
	INT32					m_BoneIndex;

	virtual void		_GetWrappingBound( I3_GETBOUND_INFO * pInfo);

public:
	i3BoneRef(void);
	virtual ~i3BoneRef(void);

	INT32					getBoneIndex(void)				{ return m_BoneIndex; }
	void					setBoneIndex( INT32 idx)		{ m_BoneIndex = idx; }

	virtual void			OnUpdate( i3SceneTracer * pTracer);
	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void			OnBuildObjectList( i3List * pList);
	virtual UINT32			OnSave( i3ResourceFile * pResFile);
	virtual UINT32			OnLoad( i3ResourceFile * pResFile);
};

#endif
