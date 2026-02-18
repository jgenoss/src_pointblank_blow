#include "i3SceneDef.h"

#include "i3SkinMeshAABBProcessor.h"

#include "i3GeometryAttr.h"
#include "i3SkinGeometryAttr.h"

#pragma push_macro("min")
#undef min

#pragma push_macro("max")
#undef max
 

namespace
{
	INT32 findBigBoneWhich(i3VertexArray* pVA, INT32 currVertIdx, INT32 numWeightPerVert, bool bNeedLastWeight)
	{
		float fWeightSum = 0.f;	float fMaxWeight = 0.f;
		INT32 resultIndex = -1;

		for (INT32 j = 0; j < numWeightPerVert; ++j)
		{
			REAL32 fWeight = pVA->GetBlendWeight(j, currVertIdx);
			if (fWeight > fMaxWeight)
			{
				fMaxWeight = fWeight;
				resultIndex = j;
			}
			fWeightSum += fWeight;
			if (fWeightSum >= 1.f - I3_EPS)
				return resultIndex;				// 여기서 바로 종료 오케이..
		}

		if (bNeedLastWeight)
		{
			REAL32 fWeight = 1.f - fWeightSum;
			if (fWeight > fMaxWeight)
				return numWeightPerVert;
		}

		return resultIndex;
	}

	void	CreateLocalAABBArray(i3VertexArray* pVA, i3::svector<i3::aabb>& aLocalAABB)
	{
		bool bNeedUnlock = false;
		if (pVA->IsLocked() == false)
		{
			bNeedUnlock = true;	pVA->Lock();
		}

		const INT32 numBonePerVert = pVA->GetBlendIndexCount();			// 최대본갯수일뿐이고, 실제로는 그 이하이다..
		const INT32 numWeightPerVert = pVA->GetBlendWeightCount();

		const INT32 vertCount = pVA->GetCount();

		bool bNeedLastWeight = (numBonePerVert > numWeightPerVert);

		i3::vector< i3::vector<INT32> > vecBoneToVertexTable;
		vecBoneToVertexTable.reserve(255);		// 최대 255가 넘을수가 없는것으로 알고는 있음..

		for (INT32 i = 0; i < vertCount; ++i)
		{
			INT32 bigBoneWhich = findBigBoneWhich(pVA, i, numWeightPerVert, bNeedLastWeight);

			if (bigBoneWhich == -1)
				continue;

			UINT32 bigBoneIndex = pVA->GetBlendIndex(bigBoneWhich, i);

			if (bigBoneIndex >= 255)
				continue;

			if (bigBoneIndex >= vecBoneToVertexTable.size())
				vecBoneToVertexTable.resize(bigBoneIndex + 1);

			vecBoneToVertexTable[bigBoneIndex].push_back(i);
		}

		const size_t num_table = vecBoneToVertexTable.size();
		aLocalAABB.resize(num_table);			// 로컬 AABB는 첫 계산 후 변경되지 않아야한다.

		VEC3D vecPos1;	i3::vec3 vecPos2;

		for (size_t i = 0; i < num_table; ++i)
		{
			const i3::vector<INT32>& vertList = vecBoneToVertexTable[i];
			i3::aabb&				 aabbPerBone = aLocalAABB[i];

			const size_t num_vert = vertList.size();

			for (size_t j = 0; j < num_vert; ++j)
			{
				pVA->GetPosition(vertList[j], &vecPos1);
				vecPos2.set(vecPos1.x, vecPos1.y, vecPos1.z);
				aabbPerBone.merge(vecPos2);
			}
		}

		if (bNeedUnlock)
			pVA->Unlock();
	}

}

i3SkinMeshAABBProcessor::i3SkinMeshAABBProcessor(i3VertexArray* pVA)
{
	CreateLocalAABBArray(pVA, m_aLocalAABB);
}

i3SkinMeshAABBProcessor::i3SkinMeshAABBProcessor(i3GeometryAttr* owner)
{
	i3VertexArray* pVA = owner->GetVertexArray();
	CreateLocalAABBArray(pVA, m_aLocalAABB);
}

i3SkinMeshAABBProcessor::i3SkinMeshAABBProcessor(i3SkinGeometryAttr* owner)
{
	i3VertexArray* pVA = owner->GetVertexArray();
	i3IndexArray*  pIA = owner->GetIndexArray();

	bool bNeedUnlockVB = false;
	bool bNeedUnlockIB = false;

	if (pVA->IsLocked() == false)
	{
		bNeedUnlockVB = true;	pVA->Lock();
	}

	if (pIA && pIA->IsLocked() == false)
	{
		bNeedUnlockIB = true;	pIA->Lock();
	}

	// 버텍스 중복문제로 인하여, 인덱스리스트 중심으로 되어있는 I3_DRAW_GROUP을 
	// 버텍스버퍼 중심으로 변경해준다..
	const UINT32 numVert = pVA->GetCount();
	i3::vector<I3_DRAW_GROUP*>	vecDrawGroup(numVert);

	const INT32 numSkinGroupCount = owner->getSkinGroupCount();

	I3G_PRIMITIVE primType = owner->GetPrimitiveType();
	INT32 idxEndCount = 0;

	for (INT32 i = 0; i < numSkinGroupCount; ++i)
	{
		I3_DRAW_GROUP * pGroup = owner->getSkinGroup(i);

		switch (primType)
		{
		case I3G_PRIM_LINELIST:
			idxEndCount = pGroup->m_idxStart + pGroup->m_PrimCount * 2;
		break;
		case I3G_PRIM_LINESTRIP:
			idxEndCount = pGroup->m_idxStart + pGroup->m_PrimCount;
		break;
		case I3G_PRIM_TRILIST:
			idxEndCount = pGroup->m_idxStart + pGroup->m_PrimCount * 3;
		break;
		case I3G_PRIM_TRISTRIP:
		case I3G_PRIM_TRIFAN:
			if (pGroup->m_PrimCount == 0)
				idxEndCount = pGroup->m_idxStart;
			else
				idxEndCount = pGroup->m_idxStart + 2 + pGroup->m_PrimCount;
		break;
		default:
			idxEndCount = pGroup->m_idxStart;
		break;
		}

		if (pIA != nullptr)
		{
			for (INT32 j = pGroup->m_idxStart; j < idxEndCount; ++j)
			{
				UINT32 vertIdx = pIA->GetIndex(j);

				if (vertIdx >= numVert)		// 처리될수 없음..그냥 컨티뉴?
					continue;

				I3_DRAW_GROUP*& refDrawGroup = vecDrawGroup[vertIdx];

				if (refDrawGroup == nullptr)  // 처음인 것만 갱신해준다...처음이 아니라면 버그라고 판단됨..
					refDrawGroup = pGroup;
			}
		}
		else // 인덱스버퍼가 없는 경우엔 그냥 순차적으로 넣어주어본다..
		{
			const INT32 idxEndCount2 = (idxEndCount > INT32(numVert) ) ? INT32(numVert) : idxEndCount;

			for (INT32 j = pGroup->m_idxStart; j < idxEndCount2; ++j)
			{
				I3_DRAW_GROUP*& refDrawGroup = vecDrawGroup[j];
				refDrawGroup = pGroup;
			}
		}
		
	}

	if (bNeedUnlockIB)
		pIA->Unlock();

	// 이제 버텍스버퍼를 돌면서...해당 버텍스의 진짜 본인덱스를 찾아서 매핑...
	
	const INT32 numBonePerVert = pVA->GetBlendIndexCount();			// 최대본갯수일뿐이고, 실제로는 그 이하이다..
	const INT32 numWeightPerVert = pVA->GetBlendWeightCount();

	const INT32 vertCount = pVA->GetCount();

	bool bNeedLastWeight = (numBonePerVert > numWeightPerVert);

	i3::vector< i3::vector<INT32> > vecBoneToVertexTable;
	vecBoneToVertexTable.reserve(255);		// 최대 255가 넘을수가 없는것으로 알고는 있음..

	for (INT32 i = 0; i < vertCount; ++i)
	{
		I3_DRAW_GROUP* pGroupPerVert = vecDrawGroup[i];

		if (pGroupPerVert == nullptr)		// 없으면 처리 안함..
			continue;

		INT32 bigBoneWhich = findBigBoneWhich(pVA, i, numWeightPerVert, bNeedLastWeight);

		if (bigBoneWhich == -1)
			continue;

		INT32 bigBoneIndex = pVA->GetBlendIndex(bigBoneWhich, i);

		if (bigBoneIndex >= pGroupPerVert->m_BoneCount)		// 본갯수를 넘어서면 아웃...
			continue;

		UINT32 realBoneIdx = pGroupPerVert->m_Bone[bigBoneIndex];		// 아..이게 20개로 고정되있구나..원샷렌더링엔 좋지 않다.
		
		if (realBoneIdx >= vecBoneToVertexTable.size())
			vecBoneToVertexTable.resize(realBoneIdx+1);

		vecBoneToVertexTable[realBoneIdx].push_back(i);
	}

	const size_t num_table = vecBoneToVertexTable.size();
	m_aLocalAABB.resize(num_table);			// 로컬 AABB는 첫 계산 후 변경되지 않아야한다.

	VEC3D vecPos1;	i3::vec3 vecPos2;

	for (size_t i = 0; i < num_table; ++i)
	{
		const i3::vector<INT32>& vertList = vecBoneToVertexTable[i];
		i3::aabb&				 aabbPerBone = m_aLocalAABB[i];

		const size_t num_vert = vertList.size();

		for (size_t j = 0; j < num_vert; ++j)
		{
			pVA->GetPosition(vertList[j], &vecPos1);
			vecPos2.set(vecPos1.x, vecPos1.y, vecPos1.z);
			aabbPerBone.merge(vecPos2);
		}
	}
	
	if (bNeedUnlockVB)
		pVA->Unlock();
}

i3SkinMeshAABBProcessor::~i3SkinMeshAABBProcessor()
{

}

void	i3SkinMeshAABBProcessor::CalcWorldAABB(i3BoneMatrixListAttr* bone_mat_attr, i3::aabb& outAABB)
{
	outAABB.reset();
	const INT32 num_bone = (bone_mat_attr->getBoneCount() < INT32(m_aLocalAABB.size())) ?
		bone_mat_attr->getBoneCount() : INT32(m_aLocalAABB.size());

	i3::aabb transformed_aabb;

	for (INT32 i = 0; i < num_bone; ++i)
	{
		const i3::aabb& aabb_per_bone = m_aLocalAABB[i];

		if (aabb_per_bone.is_valid() == false)			// 이 경우는 계산하지 않음...
			continue;

		const i3::mat4& m = reinterpret_cast<const i3::mat4&>(*bone_mat_attr->getTransformedMatrix(i));
		aabb_per_bone.transform(transformed_aabb, m);

		outAABB.merge(transformed_aabb);
	}
	
}

#pragma pop_macro("max")
#pragma pop_macro("min")

