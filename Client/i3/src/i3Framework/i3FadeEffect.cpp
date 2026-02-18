#include "i3FrameworkPCH.h"
#include "i3FadeEffect.h"

I3_CLASS_INSTANCE( i3FadeEffect);

#define REAL_FADE_TIME_RATIO	0.75f

i3FadeEffect::~i3FadeEffect()
{
	I3_SAFE_RELEASE( m_pRoot );
}

bool i3FadeEffect::Create( i3Node *pParentNode )
{
	m_pRoot = i3AttrSet::new_object();

	pParentNode->AddChild( m_pRoot);

	// Lighting Enable = FALSE
	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

		pAttr->Set( false);
		m_pRoot->AddAttr( pAttr);
	}

	// Texture Mapping Enable = FALSE
	{
		i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();

		pAttr->Set( false);
		m_pRoot->AddAttr( pAttr);
	}

	// Color 
	{
		m_pFadeColorAttr = i3ColorAttr::new_object_ref();

		m_pRoot->AddAttr( m_pFadeColorAttr);
	}

	// Blend Enable Attr
	{
		i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();

		pAttr->Set( true);
		m_pRoot->AddAttr( pAttr);
	}

	// Blend Function Attr
	{
		i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

		pAttr->SetSource( I3G_BLEND_SRCALPHA);
		pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
		m_pRoot->AddAttr( pAttr);
	}

	// Projection Matrix Attr
	{
		i3ProjectMatrixAttr * pAttr = i3ProjectMatrixAttr::new_object_ref();
		MATRIX mtx;

		pAttr->SetMatrix( &mtx);
		m_pRoot->AddAttr( pAttr);
	}

	// View Matrix Attr
	{
		i3ViewMatrixAttr * pAttr = i3ViewMatrixAttr::new_object_ref();
		MATRIX mtx;

		pAttr->SetMatrix( &mtx);
		m_pRoot->AddAttr( pAttr);
	}

	{
		i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();

		pAttr->Set( I3G_FACE_CULL_NONE);
		m_pRoot->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
		pAttr->Set( false);
		m_pRoot->AddAttr( pAttr);
	}

	// Geometry
	{
		i3Geometry * pGeo = i3Geometry::new_object_ref();

		m_pGeoAttr = i3GeometryAttr::new_object_ref();

		pGeo->AppendGeometryAttr( m_pGeoAttr);

		m_pRoot->AddChild( pGeo);
	}

	_CreateVertex();

	SetEnable( false );

	return true;
}

void i3FadeEffect::_CreateVertex(void)
{
	VEC3D pos[6];
	i3VertexFormat format;
	i3VertexArray * pVA;

	format.SetHasPosition( true);

	pVA = i3VertexArray::new_object_ref();
	pVA->Create( &format, 6, 0);

	pVA->Lock();

	// Position
	i3Vector::Set( &pos[0], -1.0f,	1.0f,	0.0f);
	i3Vector::Set( &pos[1], -1.0f,	-1.0f,	0.0f);
	i3Vector::Set( &pos[2], 1.0f,	1.0f,	0.0f);
	i3Vector::Set( &pos[3], 1.0f,	1.0f,	0.0f);
	i3Vector::Set( &pos[4], -1.0f,	-1.0f,	0.0f);
	i3Vector::Set( &pos[5], 1.0f,	-1.0f,	0.0f);
	pVA->SetPositions( 0, 6, pos);

	pVA->Unlock();

	m_pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, 2, 0, false);
}


void i3FadeEffect::SetEnable( bool bTrue )
{
	m_bEnable = bTrue;
	if( m_bEnable )
	{
		m_pRoot->ActivateNode();
	}
	else
	{
		m_pRoot->DeactivateNode();
	}
}

void i3FadeEffect::Start( i3FadeEffectType FadeType, REAL32 rFadeTime, REAL32 rDarkenTime, bool bReusePrevFrame )
{
	SetEnable();

	m_bPendingDisable	= false;

	m_FadeType			= FadeType;

	m_rElapsedTime		= 0.0f;

	I3ASSERT( rFadeTime >= 0.0f);
	m_rFadeTime			= rFadeTime;
	m_rRealFadeTime		= i3Math::Mul(rFadeTime, REAL_FADE_TIME_RATIO);
	m_rOutTime			= rFadeTime - m_rRealFadeTime;
	m_rSlope			= ( m_rRealFadeTime > 0.0f) ? ( i3Math::div(1.0f, m_rRealFadeTime) ) : 0.0f;
	m_rRatio			= -1.0f;
	m_rDarkenTime		= rDarkenTime;

	switch( FadeType )
	{
	case I3_FADE_EFFECT_FADE_IN:
		{
			m_rTotalTime		= rFadeTime;
		}
		break;

	case I3_FADE_EFFECT_FADE_OUT:
		{
			m_rTotalTime		= rFadeTime + m_rDarkenTime;
		}
		break;

	case I3_FADE_EFFECT_FADE_OUT_IN:
		{
			m_rTotalTime		= i3Math::Mul( rFadeTime, 2.0f ) + m_rDarkenTime;
		}
		break;
	}

	// 한번 호출 해 준다.
	OnUpdate( 0.0f );
}


bool i3FadeEffect::OnUpdate( REAL32 rDeltaSeconds )
{
	if( ! m_bEnable )
	{
		return false;
	}

	m_rElapsedTime += rDeltaSeconds;
	if( m_rElapsedTime > m_rTotalTime )
	{
		SetPendingDisable();
		return false;
	}

	REAL32 r;

	switch( m_FadeType )
	{
	case I3_FADE_EFFECT_FADE_IN:
		{
			r = 1.0f - i3Math::Mul( m_rSlope, ( m_rElapsedTime - m_rOutTime ) );
		}
		break;

	case I3_FADE_EFFECT_FADE_OUT:
		{
			r = i3Math::Mul(m_rSlope, m_rElapsedTime);
		}
		break;

	case I3_FADE_EFFECT_FADE_OUT_IN:
		{
			if( m_rElapsedTime < m_rFadeTime )
			{	// Fade Out
				r = i3Math::Mul(m_rSlope, m_rElapsedTime);
			}
			else if( m_rElapsedTime < ( m_rFadeTime + m_rDarkenTime ) )
			{	// 완전히 검은 화면
				r = 1.0f;
			}
			else
			{	// Fade In
				REAL32 rTime = m_rElapsedTime - m_rFadeTime - m_rDarkenTime;
				r = 1.0f - i3Math::Mul(m_rSlope, (rTime - m_rOutTime ) );
			}
		}
		break;

	default:
		{
			I3ASSERT_0;
			r = 0.0f;
		}
		break;
	}

	r = i3Math::Min( i3Math::Max( 0.0f, r ), 1.0f );
	if( r != m_rRatio )
	{
		m_rRatio = r;

		
		COLOR Color;
		i3Color::Set( &Color, 0, 0, 0, (UINT8)i3Math::Mul(r, 255) );
		m_pFadeColorAttr->Set( &Color);
	}

	return true;
}

void i3FadeEffect::OnDestroy(void)
{
	if( m_pGeoAttr != nullptr)
	{
		m_pGeoAttr->SetVertexArray( nullptr);
	}
}

void i3FadeEffect::OnRevive(void)
{
	_CreateVertex();
}

