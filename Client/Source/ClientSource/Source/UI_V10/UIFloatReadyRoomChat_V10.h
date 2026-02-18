
#include "UIFloatChat_V10.h"

class UIFloatReadyRoomChat_V10 : public UIFloatChat_V10
{
	I3_CLASS_DEFINE( UIFloatReadyRoomChat_V10, UIFloatChat_V10);

public:
	UIFloatReadyRoomChat_V10();
	virtual ~UIFloatReadyRoomChat_V10();

public: 
	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	InitChat( CHAT_AREA area, INT32 nMaxLineCount) override;
};