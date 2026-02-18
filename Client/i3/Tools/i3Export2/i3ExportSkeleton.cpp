#include "stdafx.h"
#include "i3Export.h"
#include "Util.h"
#include "i3Base/string/compare/generic_is_niequal.h"

void i3Export::_AddJointInfo( INode * pBone, INT32 depth)
{
	i3tool::exporter::SKIN_JOINT * pInfo;

	pInfo = new i3tool::exporter::SKIN_JOINT;

	pInfo->m_pBoneNode = pBone;
	pInfo->m_Depth = depth;

	m_JointList.push_back( pInfo);
}

void i3Export::_RemoveAllJointInfo(void)
{
	INT32 i;
	i3tool::exporter::SKIN_JOINT * pInfo;

	for( i = 0; i < _getJointInfoCount(); i++)
	{
		pInfo = _getJointInfo( i);

		delete pInfo;
	}

	m_JointList.clear();
}

void i3Export::_Rec_BuildJointInfo( INT32 depth, INode * pNode)
{
	INode * pChild;
	INT32 i;
	::Control * pCtrl = pNode->GetTMController();
	BONE_TYPE boneType;

	if( pCtrl != NULL)
	{
		const ulong partA = pCtrl->ClassID().PartA();
		const ulong partB = pCtrl->ClassID().PartB();

		boneType = isValidBone( pNode, m_Option.m_bTreatControlAsBone != 0);

		if( boneType == BONE_TYPE_BONEREF)
		{
			if( i3::generic_is_niequal( pNode->GetName(), "Pivot", 5) ||
				i3::generic_is_niequal( pNode->GetName(), "Bone", 4))
			{
				boneType = BONE_TYPE_SKELETON;
			}
			else
			{
				// Biped Bone이 아닌 일반적인 Dummy는 직접 Bone에 추가하지 않고
				// i3BoneRef Node를 이용해 별도로 추가한다.
				m_BoneRefList.push_back( pNode);
			}
		}
	}

	if( boneType == BONE_TYPE_SKELETON)
	{
		_AddJointInfo( pNode, depth);
	
		for( i = 0; i < pNode->NumberOfChildren(); i++)
		{
			pChild = pNode->GetChildNode( i);

			_Rec_BuildJointInfo( depth + 1, pChild);
		}
	}
}

void i3Export::AddSkeletonInfo( i3Skin * pSkin, i3Skeleton * pSkel, INode * pBoneRoot)
{
	i3tool::exporter::SKELETON_INFO * pNew;

	pNew = new i3tool::exporter::SKELETON_INFO;

	memset( pNew, 0, sizeof(i3tool::exporter::SKELETON_INFO));

	pNew->m_pSkin = pSkin;
	pNew->m_pSkeleton = pSkel;
	pNew->m_pBoneRoot = pBoneRoot;

	m_SkelList.push_back( pNew);
}

void i3Export::RemoveAllSkeletonInfo(void)
{

	i3tool::exporter::SKELETON_INFO * pInfo;

	for( size_t i = 0; i < m_SkelList.size(); i++)
	{
		pInfo = m_SkelList[i];

		delete pInfo;
	}

	m_SkelList.clear();
}

i3tool::exporter::SKELETON_INFO *	i3Export::FindSkeletonByBoneRoot( INode * pBoneRoot)
{

	i3tool::exporter::SKELETON_INFO * pInfo;

	for(size_t i = 0; i < m_SkelList.size(); i++)
	{
		pInfo = m_SkelList[i];

		if( pInfo->m_pBoneRoot == pBoneRoot)
			return pInfo;
	}

	return NULL;
}
