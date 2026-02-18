#include "pch.h"
#include "GameInterface.h"
#include "GlobalVariables.h"
#include "Game.h"
#include "GameSupportSet.h"
#include "UnitTest/GameUnitTest.h"
#include "InGameQACommandSet.h"
#include "GameFramework.h"
#include "AllStage.h"
#include "GameChatBox.h"

#include "MessageBoxManager.h"
#include "GameWorld.h"
#include <mmsystem.h>
#include "GameChara.h"
#include "GameCharaControl.h"
#include "Weapon.h"
#include "GlobalVariables.h"

#include "GameObject.h"

#include "ServerIPList.h"
#include "ServerInfo.h"
#include "AnnounceConfig.h"
#include "EquipShapeManager.h"
#include "InvenList.h"
#include "CommunityContext.h"

#include "AIDriver.h"
#include "AIDriver_Weapon.h"

#include "Crosshair.h"
#include "Crosshair_Cross.h"
#include "Crosshair_CrossDot.h"
#include "Crosshair_Dot.h"
#include "Crosshair_Ring.h"
#include "Crosshair_Dino.h"
#include "Crosshair_Dino_Raptor.h"
#include "Crosshair_Dino_Elite.h"
#include "Crosshair_Dino_Sting.h"
#include "Crosshair_Dino_TRex.h"

#include "TracerBullet.h"
#include "TracerBulletManager.h"

#include "EffectManager.h"

#include "PB_i3GameSceneInfo.h"

#include "resource.h"
#include "SecurityHeader.h"

#include "TriggerCallBack.h"
#include "Camera/GameCamera_OnObject.h"
#include "../../Avatar/AvatarGen.h"

#include "UI.h"
#include "ui/UIUtil.h"
#include "MedalManager.h"
#include "GameLoginContext.h"


#include "CommunityContext.h"


#if defined( I3_WINDOWS) && defined( I3_DEBUG)
#include "ObjSpec_Warp.h"
#include "ObjSpec_TargetTrigger.h"
#include "ObjSpec_TargetObject.h"
#include "ObjSpec_EnterHelicopter.h"
#include "ObjSpec_ScreenObject.h"
#include "ObjSpec_WeaponBox.h"
#include "ObjSpec_RespawnHelicopter.h"
#include "ObjSpec_Event.h"
#include "ObjSpec_JumpHold.h"
#include "ObjSpec_BattleUseItem.h"
#endif

#include "CoreFileChecker.h"

#include "LoadingStateDlg.h"

#include "i3Base/string/ext/contain_string.h"


//#include "i3Base/v_pool/v_storage_pool_mt.h"
//#include "i3Base/v_pool/v_compact_pool.h"
//#include "i3Base/v_pool/v_common_pool.h"

#include "GunInfo.h"
#include "SMGGunInfo.h"
#include "MeleeWeaponInfo.h"
#include "DinoInfo.h"
#include "GrenadeGunInfo.h"
#include "GrenadeInfo.h"
#include "WeaponDinoInfo.h"
#include "OnlyGrenadeInfo.h"


#define		PAGE_SIZE			(4096<<2)
#define		GAMETRACE_ERRG_TO_DBGWINDOW


CTriggerCallBack* g_pTriggerCallBack = nullptr;

extern HINSTANCE			g_hInstance;

static void SetPool( i3ClassMeta * pMeta)
{
//	i3FixedMemoryPool * pPool;

	// PSP에서 VFPU가 접근할 수 있는 Address는 반드시 16 bytes align 되어야 하기 때문에..

	// Windows는 address가 4byte 단위
	if( pMeta->class_size() < PAGE_SIZE)	// 최소 10개 이상은 페이징이 가능하면 Pool을 사용
	{
		i3::v_pool_ptr pool_mt = i3::create_v_pool_ptr<i3::v_compact_pool>(pMeta->class_size());
		pMeta->set_pool(pool_mt);
		
	}
	else
	{
		I3TRACE( "no use pool by huge size. : %s\n", pMeta->class_name() );
	}
}

static void SetDerivedClassPool( i3ClassMeta * pMeta)
{
	i3::vector<i3ClassMeta*> List;
	
	pMeta->get_all_derived_metas( List, true);

	for(size_t i = 0; i < List.size(); i++)
	{
		pMeta = List[i];

		//if( pMeta->IsAbstractClass() == false)
		if (pMeta->get_class_meta_style() != i3::cms_abstract)
		{
			SetPool( pMeta);
		}
	}
}

//bool RegisterElementPools(void)
//{
//	SetDerivedClassPool( i3PersistantElement::static_meta());
//
//	return true;
//}

void GameInterface::OnRegisterMeta(void)
{
	{
		//// Error Log initialize
		//i3Error::Init();
		//i3Error::SetExceptionHandlerOption( I3MINDUMP_WITHDATASEGS );
		//i3Error::SetChannelName( I3LOG_NOTICE, (const PSTR)getLogName() );
		//i3Error::SetChannelName( I3LOG_FATAL, (const PSTR)getLogName());
		//i3Error::SetChannelName( I3LOG_WARN, (const PSTR)getLogName());
		//i3Error::SetChannelName( I3LOG_VIEWER, (const PSTR)getLogName());
		//i3Error::SetChannelName( I3LOG_DEFALUT, (const PSTR)getLogName());

  //      i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );
		//i3Error::SetChannel( I3LOG_VIEWER, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );
		//i3Error::SetChannel( I3LOG_DEFALUT, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );

		//#if defined( I3_DEBUG) || defined( I3_RELEASE_PROFILE) //???????????????????
		//	i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE | I3ERR_CHN_CALLBACK);
		//	i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_DEBUG | I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
		//#else
		//	i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE);
		//	i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
		//#endif

		SYSTEMTIME lt;
		GetLocalTime(&lt);

		I3PRINTLOG(I3LOG_DEFALUT,  "=============================================");
		//I3PRINTLOG(I3LOG_DEFALUT,  "Version Log( GAME_VER_MONTH %d GAME_VER_DAY %d )", GAME_VER_MONTH, GAME_VER_DAY );
		I3PRINTLOG(I3LOG_DEFALUT,  "Version Log( GAME VER :  %d.%d.%d.%d )", VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_COUNT );
		I3PRINTLOG(I3LOG_DEFALUT,  "Build Log(%s,%s)", GameUI::pre_macro_date_to_string().c_str(), GameUI::pre_macro_time_to_string().c_str() );
		I3PRINTLOG(I3LOG_DEFALUT,  "Play Log(%d/%d/%d, %d:%d:%d)", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond );
		I3PRINTLOG(I3LOG_DEFALUT,  "=============================================");
	}

	i3BaseRegisterMetas();
	i3GfxRegisterMetas();
	i3SceneRegisterMetas();
	i3FrameworkRegisterMetas();
	i3GuiRegisterMetas();
	i3NetworkRegisterMetas();

	{
		CEffectManager::static_meta()->set_class_meta_style_concrete( i3EffectManager::static_meta() );
	}

	{
		PB_i3GameSceneInfo::static_meta()->set_class_meta_style_concrete(i3GameSceneInfo::static_meta());
	}


	// core
	SetPool(i3MemoryBuffer::static_meta());
	SetPool(i3PackNode::static_meta());
	SetPool(i3Node::static_meta());
	SetPool(i3BoundBox::static_meta());
	SetPool(i3MatrixArray::static_meta());
	SetPool(i3MatrixObject::static_meta());
	SetPool(i3Vector3Array::static_meta());
	SetPool(i3PackVolume::static_meta());
	SetPool(i3QueueFileInfo::static_meta());


	// MISC
	SetPool(i3SoundFMOD::static_meta());
	SetPool(i3SoundNode::static_meta());
	SetPool(i3SoundPlayInfo::static_meta());

	// gfx
	SetPool(i3Light::static_meta());
	SetPool(i3TextureDX::static_meta());
	SetPool(i3VertexArrayDX::static_meta());
	SetPool(i3VertexArrayMem::static_meta());
	SetPool(i3IndexArray::static_meta());
	SetPool(i3IndexArrayMem::static_meta());
	SetPool(i3AlphaTestEnableAttr::static_meta());
	SetPool(i3NormalMapEnableAttr::static_meta());
	SetPool(i3ReflectMapEnableAttr::static_meta());
	SetPool(i3ReflectMaskMapEnableAttr::static_meta());
	SetPool(i3SpecularMapEnableAttr::static_meta());
	SetPool(i3TextureEnableAttr::static_meta());
	SetPool(i3AlphaFuncAttr::static_meta());
	SetPool(i3FaceCullModeAttr::static_meta());
	SetPool(i3GeometryAttr::static_meta());
	SetPool(i3TextureBindAttr::static_meta());
	SetPool(i3NormalMapBindAttr::static_meta());
	SetPool(i3ReflectMaskMapBindAttr::static_meta());
	SetPool(i3SpecularMapBindAttr::static_meta());
	SetPool(i3MaterialAttr::static_meta());
	SetPool(i3ModelViewMatrixAttr::static_meta());
	SetPool(i3TextAttrDX2::static_meta());
	SetPool(i3BoneMatrixListAttr::static_meta());
	SetPool(i3BlendEnableAttr::static_meta());
	SetPool(i3BlendModeAttr::static_meta());

	SetPool(i3Surface::static_meta());
	SetPool(i3Sprite2DAttr::static_meta());
	SetPool(i3AttrSet::static_meta());
	SetPool(i3Sprite2D::static_meta());


	// 엔진쪽 캐릭터/본/애니메이션/메시/PhysX 관련..
	SetPool(i3PhysixShapeSet::static_meta());
	SetPool(i3Chara::static_meta());
	SetPool(i3AIContext::static_meta());
	SetPool(i3Animation2::static_meta());
	SetPool(i3SceneGraphInfo::static_meta());
	SetPool(i3AI::static_meta());
	SetPool(i3AIState::static_meta());
	SetPool(i3TransformSequence::static_meta());
	SetPool(i3BoneRef::static_meta());
	SetPool(i3SceneObject::static_meta());
	SetPool(i3Transform2::static_meta());
	SetPool(i3Geometry::static_meta());
	SetPool(i3TimeSequence::static_meta());
	SetPool(i3ColliderSet::static_meta());
	SetPool(i3Body::static_meta());
	SetPool(i3TransformSourceCombiner::static_meta());
	SetPool(i3CollideeMesh::static_meta());
	SetPool(i3Collider::static_meta());
	SetPool(i3LOD::static_meta());
	SetPool(i3Skeleton::static_meta());
	SetPool(i3AnimationContext::static_meta());
	SetPool(i3Object::static_meta());
	SetPool(i3Animation::static_meta());
	SetPool(i3ActionEmitEffect::static_meta());
	SetPool(i3Trigger::static_meta());
	SetPool(i3TimeEventGen::static_meta());
	SetPool(i3TimeEventInfo::static_meta());


	// UI콘트롤 관련 (UIBase는 대부분 풀대상으로 적합치 않음)
	SetPool(i3UIButton::static_meta());
	SetPool(i3UIButtonComposed3::static_meta());
	SetPool(i3UIButtonImageSet::static_meta());
	SetPool(i3UIEditBox::static_meta());
	SetPool(i3UIFrameWnd::static_meta());
	SetPool(i3UIImageBox::static_meta());
	SetPool(i3UIProgressBar::static_meta());
	SetPool(i3UIRadio::static_meta());
	SetPool(i3UIStaticText::static_meta());
	SetPool(i3UIToolTip::static_meta());
	SetPool(i3UIListView_Cell::static_meta());
	SetPool(i3UIScrollBar::static_meta());
	SetPool(i3UIListView_Item::static_meta());
	SetPool(i3UIListView_Sep::static_meta());
	SetPool(i3UIScene::static_meta());
	SetPool(i3UICheckBox::static_meta());
	SetPool(i3UIImageBoxEx::static_meta());
	SetPool(i3UIListBox::static_meta());

	SetPool(i3UITemplate_Button::static_meta());
	SetPool(i3UITemplate_HScrollBar::static_meta());
	SetPool(i3UITemplate_VScrollBar::static_meta());
	SetPool(i3UITemplate_EditBox::static_meta());
	SetPool(i3UITemplate_ImageBox::static_meta());
	SetPool(i3UITemplate_LVCell::static_meta());
	SetPool(i3UITemplate_LVSep::static_meta());
	SetPool(i3UITemplate_ListBox::static_meta());

	SetPool(i3TextNodeDX2::static_meta());
	SetPool(i3UIText::static_meta());


	// 게임에 쓰는 웨폰들 --> 풀대상..
	SetPool(WeaponBase::static_meta());
	SetPool(WeaponDualGrenade::static_meta());
	SetPool(WeaponK400::static_meta());
	SetPool(WeaponSmoke::static_meta());
	SetPool(WeaponFlash::static_meta());
	SetPool(WeaponWPSmoke::static_meta());
	SetPool(WeaponMedicalKit::static_meta());
	SetPool(WeaponEventBomb::static_meta());
	SetPool(WeaponClaymore::static_meta());
	SetPool(WeaponKnife::static_meta());
	SetPool(WeaponThrowKnife::static_meta());
	SetPool(WeaponGrenadeShell::static_meta());
	SetPool(WeaponC5::static_meta());
	SetPool(WeaponRocketLauncherShell::static_meta());
	SetPool(WeaponSmokeShell::static_meta());
	SetPool(WeaponWPSmokeShell::static_meta());
	SetPool(WeaponFlashbangShell::static_meta());
	SetPool(WeaponDecoyBomb::static_meta());

	SetPool(CTracerBullet::static_meta());
	SetPool(CGunInfo::static_meta());
	SetPool(CSMGGunInfo::static_meta());
	SetPool(CMeleeWeaponInfo::static_meta());
	SetPool(CWeaponScopeInfo::static_meta());
	SetPool(CGrenadeGunInfo::static_meta());
	SetPool(CGrenadeInfo::static_meta());
	SetPool(CWeaponDinoInfo::static_meta());
	SetPool(COnlyGrenadeInfo::static_meta());

	// 아바타
	SetPool(CFacegearInfo::static_meta());
	SetPool(CHeadInfo::static_meta());
	SetPool(CJacketInfo::static_meta());
	SetPool(CPoketInfo::static_meta());
	SetPool(CGloveInfo::static_meta());

	// 게임캐릭터
	SetPool(CDinoInfo::static_meta());
	SetPool(CGameCharaEquip::static_meta());
	SetPool(CCharaInfo2::static_meta());
	SetPool(CEquipTransform::static_meta());

	// 게임 UI 자료구조
	SetPool(ShopItemInfo::static_meta());
	SetPool(ShopPackageGoodsInfo::static_meta());


	// SceneGraph
	/*
	SetDerivedClassPool( i3Node::static_meta());

	SetDerivedClassPool( i3RenderAttr::static_meta());

	// Game Graph
	SetDerivedClassPool( i3GameNode::static_meta());
	SetDerivedClassPool( i3Action::static_meta());

	SetPool( i3AI::static_meta());
	SetPool( i3AIState::static_meta());
	SetPool( i3AIDriver::static_meta());

	// Persistant element
	SetDerivedClassPool( i3VertexArray::static_meta());
	SetDerivedClassPool( i3IndexArray::static_meta());
	SetDerivedClassPool( i3Texture::static_meta());

	SetPool( i3Surface::static_meta());
	SetPool( i3MatrixObject::static_meta());
	SetPool( i3MatrixObjectProxy::static_meta());
	SetPool( i3SoundPlayInfo::static_meta());
	SetPool( i3Sound::static_meta());
	SetPool( i3BoundBox::static_meta());

	SetPool( i3TimeSequenceInfo::static_meta());
	SetPool( i3TransformSequence::static_meta());
	SetPool( i3TransformSourceCombiner::static_meta());
	SetPool( i3TransformModifier::static_meta());
	SetPool( i3KeyframeTable::static_meta());
	SetPool( i3Animation::static_meta());
*/

	//RegisterElementPools();
}

void GameInterface::ShaderVersionControl(void)
{
	// 아래의 Shader Cache를 읽어 들이는 작업은 실패를 해도
	// 문제 삼지 않는 것을 전제로 합니다.
	// 해서 Error Report를 띄우지 않도록 Error Channel을 조정합니다.
	i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );
	i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE);
	i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_FILE);

	i3ShaderCache *	pDef  =		i3ShaderCache::Load( "Shader/DefaultSHGL");
	i3ShaderCache *	pCur	=	i3ShaderCache::Load( "Shader/Cache.i3GL");

	i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );
	#if defined( I3_DEBUG) || defined( I3_RELEASE_PROFILE)//??????????????????
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE | I3ERR_CHN_CALLBACK);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_DEBUG | I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
	#else
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
	#endif
	

	if( pDef == nullptr)
	{
		I3PRINTLOG(I3LOG_NOTICE,"------ Cannot find DafaultSHGL ---------");
		return;
	}

	if( (pCur == nullptr) ||
		(pDef->getISSVersion() > pCur->getISSVersion()))
	{
		I3PRINTLOG(I3LOG_NOTICE, " - Shader Cache 파일이 덮어 씁니다.");

		if( ::CopyFile( "Shader/DefaultSHGL", "Shader/Cache.i3GL", false) == 0)
		{
			// 파일 쓰기 실패?
			DWORD err = ::GetLastError();

			switch( err)
			{
			case ERROR_FILE_NOT_FOUND :
				I3PRINTLOG(I3LOG_FATAL,  "DefaultSHGL does not exist!!!");
				break;
			case ERROR_ACCESS_DENIED  :
				::SetFileAttributes( "Shader/Cache.i3GL", FILE_ATTRIBUTE_NORMAL);
				if( ::CopyFile( "Shader/DefaultSHGL", "Shader/Cache.i3GL", false) == 0)
				{
					I3PRINTLOG(I3LOG_FATAL,  "Access denied!!! [Shader/Cache.i3GL]");
				}
				break;
			default :
				I3PRINTLOG(I3LOG_FATAL,  "File Copy Fail %d!!! [Shader/Cache.i3GL]", err);
				break;
			}

		}

		return;
	}
}

static bool _PackFileFindProc( INT32 Level, char * pszPath, void * pFileInfo, void * pUserData)
{
	WIN32_FIND_DATA * pFile = (WIN32_FIND_DATA *) pFileInfo;

	i3::stack_string FileName;
	i3::sprintf(FileName, "%s\\%s", pszPath, pFile->cFileName);

	//i3Pack::Preview() - 파일 header info 만 미리 읽습니다.
	//이후 i3System::LookForFiles 이 끝난 후 i3Pack::Bind 를 합니다.
	//i3Pack::Preview( FileName);

	i3Pack::Bind( FileName.c_str());

	return true;
}

bool GameInterface::OnInitInstance( void * pInstHandle, void * pScrHandle)
{
	g_hInstance = (HINSTANCE) pInstHandle;
	g_hWnd		= (HWND) pScrHandle;

	// 엔진의 Missing External Ref. File의 Search 기능 Off
	i3ResourceFile::setExternRefResolveCallback( nullptr, nullptr);

	i3Language::SetCurrentCodePage( GetCodePage() );

	i3TextFontSetInfo	FontSetInfo;
	FontSetInfo.SetFontName(GetDefaultLocaleFontName());
	FontSetInfo.SetCharset(i3Language::GetCharasetFromCodePage(GetCodePage()));

	SetDefaultFont(FontSetInfo);

	SetStringFilePath(GetLanguageType());

	//
	// * 시작 디렉토리 체크 *
	// 디버깅시에 시작디렉토리를 Media로 맞추지 않으면 파일이 없는 에러가 나와서
	// 혼동이 오기때문에 검사를 해봅니다.
	//
#if defined(I3_DEBUG)
	if ( false == i3System::IsDirectory(_T("./config")) ||
		false == i3System::IsDirectory(_T("./Script")) )
	{
		::SetLastError(0);
		I3PRINTLOG(I3LOG_FATAL, "프로그램 시작 디렉토리를 확인하세요.\nMedia로 맞추셔야되요.");
		return false;
	}
#endif

#if defined( CHECK_CORE_FILE_MD5 )
	CCoreFileChecker	Checker;
	int Rv = Checker.CoreFilesCheck();
	if ( 0 != Rv )
	{
		I3PRINTLOG(I3LOG_NOTICE,"=1 : %d", Rv);
		return false;
	}
#endif	

#ifdef NC_BUILD
	__ProhibitFile_CheckOut();
#endif

	// i3Pack 파일 등록
	//
	{
		AfxSetResourceHandle(g_hInstance);

		i3Pack::Init();
		i3System::LookForFiles("Gui\\Loading", "*.i3Pack", _PackFileFindProc, NULL, FFT_SUBDIR);

		LoadingState::CreatePreLoadingScreen();
				
//		i3Pack::Init();
		i3System::LookForFiles( "Pack", "*.i3Pack", _PackFileFindProc, nullptr, FFT_SUBDIR);
		::SetFocus(g_hWnd);
		
		//i3Pack::Sort();					//i3Pack 파일을 패킹 했던 시간 순서로 정렬 합니다.
		//if( i3Pack::Bind() == false)	//찾은 i3Pack파일을 모두 Bind 합니다.
		//{
		//	return false;
		//}

		//i3Pack::DumpList();

		I3PRINTLOG(I3LOG_DEFALUT,  "Open Pack Files <<<");

		#if !defined( BUILD_RELEASE_QA_VERSION)
		if( i3ResourceFile::CheckProtectionCode( "Pack/Script.i3Pack", "Zepetto Inc.", 12) == false)
		{
			::MessageBoxW( g_hWnd, GAME_STRING("STR_TBL_GLOBALDEF_GAMEINTERFACE_DATA_CRACKED1"),	// 데이터 파일이 손상되어 게임을 구동할 수 없습니다.\r\n종료합니다.
						GAME_STRING("STR_TBL_GLOBALDEF_GAMEINTERFACE_DATA_CRACKED2"),			// 오류
						MB_OK);
			return false;
		}
		#endif
	}

	I3PRINTLOG(I3LOG_NOTICE, "= 3");

	if( !CreateGlobal())
	{
		return false;
	}

	#if !defined( I3_DEBUG)
		ShaderVersionControl();
	#endif

		Sleep(500);

	LoadingState::DestroyPreLoadingScreen();


	
	//{
	//	// Splash Window를 생성합니다.
	//	CreateSplashWindow();
	//}

	I3PRINTLOG(I3LOG_NOTICE, "= 4");



	return true;
}

void GameInterface::RegisterScript( lua_State * L)
{
	#if defined( USE_LUA_UI)
	GameContextScript::Init(L);
	#endif
}

void GameInterface::OnExitInstance( void)
{
#if defined(_FROST_)

	frostFinalize();

#endif

	i3Pack::Finish();

	CloseGame();

	I3_SAFE_DELETE(g_pTriggerCallBack);
}

void GameInterface::GetVideoConfig( i3VideoInfo * pVideo)
{
	// UI 리뉴얼 해상도 고정 기능 : 하동익 추후 Config에 반영 여부 정해야할듯. (2011-10-05)
#if defined(UI2_RESOLUTION_FIX)
	pVideo->m_bFullScreen			= false;
	pVideo->m_Width					= 1024;
	pVideo->m_Height				= 768;
#else
	pVideo->m_bFullScreen			= g_pEnvSet->m_bFullScreen;
	pVideo->m_Width					= g_pEnvSet->m_nScreenWidth;
	pVideo->m_Height				= g_pEnvSet->m_nScreenHeight;
#endif

	if( g_pEnvSet->m_b16BitsDepth)
		pVideo->m_ZBufferBits		= 16;
	else
		pVideo->m_ZBufferBits		= 24;

	pVideo->m_StencilBufferBits		= 0;
	pVideo->m_bVSync				= g_pEnvSet->m_bVSync;

	if(pVideo->m_bFullScreen)
	{
		pVideo->m_RefreshRate		= (REAL32) g_pEnvSet->m_nScreenHz;
		
		if( g_pEnvSet->m_ScreenDepth == 16)
			pVideo->m_RenderTargetFormat = I3G_IMAGE_FORMAT_BGR_565;
		else
			pVideo->m_RenderTargetFormat = I3G_IMAGE_FORMAT_BGRX_8888;
	}
	else
	{
		pVideo->m_RefreshRate		= 0;
		pVideo->m_RenderTargetFormat = I3G_IMAGE_FORMAT_BGRX_8888;
	}

	pVideo->m_MultiSampling			= g_pEnvSet->m_AA;
	pVideo->m_MultiSamplingQuality	= g_pEnvSet->m_AAQ;
}

bool GameInterface::CreatePreFramework(HWND hwnd)
{
#if defined (_NPROTECT_)
	return nProtect::Init( hwnd); 

#elif defined(_FROST_)

	frostInitialize();

	return true;
		
#elif defined(_XTRAP_)
	return true;

#else
	return true;
#endif
}

void GameInterface::InitPhysXValues( void)
{
	i3PhysixContext * pCtx = g_pFramework->getPhysixContext();
	CPhysixInfo * pInfo = g_pPhysixInfoDataBase->getInfo();

	I3ASSERT(pCtx->getScene() != nullptr);

	pCtx->setGravity( pInfo->m_rGravity );
	pCtx->setSimTime( pInfo->m_rSimTimeSlop );
	pCtx->setBoundThresHold( pInfo->m_rBounceThresHold );
	pCtx->setFxToForce( pInfo->m_rFXtoForce );
	pCtx->setSleepLinVelocity( pInfo->m_rSleepLinVelocity );
	pCtx->setSleepAngVelocity( pInfo->m_rSleepAngVelocity );
	pCtx->setMaxAngVelocity( pInfo->m_rMaxAngVelocity );

	pCtx->setStaticFriction( pInfo->m_rStaticFriction );
	pCtx->setDynamicFriction( pInfo->m_rDynamicFriction );

	if(g_pTriggerCallBack == nullptr)
	{
		g_pTriggerCallBack = new CTriggerCallBack;
		MEMDUMP_NEW( g_pTriggerCallBack, sizeof(CTriggerCallBack));
	}
	pCtx->getScene()->setUserTriggerReport(g_pTriggerCallBack);
}

i3Framework * GameInterface::CreateFramework( i3Viewer * pViewer)
{
	i3Framework * pFramework = nullptr;

	g_pViewer = pViewer;
	g_hWnd = pViewer->GetWindowHandle();

	{
		// Splash Window를 생성합니다.
		//CreateSplashWindow();
	}

	if( !CreateGlobal2())
	{
		return false;
	}

	


	SetClassLong( g_hWnd, GCL_HCURSOR, (LONG)LoadCursorFromFile( "Gui/3dgarro.cur"));

	if( g_pEnvSet->m_bFullScreen)
	{
		if( g_pSplash != nullptr)	g_pSplash->Hide();
	}

	// revision 35764 지원못하는 그래픽 카드 오류메세지 띄움
	// VertexIndex가 32비트를 지원하지 않으면 종료합니다.
	if(pViewer->GetRenderContext()->GetCaps()->GetMaxVertexIndex() <= 0xFFFF)
	{
		if( g_pSplash != nullptr) g_pSplash->Hide();
		::MessageBoxW( g_hWnd, GAME_STRING("STR_TBL_GLOBALDEF_GAMEINTERFACE_NO_SUPPORT_SHADER"),	//32비트 인덱스 버퍼를 지원하지 않음
			GAME_STRING("STR_TBL_GLOBALDEF_GAME_ERROR_TEXT2"),							//알림
			MB_OK);
		
		CloseGame();
		return nullptr;
	}

	// 쉐이더가 지원하지 않으면 게임을 종료합니다.
	if( ! pViewer->GetRenderContext()->GetCaps()->getEnableShader())
	{
		if( g_pSplash != nullptr) g_pSplash->Hide();
		::MessageBoxW( g_hWnd, GAME_STRING("STR_TBL_GLOBALDEF_GAMEINTERFACE_NO_SUPPORT_SHADER"),	//하드웨어에서 쉐이더를 지원하지 않으므로 게임을 종료합니다.
					GAME_STRING("STR_TBL_GLOBALDEF_GAME_ERROR_TEXT2"),							//알림
					MB_OK);
		CloseGame();
		return nullptr;
	}

	pViewer->SetPendingUpdateEnable( true);

	//Framework
	I3PRINTLOG(I3LOG_NOTICE, "Game configuration set >>>");
	{
		i3ResourceFile file;

		//file.Load( "config/BC_UIV3.i3Game");
		UINT32 res = file.Load( "config/BCV3.i3Game");
		I3ASSERT(res != STREAM_ERR);

		g_pFramework = (CGameFramework *) file.getKeyObject();
		pFramework = g_pFramework;
	
		I3ASSERT( i3::same_of<CGameFramework*>(g_pFramework));

		((CGameFramework*)pFramework)->SetTextureQuality( g_pEnvSet->m_nTextureQualityType);
		((CGameFramework*)pFramework)->SetEnableNormalMap( g_pEnvSet->m_bEnableNormalMap?true:false);
		((CGameFramework*)pFramework)->SetEnableSpecularMap( (g_pEnvSet->m_nSpecularQualityType>0)?true:false);

		I3_MUST_ADDREF(g_pFramework);
	}
	I3PRINTLOG(I3LOG_NOTICE, "Game configuration set <<<");

	I3PRINTLOG(I3LOG_NOTICE, "Framework creation >>>");
	if( ! g_pFramework->Create( pViewer ))
	{
		I3PRINTLOG(I3LOG_FATAL, "Could Not Create", "g_pFramework");
		return nullptr;
	}

	if (g_pEnvSet->m_nUIVersion == UFV_1_5)
	{
		PBCONSOLE_CREATE_FOR_DEBUG_RELEASEQA
	}

	I3PRINTLOG(I3LOG_NOTICE, "Framework creation <<<");

	// disable main window ime
	// if you comment this block, you'll see candidate/reading window that getting characters from main window when you use chinese/japanese ime
/*
	HIMC hImc	= ImmGetContext( g_hWnd);
	if( hImc != nullptr)
	{
		g_hIMC = ImmAssociateContext( g_hWnd, nullptr);

		ImmReleaseContext( g_hWnd, hImc);
	}
*/

	if (g_pEnvSet->m_nUIVersion == UFV_1_1
		|| g_pEnvSet->IsBaseOnUIVersion_1_0())
	{
		LoadDesignationValue();
		LoadMissionCard();
	}

	return pFramework;
}

bool GameInterface::OnInitFramework( i3Framework * pFramework, const char * pszCmdLine)
{
#if defined( LOCALE_INDONESIA)
	I3PRINTLOG(I3LOG_DEFALUT, "Read Registry Key >>>>>>>>>>>>>>>>");

	HKEY h_wpKey;

	DWORD dwType = REG_SZ;
	DWORD ptsPathSize = MAX_INDONESIA_PCBANG_KEY;
	if( RegOpenKeyEx( HKEY_CURRENT_USER, "SOFTWARE\\Gemscool\\Premium", 0, KEY_QUERY_VALUE, &h_wpKey) == ERROR_SUCCESS)
	{
		I3PRINTLOG(I3LOG_DEFALUT, "[Success Open Registry]");
		if( RegQueryValueEx( h_wpKey, "key", 0, &dwType, (LPBYTE)Warnet_Premium::g_szRegKey, &ptsPathSize) == ERROR_SUCCESS)
		{
			//I3PRINTLOG(I3LOG_DEFALUT, "[Success Query Registry Key]%s", Warnet_Premium::g_szRegKey);
			//I3PRINTLOG(I3LOG_DEFALUT, "[Registry Size]%d", ptsPathSize);
		}
		else
		{
			I3PRINTLOG(I3LOG_DEFALUT, "[Fail Query Registry Key]");
			//I3PRINTLOG(I3LOG_DEFALUT, "[Registry Size]%d	Registry Buffer Size : %d", ptsPathSize, MAX_INDONESIA_PCBANG_KEY);
			::strcpy_s( Warnet_Premium::g_szRegKey, MAX_INDONESIA_PCBANG_KEY, "");
		}

		RegCloseKey( h_wpKey);	
	}
	else
	{
		I3PRINTLOG(I3LOG_DEFALUT, "[Fail Open Registry]");
	}

	I3PRINTLOG(I3LOG_DEFALUT, "Read Registry Key >>>>>>>>>>>>>>>>");
#endif
	I3PRINTLOG(I3LOG_NOTICE, "* 1");

	g_pFramework->SetCommandString(pszCmdLine);

	// Gloval Physix Set
	InitPhysXValues();

	I3PRINTLOG(I3LOG_NOTICE, "* 2");

	// 런쳐에서 아이피 받으려고 임시함수 작성
	
	// Network Context
	{
		g_pServerInfo = CServerInfoContext::new_object();
		I3PRINTLOG(I3LOG_NOTICE, "Set Network Info.");
		if (!g_pServerInfo->Initialize())
		{
			I3PRINTLOG(I3LOG_FATAL, "Could not initialize server information. Check config/lwsi_En.sif");
			return false;
		}

		g_pGameContext	= CGameContext::new_object();	// 399968]
		I3ASSERT( g_pGameContext != nullptr );
		g_pGameContext->OnCreate();

#if defined(NC_BUILD)		

#if (defined(LOCALE_ENGLISH) || defined(LOCALE_TAIWAN))

#else
		if( ! GameLoginContext::i()->ParseCommandLineForLogin( pszCmdLine) )
		{
	#if defined(LOCALE_KOREA)
			{
				extern void SetForegroundWindowForce(HWND hWnd);
				const char* errMsg = "죄송합니다. 포인트 블랭크는 홈페이지 스타트 버튼을 통해서만 실행 가능합니다.";				

				SetForegroundWindowForce(g_hWnd);			
				MessageBox(g_hWnd, errMsg, "ERROR!", MB_ICONERROR);
			}
	#else
			I3PRINTLOG(I3LOG_FATAL, "Fail to parse command line.");
	#endif
			return false;
		}
#endif
#endif

		//g_pCommunity = CCommunityContext::new_object();
		//I3ASSERT( g_pCommunity != nullptr );
	}
	I3PRINTLOG(I3LOG_NOTICE, "* 3");

	// Normal Map
	g_pViewer->GetRenderContext()->getGfxOption()->setKeepPersistDataEnable( false);

	I3PRINTLOG(I3LOG_NOTICE, "* 4");
	{
		bool bFlag = false;
		if( g_pEnvSet->m_bEnableNormalMap) bFlag = true;
		g_pViewer->GetRenderContext()->getGfxOption()->setNormalMapEnable( bFlag);
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 5");

	// Specular Map 
	{
		switch( g_pEnvSet->m_nSpecularQualityType)
		{
			case 0 :
				g_pViewer->GetRenderContext()->getGfxOption()->setSpecularMapEnable( false);
				g_pFramework->SetFresnelEnable( false);
				break;

			case 1 :
				g_pViewer->GetRenderContext()->getGfxOption()->setSpecularMapEnable( true);
				g_pFramework->SetFresnelEnable( false);
				break;

			case 2 :
				g_pViewer->GetRenderContext()->getGfxOption()->setSpecularMapEnable( true);
				g_pFramework->SetFresnelEnable( true);
				break;
		}
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 6");

	// Texture Quality
	{
		g_pViewer->GetRenderContext()->getGfxOption()->setTextureQuality( g_pEnvSet->m_nTextureQualityType);
		//AvatarGen::setTextureQuality( g_pEnvSet->m_nTextureQualityType);
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 7");

	// Trilinear Filter
	{
		if( g_pEnvSet->m_bTriLinearFilter)
		{
			i3GfxCaps * pCaps = i3RenderContext::GetCaps();

			if( pCaps->getGPUClass() == I3G_GPU_CLASS_A)
				g_pViewer->GetRenderContext()->getGfxOption()->setForcedMipmapFilter( I3G_TEXTURE_FILTER_ANISOTROPIC);
			else
				g_pViewer->GetRenderContext()->getGfxOption()->setForcedMipmapFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR);
		}
		else
			g_pViewer->GetRenderContext()->getGfxOption()->setForcedMipmapFilter( I3G_TEXTURE_FILTER_NONE);
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 8");

	// Sound
	i3SoundContext * pCtx = g_pViewer->GetSoundContext();
	if( pCtx != nullptr)
	{
		/*
		pCtx->SetFXEnable( false);
		pCtx->SetFXType( I3SND_CMN_FX_REVERB);
		pCtx->SetReverbParam( DSFX_I3DL2_ENVIRONMENT_PRESET_ROOM);
		*/
	}

	ShowWindow( g_hWnd, SW_SHOW);

	I3PRINTLOG(I3LOG_NOTICE, "* 9");

	// UI 1.0/1.1에서는 메달이 없기 때문에 로드하지 않습니다
	if (g_pEnvSet->m_nUIVersion == UFV_1_5)
	{
		if ( !CMedalManager::i()->LoadMedalData() )
			return false;
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 10");

	return true;
}



UINT32	GameInterface::GetViewportStyle(void)
{
	return WS_POPUP | WS_BORDER | WS_MINIMIZEBOX | WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU;
}

UINT32	GameInterface::GetIcon(void)
{
	return IDI_ICON_POINTBLANK;
}

bool GameInterface::OnDrive( i3Viewer * pViewer)
{
	return DriveGame() == true;
}

//	시험삼아 atexit처리를 해보고, 문제가 되면 다시 주석으로 막겠습니다..  (11.06.03.수빈)
namespace
{
	void DeleteGameInterFace()
	{
		I3_SAFE_DELETE( g_pInterface);
	}
}
//

#ifdef USE_I3EXEC
__declspec( dllexport)
i3GameInterface *	__stdcall I3_CREATE_GAME_INTERFACE_PROC_NAME( const char * pszRunPath)
{
	GameInterface * pInterface = new GameInterface;
	MEMDUMP_NEW( pInterface, sizeof( GameInterface));
	g_pInterface = pInterface;
	::atexit(&DeleteGameInterFace);
	return pInterface;
}

__declspec( dllexport)
UINT32	__stdcall I3_GETVERSION_PROC_NAME(void)
{
	return I3_VERSION;
}
#else
GameInterface * i3CreateGameInterface(const char * pszRunPath)
{
	GameInterface * pInterface = new GameInterface;
	MEMDUMP_NEW( pInterface, sizeof(GameInterface));

	g_pInterface = pInterface;
	::atexit(&DeleteGameInterFace);
	return pInterface;
}

UINT32 i3Geti3EngineVersion()
{
	return I3_VERSION;
}
#endif

#if defined( I3_WINDOWS) && defined( USE_EDITDLG)//프로퍼티 쿼리 디버그 
INT32 GameInterface::OnQueryPropertyDlgCount(void)
{
	return 10;
}

void GameInterface::OnQueryPropertyDlg( HINSTANCE hInst, INT32 idx, i3ClassMeta ** ppMeta, i3GameObjPropertyDialog ** ppDlg)
{
	HINSTANCE hOld = AfxGetResourceHandle();

	AfxSetResourceHandle( hInst);

	switch( idx)
	{
		case 0 :		// CGameObjectJumper
			*ppMeta = CGameObjectJumper::static_meta();
			*ppDlg = new CObjSpec_Warp;
			MEMDUMP_NEW( *ppDlg, sizeof( CObjSpec_Warp));
			(*ppDlg)->Create( CObjSpec_Warp::IDD, nullptr);
			break;

		case 1 :		// TargetTrigger
			*ppMeta = TargetTrigger::static_meta();
			*ppDlg = new CObjSpec_TargetTrigger;
			MEMDUMP_NEW( *ppDlg, sizeof( CObjSpec_TargetTrigger));
			(*ppDlg)->Create( CObjSpec_TargetTrigger::IDD, nullptr);
			break;

		case 2 :
			*ppMeta = TargetObject::static_meta();
			*ppDlg = new CObjSpec_TargetObject;
			MEMDUMP_NEW( *ppDlg, sizeof( CObjSpec_TargetObject));
			(*ppDlg)->Create( CObjSpec_TargetObject::IDD, nullptr);
			break;
		case 3 :
			*ppMeta = GameScreenObject::static_meta();
			*ppDlg = new CObjSpec_ScreenObject;
			MEMDUMP_NEW( *ppDlg, sizeof( CObjSpec_ScreenObject));
			(*ppDlg)->Create( CObjSpec_ScreenObject::IDD, nullptr);
			break;

		case 4 :
			*ppMeta = CEnterHelicopterTrigger::static_meta();
			*ppDlg = new CObjSpec_EnterHelicopter;
			MEMDUMP_NEW( *ppDlg, sizeof( CObjSpec_EnterHelicopter));
			(*ppDlg)->Create( CObjSpec_EnterHelicopter::IDD, nullptr);
			break;

		case 5 :
			*ppMeta = CGameObjectWeaponBox::static_meta();
			*ppDlg = new CObjSpec_WeaponBox;
			MEMDUMP_NEW( *ppDlg, sizeof( CObjSpec_WeaponBox));
			(*ppDlg)->Create( CObjSpec_WeaponBox::IDD, nullptr);
			break;

		case 6 :
			*ppMeta = CGameObjectRespawnHelicopter::static_meta();
			*ppDlg = new CObjSpec_RespawnHelicopter;
			MEMDUMP_NEW( *ppDlg, sizeof( CObjSpec_RespawnHelicopter));
			(*ppDlg)->Create( CObjSpec_RespawnHelicopter::IDD, nullptr);
			break;
		case 7 :		// CGameObjectEvent
			*ppMeta = CGameObjectEvent::static_meta();
			*ppDlg = new CObjSpec_Event;
			MEMDUMP_NEW( *ppDlg, sizeof( CObjSpec_Event));
			(*ppDlg)->Create( CObjSpec_Event::IDD, nullptr);
			break;
		case 8:		// CGameObjectJumpHold
			*ppMeta = CGameObjectJumpHold::static_meta();
			*ppDlg = new CObjSpec_JumpHold;
			MEMDUMP_NEW(*ppDlg, sizeof(CObjSpec_JumpHold));
			(*ppDlg)->Create(CObjSpec_JumpHold::IDD, NULL);
			break;

		case 9:			// CGameObjectBattleUseItem
			*ppMeta = CGameObjectBattleUseItem::static_meta();
			*ppDlg = new ObjSpec_BattleUseItem;
			MEMDUMP_NEW( *ppDlg, sizeof(ObjSpec_BattleUseItem));
			(*ppDlg)->Create(ObjSpec_BattleUseItem::IDD, NULL);
			break;
		default :
			*ppMeta = nullptr;
			*ppDlg = nullptr;
			break;
	}

	if(*ppDlg)
	{
		static INT32 passed = 0;

		if(!passed && (*ppDlg)->GetSafeHwnd() == nullptr)
		{
			passed++;
			// chunjong.song 2014-08-22 
			// (*ppDlg)->Create(..) 이후에 CDialog의 Wnd가 생성이안되는경우가있다.
			// 예를들어 i3LevelDesign툴에서 이소스가있는 DLL(보통PointBlank_ForLevelTool.i3Exec)을 Dynamic Loading시에 
			// i3LevelDesign에서 참조하고있는 i3 Engine Distribute DLL(i3XXX_DISTRIBUTE.dll)과
			// 현재소스가 참조하는 i3 Engine Distrinute DLL의 공유 MFCDLL이 서로틀릴경우 MFC내 Dialog 생성부분 에서 실패하는 듯하다.
			// 툴을 현재소스가 참조하는 i3 Engine에 맞추어서 재빌드해서 MFC DLL을 맞춘다. i3 Distribute버전으로
			AfxMessageBox("ERROR! - i3LevelTool에서 사용하는 i3엔진(i3XXX_Distrbute.dll과 현재DLL(PointBlank_ForLevelTool.i3Exec)에서 참조하는 i3엔진(Distribute DLL)이 틀립니다\n 현재클라이언트에서 사용하는 i3엔진으로 툴 재빌드를 요청해주세요!", MB_ICONERROR | MB_APPLMODAL);
		}
	}

	AfxSetResourceHandle( hOld);
}
#endif

