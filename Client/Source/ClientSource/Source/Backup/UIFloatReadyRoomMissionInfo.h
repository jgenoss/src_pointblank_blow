#if !defined( __UI_FLOAT_MINIMISSONINFO_H__)
#define __UI_FLOAT_MINIMISSIONINFO_H__

#include "UISubBase.h"

class UIFloatReadyRoomMissionInfo : public UISubBase
{
	I3_CLASS_DEFINE( UIFloatReadyRoomMissionInfo, UISubBase);

private:
	i3UIButtonImageSet * m_pCardImg;

protected:
	void			_UpdateMissionInfo( void);
	void			_SetCardImage( CARDSET_ID id, INT32 idx);

public:
	UIFloatReadyRoomMissionInfo();
	virtual ~UIFloatReadyRoomMissionInfo();

	virtual void	BindScene( i3UIScene * pScene);

	virtual void	UnBindScene( void);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
};

#endif
