#include "i3SceneDef.h"
#include "i3MeshParticle.h"
#include "i3Geometry.h"
#include "i3GeometryResolver.h"
#include "i3Node.h"
#include "i3Body.h"
#include "i3BoneRef.h"
#include "i3SceneUtil.h"
#include "i3Transform.h"

#include "i3Base/string/ext/atoi.h"

#include <fstream>
#include <iostream>

I3_CLASS_INSTANCE(i3MeshParticle);

i3MeshParticle::~i3MeshParticle()
{
	m_MeshTriIndexList.clear();
	m_MeshVertexList.clear();
}

void i3MeshParticle::InitMeshData()
{
	m_MeshVertexList.clear();
	m_MeshTriIndexList.clear();
	m_TotalTriangleWeight = 0.0f;
	m_CurrentMeshIndex = 0;
}

INT32 i3MeshParticle::InsertVertex(VERTEX newVertex)
{
	for(size_t i = 0; i < m_MeshVertexList.size(); i++)
	{
		VERTEX vert  = m_MeshVertexList[i];

		if(newVertex.position.isAlike( vert.position, I3_EPS_REAL32) && 
		   newVertex.normal.isAlike( vert.normal, I3_EPS_REAL32))
		{
			return i;
		}
	}
	//리턴되지 않았다면, 이 점은 m_MeshVertexList에서 찾지 못한 새로운 점이다.
	m_MeshVertexList.push_back(newVertex);

	return (INT32)(m_MeshVertexList.size() - 1);
}

void i3MeshParticle::Traverse( i3Node * pNode, i3MatrixStack * pStack, i3Skeleton * pSkel)
{
	bool bStackPush = false;

	if( i3::same_of<i3Transform *>( pNode))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		pStack->PushPre( pTrans->GetCacheMatrix(), pTrans->GetMatrix());
		bStackPush = true;
	}
	else if (i3::same_of<i3BoneRef *>(pNode))
	{
		i3BoneRef * pRef = (i3BoneRef *)pNode;
		INT32 boneIndex = pRef->getBoneIndex();

		if(pSkel != nullptr)
		{
			MATRIX bone;
			pSkel->GetFullMatrix( boneIndex, &bone);

			pStack->Push( &bone, nullptr);
			bStackPush = true;
		}
	}
	else if( i3::same_of<i3Body *>( pNode))
	{
		i3Body * pBody = (i3Body *) pNode;

		if( pBody->getLODCount() > 0)
		{
			i3LOD * pLastLOD = pBody->getLOD( pBody->getLODCount() - 1);
			i3Skeleton * pNewSkel = pBody->getFrameLOD()->getSkeleton();

			Traverse( pLastLOD->getShapeNode(), pStack, pNewSkel);
		}
	}
	else if (i3::same_of<i3Geometry *>(pNode))
	{
		i3Geometry * pGeo = (i3Geometry *)pNode;

		for(INT32 i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			i3GeometryAttr * pAttr = (i3GeometryAttr *)pGeo->GetGeometryAttr(i);
			BuildSingleMeshObject(pAttr, pStack->GetTop());
		}
	}

	for(INT32 i = 0; i < pNode->GetChildCount(); i++)
	{
		Traverse( pNode->GetChild(i), pStack, pSkel);
	}

	if( bStackPush)
	{
		pStack->Pop();
	}
}

void i3MeshParticle::BuildMeshObject(i3Node * pNode)
{
	InitMeshData();

	i3MatrixStack stack;

	Traverse( pNode, &stack, nullptr);

	for(INT32 i = 0; i < (INT32)m_MeshTriIndexList.size(); i++)
	{
		TRI * tri = &m_MeshTriIndexList[i];

		tri->weight = tri->weight / m_TotalTriangleWeight;
		tri->first = tri->first / m_TotalTriangleWeight;
	}
}

void i3MeshParticle::BuildSingleMeshObject(i3GeometryAttr * pAttr, MATRIX * pMat)
{
	i3GeometryResolver resolver;

	resolver.setGeometryAttr(pAttr);

	for(INT32 i = 0; i < resolver.getPrimitiveCount(); i++)
	{
		VEC3D pos[3];
		VEC3D normal[3];

		resolver.getPositions(i, pos);
		resolver.getNormals(i, normal);

		TRI tri;

		for(INT32 j = 0; j < 3; j++)
		{
			VERTEX vert;

			i3Matrix::TransformPoints(&vert.position, &pos[j], 1, pMat);
			i3Matrix::TransformPoints(&vert.normal, &normal[j], 1, pMat);

			tri.idx[j] = InsertVertex(vert);
		}

		tri.first = m_TotalTriangleWeight;
		m_TotalTriangleWeight += BuildMeshWeight(&tri);

		m_MeshTriIndexList.push_back(tri);
	}
}

REAL32 i3MeshParticle::BuildMeshWeight(TRI * triangle)
{
	VEC3D pos[3];

	for(INT32 i = 0; i < 3; i++)
	{
		pos[i] = m_MeshVertexList[triangle->idx[i]].position;
	}

	VEC3D u, v, w;
	u = pos[0] - pos[1];
	v = pos[2] - pos[1];

	i3Vector::Cross(&w, &u, &v);

	triangle->weight = i3Vector::Length(&w) * 0.5f;

	return triangle->weight;
}

void i3MeshParticle::GetMeshPosAndNormal(VEC3D * position, VEC3D * normal)
{
	if(m_bIsRandomMesh)
	{
		INT32 meshIdx = GetRandomMesh();

		I3ASSERT(meshIdx >= 0);

		GetRandomPosAndNormal(meshIdx, position, normal);
	}
	else
	{
		if(m_CurrentMeshIndex >= (INT32)m_MeshVertexList.size())
		{
			m_CurrentMeshIndex = 0;
		}

		VERTEX vert = m_MeshVertexList[m_CurrentMeshIndex];
		
		*position = vert.position;
		*normal = vert.normal;

		m_CurrentMeshIndex++;
	}
}

INT32 i3MeshParticle::GetRandomMesh()
{
	REAL32 randIdx = i3Math::Randf();

	INT32 meshCount = (INT32)m_MeshTriIndexList.size() - 1;

	return FindMesh(randIdx, 0, meshCount / 2 ,meshCount);
}

INT32 i3MeshParticle::FindMesh(REAL32 weight, INT32 first, INT32 mid, INT32 last)
{
	TRI tri = m_MeshTriIndexList[mid];

	if(weight < tri.first)
	{
		return FindMesh(weight, first, (first + mid) / 2 , mid);
	}
	else if (weight > tri.first)
	{
		if(weight <= (tri.first + tri.weight))
		{
			return mid;
		}

		return FindMesh(weight, mid, ((mid + last) / 2) + 1, last);
	}
	else
	{
		return mid;
	}
}

void i3MeshParticle::GetRandomPosAndNormal(INT32 idx, VEC3D * position, VEC3D * normal)
{
	TRI tri = m_MeshTriIndexList[idx];

	VEC3D pos[3];
	VEC3D norm[3];
	
	for(INT32 i = 0; i < 3; i++)
	{
		pos[i] = m_MeshVertexList[tri.idx[i]].position;
		norm[i] = m_MeshVertexList[tri.idx[i]].normal;
	}
	
	REAL32 kValue = i3Math::Randf();
	REAL32 lValue = i3Math::Randf();

	kValue = 1.f - kValue*kValue;
	lValue = 1.f - lValue*lValue;

	VEC3D u, v, w;
	u = pos[2] - pos[1];
	v = pos[0] - pos[1];

	u *= kValue;
	v *= lValue;

	REAL32 t3 = i3Math::Randf();
	*position = ((pos[1] + u) * (1.f - t3)) + ((pos[1] + v) * t3);

	w = (norm[0] + norm[1] + norm[2]) / 3;
	i3Vector::Normalize(&w, &w);
	i3Vector::Copy(normal, &w);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct MESHPARTICLE
	{
		char		m_ID[4] = { 'M', 'P', 'C', '1' };
		INT32		nPrimCount = 0;
		INT32		nVertexCount = 0;
		INT32		nLength = 0;
		char		m_MeshPath[MAX_PATH] = { 0 };
		REAL32		nTotalMeshWeight = 0.0f;
		BOOL		bIsRandomMesh = FALSE;
		UINT32		pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3MeshParticle::OnSave(i3ResourceFile * pFile)
{
	UINT32 Rc, result = 0;
	pack::MESHPARTICLE info;

	info.nPrimCount			= (INT32)m_MeshTriIndexList.size();
	info.nVertexCount		= (INT32)m_MeshVertexList.size();
	info.nLength			= static_cast<INT32>(m_Path.length());
	info.nTotalMeshWeight	= m_TotalTriangleWeight;
	info.bIsRandomMesh		= m_bIsRandomMesh;

	if(info.nLength > 0)
	{
		i3::safe_string_copy(info.m_MeshPath, m_Path.c_str(), sizeof(info.m_MeshPath));
	}

	Rc = pFile->Write( &info, sizeof(info));
	
	if(info.nPrimCount > 0)
	{
		Rc = pFile->Write(&m_MeshTriIndexList.front(), info.nPrimCount * sizeof(TRI));
	}

	if(info.nVertexCount > 0)
	{
		Rc = pFile->Write(&m_MeshVertexList.front(), info.nVertexCount * sizeof(VERTEX));
	}

	return result;
}

UINT32 i3MeshParticle::OnLoad(i3ResourceFile * pFile)
{
	UINT32 Rc, Result = 0;
	pack::MESHPARTICLE info;

	Rc = pFile->Read(&info, sizeof(info));

	I3_CHKIO( Rc );

	if(info.nLength > 0)
	{
		char szTemp[MAX_PATH];
		i3::safe_string_copy(szTemp, info.m_MeshPath, sizeof(info.m_MeshPath));

		SetMeshPath(szTemp);
	}

	m_TotalTriangleWeight = info.nTotalMeshWeight;
	m_bIsRandomMesh = info.bIsRandomMesh != 0;

	if(info.nPrimCount > 0)
	{
		m_MeshTriIndexList.clear();
		m_MeshTriIndexList.resize(info.nPrimCount);

		Rc = pFile->Read(&m_MeshTriIndexList.front(), info.nPrimCount * sizeof(TRI));
		I3_CHKIO(Rc);
	}

	if(info.nVertexCount > 0)
	{
		m_MeshVertexList.clear();
		m_MeshVertexList.resize(info.nVertexCount);

		Rc = pFile->Read(&m_MeshVertexList.front(), info.nVertexCount * sizeof(VERTEX));
		I3_CHKIO(Rc);
	}

	return Result;
}