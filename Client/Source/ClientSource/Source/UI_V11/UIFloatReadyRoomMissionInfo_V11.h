#if !defined( __UI_FLOAT_MINIMISSONINFO_H__)
#define __UI_FLOAT_MINIMISSIONINFO_H__

#include "../UI/UISubBase.h"

class UIFloatReadyRoomMissionInfo_V11 : public UISubBase
{
	I3_CLASS_DEFINE( UIFloatReadyRoomMissionInfo_V11, UISubBase);

private:
	i3UIButtonImageSet * m_pCardImg;

protected:
	void			_UpdateMissionInfo( void);
	void			_SetCardImage();

public:
	UIFloatReadyRoomMissionInfo_V11();
	virtual ~UIFloatReadyRoomMissionInfo_V11();

	virtual void	BindScene( i3UIScene * pScene) override;

	virtual void	UnBindScene( void) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
};

#endif
