#if !defined( __UI_SLIDE_H__)
#define __UI_SLIDE_H__

//
//  기존에 프레임윈도우별 일괄 슬라이드 되는 것을 각 프레임윈도우 등록별 슬라이드 방식으로 변경합니다..
//

typedef void (*cb_SlideEvent)( void *, void *);
typedef void (*cb_EndEvent)();

class UISlide;

class UISlide_Callback 
{ 
public: 
	virtual void	OnSlideInOnce() = 0;								// 첫번째 업데이트만..
	virtual void	OnSlidingIn(REAL32 delta_time) = 0;					// 모든 슬라이딩 업데이트에서
	virtual void	OnSlideInDone() = 0;

	virtual void	OnSlideOutOnce() = 0;								// 첫번째 업데이트만..
	virtual void	OnSlidingOut(REAL32 delta_time) = 0;				// 모든 슬라이딩 업데이트에서
	virtual void	OnSlideOutDone() = 0;
};

template<class T,	void (T::*S_IN_ONCE)(), void (T::*S_IN_ING)(REAL32), void (T::*S_IN_DONE)(), 
					void (T::*S_OUT_ONCE)(), void (T::*S_OUT_ING)(REAL32), void (T::*S_OUT_DONE)()>
class TUISlide_Callback : public UISlide_Callback
{
public:

	TUISlide_Callback() : m_this(0) {}
	TUISlide_Callback(T* obj) : m_this(obj) {}
	void		SetObj(T* obj) { m_this = obj; }

private:
	virtual void	OnSlideInOnce() { (m_this->*S_IN_ONCE)();  }
	virtual void	OnSlidingIn(REAL32 delta_time) { (m_this->*S_IN_ING)(delta_time); }
	virtual void	OnSlideInDone() { (m_this->*S_IN_DONE)(); }

	virtual void	OnSlideOutOnce() { (m_this->*S_OUT_ONCE)(); }	
	virtual void	OnSlidingOut(REAL32 delta_time) { (m_this->*S_OUT_ING)(delta_time); }		
	virtual void	OnSlideOutDone() { (m_this->*S_OUT_DONE)(); }

	T*		m_this;
};

//
// 아래 추상클래스는 슬라이드에 등록하면 가상함수가 동작... 주어진 ratio(슬라이드인일때 0~1, 아웃일때 1~0으로 가며, 
// 0이면 화면 외부 지점, 1일때 타겟지점이 리소스포지션인것으로 가정하고 작업합니다...)
//

struct UISlideMtd 
{
	virtual void operator()( i3UIControl* ctrl, REAL32 ratio) = 0;
	virtual void self_destroy() {}
	virtual void setOriginalPos( REAL32 x, REAL32 y) {}
	virtual void setOriginalSize( REAL32 width, REAL32 height) {}
};

namespace detail { class UISlideProcess; class UISlideProcess_Sliding;	class UISlideProcess_Delay; 
					class UISlideProcess_WaitLoad;	class UISlideProcess_Done;  }


class UISlide 
{
public:
	enum slider_type { L_TO_R, R_TO_L, T_TO_B, B_TO_T, POPUP, slider_max };

	UISlide();
	~UISlide();
	void			AddFrameWnd(i3UIControl* pWnd, slider_type t = R_TO_L);		// 이걸 쓰면 미리 준비된 방식으로 슬라이드..
	void			AddFrameWnd(i3UIControl* pWnd, UISlideMtd* mtd);			// 이걸 쓰면, 주어진 ratio를 바탕으로 외부에서 조작 가능할 것이라 여겨집니다..
	void			AddFrameWnds( i3UIScene* pScn, slider_type t = R_TO_L);
	
	void			Update(REAL32 rDeltaTime);

	void			ResetOriginalPos();
	void			ClearFrameWndList();
	void			RemoveFrameWnd(i3UIControl* pWnd);
	void			RemoveFrameWnds(i3UIScene* pScn);

	void			Start(bool Is_In);
	void			ForceEnd();
	void			ReadyToStart(bool Is_In);			// 특별한 일이 아니면 쓰지 않습니다.. 초기화만 시키고 시작 안함..
	void			Restore();

	void			AddSyncSlide(UISlide* slide);	

	void			ResetTime();
	void			SetTargetTime(bool slide_in, REAL32 target_time) { m_target_time[slide_in] = (target_time > 0.f) ? target_time : 0.f; }
	void			SetInitDelayTime(bool slide_in, REAL32 delay_time) { m_init_delay_time[slide_in] = ( delay_time > 0.f) ? delay_time : 0.f; }
	void			SetCallback(UISlide_Callback* callback) { m_callback = callback; }
	void			ResetCallback();
	float			GetCurrRatio() const { return m_curr_ratio; }

	static UISlide_Callback*	GetCallback_Null();
	
	bool			IsSlideDelayWaitLoad() const;

	/** \brief 원본 위치를 조절한다. */
	void			SetOriginalPos( REAL32 x, REAL32 y);

	/** \brief 원본 크기를 조절한다 */
	void			SetOriginalSize( REAL32 width, REAL32 height);

private:

	void			_Init_Pos();
	
	UISlide_Callback*				m_callback;
	REAL32								m_target_time[2];
	REAL32								m_init_delay_time[2];

	i3::vector<i3UIControl*>			m_ctrl_list;
	i3::vector<UISlideMtd*>				m_mtd_list;
	
	UISlide*							m_parent_slide;
	i3::vector<UISlide*>				m_sync_slide_list;			// 로딩시 리소스기다리는 시간을 동기화할 목적으로..

	REAL32								m_curr_time;
	REAL32								m_curr_ratio;
	class detail::UISlideProcess*		m_slider;
	bool								m_is_slide_in;
	bool								m_is_delay_waitload;
	bool								m_waitload_all_done;

	friend class	detail::UISlideProcess_Sliding;
	friend class	detail::UISlideProcess_Delay;
	friend class	detail::UISlideProcess_WaitLoad;
	friend class	detail::UISlideProcess_Done;

};


namespace UI {

//slide 될 frame wnd에 속성을 나타낸다.
struct tmSlideAtt
{
	i3UIFrameWnd* frame;
	i3UI3DViewBox * _3dBox;
	INT32 origin_x, origin_y;
	INT32 target_x, target_y;
	REAL32 origin_size_x, origin_size_y;
	REAL32 target_size_x, target_size_y;
	
	cb_SlideEvent	cbSlideEvent;
	cb_EndEvent		cbEndEvent;


	tmSlideAtt(i3UIFrameWnd* frm) : frame(frm), _3dBox(nullptr), cbSlideEvent(nullptr), cbEndEvent(nullptr)
	{
		origin_x = (INT32)frm->getPos()->x;	origin_y = (INT32)frm->getPos()->y;
		origin_size_x = frm->getSize()->x;	origin_size_y = frm->getSize()->y;
	}

	tmSlideAtt(i3UIFrameWnd* frm, void * pBox, cb_SlideEvent evt1 = nullptr, cb_EndEvent evt2 = nullptr) : frame(frm)
	{
		_3dBox = static_cast<i3UI3DViewBox*>(pBox);
		origin_x = (INT32)frm->getPos()->x;	origin_y = (INT32)frm->getPos()->y;
		origin_size_x = frm->getSize()->x;	origin_size_y = frm->getSize()->y;
		setCBSlideEvent(evt1);
		setCBEndEvent(evt2);
	}

	tmSlideAtt(i3UIFrameWnd* frm, i3UI3DViewBox * pBox) : frame(frm), _3dBox(pBox)
	{
		origin_x = (INT32)frm->getPos()->x;	origin_y = (INT32)frm->getPos()->y;
		origin_size_x = frm->getSize()->x;	origin_size_y = frm->getSize()->y;
		cbSlideEvent= nullptr;
		cbEndEvent	= nullptr;
	}

	tmSlideAtt(i3UIFrameWnd* frm, INT32 targ_x, INT32 targ_y) : frame(frm), target_x(targ_x), target_y(targ_y), cbSlideEvent(nullptr), cbEndEvent(nullptr)
	{	origin_x = (INT32)frm->getPos()->x;	origin_y = (INT32)frm->getPos()->y;	}

	tmSlideAtt(i3UIFrameWnd* frm, INT32 ori_x, INT32 ori_y, INT32 targ_x, INT32 targ_y) : frame(frm), origin_x(ori_x), origin_y(ori_y), target_x(targ_x), target_y(targ_y), cbSlideEvent(nullptr), cbEndEvent(nullptr)
	{}

	void set_target(INT32 x, INT32 y)	{	target_x = x;		target_y = y;	}
	void set_target_by_offset(INT32 offset_x, INT32 offset_y)	{	target_x = origin_x + offset_x;		target_y = origin_y + offset_y;	}
	void set_target_by_offset_and_size(INT32 offset_x, INT32 offset_y, REAL32 size_x, REAL32 size_y)
	{
		target_x = origin_x + offset_x;		target_y = origin_y + offset_y;
		target_size_x = size_x;				target_size_y = size_y;
	}

	void			setCBSlideEvent(cb_SlideEvent cb)	{	cbSlideEvent= cb;	}
	void			setCBEndEvent(cb_EndEvent cb)		{	cbEndEvent	= cb;	}
	cb_SlideEvent *	getCBSlideEvent()					{	return &cbSlideEvent;}
	cb_EndEvent*	getCBEndEvent()						{	return &cbEndEvent;	}
};

//slide 방식
struct tmSlideMtd 
{
	virtual void operator()( tmSlideAtt* slide_att, REAL32 ratio) = 0;
};
//직선 slide 방식
struct tmLinearSlideMtd : public tmSlideMtd
{
	bool invert;
	virtual void operator()( tmSlideAtt* slide_att, REAL32 ratio);
};

//Size 조절 팝업 방식
struct tmPopupSlideMtd : public tmSlideMtd
{
	bool invert;
	virtual void operator()(tmSlideAtt* slide_att, REAL32 ratio);
};

//slide manager에 일괄 등록 container
struct tmSlideAttContainer
{
	typedef i3::vector< tmSlideAtt* > SlideAttList;
	SlideAttList mList;	
	i3UIScene* scene;

	tmSlideAttContainer(i3UIScene* s) : scene(s) {}

	void add(const char* frm_name);
	void add(const char* frm_name, void * pBox, cb_SlideEvent, cb_EndEvent);

	void set_origin(INT32 x, INT32 y, bool move = true);
	void set_origin_by_offset(INT32 offset_x, INT32 offset_y, bool move = true);

	void set_target(INT32 x, INT32 y);
	void set_target_by_offset(INT32 offset_x, INT32 offset_y);

	void set_target_by_offset_and_size(INT32 offset_x, INT32 offset_y, REAL32 targetsize_x, REAL32 targetsize_y);
};

/*	Slide 완료후에 호출된다.
*/
struct SCallBack
{ 
	virtual void Call() = 0;
	virtual void operator = ( const SCallBack& CF) { this->pObj = CF.pObj; }
	void* pObj;
	void set_obj(  void* obj) { this->pObj = obj;}
};

template <class T, void (T::*CallBack)()>
struct TCallBack : SCallBack
{		
//	typedef void (T::*Func)();
	void Call()
	{ 
		T* pThis = static_cast<T*>(pObj);
		(pThis->*CallBack)();
	}
};

/*	tmSlideAtt를 관리하는 클래스
 *		start
 **/
class tmSlide
{
	typedef i3::vector< tmSlideAtt* > SlideAttList;
	SlideAttList mSlideAttList;

	REAL32 m_accumulate_time;
	REAL32 m_play_time;
	tmSlideMtd * m_slideMtd;

	SCallBack* m_pCallFunc;

public:
	tmSlide();
	~tmSlide();

	void add(const tmSlideAttContainer & list);
	tmSlideAtt* add(tmSlideAtt* att);
	void clear();

	// mtd : slide 방법(tmSlideMtd를 상속 받아서 정의된 구조체)
	// play_time : slide 시간
	void start(tmSlideMtd * mtd, REAL32 play_time);  
	void OnUpdate(REAL32 rDeltaTime);

	void move_origin(); // 등록된 tmSlideAtt을 원점으로 이동한다.
	void move_target(); // 등록된 tmSlideAtt을 목표점으로 이동한다.
	void move_origin_and_size();
	void move_target_and_size();

	void update_origin_pos(INT32 x, INT32 y, bool move = true); // 등록된 tmSlideAtt을 원점을 수정한다.
	void update_origin_pos_by_offset(INT32 offset_x, INT32 offset_y, bool move = true); // 등록된 tmSlideAtt을 원점을 원점 위치에 offset 만큼으로 수정한다.

	void update_target_pos(INT32 x, INT32 y, bool move = true); // 등록된 tmSlideAtt을 목표점을 수정한다.
	void update_target_pos_by_offset(INT32 offset_x, INT32 offset_y, bool move = true); // 등록된 tmSlideAtt을 목표점을 목표점 위치에 offset 만큼으로 수정한다.

	void SetSlideEndCallBack( SCallBack* pCallFunc) { m_pCallFunc = pCallFunc; }
	void ResetSlideEndCallBack();
};


/*	tmSlideAtt를 관리하는 클래스
 *		start
 **/
struct multi_slide_CallBack
{ 
	virtual void finish_multi_slide(size_t idx) = 0;
	void operator = ( const multi_slide_CallBack& CF) { this->pObj = CF.pObj; }
	void* pObj;
};

template <class T, void (T::*CallBack)(size_t idx)>
struct T_multi_slide_CallBack : multi_slide_CallBack
{		
	typedef void (T::*Func)(size_t idx);
	void finish_multi_slide(size_t idx)
	{ 
		T* pThis = static_cast<T*>(pObj);
		(pThis->*CallBack)(idx);
	}
};

struct multi_slide_att
{
	REAL32 play_time;
	tmSlideMtd* play_mtd;
};

class multi_tmSlide
{
public:
	multi_tmSlide();
	~multi_tmSlide();

public:
	tmSlide& get_slide() { return m_slide; } //tmSlide 에 대한 operator은 넣지 않는다. 이 함수로 받아서 처리하자.
	void add_slide_att(tmSlideMtd* play_mtd, REAL32 play_time);

	void OnUpdate(REAL32 rDeltaTime);
	
	void start();

	void force_begin();
	void force_end();
	
	void SetSlideEndCallBack( multi_slide_CallBack* pCallFunc) { m_pCallFunc = pCallFunc; }

private:
	void apply_slide_att();

private:
	void slide_finish();
	TCallBack<multi_tmSlide, &multi_tmSlide::slide_finish> m_slide_callback;

private:
	tmSlide m_slide;
	size_t slide_att_index;
	i3::vector< multi_slide_att > multi_slide_att_list;
	multi_slide_CallBack* m_pCallFunc;
};

}	//end of namespace UI

#endif