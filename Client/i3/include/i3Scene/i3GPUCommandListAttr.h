#if !defined( __I3_GPU_COMMAND_LIST_ATTR_H)
#define __I3_GPU_COMMAND_LIST_ATTR_H

#include "i3Gfx.h"
#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3GPUCommandListAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3GPUCommandListAttr, i3RenderAttr);
protected:
	i3GPUCommandList *		m_pCmd = nullptr;

public:
	i3GPUCommandListAttr(void);
	virtual ~i3GPUCommandListAttr(void);

	i3GPUCommandList *		Get(void)			{return m_pCmd; }
	void					Set( i3GPUCommandList * pCmdList);

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	//virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
