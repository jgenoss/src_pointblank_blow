#if !defined( __UI_SMenu_Base_H__)
#define __UI_SMenu_Base_H__

#include <unordered_map>

class UISideMenu;

class iSMenu
{
public:
	iSMenu(UISideMenu*);
	virtual ~iSMenu();

public:
	virtual void OnCreate( i3GameNode * pParent) = 0;

	virtual void OnLoadAllScenes() = 0;
	virtual void OnUnloadAllScenes() = 0;

	virtual void OnUpdate( REAL32 rDeltaSeconds) {}
	virtual void OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)	{}
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) {}

	virtual void MoveBackScene() {}

	virtual bool IsStatusEnable(void) const { return true; }

	bool		IsOpenedSubMenu( void)		{ return m_bSubMenuOpened; }

protected:
	UISideMenu* mParent;

public:
	UISideMenu* GetParent()	{	return mParent	;	}

	//frame Open, data update
	virtual void Entrance(bool arg1 = false) {}
	//frame close, data update
	virtual void Exit(void* arg1= nullptr, void* arg2 = nullptr) {}

	virtual void OnRevive( i3RenderContext * pCtx) {}

protected:
	UI_CHARA_INFO  m_TempCharaInfo;		// 설정 전의 이전 상태 저장, 설정을 취소하면 이전 상태로 돌리기 위함.

	bool		m_bSubMenuOpened;
};

#endif
