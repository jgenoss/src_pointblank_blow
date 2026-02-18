#pragma once

#include <unordered_map>

class UIHudManager;

class FrameList
{
public:
	void add(i3UIScene*, const char*);
	void add(i3UIFrameWnd*);

	void show();
	void show(size_t idx);
	
	void hide();
	void hide(size_t idx);

	void clear();

	const i3UIFrameWnd* getFrameWnd(size_t idx) const;
	i3UIFrameWnd* getFrameWnd(size_t idx);

private: 
	i3::vector< i3UIFrameWnd* > m_list;
};

class iHudBase
{
	friend class UIHudManager;

public:
	iHudBase(UIHudManager* parent);
	virtual ~iHudBase() {}

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) = 0;
	virtual void ClearAtUnload( i3UIScene * pScene) {}

	const i3::string & get_name() const { return m_name; }

	//현재 캐릭터 포인터
	void SetMyChara(CGameCharaBase* pMyChara) { s_pMyChara = pMyChara; }
	CGameCharaBase * GetMyChara(void)			{ return s_pMyChara; }

	//배틀 상태에 따라서 자동 호출 되는 함수로 호출 하기 힘들때 사용하면 된다.
	virtual void SetParameter(INT32 arg1, INT32 arg2 = -1) {};

	//내부 작동 및 화면 활성화 유무 - 내부 작동(update, event, process_state 등..)을 말한다.
	//SetEnable(false) 이면 화면 비활성화 및 내부 작동 비활성화 된다. 
	virtual void SetEnable(bool enable);
	virtual bool IsEnable() const { return m_enable; }

	//화면에 보여주는 여부 - 내부 작동(update, event, process_state 등..) 은 한다.
	void SetVisible(bool visible);
	void SetVisibleFrame(bool visible, UINT32 idx);
	virtual bool IsVisible() const { return m_visivle; }

	//하위 구현된 hud에 메인 frame wnd 추가 함수
	//추가 될 시 hide 된다.
	void AddFrameWnd(i3UIScene*, const char*);
	void AddFrameWnd(i3UIFrameWnd*);
	i3UIFrameWnd* GetFrameWnd(size_t idx = 0) { return m_FrameList.getFrameWnd(idx); }
	const i3UIFrameWnd* GetFrameWnd(size_t idx = 0) const { return m_FrameList.getFrameWnd(idx); }
	void ClearFrameWnd() { m_FrameList.clear(); }

public:
	virtual void ProcessGameEvent( GAME_EVENT evt, INT32 Arg, const i3::user_data& UserData) {}
	virtual void Update( REAL32 rDeltaSeconds) {}
	virtual void OnRevive( i3RenderContext * pCtx) {}

public:
	void OnEvent(GAME_EVENT type, INT32, const i3::user_data& UserData);

protected:
	FrameList m_FrameList;		//하위 hud main frame wnd
	i3::string m_name;	//lua 랑 관련있는 이름이다.

	UIHudManager* m_parent;
	bool m_enable;	//내부 작동 활성화 유무
	bool m_visivle;

	static CGameCharaBase* s_pMyChara;

protected:
	virtual void event_start_battle(INT32 arg, const i3::user_data& UserData) {}
	virtual void event_death_battle(INT32 arg, const i3::user_data& UserData) {}
	//이 함수는 event_respawn_battle 이 이벤트에 처음 호출되는 이벤트이다. 즉, 게임 시작시에 한번만 호출된다.
	virtual void event_first_respawn_battle(INT32 arg, const i3::user_data& UserData) {} 
	virtual void event_respawn_battle(INT32 arg, const i3::user_data& UserData) {}
	virtual void event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData) {}
	virtual void event_m_roundstart_battle(INT32 arg, const i3::user_data& UserData) {}
	virtual void event_default(INT32 arg, const i3::user_data& UserData) {}

private:
	typedef void (iHudBase::*evt_fn)(INT32 arg, const i3::user_data& UserData);
	typedef i3::unordered_map< GAME_EVENT, evt_fn > container;
	static container m_evt_fn_map;

public:
	static void init_fn();
};


template <class T>
inline T FindControl(i3UIScene * pScene, const char* name)
{
	i3GameNode* node = pScene->FindChildByName( name );
	if( !node)
	{
		I3PRINTLOG(I3LOG_FATAL, "[%s] i3Uie 파일 확인", name);
	}
	return static_cast<T>(node);
}
