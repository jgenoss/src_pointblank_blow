#if !defined( __UI_TOOLTIP_MAPINFO_H__)
#define __UI_TOOLTIP_MAPINFO_H__

#include "UIToolTipBase.h"

class UIToolTipMapInfo : public UIToolTipBase
{
	I3_CLASS_DEFINE( UIToolTipMapInfo);

public:
	UIToolTipMapInfo();
	virtual ~UIToolTipMapInfo();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual void	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);

	void			SetRoomInfo( ROOM_INFO_BASIC * pInfo);
};

#endif
