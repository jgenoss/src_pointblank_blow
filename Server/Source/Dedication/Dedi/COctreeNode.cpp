#include "pch.h"
#include "COctreeNode.h"
#include "DediMath.h"

INT8 g_CurrentSubdivision =0;

COctreeNode::COctreeNode()
{
	i3Vector::Zero(&m_vCenter);
	m_r32Width=0.f;
	m_bSubDivided = false;
	m_pCollisionData = NULL;
	m_i32TriangleCount =0;

	m_ui16NodeIndex =0;

	for(INT32 i = 0 ;i< OCTREE_NODES_MAX ;i ++)
	{
		m_pChildNode[i] =NULL;
	}


	m_pCollisionData = NULL;
}


COctreeNode::~COctreeNode()
{
	if (m_pCollisionData)
	{
		for (INT32 i =0; i< m_pCollisionData->getMaxObjCount(); i++)
		{
			if(m_pCollisionData->getObjData(i)->pFaces)
			{
				delete [] m_pCollisionData->getObjData(i)->pFaces;
				m_pCollisionData->getObjData(i)->pFaces =NULL;
			}

			if(m_pCollisionData->getObjData(i)->pVerts)
			{
				delete [] m_pCollisionData->getObjData(i)->pVerts;
				m_pCollisionData->getObjData(i)->pVerts =NULL;
			}
		}
		delete m_pCollisionData;
		m_pCollisionData = NULL;
	}


	for (INT8 i =0; i < OCTREE_NODES_MAX ; i++)
	{
		if (m_pChildNode[i])
		{
			delete m_pChildNode[i];
			m_pChildNode[i] = NULL;
		}
	}
}

void COctreeNode::FindPartitionPoint(CCollisionData * pData)
{

	INT32 NumberofVerts =0;
	
	INT32 MaxObjectCnt= pData->getMaxObjCount();
	
	for (INT32 ObjIdx =0;ObjIdx < MaxObjectCnt ; ObjIdx++)
	{
		INT32 VertsCnt = GetObjData(ObjIdx)->getVertexCount();

		NumberofVerts += VertsCnt;
		
		for(INT32 VertsIdx =0; VertsIdx < VertsCnt; VertsIdx++)
		{
			i3Vector::Add(&m_vCenter,&m_vCenter,&GetObjData(ObjIdx)->pVerts[VertsIdx]);
			I3TRACE(" X : %3f  Y: %3f  Z: %3f   \n",GetObjData(ObjIdx)->pVerts[VertsIdx].x,GetObjData(ObjIdx)->pVerts[VertsIdx].y,GetObjData(ObjIdx)->pVerts[VertsIdx].z);
		}
	}
	
	I3TRACE(" ====================== \n");
	I3TRACE(" NumberofVerts : %1d \n",NumberofVerts);
	I3TRACE(" ====================== \n");

	m_vCenter.x /= NumberofVerts;
	m_vCenter.y /= NumberofVerts;
	m_vCenter.z /= NumberofVerts;

	REAL32 r32currentWidth =0;
	REAL32 r32currentHeight=0;
	REAL32 r32currentDepth =0;
	REAL32 r32maxWidth =0;
	REAL32 r32maxHeight=0;
	REAL32 r32maxDepth =0;


	for (INT32 ObjIdx =0;ObjIdx < MaxObjectCnt; ObjIdx++)
	{
		INT32 VertsCnt = GetObjData(ObjIdx)->getVertexCount();

		for (INT32 i =0; i< VertsCnt ; i++)
		{
			VEC3D vSub;

			i3Vector::Sub(&vSub, &m_vCenter,&GetObjData(ObjIdx)->pVerts[i]);

			r32currentWidth =  i3Math::abs(vSub.x);
			r32currentHeight = i3Math::abs(vSub.y);
			r32currentDepth =  i3Math::abs(vSub.z);


			if (r32currentWidth > r32maxWidth) r32maxWidth = r32currentWidth;

			if (r32currentHeight > r32maxHeight) r32maxHeight = r32currentHeight;

			if (r32currentDepth > r32maxDepth) r32maxDepth = r32currentDepth;

		}

	}


	r32maxWidth *= 2;		r32maxHeight *= 2;		r32maxDepth *= 2;

	if(r32maxWidth > r32maxHeight && r32maxWidth > r32maxDepth)
		m_r32Width = r32maxWidth;

	else if(r32maxHeight > r32maxWidth && r32maxHeight > r32maxDepth)
		m_r32Width = r32maxHeight;

	else
		m_r32Width = r32maxDepth;         //m_Width는 CreateNode()에서 사용이 된다. 


	I3TRACE("==============================================================\n");
	I3TRACE("r32maxWidth : %1f \n r32maxHeight : %1f \n r32maxDepth : %1f \n m_r32Width : %1f \n",r32maxWidth,r32maxHeight,r32maxDepth ,m_r32Width);
	I3TRACE("==============================================================\n");
	
}

void COctreeNode::CreateNode(CCollisionData * pData,INT32 i32TriangleCount, VEC3D * pCenter, REAL32 rWidth, OCTREE_NODES NODE_ID,INT32 NodeIndex)
{

	i3Vector::Copy(&m_vCenter, pCenter);

	m_r32Width = rWidth;
	m_i8Depth = g_CurrentSubdivision;
//	I3TRACE("Depth  %d \n", GetDepth());
	SetBoundBox();
	m_ui16NodeIndex =  NodeIndex +1;
//	I3TRACE("NODE IDX = %d \n",m_ui16NodeIndex);
	if( (g_CurrentSubdivision < MAX_SUBDIVIED) )
	{
		m_bSubDivided = true;
		ListNode NodeList[ OCTREE_NODES_MAX ];
		OctreeBoundBox BoundBox[ OCTREE_NODES_MAX ];

		for (INT8 i =0; i< OCTREE_NODES_MAX ; i++)
		{
			NodeList[i].pList.resize(pData->getMaxObjCount());
			VEC3D vNodeCenter;
			i3Vector::Copy(&vNodeCenter, pCenter);
			GetNewNodeCenter(&vNodeCenter,m_r32Width,(OCTREE_NODES)i);
		//	I3TRACE("node : %d \n",i);
			BoundBox[i].SetBoundBox(&vNodeCenter,m_r32Width *0.5f);
		
		}

		for(INT32 objCnt=0; objCnt< pData->getMaxObjCount(); objCnt++)
		{
			tObject *pObject = (pData->getObjData(objCnt));


			for (INT8 i =0; i< OCTREE_NODES_MAX ; i++)
			{
				NodeList[i].pList[objCnt].pFaceList.resize(pObject->getFaceCount());
				NodeList[i].pList[objCnt].totalFaceCount =0;
			}
		
	
			for (INT32 FaceCnt =0;FaceCnt < pObject->getFaceCount() ;FaceCnt++)
			{

				VEC3D tri[3];

				i3Vector::Copy(&tri[0],&pData->getObjData(objCnt)->pVerts[pData->getObjData(objCnt)->pFaces[FaceCnt].vertIndex[0]]);
				i3Vector::Copy(&tri[1],&pData->getObjData(objCnt)->pVerts[pData->getObjData(objCnt)->pFaces[FaceCnt].vertIndex[1]]);
				i3Vector::Copy(&tri[2],&pData->getObjData(objCnt)->pVerts[pData->getObjData(objCnt)->pFaces[FaceCnt].vertIndex[2]]);


			


				for(INT8 i = 0; i < OCTREE_NODES_MAX; i++ )
				{
					
					//테스트 임시 코드
					INT32 a[3];
					for (INT8 j =0; j< 3; j++)
					{
						a[j] = pData->getObjData(objCnt)->pFaces[FaceCnt].vertIndex[j];
					}
				
					if(ContainLineInNode(&tri[0],&tri[1],&tri[2],pCenter,&BoundBox[i],m_r32Width *0.5f,a,objCnt))
					{
					

						NodeList[i].pList[objCnt].pFaceList[FaceCnt] = true;
					}
				}


				/*

				for(INT32 whichVertex = 0; whichVertex < 3; whichVertex++)
				{
				INT32 FaceIndex = pData->getObjData(objCnt)->pFaces[FaceCnt].vertIndex[whichVertex];
				VEC3D vPos;
				if (!m_BoundBox.isInBound(&tri[0],&tri[1],&tri[2]))
				{
				continue;
				}
				i3Vector::Copy(&vPos,&pData->getObjData(objCnt)->pVerts[FaceIndex]);

				if( vPos.x < m_vCenter.x )	x = 0;	else x = 1;
				if( vPos.y < m_vCenter.y )	y = 0;	else y = 1;
				if( vPos.z < m_vCenter.z )	z = 0;	else z = 1;

				i3Child = x + (y<<1) + (z<<2);

				NodeList[i3Child].pList[objCnt].pFaceList[FaceCnt] = true;

				}
				*/

			}

		}



		INT32 triCount[ OCTREE_NODES_MAX ];

		for(INT8 i =0; i < OCTREE_NODES_MAX ; i++)
		{
			 triCount[i ] =0;
		}
	
		for(int objCnt = 0; objCnt <  pData->getMaxObjCount(); objCnt++)
		{
			for(int FaceCnt = 0; FaceCnt < pData->getObjData(objCnt)->getFaceCount(); FaceCnt++)
			{

				for (INT8 i =0; i< OCTREE_NODES_MAX ; i++)
				{
					if(NodeList[i].pList[objCnt].pFaceList[FaceCnt])
					{
						NodeList[i].pList[objCnt].totalFaceCount++;
						triCount[i]++; 
					}
				}
			}
		}



		for (INT8 i =0; i< OCTREE_NODES_MAX ; i++)
		{
			CreateNewNode(pData, NodeList[i].pList, triCount[i], (OCTREE_NODES)i,	pCenter, rWidth,m_ui16NodeIndex);
		}

	}
	else
	{
		AssignTrianglesToNode(pData, i32TriangleCount);
	}

}


bool COctreeNode::ContainLineInNode(VEC3D * pA,VEC3D * pB,VEC3D * pC, VEC3D *pCenter,OctreeBoundBox *pBox,REAL32 rWidth,INT32 a[3],INT32 b)
{
	//선분 교차 검사를 위해 미리 생성한 바운드 박스로 체크한다.
	
	VEC3D vPC;
	REAL32 t;
	
	if(IntersectAABBoxToLine(pA,pB,pBox->GetMin(),pBox->GetMax(),&vPC,&t))
		return true;
	
	if(IntersectAABBoxToLine(pB,pC,pBox->GetMin(),pBox->GetMax(),&vPC,&t))
		return true;
	
	if(IntersectAABBoxToLine(pC,pA,pBox->GetMin(),pBox->GetMax(),&vPC,&t))
		return true;
	
	
	//삼각형이 바운딩박스를 포함할 경우


	VEC3D vBoundTriangle[3],vTriangle[3],vCross,vDirAB,vDirBC,vPc;

	i3Vector::Copy(&vTriangle[0],pA);
	i3Vector::Copy(&vTriangle[1],pB);
	i3Vector::Copy(&vTriangle[2],pC);

	i3Vector::Sub(&vDirAB,pB,pA);
	i3Vector::Sub(&vDirBC,pC,pB);
	i3Vector::Cross(&vCross,&vDirAB,&vDirBC);
	i3Vector::Normalize(&vCross,&vCross);

	i3Vector::Copy(&vBoundTriangle[0],pBox->GetMin());
	i3Vector::Copy(&vBoundTriangle[1],pBox->GetMax());
	i3Vector::Set(&vBoundTriangle[2],pBox->GetMax()->x,pBox->GetMin()->y,pBox->GetMax()->z);



	if(IntersectTriToTri(vTriangle, &vCross,vBoundTriangle,&vPc))
	{
		if (pBox->isInside(&vPc))
		{
		//	I3TRACE("Index : %d %d %d %d \n",b,a[0],a[1],a[2]);
			return true;
		}
		
	}	
	return false;

}
void COctreeNode::CreateNewNode(CCollisionData *pWorld, vector<tFaceList> pList , INT32 triangleCount, OCTREE_NODES NODE_ID, VEC3D *pCenter, REAL32 r32Width,INT32 NodeIndex)
{
	if(!triangleCount)
		return;

	//노드에 필요한 정보만 카피한다.
	CCollisionData *pData = NULL;
	pData = new CCollisionData;
	CopyToData(pData,pWorld,pList);

	g_CurrentSubdivision++;

	m_pChildNode[NODE_ID] = new COctreeNode;

	//노드의 중심점을 구한다.
	VEC3D vNodeCenter;
	i3Vector::Copy(&vNodeCenter, pCenter);
	GetNewNodeCenter(&vNodeCenter,r32Width,NODE_ID);


	INT32 nNewNodeIndex = (NodeIndex *10) + NODE_ID;
	//노드 생성
	m_pChildNode[NODE_ID]->CreateNode(pData,triangleCount,&vNodeCenter,r32Width *0.5f,NODE_ID,nNewNodeIndex);
	g_CurrentSubdivision--;



	//CCollisionData 삭제
	for (INT32 i = 0; i< pWorld->getMaxObjCount(); i++ )
	{
		if (pData->getObjData(i)->pFaces)
		{
			delete []pData->getObjData(i)->pFaces;
			pData->getObjData(i)->pFaces = NULL;

		}

		if (pData->getObjData(i)->pVerts)
		{
		//	delete []pData->getObjData(i)->pVerts;

		}
	}
	delete pData;

}


VEC3D *COctreeNode::GetNewNodeCenter(VEC3D *pPos, REAL32 width, OCTREE_NODES NODE_ID)
{

	//새로운 노드의 중심점을 구한다.
	VEC3D vCtr;
	REAL32 rWidth = width /4;

	i3Vector::Zero(&vCtr);

	i3Vector::Copy(&vCtr,pPos);

//	i3Vector::Copy(&m_vCenter, &vCtr);

	switch(NODE_ID)							
	{
	case OCTREE_NODES_BOTTOM_LEFT_FRONT:
		i3Vector::Set(pPos,vCtr.x - rWidth, vCtr.y - rWidth, vCtr.z - rWidth);
		break;
	case OCTREE_NODES_BOTTOM_RIGHT_FRONT:
		i3Vector::Set(pPos,vCtr.x + rWidth, vCtr.y - rWidth, vCtr.z - rWidth);
		break;
	case OCTREE_NODES_TOP_LEFT_FRONT:
		i3Vector::Set(pPos,vCtr.x - rWidth, vCtr.y + rWidth, vCtr.z - rWidth);
		break;
	case OCTREE_NODES_TOP_RIGHT_FRONT:
		i3Vector::Set(pPos,vCtr.x + rWidth, vCtr.y + rWidth, vCtr.z - rWidth);
		break;
	case OCTREE_NODES_BOTTOM_LEFT_BACK:
		i3Vector::Set(pPos,vCtr.x - rWidth, vCtr.y - rWidth, vCtr.z + rWidth);
		break;
	case OCTREE_NODES_BOTTOM_RIGHT_BACK:
		i3Vector::Set(pPos,vCtr.x + rWidth, vCtr.y - rWidth, vCtr.z + rWidth);
		break;
	case OCTREE_NODES_TOP_LEFT_BACK:
		i3Vector::Set(pPos,vCtr.x - rWidth, vCtr.y + rWidth, vCtr.z + rWidth);
		break;
	case OCTREE_NODES_TOP_RIGHT_BACK:
		i3Vector::Set(pPos,vCtr.x + rWidth, vCtr.y + rWidth, vCtr.z + rWidth);
		break;
	}

	return pPos;
}


INT32 COctreeNode::GetSceneTriangleCount()
{

	INT32 numberOfTriangles = 0;

	for(int i = 0; i < m_pCollisionData->getMaxObjCount(); i++)
	{
		numberOfTriangles += m_pCollisionData->getObjData(i)->getFaceCount();
	}

	return numberOfTriangles;
}



void COctreeNode::AssignTrianglesToNode(CCollisionData *pWorld, INT32 i32TriangleCount)
{

	I3TRACE("i32TriangleCount  %d  \n",i32TriangleCount);

	m_bSubDivided = false;

	m_i32TriangleCount = i32TriangleCount;
	
	m_pCollisionData = new CCollisionData;

	memset(m_pCollisionData,0, sizeof(CCollisionData));

	m_pCollisionData->setMaxObjCount(pWorld->getMaxObjCount());
	INT32 FaceCnt =0;

	for (INT32 i =0; i< m_pCollisionData->getMaxObjCount();i++)
	{
	
		tObject *pObject = pWorld->getObjData(i);
	
		m_pCollisionData->AddObjectData();

		FaceCnt += pObject->getFaceCount();

		m_pCollisionData->getObjData(i)->numOfFaces = pObject->getFaceCount();

		m_pCollisionData->getObjData(i)->pFaces = new tFace [ pObject->getFaceCount()];
		
		// Copy the faces from the partition passed in to our end nodes face index list
		memcpy(m_pCollisionData->getObjData(i)->pFaces, pObject->pFaces, sizeof(tFace) *  pObject->getFaceCount());

	}


}

void	COctreeNode::SetBoundBox()
{
	VEC3D vMax,vMin;

	REAL32 width = m_r32Width *0.5f;

	i3Vector::Set(&vMax, m_vCenter.x + width,m_vCenter.y + width,m_vCenter.z + width);
	i3Vector::Set(&vMin, m_vCenter.x - width,m_vCenter.y - width,m_vCenter.z - width);

	m_BoundBox.SetMinMax(&vMin,&vMax);
	
	/*
	I3TRACE("i3Vector::Set(&vMax , %2ff, %2ff, %2ff);  \n  ",vMax.x,vMax.y,vMax.z);
	I3TRACE("i3Vector::Set(&vMin , %2ff, %2ff, %2ff);  \n  ",vMin.x,vMin.y,vMin.z);
	I3TRACE("g_pGameContext->DrawCollision2(DrawBox(&vMax,&vMin));\n");
	*/
	/*
	I3TRACE("Bound Box MAX: %2f %2f %2f  \n",vMax.x,vMax.y,vMax.z);
	I3TRACE("Bound Box MIN: %2f %2f %2f  \n",vMin.x,vMin.y,vMin.z);
	I3TRACE("Bound Box Width: %2f \n  ",width);
	I3TRACE("Bound Box Center: %2f %2f %2f  \n  ",m_BoundBox.getCenter()->x,m_BoundBox.getCenter()->y,m_BoundBox.getCenter()->z);
	*/
}


COctreeNode* COctreeNode::GetNode( VEC3D * pVPoint )
{
	INT8 x,y,z;
	INT8 i3Child;

	if( pVPoint->x < m_vCenter.x )	x = 0;	else x = 1;
	if( pVPoint->y < m_vCenter.y )	y = 0;	else y = 1;
	if( pVPoint->z < m_vCenter.z )	z = 0;	else z = 1;

	i3Child = x + (y<<1) + (z<<2);

	if( m_pChildNode[i3Child] == NULL )
	{
	//	I3TRACE("End Node \n",i3Child);
		return this;
	}
	else
	{
	//	I3TRACE("Search Node : %d \n",i3Child);
		return m_pChildNode[i3Child]->GetNode(pVPoint);
	}
}

bool COctreeNode::DoSphereCollision( VEC3D * pStart,VEC3D * pEnd, RaycastResult * pOut,CCollisionData *pObject )
{
	/*

	if (m_pCollisionData == NULL)
		return false;


	VEC3D vNomal,pc;

	INT32 nColCnt =0;
	INT32 MaxObjectCnt =  m_pCollisionData->getMaxObjCount();

	tObject *pWorldNode = pObject->getObjData(3);
	tObject *pLocalNode = m_pCollisionData->getObjData(0);
	INT32 MaxFace =  pLocalNode->getFaceCount();
	REAL32 t;
	
	
	
	for (INT32 i =0; i< MaxObjectCnt;i++)
	{
		tObject *pWorldNode = pObject->getObjData(i);
		tObject *pLocalNode = m_pCollisionData->getObjData(i);
		INT32 MaxFace =  pLocalNode->getFaceCount();

		for (INT32 j =0; j<MaxFace; j++)
		{

			REAL32 t;
			VEC3D v1,v2;
			VEC3D tri1,tri2,tri3;
			i3Vector::Copy(&vNomal,&pLocalNode->pFaces[j].vNomal);
			

			/*
			//if(SphereCollision(&tri1,&tri2,&tri3,&vNomal,pStart,0.2f,&pc,&t))
			if(SphereCollision( &pWorldNode->pVerts[pLocalNode->pFaces[j].vertIndex[0]],
								&pWorldNode->pVerts[pLocalNode->pFaces[j].vertIndex[1]],
								&pWorldNode->pVerts[pLocalNode->pFaces[j].vertIndex[2]],
								&vNomal,
								pStart,0.2f,&pc,&t))
			{


				//I3TRACE("Object : %d , %d,%d,%d \n", i,m_pCollisionData->getObjData(i)->pFaces[j].vertIndex[0],m_pCollisionData->getObjData(i)->pFaces[j].vertIndex[1],m_pCollisionData->getObjData(i)->pFaces[j].vertIndex[2] );
				//I3TRACE("Tri 1 :%2f %2f %2f  Index : %d \n",tri1.x,tri1.y,tri1.z,m_pCollisionData->getObjData(i)->pFaces[j].vertIndex[0]);
				//I3TRACE("Tri 2 :%2f %2f %2f Index : %d \n",tri2.x,tri2.y,tri2.z,m_pCollisionData->getObjData(i)->pFaces[j].vertIndex[1]);
				//I3TRACE("Tri 3 :%2f %2f %2f Index : %d \n",tri3.x,tri3.y,tri3.z,m_pCollisionData->getObjData(i)->pFaces[j].vertIndex[2]);
				//I3TRACE("InterSect : %2f %2f %2f \n", pc.x,pc.y,pc.z);
				//I3TRACE("Distance : %2f \n", &t);

				pOut->Reset();
				pOut->SetResult(&pc,&pLocalNode->pFaces[j].vNomal);

				return true;
			}
			
			
		}


	}
	
	
*/
	return false;

}
bool COctreeNode::DoTestRaycast(VEC3D * pStart,VEC3D * pEnd, RaycastResult * pOut,CCollisionData *pObject)
{

	if (m_pCollisionData == NULL)
		return false;

	VEC3D vNomal,pc;
	
	tObject *pWorldNode = pObject->getObjData(5);
	//tObject *pLocalNode = m_pCollisionData->getObjData(0);
	//INT32 MaxFace =  pLocalNode->getFaceCount();
	
	for (INT32 i =0; i< 60;i++)
	{

		REAL32 t;
		//VEC3D v1,v2;
		VEC3D tri1,tri2,tri3;

		i3Vector::Copy(&tri1,&pWorldNode->pVerts[pWorldNode->pFaces[0].vertIndex[0]] );
		i3Vector::Copy(&tri2,&pWorldNode->pVerts[pWorldNode->pFaces[0].vertIndex[1]] );
		i3Vector::Copy(&tri3,&pWorldNode->pVerts[pWorldNode->pFaces[0].vertIndex[2]] );
		i3Vector::Copy(&vNomal,&pWorldNode->pFaces[0].vNomal);

			if(IntersectsTriangle(&tri1,&tri2,&tri3,pStart,pEnd,&pc,&t))
			{
				pOut->Reset();
				pOut->SetResult(&pc,&vNomal,t);
	//			return true;
			}

	}
	return true;
}

bool COctreeNode::DoRaycast(VEC3D * pStart,VEC3D * pEnd, RaycastResult * pOut,CCollisionData *pObject)
{

	if (m_pCollisionData == NULL)
		return false;

	VEC3D vNomal,pc;
	

	INT32 MaxObjectCnt = m_pCollisionData->getMaxObjCount();
	
	vector<RaycastResult*>vResult;

	INT32 nCnt =0;
//	I3TRACE("==============절취선===========\n");
//	I3TRACE("Start %2f %2f %2f \n",pStart->x,pStart->y,pStart->z);
//	I3TRACE("End %2f %2f %2f \n",pEnd->x,pEnd->y,pEnd->z);
	for (INT32 i =0; i<MaxObjectCnt ;i++)
	{

		tObject *pWorldNode = pObject->getObjData(i);
		tObject *pLocalNode = m_pCollisionData->getObjData(i);
		INT32 MaxFace =  pLocalNode->getFaceCount();


		for (INT32 j =0; j< MaxFace ; j++)
		{

			REAL32 t;
			VEC3D tri1,tri2,tri3;
			
			i3Vector::Copy(&tri1,&pWorldNode->pVerts[pLocalNode->pFaces[j].vertIndex[0]] );
			i3Vector::Copy(&tri2,&pWorldNode->pVerts[pLocalNode->pFaces[j].vertIndex[1]] );
			i3Vector::Copy(&tri3,&pWorldNode->pVerts[pLocalNode->pFaces[j].vertIndex[2]] );
			i3Vector::Copy(&vNomal,&pLocalNode->pFaces[j].vNomal );

		//	I3TRACE("ObjId : %d  %d %d %d \n", i, pLocalNode->pFaces[j].vertIndex[0],pLocalNode->pFaces[j].vertIndex[1],pLocalNode->pFaces[j].vertIndex[2]);


		//	if(TestTriPoint(&vNomal,pStart,pEnd,&tri1 ))
			{
				if(IntersectsTriangle(&tri1,&tri2,&tri3,pStart,pEnd,&pc,&t))
				{
					if (nCnt == 0)
					{
						pOut->Reset();
						pOut->SetResult(&pc,&vNomal,t);
					}
					else
					{
						if (pOut->getScale() > t)
						{	
						//	I3TRACE("Scale : %f t : %f \n",pOut->getScale(), t);
							pOut->SetResult(&pc,&vNomal,t);
						}
					}
					nCnt++;
				}

			}
		}
	
	}
		
	if(nCnt>0)
	{
		I3TRACE("충돌 Size : %d \n", nCnt);
		return true;
	}

	

	return false;

}

void	COctreeNode::CopyToData(CCollisionData * pOutData, CCollisionData *pInData, vector<tFaceList> pList)
{
	for (INT32 objCnt =0; objCnt < pInData->getMaxObjCount(); objCnt ++)
	{	
		INT32 FaceCount = pInData->getObjData(objCnt)->getFaceCount();

		pOutData->AddObjectData();

		pOutData->getObjData(objCnt)->numOfFaces	= pList[objCnt].totalFaceCount;

		pOutData->getObjData(objCnt)->pVerts		=  pInData->getObjData(objCnt)->pVerts;

		if( pOutData->getObjData(objCnt)->getFaceCount() )
		{
			pOutData->getObjData(objCnt)->pFaces	= new tFace[pOutData->getObjData(objCnt)->getFaceCount()];
		}

		pOutData->setMaxObjCount(pInData->getMaxObjCount());

		INT32 i32Index = 0;

		for (INT32 FaceIndex =0; FaceIndex < FaceCount ; FaceIndex++)
		{
			if(pList[objCnt].pFaceList[FaceIndex])
			{
				pOutData->getObjData(objCnt)->pFaces[i32Index] = pInData->getObjData(objCnt)->pFaces[FaceIndex];
				i32Index++;;
			}
		}
	}
}

