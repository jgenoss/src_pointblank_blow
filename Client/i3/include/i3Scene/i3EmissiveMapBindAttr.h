#pragma once

#include "i3RenderAttr.h"
#include "i3TextureBindAttr.h"
#include "i3Gfx.h"

class I3_EXPORT_SCENE i3EmissiveMapBindAttr : public i3TextureBindAttr
{
	I3_EXPORT_CLASS_DEFINE( i3EmissiveMapBindAttr, i3TextureBindAttr);

public:
	i3EmissiveMapBindAttr(void);

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void	Dump( void) override;
	#endif
};
