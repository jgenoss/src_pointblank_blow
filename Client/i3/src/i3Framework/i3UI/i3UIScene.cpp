#include "i3FrameworkPCH.h"
#include "i3UIScene.h"
#include "i3UIManager.h"
#include "../i3Framework.h"
#include "i3UIToolTip.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/algorithm/replace_all.h"

#if !defined( I3_DEBUG)
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/extract_filename.h"
#endif
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/contain_string.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

#include "i3UIFilePathMgr.h"


namespace 
{
	std::vector<std::string>	g_vec_str_lua_fun;	
	std::vector<LuaFunction>	g_vec_lua_fun;
}

void	i3UIScene::PreRegisterLuaFunction(const char* szFunName, LuaFunction Fun)
{
	g_vec_str_lua_fun.push_back(szFunName);
	g_vec_lua_fun.push_back(Fun);
}

I3_CLASS_INSTANCE( i3UIScene);

i3UIScene::i3UIScene(void)
{
	m_UpdateInterval = 0.1f;
}

i3UIScene::~i3UIScene(void)
{
	CloseLuaContext();
}

///////////////////////////////////////////////////////////////////////////////////
void i3UIScene::OpenLuaContext()
{
	if( m_pLuaContext != nullptr)
	{
		i3Lua::Close(m_pLuaContext);
	}

	// lua state open
	m_pLuaContext = i3Lua::Create(0);
	I3ASSERT( m_pLuaContext != nullptr);

	//Register Lua Package Path
	{
		char command[MAX_PATH];

#if defined( I3_DEBUG) && 0 // Lua Script는 WorkingFolder에서 찾도록 합니다. -- swoongoo
		// Debug일때는 Engine Script를 사용합니다.
		char enginePath[MAX_PATH];

		i3System::ResolvePath("$(I3_ROOT)", enginePath, MAX_PATH);

		i3::snprintf( command, MAX_PATH, "package.path =package.path ..\";%s\\?.lua;%s\\?.lua\"", enginePath, i3ResourceFile::GetWorkingFolder());
#else
		// Release는 하위폴더의 Script를 찾습니다.
		i3::snprintf( command, MAX_PATH, "package.path =package.path ..\";%s\\?.lua\"", i3ResourceFile::GetWorkingFolder());
#endif

		i3::make_unix_path(command);

		i3Lua::DoString(m_pLuaContext, command);
	}

	//
	i3Lua::ProcessRequireFromPreload(m_pLuaContext);
	//

	i3UIScene::RegisterLuaFunction( m_pLuaContext);

	i3UI::RegisterLuaFunction( m_pLuaContext);



}

void i3UIScene::CloseLuaContext(void)
{
	if( m_pLuaContext != nullptr)
	{
		i3Lua::Close(m_pLuaContext);
	}

	m_pLuaContext = nullptr;
}

void i3UIScene::ResetUI(void)
{
	if( m_pLuaContext == nullptr)	return;

	i3Lua::DoString(m_pLuaContext, "ResetUI()");

	CloseLuaContext();
}

bool  i3UIScene::StartGUI(const char *pFilenameIn)
{
	INT32 result = 0;

	if( i3UI::getToolDevMode())
	{
		char szFull[MAX_PATH], szRel[MAX_PATH];
		char * pszWorkingFolder = i3ResourceFile::GetWorkingFolder();

		if( *pszWorkingFolder == 0)
		{
			::GetCurrentDirectory( MAX_PATH -1, szRel);
			i3::snprintf( szFull, MAX_PATH -1, "%s\\%s", szRel, pFilenameIn);
		}
		else
		{
			i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), pFilenameIn, szRel);
			i3::snprintf( szFull, MAX_PATH -1, "%s\\%s", i3ResourceFile::GetWorkingFolder(), szRel);
		}

		result = i3Lua::DoFile( m_pLuaContext, szFull);
	}
	else
	{
		// Release에서는 Memory에 Load하여 사용합니다.
		//		if( m_pLuaModule == nullptr)
		if (m_bLoadedBuffer == false)
		{

	
			i3::stack_string strUniquePath;
			i3UIFilePathMgr::i()->ObtainUniquePath(pFilenameIn, strUniquePath);

			i3::rc_string strCommonUIRoot = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(true);

			// 앞에 \붙는 UI들이 있습니다. 그 \를 지워야만 정상 작동합니다.
			INT32 contain_count = i3::contain_string(strUniquePath, "\\");
			if (contain_count == 0)
			{
				strUniquePath.erase(0, 1);
			}

			i3::stack_string strFullPath = strCommonUIRoot;
			strFullPath += '/';
			strFullPath += strUniquePath;

			i3::make_unix_path(strFullPath);

			bool bPreload = i3Lua::LoadFileFromPreload(m_pLuaContext, strFullPath.c_str() );

			if (!bPreload)
			{
				i3FileStream file;
				
				i3::stack_string strBinaryFullPath = strFullPath;
				i3::change_file_ext(strBinaryFullPath, "luab");
				
				// 1st 파일이 존재하지만 실패한 경우는 2nd로 넘기지 말고 그냥 실패로 간주함.
#if defined(I3_DEBUG)
				const i3::stack_string& filepath_1st = strFullPath;			// 디버그모드는 lua파일 우선처리..
				const i3::stack_string& filepath_2nd = strBinaryFullPath;
#else
				const i3::stack_string& filepath_1st = strBinaryFullPath;	// 릴리즈모드는 luab파일 우선 처리..
				const i3::stack_string& filepath_2nd = strFullPath;
#endif
				if (file.isFile(filepath_1st.c_str()))
				{
					if (file.Open(filepath_1st, STREAM_READ | STREAM_SHAREREAD) == false)
					{
						I3PRINTLOG(I3LOG_WARN, "Can't open %s file.", filepath_1st);
						return false;
					}
				}
				else
				if (file.isFile(filepath_2nd.c_str() ))
				{
					if (file.Open(filepath_2nd, STREAM_READ | STREAM_SHAREREAD) == false)
					{
						I3PRINTLOG(I3LOG_WARN, "Can't open %s file.", filepath_2nd);
						return false;
					}
				}
				else
				{
					I3PRINTLOG(I3LOG_WARN, "Can't open %s file.", filepath_1st);
					return false;
				}

				INT32 size = file.GetSize();
				char* pLuaModule = (char*) i3MemAlloc( sizeof(char) * size);

				file.Read( pLuaModule, sizeof(char) * size);

				file.Close();

				i3Lua::LoadBuffer( m_pLuaContext, pLuaModule, sizeof(char)*size, strFullPath.c_str() );

				I3MEM_SAFE_FREE( pLuaModule);
			}

			m_bLoadedBuffer = true;
		}

		result = i3Lua::PCall( m_pLuaContext, 0, LUA_MULTRET, 0);
	}

	if( result != 0)
		return false;
	else
		return true;
}

///////////////////////////////////////////////////////////////////////////////////
void	i3UIScene::_ForceEnable( bool bFlag)
{
	i3GameNode::_ForceEnable( bFlag);
	
	i3UIControl * pChild = (i3UIControl*)getFirstChild();
	while( pChild != nullptr)
	{
		pChild->EnableCtrl(bFlag, true);

		pChild = (i3UIControl*)pChild->getNext();
	}
}

///////////////////////////////////////////////////////////////////////////////////
void i3UIScene::AddControl( i3UIControl * pCtrl, bool bAddToGg)
{
	// Game Graph에 연결
	if( bAddToGg)
	{
		i3GameNode::AddChild( pCtrl);
	}

	// Child의 모든 Sprite들을 Parent::Sprite들의 후미로 이동
	if( i3UI::getToolDevMode())
	{
		INT32 idxEnd = -1;

		i3UIControl * pChild = (i3UIControl*)getFirstChild();
		while( pChild != nullptr)
		{
			I3ASSERT( i3::kind_of<i3UIControl*>(pChild)); 

			idxEnd = pChild->AdjustSpriteOrder( idxEnd);

			pChild = (i3UIControl*)pChild->getNext();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
void i3UIScene::setScriptFile( const char * pszFileName)
{
	if( pszFileName != nullptr && pszFileName[0] != 0 )
	{
		//라이브러리 경로 체크
		i3::stack_string strUniqueName;
		i3UIFilePathMgr::i()->ObtainUniquePath(pszFileName, strUniqueName);
		
		m_strStartupScript = strUniqueName;
	}
	else
	{
		m_strStartupScript.clear(); 
	}
}

const char* i3UIScene::getScriptFile(void) const
{
	if( m_strStartupScript.empty() ) 
		return nullptr;
	else
		return m_strStartupScript.c_str();
}

///////////////////////////////////////////////////////////////////////////////////
bool i3UIScene::Attach( bool bRunScript, HINSTANCE inst)
{
	i3UIManager * pUI = i3UI::getManager();
	I3ASSERT( pUI != nullptr);

	if( getParent() == nullptr)
	{
		pUI->AddChild( this);
	}
	else
	{
		this->ChangeParent( pUI);
	}

	if( bRunScript && !m_strStartupScript.empty() )
	{
		if( getLuaContext() == nullptr)
		{
			OpenLuaContext();

			//
			const size_t num_pre_fun = g_vec_lua_fun.size();
			for (size_t i = 0 ; i < num_pre_fun ; ++i)
			{
				i3Lua::RegisterFunc(m_pLuaContext, g_vec_str_lua_fun[i].c_str(), g_vec_lua_fun[i]);
			}

			// UIScene은 국가 폴더에 관계 없이 공통으로 로딩한다.
			char szPath[ MAX_PATH];
			i3::safe_string_copy( szPath, m_strStartupScript.c_str(), sizeof( szPath));

			if( StartGUI( szPath) == false)
			{
				return false;
			}
		}
	}

	//I3TRACE("start Gui Succeeded\n");

	return true;
}

void i3UIScene::Detach(void)
{
	i3_prof_func();

	i3UIManager* pManager = i3UI::getManager();

	if( pManager != nullptr)
	{
		i3EventReceiver* f = pManager->GetFocus();
		if (i3::kind_of<i3UIControl*>(f)) 
		{
			i3UIControl* cf = static_cast<i3UIControl*>(f);
			if (cf->getAncestor() == this)
				pManager->SetFocusControl(nullptr);
			//pManager->DispatchKillFocus();
		}

		i3UIControl* mouse_target =	pManager->getCurrMouseTarget();
		if (mouse_target == nullptr || mouse_target->getAncestor() == this )		// 지워지는 컨트롤에 대해서만 리셋한다고 가정하고 우선 수정 코딩 작업..(11.07.08.수빈)
			pManager->resetOnMouseTarget();
	}

	if( g_pUIRenderer != nullptr)
	{
		ResetUI();
	}

	if( getParent() != nullptr)
	{
		getParent()->RemoveChild( this);
	}
}
/*
const void _RecEnableScene( i3UIControl* pNode, bool bEnable)
{
	pNode->SetEnable( bEnable);

	i3UIControl* pControl = (i3UIControl*)pNode->getFirstChild();
	while( pControl != nullptr)
	{
		_RecEnableScene(pControl, bEnable);

		pControl = (i3UIControl*)pControl->getNext();
	}
}
*/
void i3UIScene::EnableScene( bool bEnable )
{
	i3UIControl* pControl = (i3UIControl*)getFirstChild();

	while( pControl != nullptr)
	{
		//_RecEnableScene(pControl, bEnable);
		pControl->EnableCtrl( bEnable, true);

		pControl = (i3UIControl*)pControl->getNext();
	}

	i3GameNode::SetEnable( bEnable);

	// tool tip은 입력에 의해 enable되지 않는한 절대로 true로 되지 않는다.
	if( m_pDefToolTip != nullptr)
	{
		m_pDefToolTip->EnableCtrl( false);
	}
}

void i3UIScene::ShowTop( void)
{
	i3UIControl * pControl = (i3UIControl*) getFirstChild();

	I3ASSERT( pControl != nullptr);

	pControl->ShowTop();
}


void i3UIScene::OnEvent( INT32 code, INT32 arg1, INT32 arg2, INT32 arg3)
{
	if( m_pLuaContext == nullptr)	return;

	i3Lua::GetGlobal( m_pLuaContext, GetName());

	INT32 type = i3Lua::StackIsType( m_pLuaContext, -1);

	if( type == LUA_TNIL)
	{
		i3Lua::SetError( m_pLuaContext, "couldn't find %s field\n", GetName());
		return;
	}

	if (type!=LUA_TTABLE && type!=LUA_TUSERDATA)// ??? 현재 user type으로 되어있는데 luabind때문일것 으로 보고 있음..하지만 맞는건지 확신없음.
	{
		i3Lua::SetError( m_pLuaContext, "%s isn't a valid type", GetName());
		return;
	}

	i3Lua::GetField(m_pLuaContext, -1, "OnEvent");
	if (i3Lua::StackIsType(m_pLuaContext, -1)!=LUA_TFUNCTION)
	{
		i3Lua::SetError( m_pLuaContext, "%s doesn't have %s function ", GetName(), "OnEvent");
		return;
	}

	i3Lua::StackReplace(m_pLuaContext, -2); // This removes the phaseLobby table from the stack

	// traceback function
	INT32 top = i3Lua::StackGetTop( m_pLuaContext);

	//i3Lua::PushCFunction( m_pLuaContext, i3Lua::TraceCallStack);	// // 호출하고자하는 함수 밑에다 _TRACEBACK 함수를 집어넣는다.
	i3Lua::StackInsert(m_pLuaContext, top);

	//self
	i3Lua::GetGlobal(m_pLuaContext, GetName());
	i3Lua::StackInsert(m_pLuaContext, -1);
	i3Lua::PushInteger(m_pLuaContext, code);
	i3Lua::PushInteger(m_pLuaContext, arg1);
	i3Lua::PushInteger(m_pLuaContext, arg2);
	i3Lua::PushInteger(m_pLuaContext, arg3);

#if defined( I3_DEBUG)
	//i3Lua::TraceStack(m_pLuaContext);
#endif

	INT32 ret = i3Lua::PCall( m_pLuaContext, 5, 1, 0);
	if( ret != 0)
	{
		I3PRINTLOG(I3LOG_NOTICE, "failed calling function ");
	}
	i3Lua::StackPop(m_pLuaContext, -1);
}

void i3UIScene::OnSendEvent( INT32 code, i3UIControl * pCtrl, INT32 arg1, INT32 arg2, INT32 arg3)
{
	if( m_pLuaContext == nullptr)	return;
	if( pCtrl == nullptr)	return;

	INT32 evtidx = 0;
	for( INT32 i=0; i<I3UI_EVT_MAXCOUNT; i++)
	{
		if( s_EventInfoTable[i].m_Code == (I3UI_EVT)code)
		{
			evtidx = i;
			break;
		}
	}

	i3Lua::GetGlobal( m_pLuaContext, pCtrl->GetName());

	INT32 type = i3Lua::StackIsType( m_pLuaContext, -1);
	if( type == LUA_TNIL)
	{
		i3Lua::SetError( m_pLuaContext, "couldn't find %s field\n", pCtrl->GetName());
		return;
	}

	if (type!=LUA_TTABLE && type!=LUA_TUSERDATA)// ??? 현재 user type으로 되어있는데 luabind때문일것 으로 보고 있음..하지만 맞는건지 확신없음.
	{
		i3Lua::SetError( m_pLuaContext, "%s isn't a valid type", pCtrl->GetName());
		return;
	}

	i3Lua::GetField(m_pLuaContext, -1, s_EventInfoTable[evtidx].m_pszEvent);
	if (i3Lua::StackIsType(m_pLuaContext, -1)!=LUA_TFUNCTION)
	{
		i3Lua::SetError( m_pLuaContext, "%s doesn't have %s function ", pCtrl->GetName(), s_EventInfoTable[evtidx].m_pszEvent);
		return;
	}

	i3Lua::StackReplace(m_pLuaContext, -2); // This removes the phaseLobby table from the stack

	// traceback function
	INT32 top = i3Lua::StackGetTop( m_pLuaContext);

	//i3Lua::PushCFunction(m_pLuaContext, i3Lua::TraceCallStack);	// // 호출하고자하는 함수 밑에다 _TRACEBACK 함수를 집어넣는다.
	i3Lua::StackInsert(m_pLuaContext, top);

	//self
	i3Lua::GetGlobal(m_pLuaContext, pCtrl->GetName());
	i3Lua::StackInsert(m_pLuaContext, -1);
	i3Lua::PushInteger(m_pLuaContext, arg1);
	//i3Lua::PushUserData(m_pLuaContext, (void*)arg2);	// from, arg2, arg3
	i3Lua::PushInteger(m_pLuaContext, arg2);
	i3Lua::PushInteger(m_pLuaContext, arg3);

#if defined( I3_DEBUG)
	//i3Lua::TraceStack(m_pLuaContext);
#endif

	INT32 ret = i3Lua::PCall( m_pLuaContext, 4, 1, 0);
	if( ret != 0)
	{
		I3PRINTLOG(I3LOG_NOTICE, "failed calling function ");
	}
	i3Lua::StackPop(m_pLuaContext, -1);
}

void i3UIScene::OnSendNotify( INT32 code, i3UIControl * pRecv, i3UIControl * pFrom, INT32 arg1, INT32 arg2)
{
	if( m_pLuaContext == nullptr)	return;
	if( pFrom == nullptr || pRecv == nullptr)	return;

	i3Lua::GetGlobal(m_pLuaContext, pRecv->GetName());
	INT32 type = i3Lua::StackIsType(m_pLuaContext, -1);
	if( type == LUA_TNIL)
	{
		i3Lua::SetError( m_pLuaContext, "couldn't find %s field\n", pRecv->GetName());
		return;
	}

	if (type!=LUA_TTABLE && type !=LUA_TUSERDATA)//현재 user type으로 되어있는데 luabind때문일것 으로 보고 있음..하지만 맞는건지 확신없음.
	{
		i3Lua::SetError( m_pLuaContext, "%s isn't a valid type\n", pRecv->GetName());
		return;
	}

	i3Lua::GetField(m_pLuaContext, -1, "OnParentNotify");
	if (i3Lua::StackIsType(m_pLuaContext, -1)!=LUA_TFUNCTION)
	{
		i3Lua::SetError( m_pLuaContext, "%s doesn't have %s function ", pRecv->GetName(), "OnParentNotify");
		return;
	}

	i3Lua::StackReplace(m_pLuaContext, -2); // This removes the phaseLobby table from the stack

	// traceback function
	INT32 top = i3Lua::StackGetTop(m_pLuaContext);

	//i3Lua::PushCFunction(m_pLuaContext, i3Lua::TraceCallStack);	// // 호출하고자하는 함수 밑에다 _TRACEBACK 함수를 집어넣는다.
	i3Lua::StackInsert(m_pLuaContext, top);

	//self
	i3Lua::GetGlobal(m_pLuaContext, pRecv->GetName());
	i3Lua::StackInsert(m_pLuaContext, -1);
	i3Lua::PushInteger(m_pLuaContext, code);
	i3Lua::PushUserData(m_pLuaContext, (void*)pFrom);
	i3Lua::PushInteger(m_pLuaContext, arg1);
	i3Lua::PushInteger(m_pLuaContext, arg2);

#if defined( I3_DEBUG)
	//i3Lua::TraceStack(m_pLuaContext);
#endif

	INT32 ret = i3Lua::PCall( m_pLuaContext, 5, 1, 0);
	if( ret != 0)
	{
		I3PRINTLOG(I3LOG_NOTICE, "failed calling function ");
	}
	i3Lua::StackPop(m_pLuaContext, -1);
}

void i3UIScene::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameNode::OnUpdate( rDeltaSeconds);

	m_timeUpdate += rDeltaSeconds;
	if( m_UpdateInterval > m_timeUpdate)
	{
		return;
	}

	const char * pszName = GetName();

	if( m_pLuaContext != nullptr && pszName[0] != 0)
	{
		i3Lua::GetGlobal( m_pLuaContext, pszName);

		INT32 type = i3Lua::StackIsType(m_pLuaContext, -1);
		if( type == LUA_TNIL)
		{
			i3Lua::SetError( m_pLuaContext, "couldn't find %s field\n", pszName);
			return;
		}

		if (type!=LUA_TTABLE && type!=LUA_TUSERDATA)// 현재 user type으로 되어있는데 luabind때문일것 으로 보고 있음..하지만 맞는건지 확신없음.
		{
			i3Lua::SetError( m_pLuaContext, "%s module is not a valid type");
			return;
		}

		i3Lua::GetField(m_pLuaContext, -1, "OnUpdate");
		if (i3Lua::StackIsType(m_pLuaContext, -1)!=LUA_TFUNCTION)
		{
			i3Lua::SetError( m_pLuaContext, "%s doesn't have %s function ", pszName, "OnUpdate");
			return;
		}

		i3Lua::StackReplace(m_pLuaContext, -2); // This removes the phaseLobby table from the stack

		// traceback function
		INT32 top = i3Lua::StackGetTop(m_pLuaContext);

		//i3mem::Check_S();
		//i3Lua::PushCFunction(m_pLuaContext, i3Lua::TraceCallStack);	// // 호출하고자하는 함수 밑에다 _TRACEBACK 함수를 집어넣는다.
		//i3mem::Check_E(__FILE__,__LINE__);
		
		i3Lua::StackInsert(m_pLuaContext, top);

		//self
		i3Lua::GetGlobal(m_pLuaContext, pszName);
		i3Lua::StackInsert(m_pLuaContext, -1);
		
		i3Lua::PushNumber(m_pLuaContext, m_timeUpdate);

#if defined( I3_DEBUG)
		//i3Lua::TraceStack(m_pLuaContext);
#endif

		INT32 ret = i3Lua::PCall( m_pLuaContext, 2, 1, 0);
		if( ret != 0)
		{
			I3PRINTLOG(I3LOG_NOTICE, "failed calling function ");
		}
		/*i3mem::Check_S();
		i3Lua::FreeCFunction( m_pLuaContext);
		i3mem::Check_E(__FILE__,__LINE__);*/
		//i3Lua::StackPop(m_pLuaContext, top);
		i3Lua::StackPop(m_pLuaContext, -1);
	}

	m_timeUpdate = 0.0f;// m_timeUpdate - m_UpdateInterval --> delta값이 너무 큰경우 매프레임 실행될 수 있어서..쌓인 시간은 걍 패쑤.하도록

}

void i3UIScene::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3GameNode::OnBuildObjectList( List);
}

void i3UIScene::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameNode::CopyTo( pDest, method);

	i3UIScene * pDestScene = (i3UIScene*) pDest;

	pDestScene->setWidth( getWidth());
	pDestScene->setHeight( getHeight());

	pDestScene->m_Depth = m_Depth;

	pDestScene->m_strStartupScript = m_strStartupScript;
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 4)					// [test required] 패킹 사이즈가 명시되지 않아 추가합니다. 2017.02.09 soon9
#endif

namespace pack
{
	struct UI_SCENE_HEADER
	{
		UINT8	_ID[4] = { 'U', 'I', 'S', '1' };
		UINT32	_ScriptPathLength = 0;
		UINT32	_LibraryPathLength = 0;
		INT32	_Depth = 0;
		INT32	_ResolutionX = 0;
		INT32	_ResolutionY = 0;
		UINT32	_pad[10] = { 0 };
	};	// size 64
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

UINT32 i3UIScene::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	Rc = i3GameNode::OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	pack::UI_SCENE_HEADER head;

	head._ScriptPathLength = m_strStartupScript.length();

	i3UIManager * pUI = i3UI::getManager();
	i3UILibrary * pUILib = pUI->getUILibrary();

	i3::stack_string strOldStyleUILibName;

	if( pUILib == 0)
	{
		head._LibraryPathLength = 0;
	}
	else
	{			// 구버전 씬파일인 경우엔 자동으로 UIRe/UI_Temp등이 들어가야함..
		i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(pUILib->GetNameString(), strOldStyleUILibName);
		head._LibraryPathLength = strOldStyleUILibName.size(); //  i3::generic_string_size(pUILib->GetNameString());
	}

	head._ResolutionX = m_nWidth;
	head._ResolutionY = m_nHeight;
	head._Depth = m_Depth;

	Rc = pResFile->Write( &head, sizeof( head));
	I3_CHKIO( Rc);
	Result += Rc;

	if( head._ScriptPathLength > 0)
	{
		Rc = pResFile->Write( m_strStartupScript.c_str(), head._ScriptPathLength);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	if( head._LibraryPathLength > 0)
	{
	//	Rc = pResFile->Write( pUILib->GetName(), head._LibraryPathLength);
		Rc = pResFile->Write(strOldStyleUILibName.c_str(), head._LibraryPathLength);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 i3UIScene::OnLoad( i3ResourceFile * pResFile)
{
i3_prof_func();

	UINT32 Rc, Result = 0;
	pack::UI_SCENE_HEADER head;
	char szTemp[ MAX_PATH];

i3_prof_start("i3GameNode::OnLoad( pResFile) call");

	Rc = i3GameNode::OnLoad( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

i3_prof_end();


i3_prof_start("fileheader reading and pUI->SetCurrentScenePath call");

	Rc = pResFile->Read( &head, sizeof( head));
	I3_CHKIO( Rc);
	Result += Rc;

	i3UIManager * pUI = i3UI::getManager();

	pUI->SetCurrentScenePath( pResFile->getFileName());

i3_prof_end();

i3_prof_start("setScriptFile( szTemp) process");

	if( head._ScriptPathLength > 0)
	{
		Rc = pResFile->Read( szTemp, head._ScriptPathLength);
		I3_CHKIO( Rc);
		Result += Rc;
		szTemp[head._ScriptPathLength] = 0;
		setScriptFile( szTemp);
	}

i3_prof_end();


i3_prof_start("pUI->OpenTemplateLibrary( szTemp) process");

	if( head._LibraryPathLength > 0)
	{
		Rc = pResFile->Read( szTemp, head._LibraryPathLength);
		I3_CHKIO( Rc);
		Result += Rc;
		szTemp[head._LibraryPathLength] = 0;

		//
		i3::stack_string strLibName_V3Style;
		i3UIFilePathMgr::i()->ConvertPathToV3UIStyle(szTemp, strLibName_V3Style);
		//
		if( pUI->LoadTemplateLibrary(strLibName_V3Style.c_str()) == false)
			return STREAM_ERR;
	}

i3_prof_end();

	m_nWidth = head._ResolutionX;
	m_nHeight = head._ResolutionY;

i3_prof_start("pUI->SetUIResolution process");
	pUI->SetUIResolution( m_nWidth, m_nHeight);
i3_prof_end();

	m_Depth = head._Depth;

	return Result;
}

bool i3UIScene::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRet = i3GameNode::OnSaveXML( pFile, pXML);

	pXML->addAttr( "Script", m_strStartupScript.c_str());

	i3UIManager * pUI = i3UI::getManager();
	I3ASSERT( pUI != nullptr);

	i3UILibrary * pUILib = pUI->getUILibrary();
	if( pUILib == 0)
	{
		pXML->addAttr( "libPath", "");
	}
	else
	{
		i3::stack_string strOldStyleUILibName;
		i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(pUILib->GetNameString(), strOldStyleUILibName);
	
	//	pXML->addAttr( "libPath", pUILib->GetName());
		pXML->addAttr("libPath", strOldStyleUILibName.c_str());
	}

	pXML->addAttr( "resolutionX", m_nWidth);
	pXML->addAttr( "resolutionY", m_nHeight);

	pXML->addAttr( "Depth", m_Depth);

	return bRet;
}

bool i3UIScene::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRet = i3GameNode::OnLoadXML( pFile, pXML);

	i3UIManager * pUI = i3UI::getManager();

	pUI->SetCurrentScenePath( pFile->getFileName());

	const char * pszScript = pXML->getAttr( "Script");
	if( pszScript != nullptr)
		setScriptFile( pszScript);

	const char * pszlib = pXML->getAttr( "libPath");
	if( pszlib != nullptr)
	{
		//
		i3::stack_string strLibName_V3Style;
		i3UIFilePathMgr::i()->ConvertPathToV3UIStyle(pszlib, strLibName_V3Style);
		//
		bRet = pUI->LoadTemplateLibrary(strLibName_V3Style.c_str());
		if( bRet == false)
			return false;
	}

	pXML->getAttr( "resolutionX", &m_nWidth);
	pXML->getAttr( "resolutionY", &m_nHeight);

	pUI->SetUIResolution(m_nWidth, m_nHeight);

	pXML->getAttr( "Depth", &m_Depth);

	return bRet;
}

#if defined( I3_DEBUG)
void i3UIScene::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	i3GameNode::GetTDKGridProperties( List);	// 먼저 Base Class의 GetTDKGridProperties를 호출

	m_bPreloadingEnable = isGNodeState(I3_GAMENODE_STATE_PRE_LOADING);

	AddMFCProperty_BeginGroup( List, "i3UIScene");
	AddMFCProperty_RcString( List, m_strStartupScript, "Start Up Script", nullptr, true);
	AddMFCProperty_Int32( List, &m_Depth, "Depth", "Scene간의 Depth값(높을수록 우선권이 높아진다.)");
	AddMFCProperty_Bool( List, &m_bPreloadingEnable, "PreLoad", "Scene이 Load될때 필요한 리소스들을 함께 로드한다.");
	AddMFCProperty_EndGroup( List);
}

void i3UIScene::OnTDKPropertyChanged( const char* szName)
{
	i3GameNode::OnTDKPropertyChanged( szName);

	if( i3::generic_is_niequal( szName, "PreLoad", MAX_PATH) )
	{
		if( m_bPreloadingEnable)
		{
			addGNodeState( I3_GAMENODE_STATE_PRE_LOADING);
		}
		else
		{
			removeGNodeState( I3_GAMENODE_STATE_PRE_LOADING);
		}
	}
}
#endif	// I3_DEBUG

void i3UIScene::CreateControl( i3UIControl* pControl)
{

}

void i3UIScene::DeleteControl( i3UIControl* pControl)
{

}

/////////////////////////////////////////////////////////////////////////////////////
extern "C" int i3UIS_setToolTip( lua_State* L)
{
	char szStr[ 260];

	if(i3Lua::GetStringArg( L, 2, szStr, sizeof( szStr), "i3UIS_setToolTip") != nullptr)
	{
		i3UIManager* pManager = i3UI::getManager();
		I3ASSERT( pManager != nullptr);

		i3UIControl* pCtrl = pManager->FindChildControlByName( L, szStr);

		if(  i3::same_of<i3UIToolTip*>(pCtrl)) 
		{
			i3UIScene* pScene = pCtrl->getAncestor();
			if( pScene != nullptr)
				pScene->setDefToolTip( (i3UIToolTip*)pCtrl);

			return 1;
		}
	}

	return 0;
}

LuaFuncReg i3UISceneGlue[] = 
{
	{"uiS_setToolTip",		i3UIS_setToolTip},

	{nullptr, nullptr}
};

void i3UIScene::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UISceneGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UISceneGlue[i].name, i3UISceneGlue[i].func);
	}
}

void i3UIScene::OnDraw( i3UIRenderer * pRenderer)
{
	i3GameNode * pChild = getFirstChild();

	while( pChild != nullptr)
	{
		if( i3::kind_of<i3UIControl*>(pChild))
		{
			i3UIControl * pCtrl = (i3UIControl *) pChild;

			pCtrl->OnDraw( pRenderer);
		}

		pChild = pChild->getNext();
	}
}

// UITool의 Edit Mode에서 Rendering 시키기 위한 Interface function
void i3UIScene::RenderCtrl(void)
{
	OnDraw( g_pUIRenderer);
}

/////////////////////////////////////////////////
// Test Automation
void i3UIScene::TA_GetCtrlPath(char * pszPath, INT32 limit)
{
	// UIScene의 Parent는 UIManager이므로, 더 상위로 Recursive하게 들어갈 필요가 없다.

	i3::safe_string_cat(pszPath, GetName(), limit);
}

i3UIControl * i3UIScene::TA_FindCtrlByPath(const char * pszPath)
{
	// Path에서 구분자(/)가 발견될 때까지의 이름을 구해낸다.
	char id[128];

	const char * pos = strchr(pszPath, '/');
	
	if (pos != nullptr)
	{
		int len = pos - pszPath;

		memcpy(id, pszPath, len);

		id[len] = 0;		// Null-Termination
	}
	else
	{
		// nullptr이라면 마지막이란 뜻이므로...
		// strcpy_s(id, pszPath);

		// Scene은 마지막일 수 없기 때문에...잘못된 경로로 간주.
		return nullptr;
	}

	if (i3::generic_icompare(GetName(), id) == 0)
	{
		// 이 Scene에 있다!
		i3GameNode * pChild = getFirstChild();

		while (pChild != nullptr)
		{
			if (pChild->kind_of(i3UIControl::static_meta()))
			{
				i3UIControl * pCtrl = (i3UIControl *)pChild;

				i3UIControl * pFind = pCtrl->TA_FindCtrlByPath(pos + 1);		// 구분자(/)가 발견된 다음부터 경로를 전달한다.

				if (pFind != nullptr)
					return pFind;
			}

			pChild = pChild->getNext();
		}
	}

	// 이 Scene이 아니니 nullptr return
	return nullptr;
}

i3UIControl * i3UIScene::TA_ScanText( const wchar_t * pszText, bool bExact)
{
	i3GameNode * pChild = getFirstChild();

	while( pChild != nullptr)
	{
		if( pChild->isEnable())
		{
			if( pChild->kind_of( i3UIControl::static_meta()))
			{
				i3UIControl * pCtrl = (i3UIControl *) pChild;

				if (pCtrl->isActivated() && pCtrl->isVisible())
				{
					i3UIControl * pFind = pCtrl->TA_ScanText(pszText, bExact, true);
					if (pFind != nullptr)
						return pFind;
				}
			}
			else if( pChild->kind_of( i3UIScene::static_meta()))
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
