/**
 * \file	GameFramework.h
 *
 * Declares the game framework class.
 */


#if !defined( __GAME_FRAMEWORK_H)
#define __GAME_FRAMEWORK_H

#include "GameChatBox.h"
#include "MessageBoxManager.h"
#include "GameSupportSet.h"
#include "InGameQACommandSet.h"
#include "OutGameQACommandSet.h"
#include "InvenList.h"
//#include "ShopContext.h"
#include "ClanContext.h"
#include "CharaInfoContext.h"
#include "StringFilter.h"
#include "VideoCapture.h"
#include "RecLiveIcon.h"

#include "i3base/itl/list.h"
#include "../../ui/UISecCount.h"

#include "./CCmdManager/PBConsoleManager.h"
#include "../ConsoleLogger/ConsoleLogger.h"
#ifdef LSR_AVATAR_SYSTEM
	#include "Avatar/Avatar.h"
#endif //LSR_AVATAR_SYSTEM

#if legacy_gui_function
	#include "GuiDef.h"
#endif

#if BUILD_RELEASE_QA_VERSION
class TestAutomationInterface;
#endif

// 캐릭터 컨트롤러와 반응하는 오브젝트
#define OBJECT_COLLISION_BY_CHARA_CONTROLER( shapeGroup)	((shapeGroup == CGRP_OBJECT_WHOLE) || (shapeGroup == CGRP_OBJECT) || (shapeGroup == CGRP_OBJECT_ALL))
// 무기 발사 히트와 반응하는 오브젝트
#define OBJECT_COLLISION_BY_WEAPON_HIT( shapeGroup)			((shapeGroup == CGRP_OBJECT_HIT) || (shapeGroup == CGRP_OBJECT_WHOLE) || (shapeGroup == CGRP_OBJECT_WHOLE_DYNAMIC) || (shapeGroup == CGRP_OBJECT_ALL) )

#define ENABLE_TEST_COMMAND

enum VISION_MASK_TYPE
{	
	VE_NIGHT	= 0,
	VE_DINO_EYE,
	VE_MAX_COUNT
};

#define CONSOLE_LOG_PROTOCOL	0
#define CONSOLE_LOG_ETC			1

#define		MAX_ATTACKTRACK				10

struct CUSTOM_VERTEX
{
	VEC3D	Pos0;	DWORD	Diffues0 = 0;
	VEC3D	Pos1;	DWORD	Diffues1 = 0;
	VEC3D	Pos2;	DWORD	Diffues2 = 0;

	void SetColor(DWORD Color)
	{
		Diffues0 = Color;
		Diffues1 = Color;
		Diffues2 = Color;
	}
};

class LUAMANAGER;
class LUASCRIPT;
class CStageBattle;
class CDomiStageCli;
class CEquipShapeManager;
class CAnnounceConfig;
class UIMainFrame;
class UIFramework;
class UIFirstLoad;
class UIBattleFrame;
class IGShopContext;
class BGLoadThread;
class CVideoPlay;
class PlayTimeAlarmBanner;
class UIHUDLoadingBG;

class CGameFramework : public i3Framework
{
	I3_CONCRETE_CLASS_DEFINE( CGameFramework, i3Framework);

protected:
	// Game Key Mode
	GAMEKEY_MODE_STATE				m_GameKeyMode;

	//Keyboard
	UINT64							m_nKeyPress, m_nKeyStroke;
	UINT64							m_nOldKeyPress, m_nOldKeyStroke;
	UINT64							m_nSysKeyState, m_nSysKeyPress;

	//Mouse
	UINT32							m_nMouseButtonState;
	UINT32							m_nMouseButtonStrokeState;
	VEC3D							m_vMouseMoveDelta;

	i3AttrSet *						m_pAttrSetLogo[4];
	i3Sprite2D				*		m_pSpriteLogo[4];
	i3Texture				*		m_pTextureLogo[4];

	// 심의등급 로고
	i3Sprite2D*						m_pRatingLogo;
	i3AttrSet*						m_pAttrSetRatingLogo;

	//
	i3CollisionContext		*		m_pCollisionCtx;

	//Light
	i3AttrSet				*		m_pAttrSet;	
	i3ReflectMapBindAttr	*		m_pReflectMapAttr;
	
	//	RasterizeChain
	i3RasterizeChain_DownScale	*				m_pRasterizeDownScale;
	i3RasterizeChain_BrightPathExtract	*		m_pRasterizeBrigthPath;
	i3RasterizeChain_GaussianBlur	*			m_pRasterizeGaussianBlur;
	i3RasterizeChain_BloomCombine	*			m_pRasterizeBloomCombine;
	i3RasterizeChain_ToneMapping	*			m_pRasterizeToneMapping;

	i3PostProcessMotionBlur *					m_pPostProcessMotionBlur;
	i3PostProcessNightVision *					m_pPostProcessNightVision;
	i3PostProcessHaze*							m_pPostProcessHaze;
	i3PostProcessGamma*							m_pPostProcessGamma;

	REAL32										m_pVisionMixRate[VE_MAX_COUNT];
	//i3Texture						*			m_pVisionMaskingTex[VE_MAX_COUNT];
	VISION_MASK_TYPE							m_curVisionMaskingType;

	// 핵실드에 콜백에 의한 종료 처리에만 사용되는 변수입니다.
	REAL32										m_fExitDeltaTime;
	bool										m_bExitGame;

	// chat Box
	CGameChatBox			*		m_pChatBox;
	UIHUDLoadingBG			*		m_pHUDLoading;



	// 공용 메시지 박스
	CMessageBoxManager		*		m_pMessageBoxManager;
	
	i3InputKeyboard			*		m_pKeyboard;
	i3InputMouse			*		m_pMouse;

	// 2D Layer
	i3SgLayer				*		m_pMessageBoxLayer;
	i3SgLayer				*		m_pGUILayer;
	i3SgLayer				*		m_pNewUILayer;
	i3SgLayer				*		m_pHUDLayer;
	
	// 3D Layer
	i3SgLayer				*		m_pWorldLayer;

	// Game Support
	CGameSupportSet			*		m_pGameSupportSet;
	REAL32							m_rLoadingPercent;
	bool							m_bEnableScreenShot;
	REAL32							m_fScreenShotDeltaTime;

	// Video Capture
	CVideoCapture			*		m_pVideoCapture;

	// Video Play
	CVideoPlay				*		m_pVideoPlay;

	// Rec Live Icon 
	CRecLiveIcon			*		m_pRecLiveIcon;

	// Command Set
	CGameCommandSet			*		m_pGameCommandSet;
	COutGameQACommandSet	*		m_pOutGameQACommandSet;

	char							m_szLoadingText[256] = { 0 };

	// StageUtils

	CEquipShapeManager		*		m_pEquipShapeManager;
	CInvenList				*		m_pInvenList;
	IGShopContext*					m_pIGShopContext;

	CClanContext *					m_pClanContext;
	CAnnounceConfig*				m_pAnnounceConfig;

	// CStringFilter
	CStrFilterPtr					m_StringFilter;
	
#if defined( LOCALE_NORTHAMERICA )
	CStrFilterPtr					m_StringFilter2;		// 북미용 특수 스트링 필터
#endif

	CChattingInputCheckerPtr		m_ChattingInputChecker;


	// VersionInfo
#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined( SALES_BUILD)
	i3TextNodeDX2			*		m_pTextVersionInfo;
	i3TextNodeDX2			*		m_pTextFPS;
	bool							m_bVersionInfo;
#endif

	INT32							m_prevUISpriteCount;
	REAL32							m_timeNP;

	bool							m_bControlEnable;

	// 공지사항 - 서버에서 받아 출력합니다. 
	//i3::rc_wstring					m_wstrNoticeList[256];
	//i3::rc_wstring					m_wstrNoticeVersion;
	//i3::rc_wstring					m_wstrNoticeChat[10];
	i3::rc_wstring					m_wstrNoticeChat;
	i3::rc_wstring					m_wstrTempNotice;		// Sliding notice - Temporary
	i3::rc_wstring					m_wstrPermNotice;		// Sliding Notice - Permanent

	INT32							m_nNoticeChatCount;
	INT32							m_nNoticeListCount;
	bool							m_bNoticeActivation;
	bool							m_bNotictCheckDone;

	i3Timer *						m_pAutoMouseTimer;
	REAL32							m_AutoMouseTrack[ MAX_ATTACKTRACK];
	INT32							m_AutoMouseCount;
	INT32							m_AutoMouseAccmCount;

	char							m_CmdString[1024];

	//동영상 재생
	i3GfxMovie *					m_pMovie;
	i3TextureDX*					m_pTexture[2];
	i3Sprite2D *					m_pLoadMove;
	bool							m_pUseMovie;

	typedef	i3::list<i3UIScene*>	UISceneList;
	UISceneList						m_postDrawUIScenes;

	i3::vector<CConsoleLoggerEx*>	m_vecConsole;
protected:
	void					_UpdateCommonLoadScreen(REAL32 fDeltaTime);
	void					_EndCommonLoadScreen(void);
	bool					_JumpNextStage( const char* szStageName, bool bPush );

	void					_InitializePostProcess(void);
	void					_InitializeNightVision(void);

public:
	// Auto Mouse 관련
	void					addInput(void);
	bool					IsAutoMouse(void);
	void					ResetAutoMouse(void);


public:
	CGameFramework(void);
	virtual ~CGameFramework(void);

	virtual	void			OnCreate( void) override;
	virtual void			OnCommonLoadStart( bool bFirst) override;
	virtual bool			OnCommonLoading( bool bFirst, REAL32 fDeltaTime) override;
	virtual void			OnCommonLoadEnd( bool bFirst) override;
	virtual void			OnLoadStart( bool bFirst) override;
	virtual void			OnLoading( bool bFirst) override;
	virtual void			OnLoadingGameData(bool bFirst) override;
	virtual void			OnLoadEnd( bool bFirst) override;

	virtual void			OnFadeInStart( bool bFirst) override;
	virtual void			OnFadeInEnd( bool bFirst) override;

	virtual bool			OnLostDevice( bool bLostDevice) override;
	virtual bool			OnRevive(i3RenderContext * pCtx) override;

	virtual i3Node *		OnQueryAttachNode( i3GameRes * pRes) override;

	void					InitCollisionContext();	
	void					InitializePhysXCollisionGroup( bool bNewCollisionMode = false);
	i3CollisionContext *	getCollisionContext()	{	return m_pCollisionCtx;}

	i3PhysixContext *		getPhysixContext(void)		{ return GetViewer()->GetPhysixContext(); }

	I3_PHYSIX_HIT_RESULT *	raycastClosest( i3CollideeLine * pLine, INT32 grp);
	I3_PHYSIX_HIT_RESULT *	raycastClosest( i3CollideeLine * pLine, INT32 grp, i3EventReceiver * pTarget);
	UINT32					raycast( i3CollideeLine * pLine, INT32 grp);
	UINT32					raycast( i3CollideeLine * pLine, INT32 grp, i3EventReceiver * pTarget);

	UINT32					getRaycastResultCount(void)			{ return getPhysixContext()->getResultCount(); }
	I3_PHYSIX_HIT_RESULT *	getRaycastResult(void)				{ return getPhysixContext()->getResult(); }
	void					resetRaycastResult(void)			{ getPhysixContext()->resetResult(); }
	void					reserveRaycastResult( UINT32 cnt)	{ getPhysixContext()->reserveResult( cnt); }

	bool					getControlEnable(void)				{ return m_bControlEnable; }
	void					setControlEnable( bool bFlag)		{ m_bControlEnable = bFlag; }

	i3SgLayer	*	getWorldLayer()		{ return 	m_pWorldLayer;}	
	
	inline bool		IsCurStage( i3ClassMeta * pStageClass)	{ return (GetCurrentStage() != nullptr)? GetCurrentStage()->same_of( pStageClass):false;	}
	void			SetStateChina(void);
	void			SetStateRatingLogo(void);

	void			AddInGamePostProcessToManager(i3PostProcessManager * pManager);

	//Input
public:
	UINT64			getKeyPress( void)				{ return m_nKeyPress;}
	UINT64			getKeyStroke( void)				{ return m_nKeyStroke;}
	UINT64			getSysKeyStroke( void)			{ return m_nSysKeyState;}
	UINT64			getSysKeyPress( void)			{ return m_nSysKeyPress;}

	UINT64			getPrevKeyPress( void)			{ return m_nOldKeyPress;}
	UINT64			getPrevKeyStroke( void)			{ return m_nOldKeyStroke;}

	UINT32			getMouseButtonState()			{ return m_nMouseButtonState;}
	UINT32			getMouseButtonStrokeState(void)	{ return m_nMouseButtonStrokeState; }
	VEC3D	*		getMouseMoveDelta()				{ return &m_vMouseMoveDelta;}

	void			SetGameKey(INT32 nGameDevice, INT32 nKey);
	void			SetGamePressKeyMap( UINT64 map);
	void			SetGameStrokeKeyMap( UINT64 map);
	void			SetSysPressKeyMap(UINT64 map);
	void			SetSysStrokeKeyMap(UINT64 map);

	void			ResetInput();

	bool			BeInput(bool bStrokeOnly = true);	// 마우스+키보드 입력이 있나?

	void			setShadowEnable( bool bFlag);
	void			setShadowQuality( INT32 qual, bool bManualBound = false);

	void			SetNormalMapEnable( bool bEnable);


	// Game Key Mode Set & Get
	void			SetGameKeyMode( GAMEKEY_MODE_STATE state);
	GAMEKEY_MODE_STATE getGameKeyMode() { return m_GameKeyMode;	}

	// Chat Box 얻어오기
	CGameChatBox*	getChatBox()						{	return m_pChatBox;		}
	void			PutUserChatting( const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng, const bool bIsGM);
	void			PutSystemChatting(const i3::rc_wstring& str);

	// 설정된 다음 Stage로 진행합니다.
	void			GotoNextStage( bool bPush = false);
	void			GotoNextStage( const char* szStageName, bool bPush = false, i3StageInfo* pInfo = nullptr);

	// 
	bool			MoveNextStage( UINT32 nStageID );

	// Input을 얻어옵니다.
	i3InputKeyboard* getKeyboard()						{ return m_pKeyboard;		}
	i3InputMouse*	getMouse()							{ return m_pMouse;		}

	// 2D Layer를 얻어옵니다.
	i3AttrSet *		getMessagBoxLayer()					{ return m_pMessageBoxLayer->getLayerRoot();}
	i3AttrSet *		getGuiLayer()						{ return m_pGUILayer->getLayerRoot();		}
	i3AttrSet *		getHudLayer()						{ return m_pHUDLayer->getLayerRoot();		}

	CMessageBoxManager * GetMessageBoxManager( void)	{ return m_pMessageBoxManager; }

	bool			IsMessageBoxEnabled(void);
	void			CloseGameMessageBox(void);
	void			ResizeMessageBox(void);
	void			PreventMessageBoxWhenPhaseChange(void);

	// 공지사항 : 서버에서 내용 받아 출력합니다. 
	void			SetNotice_Temporary(const i3::rc_wstring& wstrMsg);
	void			SetNotice_Permanant(const i3::rc_wstring& wstrMsg, INT32 i32RGB = 0);

	// Loading관련 Text
	void			setLoadingText( char* pszText)		{ i3::safe_string_copy(m_szLoadingText, pszText, 256);}
	char *			getLodingText(void)					{ return m_szLoadingText;	}
	i3::pack::POINT2D			GetCenterPosition(INT32 nDrawWidth = 0,INT32 nDrawHeight = 0);
	i3::pack::POINT2D			GetSreenSize(void);

	REAL32			GetLoadingPercent(void) const		{ return m_rLoadingPercent; }
	void			SetLoadingPercent(REAL32 rValue)	{ m_rLoadingPercent = rValue; }

	//	Post Process
	i3PostProcessMotionBlur *	GetMotionBlur() { return m_pPostProcessMotionBlur; }
	i3PostProcessGamma*			GetPostProcessGamma() { return m_pPostProcessGamma; }
	i3PostProcessHaze *					GetHazePtr() { return m_pPostProcessHaze; }
	
	// Command Set
	CGameCommandSet*	  getCommandSet(void)					{ return m_pGameCommandSet;				}
	COutGameQACommandSet* getOutGameQACommandSet(void)			{ return m_pOutGameQACommandSet;				}

	// 공지사항 관련
	//void			LoadNoticeFromFile(void);
	//INT32			getNoticeListCount(void) { return m_nNoticeListCount; }
	//INT32			getNoticeChatCount(void) { return m_nNoticeChatCount; }
	//const i3::rc_wstring&	getNoticeList(INT32 index) const { return m_wstrNoticeList[index]; }
	//const i3::rc_wstring&	getNoticeVersion(void) const { return m_wstrNoticeVersion; }
	//const i3::rc_wstring&	getNoticeChatList(INT32 index) const { return m_wstrNoticeChat[index]; }

	bool			getNoticeActivation(void)		{ return m_bNoticeActivation;			}
	void			setNoticeActivation(bool bEnable){ m_bNoticeActivation = bEnable;		}
	void			setChatNotice(const i3::rc_wstring& wstr_msg, INT32 i32RGB);
	const i3::rc_wstring&	getChatNotice() const;

	const i3::rc_wstring&	getTempNotice() const;
	const i3::rc_wstring&	getPermNotice() const;

	// 동영상 녹화
	void			SaveVideoCapture(void)				 { m_pVideoCapture->SaveVideoCapture();					}
	i3Texture*		LoadHud3Texture(void)				 {return m_pVideoCapture->LoadHud3Texture();			}
	void			SetVideoCaptureInfo(UINT32 iIdx = 0) { m_pVideoCapture->SetVideoCaptureInfo(iIdx);			}
	void			SystemStopVideoCapture()			 { m_pVideoCapture->StopVideoCapture(m_pChatBox, false);}
	bool			IsVideoCapturing()					 { return m_pVideoCapture->IsCapturing()?true:false;	 }
	
	//동영상 녹화 및 방송 아이콘
	void			RecLiveUpdate(bool isLive, bool isRec)		{ if(m_pRecLiveIcon)m_pRecLiveIcon->OnUpdate( isLive,isRec ); }							

	bool	SaveImageCapture( const char* path, INT nQuality = 80, INT nCaptureMode = BCAP_MODE_D3D9, bool bCursor = false );	// 현재 화면 캡쳐 (png, jpg, bmp 확장자로만 저장됨)
	bool	SaveImageCapture( const wchar_t* wpath, INT nQuality = 80, INT nCaptureMode = BCAP_MODE_D3D9, bool bCursor = false );

	// 동영상 재생
	CVideoPlay*		GetVideoPlayer(void)				{	return m_pVideoPlay;				}
	/*void			PlayVideo(void)					{	m_pVideoPlay->PlayVideo();			}
	void			OpenVideoFile(const char *videoFile){	m_pVideoPlay->OpenFile(videoFile);	}
	void			StopVideo(void)						{	m_pVideoPlay->StopVideo();			}
	void			CloseVideo(void)					{	m_pVideoPlay->CloseVideo();			}
	void			PauseVideo(void)					{	m_pVideoPlay->PauseVideo();			}
	void			ReviveVideo( void)					{*/

public:
	virtual void	BuildSceneStruct(void) override;
	virtual void	InitSceneGraph() override;
	virtual void	OnInput() override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param, I3_EVT_CODE code) override;
	virtual void	OnChangeVideo( i3VideoInfo * pInfo) override;

	void	SetGameCursor( bool bActive = true);

public:	// StageUtils
	void	PrintVersion( bool bPrintServerType = false );
	void	PrintUserInfo();
	void	PrintUIDebug();
	void	UpdatePrintInfoText();
	void	UpdatePolygonCount();
	void	UpdateFPS();
	void	PrintHudUnitTest(const char* text);
	void	VisiblePrintInfoText(bool visible);

#if !defined(USE_LOCALE_UI)
	void		CreateReadyStageImageRes(void);			//레디룸 리소스 
	void		DestroyReadyStageImageRes(void); 
	void		CreateWeaponDescImageRes(INT32 WeaponID, i3GuiStatic* pDescStatic1, i3GuiStatic* pDescStatic2 );
	void		DestroyWeaponDescImageRes(void);
	void		CreateBossStageStartImageRes();
#endif

	void		CreateBattleStageImageRes(void);		//배틀 리소스 
	void		DestroyBattleStageImageRes(void);

	INT32			GetWeaponOrdIdx( WEAPON_CLASS_TYPE iWeaponClass, WEAPON::RES_ID iWeaponNum);							// 11.05.19.수빈.UI리뉴얼로 인해 추가해봄.
	bool			SetWeaponKillMark(i3Sprite2D* pSprite, WEAPON_CLASS_TYPE nWeaponClass, WEAPON::RES_ID nWeaponNum, bool bHP_Up, bool bHeadShot);	// revision 57808
	bool			SetWeaponSelectMark(i3GuiStatic* pStatic, WEAPON_CLASS_TYPE nWeaponClass, WEAPON::RES_ID nWeaponNum);			//
	bool			SetCharacterShape(i3GuiStatic* pStatic, INT32 CharaID);
	INT32			GetCharacterShape( T_ItemID ItemID);

	INT32			GetMapShapeIdx(STAGE_ID id);
	INT32			GetMapShapeIdx(STAGE_UID ord);
	INT32			GetEscapeMapShapeIdx(STAGE_ID id);
	INT32			GetEscapeMapShapeIdx(STAGE_UID ord);

	bool			SetRandomMapShape(i3GuiStatic* pStatic);
	INT32			GetRandomMapIdx( void);
	bool			SetCardShape(i3GuiStatic* pStatic,INT32 cardSetId,INT32 cardIdx);
	bool			SetCardSetShape(i3GuiStatic* pStatic,INT32 cardSetId);
	bool			SetRibbonShape(i3GuiStatic* pStatic,INT32 ribbonId);
	bool			SetEnsignShape(i3GuiStatic* pStatic,INT32 ensignId);
	bool			SetMedalShape(i3GuiStatic* pStatic,INT32 medalId);
	bool			SetCashItemShape(i3GuiStatic* pStatic, INT32 nGameItemFlag); 
	bool			SetPackageGoodsShape(i3GuiStatic* pStatic, INT32 gameGoodsId);
	bool			SetSlotImage(i3GuiStatic* pStatic, T_ItemID ItemID);

	UINT32			GetAvailableWeaponFlagInMyInven( WEAPON_CLASS_TYPE nWeaponClass, bool bCheckAuth = false, bool bKnuckle = false);

	IGShopContext*	GetIGShopContext(void) const { return m_pIGShopContext; }  
	CClanContext *	GetClanContext(void)	{ return m_pClanContext; }
	
	bool IsUsedWeapon(INT32 WeaponID);
	bool IsUsedCharacter(INT32 CharaID);


	//	인벤토리에 무기를 장착하고 뷰모델을 업데이트한다.
	bool			EquipWeaponToMyInven( T_ItemID WeaponID, bool bUpdateViewModel);

	//	인벤토리에 캐릭터 또는 기타 장비를 장착하고 뷰모델을 업데이트한다.
	bool			EquipCharacterToMyInven( T_ItemID ItemID);
	bool			EquipWeaponInGameMode(void);
	T_ItemID		FindWeaponIDForGameMode(const WEAPON_CLASS_TYPE class_type);
	T_ItemID		FindWeaponIDForGameMode(const T_ItemID ItemID);

	void			SetReflectMap( i3TextureCube * pTex );

	//bool			IsReadyStage(void);
	bool			IsBattleStage(void);
	bool			IsUIStage( void);

	void			SetFresnelEnable( bool bFlag);
		
	INT32			GetAnnounceCount(void) const;
	const char*		GetAnnounceName(INT32 idx) const;
	const char*		GetAnnounceURL(INT32 idx) const;
	const char*		GetAnnouncePath(INT32 idx) const;
	bool			IsAnnounceScroll(INT32 idx) const;

	// 핵실드에 콜백에 의한 종료 처리에만 사용되는 변수입니다.
	bool			IsExitGame(void) const { return m_bExitGame; }
	void			SetExitGame(bool bExitGame) { m_bExitGame = bExitGame; }
	void			EnableExitPopup(const i3::rc_wstring& wstr_msg);

	REAL32			GetExitDeltaTime(void) const { return m_fExitDeltaTime; }

	CEquipShapeManager* GetEquipShapeManager() const;

	void			ShowPlayTimeAlarm(bool show=true);
	void			SetPlayTimeAlarmBannerText(const i3::wstring& text);

	// 모든UIScene이 그려지고난후 출력되야할것들을 여기등록해서사용
	void AddPostDrawUIScene(i3UIScene* scene);
	void RemovePostDrawUIScene(i3UIScene* scene);
	friend void Gameframework_OnI3UIPostRender(i3UIManager* uiMan, i3UIRenderer* renderer);

public:
	// StringFilter
 	CStrFilter*	GetStringFilter() const			{ return m_StringFilter.get();	}

	CChattingInputChecker*	GetChattingInputChecker() const { return m_ChattingInputChecker.get(); }

	bool		IsFilterOK(const i3::wliteral_range& wTextRng) const { return m_StringFilter->IsFilterOK(wTextRng); }
	bool		IsFilterNickOK(const i3::wliteral_range& wTextRng) const	{ return m_StringFilter->IsFilterNickOK(wTextRng); }

	template<class Seq>	typename i3::disable_if_iterator<Seq,
	bool>::type		ChangeFilterStringOK(Seq& wstrText) { return m_StringFilter->ChangeStringOK(wstrText); }		// 명명을 바꾼다.. 

	bool		CheckValidNickname(const i3::wliteral_range& wNameRng);
	bool		CheckValidCharacterByLocale( const i3::wliteral_range& wNameRng);	// 오직 문자와 숫자로 구성됐는지 검사

	bool		CheckFilter_EnvSetMacroString(const i3::wliteral_range& wMacroRng, i3::rc_wstring& out_system_msg);				//필터검사후 메세지를 채팅박스에 추가합니다

private:

#if defined( LOCALE_NORTHAMERICA )
	bool		IsFilter2(const i3::wliteral_range& wTextRng) const { return m_StringFilter2->IsFilterNickOK(wTextRng);	}	// 북미용 추가 스트링 필터링
#endif

	bool			_IsMouseClip( bool bActive);
	bool			_IsMouseShow( bool bActive);

	virtual	 void	OnActivate( bool bActivate) override;

public:
#if !defined( I3_NO_PROFILE)
	HWND							m_hWndEdigGameDlg;
	HWND							m_hWndDbgMsgDlg;
	HWND							m_hWndProfilingDlg;
	HWND							m_hWndSoundEditDlg;
	UINT32							m_nLightChangeState;
	void							UpdateRecvmsg();
	void							UpdateSendmsg();
	void							UpdatePacketState();
	void							UpdateHolepunchState();
	void							UpdateGrenadeState();
	void							UpdateTcpMessage(UINT32 nType);
	void							UpdateCharaState();
	void							LogMessageToDbgWindow( const char *format, ...);
#endif

	virtual bool					OnQueryClose(void) override;

protected:

	// Font 생성
	void			CreateGameFont( void);

	void			_LogPerf(void);

public:
	void			SetNightVisionEnable(bool bFlag) { I3ASSERT_RETURN(m_pPostProcessNightVision); m_pPostProcessNightVision->SetEnable(bFlag); }
	void			SetVisionMaskingType( VISION_MASK_TYPE type);
	void			SetVisionMixRate( REAL32 rate);

	VISION_MASK_TYPE GetCurVisionType( void)		{ return m_curVisionMaskingType;}
	REAL32			GetCurMixRate(void)				{ return m_pVisionMixRate[m_curVisionMaskingType];}

	bool			IsStage(STAGE_ID eID);

public:
	void			SetCommandString(const char * pCmdString);
	const char *	GetCommandString()	{return m_CmdString;}

#if defined( AI_MOVE_TO_WEAPON)
	void			CheckWeapons(void);
	bool			_Rec_CheckAIState( i3AIContext * pCtx, const char * pszCharaName);
#endif

public:
	bool	OpenWebBrowser(const i3::wliteral_range& wURLRng, LPCTSTR postData = nullptr, bool isPopup = false);

	//@eunil. PBRU-1065 V1 머지
	// 현재 설정되어 있는 웹브라우저(explorer, firefox, chrome ...)를 띄우기. 
	// 전세계적으로 firefox를 가장 많이 사용하고 있으며, 
	// 만약 explorer를 사용자가 삭제했다면 OpenWebBrowser()는 정상 작동하지 않을 것임.
	void	OpenWebBrowserCurrSet(const i3::wliteral_range& wURLRng);

public:
	static	LRESULT CALLBACK StaticWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	WNDPROC	m_OldWndProc;

	
public:
	CStageBattle*			GetStageBattle(void);

	//------------------------------------------------------------------------------//
	//								New UI Interface								//
	//------------------------------------------------------------------------------//
private:
	UIFirstLoad *	m_pUIFirstLoad;

public:
	UIMainFrame *	GetUIMainframe( void);

	/** \brief UI Frame으로 이동합니다.
		\desc 구버전 StageReady
			신규버전은 UIMainFrame으로..
		\param[in] bToLobby 로비로 강제이동하는 경우 true	*/
	static void		JumpToReady( bool bToLobby = false);

	/** \brief Login Stage로 이동합니다.
		\desc 구버전은 Login
			신규버전은 NewLogin으로... */
	static void		JumpToLogin( void);

	void			JumpToBattle(void);

	void			SetMainLocale(void);

	// 루아 관련
private:
	LUAMANAGER*  m_pLuaManager;

public:
	LUASCRIPT* CreateLuaScript(char* szName);
	void DeleteLuaScript(LUASCRIPT* pLuaScript);

	// 디버깅 관련
#ifdef I3_DEBUG

public:
	i3Node*		m_arDebugObject[10];
	void LoadDebugObj(void);
	void SetPosDebugObj(INT32 idx, VEC3D& v3Pos);
	void SetPosDebugObj(INT32 idx, REAL32 x, REAL32 y, REAL32 z);

#endif
	//CHAR_SOLO_RENDER
	i3Node*							m_pLoadedPlayerRoot;
	i3AttrSet*						m_pPlayerAttrSet;
	i3ReflectMapBindAttr*			m_pPlayerReflectMapAttr;

public:
	i3Node *						OnPlayerQueryAttachNode( i3GameRes * pRes);

	bool m_EnableCapture;

	// Resource management
	i3GameRes *		QuaryResource( const char * pszName, bool bCommonLoad = false);


	//////////////////////////////////////////////////////////////////////////////
	//								Background load								//
	//////////////////////////////////////////////////////////////////////////////
private:
	BGLoadThread *		m_pBGLoadThread;

public:
	BGLoadThread *	GetBGLoadThread( void);


private:
	INT32			m_TextureQual;
	bool			m_bEnableNormalMap;
	bool			m_bEnableSpecularMap;

public:
	void		SetTextureQuality( INT32 qual)			{ m_TextureQual = qual; }
	INT32		GetTextureQuality( void)				{ return m_TextureQual; }

	void		SetEnableNormalMap( bool bEnable)		{ m_bEnableNormalMap = bEnable; }
	bool		GetEnableNormalMap( void)				{ return m_bEnableNormalMap; }

	void		SetEnableSpecularMap( bool bEnable)		{ m_bEnableSpecularMap = bEnable; }
	bool		GetEnableSpecularMap( void)				{ return m_bEnableSpecularMap; }

	void PrintConsole(int attributes, const char *format, ...);
	void ConsoleLogger();

	i3AttrSet* GetLightAttrSet() { return m_pAttrSet; }
public:
	static void		LoadAIStateAnimation( i3AIState * pAIState);

	//////////////////////////////////////////////////////////////////////////////
	//								Test Automation								//
	//////////////////////////////////////////////////////////////////////////////
#if defined( TEST_AUTOMATION)
protected:
	TestAutomationInterface *			m_pTestAuto;
#endif

	///////////////////////////////////////////////////////////////////////
	///////					ChatLimit								///////
	///////////////////////////////////////////////////////////////////////
	// 채팅 도배금지 시스템 : Keyword : ChatLimit

private:
	bool					m_bChatLimitState;	// 진행중-true, 초기값/종료-false
	UISecCount				m_ChatLimitSec;		// 서버로부터 채팅금지 시작부터 시간 측정.

public:
	void SetChatLimit(INT32 result_time);
	void ChatLimit_Start();
	bool CheckChatLimit(INT32 result_time);

	void OnChatLimit_Start(INT32 result_time);
	void OnChatLimit_Change(INT32 result_time);
	void OnChatLimit_DoneOrStop(INT32 result_time);

private:
	TUISecCallback<CGameFramework,
		&CGameFramework::OnChatLimit_Start,
		&CGameFramework::OnChatLimit_Change,
		&CGameFramework::OnChatLimit_DoneOrStop,
		&CGameFramework::OnChatLimit_DoneOrStop
		>	m_ChatLimitCB;
};

#endif
