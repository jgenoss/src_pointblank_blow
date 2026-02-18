#if !defined( __UI_TAB_BASE_H__)
#define __UI_TAB_BASE_H__

#include "UIBase.h"

class UITabBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UITabBase, UIBase);

public:
	UITabBase();
	virtual ~UITabBase();

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override {  }		// 이 함수는 여기서부터 시작..UIBase에는 존재하지 않는다..

	UIMainFrame *	GetMainFrame( void);


};

#endif
