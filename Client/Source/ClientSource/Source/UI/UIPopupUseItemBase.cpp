#include "pch.h"
#include "UIPopupUseItemBase.h"

#include "ItemConsumer.h"

I3_CLASS_INSTANCE( UIPopupUseItemBase);


/*virtual*/ void UIPopupUseItemBase::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	ItemConsumer::instance()->process_game_event_in_query(event, arg, UserData);
}
