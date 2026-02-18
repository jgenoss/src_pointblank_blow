#include "i3SceneDef.h"
#include "i3AttrStack.h"
#include "i3AttrStackManager.h"
#include "i3AllAttrs.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// i3AttrStackManager
//
// 이 Class는 매우 비번한게 사용되는 Class이기 때문에 속도에 크게 의미를 두고 코딩되어야 한다.
//

// 2003-12-11
//		m_pLastAttrTrack 변수의 값을 매 Frame 마다 처음부터 다시 설정하는 것이 아니라
//		이전 Frame에서 사용된 값을 그대로 유지해서 사용하는 방법으로 수정
//		이는 매 Frame 마다 Default Render State의 값을 설정해 주어야 하는 부분을
//		대폭 감소시켜 실제로 변경되는 Render State의 회수를 최소화 시키기 위함이다.

// 2003-12-12
//		앞서 수정한 m_pLastAttrTrack 변수의 값을 원래 형식이었던 매 Frame 재 갱신하는 것으로 복귀.
//		m_pLastAttrTrack에 등록된 Render Attribute에 대한 Reference들이 사실 상 다음 Frame에 사용될 때
//		그 값이 변경될 수 있기 때문에, 실제 Render Context에 설정되어 있는 값과 괴리가 발생하기 때문이다.
//

I3_CLASS_INSTANCE( i3AttrStackManager);

// i3AttrStackManager class는 Scene Tracer가 Scene Graph를 순회하면서 발견하는 여러 render attribute들의
// 변화 상태를 추적하기 위한 class다.

i3AttrStackManager::i3AttrStackManager(void)
{
	INT32 i;

	memset( m_apLastAttrTrack, 0, sizeof(m_apLastAttrTrack));

	// 모든 종류의 Attribute에 대해 i3AttrStack class를 준비한다.
	// i3AttrStack은 각 개별 Attribute의 상태를 추적하기 위해
	// Stack 구조를 이용하여 관리한다.
	for( i = 0; i < I3_ATTR_MAX; i++)
	{
		m_pAttrStacks[i] = i3AttrStack::new_object();
		m_pAttrStacks[i]->setID( i);
		m_bFlushed[i] = true;			// 첫프레임에서 디폴트 셋팅이 들어가는 초기화를 확실히 해둠..
		m_bIgnoreAlphaSort[i] = false;
	}
	

	// 알파소트때 보관에서 뺄수 있는 불필요한 Attr들을 조금씩 추가할 것..
	m_bIgnoreAlphaSort[I3_ATTRID_RENDER_ATTR_LIST] = true;
	m_bIgnoreAlphaSort[I3_ATTRID_GEOMETRY] = true; 
	m_bIgnoreAlphaSort[I3_ATTRID_PROJECT_MATRIX] = true;
	m_bIgnoreAlphaSort[I3_ATTRID_VIEW_MATRIX] = true; 
	m_bIgnoreAlphaSort[I3_ATTRID_CLEARRENDERTARGET] = true;
	m_bIgnoreAlphaSort[I3_ATTRID_RENDERTARGET] = true;
	
	////////////////////////////////////////////////////////////////////////////////
	// set default attributes to each attribute stack

	// Alpha Function
	{
		i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();

		m_pAttrStacks[I3_ATTRID_ALPHA_FUNC]->SetDefaultAttr( pAttr);
	}

	// Alpha Test Enable
	{
		i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::new_object_ref();

		m_pAttrStacks[I3_ATTRID_ALPHA_TEST_ENABLE]->SetDefaultAttr( pAttr);
	}

	// Ambient
	{
		i3AmbientAttr * pAttr = i3AmbientAttr::new_object_ref();

		m_pAttrStacks[I3_ATTRID_AMBIENT]->SetDefaultAttr( pAttr);
	}

	// Blend Enable
	{
		i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_BLEND_ENABLE]->SetDefaultAttr( pAttr);
	}

	// Blend Mode
	{
		i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_BLEND_MODE]->SetDefaultAttr( pAttr);
	}

	// Clipping Enable
	{
		i3ClippingEnableAttr * pAttr = i3ClippingEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_CLIPPING_ENABLE]->SetDefaultAttr( pAttr);
	}

	// Color
	{
		i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_COLOR]->SetDefaultAttr( pAttr);
	}

	// Face Cull Mode 
	{
		i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_FACECULL_MODE]->SetDefaultAttr( pAttr);
	}

	// Fog Color
	{
		i3FogColorAttr * pAttr = i3FogColorAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_FOG_COLOR]->SetDefaultAttr( pAttr);
	}

	// Fog Density
	{
		i3FogDensityAttr * pAttr = i3FogDensityAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_FOG_DENSITY]->SetDefaultAttr( pAttr);
	}

	// Fog Enable
	{
		i3FogEnableAttr * pAttr = i3FogEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_FOG_ENABLE]->SetDefaultAttr( pAttr);
	}

	// Fog Mode
	{
		i3FogModeAttr * pAttr = i3FogModeAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_FOG_MODE]->SetDefaultAttr( pAttr);
	}

	// Fog Near Far
	{
		i3FogNearFarAttr * pAttr = i3FogNearFarAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_FOG_NEARFAR]->SetDefaultAttr( pAttr);
	}

	// Lighting Enable Attr
	{
		i3LightingEnableAttr * pAttr  = i3LightingEnableAttr::new_object_ref();
		
		m_pAttrStacks[ I3_ATTRID_LIGHTING_ENABLE]->SetDefaultAttr( pAttr);
	}

	// Material
	{
		i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_MATERIAL]->SetDefaultAttr( pAttr);
	}

	// Render Mode
	{
		i3RenderModeAttr * pAttr = i3RenderModeAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_RENDER_MODE]->SetDefaultAttr( pAttr);
	}

	// Shade Mode
	{
		i3ShadeModeAttr * pAttr = i3ShadeModeAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_SHADE_MODE]->SetDefaultAttr( pAttr);
	}

	// Stencil Function
	{
		i3StencilFuncAttr * pAttr = i3StencilFuncAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_STENCIL_FUNC]->SetDefaultAttr( pAttr);
	}

	// Stencil Operation
	{
		i3StencilOperationAttr * pAttr = i3StencilOperationAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_STENCIL_OPERATION]->SetDefaultAttr( pAttr);
	}

	// Stencil Test Enable
	{
		i3StencilTestEnableAttr * pAttr = i3StencilTestEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_STENCILTEST_ENABLE]->SetDefaultAttr( pAttr);
	}

	// Z Function
	{
		i3ZFuncAttr * pAttr = i3ZFuncAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_Z_FUNC]->SetDefaultAttr( pAttr);
	}

	// Z Test Enable
	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_Z_TEST_ENABLE]->SetDefaultAttr( pAttr);
	}

	// Z Write Enable
	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_Z_WRITE_ENABLE]->SetDefaultAttr( pAttr);
	}

	// Z Bias
	{
		i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_Z_BIAS]->SetDefaultAttr( pAttr);
	}

	// Projection Matrix
	{
		i3ProjectMatrixAttr * pAttr = i3ProjectMatrixAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_PROJECT_MATRIX]->SetDefaultAttr( pAttr);
	}

	// View Matrix
	{
		i3ViewMatrixAttr * pAttr = i3ViewMatrixAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_VIEW_MATRIX]->SetDefaultAttr( pAttr);
	}

	// Model View Matrix
	{
		i3ModelViewMatrixAttr * pAttr = i3ModelViewMatrixAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_MODEL_VIEW_MATRIX]->SetDefaultAttr( pAttr);
	}

	// Texture Bind
	{
		i3TextureBindAttr *		pAttr = i3TextureBindAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_TEXTURE_BIND]->SetDefaultAttr( pAttr);

		// Texture Bind는 Stack 구조를 적용하지 않고, 단순히 발견되는 자체로 
		// 선형적으로만 효과를 발휘하도록 한다.
		//m_pAttrStacks[ I3_ATTRID_TEXTURE_BIND]->SetTraceMode( true);
	}

	// Texture Enable
	{
		i3TextureEnableAttr *	pAttr = i3TextureEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_TEXTURE_ENABLE]->SetDefaultAttr( pAttr);
	}

	// Texture Filter
	{
		i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_TEXTURE_FILTER]->SetDefaultAttr( pAttr);
	}

	// Texture Wrap
	{
		i3TextureWrapAttr * pAttr = i3TextureWrapAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_TEXTURE_WRAP]->SetDefaultAttr( pAttr);
	}

	// Texture Coord Source
	{
		i3TextureCoordSourceAttr * pAttr = i3TextureCoordSourceAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_TEXTURE_COORD_SOURCE]->SetDefaultAttr( pAttr);
	}

	// Texture Matrix
	{
		i3TextureMatrixAttr * pAttr = i3TextureMatrixAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_TEXTURE_MATRIX]->SetDefaultAttr( pAttr);
	}

	// Texture Matrix
	{
		i3TextureCoordScaleAttr * pAttr = i3TextureCoordScaleAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_TEXTURE_COORD_SCALE]->SetDefaultAttr( pAttr);
	}

	// Texture Matrix
	{
		i3TextureCoordOffsetAttr * pAttr = i3TextureCoordOffsetAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_TEXTURE_COORD_OFFSET]->SetDefaultAttr( pAttr);
	}

	// Texture function
	{
		i3TextureFunctionAttr * pAttr = i3TextureFunctionAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_TEXTURE_FUNCTION]->SetDefaultAttr( pAttr);
	}

	// Render target
	{
		i3RenderTargetAttr * pAttr = i3RenderTargetAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_RENDERTARGET]->SetDefaultAttr( pAttr);
	}

	// Clear Render Target
	{
		// Clear Render Target은 Stack 형태를 적용하지 않고,
		// 단순히 발견되는 자체를 1회적으로 적용하는 선형 구조로 적용한다.
		m_pAttrStacks[ I3_ATTRID_CLEARRENDERTARGET]->SetTraceMode(true);
	}

	// ShadowMap Enable
	{
		i3ShadowMapEnableAttr * pAttr = i3ShadowMapEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_SHADOWMAP_ENABLE]->SetDefaultAttr( pAttr);
	}

	// NormalMap Enable
	{
		i3NormalMapEnableAttr * pAttr = i3NormalMapEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_NORMALMAP_ENABLE]->SetDefaultAttr( pAttr);
	}

	// SpecularMap Enable
	{
		i3SpecularMapEnableAttr * pAttr = i3SpecularMapEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_SPECULARMAP_ENABLE]->SetDefaultAttr( pAttr);
	}

	// LuxMap Enable
	{
		i3LuxMapEnableAttr * pAttr = i3LuxMapEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_LUXMAP_ENABLE]->SetDefaultAttr( pAttr);
	}

	// LuxMap bind
	{
		i3LuxMapBindAttr * pAttr = i3LuxMapBindAttr::new_object_ref();

		m_pAttrStacks[I3_ATTRID_LUXMAP_BIND]->SetDefaultAttr(pAttr);
	}


	// Reflect Map Enable Attr
	{
		i3ReflectMapEnableAttr * pAttr = i3ReflectMapEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_REFLECTMAP_ENABLE]->SetDefaultAttr( pAttr);
	}

	// Reflect Mask Map EnableAttr
	{
		i3ReflectMaskMapEnableAttr * pAttr = i3ReflectMaskMapEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_REFLECTMASKMAP_ENABLE]->SetDefaultAttr( pAttr);
	}

	{
		i3EmissiveMapEnableAttr* pAttr = i3EmissiveMapEnableAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_EMISSIVEMAP_ENABLE ]->SetDefaultAttr(pAttr);
	}

	{
		i3EmissiveMapBindAttr* pAttr = i3EmissiveMapBindAttr::new_object_ref();
		m_pAttrStacks[ I3_ATTRID_EMISSIVEMAP_BIND]->SetDefaultAttr(pAttr);
	}

	{
		i3SpecularMapBindAttr* pAttr = i3SpecularMapBindAttr::new_object_ref();
		m_pAttrStacks[ I3_ATTRID_SPECULARMAP_BIND ]->SetDefaultAttr(pAttr);
	}

	{
		i3NormalMapBindAttr* pAttr = i3NormalMapBindAttr::new_object_ref();
		m_pAttrStacks[ I3_ATTRID_NORMALMAP_BIND ]->SetDefaultAttr(pAttr);
	}

	{
		i3ReflectMaskMapBindAttr* pAttr = i3ReflectMaskMapBindAttr::new_object_ref();
		m_pAttrStacks[ I3_ATTRID_REFLECTMASKMAP_BIND ]->SetDefaultAttr(pAttr);
	}

	/// Fresnel
	{
		i3FresnelAttr * pAttr = i3FresnelAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_FRESNEL]->SetDefaultAttr( pAttr);
	}

#if defined (I3G_DX)
	// Shader
	{
		i3ShaderAttr * pAttr = i3ShaderAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_SHADER]->SetDefaultAttr( pAttr);
	}
#endif

	// Viewport
	{
		i3ViewportAttr * pAttr = i3ViewportAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_VIEWPORT]->SetDefaultAttr( pAttr);
	}

	{
		i3XrayColorAttr * pAttr = i3XrayColorAttr::new_object_ref();

		m_pAttrStacks[ I3_ATTRID_XRAYCOLOR]->SetDefaultAttr( pAttr);
	}

	{
		i3MipmapBiasAttr * pAttr = i3MipmapBiasAttr::new_object_ref();

		m_pAttrStacks[I3_ATTRID_MIPMAPBIAS]->SetDefaultAttr(pAttr);
	}

	{
		i3ProjectOffsetAttr * pAttr = i3ProjectOffsetAttr::new_object_ref();
		m_pAttrStacks[I3_ATTRID_PROJECT_OFFSET]->SetDefaultAttr(pAttr);
	}

	Reset();

}

i3AttrStackManager::~i3AttrStackManager(void)
{
	INT32 i;
	i3AttrStack * pStack;

	for( i = 0; i < I3_ATTR_MAX; i++)
	{
		pStack = m_pAttrStacks[i];

		I3_MUST_RELEASE( pStack);
	}
}

//
// 매 Frame 마다 호출되며, i3AttrStackManager를 원래의 상태로 돌리는 기능을 한다.
//
void i3AttrStackManager::Reset(void)
{
	INT32 i;
	i3AttrStack * pStack;
	register i3RenderAttr * pDefAttr; //, * pLastAttr;

	m_bEnable = true;

	resetModifiedList();
	m_bActivateAlphaSort = true;
	resetModifiedList();
	m_bActivateAlphaSort = false;


	for( i = 0; i < I3_ATTR_MAX; i++)
	{
		pStack = m_pAttrStacks[i];
//		pLastAttr = m_pLastAttrTrack[i];

		if( pStack != nullptr)
		{
			// 모든 Attribute Stack이 초기화 된 상태로 설정한다.
			pStack->Reset();

			// 만약 m_pLastAttrTrack의 값이 nullptr이 아니라면
			// 앞서의 Rendering에서 Default Render State와는 다른 State가 설정되어 있을 수 있기 때문에
			// Default Attribute를 설정해 둔다.
			pDefAttr = pStack->GetDefaultAttr();

			// 2004.1.16
			// earring
			//
			// Default Render-State를 매 frame 마다 초기에 재설정하는 것으로 i3RenderContext를
			// 수정함에 따라, Render State Tracer는 이전 Frame의 마지막 Attribute를 검사할 필요가
			// 없게 되었다.
			if( pDefAttr != nullptr)
			{
				pStack->Push( pDefAttr);

				if( m_bFlushed[i] == true)
				{
					addModifiedList( pStack);
				}
			}
		}


		m_apLastAttrTrack[0][i] = nullptr;
		m_apLastAttrTrack[1][i] = nullptr;
		m_bFlushed[i] = false;
	}
}

void i3AttrStackManager::addModifiedList( i3AttrStack * pStack)
{
	if( pStack->isLinked(m_bActivateAlphaSort) == false)
	{
		if( m_apTailAS[m_bActivateAlphaSort] != nullptr)
		{
			m_apTailAS[m_bActivateAlphaSort]->setNext( pStack, m_bActivateAlphaSort);
		}
		else
		{
			m_apHeadAS[m_bActivateAlphaSort] = pStack;
		}

		m_apTailAS[m_bActivateAlphaSort] = pStack;

		pStack->LinkState(m_bActivateAlphaSort);
	}
}

void i3AttrStackManager::resetModifiedList(void)
{
	i3AttrStack * pStack = m_apHeadAS[m_bActivateAlphaSort];

	while( pStack != nullptr)
	{
		i3AttrStack * pNext = pStack->getNext(m_bActivateAlphaSort);

		pStack->setNext( nullptr, m_bActivateAlphaSort);
		pStack->UnlinkState(m_bActivateAlphaSort);

		pStack = pNext;
	}

	m_apTailAS[m_bActivateAlphaSort] = m_apHeadAS[m_bActivateAlphaSort] = nullptr;
}

//
// 개별 Render Attribute를 Attr Stack에 추가해 둔다.
// 
// Attr. Stack에 Push 한다고 해서 반드시 그 Attribute가 적용되는 것은 아니다.
// 실제로 아래의 경우에는 Push된 Attribute라고 하더라도 무시될 수 있다.
//
// 1. 동일한 종류의 Attribute가 이 후에 다시 Push되는 경우.
// 2. Push는 되었으나, i3GeometryAttr이 존재하지 않아 버려지는 경우.
//
void i3AttrStackManager::PushAttr( i3RenderAttr * pAttr, INT32 Priority)
{
	INT32 id = pAttr->GetID();
	i3AttrStack * pStack = m_pAttrStacks[ id];

	if( isEnabled() == false)
		return;

	I3ASSERT( pStack != nullptr);

	// m_pAttrTrack은 Stack과 Scene Graph 사이에서 Cache 역할을 수행하는 Buffer이다.
	//
	if( pStack->IsEnabled() == false)
		return;

	// 현재 발견된 Attribute를 최종 Attribute로 등록 해둔다.
	if( pStack->GetCurrentPriority() >= Priority)
	{
		pStack->Push( pAttr, Priority);

		// 해당 종류의 Attribute Stack이 변경되었음을 기록해둔다.
		addModifiedList( pStack);
	}
}

//
// 근본적으로 동작하는 방법은 PushAttr() 함수와 동일하다.
// i3AttrSet과 같이 여러 Attribute를 한꺼번에 Push 하는 경우를 위한 함수이다.
//
void i3AttrStackManager::PushAttrs( i3::vector<i3RenderAttr*>& List, INT32 Priority)
{
	register INT32 id;
	register i3RenderAttr ** ppAttr = (i3RenderAttr **) &*List.begin();
	register i3AttrStack * pStack;

	if( isEnabled() == false)
		return;

	for(size_t i = 0; i < List.size(); i++)
	{
		id = ppAttr[i]->GetID();
		I3ASSERT( id >= 0 );

		pStack = m_pAttrStacks[ id];

		I3ASSERT( pStack != nullptr);

		if( (pStack->IsEnabled()) && ( pStack->GetCurrentPriority() >= Priority))
		{
			pStack->Push( ppAttr[i], Priority);

			addModifiedList( pStack);
		}
	}
}

//
// Push 했던 Attribute를 Stack에서 제거한다.
//
void i3AttrStackManager::PopAttr( UINT32 id, INT32 Priority)
{
	register i3AttrStack * pStack = m_pAttrStacks[ id];

	if( isEnabled() == false)
		return;

	I3ASSERT( ! pStack->IsEmpty());
	if( pStack->IsEnabled() == false)
		return;

	if( pStack->GetCurrentPriority() >= Priority)
	{
		// Stack에 Push 되었던 Attribute가 있다면...
		pStack->Pop();

		//pStack->SetPushState( (pStack->GetTop() != nullptr));
		if( pStack->GetTop() != nullptr)
		{
			addModifiedList( pStack);
		}

		// 실제로 Pop 한 Attribute가 있는 경우에만 반영되어야 할 Attribute가 있다는 의미이므로..
		// 수정 Flag을 설정 해둔다.
	}
}

void i3AttrStackManager::PopAttrs( i3::vector<i3RenderAttr*>& List, INT32 Priority)
{
	INT32 id;
	i3RenderAttr ** ppAttr = (i3RenderAttr **) &*List.begin();
	i3RenderAttr * pAttr;
	i3AttrStack * pStack;

	if( isEnabled() == false)
		return;

	for(size_t i = 0; i < List.size(); i++)
	{
		pAttr = ppAttr[i];

		id = pAttr->GetID();

		pStack = m_pAttrStacks[id];

		if( (pStack->IsEnabled()) && (pStack->GetCurrentPriority() >= Priority))
		{
			pStack->Pop();

			//pStack->SetPushState( (pStack->GetTop() != nullptr));
			if( pStack->GetTop() != nullptr)
			{
				addModifiedList( pStack);
			}
		}
	}
}

//
// 몇몇 Render attribute를 제외한 대부분의 Attribute는 i3GeometryAttr이 등장했을 때,
// 비로소 Display List에 추가된다.
// 이것은 실제로 Render State의 변경에 적용되는 Attribute의 개수를 최소화시키기 위한 조치이다.
//
// FlushPushedAttr() 함수는 호출된 시점까지 AttrStack에 등록되었던, 모든 Valid한 Attribute들을
// Display List에 추가하는 함수이다.
//
void i3AttrStackManager::FlushPushedAttrs( i3RenderAttrListAttr * pOutput)
{
	i3RenderAttr * pAttr;
	i3RenderAttr * pLastAttr;
	i3AttrStack * pStack;

	if( isEnabled() == false)
		return;

	pStack = m_apHeadAS[m_bActivateAlphaSort];

	while( pStack != nullptr)
	{
		pAttr = pStack->GetTop();

		if( pAttr != nullptr)
		{
			// *ppLastAttr : 최종적으로 실제로 Render Context에 적용되었던 Attribute
			//				 이 값이 nullptr이라 함은 Render Context에 실제로 적용된 해당 Attribute가 없었다는 의미.
			// *ppAttr     : Scene Graph에서 최종적으로 발견된 Attribute
			//
			pLastAttr = m_apLastAttrTrack[m_bActivateAlphaSort][ pAttr->GetID()];

			if( (pLastAttr == nullptr) || (pLastAttr->IsSame( pAttr) == false))
			{
				// 최종적으로 설정된 Attribute로 설정해 둔다.
				m_apLastAttrTrack[m_bActivateAlphaSort][ pAttr->GetID()] = pAttr;
				
				SetFlushState( pAttr->GetID());

				// 현재 nullptr이 아니라는 것은, 새로운 Attribute로 등록되었으나, 한번도 사용된 적이
				// 없었다는 것이기 때문에, 현재의 Rendering에 반영되어야 한다.

				#if defined( NO_DISPLAYLIST_RENDER)
					pAttr->Apply( g_pRenderContext);
				#else
					pOutput->AppendAttr( pAttr);
				#endif
			}
		}

		pStack = pStack->getNext(m_bActivateAlphaSort);
	}

	resetModifiedList();
}


//
// 알파소팅용은 씬그래프에서 분리되어 단독으로 개별렌더링에 쓰게 되므로 웬만한 상태값은 다 쥐고 있어야한다..
// 다만, 알파노드의 자식노드들에 대해서는 마찬가지로 변경 Attribute 최소화 과정은 동일하게 거치도록 해야함..
// (알파자식노드들에 대해서는 기존 FlushPushedAttrs를 적용하면 됨...)
// 이 함수의 호출시점은 알파 루트노드에서 FlushPushedAttrs() 직전에 호출되어야 한다..


INT32	i3AttrStackManager::GatherCurrentAttrsForAlphaSort( i3RenderAttrListAttr * pOutput)
{
	if( isEnabled() == false)
		return 0;

	m_bActivateAlphaSort = true;

	for( INT32 i = 0; i < I3_ATTR_MAX; ++i )
	{
		if (m_bIgnoreAlphaSort[i] == true)
			 continue;
	
		i3AttrStack* pStack = m_pAttrStacks[i];

		if( pStack != nullptr && pStack->IsEnabled() == true)
		{
			i3RenderAttr* pDefAttr = pStack->GetDefaultAttr();
			i3RenderAttr* pCurrAttr =	pStack->GetTop();
				
			if (pDefAttr != pCurrAttr)		// 디폴트가 아니라면 Append 필요하다..
			{
				m_apLastAttrTrack[1][ i ] = pCurrAttr;
				SetFlushState( i );
				pOutput->AppendAttr(pCurrAttr);
			}
		}
	}
	
	return pOutput->GetCount();
}

void	i3AttrStackManager::ResetCurrentAttrsForAlphaSort( i3RenderAttrListAttr* pIn, INT32 endDefaultIndex)
{
	if (isEnabled() == false)
		return;
	
	i3AttrStackManager::FlushPushedAttrs(pIn);

	const i3::vector<i3RenderAttr*>&	outputList = pIn->GetAttrList(); 
	
	const INT32 endIndex = ( endDefaultIndex > pIn->GetCount() ) ? pIn->GetCount() : endDefaultIndex;

	for (INT32 i = 0 ; i < endIndex ; ++i)
	{
		i3RenderAttr* pAttr = outputList[i];
	
		m_apLastAttrTrack[1][ pAttr->GetID() ] = nullptr;
	}

	m_bActivateAlphaSort = false;
}

void i3AttrStackManager::EnableAttrs( INT32 * pIDTable, INT32 count, bool bFlag)
{
	INT32 i;
	i3AttrStack * pStack;

	if( pIDTable == nullptr)
	{
		for( i = 0; i < I3_ATTR_MAX; i++)
		{
			pStack = m_pAttrStacks[i];

			if( pStack != nullptr)
				pStack->SetEnable( bFlag);
		}
	}
	else
	{
		for( i = 0; i < count; i++)
		{
			pStack = GetAttrStackByID( pIDTable[i]);
			
			if( pStack != nullptr)
			{
				pStack->SetEnable( bFlag);
			}
		}
	}
}
