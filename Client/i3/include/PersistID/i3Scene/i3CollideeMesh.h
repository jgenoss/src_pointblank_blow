#if !defined( __I3_COLLIDEE_MESH_H)
#define __I3_COLLIDEE_MESH_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Collidee.h"
#include "i3CollideeLine.h"
#include "i3CollideeSphere.h"
#include "i3CollideeCylinder.h"
#include "i3CollideeCapsule.h"

class i3CollisionResult;

class I3_EXPORT_SCENE i3CollideeMesh : public i3Collidee
{
	I3_CLASS_DEFINE( i3CollideeMesh);
protected:
	i3Vector3Array *	m_pPos;
	i3Vector3Array *	m_pNormal;
	
	I3_COLLIDEE_TRI32 *	m_pTri;
	UINT32				m_TriCount;

	INT32				m_TerrainType;
	INT32				m_Trigger;

	i3List				m_ReflectMapList;

	// Volatile
	BOOL				m_bRefTri;

	VEC3D				m_Center;
	REAL32				m_Radius;

public:
	i3CollideeMesh(void);
	virtual ~i3CollideeMesh(void);

	INT32				GetTerrainType(void)			{ return m_TerrainType; }
	void				SetTerrainType( INT32 val)		{ m_TerrainType = val; }

	INT32				GetTrigger(void)				{ return m_Trigger; }
	void				SetTrigger( INT32 val)			{ m_Trigger = val; }

	i3Vector3Array *	GetPositionArray(void)			{ return m_pPos; }
	UINT32				GetPositionCount(void)			{ return m_pPos->GetVectorCount(); }
	VEC3D *				GetPosition( INT32 idx)			{ return m_pPos->GetVector( idx); }
	void				SetPositionCount( INT32 count);
	void				SetPositionArray( i3Vector3Array * pArray);

	i3Vector3Array *	GetNormalArray(void)			{ return m_pNormal; }
	UINT32				GetNormalCount(void)			{ return m_pNormal->GetVectorCount(); }
	VEC3D *				GetNormal( INT32 idx)			{ return m_pNormal->GetVector( idx); }
	void				SetNormalCount( INT32 count);
	void				SetNormalArray( i3Vector3Array * pArray);
	

	I3_COLLIDEE_TRI32 *	GetTriangleArray(void)			{ return m_pTri; }
	I3_COLLIDEE_TRI32 *	GetTriangle( INT32 idx)			{ return m_pTri + idx; }
	UINT32				GetTriangleCount(void)			{ return m_TriCount; }
	void				SetTriangleCount( INT32 count);
	void				SetTriangle( INT32 idx, INT32 vidx1, INT32 vidx2, INT32 vidx3, INT32 normidx)
	{
		m_pTri[idx].m_Index[0] = (UINT16) vidx1;
		m_pTri[idx].m_Index[1] = (UINT16) vidx2;
		m_pTri[idx].m_Index[2] = (UINT16) vidx3;
		m_pTri[idx].m_NormalIndex = (UINT16) normidx;
	}
	void				SetTriangle( INT32 idx, I3_COLLIDEE_TRI32 * pTri)
	{
		i3mem::Copy( &m_pTri[idx], pTri, sizeof( I3_COLLIDEE_TRI32));
	}

	virtual void	Transform( MATRIX * pMatrix, i3Collidee * pDest);

	INT32				getReflectMapCount(void)		{ return m_ReflectMapList.GetCount(); }
	i3TextureCube *		getReflectMap( INT32 idx)		{ return (i3TextureCube *) m_ReflectMapList.Items[idx]; }
	void				addReflectMap( i3TextureCube * pTex);
	void				removeAllReflectMap(void);

	VEC3D *		GetCenter(void)				{ return &m_Center; }
	REAL32		GetRadius(void)				{ return m_Radius; }

	UINT32		TestLine( i3CollideeLine * pCollidee, I3_COLLISION_RESULT_CONTEXT * pCtx);
	UINT32		TestSphere( i3CollideeSphere * pCollidee, I3_COLLISION_RESULT_CONTEXT * pCtx);
	UINT32		TestMesh( i3CollideeMesh * pCollidee, I3_COLLISION_RESULT * pResult, UINT32 ResultLimit, UINT32 Style);
	UINT32		TestCylinder( i3CollideeCylinder * pCollidee, I3_COLLISION_RESULT * pResult, UINT32 ResultLimit, UINT32 Style);
	UINT32		TestCapsule( i3CollideeCapsule * pCollidee, I3_COLLISION_RESULT_CONTEXT * pCtx);

	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
