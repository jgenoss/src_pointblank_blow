#include "stdafx.h"
#include "i3LevelElement3D.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelViewport.h"
#include "i3LevelPrimitive_Box.h"

#include "NXU_helper.h"
#include "NXU_schema.h"
#include "NXU_cooking.h"
#include "NXU_streaming.h"
#include "PhysXLoader.h"

#include "i3LevelResPrim.h"

//#define DO_VLOG

I3_CLASS_INSTANCE( i3LevelElement3D);

#define MAX_ALPHA	255
#define MIN_ALPHA	128
#define ALPHA_STEP	3

#define SEL_R	200
#define SEL_G	32
#define SEL_B	32
#define SEL_A	96

#define NORM_R	105
#define NORM_G	151
#define	NORM_B	30
#define	NORM_A	96

#define LEAD		(1.0f / 3.0f)

static REAL32 s_SelBoxVertex[32][3] =
{
	{	-0.5f,			-0.5f,			-0.5f},
	{	-0.5f + LEAD,	-0.5f,			-0.5f},
	{	-0.5f,			-0.5f + LEAD,	-0.5f},
	{	-0.5f,			-0.5f,			-0.5f + LEAD},

	{	-0.5f,			0.5f,			-0.5f},
	{	-0.5f + LEAD,	0.5f,			-0.5f},
	{	-0.5f,			0.5f - LEAD,	-0.5f},
	{	-0.5f,			0.5f,			-0.5f + LEAD},

	{	-0.5f,			-0.5f,			0.5f},
	{	-0.5f + LEAD,	-0.5f,			0.5f},
	{	-0.5f,			-0.5f + LEAD,	0.5f},
	{	-0.5f,			-0.5f,			0.5f - LEAD},

	{	-0.5f,			0.5f,			0.5f},
	{	-0.5f + LEAD,	0.5f,			0.5f},
	{	-0.5f,			0.5f - LEAD,	0.5f},
	{	-0.5f,			0.5f,			0.5f - LEAD},

	{	0.5f,			-0.5f,			-0.5f},
	{	0.5f - LEAD,	-0.5f,			-0.5f},
	{	0.5f,			-0.5f + LEAD,	-0.5f},
	{	0.5f,			-0.5f,			-0.5f + LEAD},

	{	0.5f,			0.5f,			-0.5f},
	{	0.5f - LEAD,	0.5f,			-0.5f},
	{	0.5f,			0.5f - LEAD,	-0.5f},
	{	0.5f,			0.5f,			-0.5f + LEAD},

	{	0.5f,			-0.5f,			0.5f},
	{	0.5f - LEAD,	-0.5f,			0.5f},
	{	0.5f,			-0.5f + LEAD,	0.5f},
	{	0.5f,			-0.5f,			0.5f - LEAD},

	{	0.5f,			0.5f,			0.5f},
	{	0.5f - LEAD,	0.5f,			0.5f},
	{	0.5f,			0.5f - LEAD,	0.5f},
	{	0.5f,			0.5f,			0.5f - LEAD}
};

static i3ColorAttr *		s_pFreezeColorAttr = nullptr;
static i3MaterialAttr *		s_pFreezeMaterialAttr = nullptr;

namespace
{
	struct RAII_static_member_variable
	{
		~RAII_static_member_variable()
		{
			I3_SAFE_RELEASE(s_pFreezeColorAttr);
			I3_SAFE_RELEASE(s_pFreezeMaterialAttr);
		}
	} _RAII_static_member_variable_;	
}

static REAL32 SNAP( REAL32 val, REAL32 snap)
{
	REAL32 div = (REAL32)( val / snap);

	return div * snap;
}

static REAL32 SNAP_ANGLE( REAL32 rad, REAL32 snap)
{
	REAL32 deg = I3_RAD2DEG( rad);
	INT32 div = (INT32)( deg / snap);

	return I3_DEG2RAD( div * snap);
}


i3LevelElement3D::i3LevelElement3D(void)
{
	m_pRoot = i3AttrSet::new_object();
	m_pRoot->SetName( "ElementRoot");
	m_pRoot->AddFlag( I3_NODEFLAG_VOLATILE);

	{
		m_pColorAttr = i3ColorAttr::new_object();
		m_pRoot->AddAttr( m_pColorAttr);
	}

	{
		m_pMaterialAttr = i3MaterialAttr::new_object();

		m_pMaterialAttr->SetDiffuse( 0.9f, 0.9f, 0.9f, 1.0f);
		m_pMaterialAttr->SetSpecular( 0.5f, 0.5f, 0.5f, 1.0f);
		m_pMaterialAttr->SetEmissive( 0.0f, 0.0f, 0.0f, 0.0f);
		m_pMaterialAttr->SetShininess( 64.0f);

		m_pRoot->AddAttr( m_pMaterialAttr);
	}

	{
		m_pBlendEnableAttr = i3BlendEnableAttr::new_object();

		m_pRoot->AddAttr( m_pBlendEnableAttr);
	}

	{
		m_pZWriteEnableAttr = i3ZWriteEnableAttr::new_object();

		m_pZWriteEnableAttr->Set( TRUE);
		m_pRoot->AddAttr( m_pZWriteEnableAttr);
	}

	if( s_pFreezeColorAttr == nullptr)
	{
		{
			COLORREAL col;

			s_pFreezeColorAttr = i3ColorAttr::new_object();

			i3Color::Set( &col, 0.2f, 0.2f, 0.2f, 0.5f);
			s_pFreezeColorAttr->Set( &col);
		}

		{
			COLORREAL a, d, s, e;

			s_pFreezeMaterialAttr = i3MaterialAttr::new_object();

			i3Color::Set( &a, (UINT8) 32, 32, 32, 128);
			i3Color::Set( &d, (UINT8)92, 92, 92, 128);
			i3Color::Set( &s, (UINT8)0, 0, 0, 0);
			i3Color::Set( &e, (UINT8)0, 0, 0, 0);

			s_pFreezeMaterialAttr->SetDiffuse( &d);
			s_pFreezeMaterialAttr->SetSpecular( &s);
			s_pFreezeMaterialAttr->SetEmissive( &e);
		}
	}

	m_pScene = i3SceneObject::new_object();
	m_pRoot->AddChild( m_pScene);

	if( g_pSelBoxNode == nullptr)
	{
		CreateSelectBoxGeometry();
		_CreateTargetBoxGeometry();
	}

	m_pSelBox = i3Transform::new_object();
	m_pSelBox->SetName( "SelBox");
	m_pSelBox->AddFlag( I3_NODEFLAG_DISABLE | I3_NODEFLAG_CONTROL);
	m_pSelBox->AddChild( g_pSelBoxNode);

	m_pTargetBox = i3Transform::new_object();
	m_pTargetBox->SetName( "TargetBox");
	m_pTargetBox->AddFlag( I3_NODEFLAG_DISABLE | I3_NODEFLAG_CONTROL);
	m_pTargetBox->AddChild( g_pTargetBoxNode);

	OnCreate();
}

i3LevelElement3D::~i3LevelElement3D(void)
{
	if( m_pGroup != nullptr)
	{
		m_pGroup->RemoveElement( this);
	}
	
	if( m_pLayer != nullptr)
	{
		m_pLayer->RemoveElement( this);
	}

	_RemoveAllShapeSetInfo();
	removeAllTarget();

	I3_SAFE_RELEASE( m_pVisibleCollRoot);
	I3_SAFE_RELEASE( m_pMaterialAttr);
	I3_SAFE_RELEASE( m_pColorAttr);
	I3_SAFE_RELEASE( m_pBlendEnableAttr);
	I3_SAFE_RELEASE( m_pZWriteEnableAttr);

	I3_SAFE_RELEASE( m_pRoot);
	I3_SAFE_RELEASE( m_pScene);
	I3_SAFE_RELEASE( m_pSelBox);
	I3_SAFE_RELEASE( m_pTargetBox);
	I3_SAFE_RELEASE( m_pSymbol);
	I3_SAFE_RELEASE( m_pRes);
}

void i3LevelElement3D::_RemoveAllShapeSetInfo(void)
{
	I3_VISIBLE_COLL_MESH_INFO * pInfo;

	for(size_t i = 0; i < m_ShapeSetInfoList.size(); i++)
	{
		pInfo = m_ShapeSetInfoList[i];

		i3MemFree( pInfo);
	}

	m_ShapeSetInfoList.clear();
}

void i3LevelElement3D::BindRes( i3LevelRes * pRes)
{
	I3_REF_CHANGE( m_pRes, pRes);

	if( m_pRes != nullptr)
	{
		i3::string_ncopy_nullpad( m_szResPath, m_pRes->getPath(), _countof( m_szResPath) - 1);
	}
	else
	{
		m_szResPath[0] = 0;
	}
}

const char * i3LevelElement3D::getResPath(void) const
{
	if( m_pRes != nullptr)
		return m_pRes->getPath();

	return m_szResPath;
}

void i3LevelElement3D::CreateSelectBoxGeometry(void)
{
	i3GeometryAttr * pGeoAttr;

	{
		g_pSelBoxNode = i3AttrSet::new_object();
		g_pSelBoxNode->SetName( "SelBox");
		//g_pSelBoxNode->SetPriority( 0);

		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();

			pAttr->Set( TRUE);
			g_pSelBoxNode->AddAttr( pAttr);
		}

		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
			g_pSelBoxNode->AddAttr( pAttr);
		}

		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			COLORREAL col;

			i3Color::Set( &col, (UINT8) 255, 255, 255, 192);
			pAttr->Set( &col);
			g_pSelBoxNode->AddAttr( pAttr);
		}

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

			pAttr->Set( FALSE);
			g_pSelBoxNode->AddAttr( pAttr);
		}

		{
			i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();

			pAttr->Set( FALSE);
			g_pSelBoxNode->AddAttr( pAttr);
		}
	}

	// Create line box
	{
		// Vertex Array
		{
			VEC3D vec;
			i3VertexArray * pVA;
			i3VertexFormat format;
			
			format.SetHasPosition( true);

			pVA = i3VertexArray::new_object_ref();

			pVA->Create( &format, 32, 0);

			pVA->Lock();

			MATRIX tmp;

			i3Matrix::SetScale( &tmp, 1.01f, 1.01f, 1.01f);

			for(INT32 i = 0; i < 32; i++)
			{
				setXYZ( &vec, s_SelBoxVertex[i][0], s_SelBoxVertex[i][1], s_SelBoxVertex[i][2]);

				i3Vector::TransformCoord( &vec, &vec, &tmp);
				pVA->SetPosition( i, &vec);
			}

			pVA->Unlock();

			// Primitive
			pGeoAttr = i3GeometryAttr::new_object_ref();
			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 24);
		}

		// Index Array
		{
			i3IndexArray * pIA;
			INT32 base, count;

			pIA = i3IndexArray::new_object_ref();

			pIA->Create( 48, 0, pGeoAttr->GetVertexArray()->GetCount());

			pIA->Lock();

			count = 1;
			base = 0;

			for(INT32 i = 0; i < 48; i++)
			{
				INT32 idx;

				if( i & 1)
				{
					idx = base + count;

					count++;

					if( count > 3)
					{
						count = 1;
						base += 4;
					}
				}
				else
				{
					idx = base;
				}

				pIA->SetIndex( i, (UINT16)idx);
			}

			pIA->Unlock();

			pGeoAttr->SetIndexArray( pIA);
		}
	}

	{
		i3Geometry * pGeo;

		pGeo = i3Geometry::new_object_ref();

		pGeo->AppendGeometryAttr( pGeoAttr);

		g_pSelBoxNode->AddChild( pGeo);
	}
}

void i3LevelElement3D::_CreateTargetBoxGeometry(void)
{
	i3GeometryAttr * pGeoAttr;

	{
		g_pTargetBoxNode = i3AttrSet::new_object();
		g_pTargetBoxNode->SetName( "TargetBox");

		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();

			pAttr->Set( TRUE);
			g_pTargetBoxNode->AddAttr( pAttr);
		}

		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
			pAttr->SetSource( I3G_BLEND_SRCALPHA);
			pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
			g_pTargetBoxNode->AddAttr( pAttr);
		}

		{
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			COLORREAL col;

			i3Color::Set( &col, (UINT8) 255, 0, 0, 192);
			pAttr->Set( &col);
			g_pTargetBoxNode->AddAttr( pAttr);
		}

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set( FALSE);
			g_pTargetBoxNode->AddAttr( pAttr);
		}

		{
			i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
			pAttr->Set( FALSE);
			g_pTargetBoxNode->AddAttr( pAttr);
		}
	}

	// Create line box
	{
		// Vertex Array
		{
			VEC3D vec;
			i3VertexArray * pVA;
			i3VertexFormat format;
			MATRIX mtx;
			
			format.SetHasPosition( true);

			pVA = i3VertexArray::new_object_ref();
			pVA->Create( &format, 32, 0);
			pVA->Lock();

			i3Matrix::SetScale( &mtx, 1.03f, 1.03f, 1.03f);

			for(INT32 i = 0; i < 32; i++)
			{
				setXYZ( &vec, s_SelBoxVertex[i][0], s_SelBoxVertex[i][1], s_SelBoxVertex[i][2]);
				i3Vector::TransformCoord( &vec, &vec, &mtx);

				pVA->SetPosition( i, &vec);
			}

			pVA->Unlock();

			// Primitive
			pGeoAttr = i3GeometryAttr::new_object_ref();
			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 24);
		}

		// Index Array
		{
			i3IndexArray * pIA;
			INT32 base, count;

			pIA = i3IndexArray::new_object_ref();
			pIA->Create( 48, 0, pGeoAttr->GetVertexArray()->GetCount());
			pIA->Lock();

			count = 1;
			base = 0;

			for(INT32 i = 0; i < 48; i++)
			{
				INT32 idx;

				if( i & 1)
				{
					idx = base + count;

					count++;

					if( count > 3)
					{
						count = 1;
						base += 4;
					}
				}
				else
				{
					idx = base;
				}

				pIA->SetIndex( i, idx);
			}

			pIA->Unlock();

			pGeoAttr->SetIndexArray( pIA);
		}
	}

	{
		i3Geometry * pGeo;

		pGeo = i3Geometry::new_object_ref();

		pGeo->AppendGeometryAttr( pGeoAttr);

		g_pTargetBoxNode->AddChild( pGeo);
	}
}

void i3LevelElement3D::SetCollisionEditEnable( bool bEdit)
{
	if( bEdit)
		AddState( I3_LEVEL_STATE_COLLISION_EDIT);
	else
		RemoveState( I3_LEVEL_STATE_COLLISION_EDIT);

	SetCollisionVisible( bEdit);
}

void i3LevelElement3D::SetCollisionVisible( bool bView)
{
	if( bView)
		AddState( I3_LEVEL_STATE_COLLISION_VIEW);
	else
		RemoveState( I3_LEVEL_STATE_COLLISION_VIEW);

	if( bView)
	{
		if( m_pVisibleCollRoot == nullptr)
		{
			_CreateVisibleCollision();
		}

		m_pVisibleCollRoot->RemoveFlag( I3_NODEFLAG_DISABLE);
	}
	else
	{
		if( m_pVisibleCollRoot != nullptr)
		{
			m_pVisibleCollRoot->AddFlag( I3_NODEFLAG_DISABLE);
		}
	}

}

void i3LevelElement3D::_CreateVisibleCollision(void)
{
	m_pVisibleCollRoot = i3AttrSet::new_object();
	m_pVisibleCollRoot->SetName( "Visible Collision Mesh");
	m_pVisibleCollRoot->AddFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL | I3_NODEFLAG_DISABLE);

	{
		i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
		pAttr->Set( TRUE);
		m_pVisibleCollRoot->AddAttr( pAttr);
	}

	{
		i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
		pAttr->SetSource( I3G_BLEND_SRCALPHA);
		pAttr->SetDestination( I3G_BLEND_ONE);
		m_pVisibleCollRoot->AddAttr( pAttr);
	}

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
		pAttr->Set( FALSE);
		m_pVisibleCollRoot->AddAttr( pAttr);
	}

	{
		i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
		pAttr->Set( FALSE);
		m_pVisibleCollRoot->AddAttr( pAttr);
	}

	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();

		pAttr->Set( FALSE);
		m_pVisibleCollRoot->AddAttr( pAttr);
	}

	{
		m_pVisibleCollColorAttr = i3ColorAttr::new_object_ref();
		COLOR col;
		
		i3Color::Set( &col, (UINT8) 255, 255, 255, 255);

		m_pVisibleCollColorAttr->Set( &col);
		m_pVisibleCollRoot->AddAttr( m_pVisibleCollColorAttr);
	}

	{
		i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();

		pAttr->setBias( -0.0002f);
		m_pVisibleCollRoot->AddAttr( pAttr);
	}

	m_pScene->AddChild( m_pVisibleCollRoot);

	i3::vector<i3Node*> list;
	COLOR col;

	OnQueryCollisionNode( list);

	_RemoveAllShapeSetInfo();

	i3Color::Set( &col, (UINT8) NORM_R, NORM_G, NORM_B, NORM_A);

	for(size_t i = 0; i < list.size(); i++)
	{
		i3Node * pNode = list[i];

		i3SceneUtil::BuildRendableCollider( pNode, m_pVisibleCollRoot, TRUE, FALSE, false, &m_ShapeSetInfoList, &col);
	}
}

void i3LevelElement3D::addTarget( i3LevelElement3D * pElm)
{
	I3_MUST_ADDREF(pElm);

	m_TargetList.push_back( pElm);
}

void i3LevelElement3D::removeTarget( i3LevelElement3D * pElm)
{
	I3ASSERT( pElm != nullptr);

	size_t idx = i3::vu::index_of(m_TargetList, pElm);
	
	I3ASSERT( idx != m_TargetList.size() );

	I3_MUST_RELEASE(pElm);

	m_TargetList.erase(m_TargetList.begin() + idx);
}

void i3LevelElement3D::removeAllTarget(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < getTargetCount(); i++)
	{
		pElm = getTarget( i);

		I3_MUST_RELEASE(pElm);
	}

	m_TargetList.clear();
}

i3LevelElement3D *	i3LevelElement3D::FindTargetByGameObj( i3GameObj * pObj)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < getTargetCount(); i++)
	{
		pElm = getTarget( i);

		i3GameObj * pTempObj = pElm->CreateGameInstance();

		if( pTempObj != nullptr)
		{
			I3_MUST_RELEASE(pTempObj);

			if( pTempObj == pObj)
				return pElm;
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////
void i3LevelElement3D::Attach( i3Node * pRoot)
{
	pRoot->AddChild( m_pRoot);
}

void i3LevelElement3D::Detach(void)
{
	m_pRoot->RemoveFromParent();
}

void i3LevelElement3D::Enable(void)
{
	RemoveState( I3_LEVEL_STATE_DISABLE);

	if( m_pRoot != nullptr)
		m_pRoot->RemoveFlag( I3_NODEFLAG_DISABLE);
}

void i3LevelElement3D::Disable(void)
{
	AddState( I3_LEVEL_STATE_DISABLE);

	if( m_pRoot != nullptr)
		m_pRoot->AddFlag( I3_NODEFLAG_DISABLE);
}

void i3LevelElement3D::Reset(void)
{
	i3::vector<i3Node*> List;
	i3TimeSequence * pTimeSeq;

	i3Scene::FindNodeByExactType( m_pScene, i3TimeSequence::static_meta(), List, TRUE);

	for(size_t i = 0; i < List.size(); i++)
	{
		pTimeSeq = (i3TimeSequence *) List[i];

		pTimeSeq->OnChangeTime( 0.0f);
	}
}

void i3LevelElement3D::SetSymbol( i3Node * pSG)
{
	I3_SAFE_RELEASE( m_pSymbol);
	m_pScene->RemoveAllChild();

	m_pSymbol = pSG;

	if( m_pSymbol != nullptr)
	{
		I3_MUST_ADDREF(m_pSymbol);

		m_pScene->AddChild( m_pSymbol);

		OnInstantiate();

		i3Scene::ModifyDynamicState( m_pSymbol, I3GAME_DYNAMIC_STATE_KINEMATIC, 0);

		{
			i3BoundBox * pBox;

			pBox = i3BoundBox::new_object();

			m_pSymbol->GetWrappingBound( pBox);

			if( pBox->IsEmpty() == TRUE)
			{
				VEC3D vmin, vmax;

				i3Vector::Set( &vmin, -1.0f, -1.0f, -1.0f);
				i3Vector::Set( &vmax, 1.0f, 1.0f, 1.0f);
				pBox->SetMinMax( &vmin, &vmax);
			}

			i3Vector::Copy( &m_BoundMin, pBox->GetMin());
			i3Vector::Copy( &m_BoundMax, pBox->GetMax());

			I3_MUST_RELEASE(pBox);
		}

		// Select Box의 Scale 값을 설정한다.
		{
			REAL32 sx, sy, sz;
			VEC3D diff;
			MATRIX tmp;

			// Scale
			{
				i3Vector::Sub( &diff, &m_BoundMax, &m_BoundMin);

				sx = i3Math::abs( i3Vector::GetX(&diff));
				sy = i3Math::abs( i3Vector::GetY(&diff));
				sz = i3Math::abs( i3Vector::GetZ(&diff));

				i3Matrix::SetScale( &tmp, sx, sy, sz);
			}

			// Translate
			{
				i3Vector::Add( &diff, &m_BoundMax, &m_BoundMin);
				i3Vector::Scale( &diff, &diff, 0.5f);

				i3Matrix::PostTranslate( &tmp, &diff);
			}

			m_pSelBox->SetMatrix( &tmp);
			m_pTargetBox->SetMatrix( &tmp);

			m_pScene->AddChild( m_pSelBox);
			m_pScene->AddChild( m_pTargetBox);
		}
	}
	
	OnChangeResource();
}

void i3LevelElement3D::SetColor( COLOR * pColor)
{

	{
		COLORREAL col;

		i3Color::Set( &col, pColor);

		m_pColorAttr->Set( &col);
	}

	OnChangeColor();
}

void i3LevelElement3D::GetColor( COLOR * pColor)
{
	if( m_pColorAttr == nullptr)
		i3Color::Set( pColor, (UINT8) 255, 255, 255, 255);
	else
		i3Color::Set( pColor, m_pColorAttr->Get());
}

/*
void i3LevelElement3D::SetMaterialAmbient( REAL32 r, REAL32 g, REAL32 b)
{
	m_pMaterialAttr->SetAmbient( r, g, b, 1.0f);

	OnChangeColor();
}
*/

void i3LevelElement3D::SetMaterialDiffuse( REAL32 r, REAL32 g, REAL32 b, REAL32 a)
{
	COLORREAL col;

	i3Color::Set( &col, r, g, b, a);

	if( m_pColorAttr )
		m_pColorAttr->Set( &col);

	if( m_pMaterialAttr )
		m_pMaterialAttr->SetDiffuse( r, g, b, a);

	if( nullptr == m_pBlendEnableAttr || nullptr == m_pZWriteEnableAttr )
	{
		return;
	}

	if( a == 1.0f)
	{
		m_pBlendEnableAttr->Set( FALSE);
		m_pZWriteEnableAttr->Set( TRUE);
	}
	else
	{
		m_pBlendEnableAttr->Set( TRUE);
		m_pZWriteEnableAttr->Set( FALSE);
	}

	OnChangeColor();
}

void i3LevelElement3D::SetMaterialSpecular( REAL32 r, REAL32 g, REAL32 b)
{
	m_pMaterialAttr->SetSpecular( r, g, b, 1.0f);

	OnChangeColor();
}

void _ChangeSel( i3VertexArray * pVA, INT32 * idx, bool bSelect, SEL_MODE selMode)
{
	COLOR col;

	if( selMode != SEL_ALTERNATIVE)
	{
		if( bSelect)
			i3Color::Set( &col, (UINT8) SEL_R, SEL_G, SEL_B, SEL_A);
		else
			i3Color::Set( &col, (UINT8) NORM_R, NORM_G, NORM_B, NORM_A);
	}
	else
	{
		// Toggle 선택
		COLORREAL rcol;

		pVA->GetColor( idx[0], &rcol);

		if( rcol.r >= (200.0f / 255.0f))
			i3Color::Set( &col, (UINT8) NORM_R, NORM_G, NORM_B, NORM_A);
		else
			i3Color::Set( &col, (UINT8) SEL_R, SEL_G, SEL_B, SEL_A);
	}

	pVA->SetColor( idx[0], 3, &col);
}

void i3LevelElement3D::Select(void)
{
	AddState( I3_LEVEL_STATE_SELECTED);

	m_pSelBox->RemoveFlag( I3_NODEFLAG_DISABLE);

	OnSelected( TRUE);

	i3Level::Update( nullptr, I3_TDK_UPDATE_SELECT, this);

}

void i3LevelElement3D::Unselect(void)
{
	RemoveState( I3_LEVEL_STATE_SELECTED);

	m_pSelBox->AddFlag( I3_NODEFLAG_DISABLE);

	OnSelected( FALSE);

	if( getCollisionEditEnable())
	{
		// 선택된 Triangle이 있다면 해제...
		i3GeometryResolver resolver;
		INT32 j, idx[3];
		i3GeometryAttr * pGeoAttr;

		for(size_t i = 0; i < m_SelGeoAttrList.size(); i++)
		{
			pGeoAttr = m_SelGeoAttrList[i];

			resolver.setGeometryAttr( pGeoAttr);

			for( j = 0; j < resolver.getPrimitiveCount(); j++)
			{
				resolver.getVertexIndices( j, idx);

				_ChangeSel( pGeoAttr->GetVertexArray(), idx, FALSE, SEL_EXCLUSIVE);
			}
		}
	}

	i3Level::Update( nullptr, I3_TDK_UPDATE_UNSELECT, this);
}

void i3LevelElement3D::Targeted(void)
{
	AddState( I3_LEVEL_STATE_TARGETED);

	m_pTargetBox->RemoveFlag( I3_NODEFLAG_DISABLE);

	OnTargeted( TRUE);
}

void i3LevelElement3D::Untargeted(void)
{
	RemoveState( I3_LEVEL_STATE_TARGETED);

	m_pTargetBox->AddFlag( I3_NODEFLAG_DISABLE);

	OnTargeted( FALSE);
}

void i3LevelElement3D::Freeze(void)
{
	AddState( I3_LEVEL_STATE_FROZEN);

	Unselect();

	m_pRoot->AddAttr( s_pFreezeColorAttr);
	m_pRoot->AddAttr( s_pFreezeMaterialAttr);
}

void i3LevelElement3D::Unfreeze(void)
{
	RemoveState( I3_LEVEL_STATE_FROZEN);

	m_pRoot->RemoveAttr( s_pFreezeColorAttr);
	m_pRoot->RemoveAttr( s_pFreezeMaterialAttr);
}

void i3LevelElement3D::GetScale( VEC3D * pScale)
{
	pScale->x = i3Vector::Length( i3Matrix::GetRight( GetTransform()));
	pScale->y = i3Vector::Length( i3Matrix::GetUp( GetTransform()));
	pScale->z = i3Vector::Length( i3Matrix::GetAt( GetTransform()));
}

void i3LevelElement3D::SetScale( REAL32 x, REAL32 y, REAL32 z)
{
	VEC3D scale;

	i3Vector::Set( &scale, x, y, z);

	SetScale( &scale);
}

void i3LevelElement3D::SetScale( VEC3D * pScale)
{
	VEC3D * pVec;

	pVec = i3Matrix::GetRight( GetTransform());
	i3Vector::Normalize( pVec, pVec);
	//i3Vector::Scale( pVec, pVec, pScale->x);

	pVec = i3Matrix::GetUp( GetTransform());
	i3Vector::Normalize( pVec, pVec);
	//i3Vector::Scale( pVec, pVec, pScale->y);

	pVec = i3Matrix::GetAt( GetTransform());
	i3Vector::Normalize( pVec, pVec);
	//i3Vector::Scale( pVec, pVec, pScale->z);

	MATRIX tmp;

	i3Matrix::SetScale( &tmp, pScale->x, pScale->y, pScale->z);
	i3Matrix::Mul( GetTransform(), &tmp, GetTransform());

	OnScaling();
}

void i3LevelElement3D::SetRotate( REAL32 x, REAL32 y, REAL32 z)
{
	MATRIX mtx;

	i3Matrix::SetRotateX( &mtx, x);
	i3Matrix::PostRotateY( &mtx, y);
	i3Matrix::PostRotateZ( &mtx, z);

	i3Vector::Copy( i3Matrix::GetRight( GetTransform()),	i3Matrix::GetRight( &mtx));
	i3Vector::Copy( i3Matrix::GetUp( GetTransform()),		i3Matrix::GetUp( &mtx));
	i3Vector::Copy( i3Matrix::GetAt( GetTransform()),		i3Matrix::GetAt( &mtx));

	OnRotating();
}

void i3LevelElement3D::SetRotateDeg( REAL32 x, REAL32 y, REAL32 z)
{
	SetRotate( I3_DEG2RAD(x), I3_DEG2RAD(y), I3_DEG2RAD(z) );
}

void i3LevelElement3D::GetRotateAngle( VEC3D * pRot)
{
	QUATERNION q;
	REAL32 len;

	i3Matrix::GetRotateQuat( &q, GetTransform());
	i3Quat::Normalize( &q, &q);

	D3DXQuaternionToAxisAngle( (CONST D3DXQUATERNION *) &q, (D3DXVECTOR3 *) pRot, &len);

	i3Vector::Scale( pRot, pRot, len * I3_PI2);
}

void	i3LevelElement3D::OnInstantiate()
{
	if( m_pSymbol )
		m_pSymbol->Instantiate();
}

void i3LevelElement3D::SaveMatrix(void)
{
	i3Matrix::Copy( &m_SavedMatrix, GetTransform());
}

void i3LevelElement3D::RestoreMatrix(void)
{
	i3Matrix::Copy( GetTransform(), &m_SavedMatrix);
}

void i3LevelElement3D::Moving( I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, REAL32 snap)
{
	VEC3D vec;

	i3Vector::Set( &vec, x, y, z );

	switch( space)
	{
		case I3LV_SPACE::WORLD :
			i3Vector::Add( &vec, i3Matrix::GetPos( &m_SavedMatrix), &vec);
			break;

		case I3LV_SPACE::LOCAL :
			{
				MATRIX tmp;

				i3Matrix::Copy( &tmp, &m_SavedMatrix);
				i3Matrix::PreTranslate( &tmp, x, y, z);

				i3Vector::Copy( &vec, i3Matrix::GetPos( &tmp));
			}
			break;
	}

	if( snap > 0.0f)
	{
		vec.x = SNAP( vec.x, snap);
		vec.y = SNAP( vec.y, snap);
		vec.z = SNAP( vec.z, snap);
	}

	i3Matrix::SetPos( m_pScene->GetMatrix(), &vec);

	OnMoving();
}

void i3LevelElement3D::Rotating( I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, REAL32 snap)
{
	MATRIX * pMtx = GetTransform();

	i3Matrix::Copy( pMtx, &m_SavedMatrix);
	i3Matrix::SetPos( pMtx, 0.0f, 0.0f, 0.0f, 1.0f);

	if( snap > 0.0f)
	{
		x = SNAP_ANGLE( x, snap);
		y = SNAP_ANGLE( y, snap);
		z = SNAP_ANGLE( z, snap);
	}
	
	if( space == I3LV_SPACE::LOCAL)
	{
		if( x != 0.0f)
			i3Matrix::PreRotateX( pMtx, x);
		else if( y != 0.0f)
			i3Matrix::PreRotateY( pMtx, y);
		else if( z != 0.0f)
			i3Matrix::PreRotateZ( pMtx, z);
	}
	else
	{
		if( x != 0.0f)
			i3Matrix::PostRotateX( pMtx, x);
		else if( y != 0.0f)
			i3Matrix::PostRotateY( pMtx, y);
		else if( z != 0.0f)
			i3Matrix::PostRotateZ( pMtx, z);
	}

	i3Matrix::PostTranslate( pMtx, i3Matrix::GetPos( &m_SavedMatrix));

	OnRotating();
}

static REAL32 _AdjustScale( REAL32 len, REAL32 minValue)
{
	if( i3Math::abs( len) < minValue)
	{
		// Scale이 Eps보다 작으면 문제가 발생한다.
		(len >= 0.0f) ?	len = minValue : len = -minValue;
	}

	return len;
}

REAL32	i3LevelElement3D::_ScaleAxis( VEC3D * pAxis, REAL32 inc, REAL32 snap)
{
	REAL32 lenOrg, lenNew;

	lenOrg = i3Vector::Normalize( pAxis, pAxis);
	lenNew = _AdjustScale( lenOrg + inc, snap);

	i3Vector::Scale( pAxis, pAxis, lenNew);

	return lenOrg;
}

void i3LevelElement3D::Scaling( I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, REAL32 snap)
{
	VEC3D len;
	VEC3D * pRight, * pUp, * pAt;
	bool bX = ( i3Math::abs( x) > I3_EPS);
	bool bY = ( i3Math::abs( y) > I3_EPS);
	bool bZ = ( i3Math::abs( z) > I3_EPS);

	i3Matrix::Copy( GetTransform(), &m_SavedMatrix);

	pRight	= i3Matrix::GetRight( GetTransform());
	pUp		= i3Matrix::GetUp( GetTransform());
	pAt		= i3Matrix::GetAt( GetTransform());

	if( bX)
		len.x = _ScaleAxis( pRight, x, snap);

	if( bY)
		len.y = _ScaleAxis( pUp,	y, snap);

	if( bZ)
		len.z = _ScaleAxis( pAt,	z, snap);

	if( snap > 0.0f)
	{
		VEC3D p[4], diff, orgSize;
		REAL32 newSize;

		i3Vector::Sub( &orgSize, &m_BoundMax, &m_BoundMin);

		p[0] = VEC3D( m_BoundMin.x, m_BoundMin.y, m_BoundMin.z);
		p[1] = VEC3D( m_BoundMax.x, m_BoundMin.y, m_BoundMin.z);		// X만 다른 경우
		p[2] = VEC3D( m_BoundMin.x, m_BoundMax.y, m_BoundMin.z);		// Y만 다른 경우
		p[3] = VEC3D( m_BoundMin.x, m_BoundMin.y, m_BoundMax.z);		// Z만 다른 경우.

		i3Matrix::TransformPoints( p, p, 4, GetTransform());

		// 조정된 크기로 다시 크기 설정.
		i3Matrix::Copy( GetTransform(), &m_SavedMatrix);

		if( bX)
		{
			i3Vector::Sub( &diff, &p[1], &p[0]);
			newSize = i3Vector::Length( &diff);

			newSize = SNAP( newSize, snap);

			x = (newSize / orgSize.x) - len.x;
			_ScaleAxis( pRight, x, snap);
		}

		if( bY)
		{
			i3Vector::Sub( &diff, &p[2], &p[0]);
			newSize = i3Vector::Length( &diff);

			newSize = SNAP( newSize, snap);

			y = (newSize / orgSize.y) - len.y;
			_ScaleAxis( pUp,	y, snap);
		}

		if( bZ)
		{
			i3Vector::Sub( &diff, &p[3], &p[0]);
			newSize = i3Vector::Length( &diff);

			newSize = SNAP( newSize, snap);

			z = (newSize / orgSize.z) - len.z;
			_ScaleAxis( pAt,	z, snap);
		}
	}

	OnScaling();
}

void i3LevelElement3D::GripDrag( INT32 idxGrip, const VEC3D & scale, REAL32 scaleSnap)
{
	Scaling( I3LV_SPACE::LOCAL, scale.x, scale.y, scale.z, scaleSnap);

	// 다른 쪽 면을 움직이지 않도록 하기 위한 Translation
	{
		VEC3D oldMin, oldMax;
		VEC3D newMin, newMax;
		VEC3D newPos, gap;

		i3Vector::TransformCoord( &oldMin, &m_BoundMin, &m_SavedMatrix);
		i3Vector::TransformCoord( &oldMax, &m_BoundMax, &m_SavedMatrix);

		i3Vector::TransformCoord( &newMin, &m_BoundMin, GetTransform());
		i3Vector::TransformCoord( &newMax, &m_BoundMax, GetTransform());

		i3Vector::Sub( &gap, &newMax, &oldMax);

		if( (idxGrip & 1) == 0)
		{
			// 0, 2, 4 (Negative Direction)
			i3Vector::Scale( &gap, &gap, -1.0f);
		}

		i3Vector::Add( &newPos, i3Matrix::GetPos( &m_SavedMatrix), &gap);

		i3Matrix::SetPos( GetTransform(), &newPos);
	}

	OnMoving();
}

void i3LevelElement3D::GetTransformedBoundBox( VEC3D * pMin, VEC3D * pMax)
{
	MATRIX full;
	VEC3D pts[8];
	REAL32 x1, y1, z1, x2, y2, z2;
	INT32 i;

	i3Matrix::Copy( &full, m_pScene->GetMatrix());

	x1 = i3Vector::GetX( &m_BoundMin);
	y1 = i3Vector::GetY( &m_BoundMin);
	z1 = i3Vector::GetZ( &m_BoundMin);

	x2 = i3Vector::GetX( &m_BoundMax);
	y2 = i3Vector::GetY( &m_BoundMax);
	z2 = i3Vector::GetZ( &m_BoundMax);

	i3Vector::Set( &pts[0], x1, y1, z1);
	i3Vector::Set( &pts[1], x1, y1, z2);
	i3Vector::Set( &pts[2], x2, y1, z1);
	i3Vector::Set( &pts[3], x2, y1, z2);
	i3Vector::Set( &pts[4], x1, y2, z1);
	i3Vector::Set( &pts[5], x1, y2, z2);
	i3Vector::Set( &pts[6], x2, y2, z1);
	i3Vector::Set( &pts[7], x2, y2, z2);

	i3Matrix::TransformPoints( pts, pts, 8, &full);

	i3Vector::Copy( pMin, &pts[0]);
	i3Vector::Copy( pMax, &pts[0]);

	for( i = 1; i < 8; i++)
	{
		i3Vector::Minimize( pMin, pMin, &pts[i]);
		i3Vector::Maximize( pMax, pMax, &pts[i]);
	}
}

void i3LevelElement3D::OnCreate(void)
{
}

void i3LevelElement3D::OnMoving(void)
{
	I3ASSERT( i3Matrix::isValid( GetName(), GetTransform()));
}

static I3SG_TRAVERSAL_RESULT __Traverse_Scale_Proc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	if( pNode == nullptr)
		return I3SG_TRAVERSAL_STOP;

	i3PhysixShapeSet * pShapeSet = nullptr;

	if( i3::same_of<i3Transform2* >(pNode))
	{
		pShapeSet = ((i3Transform2*)pNode)->getShapeSet();
	}
	else if( i3::same_of<i3ShapeSetContainer*>(pNode))
	{
		pShapeSet = ((i3ShapeSetContainer*)pNode)->getShapeSet();
	}	

	if( pShapeSet != nullptr)
	{
		VEC3D scale;

		i3Matrix::GetScale( pStack->GetTop(), &scale);

		pShapeSet->AdjustScale( scale, pStack->GetTop());
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3LevelElement3D::AdjustPhysixSize(void)
{
	i3MatrixStack stack;

	i3Scene::Traverse( GetSceneObject(), (I3_SG_TRAVERSE_PROC) __Traverse_Scale_Proc, nullptr, &stack);
}

void i3LevelElement3D::OnScaling( )
{	
	AdjustPhysixSize();
}

void i3LevelElement3D::OnRotating(void)
{
}

void i3LevelElement3D::OnSelected( BOOL bSelected)
{
}

void i3LevelElement3D::OnTargeted( BOOL bTargeted)
{
}

void i3LevelElement3D::OnFrame(void)
{
}

void i3LevelElement3D::OnChangeResource(void)
{
	i3Scene::ModifyDynamicState( m_pSymbol, 0, I3_PHYSIX_STATE_NO_COLLISION);
}

inline bool		testTriRect( VEC3D * p, i3::pack::RECT * pRect)
{
	INT32 _i;
	VEC3D c, dir;

	// 삼각형의 각 꼭지점이 Rect 영역에 포함되면 OK
	for( _i = 0; _i < 3; _i++)
	{
		if( i3Math::PtInRect( pRect, (VEC2D *) &p[_i]))
			return true;
	}

	// 그렇지 않다면 삼각형 안에 사격형의 꼭지점이 포함되는지 확인
	i3Vector::Set( &dir, 0.0f, 0.0f, 1.0f);

	i3Vector::Set( &c, pRect->left, pRect->top, 0.0f);
	if( ContainTriPoint( p, &c, &dir))
		return true;

	i3Vector::Set( &c, pRect->right, pRect->top, 0.0f);
	if( ContainTriPoint( p, &c, &dir))
		return true;

	i3Vector::Set( &c, pRect->right, pRect->bottom, 0.0f);
	if( ContainTriPoint( p, &c, &dir))
		return true;

	i3Vector::Set( &c, pRect->left, pRect->bottom, 0.0f);
	if( ContainTriPoint( p, &c, &dir))
		return true;

	return false;
}

struct I3LV_HIT_RAYCAST_INFO
{
	VEC3D *				_pL1 = nullptr;
	VEC3D *				_pL2 = nullptr;
	REAL32				_ClosestT = 100000.0f;
	MATRIX *			_pViewProj = nullptr;
	MATRIX				_mtxInvProj;
	VEC3D				_AtDir;

	bool				_bCollisionEdit = false;
	bool				_bPick = false;
	bool				_bSelect = false;
	SEL_MODE			_SelMode;
	i3GeometryAttr *	_pHitGeoAttr = nullptr;
	INT32				_TriIndex = -1;

	i3LevelElement3D *	_pElement = nullptr;
} ;


// 하나의 ShapeSet으로 Raycast를 한다.
// 단, 가장 가까운 ShapeSet을 인지를 확인한다.
static void	_doRaycast( i3GeometryAttr * pGeoAttr, I3LV_HIT_RAYCAST_INFO * pInfo, i3MatrixStack * pStack)
{
	if(pGeoAttr->GetPrimitiveType() != I3G_PRIM_TRISTRIP && 
		pGeoAttr->GetPrimitiveType() != I3G_PRIM_TRIFAN && 
		pGeoAttr->GetPrimitiveType() != I3G_PRIM_TRILIST)
	{
		return;
	}

	INT32 i, idx[3];
	VEC3D dir, pc;
	VEC3D p[3], n;
	REAL32 t;
	bool bPicked = true;
	i3VertexArray * pVA = pGeoAttr->GetVertexArray();

	i3GeometryResolver resolver;
	resolver.setGeometryAttr( pGeoAttr);

	i3Vector::Sub( &dir, pInfo->_pL1, pInfo->_pL2);
	i3Vector::Normalize( &dir, &dir);

	#if defined( DO_VLOG)
		i3Color::Set( &c, 255, 0, 0, 255);
		i3VLog::SetColor( &c);
		i3VLog::Line( pInfo->_pL1, pInfo->_pL2);
	#endif

		i3Vector::Sub(&dir, pInfo->_pL2, pInfo->_pL1);
		float dir_len = i3Vector::Length(&dir);
		dir /= dir_len;

		D3DXVECTOR3 vPickRayDir;
		vPickRayDir.x = dir.x;
		vPickRayDir.y = dir.y;
		vPickRayDir.z = dir.z;

		D3DXVECTOR3 vPickRayOrig;
		vPickRayOrig.x = pInfo->_pL1->x;
		vPickRayOrig.y = pInfo->_pL1->y;
		vPickRayOrig.z = pInfo->_pL1->z;

	for( i = 0; i < (INT32) resolver.getPrimitiveCount(); i++)
	{
		resolver.getVertexIndices( i, idx);

		pVA->GetNormal( idx[0], &n);

		// 뒷면을 바라보는 것은 배제...
		pVA->GetPosition( idx[0], &p[0]);
		pVA->GetPosition( idx[1], &p[1]);
		pVA->GetPosition( idx[2], &p[2]);

		D3DXVECTOR3 v0;
		v0.x = p[0].x;
		v0.y = p[0].y;
		v0.z = p[0].z;

		D3DXVECTOR3 v1;
		v1.x = p[1].x;
		v1.y = p[1].y;
		v1.z = p[1].z;

		D3DXVECTOR3 v2;
		v2.x = p[2].x;
		v2.y = p[2].y;
		v2.z = p[2].z;

		#if defined( DO_VLOG)
			i3Color::Set( &c, 0, 0, 255, 128);
			i3VLog::SetColor( &c);
			i3VLog::Tri( &p[0], &p[1], &p[2]);
		#endif

		if( pInfo->_bPick)
		{
			if (D3DXIntersectTri(&v0, &v1, &v2, &vPickRayOrig, &vPickRayDir, nullptr, nullptr, &t))
			//if( i3Math::TestTriPoint( &p[0], &p[1], &p[2], pInfo->_pL1, &dir))
			{
				//IntersectTriLine( pInfo->_pL1, pInfo->_pL2, &n, &p[0], &pc, &t);
				t /= dir_len;

				#if defined( DO_VLOG)
					i3Color::Set( &c, 102, 244, 128, 128);
					i3VLog::SetColor( &c);
					i3VLog::Tri( &p[0], &p[1], &p[2]);

					i3Color::Set( &c, 255, 0, 0, 255);
					i3VLog::SetColor( &c);

					VEC3D v1, v2;
					i3Vector::Set( &v1, pc.x - 0.1f, pc.y, pc.z);
					i3Vector::Set( &v2, pc.x + 0.1f, pc.y, pc.z);
					i3VLog::Line( &v1, &v2);

					i3Vector::Set( &v1, pc.x, pc.y - 0.1f, pc.z);
					i3Vector::Set( &v2, pc.x, pc.y + 0.1f, pc.z);
					i3VLog::Line( &v1, &v2);
				#endif

//				if( (t >= 0.0f) && (t <= 1.0f))
				{
					if( pInfo->_ClosestT > t)
					{
						pInfo->_ClosestT = t;
						pInfo->_TriIndex = i;
						pInfo->_pHitGeoAttr = pGeoAttr;
					}
				}
			}
		}
		else
		{
			i3::pack::RECT rect;

			rect.left	=	pInfo->_pL1->x;
			rect.top	=	pInfo->_pL1->y;
			rect.right	=	pInfo->_pL2->x;
			rect.bottom	=	pInfo->_pL2->y;

			i3Matrix::TransformPoints( p, p, 3, pStack->GetTop());	// Matrix Stack에 View * Projection Matrix가 들어있다.

			// Range Check
			if( testTriRect( p, &rect))
			{
				// 영역 선택은 Multi-Select의 기능이기 때문에 해당 위치에서 바로 선택 상태로 설정.
				// i3GeometryResolver에 의해 사용되고 있기 때문에 이미 Lock되어 있고, 직접 변경가능.
				if( pInfo->_bCollisionEdit)
				{
					_ChangeSel( pVA, idx, pInfo->_bSelect, pInfo->_SelMode);
				}

				bPicked = true;
				pInfo->_ClosestT = 0.5f;
				pInfo->_TriIndex = i;
				pInfo->_pHitGeoAttr = pGeoAttr;
			}
		}
	}

	if( bPicked)
		pInfo->_pElement->addSelectedGeoAttr( pGeoAttr);
}

static I3SG_TRAVERSAL_RESULT _HitRaycastProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	I3LV_HIT_RAYCAST_INFO * pInfo = (I3LV_HIT_RAYCAST_INFO *) pUserData;
	INT32 i;
	i3GeometryAttr * pGeoAttr;
	VEC3D ls, le, * pOldL1, * pOldL2;
	MATRIX inv;

	if( !i3::same_of<i3Geometry*>(pNode))
		return I3SG_TRAVERSAL_CONTINUE;
	
	i3Geometry * pGeo = (i3Geometry *) pNode;

	pOldL1 = pInfo->_pL1;
	pOldL2 = pInfo->_pL2;

	if((pGeo->GetBound() != nullptr) || (pInfo->_bPick))
	{
		i3Matrix::Inverse( &inv, nullptr, pStack->GetTop());

		i3Vector::TransformCoord( &ls, pInfo->_pL1, &inv);
		i3Vector::TransformCoord( &le, pInfo->_pL2, &inv);

		pInfo->_pL1 = &ls;
		pInfo->_pL2 = &le;
	}

	if( pGeo->GetBound() != nullptr)
	{
		// Bound가 있다면 다시 확인

		i3BoundBox * pBox = pGeo->GetBound();

		if( !ContainAABBoxLine( &ls, &le, pBox->GetMin(), pBox->GetMax(), 100000.0f))
		{
			pInfo->_pL1 = pOldL1;
			pInfo->_pL2 = pOldL2;
			return I3SG_TRAVERSAL_CONTINUE;
		}
	}

	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
	{
		pGeoAttr = (i3GeometryAttr *) pGeo->GetGeometryAttr(i);

		_doRaycast( pGeoAttr, pInfo, pStack);
	}

	pInfo->_pL1 = pOldL1;
	pInfo->_pL2 = pOldL2;

	return I3SG_TRAVERSAL_CONTINUE;
}

REAL32 i3LevelElement3D::OnTouch( VEC3D * pStart, VEC3D * pEnd, MATRIX * pViewProj, bool bPick, bool bSelect, SEL_MODE selMode)
{
	I3LV_HIT_RAYCAST_INFO info;
	i3MatrixStack stack;

	info._pL1 = pStart;
	info._pL2 = pEnd;
	info._pViewProj = pViewProj;
	info._bCollisionEdit = false;
	info._bSelect = bSelect;
	info._SelMode = selMode;
	info._pElement = this;
	info._bPick = bPick;

	i3LevelViewport * pViewport = i3LevelViewport::GetCurrentFocusViewport();
	i3LevelFramework *	pFramework = pViewport->getFramework();

	i3Vector::Normalize( &info._AtDir, i3Matrix::GetAt( pFramework->GetCurrentCamera()->GetTransform()));

	MATRIX topMtx;

	i3Matrix::Mul( &topMtx, GetTransform(), pViewProj);

	stack.Push( &topMtx, nullptr);

#if defined( DO_VLOG)
	i3VLog::Begin();
#endif

	if( IsState( I3_LEVEL_STATE_COLLISION_EDIT))
	{
		// Collision Mesh가 보여지고 있다면...

		// i3PhysixShapeSet을 검색하여 실질적인 Raycast를 수행한다.
		//
		// Convex의 경우에는 단순히 전체적으로 선택/비선택의 상태로 바뀌지만
		// Triangle Mesh의 경우에는 개별 Triangle로 선택을 하여야 하기 때문...
		info._bCollisionEdit = true;
		
		m_SelGeoAttrList.clear();

		i3Scene::Traverse( m_pVisibleCollRoot, (I3_SG_TRAVERSE_PROC) _HitRaycastProc, &info, &stack);

		// 선택된 Triangle의 색을 바꾼다.
		if( info._TriIndex != -1)
		{
			i3GeometryResolver resolver;
			i3VertexArray * pVA;
			INT32 idx[3];

			resolver.setGeometryAttr( info._pHitGeoAttr);

			resolver.getVertexIndices( info._TriIndex, idx);
			pVA = info._pHitGeoAttr->GetVertexArray();

			_ChangeSel( pVA, idx, bSelect, selMode);

			i3::vu::push_back_once(m_SelGeoAttrList, info._pHitGeoAttr);
		}
	}
	else
	{
		if( m_pSymbol != nullptr)
		{
			i3Scene::Traverse( m_pSymbol, (I3_SG_TRAVERSE_PROC) _HitRaycastProc, &info, &stack);
		}
	}

#if defined( DO_VLOG)
	i3VLog::End();
#endif

	if( info._TriIndex == -1)
		return -1.0f;

	return info._ClosestT;
}

void i3LevelElement3D::OnChangeColor(void)
{
}

void	i3LevelElement3D::OnSetInstanceInfo(i3::pack::GAME_INSTANCE_2 * pInfo)
{
	if( getRes() == nullptr)
	{
		pInfo->nHashCodeResPath = 0xFFFFFFFF;
		pInfo->HashCode[0] = 0xFFFFFFFF;
		pInfo->HashCode[1] = 0xFFFFFFFF;
		pInfo->HashCode[2] = 0xFFFFFFFF;
	}
	else
	{
		pInfo->nHashCodeResPath = getRes()->getHashCode()[0];
		pInfo->HashCode[0] = getRes()->getHashCode()[1];
		pInfo->HashCode[1] = getRes()->getHashCode()[2];
		pInfo->HashCode[2] = getRes()->getHashCode()[3];
	}
	pInfo->nHashCodeInstClass = CRC32( 0xFFFFFFFF, (UINT8 *) m_szInstanceClass, strlen( m_szInstanceClass));

	// Name
	i3::safe_string_copy( pInfo->m_szName, GetName(), _countof( pInfo->m_szName));

	// Translation
	{
		pInfo->vPos.x = getPos()->x;
		pInfo->vPos.y = getPos()->y;
		pInfo->vPos.z = getPos()->z;
	}

	// Scale
	{
		VEC3D scale;

		GetScale( &scale);

		pInfo->vScale.x = scale.x;
		pInfo->vScale.y = scale.y;
		pInfo->vScale.z = scale.z;
	}

	// Rotation
	{
		QUATERNION rot;
		MATRIX tmp;
		VEC3D * pVec;

		i3Matrix::Copy( &tmp, GetTransform());

		pVec = i3Matrix::GetRight( &tmp);
		i3Vector::Normalize( pVec, pVec);

		pVec = i3Matrix::GetUp( &tmp);
		i3Vector::Normalize( pVec, pVec);

		pVec = i3Matrix::GetAt( &tmp);
		i3Vector::Normalize( pVec, pVec);

		i3Matrix::GetRotateQuat( &rot, &tmp);
		
		i3Quat::Normalize( &rot, &rot);

		pInfo->qRot.x = rot.x;
		pInfo->qRot.y = rot.y;
		pInfo->qRot.z = rot.z;
		pInfo->qRot.w = rot.w;
	}

	pInfo->bInitEnable = (IsStyle( I3_LEVEL_STYLE_INIT_DISABLE) == FALSE);
	pInfo->nIDPortal = 0;

	switch( getDynamicState())
	{
		case I3LV_DYNAMIC_DYNAMIC :		pInfo->nDynamicState = I3GAME_DYNAMIC_STATE_DYNAMIC;	break;
		case I3LV_DYNAMIC_KINEMATIC :	pInfo->nDynamicState = I3GAME_DYNAMIC_STATE_KINEMATIC;	break;
		case I3LV_DYNAMIC_STATIC :		pInfo->nDynamicState = I3GAME_DYNAMIC_STATE_STATIC;	break;
		default :						pInfo->nDynamicState = I3GAME_DYNAMIC_STATE_NONE;		break;
	}	

	pInfo->nNetSyncMode = (UINT8) getNetworkSyncMode();
}

i3GameObj *	i3LevelElement3D::CreateGameInstance(void)
{
	return nullptr;
}

void i3LevelElement3D::OnQueryCollisionNode( i3::vector<i3Node*>& List)
{
	List.push_back( m_pSymbol);
}

void i3LevelElement3D::OnChangeInstanceClassName(void)
{
}

void i3LevelElement3D::setLayer( i3LevelLayer * pLayer)
{
	if( m_pLayer != pLayer)
	{
		if( m_pLayer != nullptr)
		{
			m_pLayer->RemoveElement( this);
		}

		m_pLayer = pLayer;

		pLayer->AddElement( this);
	}
}

void i3LevelElement3D::setGroup( i3LevelGroup * pGrp)
{
	if( m_pGroup != pGrp)
	{
		if( m_pGroup != nullptr)
		{
			m_pGroup->RemoveElement( this);
		}

		m_pGroup = pGrp;

		pGrp->AddElement( this);
	}
}

bool i3LevelElement3D::OnRemoveIcon( i3LevelControl * pElm)
{
	return true;
}

void i3LevelElement3D::OnMovingIcon( i3LevelControl * pElm)
{
}

void i3LevelElement3D::OnScalingIcon( i3LevelControl * pElm)
{
}

void i3LevelElement3D::OnRotatingIcon( i3LevelControl * pElm)
{
}

void i3LevelElement3D::OnSelectedIcon( i3LevelControl * pElm)
{
}

void i3LevelElement3D::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelPropertyContainer::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of<i3LevelElement3D*>(pDest));

	i3LevelElement3D * pElement = (i3LevelElement3D*)pDest;

	pElement->SetStyle( GetStyle());
	pElement->setDynamicState( getDynamicState());
	pElement->setNetworkSyncMode( getNetworkSyncMode());

	pElement->setInstanceClassName( getInstanceClassName());

	MATRIX * pTransform = pElement->GetTransform();
	i3Matrix::Copy( pTransform, GetTransform());

	if( m_pLayer != nullptr)
	{
		pElement->setLayer( m_pLayer);
	}

	if( m_pGroup != nullptr)
	{
		pElement->setGroup( m_pGroup);
	}

	pElement->removeAllTarget();
	for( size_t i = 0; i < m_TargetList.size(); ++i)
	{
		pElement->addTarget( getTarget( i));
	}

	if( m_pRes != nullptr)
	{
		pElement->BindRes( m_pRes);
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ELEMENT3D
	{
		UINT8			m_ID[4] = { 'T', 'E', '3', '3'  };
		UINT32			m_Style = 0;

		i3::pack::MATRIX	m_Matrix;
		UINT32			m_Color = 0;

		OBJREF			m_pLayer = 0;

		UINT8			m_DynamicState = 0;
		UINT8			m_NetworkSyncMode = 0;
		char			m_szInstanceClass[128] = { 0 };

		OBJREF			m_pGroup = 0;

		INT32			m_TargetCount = 0;

		char			m_szResPath[MAX_PATH] = { 0 };

		UINT32			m_Reserved[16] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3LevelElement3D::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::ELEMENT3D Info;

	Result = i3LevelPropertyContainer::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	{
		Info.m_Style			= GetStyle();

		if( m_pRes != nullptr)
		{
			i3::string_ncopy_nullpad( Info.m_szResPath, m_pRes->getPath(), _countof( Info.m_szResPath) - 1);
		}

		i3::string_ncopy_nullpad( Info.m_szInstanceClass, m_szInstanceClass, _countof( Info.m_szInstanceClass) - 1);

		Info.m_DynamicState		= (UINT8) getDynamicState();
		Info.m_NetworkSyncMode	= (UINT8) getNetworkSyncMode();

		i3Matrix::Copy( & Info.m_Matrix, GetTransform());

		Info.m_pGroup	=	pResFile->GetObjectPersistID( getGroup());
		Info.m_pLayer	=	pResFile->GetObjectPersistID( getLayer());

		Info.m_TargetCount	=	getTargetCount();

		{
			COLOR col;

			GetColor( &col);

			Info.m_Color = i3Color::ConvertRGBA32( &col);
		}

		Rc = pResFile->Write( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3LevelElement3D::OnSave()", "Could not write object information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Target Object List
	if( Info.m_TargetCount > 0)
	{
		Rc = SaveFromListToResourceFile(m_TargetList, pResFile);	//m_TargetList.SaveTo( pResFile);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	// 추가 GameData가 있다면 저장.
	{
		i3GameObj * pObj = CreateGameInstance();

		if( pObj != nullptr)
		{
			Rc = pObj->OnSave( pResFile);
			I3_CHKIO( Rc);
			Result += Rc;

			I3_SAFE_RELEASE( pObj);
		}
	}

	return Result;
}

bool	i3LevelElement3D::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3LevelPropertyContainer::OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	if( m_pRes != nullptr)
	{
		pXML->SetAttribute( "Res",				m_pRes->getPath());
	}
	else
		pXML->SetAttribute( "Res",				"");

	pXML->addAttr( "Style", GetStyle());
	pXML->SetAttribute( "DynamicState",		getDynamicState());
	pXML->SetAttribute( "NetworkSyncMode",	getNetworkSyncMode());
	pXML->SetAttribute( "InstanceClass",	getInstanceClassName());

	i3Matrix::SetToXML( pXML, "Transform",	GetTransform());

	pXML->SetAttribute( "Group",			(INT32) pFile->GetObjectPersistID( getGroup()));
	pXML->SetAttribute( "Layer",			(INT32) pFile->GetObjectPersistID( getLayer()));

	{
		COLOR col;

		GetColor( &col);

		i3Color::SetToXML( pXML, "Color", &col);
	}

	{
		SaveFromListToXML(m_TargetList, "TargetList", pFile, pXML, true);		// m_TargetList.SaveToXML( "TargetList", pFile, pXML, true);
	}

	{
		i3GameObj * pObj = nullptr;
		
		pObj = CreateGameInstance();

		if((pObj != nullptr) && (pObj->IsExternRes() == FALSE))
		{
			i3XMLElement * pChild = new i3XMLElement( "GameInstanceObject");
			
			pChild = (i3XMLElement*) pXML->LinkEndChild( pChild);
			I3ASSERT( pChild != nullptr);

			pObj->OnSaveXML( pFile, pChild);

			I3_MUST_RELEASE(pObj);
		}
	}

	return true;
}

UINT32 i3LevelElement3D::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::ELEMENT3D Info;
	
	Result = i3LevelPropertyContainer::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pResFile->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelElement3D::OnLoad()", "Cuould not read object information.");
		return STREAM_ERR;
	}
	Result += Rc;

	setInstanceClassName( Info.m_szInstanceClass);
	
	{
		Info.m_Style = (m_Style & ~I3_LEVEL_LOAD_MASK) | (Info.m_Style & I3_LEVEL_LOAD_MASK);
		SetStyle( Info.m_Style);

		if( ( IsStyle( I3_LEVEL_STYLE_NO_FILERES) == FALSE) && ( Info.m_szResPath[0] != 0))
		{
			i3LevelRes * pRes;
			char szFull[ MAX_PATH];

			i3::string_ncopy_nullpad( m_szResPath, Info.m_szResPath, _countof( m_szResPath) - 1);

			pRes = g_pResDB->FindResByPath( Info.m_szResPath);

			if( pRes == nullptr)
			{
				sprintf_s( szFull, _countof( szFull)-1, "%s/%s", g_szResPath, Info.m_szResPath);

				pRes = g_pResDB->AddRes( szFull);
				
				if( pRes == nullptr)
				{
					// 그래도 결국 Resource를 읽어들이지 못했다면...					
					AddState( I3_LEVEL_STATE_FAILED_BINDRES);
				}
			}
			
			if( pRes != nullptr)
			{
				if( pRes->OnValidate( g_pScene, false))
				{
					BindRes( pRes);
				}
			}
		}

		if( Info.m_pLayer != 0)
		{
			i3LevelLayer * pLayer = (i3LevelLayer *) pResFile->FindObjectByID( Info.m_pLayer);
			
			if( pLayer != nullptr)
			{
				setLayer( pLayer);
			}
		}

		if( Info.m_pGroup != 0)
		{
			i3LevelGroup * pGroup = (i3LevelGroup *) pResFile->FindObjectByID( Info.m_pGroup);
			I3ASSERT( pGroup != nullptr);

			setGroup( pGroup);
		}

		i3Matrix::Copy( GetTransform(), & Info.m_Matrix);

		{
			COLOR col;

			//if( Info.m_Color != 0xFFFFFFFF)
			{
				i3Color::ConvertRGBA32( Info.m_Color, &col);
				SetColor( &col);
			}
		}

		setDynamicState( (I3LV_DYNAMIC_STATE) Info.m_DynamicState);
		setNetworkSyncMode( (I3_NETWORK_GAMEOBJECT_TYPE) Info.m_NetworkSyncMode);
	}

	// Target List
	if( Info.m_TargetCount > 0)
	{
		Rc = LoadFromResourceFileToList(m_TargetList, pResFile, true);	// m_TargetList.LoadFrom( pResFile, true);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	{
		i3GameObj * pObj = CreateGameInstance();

		if((pObj != nullptr) && (pObj->IsExternRes() == FALSE))
		{
			Rc = pObj->OnLoad( pResFile);
			I3_CHKIO( Rc);
			Result += Rc;

			I3_MUST_RELEASE(pObj);
		}
	}

	return Result;
}

bool	i3LevelElement3D::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;
	const char * pszStr;
	UINT32 uval;
	INT32 ival;
	i3LevelRes * pRes = nullptr;

	rv = i3LevelPropertyContainer::OnLoadXML( pFile, pXML);
	if( rv == false)
		return false;

	// Instance Class
	pszStr = pXML->getAttr( "InstanceClass");
	I3ASSERT( pszStr != nullptr);
	setInstanceClassName( (char *) pszStr);

	// Style
	{
		pXML->getAttr( "Style", & uval);

		m_Style = (m_Style & ~I3_LEVEL_LOAD_MASK) | (uval & I3_LEVEL_LOAD_MASK);
	}
	
	// Resource
	if( IsStyle( I3_LEVEL_STYLE_NO_FILERES) == FALSE)
	{
		pszStr = pXML->getAttr( "Res");
		
		if((pszStr != nullptr) && (pszStr[0] != 0))
		{
			char szFull[ MAX_PATH];

			i3::string_ncopy_nullpad( m_szResPath, pszStr, _countof( m_szResPath) - 1);

			pRes = g_pResDB->FindResByPath( pszStr);

			if( pRes == nullptr)
			{
				sprintf( szFull, "%s/%s", g_szResPath, pszStr);

				pRes = g_pResDB->AddRes( szFull);

				if( pRes == nullptr)
				{
					// 그래도 결국 Resource를 읽어들이지 못했다면...
					AddState( I3_LEVEL_STATE_FAILED_BINDRES);
				}
			}
		}

		if( pRes != nullptr)
		{
			if( pRes->OnValidate( g_pScene, false))
			{
				BindRes( pRes);
			}
		}
	}

	// Layer
	{
		i3LevelLayer * pLayer;

		pXML->getAttr( "Layer", &uval);

		if( uval != 0)
		{
			pLayer = (i3LevelLayer *) pFile->FindObjectByID( uval);
			if( pLayer != nullptr)
			{
				setLayer( pLayer);
			}
		}
	}

	// Group
	{
		i3LevelGroup * pGroup;

		if( pXML->getAttr( "Group", &uval) != nullptr)
		{
			if( uval != 0)
			{
				pGroup = (i3LevelGroup *) pFile->FindObjectByID( uval);
				I3ASSERT( pGroup != nullptr);

				setGroup( pGroup);
			}
		}
	}
	
	// Transform
	i3Matrix::GetFromXML( pXML, "Transform", GetTransform());

	// Color
	{
		COLOR col;

		i3Color::GetFromXML( pXML, "Color", &col);

		SetColor( &col);
	}

	// Dynamic State
	{
		pXML->getAttr( "DynamicState", &ival);

		setDynamicState( (I3LV_DYNAMIC_STATE) ival);
	}

	// Network Sync Mode
	{
		pXML->getAttr( "NetworkSyncMode", &ival);

		setNetworkSyncMode( (I3_NETWORK_GAMEOBJECT_TYPE) ival);
	}

	// Target List
	{
		LoadFromXMLToList(m_TargetList, "TargetList", pFile, pXML, true);	// m_TargetList.LoadFromXML( "TargetList", pFile, pXML, true);
	}

	// Additional Object Data
	{
		i3GameObj * pObj = CreateGameInstance();

		if((pObj != nullptr) && (pObj->IsExternRes() == FALSE))
		{
			i3XMLElement * pChild = (i3XMLElement *) pXML->FirstChild( "GameInstanceObject");

			// 초기 버전에선 새로운 XML Node의 구분없이 저장을 했었고,
			// 그것과의 호환성을 유지시키기 위한 코드
			if( pChild == nullptr)
				pChild = pXML;

			if( pObj->OnLoadXML( pFile, pChild) == false)
			{
				I3ERRLOG( i3Error::_READFAIL);

				I3_SAFE_RELEASE(pObj);		// 익스턴 여부와 상관없이..무조건 릴리즈해야함..(2012.09.11.수빈)
				return false;
			}
		}
		
		I3_SAFE_RELEASE(pObj);		// 익스턴 여부와 상관없이..무조건 릴리즈해야함..(2012.09.11.수빈)
	}

	return true;
}

void	i3LevelElement3D::OnAttachScene( i3LevelScene * pScene )
{
	/*if( nullptr != m_pSymbol )
		i3Scene::ModifyDynamicState( m_pSymbol, 0, I3_PHYSIX_STATE_DISABLE );*/
}

void	i3LevelElement3D::OnDetachScene( i3LevelScene * pScene )
{
	/*if( nullptr != m_pSymbol )
		i3Scene::ModifyDynamicState( m_pSymbol, I3_PHYSIX_STATE_DISABLE, 0 );*/
}