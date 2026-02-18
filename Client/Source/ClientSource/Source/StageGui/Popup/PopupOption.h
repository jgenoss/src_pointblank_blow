#ifndef _PopupOption_h
#define _PopupOption_h

#include "PopupBase.h"
#include "EnvSet.h"

#define MAX_KEYMAP_SLOT		7
#define MAX_KEY_MAP_COUNT	

// UI에서 Key 배치에 사용되어집니다.
enum GAME_KEY_MAP_ARRANGE
{
	// 이동
	GAME_KEY_MAP_ARRANGE_LEFT = 0,
	GAME_KEY_MAP_ARRANGE_RIGHT,
	GAME_KEY_MAP_ARRANGE_FORWARD,
	GAME_KEY_MAP_ARRANGE_BACKWARD,
	GAME_KEY_MAP_ARRANGE_JUMP,									// 점프 키
	GAME_KEY_MAP_ARRANGE_FOOTSTEP,								// 걷기
	GAME_KEY_MAP_ARRANGE_CROUCH,								// 앉기
	GAME_KEY_MAP_ARRANGE_TURNBACK,								// Extension 활성화 버튼

	// 무기사용
	GAME_KEY_MAP_ARRANGE_ATTACK,								// 무기 공격
	GAME_KEY_MAP_ARRANGE_SUBFUNC,								// 특수기능
	GAME_KEY_MAP_ARRANGE_RELOAD,								// 탄환 재장전

	// 무기교체
	GAME_KEY_MAP_ARRANGE_CHANGEWEAPON,							// 무기교체창 활성화
	GAME_KEY_MAP_ARRANGE_CHANGEWEAPON_1,						// 주무기로 교체
	GAME_KEY_MAP_ARRANGE_CHANGEWEAPON_2,						// 보조무기로 교체
	GAME_KEY_MAP_ARRANGE_CHANGEWEAPON_3,						// 근접무기로 교체
	GAME_KEY_MAP_ARRANGE_CHANGEWEAPON_4,						// 투척무기로 교체
	GAME_KEY_MAP_ARRANGE_CHANGEWEAPON_5,						// 특수무기로 교체
	GAME_KEY_MAP_ARRANGE_CHANGEWEAPON_6,						// 폭발물로 교체
	GAME_KEY_MAP_ARRANGE_CHANGEWEAPON_QUICK,					// 지난무기로 교체
	GAME_KEY_MAP_ARRANGE_CHANGEWEAPON_PREV,						// 이전무기로 교체
	GAME_KEY_MAP_ARRANGE_CHANGEWEAPON_NEXT,						// 다음무기로 교체
	GAME_KEY_MAP_ARRANGE_DROP,									// 무기 드랍 버튼	
	
	// 기타0
	GAME_KEY_MAP_ARRANGE_SHOWSCORE,								//	현재 게임전적 보기
	GAME_KEY_MAP_ARRANGE_TOGGLE_MAP,							// 전체맵 보기
	GAME_KEY_MAP_ARRANGE_MINIMAP_ENLARGE,						// 미니맵 확대
	GAME_KEY_MAP_ARRANGE_MINIMAP_REDUCTION,						// 미니맵 축소
	GAME_KEY_MAP_ARRANGE_CHAT,									// 채팅
	GAME_KEY_MAP_ARRANGE_CHAT_ALL,								// 전체채팅모드
	GAME_KEY_MAP_ARRANGE_CHAT_TEAM,								// 팀 채팅모드	
	GAME_KEY_MAP_ARRANGE_CHAT_CLAN,								// 클랜 채팅모드
	GAME_KEY_MAP_ARRANGE_CHAT_HISTORY,							// 지난 대화보기(게임안에서만 사용됨)
	GAME_KEY_MAP_ARRANGE_VOICECHAT,								// 음성채팅
	GAME_KEY_MAP_ARRANGE_RADIOMESSAGE1,							// 라디오 메시지 1
	GAME_KEY_MAP_ARRANGE_RADIOMESSAGE2,							
	GAME_KEY_MAP_ARRANGE_RADIOMESSAGE3,
	GAME_KEY_MAP_ARRANGE_SPECIALACTION,							// 특수 행동키(폭발물 해체등)
	GAME_KEY_MAP_ARRANGE_MOUSE_SENS_UP,							// 마우스 감도 증가
	GAME_KEY_MAP_ARRANGE_MOUSE_SENS_DOWN,						// 마우스 감도 감소
	GAME_KEY_MAP_ARRANGE_SCREENSHOT,							// 화면저장		

	GAME_KEY_MAP_ARRANGE_COUNT,
};

enum OptionSectType
{
	OS_START = 0,

	OS_PLAY = OS_START,
	OS_VIDEO,
	OS_INTERFACE,
	OS_SOUND,
	OS_CONTROL,
	OS_CAPACITY,
	OS_COMMUNITY,
	OS_MACRO,
	OS_VOICE,
	OS_RATING,

	MAX_OS
};

struct OptionSect
{
	i3GuiButton* _pButton;
	i3GuiStatic* _pSect;
};

struct KeymapSlot 
{
	i3GuiEditBox* _pKey;
	i3GuiEditBox* _pValue;
};


class CMessageBox;

const char *		GetKeyboardString(INT8 device_type, UINT32 key_state);
const char *		GetKeymapString(GAME_KEY_MAP key_map);

class CPopupOption: public CPopupBase
{
	I3_CLASS_DEFINE(CPopupOption);
protected:
	i3BinList		m_VidList;
	i3BinList		m_HzList;
	i3List			m_AAList;
	INT32			m_NormalModeCount;
	INT32			m_WideModeCount;
	INT32			m_nVideoResolution;

	bool			m_bNeedToValidate;

	BOOL			m_bOldFullScreen;


protected:
	void			_addVidInfo( I3G_DISPLAY_MODE_INFO * pInfo);
	void			_removeAllVidInfo(void);
	INT32			getVidInfoCount(void)								{ return m_VidList.GetCount(); }
	VID_INFO *		getVidInfo( INT32 idx)								{ return (VID_INFO *) m_VidList.Items[idx]; }
	void			_updateResList( INT32 bits);
	INT32			_findVideoRes( INT32 width, INT32 height);

	void			_refreshAAInfo(void);
	void			_removeAllAAInfo( bool bDestructor = false);
	INT32			_findAA( INT32 type, INT32 quality);
	void			_getAAString( INT32 aa, INT32 aaq, char * pszStr, const size_t sizeStrMax);

	void			_refreshHzInfo(void);
	void			_removeAllHzInfo( bool bDestructor = false);
	INT32			_findHz( INT32 hz);

	void			_refreshVCResInfo(void);
	void			_removeAllVCResInfo( bool bDestructor = false);

	bool			_ValidateShader(void);

public:
	CPopupOption();
	virtual ~CPopupOption();
	
	void		Process_Confirm();
	void		Process_Cancel();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnExitKey(void);
	virtual void OnWheelUpdate(INT32 scrollby);
	virtual void SetInputDisable(BOOL bDisable);
	virtual void SetPushedStateButton(INT32 menuButtonType,BOOL bPushed);
	virtual BOOL GetInputDisable(void);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

	void OnInGame(void);

	void OnPlay(void);
	void OnVideo(void);
	void OnInterface(void);
	void OnSound(void);
	void OnControl(void);
	void OnCapacity(void);
	void OnCommunity(void);
	void OnMacro(void);
	void OnVoice(void);
	void OnRating(void);

	void OnVideoResolutionCombo(void);
	void OnVideoResolutionPopup(UINT32 idxItem);
	void OnVideoColorCombo(void);
	void OnVideoColorPopup(UINT32 idxItem);
	void OnVideoScanRateCombo(void);
	void OnVideoScanRatePopup(UINT32 idxItem);
	void OnVideoAntiAliasingCombo(void);
	void OnVideoAntiAliasingPopup(UINT32 idxItem);
	void OnVideoCaptureCombo(void);
	void OnVideoCapturePopup(UINT32 idxItem);
	void OnVideoWindowMode(void);
	void OnVideoScreenSync(void);
	void OnVideoSmoothDistance(void);
	void OnVideoGammaPrev(void);
	void OnVideoGammaNext(void);

	void OnPlayBloodType(UINT32 idxBlood);
	void OnPlayBlood(void);
	void OnPlayHit(void);
	void OnPlayHandTypeCombo(void);
	void OnPlayHandTypePopup(UINT32 idx);
	void OnPlayCorsshair(UINT32 idxCrosshair);
	
	void OnInterfaceNickname(void);
	void OnInterfaceMark(void);
	void OnInterfaceEffect(void);
	void OnInterfaceChallenge(void);
	void OnInterface1VS1(void);
	void OnInterfaceRadarButton(void);
	void OnInterfaceRadarPopup(UINT32 idxItem);

	void OnSoundBgm(void);
	void OnSoundBgmPrev(void);
	void OnSoundBgmNext(void);
	void OnSoundEffect(void);
	void OnSoundEffectPrev(void);
	void OnSoundEffectNext(void);
	void OnSoundSpeakerCombo(void);
	void OnSoundSpeakerPopup(UINT32 idxSpeaker);
	void OnSoundVoice(void);
	void OnSoundVoiceCombo(void);
	void OnSoundVoicePopup(UINT32 idxVoice);

	void OnCapacityBullet(void);
	void OnCapacityBulletSmoke(void);
	void OnCapacityLight(void);
	void OnCapacityNormal(void);
	void OnCapacityTerrain(void);
	void OnCapacityDepth(void);
	void OnCapacityBloom(void);
	void OnCapacityPhysX(void);
	void OnShadowCombo(void);
	void OnShadowPopup(UINT32 idxShadow);
	void OnTextureCombo(void);
	void OnTexturePopup(UINT32 idxTexture);
	void OnSpecularCombo(void);
	void OnSpecularPopup(UINT32 idxSpecular);
	void OnEffectCombo(void);
	void OnEffectPopup(UINT32 idxEffect);
	
	void OnCommunityNoteComb(void);
	void OnCommunityInviteComb(void);
	void OnCommunityWhisperComb(void);
	void OnCommunityChatComb(void);	
	void OnCommunityNotePopup(UINT32 idxItem);
	void OnCommunityInvitePopup(UINT32 idxItem);
	void OnCommunityWhisperPopup(UINT32 idxItem);
	void OnCommunityChatPopup(UINT32 idxItem);

	void OnCommunityMacro1(UINT32 idxItem);
	void OnCommunityMacro2(UINT32 idxItem);
	void OnCommunityMacro3(UINT32 idxItem);
	void OnCommunityMacro4(UINT32 idxItem);
	void OnCommunityMacro5(UINT32 idxItem);

	void OnControlListFocus(UINT32 idxItem);
	void OnControlListSelect(INT32 idxItem);
	void OnControlListChange(INT32 idxItem);
	void OnControlListDrag(void);
	void OnControlReverse(void);
	void OnControlMousePrev(void);
	void OnControlMouseNext(void);
	void OnControlFovPrev(void);
	void OnControlFovNext(void);
	void OnControlButton(void);
	void OnControlDelete(void);

private:
	void _EnableExclusionSect(OptionSectType enableSect);	
	void _SetGammaGage(UINT32 value);
	void _InitCommonControl(void);
	void _InitGamePlay(void);
	void _InitVideo(void);
	void _InitInterface(void);
	void _InitSound(void);
	void _InitControl(void);
	void _InitPerformance(void);
	void _InitCommunity(void);
	void _InitMacro(void);
	void _InitVoiceChat(void);
	void _InitRating(void);

	BOOL _IsChangedOption(void);	
	BOOL _IsInputingIme(void);

	void					UpdateInit(void);
	void					OptionInitLoad(void);									// 옵션메뉴 활성화 최초시 한번만 실행
	void					OptionSave(void);										// 옵션 변경 완료시
	void					InGameOptionApply(void);

	// 각종 초기화 함수
	void					_ResetGamePlay(void);
	void					_ResetVideo(void);
	void					_ResetInterface(void);
	void					_ResetSound(void);
	void					_ResetControl(void);
	void					_ResetPerformance(void);
	void					_ResetCommunity(void);
	void					_ResetMacro(void);
	void					_ResetVoiceChat(void);

	// 옵션 설정 처리 버튼
	void					OnOptionConfirm(void);								// 옵션 적용 성공
	void					OnOptionCancel(void);								// 옵션 적용 취소
	void					OnOptionInitial(void);								// 옵션 초기화						
	void					OnOptionOptimize(void);								// 성능 최적화						
	static void				CbConfirm(void* pThis,INT32 nParam);
	static void				CbInit(void* pThis,INT32 nParam);
	static void				CbOptimize(void* pThis,INT32 nParam);
	static void				CbDissolveImeInput(void* pThis,INT32 nParam);

	////////////////////////////////////////////////////////////////
	// 게임플레이 환경 설정
	////////////////////////////////////////////////////////////////
	void					_SetGamePlayBloodType( INT32 nType);
	void					_SetGamePlayBlood(void);
	void					_SetGamePlayCrossHairEffect(void);
	void					_SetGamePlayCrossHairType( INT32 nType);
	void					_SetGamePlayHandType(void);
	
	////////////////////////////////////////////////////////////////
	// 게임내에서의 Video 설정
	////////////////////////////////////////////////////////////////
	void					_SetVideoOptionEnable();	// 옵션 버튼 활성화/ 비활성화
	void					_SetVideoMode( bool bInit = false);
	void					_SetVideoResolution(INT32 idxResolution, bool bInit = false);
	void					_SetVideoColor( bool bInit = false);
	void					_SetVideoScanRate(INT32 idxScanRate);
	void					_SetVideoAntiAliasing(INT32 idxAntiAliasing);
	void					_SetVideoCaptureResolution(void);
	void					_SetVideoVSync(void);
	void					_SetVideoTriLinearFilter(void);
	void					_SetVideoGammaUp(void);
	void					_SetVideoGammaDown(void);
	void					_SetVideoGammaSlider(void);
	void					SetGammaState(void);

	bool					_NeedToUserVerify(void);
	bool					_NeedToRestart(void);

	void					_SetVideo( bool bRestore);
	void					_VideoResolution( );
	void					_PopupUserVerify(void);
	static	void			_CbUserVerify( void * pThis, INT32 nParam);

	void					_PopupNotifyValidateFail(void);
	static void				_CbNotifyValidateFail( void * pThis, INT32 nParam);

	////////////////////////////////////////////////////////////////
	// 게임내에서의 Interface설정
	////////////////////////////////////////////////////////////////
	void					_SetInterfaceNick(void);
	void					_SetInterfaceMark(void);

	////////////////////////////////////////////////////////////////
	// 게임내에서의 Control설정
	////////////////////////////////////////////////////////////////
	void					_SetControlMouseInvert(void);
	void					_InitKeyboardSetting(void);
	const char *			_GetKeyboardExplain(INT32 idx);
public:
	static const char *		_GetKeyboardValue(GAME_KEY_STRUCT *pGameKey);
protected:
	void					_SetKeyMapValue( INT32 idx, I3I_KEY key);
	void					_SetMouseMapValue( INT32 idx, UINT32 key);
	void					_UpdateKeyboardValue(void);
	static void				CbKeymap(void* pThis,INT32 nParam);
	static void				CbMousemap(void* pThis,INT32 nParam);
	BOOL					_CheckKeyMap( I3I_KEY key, INT32 exception, INT32* DuplicateIdx,INT8 deviceType);
	BOOL					_CheckKeyMap( UINT32 key, INT32 exception, INT32* DuplicateIdx,INT8 deviceType);
	void					_SetControlKeySlider(INT32 nVal = 0);
	void					_UpdateKeyMap(void);
	void					_SetControlKeyChange(void);
	void					_SetControlKeyDelete(void);
	void					_SetControlMouseSense( REAL32 fVal = 0.0f);
	void					_SetControlMouseSenseSlider(void);
	void					_SetControlFov( REAL32 fVal = 0.0f);
	void					_SetControlFovSlider(void);
	void					SetDefaultControlSetting(void);
	void					SetNewControlSetting(void);

	////////////////////////////////////////////////////////////////
	// 게임내에서의 Sound설정
	////////////////////////////////////////////////////////////////
	void					_SetSoundSpeaker(void);
	void					_SetSoundBG(REAL32 fVal = 0.0f);
	void					_SetSoundFX(REAL32 fVal = 0.0f);
	void					_SetRadioMessage(void);
	void					SetSoundSetting(void);


	////////////////////////////////////////////////////////////////
	// 게임내에서의 성능설정
	////////////////////////////////////////////////////////////////
	void					_UpdateShadow(void);
	void					_SetPerformanceShadow(UINT32 idxShadow);
	void					_SetPerformanceBulletTrace(void);
	void					_SetPerformanceBulletSmokeTrace(void);
	void					_SetPerformanceDynamicLight(void);
	void					_SetPerformanceNormalMap(void);
	void					_SetPerformanceTerrainEffect(void);
	void					_SetPerformanceTextureQuality(UINT32 idxTexture);
	void					_SetPerformanceSpecularQuality(UINT32 idxSpecular);
	void					_SetPerformanceEffectQuality(UINT32 idxEffect);

	static void				_CbConfirmDynamicLight( void * pThis, INT32 nParam);
	static void				_CbConfirmNormalMap( void * pThis, INT32 nParam);
	static void				_CbConfirmTextureQuality( void * pThis, INT32 nParam);
	static void				_CbConfirmShadow( void * pThis, INT32 nParam);
	static void				_CbConfirmSpecularQuality(void* pThis,INT32 nParam);

	////////////////////////////////////////////////////////////////
	// 게임내에서의 커뮤니티 설정
	////////////////////////////////////////////////////////////////
	void					_SetCommunityChat(UINT32 idxItem);
	void					_SetCommunityWhisper(UINT32 idxItem);
	void					_SetCommunityInvite(UINT32 idxItem);	
	void					_SetCommunityMessage(UINT32 idxItem);	


	////////////////////////////////////////////////////////////////
	// 게임내에서의 매크로 설정
	////////////////////////////////////////////////////////////////
	void					_SetMacro(INT32 nType, BOOL bFlag = TRUE);
	
	void					UpdateGamePlay( REAL32 rDeltaSeconds );
	void					UpdateVideo( REAL32 rDeltaSeconds );
	void					UpdateInterface(REAL32 rDeltaSeconds);
	void					UpdateSound( REAL32 rDeltaSeconds );
	void					UpdateControl( REAL32 rDeltaSeconds );
	void					UpdatePerformance( REAL32 rDeltaSeconds );
	void					UpdateCommunity( REAL32 rDeltaSeconds );
	void					UpdateMacro( REAL32 rDeltaSeconds );
	void					UpdateVoiceChat( REAL32 rDeltaSeconds );

	void					_UpdateBgmValue(REAL32 rDeltaSeconds);
	void					_UpdateEffectValue(REAL32 rDeltaSeconds);
	void					_UpdateGammaValue(REAL32 rDeltaSeconds);
	void					_UpdateMouseValue(REAL32 rDeltaSeconds);
	void					_UpdateFovValue(REAL32 rDeltaSeconds);

private:
	i3GuiStatic* m_pOptionGui;
	i3GuiStatic* m_pOptionWin;
	i3GuiEditBox* m_pOptionCaption;
	OptionSect m_sect[MAX_OS];
	i3GuiButton* m_pInit;
	i3GuiButton* m_pOptimize;
	i3GuiButton* m_pCofirm;
	i3GuiButton* m_pCancel;	
	i3GuiEditBox* m_pFrameCaption;

	i3GuiEditBox* m_pVideoResolutionKey;
	i3GuiEditBox* m_pVideoResolutionCombo;
	i3GuiButton* m_pVideoResolutionButton;
	i3GuiPopupList* m_pVideoResolutionPopup;

	i3GuiEditBox* m_pVideoColorKey;
	i3GuiEditBox* m_pVideoColorCombo;
	i3GuiButton* m_pVideoColorButton;
	i3GuiPopupList* m_pVideoColorPopup;
	i3GuiEditBox* m_pVideoScanRateKey;
	i3GuiEditBox* m_pVideoScanRateCombo;
	i3GuiButton* m_pVideoScanRateButton;
	i3GuiPopupList* m_pVideoScanRatePopup;
	i3GuiEditBox* m_pVideoAntiAliasingKey;
	i3GuiEditBox* m_pVideoAntiAliasingCombo;
	i3GuiButton* m_pVideoAntiAliasingButton;
	i3GuiPopupList* m_pVideoAntiAliasingPopup;

	i3GuiEditBox* m_pVideoCaptureKey;
	i3GuiEditBox* m_pVideoCaptureCombo;
	i3GuiButton* m_pVideoCaptureButton;
	i3GuiButton* m_pVideoCaptureTooltip;
	i3GuiPopupList* m_pVideoCapturePopup;

	i3GuiCheckBox* m_pVideoWindowMode;
	i3GuiCheckBox* m_pVideoScreenSync;
	i3GuiCheckBox* m_pVideoSmoothDistance;
	i3GuiEditBox* m_pVideoWindowModeKey;
	i3GuiEditBox* m_pVideoScreenSyncKey;
	i3GuiEditBox* m_pVideoSmoothDistanceKey;
	i3GuiEditBox* m_pVideoGammaKey;
	i3GuiButton* m_pVideoGammaPrev;
	i3GuiButton* m_pVideoGammaNext;
	i3GuiStatic* m_pVideoGammaGage;
	i3GuiEditBox* m_pVideoGammaNumber;
	i3GuiStatic * m_pArrowVideoAntiAliasing;
	i3GuiStatic * m_pArrowVideoColor;
	i3GuiStatic * m_pArrowVideoScanRate;
	i3GuiStatic * m_pArrowVideoResolution;
	i3GuiStatic	* m_pArrowVideoCapture;

	i3GuiEditBox* m_pPlayBlood2Key;
	i3GuiRadioButton* m_pPlayBloodRadio1;
	i3GuiRadioButton* m_pPlayBloodRadio2;
	
	i3GuiCheckBox* m_pPlayBlood;
	i3GuiCheckBox* m_pPlayHit;
	i3GuiEditBox* m_pPlayBloodKey;
	i3GuiEditBox* m_pPlayHitKey;
	i3GuiEditBox* m_pPlayCrosshairKey;
	i3GuiStatic* m_pPlayHandType;
	i3GuiEditBox* m_pPlayHandTypeKey;
	i3GuiEditBox* m_pPlayHandTypeCombo;
	i3GuiButton* m_pPlayHandTypeButton;
	i3GuiPopupList* m_pPlayHandTypePopup;
	i3GuiStatic* m_pPlayCrosshairType1;
	i3GuiStatic* m_pPlayCrosshairType2;
	i3GuiStatic* m_pPlayCrosshairType3;
	i3GuiStatic* m_pPlayCrosshairType4;
	i3GuiRadioButton* m_pPlayCrosshair1;
	i3GuiRadioButton* m_pPlayCrosshair2;
	i3GuiRadioButton* m_pPlayCrosshair3;
	i3GuiRadioButton* m_pPlayCrosshair4;

	i3GuiEditBox* m_pInterfaceNicknameKey;
	i3GuiEditBox* m_pInterfaceMarkKey;
	i3GuiEditBox* m_pInterfaceEffectKey;
	i3GuiEditBox* m_pInterfaceChallengeKey;
	i3GuiEditBox* m_pInterface1VS1Key;
	i3GuiCheckBox* m_pInterfaceNickname;
	i3GuiCheckBox* m_pInterfaceMark;
	i3GuiCheckBox* m_pInterfaceEffect;
	i3GuiCheckBox* m_pInterfaceChallenge;
	i3GuiCheckBox* m_pInterface1VS1;
	i3GuiEditBox* m_pInterfaceRadarKey;
	i3GuiEditBox* m_pInterfaceRadarCombo;
	i3GuiButton* m_pInterfaceRadarButton;
	i3GuiPopupList* m_pInterfaceRadarPopup;

	i3GuiCheckBox* m_pSoundBgm;
	i3GuiEditBox* m_pSoundBgmKey;
	i3GuiEditBox* m_pSoundBgmValue;
	i3GuiButton* m_pSoundBgmPrev;
	i3GuiButton* m_pSoundBgmNext;
	i3GuiStatic* m_pSoundBgmGage;
	i3GuiCheckBox* m_pSoundEffect;
	i3GuiEditBox* m_pSoundEffectKey;
	i3GuiEditBox* m_pSoundEffectValue;
	i3GuiButton* m_pSoundEffectPrev;
	i3GuiButton* m_pSoundEffectNext;
	i3GuiStatic* m_pSoundEffectGage;
	i3GuiEditBox* m_pSoundSpeakerKey;
	i3GuiEditBox* m_pSoundSpeakerCombo;
	i3GuiButton* m_pSoundSpeakerButton;
	i3GuiPopupList* m_pSoundSpeakerPopup;
	i3GuiCheckBox* m_pSoundVoice;
	i3GuiEditBox* m_pSoundVoiceKey;
	i3GuiEditBox* m_pSoundVoiceCombo;
	i3GuiButton* m_pSoundVoiceButton;
	i3GuiPopupList* m_pSoundVoicePopup;

	i3GuiCheckBox* m_pCapacityBullet;
	i3GuiCheckBox* m_pCapacityBulletSmoke;
	i3GuiCheckBox* m_pCapacityLight;
	i3GuiCheckBox* m_pCapacityNormal;
	i3GuiCheckBox* m_pCapacityTerrain;
	i3GuiCheckBox* m_pCapacityDepth;
	i3GuiCheckBox* m_pCapacityBloom;
	i3GuiCheckBox* m_pCapacityPhysX;
	i3GuiEditBox* m_pCapacityBulletKey;
	i3GuiEditBox* m_pCapacityBulletSmokeKey;
	i3GuiEditBox* m_pCapacityLightKey;
	i3GuiEditBox* m_pCapacityNormalKey;
	i3GuiEditBox* m_pCapacityTerrainKey;
	i3GuiEditBox* m_pCapacityDepthKey;
	i3GuiEditBox* m_pCapacityBloomKey;
	i3GuiEditBox* m_pCapacityPhysXKey;
	i3GuiEditBox* m_pCapacityShadowKey;
	i3GuiEditBox* m_pCapacityShadowCombo;
	i3GuiButton* m_pCapacityShadowButton;
	i3GuiPopupList* m_pCapacityShadowPopup;
	i3GuiEditBox* m_pCapacityTextureKey;
	i3GuiEditBox* m_pCapacityTextureCombo;
	i3GuiButton* m_pCapacityTextureButton;
	i3GuiPopupList* m_pCapacityTexturePopup;
	i3GuiEditBox* m_pCapacitySpecularKey;
	i3GuiEditBox* m_pCapacitySpecularCombo;
	i3GuiButton* m_pCapacitySpecularButton;
	i3GuiPopupList* m_pCapacitySpecularPopup;
	i3GuiEditBox* m_pCapacityEffectKey;
	i3GuiEditBox* m_pCapacityEffectCombo;
	i3GuiButton* m_pCapacityEffectButton;
	i3GuiPopupList* m_pCapacityEffectPopup;

	
	i3GuiEditBox* m_pCommunityNoteCombo;
	i3GuiEditBox* m_pCommunityInviteCombo;
	i3GuiEditBox* m_pCommunityWhisperCombo;
	i3GuiEditBox* m_pCommunityChatCombo;	
	i3GuiButton* m_pCommunityNoteButton;
	i3GuiButton* m_pCommunityInviteButton;
	i3GuiButton* m_pCommunityWhisperButton;
	i3GuiButton* m_pCommunityChatButton;	
	i3GuiPopupList* m_pCommunityNotePopup;
	i3GuiPopupList* m_pCommunityInvitePopup;
	i3GuiPopupList* m_pCommunityWhisperPopup;
	i3GuiPopupList* m_pCommunityChatPopup;	
	i3GuiEditBox* m_pCommunityNoteKey;
	i3GuiEditBox* m_pCommunityInviteKey;
	i3GuiEditBox* m_pCommunityWhisperKey;
	i3GuiEditBox* m_pCommunityChatKey;

	i3GuiEditBox* m_pMacroAllKey;
	i3GuiEditBox* m_pMacroTeamKey;
	i3GuiEditBox* m_pMacroKey1;
	i3GuiEditBox* m_pMacroKey2;
	i3GuiEditBox* m_pMacroKey3;
	i3GuiEditBox* m_pMacroKey4;
	i3GuiEditBox* m_pMacroKey5;
	i3GuiEditBox* m_pMacroValue1;
	i3GuiEditBox* m_pMacroValue2;
	i3GuiEditBox* m_pMacroValue3;
	i3GuiEditBox* m_pMacroValue4;
	i3GuiEditBox* m_pMacroValue5;
	i3GuiRadioButton* m_pMacroAll1;
	i3GuiRadioButton* m_pMacroAll2;
	i3GuiRadioButton* m_pMacroAll3;
	i3GuiRadioButton* m_pMacroAll4;
	i3GuiRadioButton* m_pMacroAll5;
	i3GuiRadioButton* m_pMacroTeam1;
	i3GuiRadioButton* m_pMacroTeam2;
	i3GuiRadioButton* m_pMacroTeam3;
	i3GuiRadioButton* m_pMacroTeam4;
	i3GuiRadioButton* m_pMacroTeam5;

	i3GuiEditBox* m_pControlKeyboardKey;
	i3GuiStatic* m_pControlSelect;
	i3GuiStatic* m_pControlFocus;
	i3GuiList* m_pControlList;
	INT32		m_nListKeyIndex[GAME_KEY_MAP_COUNT];		// 컨트롤의 인덱스에 해당하는 키맵
	KeymapSlot m_keymapSlot[MAX_KEYMAP_SLOT];	
	i3GuiCheckBox* m_pControlReverse;
	i3GuiEditBox* m_pControlReverseKey;
	i3GuiEditBox* m_pControlMouseKey;
	i3GuiButton* m_pControlMouseGagePrev;
	i3GuiButton* m_pControlMouseGageNext;
	i3GuiStatic* m_pControlMouseGage;
	i3GuiEditBox* m_pControlFovKey;
	i3GuiButton* m_pControlFovPrev;
	i3GuiButton* m_pControlFovNext;
	i3GuiStatic* m_pControlFovGage;
	i3GuiButton* m_pControlKeyButton;
	i3GuiButton* m_pControlKeyDelete;
	i3GuiEditBox* m_pControlMouseValue;
	i3GuiEditBox* m_pControlFovValue;

	i3GuiEditBox* m_pRatingNotice;
	i3GuiStatic* m_pRatingNoticeBG;

	OptionSectType m_currentSect;

	//
	BOOL							m_bEnableConfirm;
	BOOL							m_bOptionInit;

	// GamePlay 관련 변수들
	BOOL							m_bEnableBlood;
	BOOL							m_bEnableBlood2;
	INT32							m_nBloodType;
	INT32							m_nOldBloodType;
	INT32							m_nCrossHairType;
	INT32							m_nOldCrossHairType;
	INT32							m_nHandType;
	BOOL							m_bCrossHairEffect;	


	// Video관련 변수들
	BOOL							m_bFullScreen;
	INT32							m_nScreenWidth;		
	INT32							m_nScreenHeight;
	INT32							m_nScreenHz;
	INT32							m_ScreenDepth;
	INT32							m_AA;
	INT32							m_AAQ;
	BOOL							m_bVSync;
	BOOL							m_bTriLinearFilter;
	REAL32							m_GammaVal;
	

	// Interface관련 변수들
	BOOL							m_bEnableFrindlyName;
	BOOL							m_bEnableFrindlyMark;
	BOOL							m_bEnableTextEffect;
	BOOL							m_bEnableChallenge;	
	BOOL							m_bEnable1VS1;	
	BOOL							m_bMinimapType;

	// Sound관련 변수들
	BOOL							m_bEnableBGM;
	BOOL							m_bEnableEffect;
	BOOL							m_bEnableRadioMessage;
	INT32							m_nRadioMessageType;
	REAL32							m_fVolBGM;
	REAL32							m_fVolEffect;
	INT32							m_nSpeakerType;		

	// Control관련 변수들
	GAME_KEY_STRUCT					m_KeyMapArray[GAME_KEY_MAP_COUNT];
	BOOL							m_bEnableMouseInvert;	
	REAL32							m_rMouseSensivity;		
	REAL32							m_fFovValue;
	INT32							m_nCurKeyMapIdx;
	INT32							m_nMaxKeyMapIdx;
	INT32							m_nSelectedKeyMapIdx;
	i3InputKeyboard *				m_pKeyboard;
	i3InputMouse *					m_pMouse;
	INT32							m_nChangeKeyMapIdx;
	I3I_KEY							m_ChangeKey;
	UINT32							m_ChangeMouse;
	INT32							m_nDuplicateIdx;
	UIMessageBox *					m_pControlPopup;
	INT32							m_nSelectedIdx;
	BOOL							m_bChangeKeyMap;

	// Performance관련 변수들
	BOOL							m_bEnableBulletTrace;	
	BOOL							m_bEnableBulletSmoke;
	BOOL							m_bDynamicLight;		
	BOOL							m_bEnableNormalMap;
	BOOL							m_bEnableTerrainEffect;
	INT32							m_nTextureQualityType;	
	INT32							m_nShadowQualityType;
	BOOL							m_b16BitsDepth;
	INT32							m_nSpecularQualityType;
	BOOL							m_bEnableBloom;
	BOOL							m_bEnablePhysX;
	INT32							m_nEffectQuality;										// 이펙트품질

	// Community관련 변수들
	INT32							m_nChatType;
	INT32							m_nWhisperType;
	INT32							m_nInviteType;
	INT32							m_nFriendType;
	INT32							m_nMessageType;
	INT32							m_nClanType;

	// Macro
	char							m_szMacro[5][MACRO_MAX_LENGHT];								// 채팅용 매크로
	BOOL							m_bMacroEnableFriend[5];	

	// 버튼 딜레이
	REAL32							m_fElapsedTime;
	REAL32							m_fDelayLocalTime;

	//
	BOOL							m_bInGame;
};

#endif //_PopupOption_h
