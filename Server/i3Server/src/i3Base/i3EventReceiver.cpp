#include "i3CommonType.h"
#include "i3EventReceiver.h"
#include "i3MetaTemplate.h"
#include "i3Macro.h"

I3_ABSTRACT_CLASS_INSTANCE( i3EventReceiver, i3NamedElement);

BOOL	i3EventReceiver::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	return TRUE;
}

void i3EventReceiver::OnInput(void)
{
}

#if defined( I3_WINDOWS)

void	i3EventReceiver::OnMapCoord( POINT * pPoint)
{
}

void	i3EventReceiver::OnLButtonDown( UINT32 nFlag, POINT point)
{
}

void	i3EventReceiver::OnLButtonUp( UINT32 nFlag, POINT point)
{
}

void	i3EventReceiver::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
}

void	i3EventReceiver::OnMButtonDown( UINT32 nFlag, POINT point)
{
}

void	i3EventReceiver::OnMButtonUp( UINT32 nFlag, POINT point)
{
}

void	i3EventReceiver::OnRButtonDown( UINT32 nFlag, POINT point)
{
}

void	i3EventReceiver::OnRButtonUp( UINT32 nFlag, POINT point)
{
}

void	i3EventReceiver::OnMouseMove( UINT32 nFlag, POINT point)
{
}

void	i3EventReceiver::OnMouseWheel( UINT nFlag, short zDelta, POINT point)
{
}

void	i3EventReceiver::OnKeyDown( UINT32 nKey)
{
}

void	i3EventReceiver::OnKeyUp( UINT32 nKey)
{
}

#endif