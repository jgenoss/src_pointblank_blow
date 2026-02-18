#include "i3FrameworkPCH.h"
#include "i3UIManager.h"
#include "i3UIEditBox.h"
#include "../i3Framework.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

#include "i3Base/itl/container_util.h"
#include "i3UIFilePathMgr.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

bool	g_bUIDumpLog = false;

I3_CLASS_INSTANCE(i3UIManager);

static i3Viewer*				s_pViewer		= nullptr;

static i3UIManager *			s_pUIManager	= nullptr;
static bool						s_bToolDevMode	= false;
static bool						s_bPropertyUpdate = false;
static	i3::vector<SERVICECOUNTRY>	s_CountryList;

static pfCallback m_cb = nullptr;
//
// i3UIConsole
//
#include "i3UIConsoleContext.h"

static bool gs_i3UIConsoleUsingConsole= false;
static bool gs_i3UIConsoleShowConsole = false;
static i3ClassMeta* gs_i3UIConsoleClientCtxMeta = nullptr;
static i3UIConsoleClientContext* gs_i3UIConsoleClientCtx = nullptr;

i3UIManager::PostDrawCallbackPFN i3UIManager::ms_cbOnPostDraw = nullptr;


static void Createi3UIConsole()
{
	if(!gs_i3UIConsoleUsingConsole)
		return;

	I3ASSERT(!gs_i3UIConsoleClientCtx);

	i3UIManager* uiMan = i3UI::getManager();

	if(!uiMan)
		return;
	
	if(!gs_i3UIConsoleClientCtxMeta)
	{
		gs_i3UIConsoleUsingConsole = false;
		i3Ime::EnableConsoleKey(false);
	}
	else
	{
		gs_i3UIConsoleClientCtx = (i3UIConsoleClientContext*)gs_i3UIConsoleClientCtxMeta
			->create_instance();
		gs_i3UIConsoleClientCtx->AddRef();

		i3Ime::EnableConsoleKey(true);
		gs_i3UIConsoleClientCtx->Attach(uiMan);
	}
}

static void Destroyi3UIConsole()
{
	// destroy console
	if(gs_i3UIConsoleClientCtx)
	{
		gs_i3UIConsoleClientCtx->Detach();

		I3_SAFE_RELEASE(gs_i3UIConsoleClientCtx);
		gs_i3UIConsoleClientCtxMeta = nullptr;
		gs_i3UIConsoleShowConsole = false;
		gs_i3UIConsoleUsingConsole = false;
	}
}

I3_EXPORT_FRAMEWORK
i3UIConsoleClientContext * i3UI::GetUIConsoleContext( void)
{
	return gs_i3UIConsoleClientCtx;
}


//
// i3UIManager
//

i3UIManager::i3UIManager()
{ 
	i3UIFilePathMgr::i()->Initialize();

	s_pUIManager = this;
	
	for (int i = 0; i < I3UI_MSGBOX_COUNT; i++)
		m_pDefMsgBox[i] = nullptr;
}

i3UIManager::~i3UIManager()
{	
	delete m_pmapControlHolderForTool;

	INT32 i;

	Destroyi3UIConsole();
	
	I3_SAFE_RELEASE( m_pDefModalDriver);

	for( i = 0; i < I3UI_MSGBOX_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_pDefMsgBox[i]);
	}
	i3UI::getCountryList()->clear();

	CloseVirtualTexture();

	RemoveAllScene();

	I3_SAFE_RELEASE( m_pUILibrary);
	I3_SAFE_RELEASE( m_pIme);

	s_pViewer = nullptr;
	s_pUIManager = nullptr;
}

void	i3UIManager::RegisterControlForTool(i3UIControl* pCtrl)
{
	if (m_pmapControlHolderForTool == nullptr)
		m_pmapControlHolderForTool = new i3::unordered_set<i3UIControl*>();

	(*m_pmapControlHolderForTool).insert(pCtrl);
}

void	i3UIManager::UnregisterControlForTool(i3UIControl* pCtrl)
{
	(*m_pmapControlHolderForTool).erase(pCtrl);
}

void	i3UIManager::RefreshAllActiveTemplateForTool()
{
	if ( m_pmapControlHolderForTool == nullptr)
		return;

	struct fn
	{
		void operator()(i3UIControl* pCtrl) const
		{
			UINT128 hashCode = pCtrl->getTemplateHashCode();
			UINT128 emptyCode; emptyCode = 0;

			if (hashCode == emptyCode)			// 전혀 템플릿 없음..스킵..
				return;

			i3UITemplate* pNewTemplate = i3UI::FindTemplateByHash(hashCode);

			if (pNewTemplate == nullptr)
			{
				I3TRACE("%s 콘트롤에 신규 템플릿이 없으므로 기존의 템플릿을 유지합니다.\n", pCtrl->GetNameString());
				return;
			}

			i3UITemplate* pCurrTemplate = pCtrl->getTemplate();

			if (pNewTemplate != pCurrTemplate)
			{
				pCtrl->SetTemplate(pNewTemplate);		// 템플릿 교체만으로 모든 처리가 가능...
			}
			else //템플릿이 동일한 경우엔 다른 함수를 호출..
			{
				pCtrl->OnChangedTemplate();				// 리프레시 역할 수행..
			}
		}
	};

	i3::cu::for_each_container(*m_pmapControlHolderForTool, fn());

}



bool i3UIManager::Create( i3Framework * pFramework, bool bEditMode /*= false*/)
{
	m_pFramework = pFramework;

	{
		m_pDefModalDriver = i3UIModalDriver::new_object();
	}

	m_bEditMode = bEditMode;

	return true;
}

INT32 i3UIManager::AddScene( I3UI_SCENE_INFO * pInfo)
{
	i3_prof_func();

	if( pInfo == nullptr)	return -1;
	I3ASSERT( pInfo->m_pScene != nullptr);
	I3ASSERT( !pInfo->m_strSceneFile.empty() );

	m_listScene.push_back(pInfo);
	return (INT32)m_listScene.size() - 1;
}

void i3UIManager::RemoveScene( i3UIScene * pScene)
{
	const size_t nCount = m_listScene.size();

	for(size_t i = 0;i < nCount; ++i)
	{
		I3UI_SCENE_INFO * pInfo = m_listScene[i];
		if( pInfo->m_pScene == pScene)
		{
			I3_SAFE_RELEASE( pInfo->m_pScene);

			I3_MUST_DELETE( pInfo);

			m_listScene.erase(m_listScene.begin() + i);		// break가 없네...우선 break를 걸기로한다.. ( 2012.05.14.수빈)
			break;
		}
	}
}

void i3UIManager::RemoveAllScene(void)
{
	const size_t nCount = m_listScene.size();

	for(size_t i = 0;i < nCount; ++i)
	{
		I3UI_SCENE_INFO * pInfo = m_listScene[i];
			
		I3_SAFE_RELEASE( pInfo->m_pScene);

		I3_MUST_DELETE( pInfo);
	}

	m_listScene.clear();
}

void i3UIManager::ChangeSceneFile( i3UIScene* pScene, const char* pszNewName)
{
	for( size_t i = 0; i < m_listScene.size(); i++)
	{
		I3UI_SCENE_INFO * pInfo = m_listScene[i];
		if( pInfo->m_pScene == pScene)
		{
			pInfo->m_strSceneFile = pszNewName;
			break;
		}
	}
}

void i3UIManager::ResetlistDraw(void)
{
	i3GameNode * tempChild = getFirstChild();;

	m_listDraw.clear();


	while (tempChild != nullptr)
	{
		if (i3::same_of<i3UIScene*>(tempChild))
		{
			i3UIScene * pScene = (i3UIScene *)tempChild;

			if (m_listDraw.size() < (UINT32)pScene->getDepth()+1)
			{
				m_listDraw.resize(pScene->getDepth()+1);
			}

			m_listDraw.at(pScene->getDepth()).push_back(pScene);
		}

		tempChild = tempChild->getNext();
	}
}

void i3UIManager::Reset(void)
{
	RemoveAllScene();
	RemoveAllChild();
}

i3UIScene*	i3UIManager::LoadUIScene( const char * szPath)
{
	char szFullPath[ MAX_PATH];
	char szExt[ MAX_PATH];
	i3UIScene * pScene= nullptr;

	i3::extract_fileext( szPath, szExt);
	i3::to_upper( szExt);

	bool bBinary = false;

	if( i3::generic_is_iequal( szExt, "I3UIS") )
		bBinary = true;
#if defined( I3_DEBUG)
	else if( i3::generic_is_iequal( szExt, "I3UIE") == false)
	{
		I3PRINTLOG(I3LOG_FATAL,  "not UI Scene file!!!!!");
	}
#endif

	i3::safe_string_copy( szFullPath, szPath, _countof( szFullPath));

	if( i3UI::getToolDevMode() == false)
	{
		if( bBinary == true )
		{	// Binary file
			i3_prof_scope("i3ResourceFile Load i3UIS File");

			i3ResourceFile File;

			if( File.Load( szFullPath) == false)
			{
			#if defined( I3_DEBUG)
				I3PRINTLOG(I3LOG_WARN,  "failed load file: %s", szPath);
				return nullptr;
			#endif
			}
			else
			{
				pScene = (i3UIScene*) File.getKeyObject();
				I3_SAFE_ADDREF( pScene);
			}
		}
	}

	if( pScene == nullptr)
	{
		if( bBinary == false )
		{	// XML file
			i3_prof_scope("i3ResourceFile Load i3UIE File");

			i3XMLFile File;
			if( File.Load( szFullPath) == false)
			{
				I3PRINTLOG(I3LOG_WARN,  "failed load file: %s", szPath);
				return nullptr;
			}
			else
			{
				pScene = (i3UIScene*)File.getKeyObject();
				I3_SAFE_ADDREF( pScene);
			}
		}
	}

	I3ASSERT( pScene != nullptr);

	{
		I3UI_SCENE_INFO * pNew = new I3UI_SCENE_INFO; // (I3UI_SCENE_INFO*)i3MemAlloc( sizeof( I3UI_SCENE_INFO));
		NetworkDump_Malloc( pNew, sizeof(I3UI_SCENE_INFO), __FILE__, __LINE__);

		pNew->m_pScene = pScene;
		pNew->m_bEnabled = true;

//		pScene->AddRef();			// i3XMLFile/i3ResourceFile 소거전에 AddRef()되도록 위치 조절..(2012.09.11.수빈)

		pNew->m_strSceneFile = szFullPath;

		AddScene( pNew);
	}

	return pScene;
}
/*
void i3UIManager::RegisterDefaultMessageBox( I3UI_MSGBOX type, const char * pszPath)
{
	i3UIFrameWnd *	pWnd;

	pWnd = LoadUI( (char *) pszPath, false);

	I3ASSERT( pWnd != nullptr);
	I3_BOUNDCHK( type, I3UI_MSGBOX_COUNT);

	I3_REF_CHANGE( m_pDefMsgBox[ type], pWnd);
}
*/

void i3UIManager::AddFromUIRoot( i3UIRoot * pRoot)
{
	i3::vector<i3UIControl*>& List = pRoot->getChildList();

	const size_t nCount = List.size();
	for( size_t i = 0;i < nCount; ++i)
	{
		i3UIControl * pCtrl = List[i];
		I3ASSERT( pCtrl != nullptr);

		AddChild( pCtrl);
	}
}

void i3UIManager::AddFrameWnd( i3UIFrameWnd * pFrameWnd)
{
	I3ASSERT( pFrameWnd != nullptr);

	AddChild( pFrameWnd);
}

void i3UIManager::_Reg_RemoveWnd( i3UIFrameWnd * pWnd)
{
	m_RemoveWndList.push_back( pWnd);
}

bool i3UIManager::DoModal( i3UIFrameWnd * pFrameWnd, bool bCenter, i3EventReceiver * pDriver)
{
	m_ModalStack.Push( m_pCurActiveFrameWnd);

	pFrameWnd->addState( I3UI_STATE_MODAL);

	// Driver 설정.
	if( pDriver != nullptr)
	{
		pFrameWnd->setEventReceiver( pDriver);
	}
	else
	{
		// 사용자가 Driver를 제공하지 않았다면 기본 Driver로 설정.
		pFrameWnd->setEventReceiver( m_pDefModalDriver);
	}

	if( bCenter)
	{
		INT32 x, y;

		x = (s_pViewer->GetViewWidth() >> 1) - (INT32)(pFrameWnd->getWidth() * 0.5f);
		y = (s_pViewer->GetViewHeight() >> 1) - (INT32)(pFrameWnd->getHeight() * 0.5f);

		// 위치를 정수좌표로 떨어지게 만들기 위해 INT32 Type으로 좌표계산을 한다.
		// 만약 좌표 계산을 REAL32 Type으로 하면, Text의 Rendering시 좌표가
		// 정수로 떨어지지 않음으로 인해, Pixel이 찌그러지는 현상이 나타날 수 있다.

		pFrameWnd->setPos( (REAL32)x, (REAL32)y);
	}

	pFrameWnd->EnableExclusively();

	{
		m_pFramework->SetFocus( pFrameWnd);
	}

	return true;
}

void i3UIManager::EndModal( i3UIControl * pCtrl, i3UIControl * pReceiver, I3UI_MODALRESULT rv)
{
	i3UIFrameWnd * pDlg, * pPrev = nullptr;

	if( pCtrl == nullptr)
		return;

	pDlg = (i3UIFrameWnd *) pCtrl;

	I3ASSERT( pDlg == m_pCurActiveFrameWnd);

	// 이전 Window를 검색한다.
	m_ModalStack.Pop( &pPrev);

	if( pReceiver != nullptr)
	{
		Notify( pReceiver, I3UI_EVT_MODALRESULT, rv, (INT32) pDlg);
	}

	if( pDlg != m_pCurActiveFrameWnd)
		return;

	if( pPrev != nullptr)
	{
		m_bExclusiveActive = pPrev->isState( I3UI_STATE_MODAL);

		m_pFramework->SetFocus( pPrev);
	}
	else
	{
		m_bExclusiveActive = false;

		m_pFramework->SetFocus( nullptr);
	}

	pDlg->EnableCtrl(false, true);

	SetActiveFrameWnd( pPrev, m_bExclusiveActive);

	m_pLastOnMouseTarget = nullptr;
	m_pFramework->setOnMouseTarget( nullptr);
}

void i3UIManager::checkControlRelease( i3UIControl * pControl)
{
//	if( GetFocus() == pControl)
//	{
//		m_pFramework->SetFocus( nullptr);
//	}

	m_pFramework->RemoveFocusFromStack(pControl);

	if( m_pLastOnMouseTarget == pControl)
		m_pLastOnMouseTarget = nullptr;

	if( m_pFramework->getOnMouseTarget() == pControl)
		m_pFramework->setOnMouseTarget( nullptr);

	i3UIControl * pParent = (i3UIControl*)pControl->getParent();
	while( pParent != nullptr && !i3::same_of<i3UIScene*>(pParent))
	{
		if( i3::same_of<i3UIFrameWnd*>(pParent))
		{
			if( ((i3UIFrameWnd*)pParent)->getFocusCtrl() == pControl)
			{
				((i3UIFrameWnd*)pParent)->SetFocusCtrl( nullptr);
				break;
			}
		}

		pParent = (i3UIControl*)pParent->getParent();
	}
}

void i3UIManager::MsgBox( I3UI_MSGBOX type, const WCHAR16 * pszMsg, i3UIModalDriver * pDriver)
{
	I3_BOUNDCHK( type, I3UI_MSGBOX_COUNT);

	i3UIFrameWnd * pWnd = m_pDefMsgBox[ type];
	I3ASSERT( pWnd != nullptr);

	i3UIEditBox * pEditBox = (i3UIEditBox *) pWnd->FindCtrlByName( "Msg");
	I3ASSERT( pEditBox != nullptr);

	pEditBox->SetText( (WCHAR16 *) pszMsg);

	AddFrameWnd( pWnd);

	DoModal( pWnd, true, pDriver);
}

void i3UIManager::RemoveFrameWnd( i3UIFrameWnd * pFrameWnd)
{
	I3ASSERT( pFrameWnd != nullptr);

	RemoveChild( pFrameWnd);
}


void i3UIManager::ShowSceneTop( i3UIScene* pScene )
{
	// Enable 되어있는 Scene만 처리할꺼임
	if( pScene->isEnable()) {
		i3UIFrameWnd* pWnd = (i3UIFrameWnd*)pScene->getFirstChild();
		SetActiveFrameWnd( pWnd);
	}
}

void i3UIManager::SetUIResolution( INT32 width, INT32 height)
{
	m_nWidth = width;
	m_nHeight = height;

	m_sfUIResolutionWidth = 1.0f; //(REAL32)s_pViewer->GetViewWidth() / m_nWidth;
	m_sfUIResolutionHeight = 1.0f; //(REAL32)s_pViewer->GetViewHeight() / m_nHeight;

	m_sfUIResolutionWidthInGame = 1.0f; //(REAL32)s_pViewer->GetViewWidth() / m_nWidth ;
	m_sfUIResolutionHeightInGame = 1.0f; //(REAL32)s_pViewer->GetViewHeight() / m_nHeight;
}

void i3UIManager::SetActualResolution( INT32 w, INT32 h)
{
	m_ActualWidth = w;
	m_ActualHeight = h;

	m_DisplayOffsetX = (s_pViewer->GetViewWidth() - m_ActualWidth) / 2;
	m_DisplayOffsetY = (s_pViewer->GetViewHeight() - m_ActualHeight) / 2;
}

void i3UIManager::OnSize(void)
{
	if( s_pViewer == nullptr)
		return;

	SetUIResolution( m_nWidth, m_nHeight);

	//SetActualResolution( m_nWidth, m_nHeight);
	{
		// 해상도를 감안하여 원래의 비율이 유지되도록 Actual Resolution을 조정한다.
		REAL32 aspect = (REAL32) m_nWidth / m_nHeight;
		INT32 ax, ay;

		ay = s_pViewer->GetViewHeight();
		ax = (INT32)(ay * aspect);

		if( ax > s_pViewer->GetViewWidth())
		{
			// 1280x1024와 같이 비율이 4:3보다 가로가 짧은 경우에 대한 처리
			ax = s_pViewer->GetViewWidth();
			ay = (INT32) (ax / aspect);
		}

		SetActualResolution( ax, ay);
	}
}

// Viewer를 설정합니다.
void i3UIManager::SetViewer(i3Viewer* pViewer, bool bNoResize)			
{
	s_pViewer = pViewer;

	OnSize();
}

//
void i3UIManager::OnUpdate( REAL32 rDeltaSeconds)
{ 
	if(gs_i3UIConsoleUsingConsole && gs_i3UIConsoleClientCtx)
		gs_i3UIConsoleClientCtx->OnUpdate(rDeltaSeconds);
	// OnEvent를 통한 입력처리 후에 이루어져야한다.

	i3InputDispatchManager::OnUpdate( rDeltaSeconds );

	// 모든 i3UIScene 들을 검색해, OnDraw() 함수를 호출해 준다.
	{
		// 콘솔에서 사용하는씬은 한개라고 가정, 맨나중에 그리기위해 따로저장
		i3UIScene* deferedDrawConsoleScene = nullptr; 
		g_pUIRenderer->Draw_Start();

		ResetlistDraw();
		for (UINT32 i = 0; i < m_listDraw.size(); i++)
		{
			for (UINT32 j = 0; j < m_listDraw[i].size(); j++)
			{
				i3UIScene * pScene = (i3UIScene *)m_listDraw[i].at(j);
				if (pScene->IsConsoleScene())
				{
					if (IsShowConsole())
						deferedDrawConsoleScene = pScene;
				}
				else if (!pScene->IsManualDraw())
				{
					pScene->OnDraw(g_pUIRenderer);
					g_pUIRenderer->DepthPlus();	// Scene간에는 Layer를 따로한다.
				}
			}
		}
	
		if(ms_cbOnPostDraw)
			ms_cbOnPostDraw(this, g_pUIRenderer);

		if(deferedDrawConsoleScene)
		{
			g_pUIRenderer->DepthPlus();
			deferedDrawConsoleScene->OnDraw(g_pUIRenderer);
			g_pUIRenderer->DepthMinus();
		}

		// 추가 Draw (ComboBox)
		g_pUIRenderer->Draw_Finish();
	}
	
	i3UIFrameWnd * pFrame;

	if( m_RemoveWndList.size() > 0)
	{
		for(size_t i = 0; i < m_RemoveWndList.size(); i++)
		{
			pFrame = m_RemoveWndList[i];

			RemoveFrameWnd( pFrame);
		}

		m_RemoveWndList.clear();
	}
}

i3UIFrameWnd*  i3UIManager::HitTest( I3UI_PICK_INFO * pInfo)
{
	pInfo->m_pPickedCtrl = nullptr;

    bool isConsoleShow = i3UIManager::IsShowConsole();

	// active frame이 있다면 먼저 테스트 해보고
	if( m_pCurActiveFrameWnd != nullptr)
	{
		if( m_pCurActiveFrameWnd->HitTest( pInfo))
		{
			return m_pCurActiveFrameWnd;
		}
	}

	i3UIScene * pScene = (i3UIScene*)getFirstChild();
	if(isConsoleShow)
		pScene = FindConsoleScene(pScene); // 콘솔이 보이는상태라면 콘솔외 UI는 처리하지않는다.

	i3UIFrameWnd * pLastHitWnd = nullptr;
	i3UIScene * pLastScene = nullptr;

	while( pScene != nullptr)
	{
		if( pScene->isEnable())
		{
			if( pLastScene == nullptr || (pLastScene != nullptr &&
				pLastScene->getDepth() <= pScene->getDepth()) )
			{
				i3UIFrameWnd* pWindow = (i3UIFrameWnd*) pScene->getFirstChild();
			
				while( pWindow != nullptr) 
				{
					//요건 active frame이미 했으니 건너뛰기
					if( m_pCurActiveFrameWnd == pWindow)
					{
						pWindow = (i3UIFrameWnd*)pWindow->getNext();

						continue;
					}

					if( pWindow->isEnable() && pWindow->isFrameWnd())
					{
						if( pWindow->HitTest( pInfo))
						{
							pLastHitWnd = pWindow;
							pLastScene = pScene;

							// ComboBox의 ListBox는 가장 우선순위가 우선되야 합니다.
							I3ASSERT( pInfo->m_pPickedCtrl != nullptr);
							if( i3::same_of<i3UIListBox*>(pInfo->m_pPickedCtrl))
							{
								i3UIControl * pParent = (i3UIControl*) pInfo->m_pPickedCtrl->getParent();
								if( pParent != nullptr)
								{
									if( i3::kind_of<i3UIComboBase*>(pParent) )
									{
										return pLastHitWnd;
									}
								}
							}
						}
					}

					pWindow = (i3UIFrameWnd*)pWindow->getNext();
				}
			}
		}

		pScene = (i3UIScene*)pScene->getNext();

		if(isConsoleShow)
			pScene = FindConsoleScene(pScene);
	}

	return pLastHitWnd;
}

i3EventReceiver * i3UIManager::GetFocus(void)
{
	if( m_pFramework != nullptr)
	{
		return m_pFramework->getFocus();
	}
	else
		return nullptr;
}

// 선택된 Window를 최상위 Layer로 변경합니다.(또한 Focus도 가집니다)
void i3UIManager::SetActiveFrameWnd( i3UIFrameWnd* pWindow, bool bExclusiveActive, bool bUpdateActiveFrame)
{
	// 이전 Focus window에 대해...
	if( m_pCurActiveFrameWnd != pWindow)
	{
		i3EventReceiver *	pOldFocus = m_pFramework->getFocus();

		if( i3::kind_of<i3UIControl*>(pOldFocus))
		{
			i3UIControl * pCtrl = (i3UIControl *) pOldFocus;

			// FrameWnd를 찾는다.
			while((pCtrl != nullptr) && !i3::kind_of<i3UIFrameWnd*>(pCtrl) )
			{
				pCtrl = (i3UIControl *) pCtrl->getParent();
			}
			
			if( pCtrl != nullptr)
			{
				// FrameWnd이어야 한다.
				pCtrl->OnActivate( false);
			}
		}
		else
		{
			// 이전 Focus가 만약 UIFrameWnd가 아니라면 
			// 항상 Stack에 Push해두어야 한다.
		}

		if( pWindow != nullptr)
		{
			if( pWindow->OnActivate( true) == false)
				return;

			if( m_bEditMode)
				pWindow->ShowTop();	// sprite order를 top으로 설정한다.
			m_bExclusiveActive = bExclusiveActive;
		}
		else
		{
			m_bExclusiveActive = false;
		}
	}

	if( bUpdateActiveFrame)
		m_pCurActiveFrameWnd = pWindow;
}

i3UIFrameWnd *	i3UIManager::GetFocusFrameWnd(void)
{
	return m_pCurActiveFrameWnd;
}

void			i3UIManager::SetFocusSceneFrameWnd(i3UIScene* scn, bool bPush )
{
	i3UIFrameWnd* wnd = nullptr;
	if (scn != nullptr)
		wnd = static_cast<i3UIFrameWnd*>(scn->getFirstChild());

	i3UIManager::SetFocusControl(wnd, bPush);
}

// 이런게 필요할 것이다..

void			i3UIManager::SetFocusControl(i3UIControl* ctrl, bool bPush)
{
	// 해당 Point에 위치한 Window를 검색한다.
	i3UIFrameWnd* pWnd = nullptr;
	
	for (i3GameNode* c = ctrl ; c != nullptr;  )
	{
		i3GameNode* parent = c->getParent();

		if ( i3::same_of<i3UIScene*>(parent))
		{
			if (i3::kind_of<i3UIFrameWnd*>(ctrl))
				pWnd = static_cast<i3UIFrameWnd*>(ctrl);

			break;
		}
		c = parent;
	}

	if( m_bExclusiveActive)
	{
		// Modal 상태에서 ModalWindow 외의 다른 window가 선택되었다면 무시.
		if( pWnd != GetFocusFrameWnd()) 
			return;
	}

	// 해당 Window를 Activate 시킨다.
	if( pWnd != GetFocusFrameWnd())
	{
		SetActiveFrameWnd( pWnd, false, false);		// nullptr일 수 있다.
	}

	// Prev
	if( i3::same_of<i3UIEditBox*>(ctrl))
	{
		//i3UIControl * pPrevFocus = (i3UIControl*)m_pFramework->getFocus();

		if( pWnd != nullptr)
			pWnd->SetFocusCtrl( nullptr);

		m_pFramework->SetFocus(ctrl, bPush);

		// 이전 focus와 pick이 같으면, filtering되어서 onsetfocus가 안불려 지는데다가, 다시 찍을때 ime의 wnd가 killfocus된다.
//		if( pPrevFocus != nullptr && pPrevFocus == ctrl)
//		{
//			ctrl->OnSetFocus();
//		}
	}
	else
	{
		if( pWnd != nullptr)
		{
			pWnd->SetPreFocusCtrl( ctrl);
			m_pFramework->SetFocus( pWnd, bPush);
			pWnd->SetFocusCtrl( ctrl);
		}
		else
		{
			m_pFramework->SetFocus( ctrl, bPush);
		}
	}

	return;
}



void i3UIManager::EnableWindow( i3UIFrameWnd * pWnd, bool bEnable, bool bExclusively, bool bUpdateActiveFrame)
{
	I3ASSERT( pWnd != nullptr);

	if( bEnable)
	{
		SetActiveFrameWnd( pWnd, bExclusively, bUpdateActiveFrame);
	}
	else
	{
		if( pWnd == m_pCurActiveFrameWnd)
		{
			SetActiveFrameWnd( nullptr);
		}
	}
}

void i3UIManager::ShowWindow( i3UIFrameWnd * pWnd, bool bShow)
{
	if( bShow)
	{
		SetActiveFrameWnd( pWnd);
	}
	else if( GetFocusFrameWnd() == pWnd)
	{
		SetActiveFrameWnd( nullptr);
	}
}

i3EventReceiver * i3UIManager::_Find_EventReceiver( i3UIControl * pLeaf)
{
	while( pLeaf != nullptr)
	{
		if( pLeaf == (void *) this)
			break;

		if( !i3::kind_of<i3UIControl*>(pLeaf))
			break;

		if( pLeaf->getEventReceiver() != nullptr)
			return pLeaf->getEventReceiver();

		pLeaf = (i3UIControl *) pLeaf->getParent();
	}

	return nullptr;
}

void i3UIManager::Notify( i3UIControl * pCtrl, I3UI_EVT nEvent,INT32 nPriParam,INT32 nSecParam)
{
	I3UI_CONTROL_NOTIFY Notify;
	
	Notify.m_nID		= pCtrl->getID();
	Notify.m_nEvent		= (I3UI_EVT) nEvent;
	Notify.m_nPriParam	= nPriParam;
	Notify.m_nSecParam	= nSecParam;
	Notify.m_pCtrl		= pCtrl;

	if( pCtrl->getOwner() == nullptr)
	{
		i3UIFrameWnd * pOwner = pCtrl->FindOwner();

		pCtrl->setOwner( pOwner);
	}

	Notify.m_pFrameWnd	= pCtrl->getOwner();

	// 자동으로 Dispatch
	i3EventReceiver * pReceiver = _Find_EventReceiver( pCtrl);

	if( pReceiver != nullptr)
	{
		pReceiver->OnEvent( I3_EVT_UI_NOTIFY, this, (UINT32) &Notify);
	}

	SendEvent( nEvent, pCtrl, nPriParam, nSecParam);
}

void i3UIManager::ParentNotify( i3UIControl * pParent, i3UIControl * pFrom, I3UI_EVT nEvent, INT32 nPriParam, INT32 nSecParam)
{
	if( pParent != nullptr)
	{
		pParent->OnNotify( pFrom, nEvent, nPriParam, nSecParam);

		SendNotify( nEvent, pParent, pFrom, nPriParam, nSecParam);
	}
}

void i3UIManager::SendEvent( INT32 nEvent, i3UIControl * pCtrl, INT32 arg, INT32 nSecParam)
{
	if( pCtrl == nullptr)	return;
	if( pCtrl->isEvtEnable((I3UI_EVT) nEvent) == false)	return;
	
	i3UIScene	*	pAncestor = pCtrl->getAncestor();
	if( pAncestor != nullptr)
	{
		pAncestor->OnSendEvent( nEvent, pCtrl, arg, nSecParam);
	}
}

void i3UIManager::SendNotify( INT32 nEvent, i3UIControl * pParent, i3UIControl * pFrom, INT32 nPriParam, INT32 nSecParam)
{
	if( pParent == nullptr)	return;

	if( pParent->isEvtEnable((I3UI_EVT) nEvent) == true)
	{
		i3UIScene	*	pAncestor = pFrom->getAncestor();
		if( pAncestor != nullptr)
		{
			pAncestor->OnSendNotify( nEvent, pParent, pFrom, nPriParam, nSecParam);
		}
	}

	// 최상단에 보낼수 있는 루틴을 일단 추가한다....11.12.14.수빈...
	i3UIControl* rootCtrl = nullptr;
	for (i3GameNode* c = pParent->getParent(); c != nullptr ; )
	{
		i3GameNode* p = c->getParent();
		if ( i3::same_of<i3UIScene*>(p))
		{
			rootCtrl = static_cast<i3UIControl*>(c);
			break;
		}
		c = p;
	}

	if (rootCtrl == nullptr)
		return;

	if ( rootCtrl->isEvtEnable((I3UI_EVT) nEvent) == true) 
	{
		i3UIScene	*	pAncestor = pFrom->getAncestor();
		if( pAncestor != nullptr)
		{
			pAncestor->OnSendNotify( nEvent, rootCtrl, pFrom, nPriParam, nSecParam);
		}
		
	}
	//
}


i3UIControl *	i3UIManager::_Rec_FindChildControlByName( LuaState * L, i3UIControl * pCtrl, const char * pszName)
{
	if( pCtrl->hasName())
	{
		if( i3::generic_is_iequal( pCtrl->GetNameString(), pszName) )
			return pCtrl;
	}

	i3UIControl * pRet  = nullptr;
	i3UIControl * pTemp = (i3UIControl *) pCtrl->getFirstChild();
	while( pTemp != nullptr)
	{
		pRet = _Rec_FindChildControlByName( L, pTemp, pszName);
		if( pRet != nullptr)
		{
			return pRet;
		}

		pTemp = (i3UIControl*)pTemp->getNext();
	}

	return pRet;
}

i3UIControl *	 i3UIManager::FindChildControlByName( LuaState * L, const char * pszName)
{
	i3UIControl * pRet = nullptr;

	i3UIScene * pScene = FindAncestorByState( L);
	if( pScene)
	{
		i3UIControl * pChild = (i3UIControl*) pScene->getFirstChild();
		while( pChild != nullptr)
		{
			pRet = _Rec_FindChildControlByName( L, pChild, pszName);
			if( pRet != nullptr)
				return pRet;

			pChild = (i3UIControl*)pChild->getNext();
		}

		pScene->Dump();
	}

	return pRet;
}

i3UIControl *	i3UIManager::FindChildControlByName( const char* pszName, i3UIScene* pScene)
{
	I3ASSERT( pScene != nullptr);

	i3UIControl* pChild = (i3UIControl*)pScene->getFirstChild();

	while( pChild != nullptr)
	{
		i3UIControl* pFound = _Rec_FindChildControlByName( nullptr, pChild, pszName);
		if( pFound != nullptr)
			return pFound;

		pChild = (i3UIControl*)pChild->getNext();
	}

	return nullptr;
}

// 
i3UIControl * i3UIManager::TA_FindControlByPath(const char * pszPath)
{
	// Path를 구분자(/) 기준으로 잘라낸다.
	i3GameNode * pChild;
	i3UIControl * pFind = nullptr;

	pChild = getFirstChild();

	while (pChild != nullptr)
	{
		if (pChild->isEnable() && pChild->kind_of(i3UIScene::static_meta()))
		{
			i3UIScene * pScene = (i3UIScene *)pChild;

			pFind = pScene->TA_FindCtrlByPath(pszPath);

			if (pFind != nullptr)
				return pFind;
		}
		
		pChild = pChild->getNext();
	}

	return nullptr;
}
 
static bool InsideCtrl( i3UIControl * pCtrl, const POINT & point)
{
	RECT rt;

	//ComboBox 예외 처리
	if( i3::kind_of<i3UIComboBase*>(pCtrl)) 
	{
		i3UIComboBox* pCombo = (i3UIComboBox*)pCtrl;
		REAL32 h = pCombo->getMinimumHeight();
		
		rt.left = (INT32)pCtrl->getAccumulatedPos()->x;
		rt.top  = (INT32)pCtrl->getAccumulatedPos()->y;
		rt.right = (INT32)(pCtrl->getAccumulatedPos()->x + pCtrl->getWidth());
		rt.bottom = (INT32)(pCtrl->getAccumulatedPos()->y + h);
	}
	else
	{
		rt.left = (INT32)pCtrl->getAccumulatedPos()->x;
		rt.top  = (INT32)pCtrl->getAccumulatedPos()->y;
		rt.right = (INT32)(pCtrl->getAccumulatedPos()->x + pCtrl->getWidth());
		rt.bottom = (INT32)(pCtrl->getAccumulatedPos()->y + pCtrl->getHeight());
	}

	if( rt.right < rt.left)
	{
		INT32 t = rt.right;
		rt.right = rt.left;
		rt.left = t;
	}

	if( rt.bottom < rt.top)
	{
		INT32 t = rt.bottom;
		rt.bottom = rt.top;
		rt.top = t;
	}
	
	if( ::PtInRect( &rt, point))
		return true;
	else
		return false;
}

bool i3UIManager::TA_Rec_GetPickedCtrl( i3GameNode * pNode, POINT point, std::vector<i3UIControl *> & list)
{
	i3GameNode * pChild = pNode->getFirstChild();

	while( pChild != nullptr)
	{
		if( pChild->isEnable())
		{
			TA_Rec_GetPickedCtrl( pChild, point, list);
		}

		pChild = pChild->getNext();
	}

	// 가장 마지막에 자기와 비교
	if( i3::kind_of( pNode, i3UIControl::static_meta()))
	{
		i3UIControl * pCtrl = (i3UIControl *) pNode;

		if( InsideCtrl( pCtrl, point))
		{
			//list.insert( list.begin(), pCtrl);
			list.push_back( pCtrl);
			return true;
		}
	}

	return true;
}

bool i3UIManager::TA_GetPickedCtrl( POINT pt, std::vector<i3UIControl *> & list)
{
	i3GameNode * pChild = getFirstChild();

	list.clear();

	while( pChild != nullptr)
	{
		if( pChild->isEnable())
		{
			TA_Rec_GetPickedCtrl( pChild, pt, list);
		}

		pChild = pChild->getNext();
	}

	if( list.size() > 0)
		return true;

	return false;
}


void i3UIManager::ConvertMouseCoord( POINT * pPt)
{
	INT32 sx, sy;
	INT32 offX, offY;
	REAL32 rx, ry;

	sx = s_pViewer->GetViewWidth();
	sy = s_pViewer->GetViewHeight();

	// Offset 적용
	{
		offX = (sx - getActualWidth()) / 2;
		offY = (sy - getActualHeight()) / 2;

		pPt->x -= offX;
		pPt->y -= offY;
	}

	rx = (REAL32) pPt->x / getActualWidth();
	ry = (REAL32) pPt->y / getActualHeight();

	pPt->x = (INT32)(rx * getWidth());
	pPt->y = (INT32)(ry * getHeight());
}

void i3UIManager::ConvertToClientCoord(POINT * pPt)
{
	INT32 sx, sy;
	INT32 offX, offY;
	REAL32 rx, ry;

	sx = s_pViewer->GetViewWidth();
	sy = s_pViewer->GetViewHeight();

	rx = (REAL32)pPt->x / getWidth();
	ry = (REAL32)pPt->y / getHeight();

	pPt->x = (INT32)(rx * getActualWidth());
	pPt->y = (INT32)(ry * getActualHeight());

	// Offset 적용
	{
		offX = (sx - getActualWidth()) / 2;
		offY = (sy - getActualHeight()) / 2;

		pPt->x += offX;
		pPt->y += offY;
	}
}

bool i3UIManager::OnLostDevice( bool bLostDevice)
{
	if( i3InputDispatchManager::OnLostDevice( bLostDevice) == false)
		return false;

	OnSize();

	return true;
}

bool i3UIManager::OnRevive( i3RenderContext * pCtx)
{
	if( i3InputDispatchManager::OnRevive( pCtx) == false)
		return false;
	
	return true;
}



///////////////////////////////////////////////////////////////
// i3UIConsole
///////////////////////////////////////////////////////////////

void i3UIManager::InitConsole(bool useConsole, i3ClassMeta* consoleClientCtxMeta)
{
	static bool tryOnce = false;

	if(tryOnce)
		return;

	tryOnce = true;

	gs_i3UIConsoleUsingConsole = useConsole;
	gs_i3UIConsoleClientCtxMeta = useConsole ? consoleClientCtxMeta : nullptr;

	if(!useConsole)
		gs_i3UIConsoleClientCtxMeta = nullptr;
}

void i3UIManager::ShowConsole(bool showConsole)
{
	if(!gs_i3UIConsoleUsingConsole && !gs_i3UIConsoleClientCtxMeta && !i3UI::getManager())
		return;

	if(!gs_i3UIConsoleClientCtx)
	{
		static bool createdOnce = false;

		if(createdOnce)
		{
			gs_i3UIConsoleShowConsole = false;
			return;
		}

		Createi3UIConsole();
		createdOnce = true;

		if(!gs_i3UIConsoleClientCtx)
		{
			gs_i3UIConsoleShowConsole = false;
			return;
		}
	}

	gs_i3UIConsoleShowConsole = gs_i3UIConsoleClientCtx->Show(showConsole);
}

bool i3UIManager::OnConsoleEvent(UINT32 event, i3ElementBase* pObj, UINT32 param2, 
	I3_EVT_CODE code)
{
	if(!gs_i3UIConsoleClientCtx || !IsShowConsole())
		return true;
	
	return gs_i3UIConsoleClientCtx->OnEvent(event, pObj, param2, code);
}

i3UIScene* i3UIManager::FindConsoleScene(i3UIScene* scene, bool skipGivenParamScene)
{
	if(scene && skipGivenParamScene)
		scene = (i3UIScene*)scene->getNext();

	while(scene && !scene->IsConsoleScene())
		scene = (i3UIScene*)scene->getNext();

	return scene;
}


bool i3UIManager::IsUsingConsole(bool onlyUseable) 
{ 
	if(onlyUseable)
		return gs_i3UIConsoleUsingConsole;
	else
		return gs_i3UIConsoleUsingConsole && gs_i3UIConsoleClientCtx ? true : false; 
}

bool i3UIManager::IsShowConsole()  { return gs_i3UIConsoleShowConsole; }


INT32 i3UIManager::CExec(const wchar_t* fmt, ...)
{
	if(!gs_i3UIConsoleClientCtx)
		return EI3UICSRST_FAIL_INIT_ATTACH;

	wchar_t buf[1024]; 
	buf[0] = 0;
	va_list arg; 
	va_start(arg, fmt);

	vswprintf_s(buf, 1024, fmt, arg); 
	va_end(arg);

	return (INT32)gs_i3UIConsoleClientCtx->Exec(buf, nullptr);		
}

void i3UIManager::CLog(const wchar_t* fmt, ...)
{
	if(!gs_i3UIConsoleClientCtx)
		return;

	wchar_t buf[1024]; 
	buf[0] = 0;
	va_list arg; 
	va_start(arg, fmt);

	vswprintf_s(buf, 1024, fmt, arg); 
	va_end(arg);

	gs_i3UIConsoleClientCtx->AddLog(0, buf);
}

void i3UIManager::CLogA(UINT32 attr, const wchar_t* fmt, ...)
{
	if(!gs_i3UIConsoleClientCtx)
		return;

	wchar_t buf[1024]; 
	buf[0] = 0;
	va_list arg; 
	va_start(arg, fmt);

	vswprintf_s(buf, 1024, fmt, arg); 
	va_end(arg);

	gs_i3UIConsoleClientCtx->AddLog(attr, buf);
}

i3UIControl * i3UIManager::TA_ScanText( const wchar_t * pszText, bool bExact)
{
	i3GameNode * pChild = getFirstChild();

	while( pChild != nullptr)
	{
		if( pChild->isEnable())
		{
			if( pChild->kind_of( i3UIScene::static_meta()))
			{
				i3UIScene * pScene = (i3UIScene *) pChild;

				i3UIControl * pFind = pScene->TA_ScanText( pszText, bExact);
				if( pFind != nullptr)
					return pFind;
			}
		}

		pChild = pChild->getNext();
	}

	return nullptr;
}

void		i3UIManager::PauseIO()
{
	if (m_pVirtualTexture)
		m_pVirtualTexture->GetIO()->Pause(true);
}

void		i3UIManager::ResumeIO()
{
	if (m_pVirtualTexture)
		m_pVirtualTexture->GetIO()->Resume();
}



///////////////////////////////////////////////////////////////


I3_EXPORT_FRAMEWORK
void i3UI::setToolDevMode(bool bSet)
{
	s_bToolDevMode = bSet;
}

I3_EXPORT_FRAMEWORK
bool i3UI::getToolDevMode(void)
{
	return s_bToolDevMode;
}

I3_EXPORT_FRAMEWORK
i3UIManager * i3UI::getManager(void)
{
	return s_pUIManager;
}

I3_EXPORT_FRAMEWORK
void i3UI::Reset(void)
{
	if( s_pUIManager != nullptr)
		s_pUIManager->Reset();
}

void i3UI::releaseControl( i3UIControl * pControl)
{
	if( s_pUIManager != nullptr)
		s_pUIManager->checkControlRelease( pControl);
}

I3_EXPORT_FRAMEWORK
REAL32 i3UI::getUIResolutionWidth(void)
{
	if( s_pUIManager != nullptr)
		return s_pUIManager->getUIResolutionWidth();

	return 0.f;
}

I3_EXPORT_FRAMEWORK
REAL32 i3UI::getUIResolutionHeight(void)
{
	if( s_pUIManager != nullptr)
		return s_pUIManager->getUIResolutionHeight();

	return 0.f;
}

I3_EXPORT_FRAMEWORK
i3Viewer* i3UI::getViewer(void)					
{ 
	return s_pViewer;	
}

I3_EXPORT_FRAMEWORK		
void i3UI::setDisableUIInput( bool bDisable)
{
	if( s_pUIManager != nullptr)
		s_pUIManager->setInputDisable( bDisable);

#if defined( I3_DEBUG) && 0
	I3TRACE( "InputDisable(%d)\n", bDisable);
#endif
}

I3_EXPORT_FRAMEWORK		
bool i3UI::getDisableUIInput(void)
{
	if( s_pUIManager != nullptr)
		return s_pUIManager->getInputDisable();

	return false;
}

I3_EXPORT_FRAMEWORK		
i3EventReceiver	*	i3UI::GetFocus(void)
{
	if( s_pUIManager != nullptr)
		return s_pUIManager->GetFocus();

	return nullptr;
}

I3_EXPORT_FRAMEWORK		
void				i3UI::SetFocusSceneFrameWnd(i3UIScene* scn, bool bPush)
{
	if (s_pUIManager != nullptr)
		s_pUIManager->SetFocusSceneFrameWnd(scn, bPush);
}

I3_EXPORT_FRAMEWORK		
void				i3UI::SetFocusControl(i3UIControl* ctrl, bool bPush)
{
	if (s_pUIManager != nullptr)
		s_pUIManager->SetFocusControl(ctrl, bPush);
}

I3_EXPORT_FRAMEWORK
void i3UI::Notify( i3UIControl * pCtrl, I3UI_EVT nEvent, INT32 nPriParam, INT32 nSecParam)
{
	if( s_pUIManager != nullptr)
		s_pUIManager->Notify( pCtrl, nEvent, nPriParam, nSecParam);
}

I3_EXPORT_FRAMEWORK
void i3UI::ParentNotify( i3UIControl * pParent, i3UIControl * pChild, I3UI_EVT nEvent, INT32 nPriParam, INT32 nSecParam)
{
	if( s_pUIManager != nullptr)
		s_pUIManager->ParentNotify( pParent, pChild, nEvent, nPriParam, nSecParam);
}

I3_EXPORT_FRAMEWORK
i3UIFrameWnd *	i3UI::FindFrameWndByName( const char * pszName, i3EventReceiver * pDriver)
{
	i3GameNode * pNode = nullptr;

	if( s_pUIManager != nullptr)
	{
		pNode = s_pUIManager->FindChildByName( pszName, false);

		if( pNode == nullptr)
			return nullptr;

		if( !i3::kind_of<i3UIFrameWnd*>(pNode))
			return nullptr;

		if( pDriver != nullptr)
		{
			((i3UIFrameWnd *) pNode)->setEventReceiver( pDriver);
		}
	}

	return (i3UIFrameWnd *) pNode;
}

I3_EXPORT_FRAMEWORK	
void i3UI::EndModal( i3UIControl * pCtrl, i3UIControl * pRecv, I3UI_MODALRESULT result)
{
	if( s_pUIManager != nullptr)
		s_pUIManager->EndModal( pCtrl, pRecv, result);
}

I3_EXPORT_FRAMEWORK		
i3UILibrary* i3UI::getUILibrary(void)
{
	if( s_pUIManager != nullptr)
		return s_pUIManager->getUILibrary();
	return nullptr;
}

I3_EXPORT_FRAMEWORK
void i3UI::setUILibrary( i3UILibrary * pLibrary)
{
	if( s_pUIManager != nullptr)
		return s_pUIManager->setUILibrary( pLibrary);
}

I3_EXPORT_FRAMEWORK
i3VirtualTexture	*	i3UI::getVirtualTexture(void)
{
	if( s_pUIManager != nullptr)
		return s_pUIManager->getVirtualTexture();

	return nullptr;
}

I3_EXPORT_FRAMEWORK
void i3UI::setVirtualTexture( i3VirtualTexture * pTexture, INT32 cx, INT32 cy)
{
	if( s_pUIManager != nullptr)
		s_pUIManager->SetVirtualTexture( pTexture, cx, cy);
}

I3_EXPORT_FRAMEWORK
bool i3UI::OpenVirtualTexture( const char * pszName, INT32 cx, INT32 cy)
{
	if( s_pUIManager != nullptr)
		return s_pUIManager->OpenVirtualTexture( pszName, cx, cy);

	return false;
}

I3_EXPORT_FRAMEWORK
bool i3UI::ResizeVRAMCache( INT32 cx, INT32 cy)
{
	if( s_pUIManager != nullptr)
		return s_pUIManager->ResizeVRAMCache( cx, cy);

	return false;
}

I3_EXPORT_FRAMEWORK
void i3UI::CloseVirtualTexture()
{
	if( s_pUIManager != nullptr)
		s_pUIManager->CloseVirtualTexture();
}

I3_EXPORT_FRAMEWORK
void i3UI::AdjustVirtualTexture(bool isNewFormat)
{
	if( s_pUIManager != nullptr)
		s_pUIManager->AdjustVirtualTexture(isNewFormat);
}

I3_EXPORT_FRAMEWORK
i3UITemplate * i3UI::FindTemplateByName( const char * pszName)
{
	i3UILibrary * pLib = getUILibrary();
	if( pLib != nullptr)
	{
		return pLib->FindByName( pszName);
	}

	I3PRINTLOG(I3LOG_WARN, "설정된 UILibrary가 없습니다.");

	return nullptr;
}

I3_EXPORT_FRAMEWORK
i3UITemplate * i3UI::FindTemplateByHash(UINT128 hash)
{
	i3UILibrary * pLib = getUILibrary();
	if( pLib != nullptr)
	{
		return pLib->FindByHash( hash);
	}

	I3PRINTLOG(I3LOG_WARN, "설정된 UILibrary가 없습니다.");

	return nullptr;
}

I3_EXPORT_FRAMEWORK		
void					i3UI::RegisterLuaFunction(LuaState * pState)
{
	if( s_pUIManager != nullptr)
		s_pUIManager->RegisterLuaFunction( pState);
}

I3_EXPORT_FRAMEWORK	
i3UIControl			*	i3UI::FindControlByName( LuaState * L, const char * pszName)
{
	if( s_pUIManager != nullptr)
		return s_pUIManager->FindChildControlByName( L, pszName);
	return nullptr;
}


I3_EXPORT_FRAMEWORK		
i3Ime				*	i3UI::getGlobalIme(void)
{
	if( s_pUIManager != nullptr)
		return s_pUIManager->getGlobalIme();

	return nullptr;
}

I3_EXPORT_FRAMEWORK		
void					i3UI::setGlobalIme(i3Ime*pIme)
{
	if( s_pUIManager != nullptr)	return s_pUIManager->setGlobalIme( pIme);
}

I3_EXPORT_FRAMEWORK	
i3UIScene	*			i3UI::FindAncestor(LuaState * pState)
{
	if( s_pUIManager != nullptr)	return s_pUIManager->FindAncestorByState(pState);
	
	return nullptr;
}

I3_EXPORT_FRAMEWORK	
i3UIControl* i3UI::FindControlByName( const char* pszName, i3UIScene* pScene)
{
	if( s_pUIManager != nullptr)	return s_pUIManager->FindChildControlByName( pszName, pScene);

	return nullptr;
}

I3_EXPORT_FRAMEWORK	
i3UIControl* i3UI::CreateControl( i3UITemplate* pTemplate, VEC3D* pPos, VEC2D* pSize, i3GameNode* pParent, const char* pszName)
{
	I3ASSERT( pTemplate != nullptr);

	i3ClassMeta* pCtrlMeta = pTemplate->GetInstanceMeta();
	I3ASSERT( i3::kind_of<i3UIControl*>(pCtrlMeta));

	i3UIControl* pControl = (i3UIControl*) pCtrlMeta->create_instance(); //	CREATEINSTANCE( pCtrlMeta);

	if( pControl == nullptr)
	{
		I3ASSERT_0;
		return nullptr;
	}

	pControl->SetTemplate( pTemplate);
	
	pControl->Init();
	pControl->EnableCtrl(true);

	//if( pParent != nullptr)
	I3ASSERT( pParent != nullptr)
	{
		i3UI::SetChildAndParent( pControl, pParent);

		i3UI::SetUniqueName( pControl);
	}

	//Parent가 결정되고 난 후에 setPos()
	pControl->setPos( pPos);
	pControl->setSize( pSize);

	return pControl;
}

I3_EXPORT_FRAMEWORK	
i3UIControl* i3UI::CreateControlByInfo( UI_CONTROL_INFO* pInfo)
{
	char	szName[MAX_PATH];
	VEC2D	vSize;
	VEC3D	vPos;
		
	i3UITemplate* pTemplate = pInfo->m_pTemplate;
	i3GameNode* pParent = pInfo->m_pParent;

	i3Vector::Copy( &vSize, &pInfo->m_vSize);
	i3Vector::Copy( &vPos, &pInfo->m_vPos);
	
	i3::string_ncopy_nullpad( szName, pInfo->m_strName.c_str(), MAX_PATH);
	
	return CreateControl( pTemplate, &vPos, &vSize, pParent, szName);
}

//DeleteControl()로 삭제했던 컨트롤을 다시 살린다.
//이경우에 pControl은 이미 부모를 가지고 있을 수도 있다. 
I3_EXPORT_FRAMEWORK		
i3UIControl* i3UI::ReCreateControl( i3UIControl* pControl, i3GameNode* pParent)
{
	I3ASSERT( pControl != nullptr);

	//Add to parent
	if( pControl->getParent() == nullptr && pParent != nullptr)
	{
		i3UI::SetChildAndParent( pControl, pParent);
	}
	
	//Enable
	pControl->EnableCtrl(true);

	return pControl;
}

void _rec_GetDeleteList( i3::vector<i3UIControl*>& List, i3UIControl* pControl)
{
	i3GameNode* pChild = pControl->getFirstChild();

	while( pChild != nullptr)
	{
		if( i3::kind_of<i3UIControl*>(pChild))
		{
			i3UIControl* pChildCtrl = (i3UIControl*)pChild;
			if( (pChildCtrl->getState() & I3UI_STYLE_SLAVE) == 0 && (pChildCtrl->getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) == 0)
			{
				_rec_GetDeleteList( List, pChildCtrl);
			}
		}

		pChild = pChild->getNext();
	}

	List.push_back( static_cast<i3UIControl*>(pControl) );
}

//Child들도 같이 지운다? ..
I3_EXPORT_FRAMEWORK	
bool i3UI::DeleteControl( i3UIControl* pControl)
{
	I3ASSERT( pControl != nullptr);

	i3GameNode* pParent = pControl->getParent();
	
	if( pParent == nullptr)
	{
		//I3ASSERT_0;
		return false;
	}

	i3::vector<i3UIControl*> list;

	_rec_GetDeleteList( list, pControl);

	for( size_t i=0; i< list.size(); i++)
	{
		i3UIControl* pCtrl = list[i];

		//Disable
		pCtrl->EnableCtrl(false);

		i3GameNode* pParent = pCtrl->getParent();
		I3ASSERT( pParent != nullptr);

		//Remove from parent
		if( i3::same_of< i3UIListView_Item*>(pCtrl))
		{
			if( i3::same_of<i3UIListView_Box* >(pParent))
			{
				i3UIListView_Box* pBox = (i3UIListView_Box*)pParent;
				pBox->UnBindItem();
			}
		}
		else if( i3::same_of<i3UIListView_Header* >(pCtrl))
		{
			if( i3::same_of<i3UIListView_Box* >(pParent))
			{
				i3UIListView_Box* pBox = (i3UIListView_Box*)pParent;
				pBox->UnBindHeader();
			}
		}
		
		if( pCtrl == pControl)
			pParent->RemoveChild( pCtrl);
	}

	return true;
}

I3_EXPORT_FRAMEWORK		
void i3UI::FillControlInfo( UI_CONTROL_INFO* pInfo, i3UIControl* pControl)
{
	I3ASSERT( pControl != nullptr);
	I3ASSERT( pInfo != nullptr);
	
	pInfo->m_pParent = pControl->getParent();		//AddRef? 
	pInfo->m_pTemplate = pControl->getTemplate();
	pInfo->m_strName = pControl->GetNameString();

	i3Vector::Copy( &pInfo->m_vPos, pControl->getPos());
	i3Vector::Copy( &pInfo->m_vSize, pControl->getSize());
}

// pParent == nullptr 이면 pSrc->getParent()에 붙인다. 아니면 pParent에 붙인다. 
// bIncludeChild == false 이면 pSrc만 복사한다. 아니면 pSrc의 child tree를 복사하여 pDest에 붙인다.
// Ex) FrameWindow0
//			├FrameWindow1
//			│		├Button0
//			│		└Button1
//			└FrameWindow2
// CreateCloneControl( FrameWindow0) 이렇게 하면 전부다 복사해서 FrameWindow0의 부모에 붙임
// CreateCloneControl( FrameWindow0, false) 이렇게 하면 FrameWindow0만 복사됨
// CreateCloneControl( FrameWindow0, true, OtherWindow) 이렇게 하면 전부 복사해서 OtherWindow 밑에 붙임
// CreateCloneControl( FrameWindow0, false, OtherWindow) 이렇게 하면 FrameWindow0만 복사해서 OtherWindow 밑에 붙임
I3_EXPORT_FRAMEWORK		
i3UIControl* i3UI::CreateCloneControl( i3UIControl* pSrc, bool bIncludeChild, i3GameNode* pParent)
{
//	i3ClassMeta* pSrcMeta = pSrc->GetMeta();
	i3UIControl* pDest =	pSrc->create_instance();		//	(i3UIControl*)CREATEINSTANCE( pSrcMeta);		// !? => notify to TDK and UndoSystem

	pSrc->CopyTo( pDest, I3_COPY_INSTANCE);

	//Init
	pDest->Init();

	//SetTemplate
	pDest->SetTemplate( pSrc->getTemplate());

	//SetParent
	i3GameNode* pParentOfDest = (pParent == nullptr) ? pSrc->getParent() : pParent;
	i3UI::SetChildAndParent( pDest, pParentOfDest);
			
	//Find a unique name
	i3UI::SetUniqueName( pDest);
	
	//Recursive
	if( bIncludeChild)
	{
		i3GameNode* pChild = pSrc->getFirstChild();

		while( pChild != nullptr)
		{
			if( i3::kind_of< i3UIControl*>(pChild))
			{
				i3UIControl* pChildCtrl = (i3UIControl*)pChild;
				if( IsIndependentControl( pChildCtrl))
				{
					i3UIControl* pCopiedChild = i3UI::CreateCloneControl( pChildCtrl, bIncludeChild, pDest);

					I3ASSERT( pCopiedChild != nullptr);
				}
			}
			
			pChild = pChild->getNext();
		}
	}

	//
	pDest->setSize( pSrc->getSize());

	if( pParent == nullptr)
	{
		I3_MUST_ADDREF( pDest);

		pParent = pDest->getParent();
		if( pParent != nullptr)
		{
			pParent->RemoveChild( pDest);
		}
	}

	return pDest;
}

I3_EXPORT_FRAMEWORK		
void i3UI::SetUniqueName( i3UIControl* pControl)
{
	I3ASSERT( pControl != nullptr);

	char conv[MAX_PATH];

	i3ClassMeta* pMeta = pControl->meta();
	i3UIScene* pScene = pControl->getAncestor();

	i3UI::FindUniqueName( pMeta, pScene, conv, MAX_PATH);
		
	pControl->SetName( conv);
}

//pScene(ancestor)이 정해지지 않은 상태에서는 unique name을 찾을 수 없습니다.
I3_EXPORT_FRAMEWORK		
void i3UI::FindUniqueName( i3ClassMeta* pMeta, i3UIScene* pScene, char* pBuf, INT32 nBufSize)
{
	I3ASSERT( pMeta != nullptr);
	I3ASSERT( pScene != nullptr);
	I3ASSERT( pBuf != nullptr);

	i3::string_ncopy_nullpad( pBuf, pMeta->class_name().c_str(), nBufSize);

	INT32 i = 0;
	while( i3UI::FindControlByName( pBuf, pScene) != nullptr)
	{
		i3::snprintf( pBuf, nBufSize, "%s%d", pMeta->class_name().c_str(), i);
		i++;
	}
}

I3_EXPORT_FRAMEWORK		
void i3UI::SetChildAndParent( i3UIControl* pChild, i3GameNode* pParent)
{
	I3ASSERT( pChild != nullptr);
	I3ASSERT( pParent != nullptr);

	i3UIScene* pAncestor = nullptr;

	if( i3::same_of<i3UIScene* >(pParent))
	{
		i3UIScene* pScene = (i3UIScene*)pParent;
		pScene->AddChild( pChild);

		// Scene에 직접 붙는 FrameWnd의 경우, 항상 Top-Framewnd으로 설정되어 있어야 한다.
		if( i3::kind_of<i3UIFrameWnd *>( pChild))
		{
			i3UIFrameWnd * pFrameWnd = (i3UIFrameWnd *) pChild;

			pFrameWnd->setTopFrameWndState( true);
		}

		pAncestor = pScene;
	}
	else if( i3::same_of<i3UIListView_Box* >(pParent))
	{
		i3UIListView_Box* pBox = (i3UIListView_Box*)pParent;
		pBox->AddControl( pChild);

		if( i3::same_of<i3UIListView_Item* >(pChild))
		{
			i3UIListView_Item* pItem = (i3UIListView_Item*)pChild;
			pBox->BindItem( pItem);
		}
		else if( i3::same_of<i3UIListView_Header* >(pChild))
		{
			i3UIListView_Header* pHeader = (i3UIListView_Header*)pChild;
			pBox->BindHeader( pHeader);
		}
		else
		{
			I3ASSERT_0;
		}

		pAncestor = pBox->getAncestor();
	}
	else if( i3::same_of<i3UIFrameWnd* >(pParent))
	{
		i3UIFrameWnd* pFrameWnd = (i3UIFrameWnd*)pParent;
		pFrameWnd->AddControl( pChild);

		pAncestor = pFrameWnd->getAncestor();
	}
	else
	{
		I3ASSERT( i3::kind_of<i3UIControl* >(pParent));

		i3UIControl* pParentCtrl = (i3UIControl*)pParent;
		pParentCtrl->AddControl( pChild);
		pAncestor = pParentCtrl->getAncestor();
	}

	I3ASSERT( pAncestor != nullptr);
	pChild->setAncestor( pAncestor);

	// i3UIScene의 가장 위 frame을 찾아 로딩완료를 설정해준다. (Scene을 그리기 위해)
	while( pParent)
	{
		if( i3::same_of<i3UIFrameWnd*>(pParent))
		{
			if( i3::same_of<i3UIScene*>(pParent->getParent()))
			{
				((i3UIFrameWnd*)pParent)->addState( I3UI_STATE_WAITLOAD);
				break;
			}
		}

		pParent = pParent->getParent();
	}
}

I3_EXPORT_FRAMEWORK		
bool i3UI::IsIndependentControl( i3UIControl* pControl)
{
	if( pControl != nullptr)
	{
		if( (pControl->getState() & I3UI_STYLE_SLAVE) == 0 &&	//SLAVE가 아니고
			(pControl->getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) == 0 )	// VOLATILE이 아니면
		{
			return true;
		}
	}

	return false;
}

I3_EXPORT_FRAMEWORK		
bool i3UI::DumpFontCache( const char * pszFolder)
{
	i3UITextManager* pTextManager = g_pUIRenderer->getTextManager();
	if( pTextManager != nullptr)
	{
		i3UIFontManager* pFontManager = pTextManager->getFontManager();
		if( pFontManager != nullptr)
		{
			pFontManager->Dump( pszFolder);
			return true;
		}
	}

	return false;
}

I3_EXPORT_FRAMEWORK
void i3UI::DumpSprite( void)
{
#if defined( I3_DEBUG)
	g_pUIRenderer->DumpSprite();
#endif
}

I3_EXPORT_FRAMEWORK
void i3UI::SetCallbackFunction(pfCallback cbFunc) 
{
	m_cb = cbFunc;
}

I3_EXPORT_FRAMEWORK
void i3UI::VirtualTexture_ProgressPos(int nPos)
{
	if (s_bToolDevMode)
	{
		if (m_cb) m_cb(-1, nPos);
	}
}
void i3UI::VirtualTexture_ProgressRange(int nRange)
{
	if (s_bToolDevMode)
	{
		if (m_cb) m_cb(nRange, -1);
	}
}
I3_EXPORT_FRAMEWORK
void i3UI::setTDKPropertyUpdate(bool bUpdate)
{
	s_bPropertyUpdate = bUpdate;
}
I3_EXPORT_FRAMEWORK
bool i3UI::IsTDKPropertyUpdate()
{
	return s_bPropertyUpdate;
}
I3_EXPORT_FRAMEWORK
i3::vector<SERVICECOUNTRY>* i3UI::getCountryList()
{
	return &s_CountryList;
}

