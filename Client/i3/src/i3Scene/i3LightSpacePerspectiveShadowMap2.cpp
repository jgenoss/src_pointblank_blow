#include "i3SceneDef.h"
#include "i3ShadowMap.h"
#include "i3SceneTracer.h"
#include "i3SceneUtil.h"

//#define SCREENSPACE_SHADOW

// the cuboid in the post-perspective space goes from [-1,-1,-1] to [1,1,1] in OpenGL,
// but from [-1,-1,0] to [1,1,1] in DirectX. this method compensates this.
void ToDxProjection( MATRIX * matProjection)
{
	MATRIX pivot;
	MATRIX scale;

	i3Matrix::SetTranslate( &pivot, 0.0f, 0.0f, 0.45f);
	i3Matrix::SetScale( &scale, 1.0f, 1.0f, 0.45f);
	i3Matrix::Mul( matProjection, matProjection, &scale);
	i3Matrix::Mul( matProjection, matProjection, &pivot);
}

void SwitchZ( MATRIX * pMtx)
{
	MATRIX tmp;

	i3Matrix::SetScale( &tmp, 1.0f, 1.0f, -1.0f);
	i3Matrix::Mul( pMtx, pMtx, &tmp);
}

inline void calcFrustumWorldCoordGL( VEC3D * pPts, MATRIX * pMtx) 
{
	REAL32 x1, y1, z1, x2, y2, z2;

	x1 = -1.0f;		y1	= -1.0f;	z1 = 0.0f;
	x2 =  1.0f;		y2	=  1.0f;	z2 = 1.0f;

	i3Vector::Set( pPts + 0,	x1, y1, z2);			// near-bottom-left
	i3Vector::Set( pPts + 1,	x2, y1, z2);			// near-bottom-right
	i3Vector::Set( pPts + 2,	x2, y2, z2);			// near-top-right
	i3Vector::Set( pPts + 3,	x1, y2, z2);			// near-top-left
	i3Vector::Set( pPts + 4,	x1, y1, z1);			// far-bottom-left
	i3Vector::Set( pPts + 5,	x2, y1, z1);			// far-bottom-right
	i3Vector::Set( pPts + 6,	x2, y2, z1);			// far-top-right
	i3Vector::Set( pPts + 7,	x1, y2, z1);			// far-top-left

	i3Matrix::TransformPoints( pPts, pPts, 8, pMtx);	//camera to world frame
}

void calcFrustObject( i3PolygonList * pList, VEC3D * pPts) 
{
	INT32 i;
	i3Vector3List * pPoly;

	pList->setCount( 6);

	for( i = 0; i < 6; i++)
	{
		pPoly = (i3Vector3List *) pList->getPolygon( i);
		
		pPoly->setCount( 4);
	}

	//near poly ccw
	pPoly = pList->getPolygon( 1);
	for( i = 0; i < 4; i++) 
	{
		i3Vector::Copy( pPoly->GetVector(i), & pPts[i]);
	}

	//far poly ccw
	pPoly = pList->getPolygon( 0);
	for( i = 4; i < 8; i++)
	{
		i3Vector::Copy( pPoly->GetVector(i - 4), &pPts[ 11-i]);
		//obj.polygon[1].points[i-4] = p[11-i];
	}

	//left poly ccw
	pPoly = pList->getPolygon( 2);
	i3Vector::Copy( pPoly->GetVector( 0), &pPts[0]);
	i3Vector::Copy( pPoly->GetVector( 1), &pPts[3]);
	i3Vector::Copy( pPoly->GetVector( 2), &pPts[7]);
	i3Vector::Copy( pPoly->GetVector( 3), &pPts[4]);

	//right poly ccw
	pPoly = pList->getPolygon( 3);
	i3Vector::Copy( pPoly->GetVector( 0), &pPts[1]);
	i3Vector::Copy( pPoly->GetVector( 1), &pPts[5]);
	i3Vector::Copy( pPoly->GetVector( 2), &pPts[6]);
	i3Vector::Copy( pPoly->GetVector( 3), &pPts[2]);

	//bottom poly ccw
	pPoly = pList->getPolygon( 4);
	i3Vector::Copy( pPoly->GetVector( 0), &pPts[4]);
	i3Vector::Copy( pPoly->GetVector( 1), &pPts[5]);
	i3Vector::Copy( pPoly->GetVector( 2), &pPts[1]);
	i3Vector::Copy( pPoly->GetVector( 3), &pPts[0]);

	//top poly ccw
	pPoly = pList->getPolygon( 5);
	i3Vector::Copy( pPoly->GetVector( 0), &pPts[6]);
	i3Vector::Copy( pPoly->GetVector( 1), &pPts[7]);
	i3Vector::Copy( pPoly->GetVector( 2), &pPts[3]);
	i3Vector::Copy( pPoly->GetVector( 3), &pPts[2]);
}

void calcFrustObjectGL( i3PolygonList * pVecList, MATRIX * pMtx) 
{
	VEC3D pts[8];

	// World-Space에서의 View-Frustum Volume을 생성한다.
	calcFrustumWorldCoordGL( pts, pMtx);

	// View-Frustum의 6개 면에 대해 Polygon 정보를 구성하여 pVecList에 추가한다.
	calcFrustObject( pVecList, pts);
}

bool intersectPlaneEdge( VEC3D * output, VEC4D * pPlane, VEC3D * pStart, VEC3D * pEnd)
{
	REAL32 t;

	return i3Plane::IntersectWithEdge( pPlane, pStart, pEnd, &t, output);
}

//the polygon is clipped to the part behind the plane (positive half-space is clip-space)
void clipPolygonByPlane( i3Vector3List * poly, VEC4D * A, i3Vector3List * polyOut, i3Vector3List * interPts) 
{
	INT32 i;
	VEC3D inter;

	if( poly->getCount() < 3) 
		return;

	bool outside[64];
	I3ASSERT( poly->getCount() < 64);

	//for each point mark the points in front of the plane
	//the points that will be clipped away
	for( i = 0; i < poly->getCount(); i++) 
	{
		outside[i] = i3Plane::isBehindPoint( A, poly->GetVector( i));
	}

	for( i = 0; i < poly->getCount(); i++) 
	{
		//id of the second point of the edge
		const unsigned idNext = (i+1) % poly->getCount();

		//both outside -> save none
		if( outside[i] && outside[idNext])
			continue;
		
		//outside to inside -> calc intersection save intersection and save i+1
		if(outside[i]) 
		{
			if( intersectPlaneEdge( &inter, A, poly->GetVector(i), poly->GetVector(idNext))) 
			{
				polyOut->Add( &inter);
				interPts->Add( &inter);
			}

			polyOut->Add( poly->GetVector( idNext));
			continue;
		}

		//inside to outside -> calc intersection save intersection
		if( outside[idNext]) 
		{
			if( intersectPlaneEdge( &inter, A, poly->GetVector(i), poly->GetVector(idNext)))
			{
				polyOut->Add( &inter);
				interPts->Add( &inter);
			}
			continue;
		}

		//both inside -> save point i+1
		polyOut->Add( poly->GetVector( idNext));
	}
}

int findSamePointInVecPoint( i3Vector3List * poly, VEC3D * p) 
{
	for( INT32 i = 0; i < poly->getCount(); i++) 
	{
		if( i3Vector::isAlike( poly->GetVector(i), p, 0.001f)) 
		{
			return i;
		}
	}

	return -1;
}

int findSamePointInObjectAndSwapWithLast( i3PolygonList * inter, VEC3D * p) 
{
	if( 1 > inter->getCount()) 
	{
		return -1;
	}

	for( unsigned i = inter->getCount(); i > 0; i--) 
	{
		i3Vector3List * pPoly = inter->getPolygon( i - 1);

		if(2 == pPoly->getCount()) 
		{
			const int nr = findSamePointInVecPoint( pPoly, p);

			if(0 <= nr) 
			{
				//swap with last
				i3Vector3List * pLast = inter->getPolygon( inter->getCount() - 1);

				I3_MUST_ADDREF(pLast);
				I3_MUST_ADDREF(pPoly);

				inter->setPolygon( i - 1, pLast);
				inter->setPolygon( inter->getCount() - 1, pPoly);

				I3_MUST_RELEASE(pLast);
				I3_MUST_RELEASE(pPoly);
				return nr;
			}
		}
	}
	return -1;
}

void makeClosingPolygon( i3Vector3List * polyOut, i3PolygonList * intersectionEdges, VEC3D * n) 
{
	INT32 i;

	//todo: compact inter: remove all polys with size != 2
	for( i = intersectionEdges->getCount(); 0 < i; i--) 
	{
		if( 2 == intersectionEdges->getPolygon( i - 1)->getCount()) 
		{
			break;
		}
	}

	intersectionEdges->setCount( i);

	//you need at least 3 sides for a polygon
	if( 3 > intersectionEdges->getCount()) 
	{
		return;
	}

	//we have line segments in each poly of inter 
	//take last linesegment as first and second point
	i3Vector3List * polyIn = intersectionEdges->getPolygon( intersectionEdges->getCount() - 1);

	polyOut->Add( polyIn->GetVector( 0));
	polyOut->Add( polyIn->GetVector( 1));

	//remove last poly from inter, because it is already saved
	intersectionEdges->removeEnd();

	//search edges that can be stitched together (have a point in common)
	//iterate over the intersectionEdges until their is no line segment left
	while( 0 < intersectionEdges->getCount()) 
	{
		//pointer on last point to compare
		VEC3D * lastPt = polyOut->GetVector( polyOut->getCount() - 1);

		//find same point in inter to continue polygon
		const int nr = findSamePointInObjectAndSwapWithLast(intersectionEdges, lastPt);

		if(0 <= nr) 
		{
			//last edge
			// const VecPoint& polyIn = intersectionEdges.polygon.back().points;
			i3Vector3List * pLast = intersectionEdges->getLastPolygon();

			//get the other point in this polygon and save into polyOut
			polyOut->Add( pLast->GetVector( (nr+1) % 2));
			//polyOut.push_back(polyIn[(nr+1)%2]);
		}

		//remove last poly from inter, because it is already saved or degenerated
		intersectionEdges->removeEnd();
	}

	//last point can be deleted, because he is the same as the first (closes polygon)
	polyOut->setCount( polyOut->getCount() - 1);

	//check for correct winding
	//Plane planePoly(polyOut[0],polyOut[1],polyOut[2]);
	VEC4D planePoly;

	i3Plane::Set( &planePoly, polyOut->GetVector(0), polyOut->GetVector( 1), polyOut->GetVector( 2));

	if( ! i3Vector::isAlike( n, (VEC3D *) &planePoly, 0.1f))
	{
		//normal of polygon points in different direction than normal of clipping plane
		//-> reverse winding
		polyOut->Reverse( 0, polyOut->getCount() - 1);
	}
}


void i3ShadowMap::clipObjectByPositiveHalfSpaceOfPlane( i3PolygonList * obj, VEC4D * A, i3PolygonList * objOut) 
{
	INT32 i;

	if( obj->getCount() == 0)
		return;

	// [50]  - call:00006,  elapsed:0.519898 ms (min:0.000000, max:0.219022, avg:0.101968)
	obj->CopyTo( &objIn, I3_COPY_INSTANCE);

	// [51]  - call:00006,  elapsed:0.009219 ms (min:0.000000, max:0.015644, avg:0.001397)
	objOut->reset();	

	intersectionEdges.reset();

	for( i = 0; i < objIn.getCount(); i++) 
	{
		const unsigned size = objOut->getCount();

		// [52]  - call:00047,  elapsed:0.285511 ms (min:0.000000, max:0.093867, avg:0.005308)
		objOut->setCount( size + 1);

		intersectionEdges.setCount( size + 1);

		// [53]  - call:00047,  elapsed:0.180470 ms (min:0.000000, max:0.051403, avg:0.003352)
		clipPolygonByPlane( objIn.getPolygon(i), A, objOut->getPolygon( size), intersectionEdges.getPolygon( size));

		//if whole poly was clipped away -> delete empty poly
		if( 0 == objOut->getPolygon(size)->getCount()) 
		{
			// [54]  - call:00002,  elapsed:0.002514 ms (min:0.000000, max:0.011454, avg:0.001117)
			objOut->setCount( size);
			intersectionEdges.setCount( size);
		}
	}

	if( intersectionEdges.getCount() > 0) 
	{ 
		// [55]  - call:00006,  elapsed:0.023467 ms (min:0.000000, max:0.018159, avg:0.003911)
		//not everything was clipped away
		//make place for one additional polygon in objOut to close it

		objOut->setCount( objOut->getCount() + 1);

		//add a polygon of all intersection edges with plane to close the object
		makeClosingPolygon( objOut->getLastPolygon(), &intersectionEdges, (VEC3D *) &A);
	}
}

void i3ShadowMap::clipObjectByAABox( i3PolygonList * pPolyList, i3BoundBox * pBox) 
{
	VEC4D planes[6];

	// World-Space planes 
	pBox->GetPlanes( planes);

	for( INT32 i = 0; i < 6; i++) 
	{
		clipObjectByPositiveHalfSpaceOfPlane( pPolyList, &planes[i], pPolyList);
	}
}

void convObject2VecPoint( i3Vector3List * points, i3PolygonList * obj) 
{
	INT32 i, j;

	points->Reset();

	for( i = 0; i < obj->getCount(); i++) 
	{
		i3Vector3List * p = obj->getPolygon( i);

		for( j = 0; j < p->getCount(); j++) 
		{
			points->Add( p->GetVector( j));
		}
	}
}

void includeObjectLightVolume( i3Vector3List * points, i3PolygonList * obj, VEC3D * lightDir, i3BoundBox * sceneAABox) 
{
	INT32 i;
	VEC3D ld;

	i3Vector::Scale( &ld, lightDir, -1.0f);

	convObject2VecPoint( points, obj);

	INT32 size = points->getCount();

	//i3VLog::SetColor( (UINT8) 64, 64, 64, 255);
	//i3VLog::Draw( sceneAABox);

	//i3VLog::SetColor( (UINT8) 64, 64, 128, 255);
	//i3VLog::Draw( obj);

	//for each point add the point on the ray in -lightDir
	//intersected with the sceneAABox
	for( i = 0; i < size; i++) 
	{
		VEC3D pt;

		//i3VLog::SetColor( (UINT8) 0, 255, 0, 255);

		//i3Vector::Scale( &e, &ld, 500.0f);
		//i3Vector::Add( &e, &e, points->GetVector( i));
		//i3VLog::Line( points->GetVector( i), &e);

		if( sceneAABox->IntersectWithLine( points->GetVector(i), &ld, &pt))
		{
			//i3VLog::SetColor( (UINT8) 255, 0, 0, 255);
			//i3VLog::Point( &pt);

			points->Add( &pt);
		}
	}

	//i3VLog::SetColor( (UINT8) 100, 100, 100, 255);
	//i3VLog::Draw( sceneAABox);

	//i3VLog::SetColor( (UINT8) 0, 255, 0, 255);
	//i3VLog::Draw( points);
}

void i3ShadowMap::_CalcB( i3SceneTracer * pTracer)
{
	m_BodyPList.reset();

	// View-Frustum에 대한 Volume을 구성한다.
	// View-Frustum의 각 꼭지점을 m_BodyPList에 추가한다.
	{
		MATRIX tmp;

		i3Matrix::Inverse( &tmp, nullptr, pTracer->getViewProjectionMatrix());

		calcFrustObjectGL( &m_BodyPList, &tmp);
	}

	// 구성된 View-Frustum Volume의 6면 Plane으로 ReceiverBound를 자른다.
	// 즉, View-Frustum 안에 들어오는 Convex Vertex들을 구성한다.
	clipObjectByAABox( &m_BodyPList, &m_ReceiverBound);

	m_LVS.Reset();
	calcPoints( &m_LVS, &m_BodyPList);			// m_BdoyPList -> m_LVS 단순 변환

	// m_LVS의 Point 중에서 World-Space에서 카메라에 가장 가까운 점을 찾는다.
	getNearCameraPointE( &m_NearestP_WS, pTracer);

	//if (Settings::GetInstance()->IsDirectionalLightUsed()) 
	{
		VEC3D lightDir;

		i3Vector::Normalize( &lightDir, i3Matrix::GetAt( &m_mLightTx));
		i3Vector::Scale( &lightDir, &lightDir, -1.0f);

		//extrudes the clipped view frustum into -lightDir and clippes by the sceneAABox
		//the defining points are returned

		includeObjectLightVolume( &m_B, &m_BodyPList, &lightDir, &m_ReceiverBound);
	}
	/*
	else {
		//done: calculate the convex hull of the clipped view frustum and the light position
		convexHullOfObjectAndPoint(temp,frameState.getSpotLightPos());
		//done: intersect with the scene bb
		clipObjectByAABox(temp,frameState.sceneAABox);
		//calculate the light frustum matrices
		M4 lv, lp;
		calcStandardSpotShadowMtx(lv,lp,frameState.getEyeDir());
		const M4 lightSpace = lp*lv;
		//done: intersect with light frustum
		clipByFrustum(temp,lightSpace);
		//calculate the defining points
		calcPoints(B,temp);
	}
	*/
}

static void look( MATRIX * output, VEC3D * pos, VEC3D * dir, VEC3D * up) 
{
	VEC3D dirN, upN, xN;
	REAL32 _x, _y, _z;

	i3Vector::Normalize( &dirN, dir);
	i3Vector::Normalize( &upN, up);

	i3Vector::Cross( &xN, &dirN, &upN);
	i3Vector::Normalize( &xN, &xN);

	i3Vector::Cross( &upN, &xN, &dirN);
	i3Vector::Normalize( &upN, &upN);

	_x = -i3Vector::Dot( &xN, pos);
	_y = -i3Vector::Dot( &upN, pos);
	_z = i3Vector::Dot( &dirN, pos);

	/*
	i3Matrix::SetRight( output,	xN.x,		xN.y,		xN.z,		_x);
	i3Matrix::SetUp(	output,	upN.x,		upN.y,		upN.z,		_y);
	i3Matrix::SetAt(	output,	-dirN.x,	-dirN.y,	-dirN.z,	_z);
	i3Matrix::SetPos(	output,	0.0f,		0.0f,		0.0f,		1.0f);
	*/

	output->m00 = xN.x; output->m01 = upN.x;	output->m02 = -dirN.x;	output->m03 = 0.f;
	output->m10 = xN.y; output->m11 = upN.y;	output->m12 = -dirN.y;	output->m13 = 0.f;
	output->m20 = xN.z; output->m21 = upN.z;	output->m22 = -dirN.z;	output->m23 = 0.f;
	output->m30 = _x;	output->m31 = _y;		output->m32 = _z;		output->m33 = 1.f;
}


void i3ShadowMap::calcStandardLightSpace( MATRIX * pLightView, MATRIX * pLightProj, i3SceneTracer * pTracer) 
{
	//if (Settings::GetInstance()->IsDirectionalLightUsed())
	{
		// directional light
		// this routine calculates a directional
		VEC3D eyePos, lightDir, up;

		i3Vector::Normalize( &lightDir, i3Matrix::GetAt( &m_mLightTx));
		i3Vector::Scale( &lightDir, &lightDir, -1.0f);

		i3Vector::Normalize( &up, i3Matrix::GetAt( pTracer->GetInverseViewMatrix()));
		i3Vector::Scale( &up, &up, -1.0f);

		i3Vector::Copy( &eyePos, i3Matrix::GetPos( pTracer->GetInverseViewMatrix()));

		//look from position(eyePos)
		//into direction(lightDir) 
		//with up vector(up)
		look( pLightView, &eyePos, &lightDir, &up);	
		i3Matrix::Identity( pLightProj);
	}
	/*
	else {
		//spot light
		calcStandardSpotShadowMtx(lightView,lightProj,fs.getEyeDir());
	}
	*/
}

void i3ShadowMap::calcPoints( i3Vector3List * B, i3PolygonList * obj) 
{
	INT32 i, j;

	/*
	Polyhedron ph;
	convert(ph,obj);
	B.clear();
	ph.addPointsToStruct(B);
	*/

	B->Reset();

	for( i = 0; i < obj->getCount(); i++) 
	{
		i3Vector3List * p = obj->getPolygon( i);

		for( j = 0; j < p->getCount(); j++) 
		{
			B->Add( p->GetVector( j));
		}
	}
}

/*
void i3ShadowMap::calcLVS( i3Vector3List * LVS, i3SceneTracer * pTracer) 
{
	//intersection object
	i3PolygonList intersect;

	//calculates the ViewFrustum Object
	{
		MATRIX tmp;

		i3Matrix::Inverse( &tmp, nullptr, pTracer->getViewProjectionMatrix());

		calcFrustObjectGL( &intersect, &tmp);
	}

	//done: intersect with the scene bb
	clipObjectByAABox( &intersect, &m_ReceiverBound);

	//if (!Settings::GetInstance()->IsDirectionalLightUsed()) 
	//{
	//	//calculate the light frustum matrices
	//	M4 lv,lp;
	//	calcStandardSpotShadowMtx(lv,lp,frameState.getEyeDir());
	//	//done: intersect with light frustum
	//	clipByFrustum(intersect,lp*lv);
	//}

	calcPoints( LVS, &intersect);
}
*/

void i3ShadowMap::getNearCameraPointE( VEC3D * pOut, i3SceneTracer * pTracer) 
{
	if( m_LVS.getCount() <= 0)
	{
		i3Vector::Set( pOut, 0.0f, 0.0f, 0.0f);
		return;
	}

	//the LVS volume is always in front of the camera
	m_LVS.TransformPoints( pTracer->GetViewMatrix());

	//the camera points along the neg z axis.
	//-> so the nearest point is the maximum
	unsigned max = 0;
	REAL32 nearestZ;
	VEC3D mid;

	i3Vector::Copy( &mid, m_LVS.GetVector(0));
	nearestZ = m_LVS.GetVector(0)->z;

	for( INT32 i = 1; i < m_LVS.getCount(); i++) 
	{
		if( m_LVS.GetVector(max)->z < m_LVS.GetVector(i)->z) 
		{
			max = i;
			nearestZ = m_LVS.GetVector( i)->z;
		}

		i3Vector::Add( &mid, &mid, m_LVS.GetVector( i));
	}

	#if 0
		i3Vector::Scale( &mid, &mid, 1.0f / m_LVS.getCount());
		mid.z = nearestZ;
	#else
		i3Vector::Copy( &mid, m_LVS.GetVector( max));
	#endif

	//transform back to world space
	i3Vector::TransformCoord( pOut, &mid, pTracer->GetInverseViewMatrix());
}

void i3ShadowMap::getProjViewDir_ls( VEC3D * pOut, MATRIX * lightSpace, i3SceneTracer * pTracer) 
{
	//get the point in the LVS volume that is nearest to the camera
	VEC3D e;
	
	//getNearCameraPointE( &e, pTracer);
	i3Vector::Copy( &e, &m_NearestP_WS);

	//construct edge to transform into light-space
	VEC3D b;
	
	i3Vector::Add( &b, &e, &m_vCamDirWS);

	//transform to light-space
	VEC3D e_lp, b_lp;

	i3Vector::TransformCoord( &e_lp, &e, lightSpace);
	i3Vector::TransformCoord( &b_lp, &b, lightSpace);

	i3Vector::Sub( pOut, &b_lp, &e_lp);
	
	//project the view direction into the shadow map plane
	pOut->y = 0.0f;
}

void Plane_Transform( VEC4D * pOut, VEC4D * pIn, MATRIX * pMtx)
{
	VEC4D tmp;

	i3Vector::Set( &tmp, pIn->x, pIn->y, pIn->z, - pIn->w);

	i3Vector::Transform( pOut, &tmp, pMtx);
	pOut->w = -pOut->w;

	REAL32 t = i3Vector::LengthSq( (VEC3D *) pOut);

	if( t != 1.0f) 
	{
		t = 1.0f / i3Math::Sqrt( t);
		pOut->x *= t;
		pOut->y *= t;
		pOut->z *= t;
		pOut->w *= t;
	}
}

//z0 is the point that lies on the parallel plane to the near plane through e (A)
//and on the near plane of the C frustum (the plane z = bZmax) and on the line x = e.x
void i3ShadowMap::getZ0_ls( VEC3D * pOut, MATRIX * lightSpace, VEC3D * e, REAL32 b_lsZmax) 
{
	//to calculate the parallel plane to the near plane through e we 
	//calculate the plane A with the three points
	// Plane A = Plane(e,frameState.getEyeDir());
	VEC4D A;

	i3Plane::Set( &A, e, &m_vCamDirWS);

	//to transform plane A into lightSpace calculate transposed inverted lightSpace	
	MATRIX invTlightSpace;
	
	i3Matrix::Inverse( &invTlightSpace, nullptr, lightSpace);
	i3Matrix::Transpose( &invTlightSpace, &invTlightSpace);

	//and transform the plane equation with it
	Plane_Transform( &A, &A, &invTlightSpace);

	//get the parameters of A from the plane equation n dot d = 0
	REAL32 d = A.w;
	REAL32 n_x = A.x, n_y = A.y, n_z = A.z;
	
	//transform to light space
	VEC3D e_ls;
	i3Vector::TransformCoord( &e_ls, e, lightSpace);

	//z_0 has the x coordinate of e, the y coord of B.max() 
	//and the z coord of the plane intersection
	i3Vector::Set( pOut, e_ls.x, (d - n_z * b_lsZmax - n_x * e_ls.x) / n_y, b_lsZmax);
}

REAL32 i3ShadowMap::calcNoptGeneral( MATRIX * lightSpace, i3BoundBox * B_ls, i3SceneTracer * pTracer)
{
	MATRIX invLightSpace;
	
	i3Matrix::Inverse( &invLightSpace, nullptr, lightSpace);

	VEC3D z0_ls, z1_ls, e;

	//getNearCameraPointE( &e, pTracer);
	i3Vector::Copy( &e, &m_NearestP_WS);

	getZ0_ls(		&z0_ls, lightSpace, &e, B_ls->GetMax()->z);

	i3Vector::Set(	&z1_ls, z0_ls.x, z0_ls.y, B_ls->GetMin()->z);

	//to world
	VEC3D z0_ws, z1_ws;
	i3Vector::TransformCoord( &z0_ws, &z0_ls, &invLightSpace);
	i3Vector::TransformCoord( &z1_ws, &z1_ls, &invLightSpace);

	//to eye
	VEC3D z0_cs, z1_cs;
	i3Vector::TransformCoord( &z0_cs, &z0_ws, pTracer->GetViewMatrix());
	i3Vector::TransformCoord( &z1_cs, &z1_ws, pTracer->GetViewMatrix());

	REAL32 z0 = z0_cs.z;
	REAL32 z1 = z1_cs.z;

	if( z0 > -0.1f)		z0 = -0.1f;

	REAL32 d = i3Math::abs( B_ls->GetMax()->z - B_ls->GetMin()->z);

	REAL32 n = d / ( i3Math::Sqrt( i3Math::abs( z1 / z0)) - 1.0f);

	return n;
}

void frustumGL( MATRIX * output, REAL32 left, REAL32 right, REAL32 bottom, REAL32 top, REAL32 zNearDis, REAL32 zFarDis) 
{
	REAL32 diffX = 1.0f / ( right - left);
	REAL32 diffY = 1.0f / ( top - bottom);
	REAL32 diffZ = 1.0f / ( zNearDis - zFarDis);

	output->m00 = 2.0f * zNearDis * diffX;
	output->m10 = 0.0f;
	output->m20 = ( right + left) * diffX;
	output->m30 = 0.0f;
			  
	output->m01 = 0.0f;
	output->m11 = 2.0f * zNearDis * diffY;
	output->m21 = ( top + bottom) * diffY;
	output->m31 = 0.0f;
			  
	output->m02 = 0.0f;
	output->m12 = 0.0f;
	output->m22 = (zFarDis + zNearDis) * diffZ;
	output->m32 = 2.0f * zNearDis * zFarDis * diffZ;
			  
	output->m03 = 0.0f;
	output->m13 = 0.0f;
	output->m23 = -1.0f;
	output->m33 = 0.0f;
}

//this is the algorithm discussed in the article
void i3ShadowMap::getLispSmMtx( MATRIX * pOut, MATRIX * lightSpace, i3Vector3List * B, i3SceneTracer * pTracer) 
{
	static i3BoundBox B_ls;
	REAL32 n;
	INT32 i;

	B_ls.Reset(); 

	for( i = 0; i < B->getCount(); i++)
	{
		VEC3D temp;

		i3Vector::TransformCoord( &temp, B->GetVector(i), lightSpace);

		B_ls.ExtendByVec( &temp);
	}

	//i3VLog::SetColor( (UINT8) 0, 255, 0, 255);
	//i3VLog::Draw( &B_ls);

	//get the coordinates of the near camera point in light space
	VEC3D Cstart_lp;

	VEC3D e_ls;
	
	//getNearCameraPointE( &e_ls, pTracer);
	i3Vector::Copy( &e_ls, &m_NearestP_WS);
	i3Vector::TransformCoord( &e_ls, &e_ls, lightSpace);

	//c start has the x and y coordinate of e, the z coord of B.min() 
	i3Vector::Set( &Cstart_lp, e_ls.x, e_ls.y, B_ls.GetMax()->z);

	//Real n = Settings::GetInstance()->GetParameterN();
	//if (Settings::GetInstance()->IsNoptUsed())
	{
		n = calcNoptGeneral( lightSpace, &B_ls, pTracer); 
	}

	if( n >= i3Math::Infinite()) 
	{
		//if n is inf. than we should do uniform shadow mapping
		i3Matrix::Identity( pOut);
		return;
	}
	//frameState.n = n;

	//calc C the projection center
	//new projection center C, n behind the near plane of P
	//we work along a negative axis so we transform +n*<the positive axis> == -n*<neg axis>
	VEC3D C;
	i3Vector::Scale( &C, &I3_ZAXIS, n);
	i3Vector::Add( &C, &C, &Cstart_lp);

	//i3VLog::SetColor( (UINT8) 255, 0, 0, 255);
	//i3VLog::Line( &Cstart_lp, &C);

	//construct a translation that moves to the projection center
	MATRIX projectionCenter;
	i3Matrix::SetTranslate( &projectionCenter, -C.x, -C.y, -C.z);

	//calc d the perspective transform depth or light space y extents
	REAL32 d = i3Math::abs( B_ls.GetMax()->z - B_ls.GetMin()->z);

	//the lispsm perspective transformation
	MATRIX P;

	//here done with a standard frustum call that maps P onto the unit cube with
	//corner points [-1,-1,-1] and [1,1,1].
	//in directX you can use the same mapping and do a mapping to the directX post-perspective cube
	//with corner points [-1,-1,0] and [1,1,1] as the final step after all the shadow mapping.
	frustumGL( &P, -1.0f, 1.0f, -1.0f, 1.0f, n, n+d);
	//i3Matrix::OrthoRH( &P, 2.0f, 2.0f, n, n + d);

	//invert the transform from right handed into left handed coordinate system for the ndc
	//done by the openGL style frustumGL call
	//so we stay in a right handed system
	//MATRIX tmp;
	//i3Matrix::SetScale( &tmp, 1.0f, 1.0f, -1.0f);

	//i3Matrix::Mul( &P, &P, &tmp);

	//return the lispsm frustum with the projection cente
	i3Matrix::Mul( pOut, &projectionCenter, &P);
}

void  scaleTranslateToFit( MATRIX * output, VEC3D * vMin, VEC3D * vMax) 
{
	REAL32 diffX = vMax->x - vMin->x;
	output->m00 = 2.0f / diffX;
	output->m10 = 0.0f;
	output->m20 = 0.0f;
	output->m30 = -(vMax->x + vMin->x) / diffX;

	REAL32 diffY = vMax->y - vMin->y;
	output->m01 = 0.0f;
	output->m11 = 2.0f / diffY;
	output->m21 = 0.0f;
	output->m31 = -(vMax->y + vMin->y) / diffY;

	REAL32 diffZ = vMax->z - vMin->z;
	output->m02 = 0.0f;
	output->m12 = 0.0f;
	output->m22 = 2.0f / diffZ;
	output->m32 = -(vMax->z + vMin->z) / diffZ;

	output->m03 = 0.0f;
	output->m13 = 0.0f;
	output->m23 = 0.0f;
	output->m33 = 1.0f;
}

void scaleTranslateShadowMtx( MATRIX * pOut, MATRIX * lightSpace, i3Vector3List * B) 
{
	//transform the light volume points from world into light space
	//calculate the cubic hull (an AABB) 
	//of the light space extents of the intersection body B
	i3BoundBox b_ls;
	INT32 i;

	for( i = 0; i < B->getCount(); i++)
	{
		VEC3D  tmp;

		i3Vector::TransformCoord( &tmp, B->GetVector(i), lightSpace);

		b_ls.ExtendByVec( &tmp);
	}

	//refit to unit cube
	//this operation calculates a scale translate matrix that
	//maps the two extreme points min and max into (-1,-1,-1) and (1,1,1)
	scaleTranslateToFit( pOut, b_ls.GetMin(), b_ls.GetMax());
}

void i3ShadowMap::CalcLSPSMShadowMatrix2( i3SceneTracer * pTracer)
{
	MATRIX lightView, lightProj;

	//i3VLog::Begin();

	//i3VLog::SetColor( (UINT8) 255, 0, 0, 255);
	//i3VLog::Draw( &m_ReceiverBound);
	
	//calc the intersection Body B
	m_B.Reset();
	_CalcB( pTracer);

	//calculate standard light space for spot or directional lights
	//this routine returns two matrices: 
	//lightview contains the rotated translated frame
	//lightproj contains in the case of a spot light the spot light perspective transformation
	//in the case of a directional light a identity matrix
	calcStandardLightSpace( &lightView, &lightProj, pTracer);

	if(0 == m_B.getCount()) 
	{
		//fs.debug() << "empty intersection body -> completely inside shadow\n";//debug output
		return;
	}

	//coordinate system change for calculations in the article
	MATRIX switchToArticle;
	switchToArticle.m00 = 1.f; switchToArticle.m01 = 0.f;	switchToArticle.m02 = 0.f;	switchToArticle.m03 = 0.f;
	switchToArticle.m10 = 0.f; switchToArticle.m11 = 0.f;	switchToArticle.m12 = -1.f;	switchToArticle.m13 = 0.f;
	switchToArticle.m20 = 0.f; switchToArticle.m21 = 1.f;	switchToArticle.m22 = 0.f;	switchToArticle.m23 = 0.f;
	switchToArticle.m30 = 0.f; switchToArticle.m31 = 0.0f; switchToArticle.m32 = 0.0f;	switchToArticle.m33 = 1.f;

	//switch to the lightspace used in the article
	i3Matrix::Mul( &lightProj, &lightProj, &switchToArticle);

	MATRIX L;
	
	i3Matrix::Mul( &L, &lightView, &lightProj);

	VEC3D projViewDir;

	getProjViewDir_ls( &projViewDir, &L, pTracer); 

	////////////////////////////////////////////////////////////
	//	frameState.debug() << std::fixed << projViewDir.normalize() << std::endl;
	//if(Settings::GetInstance()->IsLispSmUsed())
	{
		//do Light Space Perspective shadow mapping
		//rotate the lightspace so that the proj light view always points upwards
		//calculate a frame matrix that uses the projViewDir[light-space] as up vector
		//look(from position, into the direction of the projected direction, with unchanged up-vector)
		MATRIX LSM, LSPSM, tmp;

		look( &LSM, &I3_ZEROVECTOR, &projViewDir, &I3_YAXIS);
		i3Matrix::Mul( &lightProj, &lightProj, &LSM);

		//i3VLog::SetColor( (UINT8) 0, 255, 0, 255);
		//i3VLog::Draw( &m_B);

		i3Matrix::Mul( &tmp, &lightView, &lightProj);
		getLispSmMtx( &LSPSM, &tmp, &m_B, pTracer);

		i3Matrix::Mul( &lightProj, &lightProj, &LSPSM);
	}

	////////////////////////////////////////////////////////////
	MATRIX PL, tmp;

	i3Matrix::Mul( &PL, &lightView, &lightProj);

	//map to unit cube
	scaleTranslateShadowMtx( &tmp, &PL, &m_B);

	i3Matrix::Mul( &lightProj, &lightProj, &tmp);

	//coordinate system change for calculations in the article
	MATRIX switchToGL;
	switchToGL.m00 = 1.f; switchToGL.m01 = 0.f;	 switchToGL.m02 = 0.f;	switchToGL.m03 = 0.f;
	switchToGL.m10 = 0.f; switchToGL.m11 = 0.f;	 switchToGL.m12 = 1.f;	switchToGL.m13 = 0.f;
	switchToGL.m20 = 0.f; switchToGL.m21 = -1.f; switchToGL.m22 = 0.f;	switchToGL.m23 = 0.f;
	switchToGL.m30 = 0.f; switchToGL.m31 = 0.0f; switchToGL.m32 = 0.0f;	switchToGL.m33 = 1.f;

	//back to open gl coordinate system y <-> z
	i3Matrix::Mul( &lightProj, &lightProj, &switchToGL);

	//transform from right handed system into left handed ndc
	{
		i3Matrix::SetScale( &tmp, 1.0f, 1.0f, -1.0f);
		i3Matrix::Mul( &lightProj, &lightProj, &tmp);
	}

	//SwitchZ( &lightView);
	ToDxProjection( &lightProj);

	#if defined( SCREENSPACE_SHADOW)
		MATRIX inv;

		i3Matrix::Inverse( &inv, nullptr, pTracer->getViewProjectionMatrix());

		i3Matrix::Mul( &m_LitSpaceBasis, &inv, &lightView);
	#else
		i3Matrix::Mul( &m_LitSpaceBasis, pTracer->GetInverseViewMatrix(), &lightView);
	#endif
		
	i3Matrix::Mul( &m_LitSpaceBasis, &m_LitSpaceBasis, &lightProj);

	{
		/*
		{
			// Z-Bias
			// World Space에서 가볍게 Bias를 준다.
			MATRIX bias;
			VEC3D dir;

			i3Vector::Scale( &dir, &m_vLightDirWS, 0.01f);

			i3Matrix::SetTranslate( &bias, &dir);

			i3Matrix::Mul( &lightView, &bias, &lightView);
		}
		*/

		// 우선 Camera-Space로 Transform된 Vertex들을 다시 Projected Light-Space로
		// Transform하기 때문에, View Matrix는 Light-View Point가 아닌, 실제 Caemra-View Point로
		// 잡아야 한다.
		m_pLightViewMatrixAttr->SetMatrix( &lightView);

		m_pLightProjectMatrixAttr->SetMatrix( &lightProj);

		//get the point in the LVS volume that is nearest to the camera	
		m_pLight->setShadowMapMatrix( &m_LitSpaceBasis);
	}

	//i3VLog::End();
}
