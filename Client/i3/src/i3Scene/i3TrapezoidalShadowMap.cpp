#include "i3SceneDef.h"
#include "i3TrapezoidalShadowMap.h"
#include "i3AttrStackManager.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3TrapezoidalShadowMap, i3Node);

// ShadowMap 구현 (2 Pass)
//
// - Shadow Map RenderTarget으로 전환한다.
// - Shadow Map RenderTarget Clear
// - Light-Space에서 Caster를 Rendering한다. (NoLight, White, Depth, Z->R)
// - Receiver들을 Diffuse Render한다.
// - Receiveer들을 Shadow Map을 입혀 다시 Render한다. (No Diffuse)

#if 0
	#define	VLOG		i3VLog
#else
	#define VLOG		__noop
#endif


I3_TRACE_RESULT I3SG_NODE_PROC( i3TrapezoidalShadowMap)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3TrapezoidalShadowMap * pShadowMap = (i3TrapezoidalShadowMap *) pNode;
	
	pShadowMap->Apply( pTracer);

	return Result;	
}

i3TrapezoidalShadowMap::i3TrapezoidalShadowMap(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3TrapezoidalShadowMap));

	m_pReceiver = NULL;
	m_pCaster = NULL;
	m_pLight = NULL;

	i3Matrix::Identity( &m_mLightTx);

	m_pShadowRTAttr = NULL;
	m_pClearRTAttr = NULL;
	m_pCasterRenderListAttr = NULL;
	m_pCasterShaderAttr = NULL;

	m_pZTestEnableAttr = NULL;
	m_pFaceCullModeAttr = NULL;

	m_pLightViewMatrixAttr = NULL;
	m_pLightProjectMatrixAttr = NULL;
	m_pShadowMapEnableAttr = NULL;

	m_fFocusDist = -0.8f;

	m_fCamNear = 1.0f;
	m_fCamFar = 30.0f;

	i3Color::Set( &m_ShadowColor, 0.2f, 0.2f, 0.2f, 0.7f);

	m_Style = 0;
	m_State = 0;
}

i3TrapezoidalShadowMap::~i3TrapezoidalShadowMap(void)
{
	I3_SAFE_RELEASE( m_pReceiver);
	I3_SAFE_RELEASE( m_pCaster);
	I3_SAFE_RELEASE( m_pLight);
	I3_SAFE_RELEASE( m_pShadowRTAttr);
	I3_SAFE_RELEASE( m_pClearRTAttr);
	I3_SAFE_RELEASE( m_pCasterRenderListAttr);
	I3_SAFE_RELEASE( m_pCasterShaderAttr);
	I3_SAFE_RELEASE( m_pLightViewMatrixAttr);
	I3_SAFE_RELEASE( m_pLightProjectMatrixAttr);
	I3_SAFE_RELEASE( m_pShadowMapEnableAttr);
	I3_SAFE_RELEASE( m_pZTestEnableAttr);
	I3_SAFE_RELEASE( m_pFaceCullModeAttr);
}

void i3TrapezoidalShadowMap::setReceiver( i3Node * pNode)
{
	if( pNode != NULL)
		pNode->AddRef();

	I3_SAFE_RELEASE( m_pReceiver);
	m_pReceiver = pNode;
}

void i3TrapezoidalShadowMap::setCaster( i3Node * pNode)
{
	if( pNode != NULL)
		pNode->AddRef();

	I3_SAFE_RELEASE( m_pCaster);
	m_pCaster = pNode;
}

void i3TrapezoidalShadowMap::setLight( i3Light * pLight, REAL32 fCutoff, REAL32 fNear, REAL32 fFar)
{
	m_fCutOff = fCutoff;
	m_fNear = fNear;
	m_fFar = fFar;

	if( pLight != NULL)
		pLight->AddRef();

	I3_SAFE_RELEASE( m_pLight);
	m_pLight = pLight;

	switch( pLight->GetLightType())
	{
		case I3G_LIGHT_DIRECTIONAL :
			// Perspective를 사용하되, 원근감을 매우 미약한 수준으로 잡는다.
				#if 1
					i3Matrix::PerspectiveFOVRH( &m_mProject, I3_DEG2RAD( 45.0f), 1.0f, 
						m_fNear, m_fFar);
				#else
					i3Matrix::OrthoRH( &m_mProject, 
						2.0f, 2.0f, // m_pShadowRTAttr->GetWidth(), m_pShadowRTAttr->GetWidth(), 
						m_fNear, m_fFar);
				#endif
			break;

		case I3G_LIGHT_SPOT :
		case I3G_LIGHT_POINT :
			i3Matrix::PerspectiveFOVRH( &m_mProject, pLight->GetCutoffAngle(), 1.0f,
				m_fNear, pLight->GetRange());
			break;
	}

	removeState( I3SG_SHADOWMAP_STATE_CASTER_RENDERED);
}

void i3TrapezoidalShadowMap::SetLightTransform( MATRIX * pMtx)
{
	i3Matrix::Copy( &m_mLightTx, pMtx);

	i3Matrix::Inverse( &m_mInvLightTx, NULL, pMtx);

	removeState( I3SG_SHADOWMAP_STATE_CASTER_RENDERED);
}

bool i3TrapezoidalShadowMap::CreateCasterShader(void)
{
	i3ShaderCode * pCode;
	i3ShaderContext * pCtx;

	pCtx = i3ShaderContext::NewObjectRef();

	pCode = i3ShaderCode::NewObjectRef();
	pCode->addStyle( I3G_SHADER_STYLE_HLSL | I3G_SHADER_STYLE_EXTERNAL);
	pCode->SetName( "i3ShaderDefine.inl");
	pCtx->AddCode( pCode);

	pCode = i3ShaderCode::NewObjectRef();
	pCode->addStyle( I3G_SHADER_STYLE_HLSL | I3G_SHADER_STYLE_EXTERNAL);
	pCode->SetName( "i3ShadowMapCaster.fx");
	pCtx->AddCode( pCode);

	pCtx->SetVertexShaderName( "VertScene");
	pCtx->SetSkinVertexShaderName( "VertSkin");
	pCtx->SetPixelShaderName( "PS_i3ShadowMapCaster");

	pCtx->Compile( g_pRenderContext);

	m_pCasterShaderAttr = i3ShaderAttr::NewObject();
	m_pCasterShaderAttr->SetShaderContext( pCtx);

	return true;
}

void i3TrapezoidalShadowMap::Create( INT32 w, INT32 h)
{
	I3G_IMAGE_FORMAT fmt, dfmt;
	COLOR col;

	fmt = I3G_IMAGE_FORMAT_R_32F;
	dfmt = I3G_IMAGE_FORMAT_D24X8;

	m_pShadowRTAttr = i3RenderTargetAttr::NewObject();
	m_pShadowRTAttr->CreateRenderTarget( w, h, fmt, dfmt);

	i3Color::Set( &col, 0, 0, 0, 0);
	//i3Color::Set( &col, 255, 255, 255, 255);
	m_pClearRTAttr = i3ClearRenderTargetAttr::NewObject();
	m_pClearRTAttr->SetAddClearMask( I3G_CLEAR_COLOR | I3G_CLEAR_DEPTH );
	m_pClearRTAttr->SetClearZ( 1.0f);
	m_pClearRTAttr->SetClearColor( &col);

	m_pCasterRenderListAttr = i3RenderAttrListAttr::NewObject();

	CreateCasterShader();

	m_pLightViewMatrixAttr = i3ViewMatrixAttr::NewObject();
	m_pLightProjectMatrixAttr = i3ProjectMatrixAttr::NewObject();

	m_pShadowMapEnableAttr = i3ShadowMapEnableAttr::NewObject();
	m_pShadowMapEnableAttr->Set( TRUE);

	{
		m_pZTestEnableAttr = i3ZTestEnableAttr::NewObject();
		m_pZTestEnableAttr->Set( FALSE);

		m_pFaceCullModeAttr = i3FaceCullModeAttr::NewObject();
		m_pFaceCullModeAttr->Set( I3G_FACE_CULL_NONE);
	}
}

static INT32	s_EnableAttrTable[] =
{
	I3_ATTRID_GEOMETRY,
	I3_ATTRID_FACECULL_MODE,
	I3_ATTRID_VIEW_MATRIX,
	I3_ATTRID_PROJECT_MATRIX,
	I3_ATTRID_MODEL_VIEW_MATRIX,
	I3_ATTRID_BONEMATRIXLIST,
	I3_ATTRID_RENDER_ATTR_LIST,
	I3_ATTRID_QUAD,
	I3_ATTRID_SPRITE,
	I3_ATTRID_BILLBOARD,
	I3_ATTRID_TEXT,
	I3_ATTRID_SPRITE2D,
	I3_ATTRID_RENDERTARGET,
	I3_ATTRID_CLEARRENDERTARGET,
	I3_ATTRID_SHADER
};

static int s_Count = 0;

REAL32	i3TrapezoidalShadowMap::_GetDotWithAt( VEC3D * pV1, VEC3D * pV2, VEC3D * pV3)
{
	VEC3D line1, line2, lightAt, norm;

	i3Vector::Set( &lightAt, 0.0f, 0.0f, -1.0f);

	i3Vector::Sub( &line1,	pV2, pV1);
	i3Vector::Sub( &line2,	pV3, pV2);		//

	i3Vector::Cross( &norm, &line1, &line2);

	return i3Vector::Dot( &norm, &lightAt);
}

INT32 i3TrapezoidalShadowMap::_GetConvexHull( VEC3D * pvFrusta, VEC3D * pvE)
{
	// Post-Perspective Light-Space에서의 Eye-Frustum에서
	// 실제로 보여지는(Front-Faced) 면이 실제로 2D Convex Hull(E)를
	// 구성하는 Vertex를 제공한다.
	//
	// Implementation : 
	//		6개 Plane의 바깥 Normal과 Light-Space의 At direction과의 Dot로
	//		Light-View에서 보여지는 Plane을 찾고, 보여진 Plaen의 Vertex들에 대한
	//		Used-Count를 누적시킨다.
	//		만약 (1 <= Used-Count <= 2)라면 2D Convex Hull E에 포함되는 Vertex들이다.
	REAL32 dot;
	INT32	UsedCount[8], idx, i;

	for( i = 0; i < 8; i++)		UsedCount[i] = 0;

	// Top & Bottom Plane
	{
		dot = _GetDotWithAt( &pvFrusta[4], &pvFrusta[0], &pvFrusta[1]);		// Top Plane's Vertex (v4, v0, v1)
		if( dot < 0.0f)
		{
			// Top Plane이 보여지고 있다.
			UsedCount[0]++;		UsedCount[1]++;
			UsedCount[4]++;		UsedCount[5]++;
		}

		dot = _GetDotWithAt( &pvFrusta[7], &pvFrusta[3], &pvFrusta[2]);		// Bottom Plane's Vertex (v7, v3, v2)
		if( dot < 0.0f)
		{
			// Bottom Plane이 보여지고 있다.
			UsedCount[2]++;		UsedCount[3]++;
			UsedCount[6]++;		UsedCount[7]++;
		}
	}
	
	// Left & Right Plane
	{
		dot = _GetDotWithAt( &pvFrusta[4], &pvFrusta[6], &pvFrusta[2]);		// Left Plane's vertex ( v4, v6, v2)
		if( dot < 0.0f)
		{
			// Left Plane이 보여지고 있다.
			UsedCount[0]++;		UsedCount[2]++;
			UsedCount[4]++;		UsedCount[6]++;
		}

		dot = _GetDotWithAt( &pvFrusta[1], &pvFrusta[3], &pvFrusta[7]);		// Right Plane's vertex ( v1, v3, v7)
		if( dot < 0.0f)
		{
			// Right Plane이 보여지고 있다.
			UsedCount[1]++;		UsedCount[3]++;
			UsedCount[5]++;		UsedCount[7]++;
		}
	}

	// Near & Far Plane
	{
		dot = _GetDotWithAt( &pvFrusta[0], &pvFrusta[2], &pvFrusta[3]);		// Near Plane's vertex ( v0, v2, v3)
		if( dot < 0.0f)
		{
			// Near Plane이 보여지고 있다.
			UsedCount[0]++;		UsedCount[1]++;
			UsedCount[2]++;		UsedCount[3]++;
		}

		dot = _GetDotWithAt( &pvFrusta[5], &pvFrusta[7], &pvFrusta[6]);		// Far Plane's vertex ( v5, v7, v6)
		if( dot < 0.0f)
		{
			// Far Plane이 보여지고 있다.
			UsedCount[4]++;		UsedCount[5]++;
			UsedCount[6]++;		UsedCount[7]++;
		}
	}

	// UsedCount가 구해졌기 때문에 2D Convex Hull(E)를 구할 수 있다.
	for( i = 0, idx = 0; i < 8; i++)
	{
		if( (UsedCount[i] > 0) && (UsedCount[i] <= 2))
		{
			i3Vector::Copy( &pvE[idx], &pvFrusta[i]);
			//i3Vector::SetZ( &pvE[idx], 0.0f);				// 2D Hull로 구성
			
			idx++;
			if( idx >= 6)
				break;
		}
	}

	return idx;
}

int _DetermineSide( VEC3D * pQ, VEC3D * pP1, VEC3D * pP2)
{
    REAL32 Temp1, Temp2;

    Temp1 = (pQ->x - pP1->x) * (pP2->y - pP1->y);
	Temp2 = (pQ->y - pP1->y) * (pP2->x - pP1->x);

    if( Temp1 > Temp2)
        return -1;
    else if( Temp1 < Temp2)
        return 1;

    return 0;
}

#define ASSIGN_MAT(M, u0, u3, u6, u1, u4, u7, u2, u5, u8) { \
	M[0][0] = u0; M[0][1] = u3; M[0][2] = u6; \
	M[1][0] = u1; M[1][1] = u4; M[1][2] = u7; \
	M[2][0] = u2; M[2][1] = u5; M[2][2] = u8; \
	}

#define DET2(a, b, c, d) ((a) * (d) - (b) * (c))

#define DOT2(u, v) (u[0] * v[0] + u[1] * v[1])

void intersect(float i[2], float g0[3], float g1[3], float h0[3], float h1[3]) 
{
	float a, b;

	i[0] = i[1] = 1.0f / DET2(g0[0] - g1[0], g0[1] - g1[1], h0[0] - h1[0], h0[1] - h1[1]);

	a = DET2(g0[0], g0[1], g1[0], g1[1]);
	b = DET2(h0[0], h0[1], h1[0], h1[1]);

	i[0] *=	DET2(a, g0[0] - g1[0], b, h0[0] - h1[0]);
	i[1] *=	DET2(a, g0[1] - g1[1], b, h0[1] - h1[1]);
}

void map_Trapezoid_To_Square(float TR[][3], float t0[3], float t1[3], float t2[3], float t3[3]) 
{
	float i[2], a, b, c, d;

	//M1 = R * T1
	a = 0.5f * (t2[0] - t3[0]);
	b = 0.5f * (t2[1] - t3[1]);

	ASSIGN_MAT(TR, a  ,  b  , a * a + b * b,
	               b  , -a  , a * b - b * a,
	              0.0f, 0.0f, 1.0f);

	//M2 = T2 * M1 = T2 * R * T1
	intersect(i, t0, t3, t1, t2);

	TR[0][2] = -DOT2(TR[0], i);
	TR[1][2] = -DOT2(TR[1], i);

	//M1 = H * M2 = H * T2 * R * T1
	a = DOT2(TR[0], t2) + TR[0][2];
	b = DOT2(TR[1], t2) + TR[1][2];
	c = DOT2(TR[0], t3) + TR[0][2];
	d = DOT2(TR[1], t3) + TR[1][2];

	a = -(a + c) / (b + d);

	TR[0][0] += TR[1][0] * a;
	TR[0][1] += TR[1][1] * a;
	TR[0][2] += TR[1][2] * a;

	//M2 = S1 * M1 = S1 * H * T2 * R * T1
	a = 1.0f / (DOT2(TR[0], t2) + TR[0][2]);
	b = 1.0f / (DOT2(TR[1], t2) + TR[1][2]);

	TR[0][0] *= a; TR[0][1] *= a; TR[0][2] *= a;
	TR[1][0] *= b; TR[1][1] *= b; TR[1][2] *= b;

	//M1 = N * M2 = N * S1 * H * T2 * R * T1
	TR[2][0] = TR[1][0]; TR[2][1] = TR[1][1]; TR[2][2] = TR[1][2];
	TR[1][2] += 1.0f;

	//M2 = T3 * M1 = T3 * N * S1 * H * T2 * R * T1
	a = DOT2(TR[1], t0) + TR[1][2];
	b = DOT2(TR[2], t0) + TR[2][2];
	c = DOT2(TR[1], t2) + TR[1][2];
	d = DOT2(TR[2], t2) + TR[2][2];

	a = -0.5f * (a / b + c / d);

	TR[1][0] += TR[2][0] * a;
	TR[1][1] += TR[2][1] * a;
	TR[1][2] += TR[2][2] * a;

	//M1 = S2 * M2 = S2 * T3 * N * S1 * H * T2 * R * T1
	a = DOT2(TR[1], t0) + TR[1][2];
	b = DOT2(TR[2], t0) + TR[2][2];

	c = -b / a;

	TR[1][0] *= c; TR[1][1] *= c; TR[1][2] *= c;
}

void i3TrapezoidalShadowMap::_GetTPlaneVertex( VEC3D * vFrusta, VEC3D * vE, INT32 vE_Count, VEC3D * vT)
{
	VEC3D	lStart, lEnd, lDir;
	VEC3D	vTop, vBottom;
	REAL32	lLength;
	VEC3D	vTemp;
	INT32 i;

	// 2. Center Line l(lStart -> lEnd)을 구한다.
	//		lStart	: Near Clip Plane의 중앙점
	//		lEnd	: Far Clip Plane의 중앙점
	{
		// (v0 <-> v3) -> lStart
		i3Vector::Add( &lStart, &vFrusta[0], &vFrusta[1]);
		i3Vector::Add( &lStart, &lStart, &vFrusta[2]);
		i3Vector::Add( &lStart, &lStart, &vFrusta[3]);
		i3Vector::Scale( &lStart, &lStart, 0.25f);
		//i3Vector::SetZ( &lStart, 0.0f);

		// (v4 <-> v7) -> lEnd
		i3Vector::Add( &lEnd, &vFrusta[4], &vFrusta[5]);
		i3Vector::Add( &lEnd, &lEnd, &vFrusta[6]);
		i3Vector::Add( &lEnd, &lEnd, &vFrusta[7]);
		i3Vector::Scale( &lEnd, &lEnd, 0.25f);
		//i3Vector::SetZ( &lEnd, 0.0f);

		// Direction & Length
		i3Vector::Sub( &lDir, &lEnd, &lStart);
		lLength = i3Vector::Length( &lDir);
		i3Vector::Scale( &lDir, &lDir, 1.0f / lLength);

		VLOG( "c 255 0 0");
		VLOG( "m %f %f %f", i3Vector::GetX( &lStart), i3Vector::GetY( &lStart),	i3Vector::GetZ( &lStart));
		VLOG( "l %f %f %f", i3Vector::GetX( &lEnd),	i3Vector::GetY( &lEnd),		i3Vector::GetZ( &lEnd));
	}

	// 4. Center Line l과 수직하고 Convex Hull E에 닿는 두 점을 찾는다.
	// 이 중, Near Plane과 가까운 부분을 Top Line Lt,
	// Far Plane과 가까운 부분을 Base Line Lb를 찾는다.
	{
		// Implementation :
		//		Convex Hull E의 각 Vertex과 L(lStart, lEnd)와의 Perpendicular Point를 계산한다.
		//		Perpendicular Point는 L과 각 E Vertex의 거리를 구하는 것으로,
		//		L 선상에서의 길이 t 값으로 Near 및 Far와 가장 근접한 점들을 구할 수 있다.
		//		실제 필요한 값은 정확하게 Line이 아니라 Length T의 두 개 값이다.
		REAL32	minL = 0.0f, maxL = 0.0f, t;
		VEC3D p;

		for( i = 0; i < vE_Count; i++)
		{
			// Center Line(L)과 각 Vertex의 Intersect가 실제로 있는지에 대해서는
			// 그다지 중요하지 않다.
			i3Math::GetPerpendicularPoint( &lStart, &lEnd, &vE[i], &p, &t);

			if( i == 0)
			{
				minL = maxL = t;
			}
			else
			{
				minL = MIN( minL, t);
				maxL = MAX( maxL, t);
			}
		}

		i3Vector::Sub( &vTemp, &lEnd, &lStart);

		i3Vector::Scale( &vTop, &vTemp, minL);
		i3Vector::Add( &vTop, &vTop, &lStart);

		i3Vector::Scale( &vBottom, &vTemp, maxL);
		i3Vector::Add( &vBottom, &vBottom, &lStart);

		VLOG( "c 0 255 0");
		VLOG( "r %f %f %f", i3Vector::GetX( &vTop), i3Vector::GetY( &vTop), i3Vector::GetZ( &vTop));
		VLOG( "r %f %f %f", i3Vector::GetX( &vBottom), i3Vector::GetY( &vBottom), i3Vector::GetZ( &vBottom));
	}

	// Side Line을 찾는다.
	{
		REAL32 distTP, distQ;
		VEC3D vQ, dirSide1, dirSide2;
		REAL32 minDot = 0.0f, maxDot = 0.0f, dot;
		INT32 idxMin = 0, idxMax = 0, side;

		distTP = lLength * (1.0f + (1.0f - m_fFocusDist));

		distQ = (lLength * distTP) + (lLength * distTP * m_fFocusDist);
		distQ = distQ / (lLength - (distTP * 2) - (lLength * m_fFocusDist));

		i3Vector::Scale( &vQ, &lDir, distQ);			// Q점의 선정
		i3Vector::Add( &vQ, &vQ, &lStart);

		VLOG( "e %f %f %f", i3Vector::GetX( &vQ), i3Vector::GetY( &vQ), i3Vector::GetZ( &vQ));


		// Convex Hull(E)의 Vertex들과 Q와의 Direction Vector와 Center Line(L)과의 Dot가 
		// 가장 변위가 크고 작은 두개 점을 구한다.
		// 이 두 점과 Q점을 이은 선이 Trapezoid의 Side Line이 된다.
		for( i = 0; i < vE_Count; i++)
		{
			i3Vector::Sub( &vTemp, &vE[i], &vQ);
			i3Vector::Normalize( &vTemp, &vTemp);

			dot = i3Vector::Dot( &vTemp, &lDir);

			if( i == 0)
			{
				minDot = maxDot = dot;
			}
			else
			{
				side = _DetermineSide( &vE[i], &lStart, &lEnd);

				if( side > 0)
				{
					if( maxDot > i3Math::abs( dot))
					{
						idxMax = i;
						maxDot = i3Math::abs( dot);
					}
				}
				else
				{
					if( minDot > i3Math::abs( dot))
					{
						idxMin = i;
						minDot = i3Math::abs( dot);
					}
				}
			}
		}

		// Side Line들의 방향을 구한다.
		{
			i3Vector::Sub( &dirSide1, &vE[ idxMin], &vQ);
			i3Vector::Normalize( &dirSide1, &dirSide1);

			i3Vector::Sub( &dirSide2, &vE[ idxMax], &vQ);
			i3Vector::Normalize( &dirSide2, &dirSide2);
		}

		// 실제적인 Trapezoid를 구성하는 vT[4]의 계산
		// 각 점들은 모두 vQ를 시작점으로 하고 dirSide1 방향으로 2점,
		// 그리고 dirSide2 방향으로 2점이 선택되어야 한다.
		{
			REAL32 rad1, rad2, tan1, tan2, distT, distB;

			rad1 = acosf( minDot);
			rad2 = acosf( maxDot);

			tan1 = 1.0f / i3Math::cos( rad1);
			tan2 = 1.0f / i3Math::cos( rad2);

			i3Vector::Sub( &vTemp, &vQ, &vTop);
			distT = i3Vector::Length( &vTemp);

			i3Vector::Sub( &vTemp, &vQ, &vBottom);
			distB = i3Vector::Length( &vTemp);

			i3Vector::Scale( &vT[0], &dirSide1, tan1 * distB);
			i3Vector::Add( &vT[0], &vT[0], &vQ);

			i3Vector::Scale( &vT[3], &dirSide1, tan1 * distT);
			i3Vector::Add( &vT[3], &vT[3], &vQ);
			
			i3Vector::Scale( &vT[1], &dirSide2, tan2 * distB);
			i3Vector::Add( &vT[1], &vT[1], &vQ);

			i3Vector::Scale( &vT[2], &dirSide2, tan2 * distT);
			i3Vector::Add( &vT[2], &vT[2], &vQ);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////
//
void i3TrapezoidalShadowMap::CalcPostPerspectiveMatrix( i3SceneTracer * pTracer)
{
	VEC3D	vFrusta[8], vOrigin[8];
	VEC3D	vE[6];
	VEC3D	vT[4];
	UINT32	vE_Count, i;
	MATRIX		mTemp, mTN;
	
	VLOG( "s ");

	// Base Line과 Top Line을 찾는다.
	{
		// 1. Camera의 Frustum을 Post-Perpsective Light Space로 Transform 한다.
		{
			// Object-Space에서의 Eye-Frustum의 8개 Vertex를 만든다. (Camera-Projection을 반영한다.)
			i3Vector::Set( &vOrigin[0], -1.0f, 	 1.0f,	0.0f);
			i3Vector::Set( &vOrigin[1],	 1.0f,	 1.0f,	0.0f);
			i3Vector::Set( &vOrigin[2],	-1.0f,	-1.0f,	0.0f);
			i3Vector::Set( &vOrigin[3],	 1.0f,	-1.0f,	0.0f);
														
			i3Vector::Set( &vOrigin[4], -1.0f, 	 1.0f,	1.0f);
			i3Vector::Set( &vOrigin[5],	 1.0f,	 1.0f,	1.0f);
			i3Vector::Set( &vOrigin[6],	-1.0f,	-1.0f,	1.0f);
			i3Vector::Set( &vOrigin[7],	 1.0f,	-1.0f,	1.0f);

			VLOG( "c 92 92 92");
			VLOG( "m %f %f %f", i3Vector::GetX( &vOrigin[0]), i3Vector::GetY( &vOrigin[0]), i3Vector::GetZ( &vOrigin[0]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vOrigin[1]), i3Vector::GetY( &vOrigin[1]), i3Vector::GetZ( &vOrigin[1]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vOrigin[3]), i3Vector::GetY( &vOrigin[3]), i3Vector::GetZ( &vOrigin[3]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vOrigin[2]), i3Vector::GetY( &vOrigin[2]), i3Vector::GetZ( &vOrigin[2]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vOrigin[0]), i3Vector::GetY( &vOrigin[0]), i3Vector::GetZ( &vOrigin[0]));
																									   
			VLOG( "m %f %f %f", i3Vector::GetX( &vOrigin[4]), i3Vector::GetY( &vOrigin[4]), i3Vector::GetZ( &vOrigin[4]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vOrigin[5]), i3Vector::GetY( &vOrigin[5]), i3Vector::GetZ( &vOrigin[5]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vOrigin[7]), i3Vector::GetY( &vOrigin[7]), i3Vector::GetZ( &vOrigin[7]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vOrigin[6]), i3Vector::GetY( &vOrigin[6]), i3Vector::GetZ( &vOrigin[6]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vOrigin[4]), i3Vector::GetY( &vOrigin[4]), i3Vector::GetZ( &vOrigin[4]));

			// Post-Perspective Light space로의 Transform Matrix를 생성
			// = InvCam * Lv * Lp
			i3Matrix::Inverse( &mTemp, NULL, pTracer->getViewProjectionMatrix());
			i3Matrix::Mul( &mTemp, &mTemp, &m_mInvLightTx);
			i3Matrix::Mul( &mTemp, &mTemp, &m_mProject);

			// 8개 Vertex를 PPS로의 Transform
			i3Matrix::TransformPoints( vFrusta, vOrigin, 8, &mTemp);

			/*
			{
				REAL32 minZ = 0.0f, maxZ = 0.0f;

				for( i = 0; i < 8; i++)
				{
					if( i == 0)
					{
						minZ = maxZ = i3Math::abs( i3Vector::GetZ(&vFrusta[i]));
					}
					else
					{
						minZ = MIN( minZ, i3Math::abs( i3Vector::GetZ( &vFrusta[i])));
						maxZ = MAX( maxZ, i3Math::abs( i3Vector::GetZ( &vFrusta[i])));
					}
				}

				// 검색된 minZ와 maxZ로 새로운 Projection Matrix를 만든다.
				//i3Matrix::PerspectiveFOVRH( &mProj, I3_DEG2RAD(45.0f), 1.0f, minZ, maxZ);
				i3Matrix::PerspectiveRH( &mProj, m_pShadowRTAttr->GetWidth(), m_pShadowRTAttr->GetHeight(), minZ, maxZ);

				i3Matrix::Mul( &mTemp, &mTemp, &mProj);

				i3Matrix::TransformPoints( vFrusta, vOrigin, 8, &mTemp);
			}
			*/

			

			VLOG( "m %f %f %f", i3Vector::GetX( &vFrusta[0]), i3Vector::GetY( &vFrusta[0]), i3Vector::GetZ( &vFrusta[0]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[1]), i3Vector::GetY( &vFrusta[1]), i3Vector::GetZ( &vFrusta[1]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[3]), i3Vector::GetY( &vFrusta[3]), i3Vector::GetZ( &vFrusta[3]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[2]), i3Vector::GetY( &vFrusta[2]), i3Vector::GetZ( &vFrusta[2]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[0]), i3Vector::GetY( &vFrusta[0]), i3Vector::GetZ( &vFrusta[0]));
																									   
			VLOG( "m %f %f %f", i3Vector::GetX( &vFrusta[4]), i3Vector::GetY( &vFrusta[4]), i3Vector::GetZ( &vFrusta[4]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[5]), i3Vector::GetY( &vFrusta[5]), i3Vector::GetZ( &vFrusta[5]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[7]), i3Vector::GetY( &vFrusta[7]), i3Vector::GetZ( &vFrusta[7]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[6]), i3Vector::GetY( &vFrusta[6]), i3Vector::GetZ( &vFrusta[6]));
			VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[4]), i3Vector::GetY( &vFrusta[4]), i3Vector::GetZ( &vFrusta[4]));

			for( i = 0; i < 4; i++)
			{
				VLOG( "m %f %f %f", i3Vector::GetX( &vFrusta[i]),		i3Vector::GetY( &vFrusta[i]),		i3Vector::GetZ( &vFrusta[i]));
				VLOG( "l %f %f %f", i3Vector::GetX( &vFrusta[i + 4]), i3Vector::GetY( &vFrusta[i + 4]),	i3Vector::GetZ( &vFrusta[i + 4]));
			}
		}

		// 3. Light View에서의 2D Convex Hull Vertex들을 찾는다.
		//		(>= 6, <= 4 개의 Vertex가 찾아질 수 있다.)
		{
			vE_Count = _GetConvexHull( vFrusta, vE);
		}
	}

	// 만약 Convex Hull Vertex의 개수가 4개 이하라면
	// 사실 상, Trapezoidal 원래의 방법으로는 T_N Plane을 찾을 수 없기 때문에
	// Far Plane 자체를 직접 T_N Plane으로 간주한다.
	if( vE_Count > 4)
	{
		_GetTPlaneVertex( vFrusta, vE, vE_Count, vT);
	}
	else
	{
		i3Vector::Copy( &vT[0],		&vFrusta[5]);
		i3Vector::Copy( &vT[1],		&vFrusta[4]);
		i3Vector::Copy( &vT[2],		&vFrusta[6]);
		i3Vector::Copy( &vT[3],		&vFrusta[7]);
	}

	
	{
		VLOG( "c 255 128 0");
		VLOG( "m %f %f %f", i3Vector::GetX( &vT[0]), i3Vector::GetY( &vT[0]), i3Vector::GetZ( &vT[0]));
		VLOG( "l %f %f %f", i3Vector::GetX( &vT[1]), i3Vector::GetY( &vT[1]), i3Vector::GetZ( &vT[1]));
		VLOG( "l %f %f %f", i3Vector::GetX( &vT[2]), i3Vector::GetY( &vT[2]), i3Vector::GetZ( &vT[2]));
		VLOG( "l %f %f %f", i3Vector::GetX( &vT[3]), i3Vector::GetY( &vT[3]), i3Vector::GetZ( &vT[3]));
		VLOG( "l %f %f %f", i3Vector::GetX( &vT[0]), i3Vector::GetY( &vT[0]), i3Vector::GetZ( &vT[0]));
	}

	{
		float TR[3][3];
		float t0[3], t1[3], t2[3], t3[3];

		t0[0] = i3Vector::GetX( &vT[0]);
		t0[1] = i3Vector::GetY( &vT[0]);
		t0[2] = 0.0f;	//i3Vector::GetZ( &vT[0]);

		t1[0] = i3Vector::GetX( &vT[1]);
		t1[1] = i3Vector::GetY( &vT[1]);
		t1[2] = 0.0f;	//i3Vector::GetZ( &vT[1]);

		t2[0] = i3Vector::GetX( &vT[2]);
		t2[1] = i3Vector::GetY( &vT[2]);
		t2[2] = 0.0f;	//i3Vector::GetZ( &vT[2]);

		t3[0] = i3Vector::GetX( &vT[3]);
		t3[1] = i3Vector::GetY( &vT[3]);
		t3[2] = 0.0f;	//i3Vector::GetZ( &vT[3]);

		VLOG( "t %f %f %f T0", t0[0], t0[1], t0[2]);
		VLOG( "t %f %f %f T1", t1[0], t1[1], t1[2]);
		VLOG( "t %f %f %f T2", t2[0], t2[1], t2[2]);
		VLOG( "t %f %f %f T3", t3[0], t3[1], t3[2]);

		map_Trapezoid_To_Square( TR, t0, t1, t2, t3);


		#if 0
			mTN.m00 = TR[0][0];	mTN.m01 = TR[0][1];	mTN.m02 =  0.0f;	mTN.m03 = TR[0][2];
			mTN.m10 = TR[1][0];	mTN.m11 = TR[1][1];	mTN.m12 =  0.0f;	mTN.m13 = TR[1][2];
			mTN.m20 =     0.0f;	mTN.m21 =     0.0f;	mTN.m22 =  1.0f;	mTN.m23 =     0.0f;
			mTN.m30 = TR[2][0];	mTN.m31 = TR[2][1];	mTN.m32 =  0.0f;	mTN.m33 = TR[2][2];
		#else
			mTN.m00 = TR[0][0];	mTN.m10 = TR[0][1];	mTN.m20 =  0.0f;	mTN.m30 = TR[0][2];
			mTN.m01 = TR[1][0];	mTN.m11 = TR[1][1];	mTN.m21 =  0.0f;	mTN.m31 = TR[1][2];
			mTN.m02 =     0.0f;	mTN.m12 =     0.0f;	mTN.m22 =  1.0f;	mTN.m32 =     0.0f;
			mTN.m03 = TR[2][0];	mTN.m13 = TR[2][1];	mTN.m23 =  0.0f;	mTN.m33 = TR[2][2];
		#endif

		m_pLightViewMatrixAttr->SetMatrix( &m_mInvLightTx);
		m_pLightProjectMatrixAttr->SetMatrix( &m_mProject);
	}

	VLOG( "u");

	{
		//MATRIX mtxV2L;
		REAL32 fMapSz, fCutoff;
		MATRIX mTemp2;

		fCutoff = i3Math::cos( m_fCutOff);
		fMapSz = (REAL32) m_pShadowRTAttr->GetWidth();

		i3Matrix::Mul( &mTemp2, pTracer->GetInverseViewMatrix(), &m_mInvLightTx);
		i3Matrix::Mul( &mTemp2, &mTemp2, &m_mProject);

		pTracer->GetRenderContext()->AddShadowMap( m_pShadowRTAttr->GetColorRenderTarget(), 
			&mTemp2, &mTN,
			NULL, NULL,
			&m_ShadowColor, fMapSz, fCutoff);
	}
}

void i3TrapezoidalShadowMap::Apply( i3SceneTracer * pTracer)
{
	i3AttrStackManager * pManager = pTracer->GetAttrStackManager();
	I3SG_TRACER_CONTEXT ctx;

	CalcPostPerspectiveMatrix( pTracer);

	// Caster에 대한 Output을 구해온다.
	if( m_pShadowMapEnableAttr->Get())
	{
		// Scene Graph를 Trace하기 위한 준비
		pTracer->SaveContext( &ctx);
		pTracer->SetViewMatrix( m_pLightViewMatrixAttr->GetMatrix(), &m_mLightTx);
		pTracer->SetProjectionMatrix( m_pLightProjectMatrixAttr->GetMatrix());
		pTracer->GetModelViewMatrixStack()->PushIdentity();

		pTracer->PushOutputList( m_pCasterRenderListAttr);
		pTracer->SetTime( 0.0f);

		pManager->FlushPushedAttrs( pTracer->GetOutput());

		pManager->PushAttr( m_pLightViewMatrixAttr);
		pManager->PushAttr( m_pLightProjectMatrixAttr);
		pManager->PushAttr( m_pFaceCullModeAttr);
		pManager->PushAttr( m_pCasterShaderAttr, 0);		// 다른 Shader가 동작하지 못하도록...
		pManager->PushAttr( m_pShadowRTAttr);
		pManager->PushAttr( m_pClearRTAttr);

		// Dynamic Caster의 Render
		if( m_pCaster != NULL)
		{
			pTracer->Trace( m_pCaster);
		}

		// Tracer의 Restore
		pManager->PopAttr( m_pClearRTAttr->GetID());
		pManager->PopAttr( m_pShadowRTAttr->GetID());
		pManager->PopAttr( m_pFaceCullModeAttr->GetID());
		pManager->PopAttr( m_pCasterShaderAttr->GetID(), 0);		// 다른 Shader가 동작하지 못하도록...
		pManager->PopAttr( m_pLightViewMatrixAttr->GetID());
		pManager->PopAttr( m_pLightProjectMatrixAttr->GetID());

		pTracer->RestoreContext( &ctx);
		pTracer->PopOutputList();

		pTracer->AddAttributeToOutput( m_pCasterRenderListAttr);

		pTracer->GetModelViewMatrixStack()->Pop();
	}

	

	// Receiver의 Diffuse Render
	{
		pManager->FlushPushedAttrs( pTracer->GetOutput());
		pManager->PushAttr( m_pShadowMapEnableAttr);

		pTracer->Trace( m_pReceiver);

		pManager->PopAttr( m_pShadowMapEnableAttr->GetID());

		pManager->FlushPushedAttrs( pTracer->GetOutput());

		//pTracer->GetOutput()->Dump();
	}

	// Update Shader Params
}

void i3TrapezoidalShadowMap::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
}

void i3TrapezoidalShadowMap::OnBuildObjectList( i3List * pList)
{
}

UINT32 i3TrapezoidalShadowMap::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3TrapezoidalShadowMap::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}
