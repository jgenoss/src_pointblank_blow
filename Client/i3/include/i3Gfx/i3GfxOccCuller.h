#pragma once

#include "i3GfxResource.h"


class i3Shader;

namespace detail_occ 
{
	struct BOX_GEOMETRY; struct SHADER_PARAM; struct QUAD_INSTANCE;
}

namespace i3 
{ 
	struct vec3; struct mat4; 
	class octree_item;	class occ_quad;
}

class I3_EXPORT_GFX i3GfxOccCuller : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE(i3GfxOccCuller, i3GfxResource);

public:
	i3GfxOccCuller(void);
	virtual ~i3GfxOccCuller(void);

	void				Create();

	int				UpdateOcclusions(const i3::vector<i3::occ_quad*>& occluder_list,
		const i3::vector<i3::octree_item*>& occludee_list, const i3::mat4& matViewProj);
	
	
private:
	void				AddOccluderQuad(const i3::vec3& p0, const i3::vec3& p1, const i3::vec3& p2, const i3::vec3& p3);
	void				AddOccluderTri(const i3::vec3& p0, const i3::vec3& p1, const i3::vec3& p2);	//degen 시켜서 처리함..
	void				AddOccludee(i3::octree_item* item);
	int					Update(const i3::mat4& matViewProj);

	LPDIRECT3DQUERY9		m_d3dQuery = nullptr;			// The query
	LPDIRECT3DSURFACE9      m_occSurface = nullptr;		// 렌더타겟 서피스로만 계산한다. 텍스쳐를 쓴다면 디버깅용도로만 쓸 것..
	LPDIRECT3DSURFACE9		m_occDepthSurf = nullptr;

	i3Shader*				m_occShader = nullptr;		// 이건 엔진 의존적으로..

	IDirect3DVertexDeclaration9* m_VertexDecl = nullptr;

	detail_occ::BOX_GEOMETRY*	m_BoxGeom = nullptr;
	detail_occ::QUAD_INSTANCE*	m_QuadInstance = nullptr;
	detail_occ::SHADER_PARAM*	m_ShaderParam;

	virtual bool		Destroy(bool bLostDevice) override;
	virtual bool		isDestroyed(void) override;
	virtual bool		Revive(i3RenderContext * pCtx) override;
};

