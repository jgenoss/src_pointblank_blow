#include "pch.h"
#include "UIBase.h"

#include "UIMainFrame.h"
#include "UIUtil.h"
#include "UISideMenu.h"

#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "BattleSlotContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#include "i3Framework/i3UI/i3UIFilePathMgr.h"


//#define i3_profile_enable
#include "i3Base/profile/profile.h"

const REAL32 s_BGSizeWidth	= 1360.f;
const REAL32 s_BGSizeHeight	= 768.f;

namespace UILua
{
	LuaState *		CallLuaFunction( i3UIScene* pScene, const char* pszFunction)
	{
		if( pScene == nullptr)
		{// nullptr일 수 있다.
			// 해제 후에 Lua에서 이벤트가 오는 경우..
			//I3PRINTLOG(I3LOG_FATAL,  "Could open UIScene file. %s\n", pScene);
			//I3TRACE( "Could open UIScene file. %s\n", pScene);
			return nullptr;
		}

		LuaState* L = pScene->getLuaContext();

		if( L == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Not yet binding script..%s. %s!!!!", pScene->GetName(), pszFunction);
			return nullptr;
		}

		i3Lua::GetGlobal( L, pScene->GetName());
		// Lua Function
		i3Lua::GetField( L, -1, pszFunction);
		INT32 top = i3Lua::StackGetTop( L);
		//i3Lua::PushCFunction( L, i3Lua::TraceCallStack);
		i3Lua::StackInsert( L, top);

		i3Lua::GetGlobal( L, pScene->GetName());
		i3Lua::StackInsert( L, -1);

#if defined( I3_DEBUG)
		//I3TRACE( "CallLuaFunction(%s, %s)\n", pScene->GetName(), pszFunction);
#endif

		return L;
	}

	void			EndLuaFunction( LuaState* L, INT32 nArg )
	{
		if( L)
		{
			i3Lua::PCall( L, nArg+1, 1, 0);
			i3Lua::StackPop( L, -1);
		}
	}

	bool			ReturnLuaBoolean( LuaState* L, INT32 nArg)
	{
		bool bRv = false;

		if( L)
		{
			i3Lua::PCall( L, nArg+1, 1, 0);
			INT32 top = i3Lua::StackGetTop( L);
			if( i3Lua::StackIsType( L, top) == I3LUA_BOOLEAN)
			{
				bRv = i3Lua::StackToBoolean( L, top) ? true : false;
			}
			i3Lua::StackPop( L, 1);
		}

		return bRv;
	}

	REAL32			ReturnLuaNumber( LuaState * L, INT32 nArg)
	{
		REAL32 rv = 0;

		if( L)
		{
			i3Lua::PCall( L, nArg+1, 1, 0);
			INT32 top = i3Lua::StackGetTop( L);
			if( i3Lua::StackIsType( L, top) == I3LUA_NUMBER)
			{
				rv = i3Lua::StackToNumber( L, top);
			}
			i3Lua::StackPop( L, 1);
		}

		return rv;
	}

	void			ReturnLuaVec2D( VEC2D * pOut, LuaState * L, INT32 nArg)
	{
		I3ASSERT( pOut);

		if( L)
		{
			i3Lua::PCall( L, nArg+1, 2, 0);

			INT32 top = 0;
			top = i3Lua::StackGetTop( L);
			if( i3Lua::StackIsType( L, top) == I3LUA_NUMBER)
			{
				setX( pOut, i3Lua::StackToNumber( L, top));
				i3Lua::StackPop( L, 1);
			}
			top = i3Lua::StackGetTop( L);
			if( i3Lua::StackIsType( L, top) == I3LUA_NUMBER)
			{
				setY( pOut, i3Lua::StackToNumber( L, top));
				i3Lua::StackPop( L, 1);
			}
		}
	}

	INT32			ReturnLuaInteger( LuaState* L, INT32 nArg)
	{
		INT32 res = 0;

		if ( L )
		{
			i3Lua::PCall(L, nArg+1, 1, 0);
			INT32 top =  i3Lua::StackGetTop( L);
			if( i3Lua::StackIsType( L, top) == I3LUA_NUMBER)
			{
				res = static_cast<INT32>(i3Lua::StackToNumber(L, top));
			}
			i3Lua::StackPop( L, 1);
		}
		return res;
	}


	const char*		ReturnLuaString( LuaState* L, INT32 nArg)
	{
		char * pstr = nullptr;
		if( L)
		{
			i3Lua::PCall( L, nArg+1, 1, 0);
			INT32 top = i3Lua::StackGetTop( L);

			if( i3Lua::StackIsType( L, top) == I3LUA_STRING)
			{
				pstr = (char*) i3Lua::StackToString( L, top);
			}
			i3Lua::StackPop( L, 1);
		}

		return (const char*) pstr;
	}

	void *			ReturnLuaPointer( LuaState* L, INT32 nArg)
	{
		void * pstr = nullptr;
		if( L)
		{
			i3Lua::PCall( L, nArg+1, 1, 0);
			INT32 top = i3Lua::StackGetTop( L);

			if( i3Lua::StackIsType( L, top) == LUA_TLIGHTUSERDATA)		// LUA_TUSERDATA로 작동되지 않아, 타입을 이것으로 합니다.(11.06.30.수빈)
			{
				pstr = (void*) i3Lua::StackToPointer( L, top);
			}
			i3Lua::StackPop( L, 1);
		}
		return pstr;
	}

	void			RegisterLuaFunctions( i3UIScene* pScene, LuaFuncReg * GlueFunc)
	{
		if( pScene == nullptr)
		{
			//I3TRACE( "Could open UIScene file. %s\n", pScene);
			return ;
		}

		LuaState* L = pScene->getLuaContext();
		if( L == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Not yet binding script..%s. !!!!", pScene->GetName() );
			return ;
		}
		
		RegisterLuaFunctions(L, GlueFunc);
	}

	void RegisterLuaFunctions(LuaState* L, LuaFuncReg * GlueFunc)
	{
		INT32 i;

		if( L == nullptr) return;

		if( GlueFunc != nullptr)
		{
			for( i = 0; GlueFunc[i].name; i++)
			{
				i3Lua::RegisterFunc( L, GlueFunc[i].name, GlueFunc[i].func);
			}
		}	
	}

}


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
	_pScene = nullptr;
	_GlueFunc = nullptr;
	_EnableScn = false;
	_LoadedStyle = false;


}

//static List s_UISoundList;

static char * s_szUISoundFile[ UISND_MAX] = 
{
	"UI_Scroll_In.wav",
	"UI_Scroll_Out.wav",
	"UI_PopUp_Open.wav",
	"UI_PopUp_Close.wav",
	"UI_Button_Click.wav",
	"UI_Button_Click_Ok.wav",
	"UI_Button_Click_Cancel.wav",
	"PB_15ver_Equip_on.wav",
	"PB_15Ver_Error.wav",
	"PB_15ver_M_Click.wav",
	"UI_MOUSE_ON1.wav",
	"UI_MOUSE_ON2.wav",
	"UI_PAGE_SHIFT.wav",
	"UI_POPUP_FADE_IN.wav",
	"UI_SERVER_CONNECT.wav",
	"UI_SERVER_ENTER.wav",
	"UI_WARNING.wav",
	"UI_List_Over.wav",
	"UI_Mouse_Over.wav",
	"UserGuide_Start.wav",
	"UserGuide_Coin.wav",
	"UserGuide_Start4.wav",
};

static i3UITemplate * s_pToolTipTemplate = nullptr;

//------------------------------------------------------------------------------//
//								Callback rutine									//
//------------------------------------------------------------------------------//
void UIBase::CbExitNormal(void* pThis,INT32 nParam)
{
	if (nParam != MBR_OK)
	{
		UIBase* p = reinterpret_cast<UIBase*>(pThis);
		p->m_pExitMessageBox = nullptr;
	}
}

//------------------------------------------------------------------------------//
/**
	\note 모든 UI Scene 파일에서 찾게 된다.
		느릴 수 있으므로 되도록이면 사용하지 말고 직접 접근 하는 방식을 사용하도록!!
	\see GameUI::GetPhase, GameUI::GetFloating, GameUI::GetPopup
*/
static UIBase *	GetThis( LuaState * L)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	UIBase * pThis = pFrame->FindUIBase( L);

	return pThis;
}

//------------------------------------------------------------------------------//
//						Default Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int DefaultUI_PlaySound( LuaState * L)
	{
		// Play Sound
		UIBase * pThis = GetThis( L);
		if( pThis != nullptr)
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
		if( pThis != nullptr)
		{
			bool bVisible = i3Lua::GetBooleanArg( L, 1);

			pThis->SetVisible( bVisible);
		}
		return 0;
	}

	int DefaultUI_GAME_STRING( LuaState* L)
	{
		const char* cstr_input = lua_tostring(L, 1);			
		const i3::rc_wstring& wstrGameString = GAME_RCSTRING(cstr_input);
		i3Lua::PushStringUTF16To8(L, wstrGameString);
		return 1;
	}

	int DefaultUI_GAME_STRING_PTR( LuaState* L)
	{		
		const char* cstr_input = lua_tostring(L, 1);			
		const i3::rc_wstring& wstrGameString = GAME_RCSTRING(cstr_input);
		i3Lua::PushUserData(L, (const_cast<wchar_t*>( wstrGameString.c_str() )));		
		return 1;
	}
}

LuaFuncReg DefaultUI_Glue[] = {
	{"uiInitCaption",			DefaultUI_InitCaption},
	{"PlaySound",				DefaultUI_PlaySound},
	{"EnableScene",				DefaultUI_EnableScene},
	{nullptr,		nullptr}
};

namespace
{
	struct init
	{
		init() 
		{
			i3UIScene::PreRegisterLuaFunction("GAME_STRING", DefaultUI_GAME_STRING);
			i3UIScene::PreRegisterLuaFunction("GAME_STRING_PTR", DefaultUI_GAME_STRING_PTR);
		}
	}_;
}

//------------------------------------------------------------------------------//

//I3_ABSTRACT_CLASS_INSTANCE( UIBase, i3GameNode);
I3_CLASS_INSTANCE( UIBase);

UIBase::UIBase() : m_bInputDisable(false), m_pFrameWnd(nullptr), m_pBG(nullptr)
{
//	m_UISceneList.SetOptmizeEnableWhenClear( false);

	m_UIStyle = 0;

	m_rAwayInputTime = 0.f;
	m_rCautionTime = 0.f;
	m_bOffAwayInput = false;
	m_bOffCaution = false;
	m_pExitMessageBox = nullptr;

//	m_SlideTime = 0.0f;
	m_SlideState = SS_IN_OK;
//	m_SlideX = 0.0f;
//	m_SlideY = 0.0f;
//	m_SlideRight = true;

	m_slideCB.SetObj(this);
	m_slide.SetTargetTime(false, 1.f / UI_SLIDEOUT_SLOP);
	m_slide.SetTargetTime(true, 1.f / UI_SLIDEIN_SLOP);
	m_slide.SetCallback(&m_slideCB);
	
	
	
	i3mem::FillZero( m_pGuiSound, sizeof( m_pGuiSound));

	m_pLastMouseOverCtrl = nullptr;
}

UIBase::~UIBase()
{
	INT32 i;
	I3_SAFE_RELEASE(m_pBG);
	__ClearScene();

	for( i = 0; i < UISND_MAX; ++i)
	{
		I3_SAFE_RELEASE( m_pGuiSound[i]);
	}	
}

void UIBase::__FindToolTipTemplate( void)
{
	if( s_pToolTipTemplate == nullptr)
	{
		i3UILibrary * pLib = i3UI::getUILibrary();
		if( pLib != nullptr)
		{
			s_pToolTipTemplate = pLib->FindByName( "ToolTip");
//			I3ASSERT( s_pToolTipTemplate != nullptr);
		}
	}
}
 
void UIBase::__RegisterScene( const char * pszScenePath, const LuaFuncReg * GlueFunc /*= nullptr*/, bool bSceneEnable /*= true*/ )
{
	I3ASSERT( pszScenePath != nullptr);

//	UISCENEINFO * pInfo = (UISCENEINFO*) i3MemAlloc( sizeof( UISCENEINFO));
//	pInfo->Reset();
	UISCENEINFO * pInfo = new UISCENEINFO;
	MEMDUMP_NEW( pInfo, sizeof(UISCENEINFO));

	i3::safe_string_copy( pInfo->_szPath, pszScenePath, MAX_PATH );
	pInfo->_GlueFunc = (LuaFuncReg*)(GlueFunc);
	pInfo->_EnableScn = bSceneEnable;

	m_UISceneList.push_back( pInfo);
}

static void RecSetEventReceiverToControl( i3UIControl * pControl, i3EventReceiver * pReceiver)
{
	i3UIControl * pChild = (i3UIControl*) pControl->getFirstChild();

	while( pChild)
	{
		if( i3::kind_of<i3UIControl*>(pChild))
		{
			pChild->setEventReceiver( pReceiver);
		}

		RecSetEventReceiverToControl( pChild, pReceiver);

		pChild = (i3UIControl*) pChild->getNext();
	}
}

static bool sRecFindChild( i3GameNode * pNode)
{
	I3ASSERT( g_pFramework->getFocus() != nullptr);

	if( g_pFramework->getFocus() == pNode)
		return true;

	i3GameNode * pChild = pNode->getFirstChild();

	while( pChild != nullptr)
	{
		bool bRv = sRecFindChild( pChild);

		if( bRv)
			return true;

		pChild = pChild->getNext();
	}

	return false;
}

void UIBase::__LoadScene( void)
{
	for(size_t i = 0; i < m_UISceneList.size(); i++)
	{
		UISCENEINFO * pInfo = m_UISceneList[ i];

		if( pInfo->_pScene == nullptr)
		{
			i3_prof_scope("__DirectlyLoadScene call");
			__DirectlyLoadScene( pInfo);
		}
		else
		{
			i3_prof_scope("__BindScene call");
			__BindScene( pInfo->_pScene, pInfo->_GlueFunc, true);
		}
	}
	
	i3_prof_scope("OnLoadAllScenes call");
	OnLoadAllScenes();

}

/*virtual*/ i3UIScene *	UIBase::_LoadScene( const char * pszScenePath, LuaFuncReg * GlueFunc, bool bSceneEnable)
{
	VEC3D vZero;
	VEC2D vSize;

	I3ASSERT( i3UI::getManager() != nullptr);

#if defined( I3_DEBUG)
	char szExt[ 32];
	i3::extract_fileext( pszScenePath, szExt);
	i3::to_upper( szExt);
	if( i3::generic_is_iequal( szExt, "I3UIS") == false)
	{
		I3PRINTLOG(I3LOG_FATAL,  "must be fileext name i3UIs !!!");
	}
#endif
	//char szPath[ MAX_PATH];
	//i3::change_file_ext_copy(szPath, pszScenePath, ".i3UIs");
i3_prof_start("i3UI::getManager()->LoadUIScene call");
	i3UIScene* pUIScene = i3UI::getManager()->LoadUIScene( pszScenePath);
i3_prof_end();

	if( pUIScene != nullptr)
	{
		__FindToolTipTemplate();

		if( s_pToolTipTemplate != nullptr)
		{
			i3UIToolTip * pToolTip = (i3UIToolTip*) i3UI::CreateControl( s_pToolTipTemplate, &vZero, &vSize, pUIScene);
			pUIScene->setDefToolTip( pToolTip);
		}

		i3_prof_start("__BindScene call");
		__BindScene( pUIScene, GlueFunc, bSceneEnable);
		i3_prof_end();
	}

	return pUIScene;
}

void UIBase::__DirectlyLoadScene( UISCENEINFO * pInfo)
{
	i3_prof_func();

	i3_prof_start("_LoadScene call");
	i3UIScene * pScene = _LoadScene( pInfo->_szPath, pInfo->_GlueFunc, pInfo->_EnableScn);
	i3_prof_end();

	I3_REF_CHANGE( pInfo->_pScene, pScene);
	
	
	// Lua Initialize callback을 호출합니다.
	// 또는 하위 Class에서 Control을 찾는 Code를 여기에 Define합니다.

	i3_prof_start("_InitializeAtLoad call");
	_InitializeAtLoad( pInfo->_pScene);
	i3_prof_end();

	i3_prof_start("RecSetEventReceiverToControl call");
	RecSetEventReceiverToControl( (i3UIControl*)pInfo->_pScene, this);
	i3_prof_end();
}

void UIBase::__BindScene( i3UIScene * pScene, LuaFuncReg * GlueFunc, bool bEnable)
{
	I3ASSERT( pScene != nullptr);

	bool bAttached = false;
	if( pScene->getParent() == nullptr)
	{
		bAttached = true;
	}

i3_prof_start("pScene->Attach( true) call");
	pScene->Attach( true);
i3_prof_end();

	pScene->EnableScene( bEnable);

	//Register Lua Glue Functions
	if( bAttached)
	{
		LuaState* L = pScene->getLuaContext();
		if( L != nullptr)
		{
			if( m_strLuaContextName.empty() )
			{
				i3::extract_filetitle(pScene->getScriptFile(),	m_strLuaContextName);
			}
			_RegisterLuaFunctions( L, GlueFunc);
		}
	}

}

void UIBase::__UnbindScene( i3UIScene * pScene)
{
	I3ASSERT( pScene != nullptr);
	
	pScene->EnableScene( false);
	pScene->Detach();

	if( g_pFramework->getFocus() != nullptr)
	{
		if( sRecFindChild( pScene) == true)
			g_pFramework->SetFocus( nullptr);
	}
}


void UIBase::__UnloadScene( void)
{
	OnUnloadAllScenes();			// 씬리스트 소거 전에 해주는게 좋겠다..

	for(size_t i = 0; i < m_UISceneList.size(); i++)
	{
		UISCENEINFO * pInfo = m_UISceneList[ i];
		
		if( pInfo->_LoadedStyle == false)
		{
			__UnloadScene( pInfo->_pScene);
			pInfo->_pScene = nullptr;
		}
		else if(pInfo->_pScene != nullptr)
		{
			pInfo->_pScene->EnableScene( false);

			// 씬을 끈 상태에서...포지션을 원래의 위치로 환원시킨다..
			m_slide.Restore();
/*
			const INT32 num_list = pInfo->_FrameWndPosList.GetCount();
			i3GameNode* n = pInfo->_pScene->getFirstChild();

			for (INT32 i = 0; i < num_list ; n = n->getNext() , ++i )
			{
				i3UIControl* ctrl = static_cast<i3UIControl*>(n);
				VEC2D* pv_src = reinterpret_cast<VEC2D*>(pInfo->_FrameWndPosList.Items[i]);
				ctrl->setPos(pv_src->x, pv_src->y); 
			}
*/
		}
	}

	m_slide.ClearFrameWndList();

}

void UIBase::__UnloadScene( i3UIScene * pScene)
{
	if( pScene != nullptr)
	{
		if( i3UI::getManager() != nullptr)
		{
			i3UI::getManager()->RemoveScene( pScene);

			__UnbindScene( pScene);
		}

		RecSetEventReceiverToControl( (i3UIControl*)pScene, nullptr);

		_ClearAtUnload( pScene);

		I3_SAFE_RELEASE( pScene);
	}
}

void UIBase::__ClearScene( void)
{
	
	const size_t nCount = m_UISceneList.size();

	if (!m_UISceneList.empty())
	{
		bool bExistScene = false;
		for(size_t i = 0; i < nCount; i++)
		{
			UISCENEINFO* pInfo = m_UISceneList[i];
			if (pInfo->_pScene)
				bExistScene = true;
		}
		
		if (bExistScene)
			this->OnUnloadAllScenes();
	}

	for(size_t i = 0; i < nCount; i++)
	{
		UISCENEINFO* pInfo = m_UISceneList.back();
		I3ASSERT( pInfo != nullptr);
		
		__UnloadScene( pInfo->_pScene);
		i3::vu::remove(m_UISceneList, pInfo);
		I3_SAFE_DELETE( pInfo);
	}
	
	m_UISceneList.clear();

	i3GameNode::SetEnable( false);
}

LuaState* UIBase::_CallLuaFunction( const char * pszScene, const char * pszFunction )
{
	i3UIScene * pScene = GetScene( pszScene);
	return UIBase::__CallLuaFunction_Internal(pScene, pszFunction);
}

LuaState *	 UIBase::_CallLuaFunction( INT32 idxScene, const char* pszFunction)
{
	i3UIScene * pScene = GetScene(idxScene);

	return UIBase::__CallLuaFunction_Internal(pScene, pszFunction);
}

LuaState *	UIBase::__CallLuaFunction_Internal( i3UIScene* pScene, const char* pszFunction)
{
	return UILua::CallLuaFunction(pScene, pszFunction);
}

LuaState * UIBase::_CallLuaFunction( const char * pszFunction)
{
#if defined( I3_DEBUG)
	if( m_strLuaContextName.empty() ) 
	{
		I3PRINTLOG(I3LOG_FATAL,  "Lua Script가 설정되지 않았습니다..");
	}
#endif

	return _CallLuaFunction( m_strLuaContextName.c_str(), pszFunction);
	// _CallLuaFunction( 0, pszFunction); 으로 해도 될것이다. (2011.05.19.수빈)
}

void UIBase::_EndLuaFunction( LuaState* L, INT32 nArg ) 
{
	UILua::EndLuaFunction(L, nArg);
}

bool UIBase::_ReturnLuaBoolean( LuaState* L, INT32 nArg)
{
	return UILua::ReturnLuaBoolean(L, nArg);
}

REAL32 UIBase::_ReturnLuaNumber( LuaState * L, INT32 nArg)
{
	return UILua::ReturnLuaNumber(L, nArg);
}

void UIBase::_ReturnLuaVec2D(VEC2D * pOut,  LuaState * L, INT32 nArg)
{
	UILua::ReturnLuaVec2D( pOut, L, nArg);
}


const char * UIBase::_ReturnLuaString( LuaState* L, INT32 nArg)
{
	return UILua::ReturnLuaString(L, nArg);
}


void * UIBase::_ReturnLuaPointer( LuaState* L, INT32 nArg)
{
	return UILua::ReturnLuaPointer(L, nArg);
}

INT32	UIBase::_ReturnLuaInteger( LuaState* L, INT32 nArg)
{
	return UILua::ReturnLuaInteger(L , nArg);
}


INT32 UIBase::_GetLuaNumber( const char * pszScene, const char * pszGlobalName, INT32 StackPos)
{
	i3UIScene * pScene = GetScene( pszScene);
	I3ASSERT( pScene != nullptr);

	LuaState* L = pScene->getLuaContext();

	if( L == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Not yet binding script..%s. %s!!!!", pszScene, pszGlobalName);
		return 0;
	}

	i3Lua::GetGlobal( L, pszGlobalName);
	if( i3Lua::StackIsNumber( L, StackPos) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "Lua Stack is invalid.. must Ingeter type!!!");
		return 0;
	}

	return i3Lua::StackToInteger( L, StackPos);
}

INT32 UIBase::_GetLuaNumber( const char * pszGlobalName, INT32 StackPos /*= -1*/)
{
	return _GetLuaNumber( m_strLuaContextName.c_str(), pszGlobalName, StackPos);
}

/*static*/ void	UIBase::_RegisterLuaFunctions( LuaState* L, LuaFuncReg * GlueFunc)
{
	INT32 i;

	if( L == nullptr) return;

	for( i = 0; DefaultUI_Glue[i].name; i++)
	{
		i3Lua::RegisterFunc( L, DefaultUI_Glue[i].name, DefaultUI_Glue[i].func);
	}

	if( GlueFunc != nullptr)
	{
		for( i = 0; GlueFunc[i].name; i++)
		{
			i3Lua::RegisterFunc( L, GlueFunc[i].name, GlueFunc[i].func);
		}
	}
}

/*virtual*/ void UIBase::_InitializeAtLoad( i3UIScene * pScene)
{
	I3ASSERT( pScene != nullptr);

	m_SlideState = SS_OUT_OK;

	if( m_pFrameWnd == nullptr)
	{
		m_pFrameWnd = (i3UIFrameWnd*) pScene->getFirstChild();
	}
}

/*virtual*/ void UIBase::_ClearAtUnload( i3UIScene * pScene)
{
	m_pFrameWnd = nullptr;
	I3_SAFE_RELEASE(m_pBG);
}

i3GameNode * UIBase::_FindListBoxFromCombo( i3GameNode * pNode)
{
	i3GameNode * pChild = pNode->getFirstChild();
	i3GameNode * pResult;

	while( pChild != nullptr)
	{
		if( i3::same_of<i3UIListBox*>(pChild))
			return pChild;

		pResult = _FindListBoxFromCombo( pChild);

		if( pResult != nullptr)
			return pResult;

		pChild = pChild->getNext();
	}

	return nullptr;
}

bool UIBase::_IsHost( void)
{
	return BattleSlotContext::i()->getMainSlotIdx() == BattleSlotContext::i()->getMySlotIdx();
}

void UIBase::_DefaultErrorPopup( const i3::wliteral_range& wMsgRng, INT32 event, INT32 arg)
{
	// 현재로서 들어올 에러가 없다 - praptor
	i3::rc_wstring wstr_bug;
	/*디버그 팝업: Event(%d), Arg(0x%x)\n담당자: praptor*/
	i3::sprintf( wstr_bug, wMsgRng, event, arg);
	GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_bug);
}

void UIBase::_AttachToTopScreen( void)
{
	for(INT32 idx = 0 ; idx < GetSceneCount() ; ++idx)
	{
		// 최상위로 그리도록 한다.
		i3UIScene * pScene = GetScene(idx);
		i3GameNode * pParent = pScene->getParent();

		if( pParent != nullptr)
			pScene->ChangeParent(pParent);
	}
}

void UIBase::_Resize()
{
	if (m_pBG == nullptr)
		return;

	i3UIManager * pMng = i3UI::getManager();

	// 배경
	REAL32 w = (REAL32)g_pViewer->GetViewHeight() * 16.f / 9.f;
	REAL32 scale = (REAL32)g_pViewer->GetViewHeight() / (REAL32)pMng->getActualHeight();

	m_pBG->SetShapeSize(s_BGSizeWidth * scale, s_BGSizeHeight * scale);

	{
		REAL32 rateX = pMng->getScreenStretchRatioX();
		REAL32 offset = (REAL32)pMng->getDisplayOffsetX();
		REAL32 x = ((g_pViewer->GetViewWidth() - w) * 0.5f - (offset)) * rateX;

		REAL32 rateY = pMng->getScreenStretchRatioY();

		m_pBG->setPos(x, -(REAL32)pMng->getDisplayOffsetY() * rateY);
	}
}
i3UIScene *	UIBase::AddScene( const char * pszScenePath, const LuaFuncReg * GlueFunc /*= nullptr*/, bool bLoaded /*= false*/, bool bSceneEnable /*=true*/ )
{
	i3::rc_string strCommonUIFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);

	i3::stack_string strFullPath;
	i3::sprintf(strFullPath, "%s/%s", strCommonUIFolder, pszScenePath);

	__RegisterScene(strFullPath.c_str(), GlueFunc, bSceneEnable);

	m_pre_loaded = bLoaded;

	if( bLoaded)
	{
		UISCENEINFO * pInfo = m_UISceneList[ m_UISceneList.size() - 1];

		pInfo->_GlueFunc = (LuaFuncReg*) GlueFunc;
		pInfo->_EnableScn = bSceneEnable;
		pInfo->_LoadedStyle = true;

i3_prof_start("__DirectlyLoadScene call from AddScene");
		__DirectlyLoadScene( pInfo);
i3_prof_end();

		return pInfo->_pScene;
	}

	return nullptr;
}

/*virtual*/ void UIBase::OnCreate( i3GameNode * pParent)
{
	I3_GAMENODE_ADDCHILD( pParent, this);

	//	UI 사운드 로딩
	for(INT32 i = 0; i < UISND_MAX; ++i)
	{
		if( m_pGuiSound[i] == nullptr)
		{
			//	중복 로딩을 막기위해 komet
			i3GameResSound * pRes = (i3GameResSound *) g_pFramework->QuaryResource( s_szUISoundFile[i]);
 
			if( (pRes != nullptr) && (pRes->getSound() != nullptr))
			{
				I3_REF_CHANGE( m_pGuiSound[i], pRes->getSound());
			}
		}
	}

	i3GameNode::SetEnable( false);
}

void UIBase::Destroy( void)
{
	__ClearScene();

	if( getParent())
	{
		getParent()->RemoveChild( this);
	}

	I3ASSERT( GetRefCount() == 1);
	Release();
}

/*virtual*/ void UIBase::OnUpdate( REAL32 rDeltaSeconds)
{
	
// 호출순서를 바꾸었습니다.. i3GameNode에 자식리스트의 업데이트가 있는데, 기본함수부터 처리를 마친후 자식쪽 업데이트를 돌게합니다.
	m_slide.Update(rDeltaSeconds);	

	i3GameNode::OnUpdate( rDeltaSeconds);
}

/*virtual*/ bool UIBase::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{

		// Key : User Guide System
		{
			UserGuideMgr::i()->OnEvent(event, pObj, param2);
		}

		if( event == I3_EVT_UI_NOTIFY)
		{
			if( pObj != nullptr )
			{
				I3UI_CONTROL_NOTIFY * pNotify = (I3UI_CONTROL_NOTIFY*) param2;
				i3UIControl * pCtrl = pNotify->m_pCtrl;

				if( pCtrl != nullptr)
				{
					switch( pNotify->m_nEvent)
					{	
					case I3UI_EVT_ENTERMOUSE :	OnEnterMouse( pCtrl);	return true;
					case I3UI_EVT_LEAVEMOUSE :	OnLeaveMouse( pCtrl);	return true;
					case I3UI_EVT_CLICKED :		OnClick( pCtrl);		return true;
					case I3UI_EVT_R_CLICKED :	OnRClick( pCtrl);		return true;
					case I3UI_EVT_MOUSEWHEEL :	OnWheel( pCtrl, pNotify->m_nPriParam);	return true;
					case I3UI_EVT_COMPLETE_LOAD : OnCompleteLoad();		return true;
					case I3UI_EVT_DBL_CLICKED :	OnDblClick( pCtrl);		return true;

					case I3UI_EVT_DRAGSTART :	OnDragStart( pCtrl, pNotify->m_nPriParam, pNotify->m_nSecParam);	return true;
					case I3UI_EVT_DRAGGING :	OnDragging( pCtrl, pNotify->m_nPriParam, pNotify->m_nSecParam);		return true;
					case I3UI_EVT_DRAGEND :		OnDragEnd( pCtrl, pNotify->m_nPriParam, pNotify->m_nSecParam);		return true;
					}
				}
			}
		}
		else if( event == I3_EVT_INPUT)
		{
			if (i3UI::getDisableUIInput() )	// 이 경우엔 무조건 막음..
				return true;
			
			if ( IsInputDisable() )		// 인풋이 막히면 일단 해당Input은 넘김..
				return false;
			
			if( OnKeyInput( (i3InputDeviceManager*)pObj))
				return true;
		}
	}

	return false;
}

/*virtual*/ bool UIBase::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();
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

/*virtual*/ bool UIBase::OnEnterMouse( i3UIControl * pControl)
{
	if( (pControl != nullptr) && pControl->isActivated())
	{
		if( i3::same_of<i3UIButton*>(pControl)
			|| i3::same_of<i3UIButtonComposed3*>(pControl)
			|| i3::same_of<i3UIListView_Item*>(pControl))
		{
			if( m_pLastMouseOverCtrl != pControl &&
				pControl->isActivated())
			{
				m_pLastMouseOverCtrl = pControl;
			}
		}

		OnPlayEnterMouseSound(pControl);

		return true;
	}

	return false;
}

/*virtual*/ void UIBase::OnLeaveMouse( i3UIControl * pControl)
{
	if( pControl != nullptr &&
		( i3::same_of<i3UIButton*>(pControl)
		|| i3::same_of<i3UIButtonComposed3*>(pControl) ) )
	{
		m_pLastMouseOverCtrl = nullptr;
	}
}

/*virtual*/ bool UIBase::OnClick( i3UIControl * pControl)
{
	bool bValid = false;

	if( pControl != nullptr && pControl->isActivated())
	{
		OnPlayClickSound(pControl);
	
		bValid = true;
	}

	if( g_pFramework->GetUIMainframe() != nullptr)
	{
		g_pFramework->GetUIMainframe()->ClosePopupByAnotherControlInput( pControl);
	}

	return bValid;
}

/*virtual*/ void UIBase::OnRClick( i3UIControl * pControl)
{
	
}

/*virtual*/ bool UIBase::OnDblClick( i3UIControl * pControl)
{
	if( pControl != nullptr && pControl->isActivated())
		return true;

	return false;
}

/*virtual*/ void UIBase::SetEnable( bool bTrue /*= true*/, REAL32 tm /*= 0.f*/)
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
		if( pManager != nullptr)
			pManager->delActiveFrameWnd();
	}
}

/*virtual*/ void UIBase::SetVisible( bool bTrue /*= true*/, REAL32 tm /*= 0.f*/)
{
	

	if( i3GameNode::isVisible() != bTrue)
	{
		i3GameNode::SetVisible( bTrue, tm);

		for(size_t i = 0; i < m_UISceneList.size(); i++)
		{
			UISCENEINFO * pInfo = m_UISceneList[ i];
			if( pInfo->_pScene != nullptr)
			{
				pInfo->_pScene->EnableScene( bTrue != false);
			}
		}
	}
}

bool UIBase::OnLostDevice( bool bLostDevice)
{
	return i3GameNode::OnLostDevice( bLostDevice);
}

bool UIBase::OnRevive( i3RenderContext * pCtx)
{
	if( m_pre_loaded)
	{
		for(size_t i = 0; i < m_UISceneList.size(); i++)
		{
			UISCENEINFO * pInfo = m_UISceneList[ i];
			I3ASSERT( pInfo != nullptr);

			if( pInfo->_pScene != nullptr)
			{
				if( pInfo->_pScene->isEnable() == false)
				{	// 안보이지만 로딩도 되어 있지만 Node에서 처리 하지 않아(Disable상태) 임의로 호출하여 좌표등을 다시 설정해야한다.
					i3UIControl * pChild = (i3UIControl*)getFirstChild();
					while( pChild != nullptr)
					{
						if( i3::kind_of<i3UIControl*>(pChild))
						{
							pChild->OnRevive( pCtx);
						}

						pChild = (i3UIControl*)pChild->getNext();
					}
				}
			}
		}
	}

	if (i3GameNode::OnRevive(pCtx) == false)
		return false;

	m_slide.ResetOriginalPos();

	return true;
}

i3UIScene * UIBase::GetScene( const char * pszSceneName) const
{
	for(size_t i = 0; i < m_UISceneList.size(); i++)
	{
		UISCENEINFO * pInfo = m_UISceneList[ i];
		I3ASSERT( pInfo != nullptr);

		if( pInfo->_pScene != nullptr)
		{
//			if( i3String::NCompare( pInfo->_pScene->GetName(), pszSceneName, MAX_PATH) == 0)
			if (i3::generic_is_iequal(pInfo->_pScene->GetName(), pszSceneName) )
				return pInfo->_pScene;
		}
	}

	return nullptr;
}

i3UIScene * UIBase::GetScene( INT32 idxScene) const
{
	if(m_UISceneList.empty())	return 0;

	UISCENEINFO * pInfo = m_UISceneList[idxScene];
	return (pInfo) ? pInfo->_pScene : 0;
}

bool		UIBase::IsEnableFromSceneInfo(INT32 idxScene) const
{
	UISCENEINFO * pInfo = m_UISceneList[idxScene];
	return (pInfo) ? pInfo->_EnableScn : false;
}

void		UIBase::SetEnableToSceneInfo(INT32 idxScene, bool bEnable)
{
	UISCENEINFO * pInfo = m_UISceneList[idxScene];
	if (pInfo) 
		pInfo->_EnableScn = bEnable;
}


static void RecInputDisable( i3GameNode * pNode, bool bDisable)
{	
	i3GameNode * pChild = pNode->getFirstChild();

	while( pChild != nullptr)
	{
		if( i3::kind_of<i3UIControl*>(pChild))
			((i3UIControl*)pChild)->setInputDisable( bDisable);

		RecInputDisable( pChild, bDisable);

		pChild = pChild->getNext();
	}

}

void UIBase::SetInputDisable( bool bDisable)
{
	for(size_t i = 0; i < m_UISceneList.size(); i++)
	{
		UISCENEINFO* pInfo = m_UISceneList[i];
		I3ASSERT( pInfo != nullptr);

		if( pInfo->_pScene != nullptr)
		{
			RecInputDisable( pInfo->_pScene, bDisable);
		}
	}

	for (i3GameNode* n =	this->getFirstChild() ; n != nullptr ; n = n->getNext())
	{
		if ( i3::kind_of<UIBase*>(n))
		{
			static_cast<UIBase*>(n)->SetInputDisable(bDisable);
		}
	}

	m_bInputDisable = bDisable;
}

bool UIBase::PlaySound( UISOUND_TYPE type)
{
	I3_BOUNDCHK((INT32) type, UISND_MAX);

	if( m_pGuiSound[type] == nullptr ||
		m_pGuiSound[type]->getSound() == nullptr)
		return false;

	if( g_pSndMng->tellPlaySound( m_pGuiSound[type]) == nullptr)
		return false;

	return true;
}

/*virtual*/ bool UIBase::OnPlayClickSound(i3UIControl * pCtrl)
{
	if( i3::same_of<i3UIButton*>(pCtrl)
		|| i3::same_of<i3UIButtonComposed3*>(pCtrl)
		|| i3::same_of<i3UIListView_Item*>(pCtrl) ) 
	{
		return PlaySound( UISND_ITEM_SELECTING);
	}
	else if( i3::same_of<i3UIButtonImageSet*>(pCtrl))
	{
		// 버튼이미지셋의 경우, UI입력을 받지 않는 경우를 추가로 걸러내는게 좋겠다.(11.10.11.수빈)
		i3UIButtonImageSet* pimgset = static_cast<i3UIButtonImageSet*>(pCtrl);
		if( pimgset->IsViewOnly() == false)
			return PlaySound( UISND_ITEM_SELECTING);
	}
	else if( i3::same_of<i3UITab*>(pCtrl))
	{
		return PlaySound( UISND_LIST_OVER);
	}

	return false;
}

/*virtual*/ bool UIBase::OnPlayEnterMouseSound(i3UIControl * pCtrl)
{
	// 모든 버튼에 마우스 오버시 사운드 재생
	// 단 프레임윈도우 외 컨트롤 하위에 있는 버튼은 제외 (ex Scroll, ComboBox, ListView ...)
	if(i3::same_of<i3UIButton*, i3UIButtonComposed3*, i3UITab*>(pCtrl) &&
		(pCtrl->getParent() != nullptr && i3::same_of<i3UIFrameWnd*>( pCtrl->getParent())) )
	{
		return PlaySound( UISND_LIST_OVER);
	}

	return false;
}

bool UIBase::IsLuaContext( LuaState * L)
{
	
	for(size_t i = 0; i < m_UISceneList.size(); i++)
	{
		UISCENEINFO * pInfo = m_UISceneList[ i];

		if( pInfo->_pScene != nullptr)
		{
			if( pInfo->_pScene->getLuaContext() == L)
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

bool UIBase::IsIncludeControl( i3UIControl * pControl)
{
	
	for(size_t i = 0; i < m_UISceneList.size(); i++)
	{
		UISCENEINFO * pInfo = m_UISceneList[ i];

		if( pInfo->_pScene != nullptr)
		{
			if( Rec_FindChild( pInfo->_pScene, pControl))
			{
				return true;
			}
		}
	}

	return false;
}

void UIBase::OnSlideIn( void)
{
	if( SS_IN_OK != m_SlideState &&
		SS_IN_START != m_SlideState &&
		SS_SLIDING_IN != m_SlideState )
	{
		m_SlideState = SS_IN_START;
		m_slide.Start(true);
	}
}

void UIBase::OnSlideOut( void)
{
	if( SS_OUT_OK != m_SlideState && 
		SS_OUT_START != m_SlideState &&
		SS_SLIDING_OUT != m_SlideState )
	{
		m_SlideState = SS_OUT_START;
		m_slide.Start(false);
	}
}

void	UIBase::_OnSlideInOnce()
{
	m_SlideState = SS_SLIDING_IN;
	
	OnSlideInOnce();
}

void	UIBase::_OnSlidingIn(REAL32 rDeltaTime)
{
	OnEntrancing(rDeltaTime);
}

void	UIBase::_OnSlideInEnd()
{
	m_SlideState = SS_IN_OK;
	OnEntranceEnd();
}

void	UIBase::_OnSlideOutOnce()
{
	m_SlideState = SS_SLIDING_OUT;

	OnSlideOutOnce();
}

void	UIBase::_OnSlidingOut(REAL32 rDeltaTime)
{
	OnExiting(rDeltaTime);	
}

void	UIBase::_OnSlideOutEnd()
{
	m_SlideState = SS_OUT_OK;
	OnExitEnd();
}

/*virtual*/ bool UIBase::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	i3_prof_func();
	
	if( isEnable() == false)
	{
		i3_prof_start("SetEnable call");
		SetEnable( true);
		i3_prof_end();

		i3_prof_start("OnSlideIn call");
		OnSlideIn();
		i3_prof_end();

		return true;
	}

	// 다시 열수 없는 타입이면 false
	if( isReopenStyle() == false)
		return false;

	// 다시 설정하는 타입이다..
	return true;
}

/*virtual*/ bool UIBase::OnEntrancing( REAL32 rDeltaSeconds)
{
	return true;
}

/*virtual*/ void UIBase::OnEntranceEnd( void)
{
	SetInputDisable( false);
}
 
/*virtual*/ bool UIBase::OnExiting( REAL32 rDeltaSeconds)
{
	return true;
}

/*virtual*/ void UIBase::OnExitEnd( void)
{
	// Key : User Guide System
	UserGuideMgr::i()->UnLoadGuideTargetInfo(this);

	SetEnable( false);
	SetInputDisable( true);
}

void UIBase::OnEscapeKey(void)
{
	if (g_pFramework->GetMessageBoxManager()->IsTopMsgBox(m_pExitMessageBox) == true) return; //종료창이 활성화 상태이므로 리턴한다.

	UIMainFrame* MainFrame = ((UIFramework*)m_pParent)->GetUIMainframe();
	bool bDailyRecord = false;
	if (MainFrame != nullptr)
	{
		UISideMenu * pSide = UISideMenu::instance();
		if (pSide != nullptr)
			if (pSide->clicked_escape_key() == true) return; //왼쪽 메뉴가 활성화 상태면 esc 키로 비활성화

		if (MainFrame->GetCurrentPhaseType() >= UIPHASE_LOBBY) //로비일 경우 오늘의 전적으로 활성화 될 수 있다.
		{
			if (MainFrame->GetCurrentPhaseState() == PS_PLAYING)
			{
				if (LocaleValue::Enable("EnableDailyRecord"))
				{
					GameEventSender::i()->SetEvent(EVENT_EXIT_DAILYRECORD_SEND);
					bDailyRecord = true;
				}
			}
		}
	}

	if (bDailyRecord == false)
	{
		m_pExitMessageBox = GameUI::MsgBox(MSG_TYPE_SYSTEM_QUERY, GAME_RCSTRING("STBL_IDX_EP_GAME_EXIT_NORMAL_QE"), nullptr, EXIT_CALLBACK(CbExitNormal, false), this);
	}


	//alt + F4로 활성화 할 경우, 종료창이 활성화된 상태에서 키입력이 바로 되기 때문에 임의로 비활성화 한다.(중요한 코드)
	//만약, 아래 코드가 없는 경우엔 종료창이 활성화 즉시 비활성화 된다.
	i3InputKeyboard * pKeyboard = g_pFramework->getKeyboard();
	if (pKeyboard->GetStrokeState(I3I_KEY_F4) == true)
		pKeyboard->SetStrokeState(I3I_KEY_F4, false);

	if (g_pConfigEx->GetMisc().webAnnounce.IsActive())
	{
		REAL32 widthRate = i3UI::getUIResolutionWidth();
		REAL32 heightRate = i3UI::getUIResolutionHeight();

		i3::pack::POINT2D ptCenter = g_pFramework->GetCenterPosition(m_pExitMessageBox->GetWidth(), m_pExitMessageBox->GetHeight());
		ptCenter.x += static_cast<int>(static_cast<REAL32>(g_pConfigEx->GetMisc().webAnnounce.msgBoxOffsetX) * widthRate);
		ptCenter.y += static_cast<int>(static_cast<REAL32>(g_pConfigEx->GetMisc().webAnnounce.msgBoxOffsetY) * heightRate);
		m_pExitMessageBox->SetPosition(ptCenter.x, ptCenter.y);
	}
}

bool UIBase::isInputKeyboardEnter( void)
{
	bool bInputDisable = false;
	if( m_pFrameWnd)
	{
		bInputDisable = m_pFrameWnd->isIgnoreInput();
	}

	if( bInputDisable == false) 
		return g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ) ? true : false;

	return false;
}

