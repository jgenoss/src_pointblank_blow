#include "i3GfxType.h"
#include "i3GfxOcclusionTest.h"
#include "i3GfxGlobalVariableDX.h"

I3_CLASS_INSTANCE( i3GfxOcclusionTest);

i3GfxOcclusionTest::~i3GfxOcclusionTest(void)
{
	I3_SAFE_RELEASE_NODBG( m_pQuery);
}

void i3GfxOcclusionTest::Create(void)
{
	if( m_pQuery == nullptr)
	{
		g_pD3DDevice->CreateQuery( D3DQUERYTYPE_OCCLUSION, &m_pQuery);
	}
}

void i3GfxOcclusionTest::Begin(void)
{
	if( m_pQuery != nullptr)
	{
		m_pQuery->Issue( D3DISSUE_BEGIN);
	}
}

void i3GfxOcclusionTest::End(void)
{
	if( m_pQuery != nullptr)
	{
		m_pQuery->Issue( D3DISSUE_END);
	}
}

bool i3GfxOcclusionTest::Update( bool bWait)
{
	if( m_pQuery == nullptr)
	{
		m_DrawnPixelCount = 0;
		return true;
	}

	if( bWait)
	{
		while( S_FALSE == m_pQuery->GetData( &m_DrawnPixelCount, sizeof(m_DrawnPixelCount), D3DGETDATA_FLUSH))
			;

		return true;
	}

	HRESULT rv = m_pQuery->GetData( &m_DrawnPixelCount, sizeof(m_DrawnPixelCount), D3DGETDATA_FLUSH);

	return rv != S_FALSE;
}

bool i3GfxOcclusionTest::Destroy( bool bLostDevice)
{
	I3_SAFE_RELEASE_NODBG( m_pQuery);

	return i3GfxResource::Destroy( bLostDevice);
}

bool i3GfxOcclusionTest::isDestroyed( void)
{
	return (m_pQuery == nullptr);
}

bool i3GfxOcclusionTest::Revive( i3RenderContext * pCtx)
{
	Create();

	return i3GfxResource::Revive( pCtx);
}
