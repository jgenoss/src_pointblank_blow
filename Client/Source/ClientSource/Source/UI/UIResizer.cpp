#include "pch.h"
#include "UIResizer.h"

I3_CLASS_INSTANCE( UIResizer);//, i3GameNode);

UIResizer::UIResizer()
{
	m_SizeStyle = UI_SIZESTYLE_MOVEBOTTOM;
	m_SizeState = UI_SIZESTATE_MAXIMIZE_DONE; 
	m_PreSizeState = UI_SIZESTATE_MAXIMIZE_DONE;
	m_SizeUpdateTime = 0.f;

	m_pFrameWnd = nullptr;

	m_MinimizeDoneProc	= nullptr;
	m_MaximizeDoneProc	= nullptr;
	m_TargetValue		= 0.f;
	m_originalX			= 0.f;
	m_originalY			= 0.f;
}

UIResizer::~UIResizer()
{
}

void UIResizer::_MoveToBottom( void)
{
	VEC3D vPos;
	// 위치를 내린다.
	i3Vector::Copy( &vPos, m_pFrameWnd->getPos());
	setY( &vPos, m_originalY + (m_TargetValue * m_SizeUpdateTime) );
	m_pFrameWnd->setPos( &vPos);
}

void UIResizer::_MoveFromBottom( void)
{
	// 위치를 내린다.
	REAL32 y = (m_originalY + m_TargetValue) - (m_TargetValue * m_SizeUpdateTime);
	m_pFrameWnd->setPos( m_originalX, y);
}

void UIResizer::_MinimizeToTop( void)
{
	// size를 줄인다.
	REAL32 range = m_originalY - m_TargetValue;
	REAL32 height = m_originalY - (range * m_SizeUpdateTime);

	m_pFrameWnd->setSize( m_originalX, height);
}

void UIResizer::_MaximizeFromTop( void)
{
	REAL32 range = m_originalY - m_TargetValue;
	REAL32 height = m_TargetValue + (range * m_SizeUpdateTime);
	
	m_pFrameWnd->setSize( m_originalX, height);
}

void UIResizer::BindFrameWnd( i3UIFrameWnd * pFrameWnd, REAL32 targetValue, 
							UIRESIZE_PROC minimizeDoneProc /*= nullptr*/, UIRESIZE_PROC maximizeDoneProc /*= nullptr*/,
							UI_SIZESTYLE style /*= UI_SIZESTYLE_MOVEBOTTOM*/)
{
	m_SizeStyle = style;
	m_pFrameWnd = pFrameWnd;
	m_TargetValue = targetValue;

	m_MinimizeDoneProc = minimizeDoneProc;
	m_MaximizeDoneProc = maximizeDoneProc;

	I3ASSERT( m_pFrameWnd != nullptr);
	if( style < UI_SIZESTYLE_FOLDBOTTOM)
	{	// 이동에 관한 size
		m_originalX = getX( m_pFrameWnd->getPos());
		m_originalY = getY( m_pFrameWnd->getPos());
	}
	else
	{	// 크기에 관한 size
		m_originalX = getX( m_pFrameWnd->getSize());
		m_originalY = getY( m_pFrameWnd->getSize());
	}
}

void UIResizer::ResetBindFrameWnd(VEC3D* pos, VEC2D* size, REAL32 targetValue,
	UIRESIZE_PROC minimizeDoneProc /*= nullptr*/, UIRESIZE_PROC maximizeDoneProc /*= nullptr*/,
	UI_SIZESTYLE style /*= UI_SIZESTYLE_MOVEBOTTOM*/)
{
	m_SizeStyle = style;
	m_TargetValue = targetValue;

	m_MinimizeDoneProc = minimizeDoneProc;
	m_MaximizeDoneProc = maximizeDoneProc;

	I3ASSERT(m_pFrameWnd != nullptr);
	if (style < UI_SIZESTYLE_FOLDBOTTOM)
	{	// 이동에 관한 size
		m_originalX = getX(pos);
		m_originalY = getY(pos);
	}
	else
	{	// 크기에 관한 size
		m_originalX = getX(size);
		m_originalY = getY(size);
	}
}


/*virtual*/ void UIResizer::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameNode::OnUpdate( rDeltaSeconds);

	switch( m_SizeState)
	{
	case UI_SIZESTATE_MINIMIZE :
		m_SizeUpdateTime += rDeltaSeconds * 4.f;
		if( m_SizeUpdateTime > 1.f)
		{
			m_SizeUpdateTime = 1.f;
			OnMinimizeEnd();
		}

		OnMinimize();
		break;
	case UI_SIZESTATE_MAXIMIZE :
		m_SizeUpdateTime += rDeltaSeconds * 4.f;
		if( m_SizeUpdateTime > 1.f)
		{
			m_SizeUpdateTime = 1.f;
			OnMaximizeEnd();
		}

		OnMaximize();
		break;
	}
}


void UIResizer::OnMinimizeStart( void)
{
	if( m_SizeState != UI_SIZESTATE_MINIMIZE_DONE)
	{
		m_SizeState = UI_SIZESTATE_MINIMIZE;
		m_SizeUpdateTime = 0.f;
	}
}

void UIResizer::OnMinimize( void)
{
	switch( m_SizeStyle)
	{
	case UI_SIZESTYLE_MOVEBOTTOM :	_MoveToBottom();	break;
	case UI_SIZESTYLE_FOLDTOP :		_MinimizeToTop();	break;
	}
}

void UIResizer::OnMinimizeEnd( void)
{
	m_SizeState = UI_SIZESTATE_MINIMIZE_DONE;
	if( m_MinimizeDoneProc)
		m_MinimizeDoneProc();
}

void UIResizer::OnMaximizeStart( void)
{
	if( m_SizeState != UI_SIZESTATE_MAXIMIZE_DONE)
	{
		m_SizeState = UI_SIZESTATE_MAXIMIZE;
		m_SizeUpdateTime = 0.f;
	}
}

void UIResizer::OnMaximize( void)
{
	switch( m_SizeStyle)
	{
	case UI_SIZESTYLE_MOVEBOTTOM :	_MoveFromBottom();	break;
	case UI_SIZESTYLE_FOLDTOP :		_MaximizeFromTop();	break;
	}
}

void UIResizer::OnMaximizeEnd( void)
{
	m_SizeState = UI_SIZESTATE_MAXIMIZE_DONE;

	if( m_MaximizeDoneProc)
		m_MaximizeDoneProc();
}
