#include "stdafx.h"
#include "i3LevelControl.h"
#include "../i3TDKUpdateSystem.h"

I3_CLASS_INSTANCE( i3LevelControl);

i3LevelControl::i3LevelControl(void)
{
	AddStyle( I3_LEVEL_STYLE_VOLATILE | I3_LEVEL_STYLE_CONTROL);
}

void i3LevelControl::SetOwner( i3LevelElement3D * pElm)
{
	m_pOwner = pElm;
}

void	i3LevelControl::OnDetachScene( i3LevelScene * pScene)
{
	if((m_pOwner != nullptr) && (m_bNotify))
	{
		m_pOwner->OnEvent( I3LV_EVENT_DELETE, this, 0);
	}
}

void	i3LevelControl::OnMoving(void)
{
	if((m_pOwner != nullptr) && (m_bNotify))
	{
		m_pOwner->OnMovingIcon( this);
	}
}

void	i3LevelControl::OnScaling(void)
{
	if((m_pOwner != nullptr) && (m_bNotify))
	{
		m_pOwner->OnScalingIcon( this);
	}
}

void	i3LevelControl::OnRotating(void)
{
	if((m_pOwner != nullptr) && (m_bNotify))
	{
		m_pOwner->OnRotatingIcon( this);
	}
}

void	i3LevelControl::OnSelected( BOOL bSelected)
{
	i3LevelElement3D::OnSelected( bSelected);

	if( m_pOwner != nullptr)
	{
		m_pOwner->OnSelectedIcon( this);

		//if( bSelected)
		//	m_pOwner->Select();
		//else
		//	m_pOwner->Unselect();
	}

	if( m_pProxy != nullptr)
	{
		i3TDK::Update( nullptr, I3_TDK_UPDATE_SELECT, m_pProxy);
	}
}