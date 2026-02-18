#include "i3FrameworkPCH.h"
#include "i3UIManager.h"
#include "../i3Framework.h"
#include "i3Stage.h"

#include "i3Base/string/ext/extract_directoryname_filename.h"

extern LuaFuncReg i3UIGlue[];
extern LuaFuncReg i3UIServerGlue[];

i3UIScene	*	i3UIManager::FindAncestorByState(LuaState * pState)
{
	i3GameNode * pNode = getFirstChild();
	while( pNode != nullptr)
	{
		if( i3::same_of<i3UIScene*>(pNode))
		{
			if( pState == ((i3UIScene*)pNode)->getLuaContext())
			{
				return (i3UIScene*)pNode;
			}
		}

		pNode = pNode->getNext();
	}

	return nullptr;
}

extern "C" int i3UI_SetEventHandler(lua_State *L)
{
	char szEventHandler[ 260];
	char szControl[ 260];

	if( (i3Lua::GetStringArg( L, 1, szEventHandler, sizeof( szEventHandler)) != nullptr) &&
		(i3Lua::GetStringArg( L, 2, szControl, sizeof( szControl)) != nullptr) )
	{
		i3UIControl * pCtrl = i3UI::FindControlByName( L, szControl);
		
		if( pCtrl != nullptr)
			pCtrl->SetEventHandler( szEventHandler);
	}
	
	return 0;
}

extern "C" int i3UI_OnCommand(lua_State *L)
{
	char szControl[ 260];
	
	if( i3Lua::GetStringArg( L, 1, szControl, sizeof( szControl)) != nullptr)
	{
		i3UIControl * pCtrl	= i3UI::FindControlByName( L, szControl);

		if( pCtrl != nullptr)
		{
			pCtrl->OnCommand( L);
			return 1;
		}		
	}

	return 0;
}

extern "C" int i3UI_RunGUI(lua_State *L)
{
	char szFull[MAX_PATH];
	char szFileNameIn[ MAX_PATH];

	if( i3Lua::GetStringArg( L, 1, szFileNameIn, sizeof( szFileNameIn)) != nullptr)
	{
		const char * pszBasePath = i3ResourceFile::GetWorkingFolder();
		if( pszBasePath[0] != 0)
			i3::snprintf( szFull, MAX_PATH, "%s/%s", pszBasePath, szFileNameIn);
		else
			i3::snprintf( szFull, MAX_PATH, szFileNameIn);
	
		if( i3System::IsFile( szFull))
		{
			return i3Lua::DoFile( L, szFull);
		}
		else
		{
			// I3S를 읽어들이고 있는 현재 Path에서 다시 한번 검색한다.
			char szPath[MAX_PATH], szFileName[MAX_PATH];
			i3::extract_directoryname_filename(szFileNameIn, szPath, szFileName);

			if( i3::generic_string_size( szPath) == 0)
			{
				i3::snprintf( szFull, MAX_PATH, "%s", szFileName);
			}
			else
			{
				i3::snprintf( szFull, MAX_PATH, "%s/%s", szPath, szFileName);
			}

			if( i3System::IsFile( szFull))
			{
				return i3Lua::DoFile( L, szFull);
			}
			else
			{
				I3TRACE( "Couldn't find lua file : %s\n", szFileNameIn);
				I3ASSERT_0;
			}
		}
	}

	return -1;
}

extern "C" int i3UI_EnableObject(lua_State *L)
{
	char szControl[ 260];

	if( i3Lua::GetStringArg( L, 1, szControl, sizeof( szControl)) != nullptr)
	{
		INT32	nEnable			= i3Lua::GetIntegerArg( L, 2, 0);

		i3UIControl * pCtrl = i3UI::FindControlByName( L, szControl);
		if( pCtrl != nullptr)
		{
			bool bEnable = (nEnable != 0) ? true : false;
			if( pCtrl->isEnable() != bEnable)
			{
				pCtrl->EnableCtrl(bEnable, true);
			}

			return 1;
		}
	}

	return 0;
}

extern "C" int i3UI_GetEnableObject(lua_State *L)
{
	char szControl[ 260];

	if( i3Lua::GetStringArg( L, 1, szControl, sizeof( szControl)) != nullptr)
	{
		i3UIControl * pCtrl = i3UI::FindControlByName( L, szControl);
		if( pCtrl != nullptr)
		{
			i3Lua::PushBoolean(L, pCtrl->isEnable());
			return 1;
		}
	}

	return 0;
}

extern "C" int i3UI_SetCaption(lua_State *L)
{
	char szControl[ 260];
	char szCaption[ 260];
	
	if( (i3Lua::GetStringArg( L, 1, szControl, sizeof( szControl)) != nullptr) &&
		(i3Lua::GetStringArg( L, 2, szCaption, sizeof( szCaption)) != nullptr) )
	{
		i3UIControl * pCtrl = i3UI::FindControlByName( L, szControl);

		if( i3::kind_of<i3UICaptionControl*>(pCtrl))
		{
			i3UICaptionControl * pCaption = (i3UICaptionControl*)pCtrl;

			//	유니코드 문자열의 길이를 받아온다.
			INT32 length = i3String::MultiByteToWideChar( CP_UTF8, 0, szCaption, i3::generic_string_size(szCaption), nullptr, 0);

			//	유니코드 크기 만큼 버퍼 생성
			wchar_t * wTemp = (wchar_t *) i3MemAlloc( sizeof( wchar_t) * (length+1));
			I3ASSERT( wTemp != nullptr);

			wTemp[length] = 0;

			//	유니코드 버퍼로 문자열을 받아온다.
			i3String::MultiByteToWideChar( CP_UTF8, 0, szCaption, i3::generic_string_size(szCaption), wTemp, length);

			pCaption->SetText( wTemp);

			i3MemFree( wTemp);

			return 1;
		}
	}

	return 0;
}

extern "C" int i3UI_AdjustLuaPath( lua_State* L)
{
	char conv[MAX_PATH];
	char szPath[ MAX_PATH];
	
	if( i3Lua::GetStringArg( L, -1, szPath, sizeof( szPath), "") != nullptr)
	{
		const char* wf = i3ResourceFile::GetWorkingFolder();
		
		i3::snprintf( conv, sizeof(conv), "%s;%s\\?.lua", szPath, wf);

		i3Lua::PushString( L, conv);
	}

	return 1;
}

extern "C" int i3UI_PlaySound( lua_State* L)
{
	char szSoundName[ 260];
	
	if( i3Lua::GetStringArg(L, 1, szSoundName, sizeof( szSoundName), "") == nullptr)
		return 0;

	
//	i3SoundResManager * pSndMng = (i3SoundResManager*)i3ResourceFile::GetResourceManager( i3Sound::static_meta());
//  다음과 같이 i3NamedResManager로 대체해서 호출합니다..  (i3SoundResManager는 익스포트클래스가 아닌 상태임).2012.08.30.수빈.
	i3NamedResourceManager* pSndMng = i3ResourceFile::GetResourceManager( i3Sound::static_meta());
	if( pSndMng == nullptr)
		return 0;
	i3Sound * pSnd = (i3Sound*)pSndMng->GetResource( nullptr, i3ResourceFile::GetWorkingFolder(), szSoundName);
	if( pSnd == nullptr)
		return 0;

	i3Framework * pFramework = i3Framework::getCurrentFramework();
	I3ASSERT( pFramework != nullptr);

	i3Viewer * pViewer = pFramework->GetViewer();
	I3ASSERT( pViewer != nullptr);

	i3SoundContext * pSndCtx = pViewer->GetSoundContext();
	if( pSndCtx == nullptr)
		return 0;

	{
		i3SoundPlayInfo * pInfo = i3SoundPlayInfo::new_object_ref();
		pInfo->setSound( pSnd);
		pInfo->set3DMode( I3SND_RT_3DMODE_DISABLE);
		pInfo->SetDefaultVolume(100);
		pInfo->setLoopMode( I3SND_RT_LOOPMODE_ONESHOT);

		i3SoundPlayState * pState = pSndCtx->Play( pInfo);

		i3Lua::PushUserData( L, pState);
	}

	return 1;
}

extern "C" int i3UI_Trace( lua_State* L)
{
	char szStr[ 260];

	if( i3Lua::GetStringArg( L, -1, szStr, sizeof( szStr), "") != nullptr)
	{
		I3TRACE( szStr);
		I3TRACE( L"\n");
		i3Lua::Log( szStr);
	}
	
	return 0;
}

extern "C" int i3UI_ResetFocus( lua_State* L)
{
	i3Framework* pFramework = i3Framework::getCurrentFramework();
	if( pFramework != nullptr)
	{
		pFramework->ResetFocus();

		i3UIManager* pUIManager = pFramework->getUIManager();
		
		if( pUIManager != nullptr)
			pUIManager->resetOnMouseTarget();

		i3Viewer* pViewer = pFramework->GetViewer();
		SetFocus( pViewer->GetWindowHandle());
	}

	return 0;
}

extern "C" int i3UI_GetObject(lua_State *L)
{
	char szControl[MAX_PATH];
	
	i3Lua::GetStringArg( L, 1, szControl, MAX_PATH, nullptr);

	i3UIControl * pCtrl = i3UI::FindControlByName( L, szControl);
	
	if( pCtrl == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "what..!!! can't find control %s", szControl);
		return 0;
	}

	i3Lua::PushUserData(L, (void*)pCtrl);

	return 1;
}
extern "C" int i3UI_Enable(lua_State *L)
{
	i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
	INT32	nEnable		= i3Lua::GetIntegerArg( L, 2, 0);

	bool bEnable = (nEnable != 0)? true : false;

	if( pCtrl != nullptr && pCtrl->isEnable() != bEnable)
	{
		pCtrl->EnableCtrl( bEnable, true);
	}

	return 0;
}

extern "C" int i3UI_Popup(lua_State *L)
{
	i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);

	if( pCtrl != nullptr)
	{
		if( !i3::kind_of<i3UIFrameWnd*>(pCtrl))
		{
			I3TRACE("popup할수 없는 컨트롤 입니다.");
		}

		i3UIManager * pUI = i3UI::getManager();

		pUI->DoModal( (i3UIFrameWnd*)pCtrl, true, pCtrl->getEventReceiver());
	}

	return 0;
}

extern "C" int i3UI_EndModal(lua_State *L)
{
	i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);

	if( pCtrl != nullptr)
	{
		if( !i3::kind_of<i3UIFrameWnd*>(pCtrl))
		{
			I3TRACE("popup할수 없는 컨트롤 입니다.");
		}

		i3UIControl * pRecv = (i3UIControl*)i3Lua::GetUserDataArg( L, 2);

		INT32 rv = i3Lua::GetIntegerArg(L, 3, 0);

		i3UIManager * pUI = i3UI::getManager();

		pUI->EndModal((i3UIFrameWnd*)pCtrl, pRecv, (I3UI_MODALRESULT) rv);
	}

	return 0;
}

extern "C" int i3UI_Create(lua_State *L)
{
	char szStr[ 260];

	if( i3Lua::GetStringArg(L, 1, szStr, sizeof( szStr), "") == nullptr)
	{
		i3Lua::PushUserData( L, nullptr);
		return 1;
	}

	i3UIControl * pParent = (i3UIControl*)i3Lua::GetUserDataArg(L, 2);

	i3UITemplate * pUITemplate = i3UI::FindTemplateByName( szStr);

	if( pUITemplate == nullptr)
	{
		I3TRACE("couldn't find template : %s\n", szStr);	return 0;
	}

	i3UIControl * pCtrl = (i3UIControl*) pUITemplate->GetInstanceMeta()->create_instance();		// CREATEINSTANCE( pUITemplate->GetInstanceMeta());
	I3ASSERT( pCtrl != nullptr);

	i3::pack::RECT rect;
	pUITemplate->GetControlRect(&rect);
	pCtrl->SetTemplate( pUITemplate);
	pCtrl->setSize(rect.right - rect.left, rect.bottom - rect.top);

	I3_MUST_ADDREF( pCtrl);
	pCtrl->EnableCtrl(false);

	i3UIScene * pAncestor = i3UI::FindAncestor(L);

	if( pAncestor != nullptr)
	{
		pCtrl->setAncestor( pAncestor);
	}

	i3UIManager * pUI = i3UI::getManager();
	I3ASSERT( pUI != nullptr);

	if( pCtrl->getParent() == nullptr)
	{
		if( pParent != nullptr)
		{
			pParent->AddControl( pCtrl);
		}
		else if( pAncestor != nullptr)
		{
			pAncestor->AddControl ( pCtrl);
		}
		else
		{
			I3ASSERT(0);
			i3UIManager * pUI = i3UI::getManager();
			if( pUI != nullptr)
			{
				pUI->AddChild( pCtrl);
			}
		}
	}

	i3Lua::PushUserData( L, (void*)pCtrl);

	return 1;
}

extern "C" int i3UI_Delete(lua_State *L)
{
	i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);
	
	if( pCtrl != nullptr)
	{
		I3ASSERT( i3::kind_of<i3UIControl*>(pCtrl));
	
		if( pCtrl->getParent() != nullptr)
		{
			pCtrl->getParent()->RemoveChild(pCtrl);
		}

		if( pCtrl->isState( I3UI_STATE_MODAL))
		{
			i3UIManager * pUI = i3UI::getManager();
			if( pUI->getActiveFrameWnd() == pCtrl)
			{
				i3UI::EndModal( pCtrl, nullptr, I3UI_MODALRESULT_CANCEL);
			}
		}

		I3_SAFE_RELEASE( pCtrl);
	}

	return 0;
}


extern "C" int	i3UI_SetFocusControl(lua_State *L)
{
	i3UIControl * pCtrl = (i3UIControl*)i3Lua::GetUserDataArg( L, 1);

	if( pCtrl != nullptr)
	{
		I3ASSERT( i3::kind_of<i3UIControl*>(pCtrl));
		i3UIManager * pUI = i3UI::getManager();
		pUI->SetFocusControl(pCtrl);
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////
void i3UIManager::RegisterLuaFunction(LuaState * pState)
{
	for(int i=0; i3UIGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIGlue[i].name, i3UIGlue[i].func);
	}

	i3Framework::RegisterLuaFunction(pState);
	i3UIControl::RegisterLuaFunction(pState);
	i3UICaptionControl::RegisterLuaFunction(pState);
	i3UIEditBox::RegisterLuaFunction(pState);
	i3UIProgressBar::RegisterLuaFunction(pState);
	i3UIProgressBarImage::RegisterLuaFunction(pState);
	i3UIProgressBarSplit::RegisterLuaFunction(pState);
	i3UIListView_Box::RegisterLuaFunction( pState);
	i3UICheckBox::RegisterLuaFunction(pState);
	i3UIRadio::RegisterLuaFunction(pState);
	i3UIScrollBar::RegisterLuaFunction(pState);		// 11.05.19.수빈 루아함수 몇개가 필요해 급히 만듭니다.
	i3UIButton::RegisterLuaFunction(pState);

	i3UIComboBase::RegisterLuaFunction( pState);

	i3UIMessageBox::RegisterLuaFunction( pState);
	i3UISpin::RegisterLuaFunction( pState);
	i3UIListBox::RegisterLuaFunction( pState);
	i3UITab::RegisterLuaFunction( pState);

	i3UIFrameWnd::RegisterLuaFunction( pState);

	i3UIButtonImageSet::RegisterLuaFunction( pState);
	i3UIButton::RegisterLuaFunction(pState);	// 11.07.08.수빈.
	i3UIButtonComposed3::RegisterLuaFunction( pState);	// 11.07.05. 수빈. 

	i3UIImageBoxEx::RegisterLuaFunction( pState);
	i3UIUserControl::RegisterLuaFunction( pState);

	//
	if( m_pUILibrary != nullptr)
	{
		m_pUILibrary->RegisterScript(pState);
	}

	//
	i3Stage * pStage = m_pFramework->GetCurrentStage();
	if( pStage != nullptr)
	{
		pStage->RegisterLuaFunction( pState);
	}
}

int _rec_dofile( i3UIControl * pRoot, LuaState * pState)
{
	if( pRoot->getScript() != nullptr)
	{
		i3Lua::DoFile( pState, pRoot->getScript());
	}

	i3UIControl * pChild = (i3UIControl*)pRoot->getFirstChild();
	while( pChild != nullptr)
	{
		_rec_dofile( pChild, pState);

		pChild = (i3UIControl*)pChild->getNext();
	}

	return 1;
}

extern "C" int i3UI_Initialize(lua_State *L)
{
	i3UIManager * pManager = i3UI::getManager();
	I3ASSERT( pManager != nullptr);

	i3GameNode * pNode = pManager->getFirstChild();
	while( pNode != nullptr)
	{
		if( i3::same_of<i3UIScene*>(pNode))
		{
			i3UIControl * pCtrl = (i3UIControl*)pNode->getFirstChild();
			while( pCtrl != nullptr)
			{
				_rec_dofile( pCtrl, L);

				pCtrl = (i3UIControl*)pCtrl->getNext();
			}
		}

		pNode = pNode->getNext();
	}
	
	return 1;
}


/*
void i3UIManager::SendEvent( INT32 code, const char * pszEventHandler, const char * arg1, INT32 arg2, INT32 arg3, INT32 arg4)
{
	if( m_pLuaContext == nullptr)	return;
	if( pszEventHandler == nullptr)	return;

	char command[MAX_PATH];

	i3::snprintf( command, MAX_PATH-1, "%s(\"%s\",%d,%d,%d,%d);", pszEventHandler, arg1,code,arg2, arg3, arg4);
	I3TRACE("cmd: %s\n", command);

	i3Lua::DoString( m_pLuaContext, command);
}
*/

bool i3UIManager::Compile( const char* pszFilename, char* pOutResult, INT32 nBufSize )
{
	/*
	if( m_pLuaContext == nullptr)
		OpenLuaContext();
	
	INT32 result = i3Lua::LoadFileRaw( m_pLuaContext, pszFilename);	// used 'LoadFileRaw' to handle error here

	if( result != 0)	// error
	{
		const char* msg = i3Lua::GetStringArg( m_pLuaContext, -1, "No Error Msg in the Stack\n");
		i3::string_ncopy_nullpad( pOutResult, msg, nBufSize);

		CloseLuaContext();
		
		return false;
	}
	
	i3::string_ncopy_nullpad( pOutResult, "Successful\n", nBufSize);

	CloseLuaContext();
	*/

	return true;
}

LuaFuncReg i3UIGlue[] = 
{
  {"uiSetEventHandler",		i3UI_SetEventHandler},
  {"uiOnCommand",			i3UI_OnCommand},
  {"uiRunUI",				i3UI_RunGUI},

  //
  {"uiCreate",				i3UI_Create},
  {"uiDelete",				i3UI_Delete},

  {"uiEnable",				i3UI_EnableObject},
  {"uienableObj",			i3UI_Enable},
  {"uiGetEnable",			i3UI_GetEnableObject},
  {"uiSetCaption",			i3UI_SetCaption},
  {"uiGetObject",			i3UI_GetObject},
  {"uiDoModal",				i3UI_Popup},
  {"uiEndModal",			i3UI_EndModal},

  {"uiPlaySound",			i3UI_PlaySound},

  {"uiAdjustLuaPath",		i3UI_AdjustLuaPath},
  {"uiInitialize",			i3UI_Initialize},
  
  {"uiTrace",				i3UI_Trace},

  {"uiResetFocus",			i3UI_ResetFocus},
  { "uiSetFocusControl",	i3UI_SetFocusControl },
  /*
  setfont
  setstring
  setalign
  */

  {nullptr, nullptr}
};