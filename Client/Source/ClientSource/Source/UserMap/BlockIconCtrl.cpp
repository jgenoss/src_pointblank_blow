#include "pch.h"
#include "BlockIconCtrl.h"

I3_CLASS_INSTANCE( CBlockIconCtrl, i3GuiStatic);

CBlockIconCtrl::CBlockIconCtrl(void)
{
	i3Vector::Set( &m_UV, 0.0f, 0.0f);
}

CBlockIconCtrl::~CBlockIconCtrl(void)
{
}

void CBlockIconCtrl::SetUV( VEC2D * pVec)
{
	i3Vector::Copy( &m_UV, pVec);

	SetControlShapeUV( 0, (INT32)m_UV.x, (INT32)m_UV.y);
}

INT32 CBlockIconCtrl::OnGuiEvent( i3GUI::EVENT event, UINT32 param1, UINT32 param2)
{
	switch( event)
	{
		case i3GUI::EVT_ENTERMOUSE :
			SetControlShapeUV( 0, (INT32)m_UV.x, (INT32)m_UV.y + 40);
			break;

		case i3GUI::EVT_LEAVEMOUSE :
			SetControlShapeUV( 0, (INT32)m_UV.x, (INT32)m_UV.y);
			break;
	}

	return i3GuiStatic::OnGuiEvent( event, param1, param2);
}
