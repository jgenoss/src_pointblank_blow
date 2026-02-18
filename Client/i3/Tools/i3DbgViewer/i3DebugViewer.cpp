#include "stdafx.h"
#include "i3DebugViewer.h"
#include "GfxCmdCore.h"

I3_CLASS_INSTANCE( i3DebugViewer, i3Viewer);

i3DebugViewer::i3DebugViewer(void)
{
	m_pCmdBuffer = NULL;
}

i3DebugViewer::~i3DebugViewer(void)
{
}

void i3DebugViewer::Render(void)
{
	GfxCmdCore core;
	i3RenderContext * pCtx = GetRenderContext();

	if( IsState( I3_VIEWER_STATE_USER_TIME) || IsState( I3_VIEWER_STATE_PAUSED))
		m_DeltaTime = i3Math::Mul( m_UserTime, m_TimeScale);
	else
		m_DeltaTime = i3Math::Mul( __RT(m_pTimer->GetDeltaSec()), m_TimeScale);

	m_pFramework->OnUpdate( m_DeltaTime);

	if( pCtx->IsReady() == false)
		return;

	if( pCtx->Begin() == false)
		return;

	pCtx->Clear();

	{
		MATRIX viewMtx;
		i3Camera *		pCam = m_pFramework->GetDefaultCamera();

		i3Matrix::Inverse( &viewMtx, NULL, pCam->GetMatrix());

		pCtx->SetViewMatrix( &viewMtx);
		pCtx->SetProjectMatrix( pCam->GetProjectionMatrix());
		pCtx->SetWorldMatrix( &I3_IDENTITYMATRIX);
	}

	core._Draw( GetRenderContext(), m_pCmdBuffer);

	pCtx->End();

	core._DrawText( GetRenderContext(), GetWindowHandle(), m_pCmdBuffer);
}
