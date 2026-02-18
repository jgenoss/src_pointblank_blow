#if !defined( __I3_FRAMEWORK_H)
#define __I3_FRAMEWORK_H

#include "i3Stage.h"
#include "i3GameRes.h"
#include "i3GameObj.h"
#include "i3StageStack.h"
#include "i3SgLayer.h"
#include "i3StageInfo.h"
#include "i3InputDispatchManager.h"
#include "i3UI/i3UIRenderer.h"
#include "i3UI/i3UIManager.h"
#include "i3Base/itl/vector_util.h"

class i3Viewer;
class i3EffectManager;
class i3FadeEffect;
class i3ResourceManager;
class i3SceneGraphContext;

enum	i3FadeEffectType;

typedef i3EventReceiver *	i3InputDestination;

class I3_EXPORT_FRAMEWORK i3Framework : public i3GameNode
{
	I3_EXPORT_CLASS_DEFINE( i3Framework, i3GameNode);
public:
	enum STATE
	{
		STATE_NONE = -1,

		STATE_LOGO1_START,
		STATE_LOGO1,
		STATE_LOGO1_END,

		STATE_LOGO2_START,
		STATE_LOGO2,
		STATE_LOGO2_END,

		STATE_LOGO3_START,
		STATE_LOGO3,
		STATE_LOGO3_END,

		STATE_LOGO4_START,
		STATE_LOGO4,
		STATE_LOGO4_END,

		STATE_COMMON_LOAD_START,
		STATE_COMMON_LOADING,
		STATE_COMMON_LOAD_END,

		STATE_LOAD_START,
		STATE_LOADING,
		STATE_LOADING_GAMEDATA,
		STATE_LOAD_END,

		STATE_FADEIN_START,
		STATE_FADEIN,
		//STATE_FADEIN_END,

		STATE_SCENE,
		STATE_SCENE_END,

		STATE_FADEOUT_START,
		STATE_FADEOUT,
		//STATE_FADEOUT_END,
	};

protected:
	// Persistant Members
	i3::vector<i3SgLayer*>	m_LayerList;
	i3::vector<i3StageInfo*> m_StageInfoList;
	i3StageInfo	*		m_pStartStageInfo = nullptr;
	i3::vector<i3::rc_string> m_CmnResList;

	// Volatile Members
	STATE				m_State = STATE::STATE_NONE;
	STATE				m_OldState = STATE::STATE_NONE;
	STATE				m_StateAfterFadeIn = STATE::STATE_NONE;
	STATE				m_StateAfterFadeOut = STATE::STATE_NONE;
	bool				m_bCommonLoadingStarted = false;

	
	REAL32			m_rStateTime = 0.0f;

	REAL32			m_timeLocal = 0.0f;
	UINT32				m_UpdateID = 0;
	i3Viewer *			m_pViewer = nullptr;
	i3ResourceManager *	m_pResManager = nullptr;
	i3EffectManager *	m_pEffectManager = nullptr;

	i3Stage *			m_pStage = nullptr;
	i3StageStack		m_StageStack;

	INT32				m_PrevStageIndex = -1;

	i3SceneGraphContext *	m_pSgContext = nullptr;

	i3FadeEffect		*m_pFadeEffect = nullptr;
	REAL32			m_StateSwitchFadeTime = 1.0f;
	i3Camera *			m_p3DCamera = nullptr;
	i3Camera *			m_pPlayer3DCamera = nullptr;

	i3TStack<i3InputDestination>	m_FocusStack;
	bool							m_bLockFocus = false;
	// MouseMove를 받을 대상. 이것은 Focus와는 다를 수 있다.
	i3EventReceiver *				m_pOnMouseTarget = nullptr;
	i3InputDispatchManager *		m_pInputDispatcher = nullptr;

	i3UIManager	*					m_pUIManager = nullptr;
	i3UIAnimationMgr	*			m_pUIAnimationMgr = nullptr;

	i3::rc_string				m_commonfxlistFileName;
#if defined( I3_DEBUG)
protected:
	bool				m_bNeedToTagObject = false;
	bool				m_bNeedToDumpObject = false;

public:
	void				DumpObjects(void)					{ m_bNeedToTagObject = m_bNeedToDumpObject = true; }
#endif

protected:
	virtual void		InitSceneGraph(void);
	virtual void		InitFadeSceneRoot( i3Node *pParentNode );

	virtual void		BuildSceneStruct(void);

	void				UpdateBound(void);

public:
	i3Framework(void);
	virtual ~i3Framework(void);

	void				SetState( STATE state)		{ m_State = state; }
	STATE				GetState(void)				{ return m_State; }

	REAL32			GetLocalTime(void)			{ return m_timeLocal; }
	REAL32				GetAccTime(void);

	void				SetStateSwitchFadeTime( REAL32 rTime ){ m_StateSwitchFadeTime = rTime; }
	REAL32			GetStateSwitchFadeTime()	{ return m_StateSwitchFadeTime;		}

	bool				IsChangingStage(void)		{ return (m_State != STATE_SCENE); }
	i3Stage *			GetCurrentStage(void)		{ return m_pStage; }
	void				SetCurrentStage( i3Stage * pStage);
	virtual bool		JumpStage( INT32 StageIndex, bool bPush = false );
	void				JumpStage( i3StageInfo * pStageInfo, bool bPush = false)
	{
		INT32 idx = GetStageIndex( pStageInfo);
		I3ASSERT( idx != -1);

		JumpStage( idx, bPush);
	}

	void				SetCommonFxlistFileName( const char * pszFileName);

	void				ReturnStage(void);
	void				SetStageTable( i3ClassMeta ** ppStageTable);
	INT32				GetCurrentStageIndex(void)	{ return *m_StageStack.GetTop();	}
	INT32				GetPrevStageIndex()			{ return m_PrevStageIndex;				}

	i3SceneGraphContext *	getSgContext(void)		{ return m_pSgContext; }

	i3Node *			Get3DRoot(void);
	i3AttrSet *			Get2DRoot(void);
	i3AttrSet *			GetDebugTextRoot(void);

	i3FadeEffect *		GetFadeEffect(void)			{ return m_pFadeEffect;	}

	void				AddLayer( i3SgLayer * pLayer);
	void				RemoveLayer( i3SgLayer * pLayer);
	void				RemoveAllLayer(void);
	INT32				FindLayerByName( const char * pszName);
	i3LayerRootNode *	GetLayerRoot( const char * pszName);
	i3SgLayer *			getLayer( INT32 idx) const	{ return m_LayerList[idx]; }
	INT32				getLayerCount(void)	const	{ return (INT32)m_LayerList.size(); }

	const i3::vector<i3::rc_string>&		getCmnResList(void)	const		{ return m_CmnResList;}
	i3::vector<i3::rc_string>&				getCmnResList(void)				{ return m_CmnResList;}

	void				AddStageInfo( i3StageInfo * pInfo);
	void				RemoveStageInfo( i3StageInfo * pInfo);
	void				RemoveAllStageInfo(void);
	INT32				getStageInfoCount(void)		{ return (INT32)m_StageInfoList.size(); }
	i3StageInfo *		getStageInfo( INT32 idx)	{ return m_StageInfoList[idx]; }
	INT32				GetStageIndex( i3StageInfo * pInfo) const 
	{
		size_t idx = i3::vu::index_of(m_StageInfoList, pInfo); return (idx != m_StageInfoList.size()) ? INT32(idx) : -1;
	}
	INT32				FindStageInfoByName( const char * pszName);

	void				SetStartStageInfo( i3StageInfo * pInfo)	{ m_pStartStageInfo = pInfo;			}	
	i3StageInfo *		GetStartStageInfo( void)				{ return m_pStartStageInfo;				}

	INT32				getCurrentPortal(void);
	void				setCurrentPortal( INT32 id);

	UINT64				getCurrentPortalMask(void);
	void				SetCurrentPortalMask( UINT64 id);

	i3EffectManager *	getEffectManager(void)					{ return m_pEffectManager; }
	i3Viewer *			GetViewer(void)							{ return m_pViewer; }
	i3ResourceManager *	GetResourceManager(void)				{ return m_pResManager; }

	i3Camera *			GetDefaultCamera(void)					{ return m_p3DCamera; }

	void				addLight( i3Light *	pLight);
	void				removeLight( i3Light * pLight);
	void				removeAllLights(void);

	void				StopFadeEffect();
	void				StartFadeEffect( i3FadeEffectType FadeType, REAL32 rFadeTime = 0.5f, REAL32 rDarkenTime = 0.0f);
	bool				OnUpdateFadeEffect( REAL32 fDeltaTime );

	UINT32				getUpdateID( void)						{ return m_UpdateID; }

	////////////////////////
	//Property Reload
	virtual void		ReloadProperty(void);

	////////////////////////
	// Resource
	void				LoadCommonResource(void);
	void				AddResource( i3GameRes * pRes);
	void				RemoveAllResource( bool bReleaseCommon = false);
	virtual i3GameRes *	FindResourceByOnlyName( const char * pszName);
	i3GameRes*			FindResourceByOnlyNameAndClassMeta(const char* pszName, i3::class_meta* pMeta);
	virtual i3GameRes *	FindResourceByPath( const char * pszName);
	virtual i3GameRes *	FindResourceByType( INT32 type);
	INT32				GetResourceCount(void);
	i3GameRes *			GetResource( INT32 idx);
	void				DumpAllResources(void);
	i3ResourceObject *	FindResourceFromManager( const char * pszName, i3ClassMeta * pMeta);
	const char *		FindResourceFullPathByOnlyName( const char * pszName);
	virtual bool		LoadRSC( const char * pszPath, bool bCommonRes = false);
	virtual i3GameRes *	LoadResource( const char * pszPath, INT32 layer, bool bCommonRes = false);
	i3GameRes *		LoadBackgroundRes( const char * pszName, bool bCommonLoad);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual bool	Create( i3Viewer * pViewer, const bool& bLegacy = true);

	void			RenderEffect( i3SceneTracer * pTracer);
	void			ClearEffect();
	void			PostTraceEffect();
	void			PostRenderEffect();


	i3EventReceiver *	getFocus(void)
	{ 
		if( m_FocusStack.GetTop() == nullptr)
			return nullptr;

		return * m_FocusStack.GetTop(); 
	}

	void				SetFocus( i3EventReceiver * pFocus, bool bPush = false);
	void				RemoveFocusFromStack(i3EventReceiver* pFocus);
	void				ResetFocus(void);
	void				SetFocusLock( i3EventReceiver * pTarget);
	void				ReleaseFocusLock(void);

	i3EventReceiver *	getOnMouseTarget(void)									{ return m_pOnMouseTarget; }
	void				setOnMouseTarget( i3EventReceiver * pTarget)			{ m_pOnMouseTarget = pTarget; }

	i3InputDispatchManager *		getInputDispatchManager(void)				{ return m_pInputDispatcher; }
	void							setInputDispatchManager( i3InputDispatchManager * pManager)
	{
		I3_REF_CHANGE( m_pInputDispatcher, pManager);
	}

	void							setUIAnimationMgr(i3UIAnimationMgr* pMgr)
	{
		I3_REF_CHANGE(m_pUIAnimationMgr, pMgr);
	}

	i3UIManager *		getUIManager( void)						{ return m_pUIManager; }
	i3UIAnimationMgr *	getUIAnimationMgr(void)					{ return m_pUIAnimationMgr; }

	virtual void	OnUpdate( REAL32 fDeltaTime) override;
	virtual void	OnUpdatePaused(REAL32 fDeltaTime);

	virtual void	OnCreate(void);
	virtual void	OnInput(void) override;
	virtual i3Node *	OnQueryAttachNode( i3GameRes * pRes);
	virtual	i3Node *	OnPlayerQueryAttachNode( i3GameRes * pRes);	//CHAR_SOLO_RENDER

	virtual void	OnCommonLoadStart( bool bFirst);

	/** \return true이면 로딩 완료. */
	virtual bool	OnCommonLoading( bool bFirst, REAL32 fDeltaTime);
	virtual void	OnCommonLoadEnd( bool bFirst);

	virtual void	OnLoadStart( bool bFirst);
	virtual void	OnLoading( bool bFirst);
	virtual void	OnLoadingGameData( bool bFirst);
	virtual void	OnLoadEnd( bool bFirst);

	virtual void	OnFadeInStart( bool bFirst);
	virtual void	OnFadeIn( bool bFirst);
	virtual void	OnFadeInEnd( bool bFirst);

	virtual void	OnRender(void);
	virtual void	OnScene( REAL32 bDeltaTime, bool bFirst);
	virtual void	OnSceneEnd( bool bFirst);

	virtual void	OnFadeOutStart( bool bFirst);
	virtual void	OnFadeOut( bool bFirst);
	virtual void	OnFadeOutEnd( bool bFirst);

	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	virtual		void	OnChangeScene(void);
	virtual		void	OnChangeBound(void);
	virtual		void	OnChangeCamera(void);
	virtual		void	OnUpdateCameraTransform(void);
	virtual		void	OnUpdateCameraProjection(void);
	virtual		void	OnResetCameraPosition(void);
	virtual		void	OnChangeVideo( i3VideoInfo * pInfo);

	virtual void	OnSize( i3::pack::POINT2D * pSize);

#if defined( I3_WINDOWS)
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonDblClk( UINT32 nFlag, POINT point) override;
	virtual void	OnMButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnMButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point) override;

	virtual void	OnMouseMove( UINT32 nFlag, POINT point) override;
	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point) override;

	virtual	void	OnKeyDown( UINT32 nKey) override;
	virtual void	OnKeyUp( UINT32 nKey) override;

	virtual void	OnActivate( bool bActivate);
	virtual void	OnSetFocus(void);
	virtual void	OnKillFocus(void);
	virtual void	OnExitSizeMove(void);
	virtual bool	OnQueryClose(void);
#endif

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	static i3Framework *		getCurrentFramework(void);
	static void					setCurrentFramework( i3Framework * pFramework);
	static void					SetCapture( i3EventReceiver * pTarget);
	static void					ReleaseCapture(void);
	static void					SetCurrentFocus( i3EventReceiver * pFocus, bool bPush = false);
	static i3EventReceiver *    GetCurrentFocus();
	static void					RegisterLuaFunction(LuaState * pState);

	static i3GameNode *	FindGameNodeByType( i3GameNode * pRoot, i3ClassMeta * pMeta, bool bRecursive);

	////////////////////////////////
	// UI
protected:
	i3UIRenderer *			m_pUIRenderer = nullptr;

protected:
	bool			_CreateUI(const bool& bLegacy = true);
	void			_DestroyUI(void);

public:
	i3UIRenderer *			getUIRenderer(void)						{ return m_pUIRenderer; }

	////////////////////////////////
	// Performance
public:
	void				GetProfileString( char * pszBuf, INT32 len);
	void				GetProfileString(i3::string& inout);

	// DX용 작업을 한다. Thread에서 queue한 것들.
	static void		UpdateDXForThread( void);
	static void		FlushDXForThread( void);
};

I3_EXPORT_FRAMEWORK void i3FrameworkRegisterMetas(void);

#endif
