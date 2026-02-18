#if !defined( __UI_LJ_SCROLL_SLOT_H__)
#define __UI_LJ_SCROLL_SLOT_H__

#include "UILBSlot.h"
#include "UILBScroll.h"

class UISlide;

class UILBScrollSlot_Callback
{
public:
	virtual INT32 OnScroll(INT32 idx, INT32 old_idx) = 0;
};

template< class T, INT32 (T::*ON_SCROLL)(INT32,INT32) >
class TUILBScrollSlot_Callback : public UILBScrollSlot_Callback
{
public:
	TUILBScrollSlot_Callback() : m_obj(nullptr) {}
	void SetObj(T* obj) { m_obj = obj; }

private:
	virtual INT32 OnScroll(INT32 idx, INT32 old_idx ) override {	return (m_obj->*ON_SCROLL)(idx, old_idx); }
	T* m_obj;
};

namespace UI { class iSlot; }

/*	Slot + ScrollBar 결합된 클래스 입니다.
 **/
class UILBScrollSlotSet : public UILBSlotSet
{
	friend class UI::iSlot;
	
private:
	INT32 m_idxStartSlot;
	INT32 m_idxPosSlot;
	INT32 m_RefListSize;

	VEC2D m_SlotSize;

	UILBScroll m_Scroll;
	bool m_bUseScroll;

	UILBScrollSlot_Callback* m_SlotSetCallback;

private:
	void _notifyScroll( INT32 idx, INT32 old_idx );

	typedef TUILBScroll_Callback< UILBScrollSlotSet, &UILBScrollSlotSet::_notifyScroll > ScrollCallback;
	friend class ScrollCallback;
	ScrollCallback m_ScrollCallback;

public:
	UILBScrollSlotSet();
	~UILBScrollSlotSet();

	void SetSlotSetScrollCallback(UILBScrollSlot_Callback* callback) { m_SlotSetCallback = callback; }

	void EnableScroll(bool bEnable)	{	m_bUseScroll = bEnable;	}
	void SetSlotSkip(bool bSkip) { SetSkip(bSkip); } //마우스 위치에 따른 slot 동작을 멈춘다.

	void AddSlide(UISlide* pSlide, INT32 type);

	void	 RegisterSlotScene(const char* szSlotScene, const LuaFuncReg* Lua_Func, const char* szScrollScene);	
	void	 RegisterSlotScene(const char* szSlotScene, const LuaFuncReg* Lua_Func, bool bLoaded = false, bool bEquip = false);	

	void OnLoadAllScenes( INT32 start_x,  INT32 start_y, UINT32 offset_x = 0,  UINT32 offset_y = 0, UINT32 addsize = 0);
	void OnLoadAllScenes(i3UIControl* attach_pos,  UINT32 offset_x = 0,  UINT32 offset_y = 0);

	void	UpdatePosition( INT32 start_x,  INT32 start_y, UINT32 offset_x = 0,  UINT32 offset_y = 0);

	void OnUnloadAllScenes();

	bool OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code);

	const UILBScroll& GetScrollCtrl() const { return m_Scroll;	}
	UILBScroll& GetScrollCtrl() { return m_Scroll;	}

	void Adjust(INT32 list_size);
	void UpdateList(INT32 list_size, bool auto_focus_scroll = true);

	INT32 GetStartInx() const {	return m_idxStartSlot; }

	bool HitTest(INT32 x, INT32 y) const;

	void SetPosition( INT32 start_x, INT32 start_y, UINT32 offset_x = 0,  UINT32 offset_y = 0);
	void	 SetEnable(bool bEnable);

	void ResetScrollBar();
	void SetScrollFocus();

	//ui deep util
	void MoveBackScene(); //slot을 맨 뒤에 찍히도록 변경한다.
	void ReverseScene(); //slot이 상에서 하로 찍히는 걸 하에서 상으로 변경한다.
	

	VEC2D GetLeftTop() const;
	VEC2D GetRightBottom() const;
};


namespace UI
{
	class tmHover_Callback 
	{ 
	public: 
		virtual void	OnEnter(UINT32 idx, i3UIControl* pWnd) = 0;
		virtual void	OnLeave(UINT32 idx, i3UIControl* pWnd) = 0;
	};

	template<class T, void (T::*H_ENTER)(UINT32,i3UIControl*), void (T::*H_LEAVE)(UINT,i3UIControl*)>
	class tmiHover_Callback : public tmHover_Callback
	{
	public:
		tmiHover_Callback() : m_this(0) {}
		void		SetObj(T* obj) { m_this = obj; }

	private:
		virtual void	OnEnter(UINT32 idx, i3UIControl* pWnd) override { (m_this->*H_ENTER)(idx, pWnd);  }
		virtual void	OnLeave(UINT32 idx, i3UIControl* pWnd) override { (m_this->*H_LEAVE)(idx, pWnd); }

		T* m_this;
	};

	class tmHover
	{
		typedef i3::vector< i3UIControl* > ControlList;
		ControlList mControlList;

		tmHover_Callback* m_callback;
		size_t m_over_idx;
		POINT m_prevMousePos;

	public:
		tmHover();
		~tmHover();

		void	 SetCallback(tmHover_Callback* callback)	{ m_callback = callback; }

		void add(i3UIScene* s, const char* name);
		void add(i3UIControl* pWnd);
		void del(size_t idx);
		void del_front();
		void clear();

		void OnUpdate(REAL32 rDeltaTime);

		size_t get_stayed_hover_idx();
	};
}

#endif