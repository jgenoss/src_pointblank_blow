#include "i3SceneDef.h"
#include "i3SceneGraphContext.h"
#include "i3AttrSet.h"
#include "i3AllAttrs.h"
#include "i3SceneTracer.h"
#include "i3SceneDebug.h"
#include "i3SceneObject.h"
#include "i3ParticleRender.h"

#include "i3Base/smart_ptr/make_shared.h"
#include "i3Base/itl/vector_util.h"
#include "i3XrayScene.h"


I3_CLASS_INSTANCE( i3SceneGraphContext);

i3SceneGraphContext::~i3SceneGraphContext(void)
{
	removeAllLights();
	removeAllCameras();

	I3_SAFE_RELEASE( m_pShadowMap);

	I3_SAFE_RELEASE( m_p3DRoot);
	I3_SAFE_RELEASE( m_p2DMainRoot);
	I3_SAFE_RELEASE( m_p3DMainRoot);
	I3_SAFE_RELEASE( m_pParticleRoot);
	I3_SAFE_RELEASE( m_pCurCamera);
	I3_SAFE_DELETE(m_pXrayScene);
}

void i3SceneGraphContext::AddCurrentPortalMask( bool bAdd, INT32 idx)
{
	I3ASSERT( idx >= 0 && idx < 64);

	if( bAdd)
	{
		m_curPortalMask |= ((UINT64)1<<idx);
	}
	else
	{
		m_curPortalMask &= ~((UINT64)1<<idx);
	}
}

bool i3SceneGraphContext::Create(void)
{
	m_pLightList = i3::make_shared<i3::vector<i3Light*> >();
		
	m_pLightList->reserve( 64);

	I3ASSERT( m_pLightList);

	m_pCameraList = i3::make_shared<i3::vector<i3Camera*> >();
	m_pCameraList->reserve( 16);
	I3ASSERT( m_pCameraList);

	// 3D Root
	{
		m_p3DMainRoot = i3Node::new_object();

		m_p3DRoot = i3LayerRootNode::new_object();
		m_pParticleRoot = i3ParticleRenderNode::new_object();

		m_p3DMainRoot->AddChild( m_p3DRoot);
		m_p3DMainRoot->AddChild( m_pParticleRoot);

		#if defined( I3_DEBUG)  && defined( I3_WINDOWS)
		m_p3DRoot->SetName( "3D Layer Root");
		#endif

		{
			i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();

			pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
			pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT);

			m_p3DRoot->AddAttr( pAttr);
		}

		{
			m_pAmbient = i3AmbientAttr::new_object_ref();

			m_p3DRoot->AddAttr( m_pAmbient);
		}
	}

	// 2D Root
	{
		m_p2DMainRoot = i3LayerRootNode::new_object();

		#if defined( I3_DEBUG)  && defined( I3_WINDOWS)
		m_p2DMainRoot->SetName( "2D Layer Root");
		#endif

		_Init2DDefaultAttrs();
	}

	{
		m_pXrayScene = new i3XrayScene;
	}
	return true;
}

void i3SceneGraphContext::_Init2DDefaultAttrs(void)
{
	m_p2DRoot = i3AttrSet::new_object_ref();

	#if 0
	{
		i3RenderModeAttr * pAttr = i3RenderModeAttr::new_object_ref();

		pAttr->Set( I3G_RENDER_LINE);
		m_p2DRoot->AddAttr( pAttr);
	}

	m_p2DRoot->SetPriority( 0);
	#endif

	#if defined( I3_DEBUG) && defined( I3_WINDOWS)
		m_p2DRoot->SetName( "2D GuiRoot");
	#endif

	m_p2DMainRoot->AddChild( m_p2DRoot);


	// Texture Enable = true
	{
		i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();

		pAttr->Set(true);

		m_p2DRoot->AddAttr( pAttr);
	}

	// Z Writer Enable = false;
	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();

		pAttr->Set(false);

		m_p2DRoot->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();

		pAttr->Set(false);

		m_p2DRoot->AddAttr( pAttr);
	}

	{
		i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();

		pAttr->Set( I3G_FACE_CULL_NONE);

		m_p2DRoot->AddAttr( pAttr);
	}

	// Lighting Enable = false
	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

		pAttr->Set(false);

		m_p2DRoot->AddAttr( pAttr);
	}

	// Alpha Test Enable = true
	{
		i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::new_object_ref();

		pAttr->Set(true);

		m_p2DRoot->AddAttr( pAttr);
	}

	// Alpha Function
	{
		i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();

		pAttr->SetFunction( I3G_COMP_GREATER);
		pAttr->SetRefValue( 0);

		m_p2DRoot->AddAttr( pAttr);
	}

	// Blend
	{
		i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();

		pAttr->Set(true);
		m_p2DRoot->AddAttr( pAttr);
	}

	// Blend Mode
	{
		i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

		pAttr->SetSource( I3G_BLEND_SRCALPHA);
		pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
		m_p2DRoot->AddAttr( pAttr);
	}

	// Projection Matrix Attr
	{
		i3ProjectMatrixAttr * pAttr = i3ProjectMatrixAttr::new_object_ref();
		m_p2DRoot->AddAttr( pAttr);
	}

	// View Matrix Attr
	{
		i3ViewMatrixAttr * pAttr = i3ViewMatrixAttr::new_object_ref();
		m_p2DRoot->AddAttr( pAttr);
	}

	{
		i3TextureFunctionAttr * pAttr = i3TextureFunctionAttr::new_object_ref();

		pAttr->SetFunction( I3G_TEXTURE_FUNC_MODULATE);

		m_p2DRoot->AddAttr( pAttr);
	}

	{
		i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();

		pAttr->SetMagFilter( I3G_TEXTURE_FILTER_POINT);
		pAttr->SetMinFilter( I3G_TEXTURE_FILTER_POINT);

		m_p2DRoot->AddAttr( pAttr);
	}

	{
		i3TextureWrapAttr * pAttr = i3TextureWrapAttr::new_object_ref();

		pAttr->SetHorzWrap( I3G_TEXTURE_WRAP_CLAMP);
		pAttr->SetVertWrap( I3G_TEXTURE_WRAP_CLAMP);

		m_p2DRoot->AddAttr( pAttr);
	}

	{
		i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
		COLOR col;

		i3Color::Set( &col, (UINT8) 255, 255, 255, 255);
		pAttr->Set( &col);

		m_p2DRoot->AddAttr( pAttr);
	}	
}

void i3SceneGraphContext::reset(void)
{
	if( m_pShadowMap != nullptr)
	{
		m_pShadowMap->Reset();
	}

	removeAllLights();
	removeAllCameras();
}

void i3SceneGraphContext::addDebugLayer(void)
{
	//	2D Debug Text Root
	{
		m_pDebugTextRoot = i3AttrSet::new_object_ref();
		m_p2DRoot->CopyTo( m_pDebugTextRoot, I3_COPY_INSTANCE);

		#if defined( I3_DEBUG) && defined( I3_WINDOWS)
		m_pDebugTextRoot->SetName( "Debug Text Root");
		#endif

		m_p2DMainRoot->AddChild( m_pDebugTextRoot);
	}
}

///////////////////////////////////////////////////////////////////////
// Light
//

void i3SceneGraphContext::addLight( i3Light * pLight)
{
	I3_MUST_ADDREF( pLight);
	m_pLightList->push_back( pLight);
}

void i3SceneGraphContext::removeLight( i3Light * pLight)
{
	

	I3ASSERT( pLight != nullptr);

	size_t idx = i3::vu::index_of(*m_pLightList, pLight);

	if( idx != m_pLightList->size() )
	{
		I3_MUST_RELEASE( pLight);

		m_pLightList->erase(m_pLightList->begin() + idx);
	}
}

void i3SceneGraphContext::removeAllLights(void)
{
	
	i3Light * pLight;

	for(size_t i = 0; i < m_pLightList->size(); i++)
	{
		pLight = (*m_pLightList)[i];

		I3_MUST_RELEASE( pLight);
	}

	m_pLightList->clear();
}

///////////////////////////////////////////////////////////////////////
// Camera
//
void i3SceneGraphContext::addCamera( i3Camera * pCamera)
{
	I3_MUST_ADDREF( pCamera);
	m_pCameraList->push_back( pCamera);
}

void i3SceneGraphContext::removeCamera( i3Camera * pCamera)
{
	

	I3ASSERT( pCamera != nullptr);

	size_t idx = i3::vu::index_of(*m_pCameraList, pCamera);

	if( idx != m_pCameraList->size() )
	{
		I3_MUST_RELEASE( pCamera);
		m_pCameraList->erase(m_pCameraList->begin() + idx);
	}
}

void i3SceneGraphContext::removeAllCameras(void)
{
	
	i3Camera * pCamera;

	for(size_t i = 0; i < m_pCameraList->size(); i++)
	{
		pCamera = (*m_pCameraList)[i];

		I3_MUST_RELEASE( pCamera);
	}

	m_pCameraList->clear();
}

void i3SceneGraphContext::_UpdateRT( i3Camera * pCam, i3SceneTracer * pTracer)
{
	I3SG_TRACER_CONTEXT ctx;
	i3Camera * pOldCam = pTracer->getCurrentCamera();

	// i3SceneTracer::SetCamera를 직접 호출하지 않는 것은
	// SetCamera의 내부에서 View Matrix 및 Projection Matrix뿐만 아니라
	// i3SoundContext의 Listner Matrix까지 변경하기 때문이다.
	// 이 처리는 매우 느릴 수 있다.
	{
		pTracer->SaveContext( &ctx);

		pTracer->GetModelViewMatrixStack()->Push( pCam->GetCacheMatrix(), pCam->GetMatrix());
		pTracer->SetCamera( pCam);

		pTracer->GetModelViewMatrixStack()->Pop();

		pTracer->GetModelViewMatrixStack()->PushIdentity();

		pTracer->addState( I3SG_TRACER_STATE_OFFSCREEN);
		pTracer->addDisableNodeMask(I3_NODEFLAG_DISABLE_RENDER_OFFSCREEN);

		//pTracer->PushOutputList( pCam->getOutputList());

		pTracer->pushAttr( pCam->getRenderTargetAttr(), 0);
		pTracer->pushAttr( pCam->getClearRenderTargetAttr(), 0);
	}

	if( pCam->getSubCamSg() == nullptr)
	{
		// Camera가 별도의 Rendering용 Sg.를 가지고 있지 않다면,
		// 일반적인 3D Root부터 Rendering을 한다.
		pTracer->Trace( get3DMainRoot());
	}
	else
	{
		pTracer->PreTrace(pCam->getSubCamSg());
		pTracer->Trace( pCam->getSubCamSg());

		//g_pParticleRender->Render( pTracer);
	}

	{
		pTracer->popAttr( I3_ATTRID_CLEARRENDERTARGET, 0);
		pTracer->popAttr( I3_ATTRID_RENDERTARGET, 0);
		pTracer->removeState( I3SG_TRACER_STATE_OFFSCREEN);
		pTracer->removeDisableNodeMask(I3_NODEFLAG_DISABLE_RENDER_OFFSCREEN);


		pTracer->GetModelViewMatrixStack()->Pop();

		pTracer->RestoreContext( &ctx);

		// Main Output List에 추가.
		pTracer->AddAttributeToOutput( pCam->getOutputList());

		pTracer->ResetAlphaSortNodeList();
	}

//	pTracer->popAttr(I3_ATTRID_VIEW_MATRIX);
//	pTracer->popAttr(I3_ATTRID_PROJECT_MATRIX);
//	pTracer->GetModelViewMatrixStack()->Push( pOldCam->GetCacheMatrix(), pOldCam->GetMatrix());
	pTracer->SetCamera( pOldCam);
//	pTracer->GetModelViewMatrixStack()->Pop();
}

///////////////////////////////////////////////////////////////////////
//
void i3SceneGraphContext::OnUpdate( i3SceneTracer * pTracer)
{
	INT32 i;
	i3Light * pLight;


	// 이전 Frame에 사용된 Lighting은 Shadow 처리한다.
	// 이런 방법의 경우, Shadow가 적용되는 것에 1 Frame의 시간적 차이가 발생할 수 있으나,
	// 별도의 Culling 처리를 하는 것보다 효과적이라 판단.
	for( i = 0; i < getLightCount(); i++)
	{
		pLight = getLight( i);

		if( pLight->isState( I3G_LIGHT_STATE_DISABLE) == false)
		{
			if( pLight->isStyle( I3G_LIGHT_STYLE_SHADOWMAP))
			{
				if( pLight->isState( I3G_LIGHT_STATE_APPLIED))
				{
					_BuildShadowMap( pLight, pTracer);
				}
			}
		}

		// 다음 Frame에서의 처리를 위해 State Clear...
		pLight->removeState( I3G_LIGHT_STATE_APPLIED);
	}

	// Camera들에 대해 RenderTarget Update가 필요한 것들을 처리한다.
	m_idxLastSubCamera ++;

	if( m_idxLastSubCamera >= getCameraCount())
		m_idxLastSubCamera = 0;


	for( i = 0; i < getCameraCount(); i++)
	{
		i3Camera * pCam = getCamera( i);

		if((pCam->GetEnable() == false) || ((pCam->getStyle() & I3_CAM_STYLE_SUB) == 0))
		{				
			continue;
		}

		if( (pCam->getStyle() & I3_CAM_STYLE_FULLFRAME) )
		{
			_UpdateRT( pCam, pTracer);
		}
		else
		{
			if( i == m_idxLastSubCamera)
			{
				_UpdateRT( pCam, pTracer);
			}
		}
	}

}

void i3SceneGraphContext::_BuildShadowMap( i3Light * pLight, i3SceneTracer * pTracer)
{
	bool bGlobalBound = true;

	if( m_pShadowMap == nullptr)
	{
		m_pShadowMap = i3ShadowMap::new_object();
		m_pShadowMap->Create();
	}

	if( pLight->getShadowMap() == nullptr)
	{
		// 아직 ShadowMap을 생성하지 않았다면...
		pLight->CreateShadowMap();
	}

	// 전체 Scene의 BoundBox를 계산한다.
	bGlobalBound = (pLight->GetLightType() == I3G_LIGHT_DIRECTIONAL) && (pLight->isStyle(I3G_LIGHT_STYLE_CALC_BOUND) == false);

	if( bGlobalBound)
	{
		m_pShadowMap->Apply( pLight, pTracer, get3DRoot(), &m_GlobalBoundBox);
	}
	else
	{
		m_BoundBox.Reset();

		CalcBoundWST( pLight, &m_BoundBox);

		if( m_BoundBox.IsEmpty())
		{
			// 최소한의 공간을 설정
			VEC3D temp, vmin, vmax;

			i3Vector::Set( &temp, 1.0f, 1.0f, 1.0f);

			i3Vector::Sub( &vmin, m_BoundBox.GetMin(), &temp);
			i3Vector::Add( &vmax, m_BoundBox.GetMax(), &temp);

			m_BoundBox.SetMinMax( &vmin, &vmax);
		}

		m_pShadowMap->Apply( pLight, pTracer, get3DRoot(), &m_BoundBox);
	}
}

void i3SceneGraphContext::UpdateGlobalBound( bool bShadow)
{
	VEC3D vmin, vmax;

	get3DRoot()->GetWrappingBound( &m_GlobalBoundBox, bShadow);

	if( m_GlobalBoundBox.IsEmpty())
	{
		i3Vector::Set( &vmin, -5.0f, -5.0f, -5.0f);
		i3Vector::Set( &vmax,  5.0f,  5.0f,  5.0f);

		m_GlobalBoundBox.SetMinMax( &vmin, &vmax);
	}
	else
	{
		VEC3D add;

		i3Vector::Set( &add, 1.0f, 1.0f, 1.0f);

		i3Vector::Copy( &vmin, m_GlobalBoundBox.GetMin());
		i3Vector::Sub( &vmin, &vmin, &add);

		i3Vector::Copy( &vmax, m_GlobalBoundBox.GetMax());
		i3Vector::Add( &vmax, &vmax, &add);

		m_GlobalBoundBox.SetMinMax( &vmin, &vmax);
	}
}

#if 0
static I3SG_TRAVERSAL_RESULT	_CalcBoundWSTProc( i3Node * pNode, void * pData, i3MatrixStack * pStack)
{
	if( pNode->GetBound() == nullptr)
		return I3SG_TRAVERSAL_CONTINUE;

	if( pNode->IsFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE))
		return I3SG_TRAVERSAL_SKIP_NODE;

	i3BoundBox * pBound = (i3BoundBox *) pData;
	VEC3D pos[8];
	INT32 i;

	pNode->GetBound()->GetCornerVertices( pos);

	for( i = 0; i < 8; i++)
	{
		i3Vector::TransformCoord( &pos[i], &pos[i], pStack->GetTop());
		//D3DXVec3TransformCoord( (D3DXVECTOR3 *) &pos[i], (const D3DXVECTOR3 *) &pos[i], (const D3DXMATRIX *) pStack->GetTop());

		pBound->ExtendByVec( &pos[i]);
	}

	return I3SG_TRAVERSAL_SKIP_NODE;
}
#endif // 0

void i3SceneGraphContext::_Rec_CalcBound( i3Light * pLight, i3Node * pNode, i3BoundBox * pBound, i3MatrixStack * pStack)
{
	INT32 i;
	MATRIX cache;

	if( pNode->GetBound() != nullptr)
	{
		VEC3D pos[8], diff;
		INT32 i;
		REAL32 distSq, rangeSq = pLight->getRange() * pLight->getRange();
		bool bAdd = false;
		i3BoundBox tempBox;

		pNode->GetBound()->GetCornerVertices( pos);

		for( i = 0; i < 8; i++)
		{
			//i3Vector::TransformCoord( &pos[i], &pos[i], pStack->GetTop());
			D3DXVec3TransformCoord( (D3DXVECTOR3 *) &pos[i], (const D3DXVECTOR3 *) &pos[i], (const D3DXMATRIX *) pStack->GetTop());

			tempBox.ExtendByVec( &pos[i]);

			if( pLight->GetLightType() == I3G_LIGHT_DIRECTIONAL)
			{
				// 카메라에서의 거리로 바꾸어 계산
				i3Vector::Sub( &diff, i3Matrix::GetPos( m_pCurCamera->GetMatrix()), &pos[i]);
			}
			else
			{
				// Light에서의 거리로 계산
				i3Vector::Sub( &diff, pLight->GetWorldSpacePosition(), &pos[i]);
			}

			distSq = i3Vector::LengthSq( &diff);

			if( distSq < rangeSq)
				bAdd = true;
		}

		if( bAdd)
		{
			pBound->ExtendByBoundBox( &tempBox);
		}

		return;
	}

	if( pNode->IsFlag( I3_NODEFLAG_TRANSFORM))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		pStack->PushPre( &cache, pTrans->GetMatrix());
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		i3Node * pChild = pNode->GetChild( i);

		if( pChild->IsFlag( I3_NODEFLAG_DISABLE))
			continue;

		//if( (pChild->GetFlag() & (I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE_SHADOW_RECEIVER)) == (I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE_SHADOW_RECEIVER))
		if( pChild->GetFlag() & I3_NODEFLAG_DISABLE_SHADOW_CASTER)
			continue;

		_Rec_CalcBound( pLight, pChild, pBound, pStack);
	}

	if( pNode->IsFlag( I3_NODEFLAG_TRANSFORM))
	{
		pStack->Pop();
	}
}

void i3SceneGraphContext::CalcBoundWST( i3Light * pLight, i3BoundBox * pBox)
{
	i3MatrixStack MStack;

	if( pBox == nullptr)
	{
		pBox = &m_BoundBox;
	}

	pBox->Reset();

	if( get3DRoot() != nullptr)
	{
		_Rec_CalcBound( pLight, get3DMainRoot(), pBox, &MStack);
	}
}

