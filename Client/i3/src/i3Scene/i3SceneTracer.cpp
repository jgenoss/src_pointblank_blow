#include "i3SceneDef.h"
#include "i3SceneTracer.h"
#include "i3CommonNodeProcs.h"
#include "i3AllNodes.h"
#include "i3AllAttrs.h"

#include "i3Math/structure/aabb.h"
#include "i3XrayScene.h"

I3_CLASS_INSTANCE( i3SceneTracer);

static i3AttrStackManager * g_pAttrStackManager = nullptr;

i3SceneTracer::i3SceneTracer(void)
{	
	#if defined( I3G_PSP)
		m_Style = I3SG_TRACER_STYLE_NO_VIEWMATRIX;
	#endif
		
	m_pMatrixStack = i3MatrixStack::new_object();

	m_pAttrStackManager = i3AttrStackManager::new_object();

	m_pDefOutput = i3RenderAttrListAttr::new_object();
	m_pOutput = m_pDefOutput;
	
	#if defined( I3_PSP)
	__asm__ (
		".set			push\n"					//
		".set			noreorder\n"			//

		"vmidt.q		e600\n"					// View Matrix -> Identity
		"vmidt.q		e700\n"					// Projection Matrix -> Identity

		"sv.q			c600,  0 + %0\n"
		"sv.q			c610, 16 + %0\n"
		"sv.q			c620, 32 + %0\n"
		"sv.q			c630, 48 + %0\n"		// Init m_ViewMatrix

		"sv.q			c600,  0 + %1\n"
		"sv.q			c610, 16 + %1\n"
		"sv.q			c620, 32 + %1\n"
		"sv.q			c630, 48 + %1\n"		// Init m_ProjectionMatrix

		".set			pop\n"					// restore assember option
		: "=m"(m_ViewMatrix), "=m" (m_ProjectionMatrix)
		);
	#endif

	m_FrustumPlane.apply(reinterpret_cast<const i3::mat4&>(m_ViewAndProjectMatrix));
	
	m_MatrixInfo.m_pViewMatrix = &m_ViewMatrix;
	m_MatrixInfo.m_pInvertViewMatrix = &m_InvViewMatrix;
	m_MatrixInfo.m_pProjectioMatrix = &m_ProjectionMatrix;

	m_pModelViewMatrixAttrPool = i3ObjectPool::new_object();
	m_pModelViewMatrixAttrPool->Create( i3ModelViewMatrixAttr::static_meta(), 2048, 256); 
	
	m_OutputList.reserve( 32);

	i3SceneTracer::InitAlphaSortNodeList();
}


i3SceneTracer::~i3SceneTracer(void)
{
	i3SceneTracer::DestroyAlphaSortNodeList();

	I3_SAFE_RELEASE( m_pSgContext);
	if( m_pDefOutput != nullptr)
	{
		m_pDefOutput->Reset();
		I3_SAFE_RELEASE( m_pDefOutput);
	}

	I3_SAFE_RELEASE( m_pMatrixStack);
	I3_SAFE_RELEASE( m_pAttrStackManager);
	g_pAttrStackManager = nullptr;

	I3_SAFE_RELEASE( m_pModelViewMatrixAttrPool);
}

void i3SceneTracer::OutputListReset(void)
{
	
	for(size_t i = 0; i < m_OutputList.size(); i++)
	{
		i3RenderAttrListAttr * pAttr = m_OutputList[i];

		pAttr->Reset();
	}

	m_OutputList.clear();
	m_OutputList.push_back( m_pDefOutput);
	m_pOutput = m_pDefOutput;
}

void i3SceneTracer::Reset(void)
{
	if (m_pSgContext)
		m_pSgContext->GetXrayScene()->ResetXrayRenderAttList();

	m_pAttrStackManager->Reset();

	OutputListReset();		

	m_pMatrixStack->Reset();

	// 반드시 다른 모든 Reset이 완료되고 난 후, ModelViewMatrixAttrPool의 Reset을 해야한다.
	m_pModelViewMatrixAttrPool->Reset();

	#if defined( I3_PSP)
	__asm__ (
		".set			push\n"					//
		".set			noreorder\n"			//

		"vmidt.q		e600\n"					// View Matrix -> Identity
		"vmidt.q		e700\n"					// Projection Matrix -> Identity

		"sv.q			c600,  0 + %0\n"
		"sv.q			c610, 16 + %0\n"
		"sv.q			c620, 32 + %0\n"
		"sv.q			c630, 48 + %0\n"		// Init m_ViewMatrix

		"sv.q			c600,  0 + %1\n"
		"sv.q			c610, 16 + %1\n"
		"sv.q			c620, 32 + %1\n"
		"sv.q			c630, 48 + %1\n"		// Init m_ProjectionMatrix

		".set			pop\n"					// restore assember option
		: "=m"(m_ViewMatrix), "=m" (m_ProjectionMatrix)
		);
	#else
		i3Matrix::Identity( &m_ViewMatrix);
		i3Matrix::Identity( &m_ProjectionMatrix);
	#endif

	i3Matrix::Identity( &m_InvViewMatrix);
	i3Matrix::Identity( &m_ViewAndProjectMatrix);

	m_AnimCtxStack.Reset();
	m_BoneMatrixStack.Reset();

	m_PlaneMask = 0;
	m_FrameID ++;

#if !defined( I3_NO_PROFILE)
	m_NodeCount = 0;
#endif

	m_MatrixInfo.m_FrameID = m_FrameID;

	i3SceneTracer::ResetAlphaSortNodeList();
	i3SceneTracer::ResetAlphaSortNodeListForDelete();
}

void i3SceneTracer::SetCamera( i3Camera * pCamera)
{
	MATRIX invertMtx;
	i3MatrixStack * pStack = GetModelViewMatrixStack();

	I3ASSERT( pCamera != nullptr);
	m_pCurCamera = pCamera;

	if( (pCamera->getStyle() & I3_CAM_STYLE_PROJECTION) != I3_CAM_STYLE_PROJECTION)
	{
		if( m_pSoundCtx != nullptr && ((pCamera->getStyle() & I3_CAM_STYLE_SUB) == 0))
		{
			m_pSoundCtx->SetListnerMatrix( pStack->GetTop(), GetTime());
		}
	}

	i3Matrix::Inverse( &invertMtx, nullptr, pStack->GetTop());
	SetViewMatrix( &invertMtx, pStack->GetTop());

	SetProjectionMatrix( pCamera->GetProjectionMatrix());

	m_FrustumPlane.apply(reinterpret_cast<const i3::mat4&>(m_ViewAndProjectMatrix));

	{
		i3ProjectMatrixAttr * pAttr = pCamera->GetProjectMatrixAttr();

		pAttr->SetMatrix( &m_ProjectionMatrix);
		GetAttrStackManager()->PushAttr( pAttr);
	}

	{
		i3ViewMatrixAttr * pAttr = pCamera->GetViewMatrixAttr();

		/*
		if( IsStyle( I3SG_TRACER_STYLE_NO_VIEWMATRIX))
		{
			// 실제로는 Identity Matrix가 전달되어야 하지만,
			// GPU Command List를 Rendering하는 경우에는 
			// View Matrix가 올바르게 들어가 있어야 하기 때문에
			// Render Context 선에서 필요한 처리를 한다.
			i3Matrix::Identity( pAttr->GetMatrix());
		}
		else
		*/
		{
			pAttr->SetMatrix( &m_ViewMatrix);
		}
        GetAttrStackManager()->PushAttr( pAttr);
	}

}

void i3SceneTracer::RestoreCamera( i3Camera * pCam)
{
	m_pCurCamera = pCam;

	i3AttrStack * pStack;

	// View Matrix
	{
		pStack = GetAttrStackManager()->GetStack( I3_ATTRID_VIEW_MATRIX);
		GetAttrStackManager()->PopAttr( I3_ATTRID_VIEW_MATRIX);

		if( pStack->GetTop() != nullptr)
		{
			i3ViewMatrixAttr * pAttr = (i3ViewMatrixAttr *) pStack->GetTop();
			MATRIX mtx;

			i3Matrix::Inverse( &mtx, nullptr, pAttr->GetMatrix());

			SetViewMatrix( pAttr->GetMatrix(), &mtx);
		}
		else
		{
			SetViewMatrix( &I3_IDENTITYMATRIX, &I3_IDENTITYMATRIX);
		}
	}

	// Projection Matrix
	{
		pStack = GetAttrStackManager()->GetStack( I3_ATTRID_PROJECT_MATRIX);
		GetAttrStackManager()->PopAttr( I3_ATTRID_PROJECT_MATRIX);

		if( pStack->GetTop() != nullptr)
		{
			i3ProjectMatrixAttr * pAttr = (i3ProjectMatrixAttr *) pStack->GetTop();

			SetProjectionMatrix( pAttr->GetMatrix());
		}
		else
		{
			SetProjectionMatrix( &I3_IDENTITYMATRIX);
		}
	}

}

void i3SceneTracer::SetViewMatrix( MATRIX * pViewMat, MATRIX * pInvViewMat)
{
#if defined( I3_PSP)
	__asm__ (
		".set		push\n"
		".set		noreorder\n"

		"lv.q		c600,  0 + %2\n" 
		"lv.q		c610, 16 + %2\n"
		"lv.q		c620, 32 + %2\n"
		"lv.q		c630, 48 + %2\n"		// View Matrix의 Load

		"vrmmul.q	e000, e600, e700\n"		// View Matrix와 Projection Matrix의 Multiplay

		"sv.q		c600,  0 + %1\n"
		"sv.q		c610, 16 + %1\n"
		"sv.q		c620, 32 + %1\n"
		"sv.q		c630, 48 + %1\n"		// View Matrix -> m_ViewMatrix

		"sv.q		c000,  0 + %0\n"
		"sv.q		c010, 16 + %0\n"
		"sv.q		c020, 32 + %0\n"
		"sv.q		c030, 48 + %0\n"		// View & Projection Matrix

		".set		pop\n"
		: "=m" (m_ViewAndProjectMatrix), "=m" (m_ViewMatrix)
		: "m"( *pViewMat)
		);

	i3Matrix::Copy( &m_InvViewMatrix, pInvViewMat); 

#else
	i3Matrix::Copy( &m_ViewMatrix, pViewMat);
	i3Matrix::Copy( &m_InvViewMatrix, pInvViewMat);
	i3Matrix::Mul( &m_ViewAndProjectMatrix, &m_ViewMatrix, &m_ProjectionMatrix);
#endif

}

void i3SceneTracer::SetProjectionMatrix( MATRIX * pMat)
{
#if defined( I3_PSP)
	__asm__ (
		".set		push\n"
		".set		noreorder\n"

		"lv.q		c700,  0 + %2\n"
		"lv.q		c710, 16 + %2\n"
		"lv.q		c720, 32 + %2\n"
		"lv.q		c730, 48 + %2\n"		// Projection Matrix의 Load

		"vrmmul.q	e000, e600, e700\n"		// View Matrix와 Projection Matrix의 Multiplay

		"sv.q		c700,  0 + %1\n" 
		"sv.q		c710, 16 + %1\n"
		"sv.q		c720, 32 + %1\n"
		"sv.q		c730, 48 + %1\n"		// Projection Matrix -> m_ProjectionMatrix

		"sv.q		c000,  0 + %0\n"
		"sv.q		c010, 16 + %0\n"
		"sv.q		c020, 32 + %0\n"
		"sv.q		c030, 48 + %0\n"		// View & Projection Matrix Update

		".set		pop\n"
		: "=m" (m_ViewAndProjectMatrix), "=m" (m_ProjectionMatrix)
		: "m"( *pMat)
		);
#else
	i3Matrix::Copy( &m_ProjectionMatrix, pMat);
	i3Matrix::Mul( &m_ViewAndProjectMatrix, &m_ViewMatrix, &m_ProjectionMatrix);
#endif
}

inline void _ExtractPlanes( VEC4D * pPlane, MATRIX * pTmx)
{
	{
		// Left clipping plane
		i3Vector::SetX( pPlane, i3Matrix::Get14(pTmx) + i3Matrix::Get11(pTmx));
		i3Vector::SetY( pPlane, i3Matrix::Get24(pTmx) + i3Matrix::Get21(pTmx));
		i3Vector::SetZ( pPlane, i3Matrix::Get34(pTmx) + i3Matrix::Get31(pTmx));
		i3Vector::SetW( pPlane, i3Matrix::Get44(pTmx) + i3Matrix::Get41(pTmx));		pPlane++;

		// Right clipping plane
		i3Vector::SetX( pPlane, i3Matrix::Get14(pTmx) - i3Matrix::Get11(pTmx));
		i3Vector::SetY( pPlane, i3Matrix::Get24(pTmx) - i3Matrix::Get21(pTmx));
		i3Vector::SetZ( pPlane, i3Matrix::Get34(pTmx) - i3Matrix::Get31(pTmx));
		i3Vector::SetW( pPlane, i3Matrix::Get44(pTmx) - i3Matrix::Get41(pTmx));		pPlane++;

		// Top clipping plane
		i3Vector::SetX( pPlane, i3Matrix::Get14(pTmx) - i3Matrix::Get12(pTmx));
		i3Vector::SetY( pPlane, i3Matrix::Get24(pTmx) - i3Matrix::Get22(pTmx));
		i3Vector::SetZ( pPlane, i3Matrix::Get34(pTmx) - i3Matrix::Get32(pTmx));
		i3Vector::SetW( pPlane, i3Matrix::Get44(pTmx) - i3Matrix::Get42(pTmx));		pPlane++;

		// Bottom clipping plane
		i3Vector::SetX( pPlane, i3Matrix::Get14(pTmx) + i3Matrix::Get12(pTmx));
		i3Vector::SetY( pPlane, i3Matrix::Get24(pTmx) + i3Matrix::Get22(pTmx));
		i3Vector::SetZ( pPlane, i3Matrix::Get34(pTmx) + i3Matrix::Get32(pTmx));
		i3Vector::SetW( pPlane, i3Matrix::Get44(pTmx) + i3Matrix::Get42(pTmx));		pPlane++;


		// Near clipping plane
		i3Vector::SetX(pPlane, i3Matrix::Get13(pTmx));	//											i3Matrix::Get14(pTmx) + i3Matrix::Get13(pTmx));
		i3Vector::SetY(pPlane, i3Matrix::Get23(pTmx));	//											i3Matrix::Get24(pTmx) + i3Matrix::Get23(pTmx));
		i3Vector::SetZ(pPlane, i3Matrix::Get33(pTmx));	//											i3Matrix::Get34(pTmx) + i3Matrix::Get33(pTmx));
		i3Vector::SetW(pPlane, i3Matrix::Get43(pTmx));  //											i3Matrix::Get44(pTmx) + i3Matrix::Get43(pTmx));		
		pPlane++;

		// Far clipping plane
		i3Vector::SetX( pPlane, i3Matrix::Get14(pTmx) - i3Matrix::Get13(pTmx));
		i3Vector::SetY( pPlane, i3Matrix::Get24(pTmx) - i3Matrix::Get23(pTmx));
		i3Vector::SetZ( pPlane, i3Matrix::Get34(pTmx) - i3Matrix::Get33(pTmx));
		i3Vector::SetW( pPlane, i3Matrix::Get44(pTmx) - i3Matrix::Get43(pTmx));		pPlane++;
	}

	/*
	{
		VEC4D * pTempVec;

		pTempVec = pOrgPlane;
		i3Plane::Normalize( pTempVec, pTempVec);	pTempVec++;
		i3Plane::Normalize( pTempVec, pTempVec);	pTempVec++;
		i3Plane::Normalize( pTempVec, pTempVec);	pTempVec++;
		i3Plane::Normalize( pTempVec, pTempVec);	pTempVec++;
		i3Plane::Normalize( pTempVec, pTempVec);	pTempVec++;
		i3Plane::Normalize( pTempVec, pTempVec);
	}
	*/
}

/*
static UINT32		s_PlaneMask[6] =
{
	I3_VFC_LEFT,
	I3_VFC_RIGHT,
	I3_VFC_NEAR,
	I3_VFC_FAR,
	I3_VFC_TOP,
	I3_VFC_BOTTOM
};
*/

#define TRANSFORM_PLANE

// 프러스텀 컬링 구현이 원시적이라서, 
// 충분한 리팩토링 시간이 주어지면 전체적으로 수정하는게 좋을듯한데.... (Frustum_AABB 구현 함수는 기본구현도 이보다는 좋은 편이고, 최적화 정보도 웹에 많이 존재함)
// 매번 모델좌표계를 위해서 매 회 로컬 프러스텀 구하는 것도 문제이고, 정점 8개를 일일이 평면검사하는것도 문제임..
// (2015.04.16.수빈)  
namespace
{
	enum intersect_side
	{
		positive_side,
		negative_side,
		both_side,
	};

	inline
		float			plane_abs_dot_normal(const VEC4D& pl, const VEC3D& v)
	{
		return fabs(pl.x * v.x) + fabs(pl.y * v.y) + fabs(pl.z * v.z);
	}
	inline
		float			calc_plane_distance(const VEC4D& pl, const VEC3D& v)
	{
		return pl.x * v.x + pl.y * v.y + pl.z * v.z + pl.w;
	}

	intersect_side  aabb_plane(const VEC3D& center, const VEC3D& extent, const VEC4D& pl)
	{
		float fMaxDist = plane_abs_dot_normal(pl, extent);
		float fDist = calc_plane_distance(pl, center);

		if (fDist < -fMaxDist) return negative_side;
		if (fDist > fMaxDist) return positive_side;

		return both_side;
	}

	intersect_side  aabb_plane(i3BoundBox* pBox, const VEC4D& pl)
	{
		const VEC3D& center = *pBox->getCenter();
		VEC3D extent = *pBox->GetMax();	extent -= center;
		return aabb_plane(center, extent, pl);
	}
}


bool i3SceneTracer::IsInsideFrustum(i3BoundBox * pBox)
{

	VEC4D * pPlane;

	//
	if (m_pMatrixStack->IsModified())
	{
		MATRIX tmp;

		if (IsStyle(I3SG_TRACER_STYLE_NO_VIEWMATRIX))
		{
			i3Matrix::Mul(&tmp, m_pMatrixStack->GetTop(), GetProjectionMatrix());
		}
		else
		{
			i3Matrix::Mul(&tmp, m_pMatrixStack->GetTop(), &m_ViewAndProjectMatrix);
		}

		_ExtractPlanes(m_FrustPlane, &tmp);

		m_pMatrixStack->SetModified(false);
	}

	pPlane = m_FrustPlane;

	const VEC3D& center = *pBox->getCenter();
	VEC3D extent = *pBox->GetMax();	extent -= center;

	for (INT32 i = 0; i < 6; i++, pPlane++)
	{
		intersect_side  result_side = aabb_plane(center, extent, *pPlane);

		if (result_side == negative_side)
			return false;
	}

	return true;
}

/*
bool i3SceneTracer::IsInsideFrustum( i3BoundBox * pBox)
{
	VEC3D * pMin, * pMax, pos[8];
	INT32 i, j;
	REAL32 x1, y1, z1, x2, y2, z2;
	VEC4D * pPlane;
	REAL32 dot;
	INT32 count = 8;
	VEC3D * pPos;

	#if defined( I3_FIXED_POINT)
		VEC3D vmin, vmax;

		i3Vector::Copy( &vmin, pBox->GetMin());
		i3Vector::Copy( &vmax, pBox->GetMax());

		pMin = &vmin;
		pMax = &vmax;
	#else
		pMin = pBox->GetMin();
		pMax = pBox->GetMax();
	#endif

	x1 = getX( pMin);
	y1 = getY( pMin);
	z1 = getZ( pMin);

	x2 = getX( pMax);
	y2 = getY( pMax);
	z2 = getZ( pMax);

	i3Vector::Set( &pos[0], x1, y1, z1);
	i3Vector::Set( &pos[1], x1, y1, z2);

	i3Vector::Set( &pos[2], x1, y2, z1);
	i3Vector::Set( &pos[3], x1, y2, z2);

	i3Vector::Set( &pos[4], x2, y1, z1);
	i3Vector::Set( &pos[5], x2, y1, z2);

	i3Vector::Set( &pos[6], x2, y2, z1);
	i3Vector::Set( &pos[7], x2, y2, z2);

	//
	if( m_pMatrixStack->IsModified())
	{
		MATRIX tmp;

		if( IsStyle( I3SG_TRACER_STYLE_NO_VIEWMATRIX))
		{
            i3Matrix::Mul( &tmp, m_pMatrixStack->GetTop(), GetProjectionMatrix());
		}
		else
		{
			i3Matrix::Mul( &tmp, m_pMatrixStack->GetTop(), &m_ViewAndProjectMatrix);
		}

		_ExtractPlanes( m_FrustPlane, &tmp);

		m_pMatrixStack->SetModified( false);
	}

	pPlane = m_FrustPlane;

	for( i = 0; i < 6; i++, pPlane++)
	{
		count = 8;

		pPos = pos;

		for( j = 0; j < 8; j++, pPos++)
		{
			#if defined( I3_FIXED_POINT)
				dot =	i3FXD::Mul( i3Vector::GetX( pPos), i3Vector::GetX( pPlane)) +
						i3FXD::Mul( i3Vector::GetY( pPos), i3Vector::GetY( pPlane)) +
						i3FXD::Mul( i3Vector::GetZ( pPos), i3Vector::GetZ( pPlane)) + 
						i3Vector::GetW( pPlane);

				if( dot < 0)
				{
					count--;
				}
			#else
				dot =	(i3Vector::GetX( pPos) * i3Vector::GetX( pPlane)) +
						(i3Vector::GetY( pPos) * i3Vector::GetY( pPlane)) +
						(i3Vector::GetZ( pPos) * i3Vector::GetZ( pPlane)) + 
						i3Vector::GetW( pPlane);

				if( i3Math::IsNegative( dot))
				{
					count--;
				}
				else
				{
					break;
				}
			#endif
		}

		if( count == 0)
		{
			return false;
		}
	}

	return true;
}
*/

INT32 i3SceneTracer::IsInsideFrustum( REAL32 size, VEC3D * vCenter)
{
	VEC3D pos[8];
	INT32 i, j;

	VEC4D		*	pPlane;
	REAL32			dot;

	INT32 count			= 8;
	INT32 planecount	= 0;
	REAL32 half = size * 0.5f;

	REAL32 cx	= getX( vCenter);
	REAL32 cy	= getY( vCenter);
	REAL32 cz	= getZ( vCenter);

	i3Vector::Set( &pos[0], cx - half, cy - half, cz - half);
	i3Vector::Set( &pos[1], cx - half, cy - half, cz + half);
	i3Vector::Set( &pos[2], cx - half, cy + half, cz - half);
	i3Vector::Set( &pos[3], cx - half, cy + half, cz + half);
	i3Vector::Set( &pos[4], cx + half, cy - half, cz - half);
	i3Vector::Set( &pos[5], cx + half, cy - half, cz + half);
	i3Vector::Set( &pos[6], cx + half, cy + half, cz - half);
	i3Vector::Set( &pos[7], cx + half, cy + half, cz + half);

	//
	if( m_pMatrixStack->IsModified())
	{
		MATRIX tmp;

		if( IsStyle( I3SG_TRACER_STYLE_NO_VIEWMATRIX))
		{
            i3Matrix::Mul( &tmp, m_pMatrixStack->GetTop(), GetProjectionMatrix());
		}
		else
		{
			i3Matrix::Mul( &tmp, m_pMatrixStack->GetTop(), &m_ViewAndProjectMatrix);
		}

		_ExtractPlanes( m_FrustPlane, &tmp);

		m_pMatrixStack->SetModified(false);
	}

	pPlane = m_FrustPlane;

	for( i = 0; i < 6; i++, pPlane++)
	{
		count = 8;

		for( j = 0; j < 8; j++)
		{
			#if defined( I3_FIXED_POINT)
				dot =	i3FXD::Mul( i3Vector::GetX( &pos[j]), i3Vector::GetX( pPlane)) +
						i3FXD::Mul( i3Vector::GetY( &pos[j]), i3Vector::GetY( pPlane)) +
						i3FXD::Mul( i3Vector::GetZ( &pos[j]), i3Vector::GetZ( pPlane)) + 
						i3Vector::GetW( pPlane);

				if( dot < 0)
				{
					count--;
				}
			#else
				dot =	(i3Vector::GetX( &pos[j]) * i3Vector::GetX( pPlane)) +
						(i3Vector::GetY( &pos[j]) * i3Vector::GetY( pPlane)) +
						(i3Vector::GetZ( &pos[j]) * i3Vector::GetZ( pPlane)) + 
						i3Vector::GetW( pPlane);

				if( i3Math::IsNegative( dot))
				{
					count--;
				}
				else
				{
					break;
				}
			#endif
		}

		if( count == 0)
		{
			return -1;		//모든점이 대상 frustum평면에 유효하지 않은 위치에 있다. 고로. 안보인다.
		}
		else if( count == 8)
		{
			planecount++;	//모든점이 대상 frustum평면에 유효한위치에 있다.
		}
	}

	if( planecount == 6)	//모든점이 모든 frustum평면에 유효한 위치에 있다. 하위 노드까지도 무조건 보인다. 
		return 1;
	else
		return 0;			//점들중 유효한 위치에 있는것도있고, 아닌것도있다. 하위 노드는 더 검사해봐야 안다.
}

void			i3SceneTracer::PushAABBIntoStack(i3::aabb* a)
{
	m_AABBStack.push_back(a);		// 너무 간단???
}

i3::aabb*		i3SceneTracer::PopAABBFromStack()				// 없으면 nullptr리턴...
{
	if (m_AABBStack.empty()) return nullptr;

	i3::aabb* popped_aabb = m_AABBStack.back();
	m_AABBStack.pop_back();

	// 이 때, 추가적인 탑이 존재하면 확장 처리 해줌...
	if (m_AABBStack.empty() == false)
	{
		i3::aabb* top_aabb = m_AABBStack.back();
		top_aabb->merge(*popped_aabb);
	}
	//

	return popped_aabb;
}

i3::aabb*		i3SceneTracer::TopAABBInStack()		// 없으면 nullptr리턴...	// pop된 aabb의 경우, 그다음번 top의 aabb에 merge처리를 해준다.
{
	if (m_AABBStack.empty()) return nullptr;
	return m_AABBStack.back();
}

void	i3SceneTracer::PreTrace(i3Node* pRoot)
{
	if ((pRoot->GetFlag() & getDisableNodeMask()) == 0)
	{
		pRoot->PreTrace(this);
	}
}

void i3SceneTracer::Trace( i3Node * pRoot)
{
	m_bCulled = false;

	if( (pRoot->GetFlag() & getDisableNodeMask()) == 0)
	{
		pRoot->Trace( this);

		m_pAttrStackManager->FlushPushedAttrs( GetOutput());
	}
}

void i3SceneTracer::PushOutputList( i3RenderAttrListAttr * pNewOutputList)
{
	i3AttrStack * pStack;

	pStack = m_pAttrStackManager->GetAttrStackByID( I3_ATTRID_RENDER_ATTR_LIST);
	pStack->Push( m_pOutput);

	m_OutputList.push_back( pNewOutputList);

	m_pOutput = pNewOutputList;
}

void i3SceneTracer::PopOutputList(void)
{
	i3AttrStack * pStack;

	pStack = m_pAttrStackManager->GetAttrStackByID( I3_ATTRID_RENDER_ATTR_LIST);
	m_pOutput = (i3RenderAttrListAttr *) pStack->GetTop();
	pStack->Pop();
}

void i3SceneTracer::SetRenderContext( i3RenderContext * pContext)
{
	m_pContext = pContext;

	m_pAttrStackManager->SetRenderContext( pContext);

	if((m_pSgContext != nullptr) && (pContext != nullptr))
	{
		pContext->setLightList( m_pSgContext->getLightList());
	}
}

void i3SceneTracer::setSgContext( i3SceneGraphContext * pCtx)
{
	I3_REF_CHANGE( m_pSgContext, pCtx);

	if( m_pContext != nullptr)
	{
		m_pContext->setLightList( pCtx->getLightList());
	}
}

void i3SceneTracer::SaveContext( I3SG_TRACER_CONTEXT * pCtx)
{
	i3Matrix::Copy( &pCtx->m_ViewMatrix, &m_ViewMatrix);
	i3Matrix::Copy( &pCtx->m_InvViewMatrix, &m_InvViewMatrix);
	i3Matrix::Copy( &pCtx->m_ProjectionMatrix, &m_ProjectionMatrix);
	pCtx->m_Time = m_Time;
	pCtx->m_bFrustCull = m_bFrustCull;
}

void i3SceneTracer::RestoreContext( I3SG_TRACER_CONTEXT * pCtx)
{
	SetViewMatrix( &pCtx->m_ViewMatrix, &pCtx->m_InvViewMatrix);
	SetProjectionMatrix( &pCtx->m_ProjectionMatrix);
	m_Time = pCtx->m_Time;
	m_bFrustCull = pCtx->m_bFrustCull;

	m_FrustumPlane.apply(reinterpret_cast<const i3::mat4&>(m_ViewAndProjectMatrix));
}

void	i3SceneTracer::InitAlphaSortNodeList()
{
	const size_t num_init = 100;
	m_pAlphaSortNodeSetPool.resize(num_init);
	
	for (size_t i = 0 ; i < num_init ; ++i)
	{
		m_pAlphaSortNodeSetPool[i] = new i3AlphaSortNodeSet;
		m_pAlphaSortNodeSetPool[i]->m_pAlphaRoot = nullptr;		// 레퍼런스카운팅 하지 않음.
		m_pAlphaSortNodeSetPool[i]->m_startNodeAttrIndex = 0;
		m_pAlphaSortNodeSetPool[i]->m_NodeAttrList = i3RenderAttrListAttr::new_object();
	}
}

void	i3SceneTracer::DestroyAlphaSortNodeList()
{
	const size_t num_list = m_pAlphaSortNodeSetList.size();
	for (size_t i = 0 ; i < num_list ; ++i)
	{
		m_pAlphaSortNodeSetList[i]->m_NodeAttrList->Release();
		delete m_pAlphaSortNodeSetList[i];
	}
	m_pAlphaSortNodeSetList.clear();

	const size_t num_delete = m_pAlphaSortNoseSetListForDelete.size();
	for (size_t i = 0 ; i < num_delete ; ++i)
	{
		m_pAlphaSortNoseSetListForDelete[i]->m_NodeAttrList->Release();
		delete m_pAlphaSortNoseSetListForDelete[i];
	}
	m_pAlphaSortNoseSetListForDelete.clear();

	const size_t num_pool = m_pAlphaSortNodeSetPool.size();
	for (size_t i = 0 ; i < num_pool ; ++i)
	{
		m_pAlphaSortNodeSetPool[i]->m_NodeAttrList->Release();
		delete m_pAlphaSortNodeSetPool[i];
	}
	m_pAlphaSortNodeSetPool.clear();
}

void	i3SceneTracer::ActivateAlphaSortNode( i3Node* node)
{
	if (m_bActivateAlphaSortNode)		// 이미 Active상태라면 아무것도 하지 말것..
		return;
	
	if (m_pAlphaSortNodeSetPool.empty())
	{
		i3SceneTracer::InitAlphaSortNodeList();
	}
	
	i3AlphaSortNodeSet* pNewSet = m_pAlphaSortNodeSetPool.back();
	m_pAlphaSortNodeSetPool.pop_back();

	MATRIX* matWorld = i3SceneTracer::GetModelViewMatrixStack()->GetTop();
	MATRIX* matView  = i3SceneTracer::GetViewMatrix();

	MATRIX  matWorldView;
	i3Matrix::Mul(&matWorldView, matWorld, matView); // matWorld의 1~3행성분을 노멀라이즈하면, 정규 방향벡터 각각 나옴.. 1~3행 스칼라 값은 스케일값..
	
	// 노드에 대한 바운딩 박스를 OBB형식으로 얻어내야함..
	i3BoundBox* bbox = node->GetBound();			// 미리 구한 로컬 AABB -> 카메라 로컬 OBB화시키면.. 될래나..
	VEC3D	nodeCenter = *bbox->getCenter();
	i3Vector::TransformCoord(&nodeCenter, &nodeCenter, &matWorldView);

	i3AlphaOBB& OBB = pNewSet->m_OBB;
	OBB.m_center = nodeCenter;
	
	REAL32		fSemiZDepth = 0.f;
	REAL32		semiLen;

	for (size_t j = 0 ; j < 3 ; ++j )			// 축별 루프로 처리..(코드가 길어져서 이렇게)
	{
		VEC3D* axisWV =	reinterpret_cast<VEC3D*>(&matWorldView.vec[j]);
	
		semiLen = bbox->GetMax()->vec[j] - bbox->GetMin()->vec[j];
		semiLen *= 0.5f;

		REAL32 axisLen = i3Vector::Length(axisWV);
		OBB.m_axis[j] = *axisWV;
		OBB.m_axis[j].x /= axisLen;
		OBB.m_axis[j].y /= axisLen;
		OBB.m_axis[j].z /= axisLen;
		OBB.m_extent.vec[j] = semiLen * axisLen;

		fSemiZDepth += fabs( axisWV->z * semiLen );
	}

	pNewSet->m_fMinDepth = nodeCenter.z - fSemiZDepth;
	pNewSet->m_fMaxDepth = nodeCenter.z + fSemiZDepth;
	
	pNewSet->m_pAlphaRoot = node;				// 시야 전향벡터와 내적이 음수값인 방향벡터 3개를 집어서 평면 구성해야함...


	i3SceneTracer::PushOutputList(pNewSet->m_NodeAttrList);		// AttrList는 이미 리셋되어있는 상태여야한다..
	
	pNewSet->m_startNodeAttrIndex =	m_pAttrStackManager->GatherCurrentAttrsForAlphaSort(pNewSet->m_NodeAttrList);

	m_pAlphaSortNodeSetList.push_back(pNewSet);

	m_bActivateAlphaSortNode = true;
}

void	i3SceneTracer::DeactivateAlphaSortNode( i3Node* node)
{
	if (!m_bActivateAlphaSortNode)
		return;
	
	i3AlphaSortNodeSet* pLastSet = m_pAlphaSortNodeSetList.back();

	if (node != pLastSet->m_pAlphaRoot)		// 루트의 자식노드의 경우라면 스킵...
		return;

	m_pAttrStackManager->ResetCurrentAttrsForAlphaSort(pLastSet->m_NodeAttrList, pLastSet->m_startNodeAttrIndex);

	i3SceneTracer::PopOutputList();
	
	m_bActivateAlphaSortNode = false;
}

// 리셋된 경우 AttrList는 리셋해두고...풀에다가 반납..
void	i3SceneTracer::ResetAlphaSortNodeList()
{
	m_pAlphaSortNoseSetListForDelete.insert(m_pAlphaSortNoseSetListForDelete.end(), 
		m_pAlphaSortNodeSetList.begin(), m_pAlphaSortNodeSetList.end());

	m_pAlphaSortNodeSetList.clear();
}

void	i3SceneTracer::ResetAlphaSortNodeListForDelete()
{
	const size_t numNode = m_pAlphaSortNoseSetListForDelete.size();
	for ( size_t i = 0 ; i < numNode ; ++i )
	{
		m_pAlphaSortNoseSetListForDelete[i]->m_pAlphaRoot = nullptr;
		m_pAlphaSortNoseSetListForDelete[i]->m_startNodeAttrIndex = 0;
		m_pAlphaSortNoseSetListForDelete[i]->m_NodeAttrList->Reset();
	}
	m_pAlphaSortNodeSetPool.insert(m_pAlphaSortNodeSetPool.end(), 
		m_pAlphaSortNoseSetListForDelete.begin(), m_pAlphaSortNoseSetListForDelete.end());

	m_pAlphaSortNoseSetListForDelete.clear();
}
