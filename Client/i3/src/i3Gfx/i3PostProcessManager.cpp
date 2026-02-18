#include "i3GfxType.h"
#include "i3GfxGlobalVariable.h"
#include "i3PostProcessManager.h"

#include "i3PostProcessBloom.h"

I3_CLASS_INSTANCE( i3PostProcessManager);

i3PostProcessManager::i3PostProcessManager()
{
	m_PostProcessList.reserve(8);
	m_LastPostProcessList.reserve(3);
	m_RenderTargetList.reserve(8 + 3);
}

i3PostProcessManager::~i3PostProcessManager()
{
	_ReleaseLists();

	I3_SAFE_RELEASE(m_pVA);
	I3_SAFE_RELEASE(m_pShader);
}

void i3PostProcessManager::_ReleaseLists()
{

	size_t maxSize = 0;

	maxSize = m_PostProcessList.size();
	for (size_t i = 0; i < maxSize; i++)
	{
		i3PostProcess * pPP = m_PostProcessList[i];
		I3_SAFE_RELEASE(pPP);
	}
	m_PostProcessList.clear();
	
	maxSize = m_LastPostProcessList.size();
	for (size_t i = 0; i < maxSize; ++i)
	{
		i3PostProcess * pPP = m_LastPostProcessList[i];
		I3_SAFE_RELEASE(pPP);
	}
	m_LastPostProcessList.clear();

	maxSize = m_RenderTargetList.size();
	for (size_t i = 0; i < maxSize; i++)
	{
		i3RenderTarget * pRT = m_RenderTargetList[i];
		I3_SAFE_RELEASE(pRT);
	}
	m_RenderTargetList.clear();
}

bool i3PostProcessManager::Destroy(bool bLostDevice)
{
	I3_SAFE_RELEASE(m_pVA);

	for(size_t i = 0; i < m_RenderTargetList.size(); i++)
	{
		i3RenderTarget * pRT = m_RenderTargetList[i];
		I3_SAFE_RELEASE(pRT);
	}
	m_RenderTargetList.clear();

	return i3GfxResource::Destroy(bLostDevice);
}

bool i3PostProcessManager::isDestroyed()
{
	return (( m_pVA == nullptr ) && (m_RenderTargetList.size() == 0));
}

bool i3PostProcessManager::Revive(i3RenderContext * pCtx)
{
	m_pCtx = pCtx;
	_CreateQuadVertexArray();

	m_RenderTargetList.reserve(8 + 3);

	return i3GfxResource::Revive(pCtx);
}

void i3PostProcessManager::Create(i3RenderContext * pCtx)
{
	m_pCtx = pCtx;
	_CreateQuadVertexArray();

	i3Shader* pNewShader = i3ShaderCache::LoadShader("i3GaussianBlur_9.hlsl", I3G_SHADER_VERSION_1, I3G_SHADER_VERSION_2);
	I3_REF_CHANGE(m_pShader, pNewShader);
}

void i3PostProcessManager::_CreateQuadVertexArray()
{
	VEC3D _pos[4];
	VEC2D _tex[4];
	i3VertexFormat format;	

	if( m_pVA == nullptr)
	{
		m_pVA = i3VertexArray::new_object();
		I3ASSERT( m_pVA != nullptr);

		format.SetHasPosition( true);
		format.SetTextureCoordSetCount( 1);
		format.SetTextureCoordCount( 2);	

		m_pVA->Create( &format, 4, 0);
	}	

	{
		//Directly Mapping Texels to Pixels (Direct3D 9) 참조

		INT32 width = m_pCtx->getWindowWidth();
		INT32 height = m_pCtx->getWindowHeight();

		//	Position
		i3Vector::Set( &_pos[0], -1.0f - 1.f / width, 1.0f - 1.f / height, 0.0f);
		i3Vector::Set( &_pos[1], -1.0f - 1.f / width, -1.0f - 1.f / height, 0.0f);
		i3Vector::Set( &_pos[2], 1.0f - 1.f / width, 1.0f - 1.f / height, 0.0f);
		i3Vector::Set( &_pos[3], 1.0f - 1.f / width, -1.0f - 1.f / height, 0.0f);

		i3Vector::Set( &_tex[0], 0.0f, 0.0f);
		i3Vector::Set( &_tex[1], 0.0f, 1.0f);
		i3Vector::Set( &_tex[2], 1.0f, 0.0f);			
		i3Vector::Set( &_tex[3], 1.0f, 1.0f);
	}

	m_pVA->Lock( 0);
	m_pVA->SetPositions( 0, 4, _pos);
	m_pVA->SetTextureCoords( 0, 0, 4, _tex);
	m_pVA->Unlock();
}

i3PostProcess * i3PostProcessManager::_CheckEnablePostProcess()
{
	//현재 저장되어 있는 Post Process들 중, '마지막'이면서 결과물이 나와야 하는 것을 찾기 위해 쓴다.
	//이렇게 체크하지 않으면, 마지막의 Process된 결과가 적용되지 않는다.

	i3PostProcess * pLastProcess = nullptr;
	size_t sizeMax = m_PostProcessList.size();

	for (size_t i = 0; i < sizeMax; ++i)
	{
		i3PostProcess * pTemp = m_PostProcessList[i];
		pTemp->SetLast(false);

		if (pTemp->IsResultOut() && pTemp->IsEnable())
		{
			pLastProcess = pTemp;
		}
	}
	
	
	sizeMax = m_LastPostProcessList.size();
	for (size_t i = 0; i < sizeMax; ++i)
	{
		i3PostProcess * pTemp = m_LastPostProcessList[i];
		pTemp->SetLast(false);

		if (pTemp->IsResultOut() && pTemp->IsEnable())
		{
			pLastProcess = pTemp;
		}
	}
	
	
	if (pLastProcess != nullptr)
		pLastProcess->SetLast(true);

	return pLastProcess;
}

i3RenderTarget * i3PostProcessManager::RegisterRenderTarget(UINT32 width, UINT32 height, I3G_IMAGE_FORMAT format, char * ppName, bool isVolatile)
{
	//조건에 맞는 Render Target을 찾거나 만들어서 생성해준다.
	i3RenderTarget * pRT = nullptr;

	if(width <= 0)
	{
		width =  m_pCtx->getWindowWidth();
	}

	if(height <= 0)
	{
		height = m_pCtx->getWindowHeight();
	}

	if(format == I3G_IMAGE_FORMAT_NONE)
	{
		format = m_pCtx->getWindowColorFormat();
	}

	if(ppName != nullptr)
	{
		pRT = FindRenderTargetByName(ppName);

		if(pRT != nullptr && pRT->GetWidth() == width && pRT->GetHeight() == height && pRT->GetFormat() == format)
		{
			return pRT;
		}
	}
	
	pRT = FindRenderTarget(width, height, format);

	if(pRT != nullptr)
	{
		if(ppName != nullptr)
		{
			pRT->SetName(ppName);
		}
		else
		{
			pRT->SetName("");
		}

		pRT->SetVolatile(isVolatile);
		pRT->SetAllocated(true);
		return pRT;
	}
	else
	{
		pRT = AddRenderTarget(width, height, format, ppName, isVolatile);
		I3ASSERT(pRT != nullptr);
	}

	return pRT;
}

i3RenderTarget * i3PostProcessManager::RegisterRenderTarget(REAL32 ratio, I3G_IMAGE_FORMAT format, char * ppName, bool isVolatile)
{
	i3RenderTarget * pRT = nullptr;
	REAL32 width, height;
	
	width = m_pCtx->getWindowWidth() * ratio;
	height = m_pCtx->getWindowHeight() * ratio;

	pRT = RegisterRenderTarget((UINT32)width, (UINT32)height, format, ppName, isVolatile);

	I3ASSERT(pRT != nullptr);
	return pRT;
}

void i3PostProcessManager::DeAllocRenderTarget(bool bAllTarget)
{
	//모든 Render Target들의 할당 해제
	//Post Process Apply가 끝나면 불러준다.
	for(size_t i = 0; i < m_RenderTargetList.size(); i++)
	{
		i3RenderTarget * pRT = m_RenderTargetList[i];
		if(bAllTarget)
		{
			if(pRT->IsVolatile())
			{
				pRT->SetAllocated(false);
			}
		}
		else
		{
			if(pRT->GetName() == nullptr || pRT->IsVolatile())
			{
				pRT->SetAllocated(false);
			}
		}
	}
}

i3RenderTarget * i3PostProcessManager::FindRenderTarget(UINT32 width, UINT32 height, I3G_IMAGE_FORMAT format)
{
	for(size_t i = 0; i < m_RenderTargetList.size(); i++)
	{
		i3RenderTarget * pRT = m_RenderTargetList[i];
		if(!pRT->IsAllocated())
		{
			if(pRT->GetWidth() == width && pRT->GetHeight() == height && pRT->GetFormat() == format)
			{
				//동일한 RT라고 간주해도 될 법하다.
				return pRT;
			}
		}
	}

	return nullptr;
}

i3RenderTarget * i3PostProcessManager::FindRenderTargetByName(char * ppName)
{
	for(size_t i = 0; i < m_RenderTargetList.size(); i++)
	{
		i3RenderTarget * pRT = m_RenderTargetList[i];

		if(i3::generic_is_equal(pRT->GetName(), ppName))
		{
			return pRT;	
		}
	}

	return nullptr;
}

i3RenderTarget * i3PostProcessManager::AddRenderTarget(UINT32 width, UINT32 height, I3G_IMAGE_FORMAT format, char * ppName, bool isVolatile)
{
	//직접 이 함수를 호출하지 말고, RegisterRenterTarget()을 사용하는 것이 좋다.

	i3RenderTarget * pRT;
	bool bIsCreated;

	pRT = i3RenderTarget::new_object();

	bIsCreated = pRT->Create(width, height, format, ppName, isVolatile);

	if(bIsCreated)
	{
		pRT->SetAllocated(true);
		m_RenderTargetList.push_back(pRT);

		return pRT;
	}
	else
	{
		return nullptr;
	}
}

void i3PostProcessManager::DrawFullScreenQuad(i3RenderTarget * pRT, i3RenderContext * pCtx, i3VertexArray * pVA)
{
	I3ASSERT(m_pCtx != nullptr);
	I3ASSERT(GetQuadVertexArray() != nullptr);
	I3ASSERT(pRT != nullptr);

	SetRenderTarget(pRT);

	if(pVA != nullptr)
	{
		pCtx->SetVertexArray(pVA);
	}
	else
	{
		pCtx->SetVertexArray(GetQuadVertexArray());
	}
	
	pCtx->DrawPrim(nullptr, I3G_PRIM_TRISTRIP, 0, 2);
}


void i3PostProcessManager::AddPostProcess(i3PostProcess * pProcess)
{
	pProcess->Create(this);

	m_PostProcessList.push_back(pProcess);
	I3_SAFE_ADDREF(pProcess);
}

void i3PostProcessManager::AddLastPostProcess(i3PostProcess * pProcess)
{
	pProcess->Create(this);

	m_LastPostProcessList.push_back(pProcess);
	I3_SAFE_ADDREF(pProcess);
}

void i3PostProcessManager::RemovePostProcess(i3PostProcess * pProcess)
{
	INT32 index = i3::vu::int_index_of(m_PostProcessList, pProcess);

	if(index != -1)
	{
		i3::vu::erase_index(m_PostProcessList, index);
		I3_SAFE_RELEASE(pProcess);
	}
}

void i3PostProcessManager::RemoveAllPostProcess()
{
	for(size_t i = 0; i < m_PostProcessList.size(); i++)
	{
		i3PostProcess * pPP = m_PostProcessList[i];
		I3_SAFE_RELEASE(pPP);
	}

	m_PostProcessList.clear();
}

i3PostProcess * i3PostProcessManager::FindPostProcess(i3ClassMeta * pMeta)
{
	for(size_t i = 0; i < m_PostProcessList.size(); i++)
	{
		i3PostProcess * pp = m_PostProcessList[i];

		if(pp->same_of(pMeta))
		{
			return pp;
		}
	}

	return nullptr;
}

void i3PostProcessManager::SetRenderTarget(i3RenderTarget * pTarget)
{
	m_pCtx->SetCurrentRenderTarget(pTarget);
}


i3RenderTarget * i3PostProcessManager::ConfigRenderTarget()
{
	i3RenderTarget * pRT;

	i3PostProcess * pPP = _CheckEnablePostProcess();

	if( pPP != nullptr)
	{
		pRT = RegisterRenderTarget(1.0f, I3G_IMAGE_FORMAT_NONE, "ConfigRenderTarget", FALSE);

		m_pOldDefaultRT = m_pCtx->GetDefaultRenderTarget();
	}
	else
	{
		pRT = m_pCtx->GetDefaultRenderTarget();

		m_pOldDefaultRT = nullptr;
	}

	m_pCtx->SetDefaultRenderTarget( pRT);

	SetRenderTarget( pRT);

	m_pOriginRT = pRT;

	return pRT;
}


void i3PostProcessManager::Apply(REAL32 deltaTime)
{
	m_fDeltaTime = deltaTime;
	//지금까지 모인 Post Process들을 한꺼번에 전부 적용한다.
	COLOR oldClearColor;
	i3Color::Copy(&oldClearColor, m_pCtx->GetClearColor());
	
	//오리지널 Render Target -> 현재 그려진 Render Target
	m_pLastRT = m_pOriginRT;

	// 원래의 DefaultRT로 복구해준다.
	if( m_pOldDefaultRT != nullptr)
		m_pCtx->SetDefaultRenderTarget( m_pOldDefaultRT);

	for(size_t i = 0; i < m_PostProcessList.size(); i++)
	{
		i3PostProcess * pProcess = m_PostProcessList[i];

		i3RenderTarget * pNewRT;

		if(pProcess->IsEnable())
		{
			if(pProcess->IsResultOut())
			{
				if(pProcess->IsLast())
				{
					pNewRT = m_pCtx->GetDefaultRenderTarget();
				}
				else
				{
					pNewRT = RegisterRenderTarget(1.0f, I3G_IMAGE_FORMAT_NONE);
				}
			}
			else
			{
				//결과물을 내지 않는다면 그대로 놔둔다.
				pNewRT = m_pLastRT;
			}

			m_pCtx->SetZTestEnable(false);
			m_pCtx->SetZWriteEnable(false);
			m_pCtx->SetBlendEnable(false);
			m_pCtx->SetAlphaTestEnable(false);
			m_pCtx->SetLightingEnable(false);

			pProcess->ApplyPostProcess(this, m_pCtx, m_pLastRT, pNewRT);

			if( pNewRT != m_pLastRT)
			{
				m_pLastRT = pNewRT;
			}

		}

		//현재 Render Target의 리스트는 이름이 없거나, IsVolatile 세팅이 적용되어 있지 않다면 다른 Post Process들이 RegisterPostProcess 혹은
		//FindPostProcess로 찾아서 재활용이 가능하다.
		//따라서, 이전의 결과가 재활용이 되어야 한다면 IsVolatile = false로 설정한 후 
		//이름으로 검색해야 결과가 다음 프레임에도 온전히 남는다.

		DeAllocRenderTarget(false);
	}

	DeAllocRenderTarget(true);

	m_pCtx->SetShader(nullptr);
	m_pCtx->SetClearColor(&oldClearColor);
}

void i3PostProcessManager::ApplyLast()
{
	//지금까지 모인 Post Process들을 한꺼번에 전부 적용한다.
	COLOR oldClearColor;
	i3Color::Copy(&oldClearColor, m_pCtx->GetClearColor());

	size_t maxSize = m_LastPostProcessList.size();

	for (size_t i = 0; i < maxSize; ++i)
	{
		i3PostProcess* pProcess = m_LastPostProcessList[i];
		i3RenderTarget* pNewRT = nullptr;

		if (pProcess->IsEnable())
		{
			if (pProcess->IsResultOut())
			{				
				if (pProcess->IsLast())
				{
					pNewRT = m_pCtx->GetDefaultRenderTarget();
				}
				else
				{
					pNewRT = RegisterRenderTarget(1.0f, I3G_IMAGE_FORMAT_NONE);
				}			
			}
			else
			{
				//결과물을 내지 않는다면 그대로 놔둔다.
				pNewRT = m_pLastRT;
			}

			m_pCtx->SetZTestEnable(false);
			m_pCtx->SetZWriteEnable(false);
			m_pCtx->SetBlendEnable(false);
			m_pCtx->SetAlphaTestEnable(false);
			m_pCtx->SetLightingEnable(false);

			pProcess->ApplyPostProcess(this, m_pCtx, m_pLastRT, pNewRT);

			if (pNewRT != m_pLastRT)
			{
				m_pLastRT = pNewRT;
			}

		}

		DeAllocRenderTarget(false);
	}

	DeAllocRenderTarget(true);

	m_pCtx->SetShader(nullptr);
	m_pCtx->SetClearColor(&oldClearColor);
	
}

void i3PostProcessManager::ApplyGaussianBlur(i3RenderTarget * pDest, INT32 iterationCount, REAL32 theta)
{
	m_pCtx->SetShader(m_pShader);

	i3RenderTarget * temp = RegisterRenderTarget( pDest->GetWidth(), pDest->GetHeight(), pDest->GetFormat());

	REAL32 renderWidth, renderHeight;

	renderWidth = (REAL32)temp->GetWidth();
	renderHeight = (REAL32)temp->GetHeight();

	for(size_t i = 0; i < SAMPLE_COUNT; i++)
	{
		m_fBlurOffsetWeight[i] = VEC4D(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	for( INT32 i = 0; i < iterationCount; i++)
	{
		{
			//x축
			_SetBlurParameter(0.0f, 1.0f / renderHeight, 9, theta);

			m_Ctx.setTexture(pDest->GetColorTexture());
			m_pShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInput", &m_Ctx);

			DrawFullScreenQuad(temp, m_pCtx);
		}
		
		{
			//y축
			_SetBlurParameter(1.0f / renderWidth, 0.0f, 9, theta);

			m_Ctx.setTexture(temp->GetColorTexture());
			m_pShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInput", &m_Ctx);

			DrawFullScreenQuad(pDest, m_pCtx);
		}
	}
}

void i3PostProcessManager::_SetBlurParameter(REAL32 fx, REAL32 fy, UINT8 sampleCount, REAL32 theta)
{
	REAL32 totalWeight = _ComputeGaussian(0, theta);

	m_fBlurOffsetWeight[0] = VEC4D( 0.0f, 0.0f, totalWeight, 0.0f);
	
	INT32 i;

	for(i = 0; i < sampleCount / 2; i++)
	{
		REAL32 weight = _ComputeGaussian((REAL32)(i + 1), theta);

		totalWeight += weight * 2;

		REAL32 sampleOffset = i * 2 + 1.5f;
		VEC2D delta;

		delta.x = fx * (sampleOffset);
		delta.y = fy * (sampleOffset);

		// Weight
		m_fBlurOffsetWeight[ i*2 + 1].z = weight;
		m_fBlurOffsetWeight[ i*2 + 2].z = weight;

		// Offset
		m_fBlurOffsetWeight[ i*2 + 1].x = delta.x;
		m_fBlurOffsetWeight[ i*2 + 1].y = delta.y;

		m_fBlurOffsetWeight[ i*2 + 2].x = -delta.x;
		m_fBlurOffsetWeight[ i*2 + 2].y = -delta.y;
	}

	for(i = 0; i < sampleCount; i++)
	{
		m_fBlurOffsetWeight[i].z /= totalWeight;
	}

	//설정되어 있는 Shader Parameter들을 적용시켜 줘야 한다.
	m_pShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_OffsetWeight", m_fBlurOffsetWeight, false);
}	


REAL32 i3PostProcessManager::_ComputeGaussian(REAL32 n, REAL32 theta)
{
	return (REAL32)((1.0 / i3Math::Sqrt(2 * I3_PI * theta)) * exp(-(n * n) / (2 * theta * theta)));
}

void i3PostProcessManager::LoadPostProcessFromXML(i3XMLFile * pXML)
{
	RemoveAllPostProcess();

	INT32 i;
	//XML에 저장해 둔 Post Process들 및 설정을 긁어와서 저장한다.
	for(i = 0; i < pXML->GetObjectCount(); i++)
	{
		i3PostProcess * pPP = (i3PostProcess *)pXML->GetObject(i);
		if(pPP != nullptr)
		{
			AddPostProcess(pPP);
		}
	}
}

void i3PostProcessManager::SavePostProcessToXML(i3XMLFile * pXML)
{
	//현재 적용되어 있는 Post Process들 및 그 설정을 XML 파일에 저장한다.
	for(size_t i = 0; i < m_PostProcessList.size(); i++)
	{
		i3PostProcess * pPP = m_PostProcessList[i];
		pXML->AddObject(pPP);
	}
}