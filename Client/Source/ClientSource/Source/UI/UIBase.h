/**
 * \file	UI\UIBase.h
 *
 * Declares the user interface base class.
 */


#if !defined( __UI_BASE_H__)
#define __UI_BASE_H__

#include <unordered_map>

#include "UIFramework.h"
#include "UISlide.h"
#include "i3Base/itl/range/generic_empty.h"

typedef void	(*ErrMessageHandler)(void* pThis,INT32 nParam);

typedef UINT32 UI_STYLE;
#define UI_STYLE_NO_CLOSEBUTTON		0x00000001		// Close Button이 없는 Popup. Popup 이외의 영역을 Click하면 닫히도록 한다.
#define UI_STYLE_REOPENTYPE			0x00000002		// 열려 있어도 다시 열수 있는 팝업 형태
#define UI_STYLE_INCLUDE_OKBUTTON	0x00000004		// OK Button을 갖고 있다. (기본적으로 Enter Key를 눌러도 동작됨)

#define UI_SLIDEIN_SLOP	2.f
#define UI_SLIDEOUT_SLOP 3.f


enum UISOUND_TYPE
{
	UISND_SCROLLIN = 0,
	UISND_SCROLLOUT,
	UISND_POPUP_OPEN,
	UISND_POPUP_CLOSE,
	UISND_ITEM_SELECTING,
	UISND_ITEM_CLICK_OK,
	UISND_ITEM_CLICK_CANCEL,
	UISND_ITEM_EQUIP,

	// 추가된 Sound
	UISND_ERROR,
	UISND_CLICK,
	UISND_ENTER_MOUSE1,
	UISND_ENTER_MOUSE2,
	UISND_PAGE_SHIFT,
	UISND_POPUP_FADE_IN,
	UISND_SERVER_CONNECT,
	UISND_SERVER_ENTER,
	UISND_WARNING,
	UISND_LIST_OVER,
	UISND_MOUSE_OVER,

	UISND_USER_GUIDE_START,
	UISND_USER_GUIDE_COIN,
	UISND_USER_GUIDE_START4,

	UISND_MAX
};

enum	SLIDESTATE
{
	SS_IN_OK = 0,	// 등장 완료
	SS_OUT_OK,		// 퇴장 완료
	SS_IN_START,	// 등장 시작
	SS_OUT_START,	// 퇴장 시작
	SS_SLIDING_IN,	// 등장 중
	SS_SLIDING_OUT,	// 퇴장 중

	SS_MAX
};

struct UISCENEINFO
{
	char			_szPath[ MAX_PATH];
	i3UIScene *		_pScene;
	LuaFuncReg *	_GlueFunc;
	bool			_EnableScn;			//추가..
	bool			_LoadedStyle;		// Memory에 Loaded상태
	
	UISCENEINFO();
	~UISCENEINFO();
	void Reset( void);
};


namespace UILua
{
	LuaState *		CallLuaFunction( i3UIScene* pScene, const char* pszFunction);
	void			EndLuaFunction( LuaState* L, INT32 nArg );
	bool			ReturnLuaBoolean( LuaState* L, INT32 nArg);
	REAL32			ReturnLuaNumber( LuaState * L, INT32 nArg);
	const char*		ReturnLuaString( LuaState* L, INT32 nArg);
	void *			ReturnLuaPointer( LuaState* L, INT32 nArg);
	INT32			ReturnLuaInteger( LuaState* L, INT32 nArg);
	void			RegisterLuaFunctions( i3UIScene* pScene, LuaFuncReg * GlueFunc);
	void			RegisterLuaFunctions(LuaState* L, LuaFuncReg * GlueFunc);
}

typedef void	(*GameEventFunc)(INT32, const i3::user_data&);

class UIBase : public i3GameNode
{
	I3_ABSTRACT_CLASS_DEFINE( UIBase, i3GameNode);

	friend class UIPhaseLogin;
	friend class UIPhaseLogin_V11;

	friend class UISubBase;
	friend class UIMainFrame;
	friend class UIFramework;

private:
	i3::vector<UISCENEINFO*> m_UISceneList;

	REAL32		m_rAwayInputTime;
	REAL32		m_rCautionTime;
	bool		m_bOffAwayInput;
	bool		m_bOffCaution;	

	// Sound
	i3SoundPlayInfo * m_pGuiSound[UISND_MAX];

	i3UIControl *	m_pLastMouseOverCtrl;
	bool m_pre_loaded;

protected:
	bool		m_bInputDisable;
	UIMessageBox*	m_pExitMessageBox;

private:
	void		__FindToolTipTemplate( void);

	/** \brief SceneFile Path를 등록합니다.
		\desc __LoadScene()을 호출하여 등록된 Path의 Scene 로드하게 됩니다. */
	void		__RegisterScene( const char * pszScenePath, const LuaFuncReg * GlueFunc = nullptr, bool bSceneEnable = true);

	/** \brief Register된 Scene을 Load합니다. */
	void		__LoadScene( void);
	/** \brief pInfo에 등록된 Path로 UIScene을 Load합니다 */
	void		__DirectlyLoadScene( UISCENEINFO * pInfo);

	void		__BindScene( i3UIScene * pScene, LuaFuncReg * GlueFunc, bool bEnale);
	void		__UnbindScene( i3UIScene * pScene);

	/** \brief Loading된 scene을 메모리에서 해제합니다. */
	void		__UnloadScene( void);

	/** \brief 해당 Scene을 해제합니다. */
	void		__UnloadScene( i3UIScene * pScene);

	/** \brief 모든 Scene을 해제하고 Register정보를 삭제합니다. */
	void		__ClearScene( void);

protected:
	UI_STYLE	m_UIStyle;

	/** \brief CloseButton이 없는 경우 확인
		\desc Close Button이 없는 Popup. Popup 이외의 영역을 Click하면 닫히도록 한다. */
	bool		isNoCloseButton( void)			{ return (m_UIStyle & UI_STYLE_NO_CLOSEBUTTON) == UI_STYLE_NO_CLOSEBUTTON; }

	/** \brief 열려 있어도 다시 열수 있는 팝업(Floating) 형태 */
	bool		isReopenStyle( void)			{ return (m_UIStyle & UI_STYLE_REOPENTYPE) == UI_STYLE_REOPENTYPE; }

	/** \brief OKButton을 갖고 있는 형태
		\desc Enter Key를 눌러도 동작한다. */
	bool		isIncludeOKButton( void)		{ return (m_UIStyle & UI_STYLE_INCLUDE_OKBUTTON) == UI_STYLE_INCLUDE_OKBUTTON; }


	//--------------------------------------------------------------------------------------//
	//										Slide system									//
	//--------------------------------------------------------------------------------------//
	i3UIFrameWnd *			m_pFrameWnd;
	i3UIImageBoxEx *		m_pBG;

	SLIDESTATE	m_SlideState;

	/** \BG 사이즈 세팅합니다.  */
	virtual void			_Resize();

	void		_OnSlideInOnce();
	void		_OnSlidingIn(REAL32 rDeltaTime);
	void		_OnSlideInEnd();

	void		_OnSlideOutOnce();
	void		_OnSlidingOut(REAL32 rDeltaTime);
	void		_OnSlideOutEnd();
	
	virtual void OnSlideInOnce() {}
	virtual void OnSlideOutOnce() {}

	UISlide  m_slide;
	TUISlide_Callback< UIBase,	&UIBase::_OnSlideInOnce, &UIBase::_OnSlidingIn, &UIBase::_OnSlideInEnd, 
						&UIBase::_OnSlideOutOnce, &UIBase::_OnSlidingOut, &UIBase::_OnSlideOutEnd > m_slideCB;

	//--------------------------------------------------------------------------------------//
	//									LuaContext											//
	//--------------------------------------------------------------------------------------//
	i3::rc_string m_strLuaContextName;

	/** \brief Lua function을 호출한다.
		\param[in] pszScene UI Scene Name
		\param[in] pszFunction Lua Function Name
		\return LuaState */
	LuaState *	_CallLuaFunction( const char * pszScene, const char * pszFunction);
	LuaState *	_CallLuaFunction( INT32 idxScene, const char* pszFunction);
	LuaState *	_CallLuaFunction( const char * pszFunction);

	/** \brief Lua function 호출을 끝낸다. */
	void		_EndLuaFunction( LuaState* L, INT32 nArg );

	/** \brief Lua Function 호출
		\desc call 후에 return값을 받는다.
		\return bool		*/
	bool		_ReturnLuaBoolean( LuaState* L, INT32 nArg);

	REAL32		_ReturnLuaNumber( LuaState * L, INT32 nArg);

	void		_ReturnLuaVec2D(VEC2D * pOut,  LuaState * L, INT32 nArg);

	const char *_ReturnLuaString( LuaState* L, INT32 nArg);

	void *		_ReturnLuaPointer( LuaState* L, INT32 nArg);
	
	INT32		_ReturnLuaInteger( LuaState* L, INT32 nArg);

	/** \brief Lua에서 Global variable을 가져옵니다.
		\param[in] pszScene Scene Name
		\param[in] pGlobalName Lua script의 변수명
		\param[in] stackPos Stack에 들어가는 위치값.
			스택은 반대로 가져오기때문에 먼저 선언된 변수가 더 낮은 값을 갖습니다.
			예) g_Width, 와 g_Height 의 두 변수를 접근하기 위해서는 g_Width는 -2를 g_Height는 -1의 스택Position을 주어야 됩니다.
			*/
	INT32		_GetLuaNumber( const char * pszScene, const char * pszGlobalName, INT32 StackPos);

	INT32		_GetLuaNumber( const char * pszGlobalName, INT32 StackPos = -1);

	/** \brief LuaGlue Function을 등록합니다. */
	void		_RegisterLuaFunctions( LuaState* L, LuaFuncReg * GlueFunc = nullptr);

	/** \brief 해당 Path의 SceneFile을 Load합니다 */
	virtual i3UIScene *	_LoadScene( const char * pszScenePath, LuaFuncReg * GlueFunc = nullptr, bool bSceneEnable = true);

	/** \brief SceneFile을 Load하고 LuaContext가 생성되면 호출됩니다.
		\desc Lua Function을 호출하여 초기화할 것을 여기에 define합니다. */
	virtual void	_InitializeAtLoad( i3UIScene * pScene);

	/** \brief SceneFile을 Unload하고 초기화를 합니다.
		\desc Scene을 변경한 경우 해제해주십시오. */
	virtual void	_ClearAtUnload( i3UIScene * pScene);

	/** \brief 모든 Scene을 Load하고 호출됩니다. */
	virtual void	OnLoadAllScenes() {}

	/** \brief 모든 Scene을 Unload하기 전에 호출됩니다. */
	virtual void	OnUnloadAllScenes() {}

	i3GameNode *	_FindListBoxFromCombo( i3GameNode * pNode);

	bool			_IsHost( void);

	void			_DefaultErrorPopup( const i3::wliteral_range& wMsgRng, INT32 event, INT32 arg);
	
	void			_AttachToTopScreen( void);


	//------------------------------------------------------------------------------//
	//								Inline Functions								//
	//------------------------------------------------------------------------------//
public:
	UISlide*		GetSlide(void) { return &m_slide; }
	SLIDESTATE		GetSlideState( void) const { return m_SlideState; }
	i3UIFrameWnd*	GetFirstFrameWnd() const { return m_pFrameWnd; }
	bool			IsReadyToUpdate( void)	{ if( m_pFrameWnd == nullptr || m_pFrameWnd->IsLoaded() == false) return false; return true;}

	const VEC3D*	GetMainFrameWndPos()	{ if (m_pFrameWnd == nullptr || m_pFrameWnd->IsLoaded() == false) return nullptr; return m_pFrameWnd->getPos(); };
	const VEC2D*	GetMainFrameWndSize()	{ if (m_pFrameWnd == nullptr || m_pFrameWnd->IsLoaded() == false) return nullptr; return m_pFrameWnd->getSize(); };

public:
	UIBase();
	virtual ~UIBase();

	/** \brief i3UIE 파일을 등록합니다.
		\param[in] pszScenePath Path
		\param[in] GlueFunc Lua Glue Function을 등록하는 Struture pointer		 */
	i3UIScene *		AddScene( const char * pszScenePath, const LuaFuncReg * GlueFunc = nullptr, bool bLoaded = false, bool bSceneEnable = true);
	
	/** \brief 기본 생성 코드입니다.
		\desc Scene file을 등록합니다. */
	virtual void	OnCreate( i3GameNode * pParent);

	/** \brief Scene을 제거하고 자신을 메모리에서 해제합니다. */
	void			Destroy( void);

	/** \brief Update
		\see i3GameNode::OnUpdate */
	virtual void	OnUpdate( REAL32 rDeltaSeconds);

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) {}

	virtual void	ReceiveGameEvent_Unknown( GAME_EVENT evt, INT32 arg) {}

#if defined(I3_DEBUG)
#pragma warning(push)
#pragma warning(disable : 4481)
	virtual void	ProcessGameEvent(GAME_EVENT evt, INT32 arg) sealed {}		// 실수방지를 위해 추가..(2013.07.17.수빈)
#pragma warning(pop)
#endif
	/** \brief i3Engine Event입니다.
		\see i3EventReceiver::OnEvent */
	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);

	/** \brief Key 입력 처리 */
	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng);

	/** \brief Key입력 Enter */
	virtual bool	OnKey_Enter()		{ return false; }

	/** \brief Key입력 Escape */
	virtual bool	OnKey_Escape()		{ return false; }

	/** \brief Spirte Loading이 완료되면 호출됩니다. */
	virtual void	OnCompleteLoad( void) {}

	/** \brief Control에 Mouse Over되면 호출됩니다. */
	virtual bool	OnEnterMouse( i3UIControl * pControl);

	/** \brief Control에 Mouse leave되면 호출됩니다. */
	virtual void	OnLeaveMouse( i3UIControl * pControl);

	/** \brief UI Event Click*/
	virtual bool	OnClick( i3UIControl * pControl);

	/** \brief UI Event Rbutton Click */
	virtual void	OnRClick( i3UIControl * pControl);

	virtual bool	OnDblClick( i3UIControl * pControl);

	/** \brief UI Event Mouse Wheel */
	virtual void	OnWheel( i3UIControl * pControl, INT32 zDelta) {}

	virtual void	OnDragStart( i3UIControl * pControl, INT32 x, INT32 y) {}
	virtual void	OnDragging( i3UIControl * pControl, INT32 x, INT32 y)	{}
	virtual void	OnDragEnd( i3UIControl * pControl, INT32 x, INT32 y) {}

	/** \brief Enable을 설정합니다.
		\desc i3UIScene의 Enable도 같이 설정합니다.
		\note Scene을 Enable시 Load를 합니다.
			자주 Enable이 호출될 경우 성능 저하가 일어날 수 있으므로 EnableCtrl을 이용한 방식(또는 SetVisible)을 사용하십시오. */
	virtual void	SetEnable( bool bTrue = true, REAL32 tm = 0.f);

	/** \brief Visible을 설정합니다.*/
	virtual void	SetVisible( bool bTrue = true, REAL32 tm = 0.f);
	
	i3UIScene *		GetScene( const char * pszSceneName) const;
	i3UIScene *		GetScene( INT32 idxScene) const;
	INT32			GetSceneCount() const { return (INT32)m_UISceneList.size(); }
	bool			IsEnableFromSceneInfo(INT32 idxScene) const;
	void			SetEnableToSceneInfo(INT32 idxScene, bool bEnable);

	virtual bool	OnLostDevice( bool bLostDevice);
	virtual bool	OnRevive( i3RenderContext * pCtx);


	/** \brief Disable Input
		\desc Scene의 i3UIControl들의 Input을 막습니다.
			Scene에 등록하지 않고 따로 참조하는 Control이나 UIBase가 있는 경우 하위 Class에 정의하십시오. */
	virtual void	SetInputDisable( bool bDisable);
	bool			IsInputDisable() const { return m_bInputDisable || i3UI::getDisableUIInput(); }

	/** \brief 효과음을 출력한다. */
	bool			PlaySound( UISOUND_TYPE type);

	// Phase / State별 재생 사운드 구분.
	virtual bool	OnPlayClickSound(i3UIControl * pCtrl);
	virtual bool	OnPlayEnterMouseSound(i3UIControl * pCtrl);

	/** \brief LuaState를 갖고 있는지 검사합니다.
		\note false라면 다른 Scene의 LuaState 입니다. */
	bool			IsLuaContext( LuaState * L);

	/** \brief Scene에 해당 Control이 있는지 확인한다. */
	bool			IsIncludeControl( i3UIControl * pControl);

	bool			_isEmptyLuaContext() const	{ return m_strLuaContextName.empty(); }

	//------------------------------------------------------------------------------//
	//								Game Event Functions							//
	//------------------------------------------------------------------------------//
	/** \brief 초기화 함수	*/
	virtual void	OnReset( void) {}

	/** \brief Sliding 명령을 줍니다.
		\return N/A */
	virtual void	OnSlideIn( void);

	/** \brief Sliding 명령을 줍니다.
		\return N/A */
	virtual void	OnSlideOut( void);

	bool			isSliding( void)				{ return (m_SlideState == SS_SLIDING_IN) || ( m_SlideState == SS_IN_START) ||	// 등장 시작
															( m_SlideState == SS_OUT_START) || (m_SlideState == SS_SLIDING_OUT); }

	/** \brief 입장 시작
		\desc 기본적으로 Slide in을 설정합니다.
		\return false이면 창을 띄우지 않는다.*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr);

	/** \brief 입장중 처리 */
	virtual bool	OnEntrancing( REAL32 rDeltaSeconds);

	/** \brief 입장 완료 처리 */
	virtual void	OnEntranceEnd( void);

	/** \brief 퇴장 시작
		\desc 기본적으로 Slide Out을 한다. */
	virtual bool	OnExitStart( void) { OnSlideOut(); return true;}

	/** \brief 퇴장중 */
	virtual bool	OnExiting( REAL32 rDeltaSeconds);

	/** \brief 퇴장 완료 */
	virtual void	OnExitEnd( void);

	/** \brief 나가기 버튼을 누르면 동작
		\desc 일반적으로 종료하기 Popup을 띄운다. */
	virtual void	OnEscapeKey( void);

	/** \brief 키보드 Enter 입력 여부 */
	bool			isInputKeyboardEnter( void);

	//------------------------------------------------------------------------------//
	//								Callback Functions								//
	//------------------------------------------------------------------------------//
	static void		CbExitNormal( void* pThis, INT32 nParam);
					
	LuaState *		__CallLuaFunction_Internal( i3UIScene* pScene, const char* pszFunction);


	//------------------------------------------------------------------------------//
	//								Old Gui Function								//
	//------------------------------------------------------------------------------//
	virtual void	OnPreGuiNotify(void)	{}
	virtual void	OnGuiNotify( I3GUI_CONTROL_NOTIFY* pNotify){}
	virtual void	OnPostGuiNotify(void)	{}


	//------------------------------------------------------------------------------//
	//								ETC Function									//
	//------------------------------------------------------------------------------//

};

struct MultiSceneInfo
{
	MultiSceneInfo() : idx(-1), scn(0) {}

	INT32 idx;
	i3UIScene* scn;
};

#endif
