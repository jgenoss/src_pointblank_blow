#pragma once

#include "d3dx9.h"
struct tFaceList
{
	vector<bool> pFaceList;	

	int totalFaceCount;
};


struct tFace
{
	int vertIndex[3];			// indicies for the verts that make up this triangle
	VEC3D vNomal;				// indicies for the tex coords to texture this face
};	



struct tObject
{
	INT32  numOfVerts;			// The number of verts in the model
	INT32  numOfFaces;			// The number of faces in the model

	VEC3D  *pVerts;				// The object's verticescoordinates
	tFace  *pFaces;				// The faces information of the object


	INT32 getVertexCount()		{return numOfVerts;}
	INT32 getFaceCount()		{return numOfFaces;}
};


class CCollisionData{

private:

	NxShapeSet*					m_ppCollisionObjects[DS_MAX_ACTORS];		// physX objects 

	INT32						m_nNumberofObject;

	vector<tObject>				m_pObject;

	INT32						m_i32MaxObjectCnt;

	NxMat34						m_matWorld;		

	

public:
	
	CCollisionData();
	~CCollisionData();

	void		CreateNxShapeSet(INT32 Idx);

	void		ReleaseNxShapeSet();

	void		CovertCollisionData(INT32 Idx);

	void		AddObjectData();
	
	NxShapeSet* getNxShapeData(INT32 Idx)		{return m_ppCollisionObjects[Idx]; }

	tObject*	getObjData(INT32 Idx)			{return &m_pObject[Idx]; }

	void		setMaxObjCount(INT32 objCnt)	{m_i32MaxObjectCnt = objCnt;}
	
	INT32		getMaxObjCount()				{return m_i32MaxObjectCnt;}



	void TransformCoord( VEC3D *pOut, const VEC3D *pV, const MATRIX *pM );
	

};
