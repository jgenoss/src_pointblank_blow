#include "i3SceneDef.h"
#include "i3ShadowMap.h"
#include "i3SceneTracer.h"
#include "i3SceneUtil.h"

struct __CHECKBOUND_INFO
{
	VEC3D			_SweepDir;
	MATRIX *		_pViewMtx = nullptr;
	bool			_hit = false;
	REAL32			_minZ = 0.0f, _maxZ = 0.0f;
	i3ShadowMap *	_pInst = nullptr;
	i3SceneTracer *	_pTracer = nullptr;
} ;

I3SG_TRAVERSAL_RESULT i3ShadowMap::_CheckBoundProc( i3Node * pNode, void * pData, i3MatrixStack * pStack)
{
	__CHECKBOUND_INFO * pInfo = (__CHECKBOUND_INFO *) pData;
	INT32 _case = 1, i;
	VEC3D	pos[8];
	i3BoundBox * pBox;
	i3ShadowMap * pShadowMap = pInfo->_pInst;

	if( pNode->IsFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER))
		return I3SG_TRAVERSAL_CONTINUE;

	if( pNode->GetBound() == nullptr)
		return I3SG_TRAVERSAL_CONTINUE;

	i3Frustum Frustum;

	pBox = pNode->GetBound();

	// 거리를 확인한다.
	{
		REAL32 len;
		VEC3D vec;

		i3Vector::TransformCoord( &vec, i3Matrix::GetPos( pStack->GetTop()), pInfo->_pViewMtx);

		len = i3Vector::LengthSq( &vec);

		if( len > ( pShadowMap->m_AutoFitCamDist * pShadowMap->m_AutoFitCamDist))
			return I3SG_TRAVERSAL_CONTINUE;
	}

	{
		MATRIX mtxFull;

		i3Matrix::Mul( &mtxFull, pStack->GetTop(), pInfo->_pTracer->getViewProjectionMatrix());

		Frustum.Set( &mtxFull );
	}

	_case = Frustum.TestBox( pBox);  //  0 = outside.  1 = inside.   2 = intersection

	switch( _case)
    {
		case 0:   // outside frustum -- test swept sphere for potential shadow caster
			{
				VEC3D center, temp;
				REAL32 radius;

				pBox->GetCenter( &center);
				i3Vector::Sub( &temp, pBox->GetMin(), pBox->GetMax());
				radius = i3Vector::Length( &temp) * 0.5f;
				
				if( Frustum.TestSweptSphere( &center, radius, &pInfo->_SweepDir))
				{
					pInfo->_hit = true;
					i3BoundBox box, lsBox;
					MATRIX tmp;

					i3Matrix::Mul( &tmp, pStack->GetTop(), pInfo->_pViewMtx);

					pBox->Transform( &box, &tmp);
					box.GetCornerVertices( pos);

					i3Matrix::TransformPoints( pos, pos, 8, &pShadowMap->m_LitSpaceBasis);

					for( i = 0; i < 8; i++)
					{
						pShadowMap->m_LitSpaceBound.ExtendByVec( &pos[i]);
						lsBox.ExtendByVec( &pos[i]);
					}

					pInfo->_minZ = min( pInfo->_minZ, -box.GetMax()->z);
					pInfo->_maxZ = max( pInfo->_maxZ, -box.GetMin()->z);
				}
	            
				break;
			}

		case 2:
		case 1:  //  fully inside frustum.  so store large bounding box
			{
				i3BoundBox box, lsBox;
				MATRIX tmp;

				pInfo->_hit = true;

				i3Matrix::Mul( &tmp, pStack->GetTop(), pInfo->_pViewMtx);

				pBox->Transform( &box, &tmp);
				box.GetCornerVertices( pos);

				i3Matrix::TransformPoints( pos, pos, 8, &pShadowMap->m_LitSpaceBasis);

				for( i = 0; i < 8; i++)
				{
					pShadowMap->m_LitSpaceBound.ExtendByVec( &pos[i]);
					lsBox.ExtendByVec( &pos[i]);
				}

				// At 방향이 -Z를 향하기 때문에 작은 값일 수록 멀리 있는 Object이다.
				pInfo->_minZ = min( pInfo->_minZ, -box.GetMax()->z);
				pInfo->_maxZ = max( pInfo->_maxZ, -box.GetMin()->z);

				// ReceiverPoints << instanceBox
				// ReceiverObjects << instance
				break;
			}
			/*
		case 2:   //  big box intersects frustum.  test sub-boxen.  this improves shadow quality, since it allows
					//  a tighter bounding cone to be used.
			{
				//  only include objects in list once
				// CasterObjects << instance
				// ReceiverObjects << instance

				const std::vector<BoundingBox>& boxen = *instance->aaBoxen;
				for (int box=0; box<int(boxen.size()); box++)
				{
					BoundingBox smallBox = boxen[box];
					smallBox.minPt += instance->translation;
					smallBox.maxPt += instance->translation;
					if (sceneFrustum.TestBox(&smallBox)!=0)  // at least part of small box is in frustum
					{
						hit = true;
						XFormBoundingBox(&smallBox, &smallBox, &modelView);
						m_ShadowCasterPoints.push_back(smallBox);
						m_ShadowReceiverPoints.push_back(smallBox);
					}
				}
				break;
			}
			*/
	}

	return I3SG_TRAVERSAL_CONTINUE;

}

void i3ShadowMap::_CalcVirtualCamParam( i3SceneTracer * pTracer)
{
	__CHECKBOUND_INFO info;

	info._hit = false;
	info._pViewMtx = pTracer->GetViewMatrix();
	info._pInst = this;
	info._pTracer = pTracer;
	info._minZ = 1e32f;
	info._maxZ = 0.0f;

    //  frustum is in world space, so that bounding boxes are minimal size (xforming an AABB
    //  generally increases its volume).
	VEC3D vLightDir;
	MATRIX modelView, modelViewProj;

	// Light에 대한 Matrix setting up
	i3Matrix::Mul( &modelView, &m_mLightTx, pTracer->GetViewMatrix());
	i3Matrix::Mul( &modelViewProj, &modelView, pTracer->GetProjectionMatrix());

	// Light Direction (World-Space)
	i3Vector::Normalize( &vLightDir, i3Matrix::GetAt( &m_mLightTx));
	i3Vector::Scale( &info._SweepDir, &vLightDir, -1.0f);

	m_MStack.Reset();
	i3Scene::Traverse( m_pSg, _CheckBoundProc, &info, &m_MStack);

	i3Camera * pCam = pTracer->getCurrentCamera();

	// Object Bound와 View-Frustum 간의 Intersect 영역을 구한다.
	// 최소 교집합 영역
	if( ! info._hit)
    {
		m_zNear = pCam->GetNear();
        m_zFar = pCam->GetFar();
    }
    else
    {
		m_zNear = max( pCam->GetNear(), info._minZ);  
		m_zFar = min( pCam->GetFar(), info._maxZ);
    }
}

void i3ShadowMap::_BuildLiSP_Method1( i3SceneTracer * pTracer)
{
	INT32 i;
	REAL32 sinGamma = i3Math::Sqrt( 1.0f - m_fCosGamma * m_fCosGamma );

	// ShadowMap의 중심 초점을 잡는다. (bodyB의 가운데 점)
	//
    //  for some reason, the paper recommended using the x coordinate of the xformed viewpoint as
    //  the x-origin for lightspace, but that doesn't seem to make sense...  instead, we'll take
    //  the x-midpt of body B (like for the Y axis)
	VEC3D Origin;

	m_LitSpaceBound.GetCenter( &Origin);
    
    //  use average of the "real" near/far distance and the optimized near/far distance to get a more pleasant result
	i3Camera * pCam = pTracer->getCurrentCamera();
	REAL32 n0 = m_zNear + i3Math::Sqrt( m_zNear * m_zFar);
    REAL32 n1 = pCam->GetNear() + i3Math::Sqrt( pCam->GetNear() * pCam->GetFar());
    m_fLSPSM_Nopt  = (n0 + n1) / (2.0f * sinGamma);

    //  now use the weighting to scale between 0.1 and the computed freeN
    REAL32 freeN = (m_fLSPSM_NoptWeight * m_fLSPSM_Nopt) + 0.1f;

	Origin.z = m_LitSpaceBound.GetMin()->z - freeN;

	//  xlate all points in lsBodyB, to match the new lightspace origin, and compute the fov and aspect ratio
    REAL32 maxx=0.f, maxy=0.f, maxz=0.f;

	{
		VEC3D pos[8];
		VEC3D tmp;

		m_LitSpaceBound.GetCornerVertices( pos);
																						
		for( i = 0; i < 8; i++)
		{
			i3Vector::Sub( &tmp, &pos[i], &Origin);

			i3Vector::Copy( &pos[i], &tmp);

			maxx = max( maxx, fabsf( tmp.x / tmp.z));		// X/Z 비율
			maxy = max( maxy, fabsf( tmp.y / tmp.z));		// Y/Z 비율
			maxz = max( maxz, tmp.z);						// Z 좌표가 -가 나오는 것이 Front 방향이다.
		}
	}
	
    MATRIX lsTranslate, lsPerspective;  
    
	// View-Matrix화 시키기 위해...
	i3Matrix::SetTranslate( &lsTranslate, -Origin.x, -Origin.y, -Origin.z); 
	i3Matrix::PerspectiveRH( &lsPerspective, 2.f*maxx*freeN, 2.f*maxy*freeN, freeN, maxz );
	//i3Matrix::PerspectiveFOVRH( &lsPerspective, maxy / maxx, 1.0f,	freeN, maxz);

	//I3TRACE( "%f    %f    %f\n", m_fCosGamma, freeN, maxz);

	{
		MATRIX rot;
		MATRIX * pProj = &lsPerspective;

		i3Matrix::SetRotateAxis( &rot, &I3_XAXIS, I3_DEG2RAD( 180.0f));
	
		i3Matrix::Mul( pProj, &rot, pProj);
	}

	i3Matrix::Mul( &m_LitSpaceBasis, &m_LitSpaceBasis, &lsTranslate );
	i3Matrix::Mul( &m_LitSpaceBasis, &m_LitSpaceBasis, &lsPerspective );

	//  now rotate the entire post-projective cube, so that the shadow map is looking down the Y-axis
	{
		MATRIX lsPermute, lsOrtho;

		lsPermute.m00 = 1.f; lsPermute.m01 = 0.f;	lsPermute.m02 = 0.f;	lsPermute.m03 = 0.f;
		lsPermute.m10 = 0.f; lsPermute.m11 = 0.f;	lsPermute.m12 = -1.f;	lsPermute.m13 = 0.f;
		lsPermute.m20 = 0.f; lsPermute.m21 = 1.f;	lsPermute.m22 = 0.f;	lsPermute.m23 = 0.f;
		lsPermute.m30 = 0.f; lsPermute.m31 = -0.5f; lsPermute.m32 = 1.5f;	lsPermute.m33 = 1.f;

		i3Matrix::OrthoRH( &lsOrtho, 2.f, 1.f, 0.5f, 2.5f * 1.4f);

		{
			MATRIX rot;
			MATRIX * pProj = &lsOrtho;

			i3Matrix::SetRotateAxis( &rot, &I3_XAXIS, I3_DEG2RAD( 180.0f));
			i3Matrix::Mul( pProj, &rot, pProj);
		}

		i3Matrix::Mul( &lsPermute, &lsPermute, &lsOrtho );
		i3Matrix::Mul( &m_LitSpaceBasis, &m_LitSpaceBasis, &lsPermute );
	}

	if ( m_bUnitCubeClip )
    {
		/*
        std::vector<D3DXVECTOR3> receiverPts;
        std::vector<BoundingBox>::iterator rcvrIt = m_ShadowReceiverPoints.begin();
        receiverPts.reserve(m_ShadowReceiverPoints.size() * 8);
        while ( rcvrIt++ != m_ShadowReceiverPoints.end() )
        {
            for ( int i=0; i<8; i++ )
                receiverPts.push_back( rcvrIt->Point(i) );
        }

        D3DXVec3TransformCoordArray( &receiverPts[0], sizeof(D3DXVECTOR3), &receiverPts[0], sizeof(D3DXVECTOR3), &m_LitSpaceBasis, (UINT)receiverPts.size() );

        BoundingBox receiverBox( &receiverPts );
        receiverBox.maxPt.x = min( 1.f, receiverBox.maxPt.x );
        receiverBox.minPt.x = max(-1.f, receiverBox.minPt.x );
        receiverBox.maxPt.y = min( 1.f, receiverBox.maxPt.y );
        receiverBox.minPt.y = max(-1.f, receiverBox.minPt.y );
        REAL32 boxWidth = receiverBox.maxPt.x - receiverBox.minPt.x;
        REAL32 boxHeight = receiverBox.maxPt.y - receiverBox.minPt.y;

        if ( !ALMOST_ZERO(boxWidth) && !ALMOST_ZERO(boxHeight) )
        {
            REAL32 boxX = ( receiverBox.maxPt.x + receiverBox.minPt.x ) * 0.5f;
            REAL32 boxY = ( receiverBox.maxPt.y + receiverBox.minPt.y ) * 0.5f;

            D3DXMATRIX clipMatrix( 2.f/boxWidth,  0.f, 0.f, 0.f,
                                    0.f, 2.f/boxHeight, 0.f, 0.f,
                                    0.f,           0.f, 1.f, 0.f,
                                    -2.f*boxX/boxWidth, -2.f*boxY/boxHeight, 0.f, 1.f );
            D3DXMatrixMultiply( &m_LitSpaceBasis, &m_LitSpaceBasis, &clipMatrix );
        }
		*/
    }
}

void i3ShadowMap::CalcLSPSMShadowMatrix( i3SceneTracer * pTracer)
{
	INT32 i;
	VEC3D vRight, vUp, vAt;
	VEC3D vEye;
	VEC3D vLightDir;

	{
		i3Vector::Normalize( &vLightDir, i3Matrix::GetAt( &m_mLightTx));
		i3Vector::Scale( &vLightDir, &vLightDir, -1.0f);

		i3Vector::Normalize( &vAt, i3Matrix::GetAt( pTracer->GetInverseViewMatrix()));
		i3Vector::Scale( &vAt, &vAt, -1.0f);

		m_fCosGamma = i3Vector::Dot( &vLightDir, &vAt);
	}

	if ( fabsf(m_fCosGamma) >= 0.999f )  // degenerates to uniform shadow map
    {
		//BuildOrthoShadowProjectionMatrix();
		I3TRACE( "Orthogonal situation : %f\n", m_fCosGamma);
    }
    else
    {
		m_LitSpaceBound.Reset();

		// Light-Space에서의 Basic Matrix를 생성한다.
		//
		// Light가 비추는 방향(lightDir)을 Y-축으로 하는 Space를 상정한다.
		// 이것은 Shadow Map의 Quality가 가장 고르게 배분될 수 있는 Space이기 때문이다.
		//
		// bodyB에 포함되어 있는 모든 좌표들은 모두 Eye-Space의 것이기 때문에
		// Axis-Vector들 역시 Eye-Space의 것으로 설정되어야 한다.
		{
			i3Vector::Set( &vEye, 0.0f, 0.0f, 1.0f);

			// Camera-Space에서의 Light-Dir을 Y-축으로 잡는다.
			i3Vector::TransformNormal( &vUp, i3Matrix::GetAt( &m_mLightTx), pTracer->GetViewMatrix());
			i3Vector::Normalize( &vUp, &vUp);
			i3Vector::Scale( &vUp, &vUp, -1.0f);

			//  Camera-Space에서의 Camera가 바라보는 At 방향(-Z 축)을 Z-축으로 잡는다.
			i3Vector::Cross( &vRight, &vUp, &vEye );
			i3Vector::Normalize( &vRight, &vRight );
			i3Vector::Cross( &vAt, &vUp, &vRight );
			i3Vector::Normalize( &vAt, &vAt);

			// 각 Axis Vector들을 가지는 Space-Matrix의 Inverse Matrix (Transpose)
			m_LitSpaceBasis.m00 = vRight.x; m_LitSpaceBasis.m01 = vUp.x;	m_LitSpaceBasis.m02 = vAt.x; m_LitSpaceBasis.m03 = 0.f;
			m_LitSpaceBasis.m10 = vRight.y; m_LitSpaceBasis.m11 = vUp.y;	m_LitSpaceBasis.m12 = vAt.y; m_LitSpaceBasis.m13 = 0.f;
			m_LitSpaceBasis.m20 = vRight.z; m_LitSpaceBasis.m21 = vUp.z;	m_LitSpaceBasis.m22 = vAt.z; m_LitSpaceBasis.m23 = 0.f;
			m_LitSpaceBasis.m30 = 0.0f;		m_LitSpaceBasis.m31 = 0.0f;		m_LitSpaceBasis.m32 = 0.0f;  m_LitSpaceBasis.m33 = 1.f;
			//m_LitSpaceBasis.m30 = -pCamPos->x;      m_LitSpaceBasis.m31 = -pCamPos->y;   m_LitSpaceBasis.m32 = -pCamPos->z;   m_LitSpaceBasis.m33 = 1.f;
		}

        //  Caster들의 Bound를 모두 구한다.
		_CalcVirtualCamParam( pTracer);

		// View-Frustum Volume을 bodyB에 포함시킨다.
		// Camera의 Far 값이 매우 큰 값을 가지는 경우
		// Shadow의 Quality를 저하시킬 수 있다.
		if( m_LitSpaceBound.IsEmpty())
		{
			i3Frustum eyeFrustum;
			i3BoundBox viewBox;

			eyeFrustum.Set( pTracer->GetProjectionMatrix());
	        
			for( i = 0; i < 8; i++ ) 
			{
				VEC3D temp;

				i3Vector::TransformCoord( &temp, &eyeFrustum.pntList[i], &m_LitSpaceBasis);

				m_LitSpaceBound.ExtendByVec( &temp);
			}
		}

		// Free-Parameter n을 계산..
		// 
		_BuildLiSP_Method1( pTracer);
		//_BuildLiSP_Method2( pTracer, &vLightDir, &vRight, &vUp, &vAt);

		{
			//REAL32 fMapSz, fCutoff;

			// Pseudo Light Position
			//i3Vector::TransformCoord( (VEC3D*)&m_LightPos, i3Matrix::GetPos( &m_mLightTx), pTracer->GetViewMatrix());

			// Pseudo Light Direction
			///i3Vector::TransformNormal( (VEC3D*)&m_LightDir, i3Matrix::GetAt( &m_mLightTx), pTracer->GetViewMatrix());
			//i3Vector::Normalize( (VEC3D*)&m_LightDir, (VEC3D*)&m_LightDir);

			// 우선 Camera-Space로 Transform된 Vertex들을 다시 Projected Light-Space로
			// Transform하기 때문에, View Matrix는 Light-View Point가 아닌, 실제 Caemra-View Point로
			// 잡아야 한다.
			m_pLightViewMatrixAttr->SetMatrix( pTracer->GetViewMatrix());
			m_pLightProjectMatrixAttr->SetMatrix( &m_LitSpaceBasis);

			m_pLight->setShadowMapMatrix( &m_LitSpaceBasis);

			/*
			pTracer->GetRenderContext()->AddShadowMap( m_pShadowRTAttr->GetColorRenderTarget(), &m_LitSpaceBasis, nullptr, 
				(VEC3D *) &m_LightPos, (VEC3D *) &m_LightDir, 
				&m_ShadowColor, fMapSz, fCutoff);
				*/
		}
    }
}