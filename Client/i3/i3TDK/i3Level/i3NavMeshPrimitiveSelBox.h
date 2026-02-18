#pragma once

#include "i3NavMeshPrimitive.h"

class I3_EXPORT_TDK i3NavMeshPrimitiveSelBox : public i3NavMeshPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshPrimitiveSelBox, i3NavMeshPrimitive );
	
public:
	void	OnBuildMesh();
	void	OnRender();
};