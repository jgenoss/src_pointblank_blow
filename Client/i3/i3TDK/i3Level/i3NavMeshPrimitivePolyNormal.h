#pragma once

#include "i3NavMeshPrimitive.h"

class I3_EXPORT_TDK i3NavMeshPrimitivePolyNormal : public i3NavMeshPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshPrimitivePolyNormal, i3NavMeshPrimitive );
	
public:
	void	OnBuildMesh();
	void	OnRender();
};