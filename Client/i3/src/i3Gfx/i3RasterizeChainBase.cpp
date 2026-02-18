#include "i3GfxType.h"
#include "i3GfxGlobalVariable.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3RasterizeChainBase.h"
#include "i3RasterizeChainManager.h"

I3_CLASS_INSTANCE( i3RasterizeChainBase);

i3RasterizeChainBase::i3RasterizeChainBase(void)
{
	// 초기생성시 효과는 자동 비활성화 되어있음.
	SetEnable(false);
}

void i3RasterizeChainBase::SetEnable(bool bFlag)
{
	bool bChanged = m_bEnable != bFlag;

	m_bEnable = bFlag;

	i3RasterizeChainManager * pManager = i3RasterizeChainManager::getInstance();

	if((bChanged) && ( pManager != nullptr))
	{
		pManager->OnEnableChain( this);
	}
}

bool i3RasterizeChainBase::BeginChain( i3RenderContext * pCtx)
{
	getManager()->SetRenderTarget( getTarget());

	return true;
}

void i3RasterizeChainBase::EndChain(i3RenderContext * pCtx)
{
}


i3RasterizeChainManager * i3RasterizeChainBase::getManager(void)
{
	return i3RasterizeChainManager::getInstance();
}

void i3RasterizeChainBase::OnCreate( i3RasterizeChainManager * pManager)
{
}

void i3RasterizeChainBase::Draw( i3VertexArray * pVA)
{
	if( pVA == nullptr)
	{
		i3RasterizeChainManager * pManager = getManager();

		pVA = pManager->getDefaultVertexArray();
	}

	g_pRenderContext->SetVertexArray( pVA);
	g_pRenderContext->DrawPrim( nullptr, I3G_PRIM_TRISTRIP, 0, 2);
}


void i3RasterizeChainBase::OnSize( i3RenderContext * pCtx)
{
}
