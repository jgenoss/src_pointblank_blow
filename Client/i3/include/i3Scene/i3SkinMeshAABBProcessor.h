#pragma once


class i3BoneMatrixListAttr;
class i3GeometryAttr;
class i3SkinGeometryAttr;

#include "i3Base/itl/svector.h"
#include "i3Math/structure/aabb.h"

// SetVertexArray 내부에서 다른 처리 직후에 생성하도록 한다...
#pragma push_macro("min")
#undef min

#pragma push_macro("max")
#undef max

class i3SkinMeshAABBProcessor
{
public:
	i3SkinMeshAABBProcessor(i3VertexArray* pVA);
	i3SkinMeshAABBProcessor(i3GeometryAttr* owner);
	i3SkinMeshAABBProcessor(i3SkinGeometryAttr* owner);
	~i3SkinMeshAABBProcessor();

	void						CalcWorldAABB(i3BoneMatrixListAttr* bone_mat_attr, i3::aabb& outAABB);
	
	INT32						GetNumBoneAABB() const { return INT32(m_aLocalAABB.size());  }
	const i3::aabb&				GetLocalAABB(INT32 boneIdx) const { return m_aLocalAABB[boneIdx]; }
	bool						IsLocalAABBValid(INT32 boneIdx) const { return m_aLocalAABB[boneIdx].is_valid(); }

private:
	i3::svector<i3::aabb>		m_aLocalAABB;			// 3개의 벡터사이즈는 버텍스버퍼가 쥐고 있는 대표 본갯수(가중치가 제법 높은 것들)과 일치할 것이다...
	
};


#pragma pop_macro("max")
#pragma pop_macro("min")