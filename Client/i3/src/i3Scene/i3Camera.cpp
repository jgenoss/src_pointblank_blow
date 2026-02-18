#include "i3SceneDef.h"
#include "i3Math.h"
#include "i3Camera.h"
#include "i3SceneTracer.h"
#include "i3CommonNodeProcs.h"

//I3_CLASS_INSTANCE( i3Camera, i3Transform);
I3_CLASS_INSTANCE( i3Camera);


I3_TRACE_RESULT I3SG_NODE_PROC( i3Camera)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3Camera * pCamera = (i3Camera *) pNode;
	i3Camera * pOldCamera = nullptr;
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	bool bFlag = false;

	#if NODE_PROFILE
	START_PROFILE( 24);
	#endif

	if( (pCamera->getStyle() & I3_CAM_STYLE_SUB) != 0)
	{
		// Sub Camera다.!

		// Offscreen이 아닌 경우에는 Traversal을 수행하지 않아야 한다.
		if( pTracer->isState( I3SG_TRACER_STATE_OFFSCREEN) == false)
			return Result;
	}

	if( pCamera->GetEnable() && (pTracer->isState( I3SG_TRACER_STATE_GEN_SHADOW) == false))
	{
		if( pCamera->IsAnimated())
		{
			I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();

			pEnv->m_Time = pTracer->GetTime();
			pEnv->m_pIn = pCamera->GetMatrix();
			pEnv->m_pOut = pCamera->GetMatrix();
			pEnv->m_pModelViewMatrix = pStack->GetTop();

			pCamera->GetAnimatedMatrix( pEnv);
		}

		pStack->PushPre( pCamera->GetCacheMatrix(), pCamera->GetMatrix());

		pOldCamera = pTracer->getCurrentCamera();
		pTracer->SetCamera( pCamera);

		pStack->Pop();

		pCamera->AddFlag( I3_NODEFLAG_CACHEMATRIX);

		bFlag = true;
	}

	#if NODE_PROFILE
	END_PROFILE( 24);
	#endif

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}

	#if NODE_PROFILE
	START_PROFILE( 24);
	#endif

	// 2004.1.19
	// earring
	// version 1.4 update
	//
	// Scene-Graph에서 Camera의 설정을 Hierarchical 적용을 하지 않고,
	// Sequence 처리로 대체한다.
	// pStackManager->PopAttr( I3_ATTRID_PROJECT_MATRIX);

	if( bFlag)
	{
		//pStackManager->PopAttr( I3_ATTRID_MODEL_VIEW_MATRIX);
		//pStack->Pop();

		if( pTracer->IsStyle( I3SG_TRACER_STYLE_NO_VIEWMATRIX))
		{
			// View Matrix를 Matrix Stack에 Push해 둔다.
			pStack->PushPost( nullptr, pTracer->GetViewMatrix());
		}

		if( pCamera->getStyle() & I3_CAM_STYLE_SUB)
		{
			pTracer->RestoreCamera( pOldCamera);
		}
	}

	#if NODE_PROFILE
	END_PROFILE( 24);
	#endif

	return Result;
}

i3Camera::i3Camera(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3Camera));

	m_pProjectMatrixAttr = i3ProjectMatrixAttr::new_object();
	m_pViewMatrixAttr = i3ViewMatrixAttr::new_object();
}

i3Camera::~i3Camera(void)
{
	I3_SAFE_RELEASE( m_pProjectMatrixAttr);
	I3_SAFE_RELEASE( m_pViewMatrixAttr );
	I3_SAFE_RELEASE( m_pRTAttr);

	I3_SAFE_RELEASE( m_pClearAttr);
	I3_SAFE_RELEASE( m_pOutput);

	I3_SAFE_RELEASE( m_pSubCamSg);
}

void i3Camera::UpdateProjectionMatrix(void)
{
	switch( m_Mode)
	{
		case I3_CAMERA_MODE_PERSPECTIVE :
			SetPerspective( m_FOV, m_Width, m_Height, m_Near, m_Far);
			break;

		case I3_CAMERA_MODE_ORTHOGONAL :
			SetOrthogonal( m_Width, m_Height, m_Near, m_Far);
			break;
	}
}

bool i3Camera::CreateRenderTarget( REAL32 width, REAL32 height, I3G_IMAGE_FORMAT format, I3G_IMAGE_FORMAT depthFormat)
{
	I3_SAFE_RELEASE( m_pClearAttr);
	I3_SAFE_RELEASE( m_pOutput);
	I3_SAFE_RELEASE( m_pRTAttr);

	{
		m_pRTAttr = i3RenderTargetAttr::new_object();
		m_pRTAttr->CreateRenderTarget( (INT32) width, (INT32) height, format, depthFormat);
	}

	m_pClearAttr = i3ClearRenderTargetAttr::new_object();
	m_pClearAttr->SetAddClearMask( I3G_CLEAR_DEPTH);
	m_pClearAttr->SetRemoveClearMask( 0);

	m_pOutput = i3RenderAttrListAttr::new_object();

	m_nidxcurRT = 0;

	return true;
}

void i3Camera::SetMode( I3_CAMERA_MODE mode)
{
	m_Mode = mode;

	UpdateProjectionMatrix();
}

void i3Camera::SetClearColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{
	COLOR c;

	i3Color::Set( &c, (UINT8) r, g, b, a);

	m_pClearAttr->SetClearColor( &c);
}

void i3Camera::SetPerspective( REAL32 fov, REAL32 width, REAL32 height, REAL32 zNear, REAL32 zFar)
{
	REAL32 aspect;

	m_FOV = fov;
	m_Width = width;
	m_Height = height;
	m_Near = zNear;
	m_Far = zFar;

	m_Mode = I3_CAMERA_MODE_PERSPECTIVE;

	aspect = i3Math::div(width, height);

	i3Matrix::PerspectiveFOVRH( m_pProjectMatrixAttr->GetMatrix(), m_FOV, aspect, m_Near, m_Far);
}

void i3Camera::SetOrthogonal( REAL32 width, REAL32 height, REAL32 zNear, REAL32 zFar)
{
	m_Near = zNear;
	m_Far = zFar;
	m_Width = width;
	m_Height = height;

	m_Mode = I3_CAMERA_MODE_ORTHOGONAL;

	i3Matrix::OrthoRH( m_pProjectMatrixAttr->GetMatrix(), width, height, m_Near, m_Far);
}

void i3Camera::SetFOV( REAL32 val)
{
	m_FOV = val;

	UpdateProjectionMatrix();
}

void i3Camera::SetNearFar( REAL32 Near, REAL32 Far)
{
	m_Near = Near;
	m_Far = Far;

	UpdateProjectionMatrix();
}

void i3Camera::SetWindowSize( REAL32 width, REAL32 height)
{
	m_Width = width;
	m_Height = height;

	UpdateProjectionMatrix();
}

void i3Camera::setSubCamSg( i3Node * pNode)
{
	I3_REF_CHANGE( m_pSubCamSg, pNode);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED CAMERA_INFO_1_3
	{
		REAL32	m_FOV = 0.0f;
		REAL32	m_Near = 0.0f;
		REAL32	m_Far = 0.0f;
		REAL32	m_Width = 0.0f;
		REAL32	m_Height = 0.0f;
		UINT8	m_Mode = 0;
	};

	struct PACKED CAMERA_INFO_1_4
	{
		REAL32	m_FOV = 0.0f;
		REAL32	m_Near = 0.0f;
		REAL32	m_Far = 0.0f;
		REAL32	m_Width = 0.0f;
		REAL32	m_Height = 0.0f;
		UINT8	m_Mode = 0;
		UINT8	m_bEnable = 0;
	};

	struct PACKED CAMERA_INFO_1_7
	{
		REAL32	m_FOV = 0.0f;
		REAL32	m_Near = 0.0f;
		REAL32	m_Far = 0.0f;
		REAL32	m_Width = 0.0f;
		REAL32	m_Height = 0.0f;
		UINT8	m_Mode = 0;
		UINT8	m_bEnable = 0;
		REAL32	m_UpdateInterval = 0.0f;
		UINT32	pad[3] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32 i3Camera::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::CAMERA_INFO_1_7 Info;

	Result = i3Transform::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Info.m_FOV = GetFOV();
	Info.m_Near = GetNear();
	Info.m_Far = GetFar();
	Info.m_Width = GetWindowWidth();
	Info.m_Height = GetWindowHeight();
	Info.m_Mode = (UINT8) m_Mode;
	Info.m_bEnable = (UINT8) m_bEnable;
	Info.m_UpdateInterval = getUpdateInterval();

	Rc = pResFile->GetStream()->Write( &Info, sizeof(pack::CAMERA_INFO_1_7));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Camera::OnSave()", "Could not write camera informations.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

bool	i3Camera::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3Transform::OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	pXML->SetAttribute( "FOV", GetFOV());
	pXML->SetAttribute( "Near", GetNear());
	pXML->SetAttribute( "Far", GetFar());
	pXML->SetAttribute( "Width", GetWindowWidth());
	pXML->SetAttribute( "Height", GetWindowHeight());
	pXML->SetAttribute( "Mode",	 m_Mode);
	pXML->SetAttribute( "Enable", m_bEnable);
	pXML->SetAttribute( "UpdateInterval", getUpdateInterval());

	return true;
}


UINT32 i3Camera::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3Camera::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;

	Result = i3Transform::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	if((pResFile->GetVersionMajor() == 1))
	{
		if( pResFile->GetVersionMinor() >= 7)
		{
			pack::CAMERA_INFO_1_7 Info;

			Rc = pResFile->GetStream()->Read( &Info, sizeof(Info));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3Camera::OnLoad()", "Could not read camera informations.");
				return STREAM_ERR;
			}
			Result += Rc;

			m_FOV = Info.m_FOV;
			m_Near = Info.m_Near;
			m_Far = Info.m_Far;
			m_Width = Info.m_Width;
			m_Height = Info.m_Height;
			m_Mode = (I3_CAMERA_MODE) Info.m_Mode;
			m_bEnable = Info.m_bEnable != 0;
			m_UpdateInterval = Info.m_UpdateInterval;
		}
		else if( pResFile->GetVersionMinor() >= 4)
		{
			pack::CAMERA_INFO_1_4 Info;

			Rc = pResFile->GetStream()->Read( &Info, sizeof(Info));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3Camera::OnLoad()", "Could not read camera informations.");
				return STREAM_ERR;
			}
			Result += Rc;

			m_FOV = Info.m_FOV;
			m_Near = Info.m_Near;
			m_Far = Info.m_Far;
			m_Width = Info.m_Width;
			m_Height = Info.m_Height;
			m_Mode = (I3_CAMERA_MODE) Info.m_Mode;
			m_bEnable = Info.m_bEnable != 0;
		}
		else
		{
			pack::CAMERA_INFO_1_3 Info;

			Rc = pResFile->GetStream()->Read( &Info, sizeof(pack::CAMERA_INFO_1_3));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3Camera::OnLoad()", "Could not read camera informations.");
				return STREAM_ERR;
			}
			Result += Rc;

			m_FOV = Info.m_FOV;
			m_Near = Info.m_Near;
			m_Far = Info.m_Far;
			m_Width = Info.m_Width;
			m_Height = Info.m_Height;
			m_Mode = (I3_CAMERA_MODE) Info.m_Mode;
			m_bEnable = true;
		}
	}

	UpdateProjectionMatrix();

	return Result;
}

bool	i3Camera::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3Transform::OnLoadXML( pFile, pXML);
	if( rv == false)
		return false;

	pXML->getAttr( "FOV", &m_FOV);
	pXML->getAttr( "Near", &m_Near);
	pXML->getAttr( "Far", &m_Far);
	pXML->getAttr( "Width",  &m_Width);
	pXML->getAttr( "Height", &m_Height);
	pXML->getAttr( "Mode",	 (INT32 *) &m_Mode);
	pXML->getAttr( "Enable", &m_bEnable);
	pXML->getAttr( "UpdateInterval", &m_UpdateInterval);

	return true;
}

void i3Camera::CopyTo( i3ElementBase * pNode, I3_COPY_METHOD method)
{
	i3Camera * pCamera = (i3Camera *) pNode;

	i3Transform::CopyTo( pNode, method);

	pCamera->SetEnable( GetEnable());
	pCamera->SetMode( GetMode());

	switch( GetMode())
	{
		case I3_CAMERA_MODE_PERSPECTIVE :
			pCamera->SetPerspective( m_FOV, m_Width, m_Height, m_Near, m_Far);
			break;

		case I3_CAMERA_MODE_ORTHOGONAL :
			pCamera->SetOrthogonal( m_Width, m_Height, m_Near, m_Far);
			break;
	}
}
