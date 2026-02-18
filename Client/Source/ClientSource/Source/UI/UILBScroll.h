#if !defined( __UI_LJ_SCROLL_H__)
#define __UI_LJ_SCROLL_H__

class UIBase;
class UILBScrollSlotSet;

class UILBScroll_Callback
{
public:
	virtual void	OnScroll(INT32 idx, INT32 old_idx) = 0;
};

template< class T, void (T::*ON_SCROLL)(INT32,INT32) >
class TUILBScroll_Callback : public UILBScroll_Callback
{
public:
	TUILBScroll_Callback() : m_obj(nullptr) {}
	void SetObj(T* obj) { m_obj = obj; }

private:
	virtual void OnScroll(INT32 idx, INT32 old_idx ) override {	(m_obj->*ON_SCROLL)(idx, old_idx); }
	T* m_obj;
};

/*	자동 스크롤바 입니다.
 **/
class UILBScroll
{
	friend class UILBScrollSlotSet;

public:
	UILBScroll();
	virtual ~UILBScroll();

	void SetCallback(UILBScroll_Callback* callback) { m_callback = callback; }

	void OnCreate(UIBase* parent);

	void RegisterScene( const char* szScene );		// 씬이름과 루아 함수 등록..

	void OnLoadAllScenes();
	void OnUnloadAllScenes();

	bool OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code );

	i3UIScene* GetScene() const;
	i3UIFrameWnd* GetFrameWnd() const {	return m_pFrameWnd; }
	i3UIScrollBar* GetScrollBar() const { return m_pControl; }

	void Hide(); //실제로는 SetEnable함수를 이용한다. 차이점은 IsEnable()을 true를 반환한다.
	void Show() { SetEnable(true); }

	void SetEnable(bool bEnable);
	bool IsEnable() const;

	void SetFocus();
	void KillFocus();
	bool IsFocused() const;
	
	void SetPosition(UINT32 x, UINT32 y);
	void SetSize(UINT32 w,UINT32 h);

	void SetScrollPos(UINT32 idx);
	void SetRange(UINT32 max, UINT32 min  = 0);
	void SetPage(UINT32 page);
	void SetTick(UINT32 tick);

	bool HitTest(INT32 x, INT32 y) const;

	void SetAlpha(REAL32 factor); //0.f ~ 1.f 값

private:
	bool m_once_disable_callback;
	void _SetScrollPos(UINT32 idx);

protected:
	INT32 m_scn_idx;
	INT32 m_curr_scroll_idx, m_old_scroll_idx;
	i3UIFrameWnd* m_pFrameWnd;
	i3UIScrollBar* m_pControl;
	UIBase*				m_parent;
	UILBScroll_Callback*	m_callback;
	bool is_hide_action;
};

#endif