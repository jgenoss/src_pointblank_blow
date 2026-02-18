#include "i3FrameworkPCH.h"
#include "i3ViewerMode.h"

I3_ABSTRACT_CLASS_INSTANCE( i3ViewerMode, i3ElementBase);

i3ViewerMode::i3ViewerMode(void)
{
	m_pViewer = NULL;
	m_bDebugControl = FALSE;
}

i3ViewerMode::~i3ViewerMode(void)
{
}

void i3ViewerMode::OnCreate(void)
{
}

void i3ViewerMode::OnAttach(void)
{
}

void i3ViewerMode::OnDetach(void)
{
}

void i3ViewerMode::OnChangeScene(void)
{
}

void i3ViewerMode::OnChangeBound(void)
{
}

void i3ViewerMode::OnChangeCamera(void)
{
}

void i3ViewerMode::OnResetCameraPosition(void)
{
}

void i3ViewerMode::OnUpdateCamera(void)
{

}

void i3ViewerMode::OnInput(void)
{
}

#if defined( I3_WINDOWS)
void i3ViewerMode::OnLButtonDown( UINT32 nFlag, POINT point)
{
}

void i3ViewerMode::OnLButtonUp( UINT32 nFlag, POINT point)
{
}

void i3ViewerMode::OnMButtonDown( UINT32 nFlag, POINT point)
{
}

void i3ViewerMode::OnMButtonUp( UINT32 nFlag, POINT point)
{
}

void i3ViewerMode::OnRButtonDown( UINT32 nFlag, POINT point)
{
}

void i3ViewerMode::OnRButtonUp( UINT32 nFlag, POINT point)
{
}

void i3ViewerMode::OnMouseMove( UINT32 nFlag, POINT point)
{
}

void i3ViewerMode::OnMouseWheel( UINT nFlag, short zDelta, POINT point)
{
}

void i3ViewerMode::OnKeyDown( UINT32 nKey)
{
}

void i3ViewerMode::OnKeyUp( UINT32 nKey)
{
}
#endif
