#include "StdAfx.h"
#include "i3RenderContextUI.h"
#include "GlobalVariables.h"
/*
I3_CONCRETE_CLASS_INSTANCE( i3RenderContextUI, i3RenderContext);

i3RenderContextUI::i3RenderContextUI(void)
{
	m_pUIRenderTarget = NULL;
}

i3RenderContextUI::~i3RenderContextUI(void)
{
	I3_SAFE_RELEASE( m_pUIRenderTarget);
}

void i3RenderContextUI::End()
{
	while( m_pSwapChain->Present( NULL, NULL, NULL, NULL, D3DPRESENT_DONOTWAIT) == D3DERR_WASSTILLDRAWING)
	{
		INT32 i;

		for( i = 0; i < m_WaitProcList.GetCount(); i++)
		{
			WAIT_VSYNC_INFO * pInfo = (WAIT_VSYNC_INFO *) m_WaitProcList.Items[i];
			WAITVSYNCPROC pProc = pInfo->m_pProc;

			pProc(  this, pInfo->m_pUserData);
		}
	}

	{
		m_pPerfReport->SetDrawCallCountPerFrame( m_DrawCallCount);
		m_pPerfReport->SetTriangleCountPerFrame( m_TriangleCount);
		m_pPerfReport->End();

		m_FrameCountPerSec++;

		m_DeltaSec = m_pTimer->GetDeltaSec();

		m_secFrameTime += m_DeltaSec;

		if( m_secFrameTime >= 1.0f)
		{
			//1
			m_pPerfReport->SetFPS( m_FrameCountPerSec / m_secFrameTime);
			m_FrameCountPerSec = 0;
			m_secFrameTime = 0.0f;
		}

		m_DrawCallCount = 0;
		m_TriangleCount = 0;
	}

	if( m_bRivived) m_bRivived = FALSE;
}
*/