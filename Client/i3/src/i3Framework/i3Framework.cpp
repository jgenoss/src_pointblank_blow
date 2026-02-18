#include "i3FrameworkPCH.h"
#include "i3Framework.h"
#include "i3GameNode.h"
#include "i3GameRes.h"
#include "i3GameResSceneGraph.h"
#include "i3GameResChara.h"
#include "i3GameResAnimation.h"
#include "i3ResourceManager.h"
#include "i3GameObj.h"
#include "i3Stage.h"
#include "i3Viewer.h"
#include "i3Framework.h"
#include "i3Framework.h"
#include "i3AI.h"
#include "i3FadeEffect.h"
#include "i3EffectManager.h"
#include "i3Game.h"
#include "i3UIFilePathMgr.h"

#include "i3VirtualTextureSystem.h"

#include "i3Base/itl/vector_util.h"
#include "i3Base/i3StringListBuffer.h"

extern I3_EXPORT_BASE REAL32 g_rAccTime;

I3_CLASS_INSTANCE( i3Framework);

i3Framework::i3Framework(void)
{
	i3FrameworkRegisterMetas();

	i3Game::setFramework( this);
		
	SetCommonFxlistFileName( "Scene/_Common/Default.i3FxList");

	setCurrentFramework( this);
}

i3Framework::~i3Framework(void)
{
	_DestroyUI();

	RemoveAllStageInfo();
	RemoveAllLayer();

	m_CmnResList.clear();

	I3_SAFE_RELEASE( m_pUIManager);
	I3_SAFE_RELEASE( m_pInputDispatcher);
	I3_SAFE_RELEASE( m_pFadeEffect );

	I3_SAFE_RELEASE( m_p3DCamera);
	I3_SAFE_RELEASE( m_pPlayer3DCamera);

	I3_SAFE_RELEASE( m_pEffectManager);
	I3_SAFE_RELEASE( m_pResManager);
	I3_SAFE_RELEASE( m_pStage);

	I3_SAFE_RELEASE( m_pSgContext);

	setCurrentFramework(nullptr);
	I3_SAFE_RELEASE(m_pUIAnimationMgr);
}

void i3Framework::InitSceneGraph(void)
{	
	m_pSgContext = i3SceneGraphContext::new_object();

	m_pSgContext->Create();

	InitFadeSceneRoot( m_pSgContext->get2DMainRoot());

//#if !defined( I3_NO_PROFILE)
	m_pSgContext->addDebugLayer();
//#endif

}

// Fade Root
void i3Framework::InitFadeSceneRoot( i3Node *pParentNode )
{
	m_pFadeEffect = i3FadeEffect::new_object();
	m_pFadeEffect->Create( pParentNode );

#if defined( I3_DEBUG) && defined( I3_WINDOWS)
	i3Node *pFadeNode = m_pFadeEffect->GetRootNode();

	pFadeNode->SetName( "FadeRoot");
#endif

}

INT32 i3Framework::getCurrentPortal(void)
{
	return m_pSgContext->getCurrentPortal();
}

void i3Framework::setCurrentPortal( INT32 id)
{
	m_pSgContext->setCurrentPortal( id);
}

UINT64 i3Framework::getCurrentPortalMask(void)
{
	return m_pSgContext->getCurrentPortalMask();
}

void i3Framework::SetCurrentPortalMask( UINT64 id)
{
	m_pSgContext->setCurrentPortalMask( id); 

	i3Stage * pStage = GetCurrentStage();
	if( pStage == nullptr)
		return;

	i3World * pWorld = pStage->getWorld();
	if( pWorld == nullptr)
		return;

	i3WorldSectionTable * pTable = pWorld->getWorldSectionTable();
	if( pTable == nullptr)
		return;

	UINT64 nMask = m_pSgContext->getCurrentPortalMask();
	INT32 start = 0;
	UINT64	maskShadow = 0;
	UINT64	maskVisible = 0;

	if( nMask > 1)	start = 1;	//section 0이 아닌곳에 있다면 0은 무시 한다.

	for( INT32 portal = start; portal < pTable->getCount(); ++portal)
	{
		UINT64 mask = ((UINT64)1) << portal;

		if( nMask & mask)
		{
			i3::pack::WORLD_SECTION_INFO * pSection	= pTable->getWorldSection( portal);

			maskShadow |= pSection->m_ShadowMask;
			maskVisible |= pSection->m_VisibilityMask;
		}
	}

	m_pSgContext->setCurrentPortalMasks( maskShadow, maskVisible);
}

void i3Framework::addLight( i3Light *	pLight)
{
	m_pSgContext->addLight( pLight);
}

void i3Framework::removeLight( i3Light * pLight)
{
	m_pSgContext->removeLight( pLight);
}

void i3Framework::removeAllLights(void)
{
	m_pSgContext->removeAllLights();
}

i3Node * i3Framework::Get3DRoot(void)
{
	return m_pSgContext->get3DRoot();
}

i3AttrSet * i3Framework::Get2DRoot(void)
{
	return m_pSgContext->get2DRoot();
}

i3AttrSet * i3Framework::GetDebugTextRoot(void)
{
	return m_pSgContext->getDebugTextRoot();
}

bool i3Framework::Create( i3Viewer * pViewer, const bool& bLegacy)
{
	I3ASSERT( pViewer != nullptr);

	// Scene Graph
	InitSceneGraph();

	// Viewer
	{
		pViewer->SetFramework( this);

		m_pViewer = pViewer;

		m_pViewer->SetSceneGraphContext( m_pSgContext);

		{
			COLOR col;

			i3Color::Set( &col, (UINT8) 0, 0, 0, 255);
			m_pViewer->GetRenderContext()->SetClearColor( &col);
		}
	}

	// Default 3D Camera
	{
		m_p3DCamera = i3Camera::new_object();
		m_p3DCamera->SetEnable(true);

		m_pSgContext->setCurrentCamera( m_p3DCamera);

	}

	// Resource Manager
	{
		m_pResManager = i3ResourceManager::new_object();
		I3ASSERT( m_pResManager != nullptr);

		m_pResManager->BindFramework( this);

		//i3Pack::BindCallbackUpdateFunc( (CALLBACK_UPDATEFUNC) UpdateDXForThread);

		AddChild( m_pResManager);
	}

	// Effect Manager
	{
		m_pEffectManager = i3EffectManager::new_object();
		I3ASSERT( m_pEffectManager != nullptr);

		m_pEffectManager->BindFramework( this);

		// 기본 초기화
		m_pEffectManager->Create();

		AddChild( m_pEffectManager);
	}

	{
		m_pUIManager = i3UIManager::new_object();

		m_pUIManager->Create( this);
		m_pUIManager->SetViewer( pViewer);

		AddChild( m_pUIManager);

		setInputDispatchManager( m_pUIManager);
	}

	_CreateUI(bLegacy);

	BuildSceneStruct();

	OnCreate();

	//animation 파일로드
	{
		i3XMLFile File;
		i3::rc_string strCommonUIFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);

		i3::stack_string strFullPath;
		i3::sprintf(strFullPath, "%s\\VirtualTexture\\filelist.i3Animation", strCommonUIFolder);

		if (i3System::IsFile(strFullPath.c_str()))
		{
			if (File.Load(strFullPath.c_str()) == false)
			{
				I3PRINTLOG(I3LOG_NOTICE, "2DAnimation file load failed");
			}
			else
			{
				m_pUIAnimationMgr = (i3UIAnimationMgr*)File.getKeyObject();
				I3_SAFE_ADDREF(m_pUIAnimationMgr);
			}
		}

	}
	return true;
}

void	i3Framework::AddLayer( i3SgLayer * pLayer)
{
	I3_MUST_ADDREF( pLayer);
	m_LayerList.push_back( pLayer);
}

void	i3Framework::RemoveLayer( i3SgLayer * pLayer)
{
	size_t idx = i3::vu::index_of(m_LayerList, pLayer);
		//m_LayerList.IndexOf( pLayer);

	I3ASSERT( idx != m_LayerList.size() );

	pLayer = getLayer( idx);

	I3_MUST_RELEASE( pLayer);
	m_LayerList.erase( m_LayerList.begin() + idx);
}

void	i3Framework::RemoveAllLayer(void)
{
	INT32 i;
	i3SgLayer * pLayer;

	for( i = 0; i < getLayerCount(); i++)
	{
		pLayer = getLayer( i);
		I3_MUST_RELEASE( pLayer);
	}

	m_LayerList.clear();
}

INT32	i3Framework::FindLayerByName( const char * pszName)
{
	INT32 i;
	i3SgLayer * pLayer;

	for( i = 0; i < getLayerCount(); i++)
	{
		pLayer = getLayer( i);

		if( i3::generic_is_iequal( pLayer->GetNameString(), pszName) )
			return i;
	}

	return -1;
}

i3LayerRootNode * i3Framework::GetLayerRoot( const char * pszName)
{
	INT32 idx;
	i3SgLayer * pLayer;

	idx = FindLayerByName( pszName);
	I3ASSERT( idx != -1);

	pLayer = getLayer( idx);

	return pLayer->getLayerRoot();
}

void i3Framework::BuildSceneStruct(void)
{
	INT32 i;
	i3SgLayer * pLayer;

	for( i = 0; i < getLayerCount(); i++)
	{
		pLayer = getLayer( i);

		if( pLayer->getStyle() & I3_SG_LAYER_STYLE_2D)
		{
			m_pSgContext->get2DRoot()->AddChild( pLayer->getLayerRoot());
		}
		else
		{
			m_pSgContext->get3DRoot()->AddChild( pLayer->getLayerRoot());
		}
	}
}

void	i3Framework::AddStageInfo( i3StageInfo * pInfo)
{
	I3_MUST_ADDREF( pInfo);
	m_StageInfoList.push_back( pInfo);
}

void	i3Framework::RemoveStageInfo( i3StageInfo * pInfo)
{
	size_t idx = i3::vu::index_of(m_StageInfoList, pInfo);

	I3ASSERT( idx != m_StageInfoList.size() );

	I3_MUST_RELEASE( pInfo);
	m_StageInfoList.erase( m_StageInfoList.begin() + idx);
}

void	i3Framework::RemoveAllStageInfo(void)
{
	INT32 i;
	i3StageInfo * pInfo;

	for( i = 0; i < getStageInfoCount(); i++)
	{
		pInfo = getStageInfo( i);

		I3_MUST_RELEASE( pInfo);
	}

	m_StageInfoList.clear();
}

INT32	i3Framework::FindStageInfoByName( const char * pszName)
{
	INT32 i;
	i3StageInfo * pInfo;

	for( i = 0; i < getStageInfoCount(); i++)
	{
		pInfo = getStageInfo( i);

		if( i3::generic_is_iequal( pInfo->GetNameString(), pszName) )
		{
			return i;
		}
	}

	return -1;
}

void i3Framework::SetCurrentStage( i3Stage * pStage)
{
	if( pStage != nullptr)
	{
		I3_MUST_ADDREF( pStage);

		pStage->SetFramework( this);

		pStage->addGNodeState( I3_GAMENODE_STATE_DISABLE);

		AddChildToHead( pStage);
	}

	if( m_pStage != nullptr)
	{
		RemoveChild( m_pStage);
		I3_MUST_RELEASE( m_pStage);
	}

	m_pStage = pStage;
}

bool i3Framework::JumpStage( INT32 StageIndex, bool bPush)
{
	if( IsChangingStage() && (m_OldState != STATE_NONE) )
		return false;

	if( m_StageStack.IsEmpty() )
	{
		m_PrevStageIndex = -1;
	}
	else
	{
		m_PrevStageIndex = GetCurrentStageIndex();
	}

	if( m_pStage == nullptr)
		m_State = STATE_LOAD_START;
	else
		m_State = STATE_SCENE_END;

	if( bPush == true)
		m_StageStack.Push( StageIndex);
	else
		m_StageStack.SetTop( StageIndex);

	return true;
}

void i3Framework::ReturnStage(void)
{
	if( m_StageStack.IsEmpty() )
	{
		m_PrevStageIndex = -1;
	}
	else
	{
		m_PrevStageIndex = GetCurrentStageIndex();
	}

	if( m_pStage == nullptr)
		m_State = STATE_LOAD_START;
	else
		m_State = STATE_SCENE_END;

	I3ASSERT( m_StageStack.IsEmpty() == false);

	m_StageStack.Pop(nullptr);
}

void i3Framework::RenderEffect( i3SceneTracer * pTracer)
{
	m_pEffectManager->RenderEffect( pTracer);
}

void	i3Framework::ClearEffect()
{
	m_pEffectManager->ClearEffect();
}

void	i3Framework::PostTraceEffect()
{
	m_pEffectManager->PostTraceEffect();
}

void	i3Framework::PostRenderEffect()
{
	m_pEffectManager->PostRenderEffect();
}

void i3Framework::SetFocus( i3EventReceiver * pFocus, bool bPush)
{
	if( getFocus() == pFocus)
		return;
	
#if 0
	if( pFocus != nullptr)
		I3TRACE("i3Framework::SetFocus   %s\n", pFocus->meta()->class_name());
	else
		I3TRACE("i3Framework::SetFocus    nullptr\n");
#endif
	
	i3EventReceiver* prevFocus = getFocus();
/*
	if( m_pInputDispatcher != nullptr)
	{
		m_pInputDispatcher->OnChangeFocus( this, getFocus(), pFocus);
	}
*/
	if( pFocus != nullptr)
	{
		const INT32 num_stack = m_FocusStack.GetCount();
		i3EventReceiver** buff = m_FocusStack.GetBuffer();

		for (INT32 i = 1; i < num_stack ; ++i)			// 1부터 조사한다..
		{
			INT32 idx = num_stack - i - 1;
			if (buff[idx] == pFocus)
			{
				i3TStack<i3InputDestination>	TempFocusStack;
				const INT32 numRem = i;

				for (INT32 j = 0 ; j < numRem ; ++j)
				{
					TempFocusStack.Push(getFocus());
					m_FocusStack.Pop(nullptr);
				}
				m_FocusStack.Pop(nullptr);

				for (INT32 j = 0; j < numRem ; ++j)
				{
					m_FocusStack.Push( *TempFocusStack.GetTop() );
					TempFocusStack.Pop(nullptr);
				}
				break;
			}
		}
			
		if (bPush || m_FocusStack.GetCount() <= 0)
			m_FocusStack.Push( pFocus);
		else
			m_FocusStack.SetTop( pFocus);
	}
	else
	{
		if( m_FocusStack.GetCount() > 0)
		{
			m_FocusStack.Pop(nullptr);
		}
	}

	i3EventReceiver* nextFocus = getFocus();

	if( m_pInputDispatcher != nullptr)
	{
		m_pInputDispatcher->OnChangeFocus( this, prevFocus, nextFocus);
		
#if 0
		I3TRACE("m_pInputDispatcher->OnChangeFocus From: %s To: %s\n", (prevFocus) ? prevFocus->meta()->class_name() : "nullptr",
			(nextFocus) ? nextFocus->meta()->class_name() : "nullptr");
#endif
			
	}

}

void i3Framework::RemoveFocusFromStack(i3EventReceiver* pTarget)
{
	INT32 num = m_FocusStack.GetCount();
	if (num == 0) return;
	
	i3EventReceiver* prevFocus = getFocus();
	if (prevFocus == pTarget)	// 포커스가 바뀜..
	{
		m_FocusStack.Pop(nullptr);
		i3EventReceiver* nextFocus = getFocus();
		m_pInputDispatcher->OnChangeFocus(this, prevFocus, nextFocus);	

		if( nextFocus != nullptr)
			I3TRACE("i3Framework::RemoveFocusFromStack Result: %s NUM:%d \n", nextFocus->meta()->class_name(), m_FocusStack.GetCount());
		else
			I3TRACE("i3Framework::RemoveFocusFromStack Result: nullptr NUM:%d \n", m_FocusStack.GetCount());

		return;
	}

	i3EventReceiver** buff = m_FocusStack.GetBuffer();

	for (INT32 i = 1; i < num ; ++i)			// 1부터 조사한다..
	{
		INT32 idx = num - i - 1;
		
		if (buff[idx] == pTarget)
		{
			// 중간에 존재하므로 포커스가 바뀌지 않음.
			i3TStack<i3InputDestination>	TempFocusStack;
			const INT32 numRem = i;

			for (INT32 j = 0 ; j < numRem ; ++j)
			{
				TempFocusStack.Push(getFocus());
				m_FocusStack.Pop(nullptr);
			}
			m_FocusStack.Pop(nullptr);
			
			for (INT32 j = 0; j < numRem ; ++j)
			{
				m_FocusStack.Push( *TempFocusStack.GetTop() );
				TempFocusStack.Pop(nullptr);
			}
			break;
		}
	}
		
}


void i3Framework::SetFocusLock( i3EventReceiver * pTarget)
{
	SetFocus( pTarget, true);

	m_bLockFocus = true;
}

void i3Framework::ReleaseFocusLock(void)
{
	SetFocus(nullptr);

	m_bLockFocus = false;
}

void i3Framework::ResetFocus(void)
{
	m_FocusStack.Reset();
}

//#define DEBUG_STATE

#if defined( DEBUG_STATE)
static char s_StateStr[][64] =
{
	"STATE_LOAD_LOAD_START",
	"STATE_LOAD_LOADING",
	"STATE_LOAD_LOAD_END",

	"STATE_COMMON_LOAD_START",
	"STATE_COMMON_LOADING",
	"STATE_COMMON_LOAD_END",

	"STATE_LOAD_START",
	"STATE_LOADING",
	"STATE_LOADING_GAMEDATA",
	"STATE_LOAD_END",

	"STATE_FADEIN_START",
	"STATE_FADEIN",
	"STATE_FADEIN_END",
	"STATE_SCENE",
	"STATE_SCENE_END",
	"STATE_FADEOUT_START",
	"STATE_FADEOUT",
	"STATE_FADEOUT_END",
};
#endif

void i3Framework::OnUpdatePaused(REAL32 fDeltaTime)
{
	
}


void i3Framework::OnUpdate( REAL32 fDeltaTime)
{
	bool bFirst = false;
	bool bOnUpdate = true;

	m_UpdateID++;

	g_rAccTime = g_rAccTime + fDeltaTime; //시간 누적

	if( m_State != m_OldState)
	{
		bFirst = true;
		m_OldState = m_State;

		if( m_State == STATE_LOAD_END || m_State == STATE_FADEIN_START || m_State == STATE_FADEIN )
		{
			fDeltaTime = 0.0f;
		}
	}

	#if defined( DEBUG_STATE)
		I3TRACE1( "%s\n", s_StateStr[ m_State]);
	#endif

	if( m_pFadeEffect != nullptr )
	{
		m_pFadeEffect->TreatPendingDisable();
		OnUpdateFadeEffect( fDeltaTime );
	}

	switch( m_State)
	{
		case STATE_COMMON_LOAD_START :

			m_bCommonLoadingStarted = true;
			// 이 시점에서 Loading되는 모든 Resource들은 무조건 Common Res.로 취급.
			m_pResManager->setCommonResourceState( true);

			if( m_pViewer != nullptr)
			{
	#if defined( I3_PHYSX)
				m_pViewer->GetPhysixContext()->setEnableSimulate(false);
	#endif

				if( m_pViewer->GetSoundContext())
				{
					m_pViewer->GetSoundContext()->SetPendingEnable( true);
				}
			}
			OnCommonLoadStart(true);

			m_State = STATE_COMMON_LOADING;
			break;

		case STATE_COMMON_LOADING :
			if( OnCommonLoading( bFirst, fDeltaTime) )
			{
				if( m_pResManager->GetLoadingProgress() >= 100)
				{
					// Stage가 Ttrue를 반환한 것은 필요한 Resource를 완전히
					// Loading하였다는 의미이기 때문에, 다음 상태로 넘긴다.
					m_State = STATE_COMMON_LOAD_END;
				}
			}
			break;

		case STATE_COMMON_LOAD_END :

			OnCommonLoadEnd( bFirst);

			if( m_pViewer != nullptr && m_pViewer->GetSoundContext())
			{
				m_pViewer->GetSoundContext()->SetPendingEnable( false);
			}
			m_pResManager->setCommonResourceState( false);

			if( m_pStartStageInfo != nullptr)
			{
				m_State = STATE_SCENE;
				JumpStage( m_pStartStageInfo, true);
			}
			break;

			// 로딩 시작
		case STATE_LOAD_START :
			{
				if( getStageInfoCount() > 0)
				{
					if( m_StageStack.IsEmpty() == false)
					{
						// 새로운 Stage를 위한 Class 생성
						INT32 newStage = *m_StageStack.GetTop();
						I3ASSERT( newStage < getStageInfoCount());

						i3StageInfo * pStageInfo = getStageInfo( newStage); 
						i3ClassMeta * pStageMeta = i3ClassMetaSystem::i()->find_class_meta(pStageInfo->getStageClassName()); 
													//::FindClassMetaByName( (const char *) pStageInfo->getStageClassName());
						
						if( pStageMeta == nullptr)
						{							
							char msg[256] = "";
							char szClassName[256] = "(null)";

							if( i3::generic_string_size( pStageInfo->getStageClassName()) > 0)
							{
								i3::safe_string_copy( szClassName, pStageInfo->getStageClassName(), 256);								
							} 

							i3::snprintf( msg, 256, "%s Stage Class를 생성할 수가 없습니다. RegisterMeta를 해야합니다.\r\n.i3Game 파일에 해당 Stage를 정상적으로 설정했는지 확인하시오.", szClassName);

							I3PRINTLOG(I3LOG_FATAL,  msg);
							I3ASSERT_0;
						}

						{
							i3Stage * pNewStage;

							I3ASSERT( pStageMeta != nullptr);

							pNewStage = (i3Stage *) pStageMeta->create_instance(); //  CREATEINSTANCE( pStageMeta);
							I3ASSERT( pNewStage != nullptr);

							SetCurrentStage( pNewStage);
						}
					}

					m_pStage->OnQueryLoad( *m_StageStack.GetTop());

					OnLoadStart( bFirst);

					StopFadeEffect();
					m_State = STATE_LOADING;
				}
			}
			break;

			// 로딩 중
		case STATE_LOADING :
			{
				if( m_pStage != nullptr)
				{
					if( m_pResManager->GetLoadingProgress() >= 100 )
					{
						// Stage에 추가로 Loading할 Resoruce가 있는지 계속 호출하여
						// Query한다.
						//
						// Stage가 true를 반환한 것은 필요한 Resource를 완전히
						// Loading하였다는 의미이기 때문에, 다음 상태로 넘긴다.
						m_pStage->OnCreate();
						m_State = STATE_LOADING_GAMEDATA;
					}

					m_pStage->OnLoading();
				}
				else
				{
					m_State = STATE_LOADING_GAMEDATA;
				}

				OnLoading( bFirst);
			}
			break;

		case STATE_LOADING_GAMEDATA:
			{
				if(m_pStage != nullptr)
				{
					if( m_pStage->IsLoadCompleteGameData() )
						m_State = STATE_LOAD_END;
					else
						m_pStage->OnLoadingGameData();
				}
				else
					m_State = STATE_LOAD_END;

				OnLoadingGameData( bFirst);
			}
			break;

			// 로딩 종료
		case STATE_LOAD_END :
			{
				I3PRINTLOG(I3LOG_NOTICE,  "i3Framework::OnUpdate() - STATE_LOAD_END Enter");

				#if defined( I3_DEBUG)
				i3ElementBase::setMemTagValue( 10);
				#endif

				// Loading이 완료된 순간에 1번만 호출한다.
				if( m_pStage != nullptr)
				{
					m_pStage->OnLoadEnd();
				}

				OnLoadEnd( bFirst);

				m_pResManager->RemoveAllLoadRequest();

				// 여기서 Fade 효과를 시작하지 않으면, 다음 상태로 가기전에 한 프레임이 비게 된다. -_-;
				StartFadeEffect( I3_FADE_EFFECT_FADE_IN, m_StateSwitchFadeTime );
				
				GetViewer()->ResetTime();

				m_State = STATE_FADEIN_START;
				m_StateAfterFadeIn = STATE_SCENE;

				UpdateBound();

				I3PRINTLOG(I3LOG_NOTICE,  "i3Framework::OnUpdate() - STATE_LOAD_END Leave");
			}
			break;

			// Fade In 시작
		case STATE_FADEIN_START :
			{
				// STATE_LOAD_END에서 호출 하므로 여기서 하지 않아도 된다.
				// 꼭! 넣어 줘야 합니다. 호출을 안하면 프레임이 떨어지는 폰에서는 FadeIn이 작동안합니다. <- KOMET
				StartFadeEffect( I3_FADE_EFFECT_FADE_IN, m_StateSwitchFadeTime );

				m_State = STATE_FADEIN;

				if( GetViewer()->GetSoundContext())
				{
					GetViewer()->GetSoundContext()->StopAllSound(true);
				}

				GetViewer()->ResetTime();

				I3TRACE( "***************************************************************************\n");
				I3TRACE( " Scene Start\n");
				I3TRACE( "***************************************************************************\n");

				OnFadeInStart( bFirst);
			}
			break;

			// Fade In 진행중/완료시점
		case STATE_FADEIN :
			{
				// Fade In이 시작될때는 Stage를 업데이트합니다.
				if( m_pStage != nullptr)
				{
					if( (m_pStage->getGNodeState() & I3_GAMENODE_STATE_DISABLE) != 0)
					{
						m_pStage->removeGNodeState( I3_GAMENODE_STATE_DISABLE);
					}
				}

				if( OnUpdateFadeEffect( fDeltaTime ) )
				{
					// Fade 효과가 진행중.
					OnFadeIn( bFirst);	
				}
				else
				{
					// Fade 효과가 끝나는 순간 한번 호출 되는 부분
					m_State = m_StateAfterFadeIn;

					#if defined( I3_PHYSX)
						GetViewer()->GetPhysixContext()->setEnableSimulate(true);
					#endif

					// 

//					if( m_pStage != nullptr)
//					{
//						m_pStage->removeGNodeState( I3_GAMENODE_STATE_DISABLE);
//					}

					OnFadeInEnd( bFirst);
				}
			}
			break;

			// Scene 진행중
		case STATE_SCENE :
			{
				OnScene( fDeltaTime, bFirst);
			}
			break;

			// Scene 종료
		case STATE_SCENE_END :
			{
				if( m_pStage != nullptr)
				{
					m_pStage->OnFinish();
				}

				OnSceneEnd( bFirst);

				#if defined( I3_PHYSX)
					GetViewer()->GetPhysixContext()->setEnableSimulate(false);
				#endif

				m_State = STATE_FADEOUT_START;
				m_StateAfterFadeOut = STATE_LOAD_START;

				bOnUpdate = false;
			}
			break;

			// Fade Out 시작
		case STATE_FADEOUT_START :
			{
				StartFadeEffect( I3_FADE_EFFECT_FADE_OUT, m_StateSwitchFadeTime );

				m_State = STATE_FADEOUT;

				OnFadeOutStart( bFirst);

				bOnUpdate = false;
			}
			break;

			// Fade Out 진행중/완료시점
		case STATE_FADEOUT :
			{
				// Fade Out이 시작될때는 Stage를 업데이트 하지 않습니다.
				if( m_pStage != nullptr)
				{
					if( (m_pStage->getGNodeState() & I3_GAMENODE_STATE_DISABLE) == 0)
					{
						m_pStage->addGNodeState( I3_GAMENODE_STATE_DISABLE);
					}
				}

				if( OnUpdateFadeEffect( fDeltaTime ) )
				{
					// Fade 효과가 진행 중.
					OnFadeOut( bFirst);
				}
				else
				{
					// Fade 효과가 끝나는 순간 한번 호출 되는 부분
					OnFadeOutEnd( bFirst);

					if( GetViewer()->GetSoundContext())
					{
						GetViewer()->GetSoundContext()->StopAllSound();
					}

					m_pEffectManager->LeaveStage();
					m_pEffectManager->Reset();

					SetCurrentStage(nullptr);

					RemoveAllResource();

					m_pSgContext->reset();

					GetViewer()->GetPhysixContext()->ReleaseScene();
					GetViewer()->GetPhysixContext()->reset();

					GetViewer()->GetRenderContext()->FlushVideoMemory();

					i3mem::Compact();

					#if defined( I3_DEBUG)
					if( m_bNeedToDumpObject == true)
					{
						i3ElementBase::DumpObjects( I3ELEMENT_DUMP_ALL, nullptr, 10);
						m_bNeedToDumpObject = false;
					}

					i3ElementBase::SnapMemoryTag( 0);
					#endif
					//i3mem::Dump();
					//i3mem::DumpEach(100); 

					//i3GfxResource::Dump();
#if defined( I3_DEBUG) && defined( I3_MEM_NET_DUMP)
					MEM_NET_ALLOC_INFO SendBuffer; 
					SendBuffer._Command	= MEMDUMP_CMD_PRINT;
					DumpWriteFileBuffer( &SendBuffer );
#endif

					m_State = m_StateAfterFadeOut;
				}

				bOnUpdate = false;
			}
			break;

		default :
			break;
	}

	if( bOnUpdate)
		i3GameNode::OnUpdate( fDeltaTime);

	// Thread에 영향을 받는 리소스를 업데이트
	UpdateDXForThread();
}

void i3Framework::StopFadeEffect()
{
	if( m_pFadeEffect != nullptr )
	{
		m_pFadeEffect->SetEnable(false);
	}
}

// Fade 효과 시작하기
// FAD_IN, FADE_OUT 에서는 rFadeTime이 전체 이펙트 시간과 동일
// FADE_OUT_IN에서는 rFadeTime * 2 + rDarkenTime 만큼의 시간이 소요.
// rDarkenTime는 검은화면 지속 시간. FADE_OUT, FADE_OUT_IN에만 적용.
void i3Framework::StartFadeEffect( i3FadeEffectType FadeType, REAL32 rFadeTime, REAL32 rDarkenTime )
{
	if( m_pFadeEffect != nullptr )
	{
		m_pFadeEffect->Start( FadeType, rFadeTime, rDarkenTime );
	}
}

bool i3Framework::OnUpdateFadeEffect( REAL32 fDeltaTime )
{
	if( m_pFadeEffect != nullptr )
	{
		return m_pFadeEffect->OnUpdate( fDeltaTime );
	}
	return false;
}

void i3Framework::OnCreate(void)
{
#if defined( I3_WINDOWS)
	// Multi-Processor System에서 소수의 BIOS에서 Bug로 인해 QueryPerformanceCounter 함수가 올바르지
	// 않은 값을 반환하는 경우가 있다.
	// 그것을 방지하기 위한 코드 추가

	//::SetThreadAffinityMask( GetCurrentThread(), 1);
#endif
}

void i3Framework::LoadCommonResource(void)
{
	
	for(size_t i = 0; i < m_CmnResList.size(); ++i)
	{
		const char* pszRSCPath = m_CmnResList[i].c_str();

		I3ASSERT( pszRSCPath != nullptr);

		if( LoadRSC( pszRSCPath, true) == false)
		{
			#if !defined( I3_NO_PROFILE)
				I3TRACE1( "Failed Load Common ResFile: %s\n", pszRSCPath);
			#endif
		}
	}
}

void i3Framework::AddResource( i3GameRes * pRes)
{
	m_pResManager->AddResource( pRes);
}

void i3Framework::RemoveAllResource( bool bReleaseCommon)
{
	m_pResManager->RemoveAllResource( bReleaseCommon);
}

i3GameRes *	i3Framework::FindResourceByOnlyName( const char * pszName)
{
	return m_pResManager->FindResourceByOnlyName( pszName);
}

i3GameRes*	i3Framework::FindResourceByOnlyNameAndClassMeta(const char* pszName, i3::class_meta* pMeta)
{
	return m_pResManager->FindResourceByOnlyNameAndClassMeta(pszName, pMeta);
}

i3GameRes *	i3Framework::FindResourceByPath( const char * pszName)
{
	return m_pResManager->FindResourceByPath( pszName);
}

i3GameRes *	i3Framework::FindResourceByType( INT32 type)
{
	return m_pResManager->FindResourceByType( type);
}

INT32 i3Framework::GetResourceCount(void)
{
	return m_pResManager->GetResourceCount();
}

i3GameRes * i3Framework::GetResource( INT32 idx)
{
	return m_pResManager->GetResource( idx);
}

void i3Framework::DumpAllResources(void)
{
	return m_pResManager->DumpAllResources();
}

i3ResourceObject * i3Framework::FindResourceFromManager( const char * pszName, i3ClassMeta * pMeta)
{
	return m_pResManager->FindResourceFromManager( pszName, pMeta);
}

const char * i3Framework::FindResourceFullPathByOnlyName( const char * pszName)
{
	return m_pResManager->FindFullPathByOnlyName( pszName); 
}

bool i3Framework::LoadRSC( const char * pszPath, bool bCommonRes)
{
	return m_pResManager->LoadRSC( pszPath, bCommonRes); 
}

i3GameRes * i3Framework::LoadResource( const char * pszPath, INT32 layer, bool bCommonRes)
{
	return m_pResManager->LoadResource( pszPath, layer, bCommonRes, false); 
}

i3GameRes * i3Framework::LoadBackgroundRes( const char * pszName, bool bCommonLoad)
{
	return m_pResManager->LoadBackgroundRes( pszName, bCommonLoad);
}

void i3Framework::OnCommonLoadStart( bool bFirst)
{
	LoadCommonResource();
}

bool i3Framework::OnCommonLoading( bool bFirst, REAL32 fDeltaTime)
{
	return true;
}

void i3Framework::SetCommonFxlistFileName( const char * pszFileName)
{
	m_commonfxlistFileName = pszFileName;
}

void i3Framework::OnCommonLoadEnd( bool bFirst)
{
	// Effect Manager의 초기화
	{
		// Script/Effect/Default.i3FxList 파일을 찾아, 존재한다면 Loading한다.
		if( i3FileStream::isFile( m_commonfxlistFileName.c_str()))
		{
			m_pEffectManager->Load( m_commonfxlistFileName.c_str(), true);
		}
	}
}

void i3Framework::OnLoadStart( bool bFirst)
{
}

void i3Framework::OnLoading( bool bFirst)
{
}

void i3Framework::OnLoadingGameData( bool bFirst)
{
}

void i3Framework::OnLoadEnd( bool bFirst)
{
}

void i3Framework::OnFadeInStart( bool bFirst)
{
}

void i3Framework::OnFadeIn( bool bFirst)
{
}

void i3Framework::OnFadeInEnd( bool bFirst)
{
}

void i3Framework::OnRender(void)
{
}

void i3Framework::OnScene( REAL32 fDeltaTime, bool bFirst)
{
}

void i3Framework::OnSceneEnd( bool bFirst)
{
}

void i3Framework::OnFadeOutStart( bool bFirst)
{
}

void i3Framework::OnFadeOut( bool bFirst)
{
}

void i3Framework::OnFadeOutEnd( bool bFirst)
{
}

i3Node * i3Framework::OnQueryAttachNode( i3GameRes * pRes)
{
	i3Node * pRoot;

	if( GetCurrentStage() != nullptr)
	{
		pRoot = GetCurrentStage()->OnQueryAttachNode( pRes);

		if( pRoot != nullptr)
			return pRoot;
	}

	return m_pSgContext->get3DRoot();
}

i3Node * i3Framework::OnPlayerQueryAttachNode( i3GameRes * pRes)
{
	return nullptr;
}

void i3Framework::ReloadProperty(void)
{
	i3GameNode::OnReloadProperty();
}

void i3Framework::OnChangeScene(void)
{
}

void i3Framework::OnChangeBound(void)
{
}

void i3Framework::OnChangeCamera(void)
{
}

void i3Framework::OnResetCameraPosition(void)
{
}

void i3Framework::OnUpdateCameraTransform(void)
{
}

void i3Framework::OnUpdateCameraProjection(void)
{
}

void i3Framework::OnSize( i3::pack::POINT2D * pSize)
{
	if( m_pUIManager != nullptr)
	{
		m_pUIManager->OnSize();
	}

	if( m_pUIRenderer != nullptr)
	{
		m_pUIRenderer->OnSize(pSize->x, pSize->y);
	}
}

void i3Framework::OnInput(void)
{
	
#if !defined( I3_NO_PROFILE)
	i3InputController * pCtrl;
	i3InputDeviceManager * pInputManager;
#endif

	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchInput() == false)
			return;
	}

	if( getFocus() == nullptr)
	{
		if( m_pStage != nullptr)
		{
			m_pStage->OnInput();
		}
	}
	else
	{
		getFocus()->OnInput();
	}

#if !defined( I3_NO_PROFILE)

#if defined( I3_WINDOWS)
	i3InputKeyboard * pKB;
#endif

	pInputManager = m_pViewer->GetInputDeviceManager();

#if defined( I3_WINDOWS)
	if( pInputManager->GetKeyboard() != nullptr)
	{
		pKB = pInputManager->GetKeyboard();

		#if defined( I3_WIPI)
		if( pKB->GetStrokeState( I3I_KEY_1))
		{
			m_pViewer->ReportPerformance();
			REPORT_PROFILE();
		}
		#endif

		if( pKB->GetPressState( I3I_KEY_LCTRL))
		{
			if( pKB->GetStrokeState( I3I_KEY_F1))
			{
				if( pKB->GetPressState( I3I_KEY_LSHIFT))
				{
					if( m_pViewer->getProfileMode() == i3Viewer::PROFILE_RENDER)
						m_pViewer->setProfileMode( i3Viewer::PROFILE_NONE);
					else
						m_pViewer->setProfileMode( i3Viewer::PROFILE_RENDER);
				}
				else
					m_pViewer->ReportPerformance();
			}

			if( pKB->GetStrokeState( I3I_KEY_F2))
			{
				if( pKB->GetPressState( I3I_KEY_LSHIFT))
				{
					if( m_pViewer->getProfileMode() == i3Viewer::PROFILE_FRAMEWORK)
						m_pViewer->setProfileMode( i3Viewer::PROFILE_NONE);
					else
						m_pViewer->setProfileMode( i3Viewer::PROFILE_FRAMEWORK);
				}
				else
				{
					REPORT_PROFILE();
				}
			}

			if( pKB->GetStrokeState( I3I_KEY_F3))
			{
				if( pKB->GetPressState( I3I_KEY_LSHIFT))
				{
					if( m_pViewer->getProfileMode() == i3Viewer::PROFILE_PHYSIX)
						m_pViewer->setProfileMode( i3Viewer::PROFILE_NONE);
					else
						m_pViewer->setProfileMode( i3Viewer::PROFILE_PHYSIX);
				}
				else
				{
					I3TRACE( "MEMORY STATE : %d bytes (%d blocks)\n", i3mem::GetAllocSize(), i3mem::GetAllocCount());

					i3MemDump(I3MEMORY_DEBUG_EACH | I3MEMORY_DEBUG_FILE);
				}
			}

			if( pKB->GetStrokeState( I3I_KEY_F4))
			{
				if( pKB->GetPressState( I3I_KEY_LSHIFT))
				{
					if( m_pViewer->getProfileMode() == i3Viewer::PROFILE_CUSTOM)
						m_pViewer->setProfileMode( i3Viewer::PROFILE_NONE);
					else
						m_pViewer->setProfileMode( i3Viewer::PROFILE_CUSTOM);
				}
			}

			if( pKB->GetStrokeState( I3I_KEY_F5))
			{
				if( pKB->GetPressState( I3I_KEY_LSHIFT))
				{
					DumpAllResources();
				}
			}

			#if defined( I3_DEBUG)

			//if( pKB->GetStrokeState( I3I_KEY_F6))
//			{
	//			if( pKB->GetPressState( I3I_KEY_LSHIFT))
		//		{
			//		this->GetViewer()->GetRenderContext()->DumpDrawPrimitiveList();
				//}
//			}

			if( pKB->GetPressState( I3I_KEY_LSHIFT))
			{
				if( pKB->GetStrokeState( I3I_KEY_V))
				{
					m_pUIRenderer->SaveVRAMCache( "D:\\Dump\\VTex.png");
					m_pUIRenderer->DumpVirtualTexture();
				}
				
				if( pKB->GetStrokeState( I3I_KEY_F))
				{
					m_pUIRenderer->SaveFontCache( "C:\\Dump\\Font.png");
				}

				if( pKB->GetStrokeState( I3I_KEY_D))
				{
					i3SceneFile file;

					file.SetSceneGraph( m_pSgContext->get3DMainRoot());

					file.Save( "D:\\Dump\\3DMainRoot.i3s");
				}
			}
			#endif
		}
	}
#endif

	if( pInputManager->GetControllerCount() > 0)
	{
		pCtrl = pInputManager->GetController( 0);

		if((pCtrl->GetPressState() & (I3I_BTN_L1 | I3I_BTN_R1)) == (I3I_BTN_L1 | I3I_BTN_R1))
		{
			// L1과 R1이 동시에 눌려진 상태
			// Debug 기능들을 나열한다.

			if( pCtrl->GetStrokeState() & I3I_BTN_RLEFT)
			{
				m_pViewer->ReportPerformance();
			}
			else if( pCtrl->GetStrokeState() & I3I_BTN_RRIGHT)
			{
				REPORT_PROFILE();
			}
			else if( pCtrl->GetStrokeState() & I3I_BTN_RDOWN)
			{
				m_pViewer->SetDumpState( true);
			}
		}
		else
		{
			if( pCtrl->GetPressState() & I3I_BTN_START)
			{
				do
				{
					pInputManager->Update();
				} while( pCtrl->GetPressState() & I3I_BTN_START);
			}
		}
	}
#endif
}

void i3Framework::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	
	
	for(size_t i = 0; i < m_StageInfoList.size(); i++)
	{
		i3StageInfo * pInfo = m_StageInfoList[i];

		pInfo->OnBuildObjectList( List);
	}

	for(size_t i = 0; i < m_LayerList.size(); i++)
	{
		i3SgLayer * pLayer = m_LayerList[i];

		pLayer->OnBuildObjectList( List);
	}

	i3GameNode::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 FRAMEWORK
	{
		UINT8			m_ID[4] = { 'F', 'R', 'W', '1' };
		OBJREF64		m_pStartStage = 0;
		UINT32			pad[30] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Framework::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::FRAMEWORK data;

	Result = i3GameNode::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_pStartStage = (OBJREF64) pResFile->GetObjectPersistID( m_pStartStageInfo);

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	// Stage Info
	{
		Rc = SaveFromListToResourceFile(m_StageInfoList, pResFile);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	// Layer Info
	{
//		Rc = m_LayerList.SaveTo( pResFile);
		Rc = SaveFromListToResourceFile(m_LayerList, pResFile);

		I3_CHKIO( Rc);
		Result += Rc;
	}

	// Common Resource Info
	{
		Rc = SaveFromStringListToResourceFile(m_CmnResList, pResFile);		//m_CmnResList.SaveTo( pResFile);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 i3Framework::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::FRAMEWORK data;

	Result = i3GameNode::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = LoadFromResourceFileToList(m_StageInfoList, pResFile, true);
		//m_StageInfoList.LoadFrom( pResFile, true);
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = LoadFromResourceFileToList(m_LayerList, pResFile, true);
		//m_LayerList.LoadFrom( pResFile, true);
	I3_CHKIO( Rc);
	Result += Rc;

	//Common ResList
	Rc = LoadFromResourceFileToStringList(m_CmnResList, pResFile);			//m_CmnResList.LoadFrom( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	// Start Stage
	if( data.m_pStartStage != 0)
	{
		i3StageInfo * pInfo = (i3StageInfo *) pResFile->FindObjectByID( (OBJREF) data.m_pStartStage);

		I3ASSERT( pInfo != nullptr);

		SetStartStageInfo( pInfo);
	}

	return Result;
}

static i3Framework *	s_pCurrentFramework = nullptr;

i3Framework *		i3Framework::getCurrentFramework(void)
{
	return s_pCurrentFramework;
}

void i3Framework::setCurrentFramework( i3Framework * pFramework)
{
	s_pCurrentFramework = pFramework;
}

void i3Framework::SetCapture( i3EventReceiver * pTarget)
{
	s_pCurrentFramework->SetFocusLock( pTarget);
}

void i3Framework::ReleaseCapture(void)
{
	s_pCurrentFramework->ReleaseFocusLock();
}

void i3Framework::SetCurrentFocus( i3EventReceiver * pFocus, bool bPush)
{
	s_pCurrentFramework->SetFocus( pFocus, bPush);
}

i3EventReceiver *   i3Framework::GetCurrentFocus()
{
	// revision 2251
	if( s_pCurrentFramework == nullptr)
	{
		I3PRINTLOG(I3LOG_NOTICE, "i3Framework::GetCurrentFocus() s_pCurrentFramework : nullptr");
		return nullptr;
	}
	return s_pCurrentFramework->getFocus();
}

i3GameNode * i3Framework::FindGameNodeByType( i3GameNode * pRoot, i3ClassMeta * pMeta, bool bRecursive)
{
	i3GameNode * pResult;

	if( pRoot->kind_of( pMeta))
		return pRoot;

	if( bRecursive)
	{
		i3GameNode * pChild = pRoot->getFirstChild();
		while ( pChild != nullptr)
		{
			pResult = FindGameNodeByType( pChild, pMeta, true);
			if( pResult != nullptr)
				return pResult;

			pChild = pChild->getNext();
		}
	}

	return nullptr;
}

bool	i3Framework::OnLostDevice( bool bLostDevice)
{
	if( m_pFadeEffect != nullptr)
	{
		m_pFadeEffect->OnDestroy();
	}

	m_pUIManager->PauseIO();
	
	FlushDXForThread();

	return true;
}

bool	i3Framework::OnRevive( i3RenderContext * pCtx)
{
	if( m_pFadeEffect != nullptr)
	{
		m_pFadeEffect->OnRevive();
	}

	m_pUIManager->ResumeIO();

	return true;
}

void i3Framework::GetProfileString( char * pszBuf, INT32 len)
{
	if( m_pUIRenderer != nullptr)
	{
		m_pUIRenderer->GetProfileString( pszBuf, len);
	}
}

void	i3Framework::GetProfileString(i3::string& inout)
{
	if( m_pUIRenderer != nullptr)
	{
		m_pUIRenderer->GetProfileString( inout);
	}
}

void i3Framework::UpdateBound(void)
{
	m_pSgContext->UpdateGlobalBound( true);
}


#if defined( I3_WINDOWS)
void i3Framework::OnLButtonDown( UINT32 nFlag, POINT point)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchLButtonDown( nFlag, point) == false)
			return;
	}

	if( getFocus() == nullptr)
	{
		if( m_pStage != nullptr)
			m_pStage->OnLButtonDown( nFlag, point);
	}
	else
	{
		getFocus()->OnMapCoord( &point);
		getFocus()->OnLButtonDown( nFlag, point);
	}
}

void i3Framework::OnLButtonUp( UINT32 nFlag, POINT point)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchLButtonUp( nFlag, point) == false)
			return;
	}

	if( getFocus() == nullptr)
	{
		if( m_pStage != nullptr)
			m_pStage->OnLButtonUp( nFlag, point);
	}
	else
	{
		getFocus()->OnMapCoord( &point);
		getFocus()->OnLButtonUp( nFlag, point);
	}
}

void i3Framework::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchLButtonDblClk( nFlag, point) == false)
			return;
	}

	if( getFocus() == nullptr)
	{
		if( m_pStage != nullptr)
			m_pStage->OnLButtonDblClk( nFlag, point);
	}
	else
	{
		getFocus()->OnMapCoord( &point);
		getFocus()->OnLButtonDblClk( nFlag, point);
	}
}

void i3Framework::OnMButtonDown( UINT32 nFlag, POINT point)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchMButtonDown( nFlag, point) == false)
			return;
	}

	if( getFocus() == nullptr)
	{
		if( m_pStage != nullptr)
			m_pStage->OnMButtonDown( nFlag, point);
	}
	else
	{
		getFocus()->OnMapCoord( &point);
		getFocus()->OnMButtonDown( nFlag, point);
	}
}

void i3Framework::OnMButtonUp( UINT32 nFlag, POINT point)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchMButtonUp( nFlag, point) == false)
			return;
	}

	if( getFocus() == nullptr)
	{
		if( m_pStage != nullptr)
			m_pStage->OnMButtonUp( nFlag, point);
	}
	else
	{
		getFocus()->OnMapCoord( &point);
		getFocus()->OnMButtonUp( nFlag, point);
	}
}

void i3Framework::OnRButtonDown( UINT32 nFlag, POINT point)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchRButtonDown( nFlag, point) == false)
			return;
	}

	if( getFocus() == nullptr)
	{
		if( m_pStage != nullptr)
			m_pStage->OnRButtonDown( nFlag, point);
	}
	else
	{
		getFocus()->OnMapCoord( &point);
		getFocus()->OnRButtonDown( nFlag, point);
	}
}

void i3Framework::OnRButtonUp( UINT32 nFlag, POINT point)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchRButtonUp( nFlag, point) == false)
			return;
	}

	if( getFocus() == nullptr)
	{
		if( m_pStage != nullptr)
			m_pStage->OnRButtonUp( nFlag, point);
	}
	else
	{
		getFocus()->OnMapCoord( &point);
		getFocus()->OnRButtonUp( nFlag, point);
	}
}

void i3Framework::OnMouseMove( UINT32 nFlags, POINT point)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchMouseMove( nFlags, point) == false)
			return;
	}

	if( getOnMouseTarget() == nullptr)
	{
		if( m_pStage != nullptr)
			m_pStage->OnMouseMove( nFlags, point);
	}
	else
	{
		getOnMouseTarget()->OnMapCoord( &point);
		getOnMouseTarget()->OnMouseMove( nFlags, point);
	}
}

void i3Framework::OnMouseWheel( UINT nFlag, short zDelta, POINT point)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchMouseWheel( nFlag, zDelta, point) == false)
			return;
	}

	if( getFocus() == nullptr)
	{
		if( m_pStage != nullptr)
			m_pStage->OnMouseWheel( nFlag, zDelta, point);
	}
	else
	{
		getFocus()->OnMapCoord( &point);
		getFocus()->OnMouseWheel( nFlag, zDelta, point);
	}
}

void i3Framework::OnKeyDown( UINT32 nKey)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchKeyDown( nKey) == false)
			return;
	}

	if( getFocus() == nullptr)
	{
		if( m_pStage != nullptr)
			m_pStage->OnKeyDown( nKey);
	}
	else
	{
		i3EventReceiver * pReceiver = getFocus();

		pReceiver->OnKeyDown( nKey);
	}
}

void i3Framework::OnKeyUp( UINT32 nKey)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchKeyUp( nKey) == false)
			return;
	}

	if( getFocus() == nullptr)
	{
		if( m_pStage != nullptr)
			m_pStage->OnKeyUp( nKey);
	}
	else
	{
		getFocus()->OnKeyUp( nKey);
	}
}

void i3Framework::OnActivate( bool bActivate)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchActivate( bActivate) == false)
			return;
	}
}

void i3Framework::OnSetFocus(void)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchSetFocus() == false)
			return;
	}
}

void i3Framework::OnKillFocus(void)
{
	if( (m_pInputDispatcher != nullptr) && (m_bLockFocus == false))
	{
		if( m_pInputDispatcher->DispatchKillFocus() == false)
			return;
	}
}

void i3Framework::OnExitSizeMove(void)
{
}

bool i3Framework::OnQueryClose(void)
{
	return true;
}

void	i3Framework::OnChangeVideo( i3VideoInfo * pInfo)
{
	if( m_pStage != nullptr)
	{
		m_pStage->OnChangeVideo( pInfo);
	}

	if( m_pSgContext->getCurrentCamera() != nullptr)
	{
		i3Camera * pCam = m_pSgContext->getCurrentCamera();

		pCam->SetWindowSize( (REAL32) pInfo->m_Width, (REAL32) pInfo->m_Height);
	}

	if( m_pUIRenderer != nullptr)
	{
		m_pUIRenderer->OnSize( pInfo->m_Width, pInfo->m_Height);
	}
}

REAL32 i3Framework::GetAccTime()
{
	return g_rAccTime;
}

#endif


void i3Framework::UpdateDXForThread( void)
{
	i3VertexArrayDX::UpdateForThread();
	i3IndexArray::UpdateForThread();
	i3TextureDX::UpdateForThread();
	i3Surface::UpdateForThread();
}

void i3Framework::FlushDXForThread( void)
{
	i3VertexArrayDX::LostDeviceForThread();
	//i3IndexArray::LostDeviceForThread();
	i3TextureDX::LostDeviceForThread();
	i3Surface::LostDeviceForThread();
}