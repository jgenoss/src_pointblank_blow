#if !defined(__I3_MESH_PARTICLE_H)
#define __I3_MESH_PARTICLE_H

#include "i3Node.h"
#include "i3GeometryAttr.h"

struct VERTEX
{
	VEC3D		position;
	VEC3D		normal;
};

struct TRI
{
	INT32		idx[3] = { -1, -1, -1 };
	REAL32		weight = 0.0f;
	REAL32		first = 0.0f;
};

class I3_EXPORT_SCENE i3MeshParticle : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE(i3MeshParticle, i3NamedElement);

protected:
	i3::rc_string m_Path;
	i3::vector<VERTEX> m_MeshVertexList;
	i3::vector<TRI> m_MeshTriIndexList;

	REAL32	m_TotalTriangleWeight = 0.0f;
	INT32	m_CurrentMeshIndex = 0;

	bool	m_bIsRandomMesh = false;

private:
	void	BuildSingleMeshObject(i3GeometryAttr * pAttr, MATRIX * pMat);
	REAL32	BuildMeshWeight(TRI * triangle);

	INT32	FindMesh(REAL32 weight, INT32 first, INT32 mid, INT32 last);

	INT32	GetRandomMesh();
	void	GetRandomPosAndNormal(INT32 idx, VEC3D * position, VEC3D * normal);

	INT32	InsertVertex(VERTEX newVertex);

	void	Traverse( i3Node * pNode, i3MatrixStack * pStack, i3Skeleton * pSkel);


public:
	virtual ~i3MeshParticle();

	void	InitMeshData();

	void	BuildMeshObject(i3Node * pNode);

	VEC3D	GetVertexPosition(INT32 idx)					{ return m_MeshVertexList[idx].position; }
	VEC3D	GetVertexNormal(INT32 idx)						{ return m_MeshVertexList[idx].normal; }

	REAL32	GetMeshWeight(INT32 idx)						{ return m_MeshTriIndexList[idx].weight; }

	INT32	GetVertexCount()								{ return m_MeshVertexList.size(); }
	INT32	GetMeshCount()									{ return m_MeshTriIndexList.size(); }

	void	GetMeshPosAndNormal(VEC3D * position, VEC3D * normal);

	void			SetMeshPath(const i3::rc_string& strPath)		{ m_Path = strPath; }
	const char *	GetMeshPath()									{ return m_Path.c_str(); }

	void			SetRandomMesh(bool bIsRandomMesh)				{ m_bIsRandomMesh = bIsRandomMesh; }
	bool			IsRandomMesh()									{ return m_bIsRandomMesh; }
	
	virtual UINT32 OnSave(i3ResourceFile * pResFile) override;
	virtual UINT32 OnLoad(i3ResourceFile * pResFile) override;
};

#endif