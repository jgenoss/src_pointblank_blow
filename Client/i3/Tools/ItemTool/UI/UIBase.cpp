#include "stdafx.h"
#include "UIBase.h"
#include "i3UIScene.h"

#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/compare/generic_is_iequal.h"

using namespace std;

UISCENEINFO::UISCENEINFO() 
{ 
	UISCENEINFO::Reset(); 
}

UISCENEINFO::~UISCENEINFO() 
{ 
	UISCENEINFO::Reset(); 
}  

void	UISCENEINFO::Reset( void)
{
	_szPath[0] = 0;
	_pScene = NULL;
	_GlueFunc = NULL;
	_EnableScn = false;
	_LoadedStyle = false;


}


static i3UITemplate * s_pToolTipTemplate = NULL;



//------------------------------------------------------------------------------//
//						Default Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	/*int DefaultUI_PlaySound( LuaState * L)
	{
		// Play Sound
		UIBase * pThis = GetThis( L);
		if( pThis != NULL)
		{
			UISOUND_TYPE type = (UISOUND_TYPE) i3Lua::GetIntegerArg( L, 1);

			pThis->PlaySound( type);
		}

		return 0;
	}

	int DefaultUI_InitCaption( LuaState * L)
	{
		return 0;
	}

	int DefaultUI_EnableScene( LuaState * L)
	{
		UIBase * pThis = GetThis( L);
		if( pThis != NULL)
		{
			BOOL bVisible = i3Lua::GetBooleanArg( L, 1);

			pThis->SetVisible( bVisible);
		}
		return 0;
	}*/

	int DefaultUI_GAME_STRING( LuaState* L)
	{
		//const char* cstr_input = lua_tostring(L, 1);			// 키문자열이므로 영문자가 확실하고, 변환과정이 딱히 필요한것은 아니다..(이건 지금은 UTF-8이다..)

		//const char* cstr_res = GAME_STRING(cstr_input);			// 현재, 멀티바이트형임..
		// 효율성을 위해서는 엔진쪽에서 멀티바이트를 그냥 받아들일수 있는 함수를 세우는게 좋겠다..
		//i3Lua::PushString(L, cstr_res);							// 변환없이 막바로 보낸다..
		return 1;
	}
	int DefaultUI_GAME_STRING_U8( LuaState* L)
	{		
		//const char* cstr_input = lua_tostring(L, 1);			// 키문자열이므로 영문자가 확실하고, 변환과정이 딱히 필요한것은 아니다..(이건 지금은 UTF-8이다..)
		//const char* cstr_res = GAME_STRING(cstr_input);			// 현재, 멀티바이트형임..
		//i3Lua::PushA2UTF8String(L, cstr_res);
		return 1;
	}
	int DefaultUI_GAME_STRING_PTR( LuaState* L)
	{		
		//const char* cstr_input = lua_tostring(L, 1);			// 키문자열이므로 영문자가 확실하고, 변환과정이 딱히 필요한것은 아니다..(이건 지금은 UTF-8이다..)
		//const char* cstr_res = GAME_STRING(cstr_input);			// 현재, 멀티바이트형임..
		//i3Lua::PushUserData(L, (const_cast<char*>( (cstr_res) )));
		return 1;
	}
}



namespace
{
	struct init
	{
		init() 
		{
			i3UIScene::PreRegisterLuaFunction("GAME_STRING", DefaultUI_GAME_STRING);
			i3UIScene::PreRegisterLuaFunction("GAME_STRING_U8", DefaultUI_GAME_STRING_U8);
			i3UIScene::PreRegisterLuaFunction("GAME_STRING_PTR", DefaultUI_GAME_STRING_PTR);
		}
	}_;
}


//------------------------------------------------------------------------------//

//I3_ABSTRACT_CLASS_INSTANCE( cUIBase, i3GameNode);
I3_CLASS_INSTANCE( cUIBase); //, i3GameNode);

cUIBase::cUIBase() : m_bInputDisable(false)
{
	m_UIStyle = 0;

	m_rAwayInputTime = 0.f;
	m_rCautionTime = 0.f;
	m_bOffAwayInput = false;
	m_bOffCaution = false;
	
	m_pLuaContextName[0] = NULL;

	m_pLastMouseOverCtrl = NULL;
}

cUIBase::~cUIBase()
{
	__ClearScene();
}

void cUIBase::__FindToolTipTemplate( void)
{
	if( s_pToolTipTemplate == NULL)
	{
		i3UILibrary * pLib = i3UI::getUILibrary();
		if( pLib != NULL)
		{
			s_pToolTipTemplate = pLib->FindByName( "ToolTip");
//			I3ASSERT( s_pToolTipTemplate != NULL);
		}
	}
}
 
UISCENEINFO& cUIBase::__RegisterScene( const char * pszScenePath, const LuaFuncReg * GlueFunc /*= NULL*/, bool bSceneEnable /*= true*/ )
{
	I3ASSERT( pszScenePath != NULL);

//	UISCENEINFO * pInfo = (UISCENEINFO*) i3MemAlloc( sizeof( UISCENEINFO));
//	pInfo->Reset();
	UISCENEINFO		Info;

	Info.Reset();
	i3::safe_string_copy( Info._szPath, pszScenePath, MAX_PATH );
	Info._GlueFunc	= (LuaFuncReg*)(GlueFunc);
	Info._EnableScn	= bSceneEnable;

	m_UISceneList.push_back( Info );

	return ( *m_UISceneList.rbegin() );
}

static void RecSetEventReceiverToControl( i3UIControl * pControl, i3EventReceiver * pReceiver)
{
	i3UIControl * pChild = (i3UIControl*) pControl->getFirstChild();

	while( pChild)
	{
		if( i3::kind_of<i3UIControl*>(pChild)) //->IsTypeOf( i3UIControl::GetClassMeta()))
		{
			pChild->setEventReceiver( pReceiver);
		}

		RecSetEventReceiverToControl( pChild, pReceiver);

		pChild = (i3UIControl*) pChild->getNext();
	}
}

void cUIBase::__LoadScene( void)
{
	vector<UISCENEINFO>::iterator		iterInfo	= m_UISceneList.begin();
	vector<UISCENEINFO>::const_iterator	iterEnd		= m_UISceneList.end();

	for(; iterInfo != iterEnd; iterInfo++)
	{
		if( iterInfo->_pScene == NULL)
		{
			__DirectlyLoadScene( *iterInfo );
		}
		else
		{
			iterInfo->_pScene->EnableScene( true);
		}
	}
	
	OnLoadAllScenes();

}

/*virtual*/ i3UIScene *	cUIBase::_LoadScene( const char * pszScenePath, LuaFuncReg * GlueFunc, bool bSceneEnable)
{
	VEC3D vZero;
	VEC2D vSize;

	i3Vector::Zero( &vZero);
	i3Vector::Zero( &vSize);

	I3ASSERT( i3UI::getManager() != NULL);

	char szPath[ MAX_PATH];

#if defined( I3_DEBUG) && 0
	i3::safe_string_copy( szPath, pszScenePath);
#else
	char szTemp[ MAX_PATH];
	char szFileName[ MAX_PATH];

//	i3String::SplitPath( pszScenePath, szTemp);
//	i3String::SplitFileName( pszScenePath, szFileName, FALSE);
//	i3::snprintf( szPath, MAX_PATH, "%s/%s.i3UIs", szTemp, szFileName);
	i3::change_file_ext_copy(szPath, pszScenePath, ".i3UIs");

	if( i3FileStream::isFile( szPath) == false)
	{	// i3UIE를 읽자
		i3::snprintf( szPath, MAX_PATH, "%s/%s.i3UIe", szTemp, szFileName);
	}

#endif

	i3UIScene* pUIScene = i3UI::getManager()->LoadUIScene( szPath);

	if( pUIScene != NULL)
	{
		pUIScene->Attach( TRUE);
		
		__FindToolTipTemplate();

		if( s_pToolTipTemplate != NULL)
		{
			i3UIToolTip * pToolTip = (i3UIToolTip*) i3UI::CreateControl( s_pToolTipTemplate, &vZero, &vSize, pUIScene);
			pUIScene->setDefToolTip( pToolTip);
		}
		
		pUIScene->EnableScene(bSceneEnable); // (true);  고정된 true인자대신에 함수 인수로 true를 지정하게 합니다...

		//Register Lua Glue Functions
		LuaState* L = pUIScene->getLuaContext();
		if( L != NULL)
		{
			const char* szScript = pUIScene->getScriptFile();
			pUIScene->getWidth();
			if( m_pLuaContextName[0] == 0)
			{
			//	i3String::SplitFileName( szScript,	m_pLuaContextName, FALSE);
				i3::extract_filetitle(szScript,	m_pLuaContextName);
			}
			//_RegisterLuaFunctions( L, GlueFunc);
		}
	}

	return pUIScene;
}

void cUIBase::__DirectlyLoadScene( UISCENEINFO& Info )
{
	i3UIScene * pScene = _LoadScene( Info._szPath, Info._GlueFunc, Info._EnableScn );

	I3_REF_CHANGE( Info._pScene, pScene);
	
	
	// Lua Initialize callback을 호출합니다.
	// 또는 하위 Class에서 Control을 찾는 Code를 여기에 Define합니다.
	_InitializeAtLoad( Info._pScene);

	RecSetEventReceiverToControl( (i3UIControl*)Info._pScene, this);
}

void cUIBase::__UnloadScene( void)
{
	OnUnloadAllScenes();			// 씬리스트 소거 전에 해주는게 좋겠다..

	vector<UISCENEINFO>::iterator		iterInfo	= m_UISceneList.begin();
	vector<UISCENEINFO>::const_iterator	iterEnd		= m_UISceneList.end();

	for(; iterInfo != iterEnd; iterInfo++)
	{		
		if( iterInfo->_LoadedStyle == false)
		{
			__UnloadScene( iterInfo->_pScene );
			iterInfo->_pScene	= NULL;
		}
		else if(iterInfo->_pScene != NULL)
		{
			iterInfo->_pScene->EnableScene(false);				
		}
	}
}

static bool sRecFindChild( i3GameNode * pNode)
{
	I3ASSERT( g_pMainSCFramework->getFocus() != NULL);

	if( g_pMainSCFramework->getFocus() == pNode)
		return true;

	i3GameNode * pChild = pNode->getFirstChild();

	while( pChild != NULL)
	{
		bool bRv = sRecFindChild( pChild);

		if( bRv)
			return true;

		pChild = pChild->getNext();
	}

	return false;
}

void cUIBase::__UnloadScene( i3UIScene * pScene)
{
	if( pScene != NULL)
	{
		pScene->EnableScene( false);

		if( i3UI::getManager() != NULL)
		{
			i3UI::getManager()->RemoveScene( pScene);
			pScene->Detach();

			if( g_pMainSCFramework->getFocus() != NULL)
			{
				if( sRecFindChild( pScene) ==  true)
					g_pMainSCFramework->SetFocus( NULL);
			}
		}

		RecSetEventReceiverToControl( (i3UIControl*)pScene, NULL);

		_ClearAtUnload( pScene);

		I3NOTICE("cUIBase::__UnloadScene(), UIName = %s\n", pScene->GetName());
		I3_SAFE_RELEASE( pScene);
	}
}

void cUIBase::__ClearScene( void)
{
	m_UISceneList.clear();
}



/*static*/ void	cUIBase::_RegisterLuaFunctions( LuaState* L, LuaFuncReg * GlueFunc)
{
	INT32 i;

	if( L == NULL) return;

/*	for( i = 0; DefaultUI_Glue[i].name; i++)
	{
		i3Lua::RegisterFunc( L, DefaultUI_Glue[i].name, DefaultUI_Glue[i].func);
	}
*/
	if( GlueFunc != NULL)
	{
		for( i = 0; GlueFunc[i].name; i++)
		{
			i3Lua::RegisterFunc( L, GlueFunc[i].name, GlueFunc[i].func);
		}
	}
}


/*virtual*/ void cUIBase::_InitializeAtLoad( i3UIScene * pScene)
{
	I3ASSERT( pScene != NULL);

	if( m_pFrameWnd == NULL)
	{
		m_pFrameWnd = (i3UIFrameWnd*) pScene->getFirstChild();
	}
}

/*virtual*/ void cUIBase::_ClearAtUnload( i3UIScene * pScene)
{
	m_pFrameWnd = NULL;
}

i3GameNode * cUIBase::_FindListBoxFromCombo( i3GameNode * pNode)
{
	i3GameNode * pChild = pNode->getFirstChild();
	i3GameNode * pResult;

	while( pChild != NULL)
	{
		if( i3::same_of<i3UIListBox*>(pChild)) //->IsExactTypeOf( i3UIListBox::GetClassMeta()))
			return pChild;

		pResult = _FindListBoxFromCombo( pChild);

		if( pResult != NULL)
			return pResult;

		pChild = pChild->getNext();
	}

	return NULL;
}


i3UIScene *	cUIBase::AddScene( const char * pszScenePath, const LuaFuncReg * GlueFunc /*= NULL*/, bool bLoaded /*= false*/, bool bSceneEnable /*=true*/ )
{
	UISCENEINFO&	Info	= __RegisterScene( pszScenePath, GlueFunc, bSceneEnable );

	if( bLoaded)
	{
		//UISCENEINFO * pInfo = m_UISceneList.Items[ m_UISceneList.size() - 1];

		Info._GlueFunc		= (LuaFuncReg*) GlueFunc;
		Info._EnableScn		= bSceneEnable;
		Info._LoadedStyle	= true;

		__DirectlyLoadScene( Info );
	
		return Info._pScene;
	}

	return NULL;
}


/*virtual*/ void cUIBase::OnCreate( i3GameNode * pParent)
{
	I3_GAMENODE_ADDCHILD( pParent, this);

	i3GameNode::SetEnable( FALSE);
}

void cUIBase::Destroy( void)
{
	SetEnable( FALSE);

	__ClearScene();

	if( getParent())
	{
		getParent()->RemoveChild( this);
	}

	I3ASSERT( GetRefCount() == 1);
	Release();
}

/*virtual*/ void cUIBase::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameNode::OnUpdate( rDeltaSeconds);
}

/*virtual*/ BOOL cUIBase::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if( event == I3_EVT_UI_NOTIFY)
		{
			if( pObj != NULL )
			{
				I3UI_CONTROL_NOTIFY * pNotify = (I3UI_CONTROL_NOTIFY*) param2;
				i3UIControl * pCtrl = pNotify->m_pCtrl;

				if( pCtrl != NULL)
				{
					switch( pNotify->m_nEvent)
					{	
					case I3UI_EVT_ENTERMOUSE :
						if( OnEnterMouse( pCtrl) == true)
							return TRUE;
						break;
					case I3UI_EVT_LEAVEMOUSE :	OnLeaveMouse( pCtrl);	return TRUE;
					case I3UI_EVT_CLICKED :
						if( OnClick( pCtrl) == true)
							return TRUE;
						break;
					case I3UI_EVT_R_CLICKED :	OnRClick( pCtrl);		return TRUE;
					case I3UI_EVT_MOUSEWHEEL :	OnWheel( pCtrl, pNotify->m_nPriParam);	return TRUE;
					case I3UI_EVT_COMPLETE_LOAD : OnCompleteLoad();		return TRUE;
					case I3UI_EVT_DBL_CLICKED :
						if( OnDblClick( pCtrl) == true)
							return TRUE;
						break;
					}
				}
			}
		}
		else if( event == I3_EVT_INPUT)
		{
			if (i3UI::getDisableUIInput() )	// 이 경우엔 무조건 막음..
				return TRUE;
			
			if ( IsInputDisable() )		// 인풋이 막히면 일단 해당Input은 넘김..
				return FALSE;
			
			if( OnKeyInput( (i3InputDeviceManager*)pObj))
				return TRUE;
		}
	}

	return FALSE;
}

/*virtual*/ bool cUIBase::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();
	i3InputMouse* pMouse = pInputMng->GetMouse();

	if( pKeyboard->GetStrokeState( I3I_KEY_ESC ) && I3I_MOUSE_LBUTTON == pMouse->GetButtonState() )
	{
		return true;
	}


	if( pKeyboard->GetStrokeState( I3I_KEY_ESC))
	{
		if( OnKey_Escape())
			return true;
	}

	if( pKeyboard->GetStrokeState( I3I_KEY_ENTER))
	{
		if( OnKey_Enter())
			return true;
	}

	return false;
}

/*virtual*/ bool cUIBase::OnEnterMouse( i3UIControl * pControl)
{
	if( (pControl != NULL) && pControl->isActivated())
	{
		if( i3::same_of<i3UIButton*>(pControl) //->IsExactTypeOf( i3UIButton::GetClassMeta()) 
			|| i3::same_of<i3UIButtonComposed3*>(pControl) //->IsExactTypeOf( i3UIButtonComposed3::GetClassMeta()) 
			|| i3::same_of<i3UIListView_Item*>(pControl)) //->IsExactTypeOf( i3UIListView_Item::GetClassMeta()) )
		{
			if( m_pLastMouseOverCtrl != pControl &&
				pControl->isActivated())
			{
				/*if( !i3::same_of<i3UIListView_Item*>(pControl)) //->IsExactTypeOf( i3UIListView_Item::GetClassMeta()) == FALSE)
					PlaySound( UISND_ENTER_MOUSE2);*/

				m_pLastMouseOverCtrl = pControl;
			}
		}

		return true;
	}

	return false;
}

/*virtual*/ void cUIBase::OnLeaveMouse( i3UIControl * pControl)
{
	if( i3::same_of<i3UIButton*>(pControl) //->IsExactTypeOf( i3UIButton::GetClassMeta()) 
		|| i3::same_of<i3UIButtonComposed3*>(pControl)) //->IsExactTypeOf( i3UIButtonComposed3::GetClassMeta()) )
	{
		m_pLastMouseOverCtrl = NULL;
	}
}

/*virtual*/ bool cUIBase::OnClick( i3UIControl * pControl)
{
	bool bValid = false;

	if( pControl != NULL && pControl->isActivated())
	{
		/*if(  i3::same_of<i3UIButton*>(pControl) //->IsExactTypeOf( i3UIButton::GetClassMeta()) 
			|| i3::same_of<i3UIButtonComposed3*>(pControl)) //->IsExactTypeOf( i3UIButtonComposed3::GetClassMeta()) )
		{
			PlaySound( UISND_CLICK);
		}
		else if( i3::same_of<i3UIButtonImageSet*>(pControl)) //->IsExactTypeOf( i3UIButtonImageSet::GetClassMeta()) )
		{
			// 버튼이미지셋의 경우, UI입력을 받지 않는 경우를 추가로 걸러내는게 좋겠다.(11.10.11.수빈)
			i3UIButtonImageSet* pimgset = static_cast<i3UIButtonImageSet*>(pControl);
			if( pimgset->IsViewOnly() == false)
				PlaySound( UISND_CLICK);
		}
		else if( i3::same_of<i3UITab*>(pControl)) //->IsExactTypeOf( i3UITab::GetClassMeta()))
		{
			PlaySound( UISND_LIST_OVER);
		}*/

		bValid = true;
	}
/*
	if( g_pMainSCFramework->GetUIMainframe() != NULL)
	{
		g_pMainSCFramework->GetUIMainframe()->ClosePopupByAnotherControlInput( pControl);
	}
*/
	return bValid;
}

/*virtual*/ void cUIBase::OnRClick( i3UIControl * pControl)
{
	
}

/*virtual*/ bool cUIBase::OnDblClick( i3UIControl * pControl)
{
	if( pControl != NULL && pControl->isActivated())
		return true;

	return false;
}

/*virtual*/ void cUIBase::SetEnable( BOOL bTrue /*= TRUE*/, REAL32 tm /*= 0.f*/)
{
	if ( i3GameNode::isEnable() != bTrue)
	{
		i3GameNode::SetEnable( bTrue, tm);

		if( bTrue)
			__LoadScene();
		else
			__UnloadScene();

		// 
		i3UIManager * pManager = i3UI::getManager();
		if( pManager != NULL)
			pManager->delActiveFrameWnd();
	}
}

/*virtual*/ void cUIBase::SetVisible( BOOL bTrue /*= TRUE*/, REAL32 tm /*= 0.f*/)
{
	if( i3GameNode::isVisible() != bTrue)
	{
		i3GameNode::SetVisible( bTrue, tm);

		vector<UISCENEINFO>::iterator		iterInfo	= m_UISceneList.begin();
		vector<UISCENEINFO>::const_iterator	iterEnd		= m_UISceneList.end();

		for(; iterInfo != iterEnd; iterInfo++)
		{
			if( iterInfo->_pScene != NULL)
			{
				iterInfo->_pScene->EnableScene( bTrue ? true : false );
			}
		}
	}
}

i3UIScene* cUIBase::GetScene( const char * pszSceneName)
{
	vector<UISCENEINFO>::iterator		iterInfo	= m_UISceneList.begin();
	vector<UISCENEINFO>::const_iterator	iterEnd		= m_UISceneList.end();

	for(; iterInfo != iterEnd; iterInfo++ )
	{
		if( iterInfo->_pScene != NULL)
		{
			if( i3::generic_is_iequal( iterInfo->_pScene->GetName(), pszSceneName) )
				return iterInfo->_pScene;
		}
	}

	return NULL;
}

i3UIScene* cUIBase::GetScene( INT32 idxScene) const
{
	return m_UISceneList.at(idxScene)._pScene;
}

bool		cUIBase::IsEnableFromSceneInfo(INT32 idxScene) const
{
	return m_UISceneList.at(idxScene)._EnableScn;
}

void		cUIBase::SetEnableToSceneInfo(INT32 idxScene, bool bEnable)
{
	m_UISceneList.at(idxScene)._EnableScn	= bEnable;
}


static void RecInputDisable( i3GameNode * pNode, bool bDisable)
{	
	i3GameNode * pChild = pNode->getFirstChild();

	while( pChild != NULL)
	{
		if( i3::kind_of<i3UIControl*>(pChild)) //->IsTypeOf( i3UIControl::GetClassMeta()))
			((i3UIControl*)pChild)->setInputDisable( bDisable);

		RecInputDisable( pChild, bDisable);

		pChild = pChild->getNext();
	}

}

void cUIBase::SetInputDisable( bool bDisable)
{
	vector<UISCENEINFO>::iterator		iterInfo	= m_UISceneList.begin();
	vector<UISCENEINFO>::const_iterator	iterEnd		= m_UISceneList.end();

	for(; iterInfo != iterEnd; iterInfo++ )
	{
		if( iterInfo->_pScene != NULL)
		{
			RecInputDisable( iterInfo->_pScene, bDisable);
		}
	}
	
	m_bInputDisable = bDisable;
}


bool cUIBase::IsLuaContext( LuaState * L)
{
	vector<UISCENEINFO>::iterator		iterInfo	= m_UISceneList.begin();
	vector<UISCENEINFO>::const_iterator	iterEnd		= m_UISceneList.end();

	for(; iterInfo != iterEnd; iterInfo++ )
	{
		if( iterInfo->_pScene != NULL)
		{
			if( iterInfo->_pScene->getLuaContext() == L)
			{
				return true;
			}
		}
	}

	return false;
}

static bool Rec_FindChild( i3GameNode * pNode, i3GameNode * pDest)
{
	if( pNode == pDest)
		return true;

	i3GameNode * pChild = pNode->getFirstChild();

	while( pChild)
	{
		if( Rec_FindChild( pChild, pDest))
			return true;

		pChild = pChild->getNext();
	}

	return false;
}

bool cUIBase::IsIncludeControl( i3UIControl * pControl)
{
	vector<UISCENEINFO>::iterator		iterInfo	= m_UISceneList.begin();
	vector<UISCENEINFO>::const_iterator	iterEnd		= m_UISceneList.end();

	for( ; iterInfo != iterEnd; iterInfo++ )
	{
		if( iterInfo->_pScene != NULL)
		{
			if( Rec_FindChild( iterInfo->_pScene, pControl ))
			{
				return true;
			}
		}
	}

	return false;
}



/*virtual*/ bool cUIBase::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( isEnable() == FALSE)
	{
		SetEnable( TRUE);

		return true;
	}

	// 다시 열수 없는 타입이면 false
	if( isReopenStyle() == false)
		return false;

	// 다시 설정하는 타입이다..
	return true;
}

/*virtual*/ bool cUIBase::OnEntrancing( REAL32 rDeltaSeconds)
{
	return true;
}

/*virtual*/ void cUIBase::OnEntranceEnd( void)
{
	SetInputDisable( false);
}
 
/*virtual*/ bool cUIBase::OnExitStart(void)
{
	if( isEnable() == FALSE )
		return false;

	return true;
}

/*virtual*/ bool cUIBase::OnExiting( REAL32 rDeltaSeconds)
{
	return true;
}

/*virtual*/ void cUIBase::OnExitEnd( void)
{
	SetEnable( FALSE);
	SetInputDisable( true);
}

void	cUIBase::OnEscapeKey( void)
{
	/*if( LocaleValue::Enable("EnableGameEndPopup") == FALSE)
	{
		_OpenExitMessageBox();
	}		
	else
	{
		if( i3::same_of<UIMainFrame*>(m_pParent)) //->IsExactTypeOf( UIMainFrame::GetClassMeta())) // MainFrame이 UI일경우. 즉(로그인화면 X) 서버Select부터...레디룸까지
		{
			UIMainFrame * MainFrame = (UIMainFrame*) m_pParent;
			UIExitPopup * t = (UIExitPopup *)MainFrame->GetPopup(UPW_EXIT_POPUP);
			if( t != NULL && MainFrame->GetCurrentPhaseType() >= UIPHASE_LOBBY)
			{
				if( MainFrame->GetCurrentPhaseState() == PS_PLAYING )
				{
					g_pGameContext->SetEvent(EVENT_EXIT_USERINFO);
					((UIMainFrame*) m_pParent)->TogglePopup(UPW_EXIT_POPUP);
				}
			}
			else
			{
				_OpenExitMessageBox();
			}
		}
		else // 로그인 창에서 들어온다.
		{
			_OpenExitMessageBox();
		}
	}

	if ( g_pConfigEx->GetMisc().webAnnounce.IsActive() )
	{
		REAL32 widthRate = i3UI::getUIResolutionWidth();
		REAL32 heightRate = i3UI::getUIResolutionHeight();

		POINT2D ptCenter = g_pMainSCFramework->GetCenterPosition( m_pExitMessageBox->GetWidth(), m_pExitMessageBox->GetHeight());
		ptCenter.x += static_cast<int>(static_cast<REAL32>(g_pConfigEx->GetMisc().webAnnounce.msgBoxOffsetX) * widthRate);
		ptCenter.y += static_cast<int>(static_cast<REAL32>(g_pConfigEx->GetMisc().webAnnounce.msgBoxOffsetY) * heightRate);
		if( LocaleValue::Enable("EnableGameEndPopup") == FALSE)
		{

		//#if !defined(NEW_GAME_END)
		m_pExitMessageBox->SetPosition(ptCenter.x, ptCenter.y);
		//#endif

		}
	}*/
}

bool cUIBase::isInputKeyboardEnter( void)
{
	bool bInputDisable = false;
	if( m_pFrameWnd)
	{
		bInputDisable = m_pFrameWnd->isIgnoreInput();
	}

	//if( bInputDisable == false) 
	//	return g_pMainSCFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ) ? true : false;

	return false;
}