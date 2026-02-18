#pragma once

#include "i3NavMeshPrimitive.h"
#include "i3NavMeshUtil.h"

class i3NavMesh;

class I3_EXPORT_TDK i3NavMeshPrimitivePolyCut : public i3NavMeshPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshPrimitivePolyCut, i3NavMeshPrimitive );

public:
	void	OnBuildMesh();
	void	OnRender();
};