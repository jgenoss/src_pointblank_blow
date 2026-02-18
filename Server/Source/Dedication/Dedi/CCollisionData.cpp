#include "pch.h"
#include "COctreeNode.h"
#include "CCollisionData.h"

CCollisionData::CCollisionData()
{

	for (INT32 i =0; i< DS_MAX_ACTORS; i++)
	{
		m_ppCollisionObjects[i] = NULL;

	}
}

CCollisionData::~CCollisionData()
{
	for (INT32 i =0; i< DS_MAX_ACTORS; i++)
	{
		I3_SAFE_DELETE( m_ppCollisionObjects[ i ] );
	}
}

void CCollisionData::CreateNxShapeSet(INT32 Idx)
{
	m_ppCollisionObjects[Idx] = new NxShapeSet;
}


void CCollisionData::ReleaseNxShapeSet()
{
	for (INT32 i =0; i< m_i32MaxObjectCnt; i++)
	{
			delete m_ppCollisionObjects[i];
	}
}

void CCollisionData::CovertCollisionData(INT32 Idx)
{
	tObject tempObject;

	memset(&tempObject, 0, sizeof(tObject));

	m_pObject.push_back(tempObject);

	m_pObject[Idx].numOfVerts = m_ppCollisionObjects[Idx]->getVertexCount();
	m_pObject[Idx].numOfFaces = m_ppCollisionObjects[Idx]->getFaceCount();


	m_pObject[Idx].pVerts = new VEC3D [m_pObject[Idx].numOfVerts];
	//memset(m_pObject[Idx]->pVerts, 0, sizeof(VEC3D)  * m_pObject[Idx]->getVertexCount() );

	m_pObject[Idx].pFaces = new tFace [m_pObject[Idx].numOfFaces];
	//memset(m_pObject[Idx]->pFaces, 0, sizeof(VEC3D)  * m_pObject->pFaces );

	m_matWorld	= m_ppCollisionObjects[0]->getTransform();


	MATRIX localTm;

	REAL32 m[16];
	m_matWorld.getColumnMajor44(m);

	
	
	localTm.m00 = m[0];
	localTm.m01 = m[1];
	localTm.m02 = m[2];
	localTm.m03 = m[3];
	localTm.m10 = m[4];
	localTm.m11 = m[5];
	localTm.m12 = m[6];
	localTm.m13 = m[7];
	localTm.m20 = m[8];
	localTm.m21 =m[9];
	localTm.m22 =m[10];
	localTm.m23 =m[11];
	localTm.m30 =0;
	localTm.m31 =0;
	localTm.m32 =0;
	localTm.m33 =1;




//	I3TRACE("Object Index : %d VertsCnt : %d FacesCnt: %d \n",Idx, m_ppCollisionObjects[Idx]->getVertexCount(),  m_ppCollisionObjects[Idx]->getFaceCount());

	for (INT32 i=0; i< m_pObject[Idx].numOfVerts; i++)
	{

		VEC3D vTemp;

		i3Vector::Set(&vTemp, m_ppCollisionObjects[Idx]->m_pvVertList[3* i]	+   m_ppCollisionObjects[Idx]->getTransform().t.x
							,m_ppCollisionObjects[Idx]->m_pvVertList[3* i +1] + m_ppCollisionObjects[Idx]->getTransform().t.y,
							m_ppCollisionObjects[Idx]->m_pvVertList[3* i +2] + m_ppCollisionObjects[Idx]->getTransform().t.z);


		
		
		TransformCoord( &m_pObject[Idx].pVerts[i], &vTemp, &localTm);
		
	
	//	I3TRACE("  %d   %2f  ,  %2f  ,%2f \n", i,m_pObject[Idx].pVerts[i].x, m_pObject[Idx].pVerts[i].y,m_pObject[Idx].pVerts[i].z);
//
	//	I3TRACE(" i3Vector::Set(&tri[%d],%2f,%2f,%2f); \n", i,m_pObject[Idx].pVerts[i].x, m_pObject[Idx].pVerts[i].y,m_pObject[Idx].pVerts[i].z);
		I3TRACE(" i3Vector::Set(&m_tMapData.vTri[%d][%d],%2ff,%2ff,%2ff); \n",Idx, i,m_pObject[Idx].pVerts[i].x, m_pObject[Idx].pVerts[i].y,m_pObject[Idx].pVerts[i].z);

	}

	for (INT32 i=0; i< m_pObject[Idx].numOfFaces; i++)
	{
		m_pObject[Idx].pFaces[i].vertIndex[0] = m_ppCollisionObjects[Idx]->m_pI3FaceList[i].m_Index[0];
		m_pObject[Idx].pFaces[i].vertIndex[1] =	m_ppCollisionObjects[Idx]->m_pI3FaceList[i].m_Index[1];
		m_pObject[Idx].pFaces[i].vertIndex[2] =	m_ppCollisionObjects[Idx]->m_pI3FaceList[i].m_Index[2];

		//I3TRACE("A : %2d  B : %2d  C : %2d \n", m_pObject[Idx].pFaces[i].vertIndex[0],m_pObject[Idx].pFaces[i].vertIndex[1],m_pObject[Idx].pFaces[i].vertIndex[2]);

	//	I3TRACE("g_pGameContext->DrawCollision(&tri[%2d],&tri[%2d],&tri[%2d]); \n", m_pObject[Idx].pFaces[i].vertIndex[0],m_pObject[Idx].pFaces[i].vertIndex[1],m_pObject[Idx].pFaces[i].vertIndex[2]);


		
	
		VEC3D v1,v2;
		VEC3D vNomal, tri1,tri2,tri3;

		i3Vector::Copy(&tri1,&m_pObject[Idx].pVerts[m_pObject[Idx].pFaces[i].vertIndex[0]] );
		i3Vector::Copy(&tri2,&m_pObject[Idx].pVerts[m_pObject[Idx].pFaces[i].vertIndex[1]] );
		i3Vector::Copy(&tri3,&m_pObject[Idx].pVerts[m_pObject[Idx].pFaces[i].vertIndex[2]] );


		i3Vector::Sub(&v1,&tri2,&tri1);
		i3Vector::Sub(&v2,&tri3,&tri2);

		i3Vector::Cross(&vNomal, &v1, &v2);
		i3Vector::Normalize(&vNomal,&vNomal);

		i3Vector::Copy(&m_pObject[Idx].pFaces[i].vNomal,&vNomal);
	}
	


	//m_pObject[Idx] = m_ppCollisionObjects[Idx]
}


void CCollisionData::AddObjectData()
{
	tObject tempObject;

	memset(&tempObject, 0, sizeof(tObject));

	m_pObject.push_back(tempObject);

}

void CCollisionData::TransformCoord( VEC3D *pOut, const VEC3D *pV, const MATRIX *pM )
{
	
	REAL32 x,y,z,w,rw;

	x = (pM->m[0][0] * pV->vec[0]) + (pM->m[1][0] * pV->vec[1]) + (pM->m[2][0] * pV->vec[2]) + pM->m[3][0];
	y = (pM->m[0][1] * pV->vec[0]) + (pM->m[1][1] * pV->vec[1]) + (pM->m[2][1] * pV->vec[2]) + pM->m[3][1];
	z = (pM->m[0][2] * pV->vec[0]) + (pM->m[1][2] * pV->vec[1]) + (pM->m[2][2] * pV->vec[2]) + pM->m[3][2];
	w = (pM->m[0][3] * pV->vec[0]) + (pM->m[1][3] * pV->vec[1]) + (pM->m[2][3] * pV->vec[2]) + pM->m[3][3];
	rw = 1.0f / w;
	pOut->vec[0] = x * rw;
	pOut->vec[1] = y * rw;
	pOut->vec[2] = z * rw;

}

