#if !defined( __I3_GPU_COMMAND_LIST_ATTR_H)
#define __I3_GPU_COMMAND_LIST_ATTR_H

#include "i3Gfx.h"
#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3GPUCommandListAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3GPUCommandListAttr);
protected:
	i3GPUCommandList *		m_pCmd;

public:
	i3GPUCommandListAttr(void);
	virtual ~i3GPUCommandListAttr(void);

	i3GPUCommandList *		Get(void)			{return m_pCmd; }
	void					Set( i3GPUCommandList * pCmdList);

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
