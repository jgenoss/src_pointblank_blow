#if !defined( __I3UI_MANAGER_H)
#define __I3UI_MANAGER_H

#include "i3UIDefines.h"
#include "i3UIFrameWnd.h"
#include "i3UIControl.h"
#include "i3UIRoot.h"
#include "../i3InputDispatchManager.h"
#include "../i3Viewer.h"
#include "i3UIModalDriver.h"
#include "i3UILibrary.h"
#include "i3UIScene.h"
#include "i3Ime.h"
#include "i3UIButtonImageSet.h"
#include "i3UIAnimationMgr.h"

enum I3UI_MSGBOX
{
	I3UI_MSGBOX_OK = 0,
	I3UI_MSGBOX_YESNO,
	I3UI_MSGBOX_YESNOCANCEL,

	I3UI_MSGBOX_COUNT
};

struct I3UI_SCENE_INFO : i3::class_common_pool<I3UI_SCENE_INFO>
{
	i3UIScene	*	m_pScene = nullptr;					// name, attached states
	i3::rc_string	m_strSceneFile;				//
	bool			m_bEnabled = false;
};

//i3UI::CreateControl을 위한 최소한의 정보
struct UI_CONTROL_INFO
{
	i3::rc_string	m_strName;
	i3GameNode*		m_pParent = nullptr;
	i3UITemplate*	m_pTemplate = nullptr;
	VEC2D			m_vSize;
	VEC3D			m_vPos;
};

class i3UIConsoleClientContext;
struct SERVICECOUNTRY;

class I3_EXPORT_FRAMEWORK i3UIManager : public i3InputDispatchManager
{
	I3_EXPORT_CLASS_DEFINE(i3UIManager, i3InputDispatchManager);

	friend	class			i3UIFrameWnd;

protected:
	i3Framework *			m_pFramework = nullptr;

	i3::vector<I3UI_SCENE_INFO*> m_listScene;

	i3::vector<i3::vector<i3UIScene*>> m_listDraw;

	i3VirtualTexture	*	m_pVirtualTexture = nullptr;
	i3UILibrary	*			m_pUILibrary = nullptr;

	i3Ime				*	m_pIme = nullptr;


	i3::vector<i3UIFrameWnd*> m_RemoveWndList;				

	i3UIModalDriver *		m_pDefModalDriver = nullptr;

	i3TStack<i3UIFrameWnd *>	m_ModalStack;

	i3UIFrameWnd *			m_pCurActiveFrameWnd = nullptr;
	i3UIControl *			m_pLastOnMouseTarget = nullptr;
	bool					m_bExclusiveActive = false;

	REAL32					m_sfUIResolutionWidth = 1.0f;
	REAL32					m_sfUIResolutionHeight = 1.0f;

	REAL32					m_sfUIResolutionWidthInGame = 1.0f;
	REAL32					m_sfUIResolutionHeightInGame = 1.0f;

	INT32					m_nWidth = 1024;
	INT32					m_nHeight = 768;

	INT32					m_ActualWidth = 1024;
	INT32					m_ActualHeight = 768;

	INT32					m_DisplayOffsetX = 0;
	INT32					m_DisplayOffsetY = 0;

	bool					m_bDisableInput = false;

	UINT32					m_mkPress = 0;				// I3I_MOUSE
	UINT32					m_mkStroke= 0;				// I3I_MOUSE
	UINT32					m_mkLastStroke = 0;			// I3I_MOUSE
	REAL32					m_rClickTimeStamp = 0.0f;
	REAL32					m_rRepeatTimeStamp[3] = { 0.0f, 0.0f, 0.0f };	// LBUTTON, RBUTTON, MBUTTON

	POINT					m_nCurrentMousePos = { 0, 0 };

	i3UIFrameWnd *			m_pDefMsgBox[I3UI_MSGBOX_COUNT];

	//현재 Scene의 Full Path
	//나중에 Virtual Texture, Template Library를 로딩하다 파일을 못찾는 경우(워킹폴더이용)
	//Scene이 있는 폴더를 뒤져보기 위해 사용된다. 
	i3::rc_string			m_strCurrentScenePath;

//	i3::rc_string			m_strLocale;	// 국가 폴더 path 

	bool					m_bEditMode = false;

	i3::unordered_set<i3UIControl*>*		m_pmapControlHolderForTool = nullptr;	// 툴에서만 쓸것이므로 포인터로 처리..
	
	typedef void (*PostDrawCallbackPFN)(i3UIManager*, i3UIRenderer*);
	static PostDrawCallbackPFN ms_cbOnPostDraw;

private:
	void					PauseIO();
	void					ResumeIO();
	friend class			i3Framework;

protected:
	//void					_Rec_BuildSceneGraphByWindow( i3UIControl * pObj);
	//void					_Rec_BuildSceneGraphByControl( i3UIControl * pObj);
	i3UIControl *			_Rec_FindChildControlByName( LuaState * L, i3UIControl * pCtrl, const char * pszName);

	i3EventReceiver *		_Find_EventReceiver( i3UIControl * pLeaf);

	// SetActiveFrameWnd,EnableWindow  bUpdateActiveFrame 추가 ;jisoo
	void					SetActiveFrameWnd( i3UIFrameWnd * pWindow, bool bExclusiveActive = false, bool bUpdateActiveFrame = true);
	void					EnableWindow( i3UIFrameWnd * pWnd, bool bEnable, bool bExclusiveActive = false, bool bUpdateActiveFrame = true);
	void					ShowWindow( i3UIFrameWnd * pWnd, bool bShow);

	void					_Reg_RemoveWnd( i3UIFrameWnd * pWnd);		// 이 함수는 쓰이지 않는것같다..(2012.05.14.수빈)
																		// 지연삭제는 FrameWnd보다는 i3UIScene수준에서 하는게 더 좋아보인다..
	
	bool					TA_Rec_GetPickedCtrl( i3GameNode * pNode, POINT pt, std::vector<i3UIControl *> & list);

	void					RegisterControlForTool(i3UIControl* pCtrl);
	void					UnregisterControlForTool(i3UIControl* pCtrl);

	friend class i3UIControl;
public:
	void					RefreshAllActiveTemplateForTool();

	i3Framework *			getFramework( void)					{ return m_pFramework; }
	void					setInputDisable(bool bDisable)		{	m_bDisableInput = bDisable;}
	bool					getInputDisable(void)				{	return m_bDisableInput;}

	bool					isEditMode( void)					{ return m_bEditMode; }
	void					SetEditMode( bool bEditMode)		{ m_bEditMode = bEditMode; }



public:
	i3UIManager(void);
	virtual					~i3UIManager( void);

	virtual void			OnUpdate( REAL32 rDeltaSeconds ) override;

	bool					Create( i3Framework * pFramework, bool bEditMode = false);	
	void					Reset(void);
	void					ResetlistDraw(void);

	i3UIScene*				LoadUIScene( const char * szPath);
	void					AddFromUIRoot( i3UIRoot * pRoot);

	i3UIFrameWnd	*		getActiveFrameWnd(void)							{ return m_pCurActiveFrameWnd;}
	void					delActiveFrameWnd(void)							{ m_pCurActiveFrameWnd = nullptr; }
	void					AddFrameWnd( i3UIFrameWnd * pFrameWnd);
	void					RemoveFrameWnd( i3UIFrameWnd * pFrameWnd);

	void					ShowSceneTop( i3UIScene* pScene);			// pScene Draw시 최상단으로 가져오도록
	void					RegisterDefaultMessageBox( I3UI_MSGBOX, const char * pszPath);
	void					MsgBox( I3UI_MSGBOX msg, const WCHAR16 * pszMsg, i3UIModalDriver * pDriver);

	bool					DoModal( i3UIFrameWnd * pFrameWnd, bool bCenter = true, i3EventReceiver * pReceiver = nullptr);
	void					EndModal( i3UIControl * pCtrl, i3UIControl*pRecv, I3UI_MODALRESULT rv);

	REAL32					getUIResolutionWidth(void)						{ return m_sfUIResolutionWidth; }
	void					setUIResolutionWidth( REAL32 val)				{ m_sfUIResolutionWidth = val; }
	REAL32					getUIResolutionHeight(void)						{ return m_sfUIResolutionHeight; }
	void					setUIResolutionHeight( REAL32 val)				{ m_sfUIResolutionHeight = val; }

	REAL32					getUIResolutionWidthInGame(void)				{ return m_sfUIResolutionWidthInGame; }
	void					setUIResolutionWidthInGame( REAL32 val)			{ m_sfUIResolutionWidthInGame = val; }
	REAL32					getUIResolutionHeightInGame(void)				{ return m_sfUIResolutionHeightInGame; }
	void					setUIResolutionHeightInGame( REAL32 val)		{ m_sfUIResolutionHeightInGame = val; }

	INT32					getWidth(void)									{ return m_nWidth;}
	INT32					getHeight(void)									{ return m_nHeight;}

	INT32					getActualWidth(void)							{ return m_ActualWidth; }
	INT32					getActualHeight(void)							{ return m_ActualHeight; }
	void					SetActualResolution( INT32 w, INT32 h);

	INT32					getDisplayOffsetX(void)							{ return m_DisplayOffsetX; }
	INT32					getDisplayOffsetY(void)							{ return m_DisplayOffsetY; }

	REAL32					getScreenStretchRatioX(void)					{ return (REAL32) m_nWidth / m_ActualWidth; }
	REAL32					getScreenStretchRatioY(void)					{ return (REAL32) m_nHeight / m_ActualHeight; }

	void					ConvertMouseCoord( POINT * pPt);
	void					ConvertToClientCoord(POINT * pPt);

	POINT					getCurrentMousePosition( void)					{ return m_nCurrentMousePos; }



	void					SetUIResolution( INT32 width, INT32 height);
	void					OnSize(void);

	void					SetViewer(i3Viewer* pViewer, bool bNoResize = true);

	i3UIControl *			FindChildControlByName( LuaState * L, const char * pszName);
	i3UIControl *			FindChildControlByName( const char* pszName, i3UIScene* pScene);
	i3UIScene	*			FindAncestorByState(LuaState * pState);

	// 이 함수는 CaseRun에서 Control의 경로 ( ex. Ancester1/...Parent/Control)를 이용해
	// 특정 Control을 찾아내기 위한 함수다.
	// Control의 경로는 i3UIControl::GetCtrlPath() 함수를 통해 얻어진다.
	i3UIControl *			TA_FindControlByPath(const char * pszPath);
	bool					TA_GetPickedCtrl( POINT pt, std::vector<i3UIControl *> & list);

	INT32					AddScene( I3UI_SCENE_INFO * pInfo);
	void					RemoveScene( i3UIScene * pScene);
	void					RemoveAllScene(void);
	i3::vector<I3UI_SCENE_INFO*>&	getSceneInfoList(void)							{ return m_listScene;}
	void					ChangeSceneFile( i3UIScene* pScene, const char* pszNewName);

	// 현재 Mouse가 올려져 있는 Window를 반환합니다.
	i3UIFrameWnd *			HitTest( I3UI_PICK_INFO * pInfo);

	i3UIFrameWnd *			GetFocusFrameWnd(void);
	i3EventReceiver	*		GetFocus(void);
	void					SetFocusSceneFrameWnd(i3UIScene* scn, bool bPush = false);
	void					SetFocusControl(i3UIControl* ctrl, bool bPush = false);

	virtual bool			DispatchLButtonDown( UINT32 nFlag, POINT point) override;
	virtual bool			DispatchLButtonUp( UINT32 nFlag, POINT point) override;
	virtual bool			DispatchLButtonDblClk( UINT32 nFlag, POINT point) override;
	virtual bool			DispatchMButtonDown( UINT32 nFlag, POINT point) override;
	virtual bool			DispatchMButtonUp( UINT32 nFlag, POINT point) override;
	virtual bool			DispatchRButtonDown( UINT32 nFlag, POINT point) override;
	virtual bool			DispatchRButtonUp( UINT32 nFlag, POINT point) override;
	virtual bool			DispatchMouseMove( UINT32 nFlag, POINT point) override;
	virtual bool			DispatchMouseWheel( UINT nFlag, short zDelta, POINT point) override;
	virtual bool			DispatchKeyDown( UINT32 nKey) override;
	virtual bool			DispatchKeyUp( UINT32 nKey) override;
	virtual bool			DispatchActivate( bool bActivate) override;
	virtual bool			DispatchSetFocus(void) override;
	virtual bool			DispatchKillFocus(void) override;
	virtual bool			DispatchInput(void) override;

	virtual void			OnChangeFocus( i3Framework * pFramework, i3EventReceiver * pFrom, i3EventReceiver * pTo) override;

	// Control notify handler functions
	void					Notify( i3UIControl * pCtrl, I3UI_EVT nEvent, INT32 nPriParam,INT32 nSecParam);
	void					ParentNotify( i3UIControl * pParent, i3UIControl * pChild, I3UI_EVT nEvent, INT32 nPriParam, INT32 nSecParam);

	void					resetOnMouseTarget(void);
	i3UIControl*			getCurrMouseTarget() const { return m_pLastOnMouseTarget; }
	void					checkControlRelease( i3UIControl * pControl);

	// post rendering handler
	static void				SetPostDrawCallback(PostDrawCallbackPFN pfn) { ms_cbOnPostDraw = pfn; }

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// UI library
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool					OpenTemplateLibrary_Depricated(const char * pszName);
	void					MergeTemplateLibrary( const char * pszName);
	void					CloseTemplateLibrary();

	bool					LoadTemplateLibrary(const char* pszName);
	bool					LoadToolTemplateLibrary(const char* pszName);

	// 경로가 고정되기 때문에, 파일 이름이외에는 경로가 무시된다.
	void					SaveTemplateLibrary_UilVersion(const i3::literal_range& filename);
	void					SaveTemplateLibrary_TplVersion(const i3::literal_range& filename);


	void					setUILibrary(i3UILibrary * pLibrary)	
	{ 
		I3_REF_CHANGE(m_pUILibrary, pLibrary); 
	}
	i3UILibrary		*		getUILibrary( void)						{	return m_pUILibrary;}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Virtual Texture
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void					SetVirtualTexture( i3VirtualTexture * pVTex, INT32 cache_cx = 2048, INT32 cache_cy = 2048);
	bool					OpenVirtualTexture( const char * pszname, INT32 cache_cx = 2048, INT32 cache_cy = 2048);
	void					AdjustVirtualTexture(bool isNewFormat);
	void					CloseVirtualTexture(void);
	bool					ResizeVRAMCache( INT32 cx, INT32 cy);

	i3VirtualTexture	*	getVirtualTexture(void)	{	return m_pVirtualTexture;}

	i3Ime				*	getGlobalIme(void)		{	return m_pIme;}
	void					setGlobalIme(i3Ime*pIme){	I3_REF_CHANGE( m_pIme, pIme);}

	void					MergeVirtualTexture( const char * pszName);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Lua
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void					RegisterLuaFunction(LuaState * pState);

	void					SendEvent( INT32 code, i3UIControl * pCtrl, INT32 arg1, INT32 arg2);
	void					SendNotify( INT32 code, i3UIControl * pRecv, i3UIControl * pFrom, INT32 arg1 = 0, INT32 arg2 = 0);

	bool					Compile( const char* pszFilename, char* pOutResult, INT32 nBufSize );
	void					SetCurrentScenePath( const char * pszCurrentScenePath)	{	m_strCurrentScenePath = pszCurrentScenePath;	}
	const i3::rc_string&	GetCurrentScenePath() const { return m_strCurrentScenePath;  }

	virtual bool			OnLostDevice( bool bLostDevice) override;
	virtual bool			OnRevive( i3RenderContext * pCtx) override;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Console
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// i3UIManager가 생성된이후에는 호출이 무시됩니다.
	static void				InitConsole(bool useConsole, i3ClassMeta* metaConsoleClientCtx);
	static void				ShowConsole(bool showConsole);

	static bool				IsUsingConsole(bool onlyUseable=false);
	static bool				IsShowConsole();

	static INT32			CExec(const wchar_t* fmt, ...); // ret: 0 for OK (EI3UIConsoleRst)

	static void				CLog(const wchar_t* fmt, ...);  // 콘솔이 열려있든 안열려있든 로그기록
	//컬러로그, attr= attr(8bit), r(8),g(8),b(8) ex) 0=default, 0x00ff0000 빨강, 0x000000ff 파랑
	static void				CLogA(UINT32 attr, const wchar_t* fmt, ...);
	
	// skipGivenParamScene=true이면, 파라미터로 주어진 scene의 다음 scene부터 찾습니다.
	static i3UIScene*		FindConsoleScene(i3UIScene* scene, bool skipGivenParamScene=false);
	static bool				OnConsoleEvent(UINT32 event, i3ElementBase* pObj, UINT32 param2, I3_EVT_CODE code=I3_EVT_CODE_ACTIVATE);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Test Automation
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	i3UIControl *			TA_ScanText( const wchar_t * pszText, bool bExact);
};

typedef void(CALLBACK* pfCallback)(int,int);
namespace i3UI
{
	I3_EXPORT_FRAMEWORK		void					setToolDevMode(bool bSet);
	I3_EXPORT_FRAMEWORK		bool					getToolDevMode(void);

	I3_EXPORT_FRAMEWORK		i3UIManager *			getManager(void);
	I3_EXPORT_FRAMEWORK		void					Reset(void);
	I3_EXPORT_FRAMEWORK		void					releaseControl( i3UIControl * pControl);

	I3_EXPORT_FRAMEWORK		i3Viewer*				getViewer(void);	
	I3_EXPORT_FRAMEWORK		REAL32					getUIResolutionWidth(void);
	I3_EXPORT_FRAMEWORK		REAL32					getUIResolutionHeight(void);

	//	I3_EXPORT_FRAMEWORK		void					ChangeResolution( i3Viewer* pViewer);

	I3_EXPORT_FRAMEWORK		void					setDisableUIInput( bool bTrue);
	I3_EXPORT_FRAMEWORK		bool					getDisableUIInput(void);

	I3_EXPORT_FRAMEWORK		i3EventReceiver	*		GetFocus(void);
	I3_EXPORT_FRAMEWORK		void					SetFocusSceneFrameWnd(i3UIScene* scn, bool bPush = false);
	I3_EXPORT_FRAMEWORK		void					SetFocusControl(i3UIControl* ctrl, bool bPush = false);


	I3_EXPORT_FRAMEWORK		bool					LoadUI( const char * pszPath);
	I3_EXPORT_FRAMEWORK		i3UIFrameWnd *			FindFrameWndByName( const char * pszName, i3EventReceiver * pDriver = nullptr);

	I3_EXPORT_FRAMEWORK		void					Notify(  i3UIControl * pCtrl, I3UI_EVT nEvent, INT32 nPriParam,INT32 nSecParam);
	I3_EXPORT_FRAMEWORK		void					ParentNotify( i3UIControl * pParent, i3UIControl * pChild, I3UI_EVT nEvent, INT32 nPriParam, INT32 nSecParam);

	I3_EXPORT_FRAMEWORK		bool					DoModal( const char * pszPath, bool bCenter = true, i3UIModalDriver * pDriver = nullptr);
	I3_EXPORT_FRAMEWORK		void					EndModal( i3UIControl * pCtrl, i3UIControl * pRecv, I3UI_MODALRESULT result);

	I3_EXPORT_FRAMEWORK		i3UITemplate		*	FindTemplateByName( const char * pszName);
	I3_EXPORT_FRAMEWORK		i3UITemplate		*	FindTemplateByHash( UINT128 hash);

	I3_EXPORT_FRAMEWORK		i3UIControl			*	FindControlByName( LuaState * L, const char * pszName);
	I3_EXPORT_FRAMEWORK		i3UIControl			*	FindControlByName( const char* pszName, i3UIScene* pScene);
	I3_EXPORT_FRAMEWORK		i3UIScene			*	FindAncestor(LuaState * pState);

	// Lua
	I3_EXPORT_FRAMEWORK		void					RegisterLuaFunction(LuaState * pState);

	// ui library
	I3_EXPORT_FRAMEWORK		i3UILibrary*			getUILibrary(void);
	I3_EXPORT_FRAMEWORK		void					setUILibrary(i3UILibrary * pLibrary);

	// virtual texture
	I3_EXPORT_FRAMEWORK		bool					OpenVirtualTexture( const char * pszName, INT32 cx, INT32 cy);
	I3_EXPORT_FRAMEWORK		void					CloseVirtualTexture();
	I3_EXPORT_FRAMEWORK		bool					ResizeVRAMCache( INT32 cx, INT32 cy);

	I3_EXPORT_FRAMEWORK		i3VirtualTexture	*	getVirtualTexture(void);
	I3_EXPORT_FRAMEWORK		void					setVirtualTexture( i3VirtualTexture * pTexture, INT32 cx, INT32 cy);

	I3_EXPORT_FRAMEWORK		void					AdjustVirtualTexture(bool isNewFormat);

	I3_EXPORT_FRAMEWORK		i3Ime				*	getGlobalIme(void);
	I3_EXPORT_FRAMEWORK		void					setGlobalIme(i3Ime*pIme);

	I3_EXPORT_FRAMEWORK		i3UIControl*			CreateControl( i3UITemplate* pTemplate, VEC3D* pPos, VEC2D* pSize, i3GameNode* pParent, const char* pszName = nullptr);
	I3_EXPORT_FRAMEWORK		i3UIControl*			CreateControlByInfo( UI_CONTROL_INFO* pInfo);
	I3_EXPORT_FRAMEWORK		i3UIControl*			ReCreateControl( i3UIControl* pControl, i3GameNode* pParent);
	I3_EXPORT_FRAMEWORK		bool					DeleteControl( i3UIControl* pControl);
	I3_EXPORT_FRAMEWORK		void					FillControlInfo( UI_CONTROL_INFO* pInfo, i3UIControl* pControl);
	I3_EXPORT_FRAMEWORK		i3UIControl*			CreateCloneControl( i3UIControl* pSrc, bool bIncludeChild = true, i3GameNode* pParent = nullptr);

	I3_EXPORT_FRAMEWORK		void					SetUniqueName( i3UIControl* pControl);
	I3_EXPORT_FRAMEWORK		void					FindUniqueName( i3ClassMeta* pMeta, i3UIScene* pScene, char* pBuf, INT32 nBufSize);
	I3_EXPORT_FRAMEWORK		void					SetChildAndParent( i3UIControl* pChild, i3GameNode* pParent);

	I3_EXPORT_FRAMEWORK		bool					IsIndependentControl( i3UIControl* pControl);

	I3_EXPORT_FRAMEWORK		i3UIConsoleClientContext *	GetUIConsoleContext( void);

	I3_EXPORT_FRAMEWORK		bool					DumpFontCache( const char * pszFolder);

	I3_EXPORT_FRAMEWORK		void					DumpSprite( void);
	I3_EXPORT_FRAMEWORK		void					SetCallbackFunction(pfCallback cbFunc);
	I3_EXPORT_FRAMEWORK		void					VirtualTexture_ProgressPos(int nPos);
	I3_EXPORT_FRAMEWORK		void					VirtualTexture_ProgressRange(int nRange);

	I3_EXPORT_FRAMEWORK		void					setTDKPropertyUpdate(bool bUpdate);
	I3_EXPORT_FRAMEWORK		bool					IsTDKPropertyUpdate();
	I3_EXPORT_FRAMEWORK		i3::vector<SERVICECOUNTRY>* getCountryList();
};


#endif
