#include "StdAfx.h"
#include "UIGripObjHandler.h"
#include "i3TDKUpdateSystem.h"
//I3_ABSTRACT_CLASS_INSTANCE( CUIGripObjHandler, i3ElementBase);
I3_CLASS_INSTANCE( CUIGripObjHandler);
I3_CLASS_INSTANCE( CUIGripObjHandler_Shape); //, CUIGripObjHandler);
I3_CLASS_INSTANCE( CUIGripObjHandler_RECT); //, CUIGripObjHandler);
I3_CLASS_INSTANCE( CUIGripObjHandler_UIControl); //, CUIGripObjHandler);

CUIGripObjHandler::CUIGripObjHandler(void)
{
}

CUIGripObjHandler::~CUIGripObjHandler(void)
{
}

//////////////////////////////////////////////////////////////////
CUIGripObjHandler_Shape::CUIGripObjHandler_Shape(void)
{
	m_pShape = NULL;
}

CUIGripObjHandler_Shape::~CUIGripObjHandler_Shape(void)
{
}

void CUIGripObjHandler_Shape::OnSelect(void)
{
	i3TDK::Update(NULL, I3_TDK_UPDATE_SELECT, m_pShape);
}
void CUIGripObjHandler_Shape::OnUnSelect(void)
{
	i3TDK::Update(NULL, I3_TDK_UPDATE_UNSELECT, m_pShape);
}

/////////////////////////////////////////////////////////////////
CUIGripObjHandler_RECT::CUIGripObjHandler_RECT(void)
{
	m_pRect = NULL;
}

CUIGripObjHandler_RECT::~CUIGripObjHandler_RECT(void)
{

}

void CUIGripObjHandler_RECT::setPositionX( REAL32 left)
{
	REAL32 width = m_pRect->right - m_pRect->left;

	m_pRect->left = left;
	m_pRect->right = left + width;
}

void CUIGripObjHandler_RECT::setPositionY( REAL32 top)
{
	REAL32 height = m_pRect->bottom - m_pRect->top;

	m_pRect->top = top;
	m_pRect->bottom = top + height;
}

////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
CUIGripObjHandler_UIControl::CUIGripObjHandler_UIControl(void)
{
	m_pControl = NULL;
}

CUIGripObjHandler_UIControl::~CUIGripObjHandler_UIControl(void)
{
}
