#if !defined( __I3_FRAMEWORK_H)
#define __I3_FRAMEWORK_H

#include "i3Viewer.h"
#include "i3ResourceManager.h"
#include "i3Stage.h"
#include "i3GameRes.h"
#include "i3GameObj.h"
#include "i3StageStack.h"
#include "i3FadeEffect.h"
#include "i3SgLayer.h"
#include "i3StageInfo.h"

class i3EffectManager;

class I3_EXPORT_FRAMEWORK i3Framework : public i3GameNode
{
	I3_CLASS_DEFINE( i3Framework);
public:
	enum STATE
	{
		STATE_NONE = -1,

		STATE_COMMON_LOAD_START,
		STATE_COMMON_LOADING,
		STATE_COMMON_LOAD_END,

		STATE_LOAD_START,
		STATE_LOADING,
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
	i3List				m_LayerList;
	i3List				m_StageInfoList;
	i3StageInfo	*		m_pStartStageInfo;
	i3StringList		m_CmnResList;

	// Volatile Members
	STATE				m_State;
	STATE				m_OldState;

	RT_REAL32			m_timeLocal;
	i3Viewer *			m_pViewer;
	i3ResourceManager *	m_pResManager;
	i3EffectManager *	m_pEffectManager;

	i3Stage *			m_pStage;
	i3StageStack		m_StageStack;

	INT32				m_PrevStageIndex;

	i3SceneGraphContext *	m_pSgContext;

	i3FadeEffect		*m_pFadeEffect;
	RT_REAL32			m_StateSwitchFadeTime;
	i3Camera *			m_p3DCamera;

#if defined( I3_DEBUG)
protected:

	bool				m_bNeedToTagObject;
	bool				m_bNeedToDumpObject;

public:
	void				DumpObjects(void)					{ m_bNeedToTagObject = m_bNeedToDumpObject = true; }
#endif

protected:
	virtual void		InitSceneGraph(void);
	virtual void		InitFadeSceneRoot( i3Node *pParentNode );

	virtual void		BuildSceneStruct(void);

public:
	i3Framework(void);
	virtual ~i3Framework(void);

	void				SetState( STATE state)		{ m_State = state; }
	STATE				GetState(void)				{ return m_State; }

	RT_REAL32			GetLocalTime(void)			{ return m_timeLocal; }

	void				SetStateSwitchFadeTime( RT_REAL32 rTime ){ m_StateSwitchFadeTime = rTime; }
	RT_REAL32			GetStateSwitchFadeTime()	{ return m_StateSwitchFadeTime;		}

	BOOL				IsChangingStage(void)		{ return (m_State != STATE_SCENE); }
	i3Stage *			GetCurrentStage(void)		{ return m_pStage; }
	void				SetCurrentStage( i3Stage * pStage);
	virtual BOOL		JumpStage( INT32 StageIndex, BOOL bPush = FALSE );
	void				JumpStage( i3StageInfo * pStageInfo, BOOL bPush = FALSE)
	{
		INT32 idx = GetStageIndex( pStageInfo);
		I3ASSERT( idx != -1);

		JumpStage( idx, bPush);
	}


	void				ReturnStage(void);
	void				SetStageTable( i3ClassMeta ** ppStageTable);
	INT32				GetCurrentStageIndex(void)	{ return *m_StageStack.GetTop();	}
	INT32				GetPrevStageIndex()			{ return m_PrevStageIndex;				}

	i3SceneGraphContext *	getSgContext(void)		{ return m_pSgContext; }

	i3Node *			Get3DRoot(void)				{ return m_pSgContext->get3DRoot(); }
	i3AttrSet *			Get2DRoot(void)				{ return m_pSgContext->get2DRoot(); }
	i3FadeEffect *		GetFadeEffect(void)			{ return m_pFadeEffect;	}

	void				AddLayer( i3SgLayer * pLayer);
	void				RemoveLayer( i3SgLayer * pLayer);
	void				RemoveAllLayer(void);
	INT32				FindLayerByName( const char * pszName);
	i3LayerRootNode *	GetLayerRoot( const char * pszName);
	i3SgLayer *			getLayer( INT32 idx)		{ return (i3SgLayer *) m_LayerList.Items[idx]; }
	INT32				getLayerCount(void)			{ return m_LayerList.GetCount(); }

	i3StringList*		getCmnResList(void)			{ return &m_CmnResList;}

	void				AddStageInfo( i3StageInfo * pInfo);
	void				RemoveStageInfo( i3StageInfo * pInfo);
	void				RemoveAllStageInfo(void);
	INT32				getStageInfoCount(void)		{ return m_StageInfoList.GetCount(); }
	i3StageInfo *		getStageInfo( INT32 idx)	{ return (i3StageInfo *) m_StageInfoList.Items[idx]; }
	INT32				GetStageIndex( i3StageInfo * pInfo)		{ return m_StageInfoList.IndexOf( pInfo); }
	INT32				FindStageInfoByName( const char * pszName);

	void				SetStartStageInfo( i3StageInfo * pInfo)	{ m_pStartStageInfo = pInfo;			}	
	i3StageInfo *		GetStartStageInfo( void)				{ return m_pStartStageInfo;				}

	INT32				getCurrentPortal(void)					{ return m_pSgContext->getCurrentPortal(); }
	void				setCurrentPortal( INT32 id)				{ m_pSgContext->setCurrentPortal( id); }
	
	i3EffectManager *	getEffectManager(void)					{ return m_pEffectManager; }
	i3Viewer *			GetViewer(void)							{ return m_pViewer; }
	i3ResourceManager *	GetResourceManager(void)				{ return m_pResManager; }

	i3Camera *			GetDefaultCamera(void)					{ return m_p3DCamera; }

	void				addLight( i3Light *	pLight)				{ m_pSgContext->addLight( pLight); }
	void				removeLight( i3Light * pLight)			{ m_pSgContext->removeLight( pLight); }
	void				removeAllLights(void)					{ m_pSgContext->removeAllLights(); }

	void				StopFadeEffect();
	void				StartFadeEffect( i3FadeEffect::FADE_TYPE FadeType, RT_REAL32 rFadeTime = __RT( 0.5f ), RT_REAL32 rDarkenTime = __RT( 0.0f ) );
	BOOL				OnUpdateFadeEffect( RT_REAL32 fDeltaTime );

	////////////////////////
	//Property Reload
	virtual void		ReloadProperty(void);

	////////////////////////
	// Resource
	void				LoadCommonResource(void);
	void				AddResource( i3GameRes * pRes)						{ m_pResManager->AddResource( pRes); }
	void				RemoveAllResource( BOOL bReleaseCommon = FALSE)		{ m_pResManager->RemoveAllResource( bReleaseCommon); }
	virtual i3GameRes *	FindResourceByOnlyName( const char * pszName)		{ return m_pResManager->FindResourceByOnlyName( pszName); }
	virtual i3GameRes *	FindResourceByPath( const char * pszName)			{ return m_pResManager->FindResourceByPath( pszName); }
	virtual i3GameRes *	FindResourceByType( INT32 type)						{ return m_pResManager->FindResourceByType( type); }
	INT32				GetResourceCount(void)								{ return m_pResManager->GetResourceCount(); }
	i3GameRes *			GetResource( INT32 idx)								{ return m_pResManager->GetResource( idx); }

	virtual BOOL		LoadRSC( const char * pszPath, BOOL bCommonRes = FALSE)
	{
		return m_pResManager->LoadRSC( pszPath, bCommonRes); 
	}

	virtual i3GameRes *	LoadResource( const char * pszPath, INT32 layer, BOOL bCommonRes = FALSE)
	{
		return m_pResManager->LoadResource( pszPath, layer, bCommonRes); 
	}

	BOOL			Create( i3Viewer * pViewer);

	void			RenderEffect( i3SceneTracer * pTracer);

	virtual void	OnUpdate( RT_REAL32 fDeltaTime);
	virtual void	OnUpdatePaused(RT_REAL32 fDeltaTime);

	virtual void	OnCreate(void);
	virtual void	OnInput(void);
	virtual i3Node *	OnQueryAttachNode( i3GameRes * pRes);

	virtual void	OnCommonLoadStart( BOOL bFirst);
	virtual void	OnCommonLoading( BOOL bFirst, REAL32 fDeltaTime);
	virtual void	OnCommonLoadEnd( BOOL bFirst);

	virtual void	OnLoadStart( BOOL bFirst);
	virtual void	OnLoading( BOOL bFirst);
	virtual void	OnLoadEnd( BOOL bFirst);

	virtual void	OnFadeInStart( BOOL bFirst);
	virtual void	OnFadeIn( BOOL bFirst);
	virtual void	OnFadeInEnd( BOOL bFirst);

	virtual void	OnRender(void);
	virtual void	OnScene( RT_REAL32 bDeltaTime, BOOL bFirst);
	virtual void	OnSceneEnd( BOOL bFirst);

	virtual void	OnFadeOutStart( BOOL bFirst);
	virtual void	OnFadeOut( BOOL bFirst);
	virtual void	OnFadeOutEnd( BOOL bFirst);

	virtual		void	OnChangeScene(void);
	virtual		void	OnChangeBound(void);
	virtual		void	OnChangeCamera(void);
	virtual		void	OnUpdateCameraTransform(void);
	virtual		void	OnUpdateCameraProjection(void);
	virtual		void	OnResetCameraPosition(void);
	virtual		void	OnChangeVideo( i3VideoInfo * pInfo);

	virtual void	OnSize( POINT2D * pSize);

#if defined( I3_WINDOWS)
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnMButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnMButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point);

	virtual void	OnMouseMove( UINT32 nFlag, POINT point);
	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point);

	virtual	void	OnKeyDown( UINT32 nKey);
	virtual void	OnKeyUp( UINT32 nKey);

	virtual void	OnActivate( bool bActivate);
	virtual void	OnExitSizeMove(void);
	virtual bool	OnQueryClose(void);
#endif

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	static i3Framework *		getCurrentFramework(void);
	static void					setCurrentFramework( i3Framework * pFramework);
};

I3_EXPORT_FRAMEWORK void i3FrameworkRegisterMetas(void);

#endif
