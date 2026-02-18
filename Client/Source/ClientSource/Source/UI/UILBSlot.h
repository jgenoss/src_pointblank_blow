#if !defined( __UI_LB_SLOT_H__)
#define __UI_LB_SLOT_H__

//
//
//  우선, 상점용슬롯을 UIBase나 i3GameNode에서 모두 배제시키고 상위페이즈에 추가등록되는 슬롯씬으로부터 클래스만 분리해서 관리합니다...
//  콜백클래스를 두고, 슬롯처리를 원래의 작업클래스로 되돌리는 방식을 선택해보았는데...생각보다, 우회할만한 가치가 많지 않아보여서...다시 수정을 고려중...

class UIBase;

class UILBSlot_Callback
{
public:
	virtual void	OnLoadSlot( INT32 slot_idx, i3UIScene* slot_scn ) = 0;				// 로드된 경우, 초기화할 호출 위치..
	virtual void	OnUnloadSlot( INT32 slot_idx, i3UIScene* slot_scn) = 0;				// 언로드된 경우 정리할 호출 위치..
	virtual void	OnEnabledSlot( INT32 slot_idx, i3UIScene* slot_scn) = 0;			// enable호출된 경우 업데이트할 정보를 채울 함수..
	virtual void	OnDisabledSlot( INT32 slot_idx, i3UIScene* slot_scn) = 0;
	virtual void	OnEnterSlot( INT32 slot_idx, i3UIScene* slot_scn) = 0; 
	virtual void	OnLeaveSlot( INT32 slot_idx, i3UIScene* slot_scn) = 0;
	virtual bool	EnableMouseOver() = 0;
};

template<class T,
void (T::*ON_LOAD_SLOT)(INT32, i3UIScene*),
void (T::*ON_UNLOAD_SLOT)(INT32, i3UIScene*),
void (T::*ON_ENABLED_SLOT)(INT32, i3UIScene*),
void (T::*ON_DISABLED_SLOT)(INT32, i3UIScene*),
void (T::*ON_ENTER_SLOT)(INT32, i3UIScene*),
void (T::*ON_LEAVE_SLOT)(INT32, i3UIScene*),
bool (T::*ENABLE_MOUSEOVER)()
>

class TUILBSlot_Callback : public UILBSlot_Callback
{
public:
	TUILBSlot_Callback() : m_obj(nullptr) {}
	void			SetObj(T* obj) { m_obj = obj; }
private:
	virtual void	OnLoadSlot( INT32 slot_idx, i3UIScene* slot_scn ) override {	(m_obj->*ON_LOAD_SLOT)(slot_idx, slot_scn); }
	virtual void	OnUnloadSlot( INT32 slot_idx, i3UIScene* slot_scn) override {  (m_obj->*ON_UNLOAD_SLOT)(slot_idx, slot_scn); }
	virtual void	OnEnabledSlot( INT32 slot_idx, i3UIScene* slot_scn) override {  (m_obj->*ON_ENABLED_SLOT)(slot_idx, slot_scn); }
	virtual void	OnDisabledSlot( INT32 slot_idx, i3UIScene* slot_scn) override { (m_obj->*ON_DISABLED_SLOT)(slot_idx, slot_scn);  }
	virtual void	OnEnterSlot( INT32 slot_idx, i3UIScene* slot_scn) override { (m_obj->*ON_ENTER_SLOT)(slot_idx, slot_scn);  }
	virtual void	OnLeaveSlot( INT32 slot_idx, i3UIScene* slot_scn) override {  (m_obj->*ON_LEAVE_SLOT)(slot_idx, slot_scn); }
	virtual bool	EnableMouseOver() override { return (m_obj->*ENABLE_MOUSEOVER)();  }
	T*				m_obj;
};


class UILBSlotSet
{
public:
	UILBSlotSet();
	virtual ~UILBSlotSet();
	void SetEnableMouseOver(bool bEnable);
	bool IsEnableMouseOver() const { return m_isEnableMouseOver && m_isEnableMouseOverFromCB; }
	void	 SetCallback(UILBSlot_Callback* callback);
	void OnCreate(UIBase* parent, INT32 row, INT32 col);
	void RegisterSlotScene( const char* szSlotScene, const LuaFuncReg* Lua_Func, bool bLoaded = false);		// 씬이름과 루아 함수 등록..

	void SetVisible(bool bEnable);
	void SetSkip(bool bSkip) { m_skip_slot = bSkip; }

	void	 OnLoadAllScenes( INT32 start_x,  INT32 start_y, UINT32 offset_x = 0,  UINT32 offset_y = 0);
	void	 OnLoadAllScenes(i3UIControl* attach_pos,  UINT32 offset_x = 0,  UINT32 offset_y = 0);

	void OnUnloadAllScenes();
	void	 OnUpdate();				// OnMouseMove에 대한 완전한 외부 콜백이 보이지 않아서, 여기서 직접 포지션을 감시합니다...

	void	UpdatePosition( INT32 start_x,  INT32 start_y, UINT32 offset_x = 0,  UINT32 offset_y = 0);
	void	 UpdateList(const INT32 size_list, const INT32 idx_scroll_start);

	void	 SetEnable(INT32 slot, bool bEnable);

	bool HitTestSet(INT32 x, INT32 y, INT32& outSlotIdx);
	bool HitTestSlot(INT32 slot, INT32 x, INT32 y);

	INT32 GetRowCount() const { return m_numRow; }
	INT32 GetColCount() const { return m_numCol; }
	INT32 GetSlotCount() const { return m_numSlot; }
	INT32 GetActiveSlotCount() const { return m_numActiveSlot; }
	INT32 GetMouseOverSlot() const { return m_idx_slot_mouseover; }

	i3UIScene* GetSlotScene(INT32 slot) const;				// 콜백으로 쓰는 Enable, Load, Unload를 제외하고는 씬을 직접 참조해서 처리하도록 합니다...

	const VEC2D&	GetSlotSize() const;
	const VEC3D&	GetSlotPos(INT32 slot) const;

	void MoveBackScene();		//모든 slot을 최상위로 변경한다.
	void ReverseScene();		//모든 slot을 생성 역순으로 변경한다.(slot 안에 combo box가 있을 경우에 사용하면 좋다.)

	void	ResetMouseoverSlot() { m_idx_slot_mouseover = -1; m_prevMousePos.x = m_prevMousePos.y = LONG_MAX; }

protected:
	void			_ProcessEnableMouseOver(bool bEnable);
	UIBase*				m_parent;
	UILBSlot_Callback*	m_callback;
	INT32			m_offset_scn_idx;					// Parent로부터 씬에 접근 가능하기 때문에 더 이상 배열을 만들지 않아도 된다..
	INT32			m_idx_slot_mouseover;
	INT32			m_numRow, m_numCol, m_numSlot, m_numActiveSlot;
	POINT			m_prevMousePos;
	bool			m_isEnableMouseOver;
	bool			m_isEnableMouseOverFromCB;
	bool m_skip_slot;
	POINT m_offsetPos;
};

#endif