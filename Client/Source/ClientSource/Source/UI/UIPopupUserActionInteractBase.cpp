#include "pch.h"
#include "UIPopupUserActionInteractBase.h"

I3_ABSTRACT_CLASS_INSTANCE( UIPopupUserActionInteractBase, UIPopupBase);

UIPopupUserActionInteractBase::UIPopupUserActionInteractBase()
{
}

UIPopupUserActionInteractBase::~UIPopupUserActionInteractBase()
{
}

/*virtual*/ bool UIPopupUserActionInteractBase::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	i3String::Copy( m_szNick, (const char*) pArg1);

	return true;
}


