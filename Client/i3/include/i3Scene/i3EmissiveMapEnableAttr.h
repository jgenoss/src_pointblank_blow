#pragma once

#include "i3RenderAttr.h"
#include "i3AbstractBoolAttr.h"

class I3_EXPORT_SCENE i3EmissiveMapEnableAttr : public i3AbstractBoolAttr
{
	I3_EXPORT_CLASS_DEFINE( i3EmissiveMapEnableAttr, i3AbstractBoolAttr);

public:
	i3EmissiveMapEnableAttr(void);
	virtual void	Apply( i3RenderContext * pContext) override;
};
