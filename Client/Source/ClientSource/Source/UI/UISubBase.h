/**
 * \file	UI\UISubBase.h
 *
 * Declares the user interface sub base class.
 */


#if !defined( __UI_SUB_BASE_H__)
#define __UI_SUB_BASE_H__

#include "UIBase.h"
#include "UIDefine.h"

//-------------------------------------------------------------------------------//
//	UIBase의 Scene을 참조하여 처리하는 Class
//	직접 i3UIScene을 갖고 있지 않으며 Parent의 UIBase에서 참조한다.
//-------------------------------------------------------------------------------//

class UIMainFrame;
class UIFramework;

class UISubBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UISubBase, UIBase);

protected:
	LuaState *		_CallLuaFunction( const char * pszFunction);

	UIMainFrame *	GetMainFrame( void);

public:
	UISubBase();
	virtual ~UISubBase();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override {}

	/** \brief Phase 이동시 호출됩니다. */
	virtual void	OnMovePhase( UIPHASE phase) {}

	/** \brief Bind Scene
		\desc 특정 Control Pointer를 접근하거나 Lua Glue Function을 등록합니다. */
	virtual void	BindScene( i3UIScene * pScene) = 0;

	/** \brief Scene 해제시 호출되어 멤버를 초기화해준다. */
	virtual void	UnBindScene( void) = 0;
};

#endif
