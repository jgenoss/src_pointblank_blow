#pragma once

#include "i3NavMeshPrimitive.h"
#include "i3NavMeshUtil.h"

class i3NavMesh;
class i3NavMeshDataMgr;

class i3LevelFramework;

class I3_EXPORT_TDK i3NavMeshPrimitiveVertex : public i3NavMeshPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshPrimitiveVertex, i3NavMeshPrimitive );
	
public:
	void	OnBuildMesh();
	void	OnRender();

private:
	bool		_Calculate_DrawList( i3NavMesh* pNav, i3NavMeshDataMgr* dataMgr, i3LevelFramework* pFramework, i3::vector< int >& drawList, i3::set< int >& selList );

	void		_Render_Vertex( const VEC3D& pos, float scale, I3G_RENDER_MODE _mode, COLORREAL& cr );

private:
	COLORREAL		m_crNormal = { 1.0f, 0.1f, 0.1f, 1.0f };
	COLORREAL		m_crSelect = { 1.0f, 1.0f, 0.2f, 1.0f };
	COLORREAL		m_crPickAdd = { 0.7f, 0.7f, 0.2f, 1.0f };
};