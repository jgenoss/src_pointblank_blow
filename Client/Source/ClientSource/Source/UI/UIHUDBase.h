#if !defined( __UI_HUD_BASE_H__)
#define __UI_HUD_BASE_H__

#include "UIBase.h"

class UIHUDBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIHUDBase, UIBase);

protected:
	/** \brief 로드시 초기화
		\desc HUD의 경우 미리 로딩을 하기 때문에 Texture는 삭제하지 말아야 한다.
			엔진에서 자동으로 삭제됨으로 이를 막아준다.*/
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;

	/** \brief 언로드시 초기화
		\desc Stage 해제시 Texture가 자동으로 삭제될 수 있도록 해준다.*/
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIHUDBase();
	virtual ~UIHUDBase();

	virtual void	ProcessGameEvent( GAME_EVENT Event, INT32 Arg, const i3::user_data& UserData) override {}

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	ClearData() {}

};

#endif
