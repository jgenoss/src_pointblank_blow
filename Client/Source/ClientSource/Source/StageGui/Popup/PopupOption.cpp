#include "pch.h"
#include "PopupOption.h"
#include "StageReady.h"
#include "BattleGui.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"
#include "GlobalVariables.h"

#include "UI/UIMainFrame.h"
#include "UI/UIUtil.h"

#include "CrosshairMgr.h"
#include "BattleHud.h"

/** \brief 임시 옵션창에 가려져 옆으로 옮깁니다. */
static void		RePositionMsgBox( UIMessageBox * pBox)
{
	if( pBox != NULL)
	{
		if( g_pFramework->IsBattleStage())
			pBox->SetPosition( pBox->GetPositionX() - 100, pBox->GetPositionY() + 200);
		else
			pBox->SetPosition( pBox->GetPositionX() - 100, pBox->GetPositionY());
	}
}

I3_CLASS_INSTANCE(CPopupOption, CPopupBase);

static INT32	_VidInfoCompareProc( VID_INFO * p1, VID_INFO * p2)
{
	if( p1->m_Width > p2->m_Width)
		return 1;
	else if( p1->m_Width < p2->m_Width)
		return -1;

	if( p1->m_Height > p2->m_Height)
		return 1;
	else if( p1->m_Height < p2->m_Height)
		return -1;

	if( p1->m_Hz > p2->m_Hz)
		return 1;
	else if( p1->m_Hz < p2->m_Hz)
		return -1;

	return 0;
}

CPopupOption::CPopupOption()
{
	UINT32 i;

	m_VidList.SetCompareProc( (COMPAREPROC) _VidInfoCompareProc);

	m_pOptionGui = NULL;
	m_pOptionWin = NULL;
	m_pOptionCaption = NULL;

	for( i = 0; i < MAX_OS; i++)
	{
		m_sect[i]._pButton = NULL;
		m_sect[i]._pSect = NULL;
	}
	m_pInit = NULL;
	m_pOptimize = NULL;
	m_pCofirm = NULL;
	m_pCancel = NULL;

	m_pFrameCaption = NULL;
	m_pVideoResolutionKey = NULL;
	m_pVideoResolutionCombo = NULL;
	m_pVideoResolutionButton = NULL;
	m_pVideoResolutionPopup = NULL;
	m_pVideoColorKey = NULL;
	m_pVideoColorCombo = NULL;
	m_pVideoColorButton = NULL;
	m_pVideoColorPopup = NULL;
	m_pVideoScanRateKey = NULL;
	m_pVideoScanRateCombo = NULL;
	m_pVideoScanRateButton = NULL;
	m_pVideoScanRatePopup = NULL;
	m_pVideoAntiAliasingKey = NULL;
	m_pVideoAntiAliasingCombo = NULL;
	m_pVideoAntiAliasingButton = NULL;
	m_pVideoAntiAliasingPopup = NULL;
	m_pVideoCaptureKey = NULL;
	m_pVideoCaptureCombo = NULL;
	m_pVideoCaptureButton = NULL;
	m_pVideoCaptureTooltip = NULL;
	m_pVideoCapturePopup = NULL;
	m_pVideoWindowMode = NULL;
	m_pVideoScreenSync = NULL;
	m_pVideoSmoothDistance = NULL;
	m_pVideoWindowModeKey = NULL;
	m_pVideoScreenSyncKey = NULL;
	m_pVideoSmoothDistanceKey = NULL;
	m_pVideoGammaKey = NULL;
	m_pVideoGammaPrev = NULL;
	m_pVideoGammaNext = NULL;
	m_pVideoGammaGage = NULL;
	m_pVideoGammaNumber = NULL;
	m_pArrowVideoAntiAliasing = NULL;
	m_pArrowVideoColor = NULL;
	m_pArrowVideoScanRate = NULL;
	m_pArrowVideoResolution = NULL;
	m_pArrowVideoCapture = NULL;


	m_pPlayBlood2Key = NULL;
	m_pPlayBloodRadio1 = NULL;
	m_pPlayBloodRadio2 = NULL;
	m_pPlayBlood = NULL;
	m_pPlayHit = NULL;
	m_pPlayBloodKey = NULL;
	m_pPlayHitKey = NULL;
	m_pPlayCrosshairKey = NULL;
	m_pPlayCrosshairType1 = NULL;
	m_pPlayCrosshairType2 = NULL;
	m_pPlayCrosshairType3 = NULL;
	m_pPlayCrosshairType4 = NULL;
	m_pPlayCrosshair1 = NULL;
	m_pPlayCrosshair2 = NULL;
	m_pPlayCrosshair3 = NULL;
	m_pPlayCrosshair4 = NULL;

	m_pInterfaceNicknameKey = NULL;
	m_pInterfaceMarkKey = NULL;
	m_pInterfaceEffectKey = NULL;
	m_pInterfaceChallengeKey = NULL;
	m_pInterface1VS1Key = NULL;
	m_pInterfaceNickname = NULL;
	m_pInterfaceMark = NULL;
	m_pInterfaceEffect = NULL;
	m_pInterfaceChallenge = NULL;
	m_pInterface1VS1 = NULL;
	m_pInterfaceRadarKey = NULL;
	m_pInterfaceRadarCombo = NULL;
	m_pInterfaceRadarButton = NULL;
	m_pInterfaceRadarPopup = NULL;

	m_pSoundBgm = NULL;
	m_pSoundBgmKey = NULL;
	m_pSoundBgmValue = NULL;
	m_pSoundBgmPrev = NULL;
	m_pSoundBgmNext = NULL;
	m_pSoundBgmGage = NULL;
	m_pSoundEffect = NULL;
	m_pSoundEffectKey = NULL;
	m_pSoundEffectValue = NULL;
	m_pSoundEffectPrev = NULL;
	m_pSoundEffectNext = NULL;
	m_pSoundEffectGage = NULL;
	m_pSoundSpeakerKey = NULL;
	m_pSoundSpeakerCombo = NULL;
	m_pSoundSpeakerButton = NULL;
	m_pSoundSpeakerPopup = NULL;
	m_pSoundVoice = NULL;
	m_pSoundVoiceKey = NULL;
	m_pSoundVoiceCombo = NULL;
	m_pSoundVoiceButton = NULL;
	m_pSoundVoicePopup = NULL;
	m_pCapacityBullet = NULL;
	m_pCapacityBulletSmoke = NULL;
	m_pCapacityLight = NULL;
	m_pCapacityNormal = NULL;
	m_pCapacityTerrain = NULL;
	m_pCapacityDepth = NULL;
	m_pCapacityBloom = NULL;
	m_pCapacityPhysX = NULL;
	m_pCapacityBulletKey = NULL;
	m_pCapacityBulletSmokeKey = NULL;
	m_pCapacityLightKey = NULL;
	m_pCapacityNormalKey = NULL;
	m_pCapacityTerrainKey = NULL;
	m_pCapacityDepthKey = NULL;
	m_pCapacityBloomKey = NULL;
	m_pCapacityShadowKey = NULL;
	m_pCapacityShadowCombo = NULL;
	m_pCapacityShadowButton = NULL;
	m_pCapacityShadowPopup = NULL;
	m_pCapacityTextureKey = NULL;
	m_pCapacityTextureCombo = NULL;
	m_pCapacityTextureButton = NULL;
	m_pCapacityTexturePopup = NULL;
	m_pCapacitySpecularKey = NULL;
	m_pCapacitySpecularCombo = NULL;
	m_pCapacitySpecularButton = NULL;
	m_pCapacitySpecularPopup = NULL;
	m_pCapacityEffectKey = NULL;
	m_pCapacityEffectCombo = NULL;
	m_pCapacityEffectButton = NULL;
	m_pCapacityEffectPopup = NULL;
	m_pCommunityNoteCombo = NULL;
	m_pCommunityInviteCombo = NULL;
	m_pCommunityWhisperCombo = NULL;
	m_pCommunityChatCombo = NULL;
	m_pCommunityNoteButton = NULL;
	m_pCommunityInviteButton = NULL;
	m_pCommunityWhisperButton = NULL;
	m_pCommunityChatButton = NULL;
	m_pCommunityNotePopup = NULL;
	m_pCommunityInvitePopup = NULL;
	m_pCommunityWhisperPopup = NULL;
	m_pCommunityChatPopup = NULL;
	m_pCommunityNoteKey = NULL;
	m_pCommunityInviteKey = NULL;
	m_pCommunityWhisperKey = NULL;
	m_pCommunityChatKey = NULL;
	m_pMacroAllKey = NULL;
	m_pMacroTeamKey = NULL;
	m_pMacroKey1 = NULL;
	m_pMacroKey2 = NULL;
	m_pMacroKey3 = NULL;
	m_pMacroKey4 = NULL;
	m_pMacroKey5 = NULL;
	m_pMacroValue1 = NULL;
	m_pMacroValue2 = NULL;
	m_pMacroValue3 = NULL;
	m_pMacroValue4 = NULL;
	m_pMacroValue5 = NULL;
	m_pMacroAll1 = NULL;
	m_pMacroAll2 = NULL;
	m_pMacroAll3 = NULL;
	m_pMacroAll4 = NULL;
	m_pMacroAll5 = NULL;
	m_pMacroTeam1 = NULL;
	m_pMacroTeam2 = NULL;
	m_pMacroTeam3 = NULL;
	m_pMacroTeam4 = NULL;
	m_pMacroTeam5 = NULL;
	m_pControlKeyboardKey = NULL;
	m_pControlSelect = NULL;
	m_pControlFocus = NULL;
	m_pControlList = NULL;

	for( i = 0; i < MAX_KEYMAP_SLOT; i++)
	{
		m_keymapSlot[i]._pKey = NULL;
		m_keymapSlot[i]._pValue = NULL;
	}

	m_pControlReverse = NULL;
	m_pControlReverseKey = NULL;
	m_pControlMouseKey = NULL;
	m_pControlMouseGagePrev = NULL;
	m_pControlMouseGageNext = NULL;
	m_pControlMouseGage = NULL;
	m_pControlFovKey = NULL;
	m_pControlFovPrev = NULL;
	m_pControlFovNext = NULL;
	m_pControlFovGage = NULL;
	m_pControlKeyButton = NULL;
	m_pControlKeyDelete = NULL;
	m_pControlMouseValue = NULL;
	m_pControlFovValue = NULL;

	m_pRatingNotice = NULL;
	m_pRatingNoticeBG = NULL;

	m_bEnableConfirm = FALSE;
	m_bOptionInit = FALSE;

	// GamePlay 관련 변수들
	m_bEnableBlood = TRUE;
	m_bEnableBlood2 = TRUE;
	m_nBloodType = 0;
	m_nOldBloodType = 0;
	m_nCrossHairType = 0;
	m_nHandType = 0;
	m_bCrossHairEffect = FALSE;

	// Video관련 변수들
	m_bFullScreen = TRUE;
	m_nScreenWidth = 1024;
	m_nScreenHeight = 768;
	m_nScreenHz = 60;
	m_ScreenDepth = 32;
	m_AA = 0;
	m_AAQ = 0;
	m_bVSync = FALSE;
	m_bTriLinearFilter = FALSE;
	m_GammaVal = 50.0f;
	m_nVideoResolution = 1;		// 동영상 해상도( Low = 0 , Default = 1 , High = 2)
	
	// Interface관련 변수들
	m_bEnableFrindlyName = TRUE;
	m_bEnableFrindlyMark = TRUE;
	m_bEnableTextEffect = TRUE;
	m_bEnableChallenge = FALSE;
	m_bEnable1VS1 = FALSE;
	m_bMinimapType = TRUE;

	// Sound관련 변수들
	m_bEnableBGM = TRUE;
	m_bEnableEffect = TRUE;
	m_bEnableRadioMessage = TRUE;
	m_nRadioMessageType = 0;
	m_fVolBGM = 60.0f;
	m_fVolEffect = 100.0f;
	m_nSpeakerType = 0;

	// Control관련 변수들
	m_bEnableMouseInvert = FALSE;
	m_rMouseSensivity = 0.5f;
	m_fFovValue = DEF_CAM_FOV;
	m_nCurKeyMapIdx = 0;
	m_nMaxKeyMapIdx = 0;
	m_nSelectedKeyMapIdx = 0;
	m_nChangeKeyMapIdx = 0;
	m_ChangeKey = I3I_KEY_UNKNOWN;
	m_pKeyboard = NULL;
	m_pMouse = NULL;
	m_ChangeMouse = 0;
	m_nDuplicateIdx = -1;
	m_pControlPopup = NULL;
	m_nSelectedIdx = -1;

	// Performance관련 변수들
	m_bEnableBulletTrace = TRUE;
	m_bEnableBulletSmoke = TRUE;
	m_bDynamicLight = FALSE;
	m_bEnableNormalMap = TRUE;
	m_bEnableTerrainEffect = TRUE;
	m_nTextureQualityType = 0;
	m_nShadowQualityType = 0;
	m_b16BitsDepth = FALSE;
	m_nSpecularQualityType = 0;
	m_bEnableBloom = FALSE;
	m_bEnablePhysX = TRUE;
	m_nEffectQuality = 2;

	// Community관련 변수들
	m_nChatType = 0;
	m_nWhisperType = 0;
	m_nInviteType = COMMUNITY_PERMISSION_ALL;
	m_nFriendType = 0;
	m_nMessageType = 0;
	m_nClanType = 0;

	// Macro
	for( i=0; i<5; i++)
	{
		i3mem::FillZero( m_szMacro[i], sizeof(char)*MACRO_MAX_LENGHT);
		m_bMacroEnableFriend[i] = TRUE;
	}

	i3mem::FillZero( m_KeyMapArray, sizeof(GAME_KEY_STRUCT)*GAME_KEY_MAP_COUNT);

	m_bNeedToValidate = false;

	m_fElapsedTime = 0.0f;
	m_fDelayLocalTime = 0.0f;

	m_bInGame = FALSE;

	m_bOldFullScreen = g_pEnvSet->m_bFullScreen;
}

CPopupOption::~CPopupOption()
{
	_removeAllAAInfo( true);

	_removeAllVidInfo();
}

void CPopupOption::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_OPP_B_PLAY:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnPlay();
		break;
	case GCI_OPP_B_VIDEO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideo();
		break;
	case GCI_OPP_B_INTERFACE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnInterface();
		break;
	case GCI_OPP_B_SOUND:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSound();
		break;
	case GCI_OPP_B_CONTROL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnControl();
		break;
	case GCI_OPP_B_CAPACITY:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCapacity();
		break;
	case GCI_OPP_B_COMMUNITY:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunity();
		break;
	case GCI_OPP_B_MACRO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnMacro();
		break;
	case GCI_OPP_B_VOICE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnVoice();
		break;
	case GCI_OPP_B_RATING:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnRating();
		break;
	case GCI_OPP_B_INIT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnOptionInitial();
		break;
	case GCI_OPP_B_OPTIMIZE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnOptionOptimize();
		break;
	case GCI_OPP_B_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnOptionConfirm();
		break;
	case GCI_OPP_B_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnOptionCancel();
		break;
	case GCI_OPP_B_VIDEO_RESOLUTION:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoResolutionCombo();
		break;
	case GCI_OPP_L_VIDEO_RESOLUTION:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoResolutionPopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_B_VIDEO_COLOR:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoColorCombo();
		break;
	case GCI_OPP_L_VIDEO_COLOR:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoColorPopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_B_VIDEO_SCAN_RATE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoScanRateCombo();
		break;
	case GCI_OPP_L_VIDEO_SCAN_RATE:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoScanRatePopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_B_VIDEO_ANTI_ALIASING:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoAntiAliasingCombo();
		break;
	case GCI_OPP_L_VIDEO_ANTI_ALIASING:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoAntiAliasingPopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_B_VIDEO_CAPTURE:
		if (I3GUI_BUTTON_NOTIFY_ONMOUSE == pNotify->m_nEvent) SetTooltip(TIP_VIDEO_CAPTURE);
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoCaptureCombo();
		break;
	case GCI_OPP_L_VIDEO_CAPTURE:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoCapturePopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_C_VIDEO_WINDOW_MODE:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoWindowMode();
		break;
	case GCI_OPP_C_VIDEO_SCREEN_SYNC:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoScreenSync();
		break;
	case GCI_OPP_C_VIDEO_SMOOTH_DISTANCE:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoSmoothDistance();
		break;
	case GCI_OPP_B_VIDEO_GAMMA_PREV:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoGammaPrev();
		break;
	case GCI_OPP_B_VIDEO_GAMMA_NEXT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnVideoGammaNext();
		break;
	case GCI_OPP_C_VIDEO_CAPTURE :
		if (I3GUI_BUTTON_NOTIFY_ONMOUSE == pNotify->m_nEvent) SetTooltip(TIP_VIDEO_CAPTURE);
		break;
	case GCI_OPP_R_PLAY_BLOOD1:
	case GCI_OPP_R_PLAY_BLOOD2:
		if (I3GUI_RADIO_NOTIFY_CLICKED == pNotify->m_nEvent) OnPlayBloodType(pNotify->m_nID - GCI_OPP_R_PLAY_BLOOD1);
		break;
	case GCI_OPP_C_PLAY_BLOOD:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnPlayBlood();
		break;
	case GCI_OPP_C_PLAY_HIT:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnPlayHit();
		break;
	case GCI_OPP_B_PLAY_HANDTYPE_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnPlayHandTypeCombo();
		break;
	case GCI_OPP_B_PLAY_HANDTYPE_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnPlayHandTypePopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_R_PLAY_CROSSHAIR_1:
	case GCI_OPP_R_PLAY_CROSSHAIR_2:
	case GCI_OPP_R_PLAY_CROSSHAIR_3:
	case GCI_OPP_R_PLAY_CROSSHAIR_4:
		if (I3GUI_RADIO_NOTIFY_CLICKED == pNotify->m_nEvent) OnPlayCorsshair(pNotify->m_nID - GCI_OPP_R_PLAY_CROSSHAIR_1);
		break;
	case GCI_OPP_C_INTERFACE_NICKNAME:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnInterfaceNickname();
		break;
	case GCI_OPP_C_INTERFACE_MARK:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnInterfaceMark();
		break;
	case GCI_OPP_C_INTERFACE_EFFECT:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnInterfaceEffect();
		break;
	case GCI_OPP_C_INTERFACE_CHALLENGE:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnInterfaceChallenge();
		break;
	case GCI_OPP_C_INTERFACE_1VS1:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnInterface1VS1();
		break;
	case GCI_OPP_B_INTERFACE_RADAR:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnInterfaceRadarButton();
		break;
	case GCI_OPP_P_INTERFACE_RADAR:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnInterfaceRadarPopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_C_SOUND_BGM:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnSoundBgm();
		break;
	case GCI_OPP_B_SOUND_BGM_PREV:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSoundBgmPrev();
		break;
	case GCI_OPP_B_SOUND_BGM_NEXT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSoundBgmNext();
		break;
	case GCI_OPP_C_SOUND_EFFECT:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnSoundEffect();
		break;
	case GCI_OPP_B_SOUND_EFFECT_PREV:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSoundEffectPrev();
		break;
	case GCI_OPP_B_SOUND_EFFECT_NEXT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSoundEffectNext();
		break;
	case GCI_OPP_B_SOUND_SPEAKER_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSoundSpeakerCombo();
		break;
	case GCI_OPP_B_SOUND_SPEAKER_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnSoundSpeakerPopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_C_SOUND_VOICE:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnSoundVoice();
		break;
	case GCI_OPP_C_SOUND_VOICE_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSoundVoiceCombo();
		break;
	case GCI_OPP_C_SOUND_VOICE_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnSoundVoicePopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_C_CAPACITY_BULLET:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnCapacityBullet();
		break;
	case GCI_OPP_C_CAPACITY_BULLETSMOKE:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnCapacityBulletSmoke();
		break;
	case GCI_OPP_C_CAPACITY_LIGHT:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnCapacityLight();
		break;
	case GCI_OPP_C_CAPACITY_NORMAL:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnCapacityNormal();
		break;
	case GCI_OPP_C_CAPACITY_TERRAIN:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnCapacityTerrain();
		break;
	case GCI_OPP_C_CAPACITY_DEPTH:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnCapacityDepth();
		break;
	case GCI_OPP_C_CAPACITY_BLOOM:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnCapacityBloom();
		break;

	case GCI_OPP_C_CAPACITY_PHYSX:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnCapacityPhysX();
		break;

	case GCI_OPP_B_CAPACITY_SHADOW_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnShadowCombo();
		break;
	case GCI_OPP_L_CAPACITY_SHADOW_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnShadowPopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_B_CAPACITY_TEXTURE_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnTextureCombo();
		break;
	case GCI_OPP_L_CAPACITY_TEXTURE_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnTexturePopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_B_CAPACITY_SPECULAR_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSpecularCombo();
		break;
	case GCI_OPP_L_CAPACITY_SPECULAR_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnSpecularPopup(pNotify->m_nPriParam);
		break;

	case GCI_OPP_B_CAPACITY_EFFECT_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnEffectCombo();
		break;
	case GCI_OPP_L_CAPACITY_EFFECT_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnEffectPopup(pNotify->m_nPriParam);
		break;

	case GCI_OPP_B_COMMUNITY_NOTE_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityNoteComb();
		break;
	case GCI_OPP_B_COMMUNITY_INVITE_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityInviteComb();
		break;
	case GCI_OPP_B_COMMUNITY_WHISPER_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityWhisperComb();
		break;
	case GCI_OPP_B_COMMUNITY_CHAT_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityChatComb();
		break;
	case GCI_OPP_L_COMMUNITY_NOTE_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityNotePopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_L_COMMUNITY_INVITE_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityInvitePopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_L_COMMUNITY_WHISPER_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityWhisperPopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_L_COMMUNITY_CHAT_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityChatPopup(pNotify->m_nPriParam);
		break;
	case GCI_OPP_R_MACRO_ALL_1:
	case GCI_OPP_R_MACRO_TEAM_1:
		if (I3GUI_RADIO_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityMacro1(pNotify->m_nID - GCI_OPP_R_MACRO_ALL_1);
		break;
	case GCI_OPP_R_MACRO_ALL_2:
	case GCI_OPP_R_MACRO_TEAM_2:
		if (I3GUI_RADIO_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityMacro2(pNotify->m_nID - GCI_OPP_R_MACRO_ALL_2);
		break;
	case GCI_OPP_R_MACRO_ALL_3:
	case GCI_OPP_R_MACRO_TEAM_3:
		if (I3GUI_RADIO_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityMacro3(pNotify->m_nID - GCI_OPP_R_MACRO_ALL_3);
		break;
	case GCI_OPP_R_MACRO_ALL_4:
	case GCI_OPP_R_MACRO_TEAM_4:
		if (I3GUI_RADIO_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityMacro4(pNotify->m_nID - GCI_OPP_R_MACRO_ALL_4);
		break;
	case GCI_OPP_R_MACRO_ALL_5:
	case GCI_OPP_R_MACRO_TEAM_5:
		if (I3GUI_RADIO_NOTIFY_CLICKED == pNotify->m_nEvent) OnCommunityMacro5(pNotify->m_nID - GCI_OPP_R_MACRO_ALL_5);
		break;
	case GCI_OPP_L_CONTROL_LIST:
		if (I3GUI_LIST_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnControlListFocus(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnControlListSelect(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_DOUBLECLICK == pNotify->m_nEvent) OnControlListChange(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnControlListDrag();
		break;
	case GCI_OPP_C_CONTROL_REVERSE:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnControlReverse();
		break;
	case GCI_OPP_B_CONTROL_MOUSE_PREV:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnControlMousePrev();
		break;
	case GCI_OPP_B_CONTROL_MOUSE_NEXT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnControlMouseNext();
		break;
	case GCI_OPP_B_CONTROL_FOV_PREV:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnControlFovPrev();
		break;
	case GCI_OPP_B_CONTROL_FOV_NEXT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnControlFovNext();
		break;
	case GCI_OPP_B_CONTROL_BUTTON:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnControlButton();
		break;
	case GCI_OPP_B_CONTROL_DELETE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnControlDelete();
		break;
	}
}

BOOL CPopupOption::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist, "Option", m_pOptionGui, -1);
	LinkControlEx(plist, "opOptionWin", m_pOptionWin, -1);
	LinkControlEx(plist, "opOptionCaption", m_pOptionCaption, -1);
	LinkControlEx(plist, "opPlay", m_sect[OS_PLAY]._pButton, GCI_OPP_B_PLAY);
	LinkControlEx(plist, "opVideo", m_sect[OS_VIDEO]._pButton, GCI_OPP_B_VIDEO);
	LinkControlEx(plist, "opInterface", m_sect[OS_INTERFACE]._pButton, GCI_OPP_B_INTERFACE);
	LinkControlEx(plist, "opSound", m_sect[OS_SOUND]._pButton, GCI_OPP_B_SOUND);
	LinkControlEx(plist, "opControl", m_sect[OS_CONTROL]._pButton, GCI_OPP_B_CONTROL);
	LinkControlEx(plist, "opCapacity", m_sect[OS_CAPACITY]._pButton, GCI_OPP_B_CAPACITY);
	LinkControlEx(plist, "opCommunity", m_sect[OS_COMMUNITY]._pButton, GCI_OPP_B_COMMUNITY);
	LinkControlEx(plist, "opMacro", m_sect[OS_MACRO]._pButton, GCI_OPP_B_MACRO);
	LinkControlEx(plist, "opVoice", m_sect[OS_VOICE]._pButton, GCI_OPP_B_VOICE);
	LinkControlEx(plist, "opRating", m_sect[OS_RATING]._pButton, GCI_OPP_B_RATING);
	LinkControlEx(plist, "opInit", m_pInit, GCI_OPP_B_INIT);
	LinkControlEx(plist, "opOptimize", m_pOptimize, GCI_OPP_B_OPTIMIZE);
	LinkControlEx(plist, "opConfirm", m_pCofirm, GCI_OPP_B_CONFIRM);
	LinkControlEx(plist, "opCancel", m_pCancel, GCI_OPP_B_CANCEL);
	LinkControlEx(plist, "opPlaySect", m_sect[OS_PLAY]._pSect, -1);
	LinkControlEx(plist, "opVideoSect", m_sect[OS_VIDEO]._pSect, -1);
	LinkControlEx(plist, "opInterfaceSect", m_sect[OS_INTERFACE]._pSect, -1);
	LinkControlEx(plist, "opSoundSect", m_sect[OS_SOUND]._pSect, -1);
	LinkControlEx(plist, "opControlSect", m_sect[OS_CONTROL]._pSect, -1);
	LinkControlEx(plist, "opCapacitySect", m_sect[OS_CAPACITY]._pSect, -1);
	LinkControlEx(plist, "opCommunitySect", m_sect[OS_COMMUNITY]._pSect, -1);
	LinkControlEx(plist, "opMacroSect", m_sect[OS_MACRO]._pSect, -1);
	LinkControlEx(plist, "opVoiceSect", m_sect[OS_VOICE]._pSect, -1);
	LinkControlEx(plist, "opRatingSect", m_sect[OS_RATING]._pSect, -1);


	LinkControlEx(plist, "opFrameCaption", m_pFrameCaption, -1);
	LinkControlEx(plist, "opVideoResolutionKey", m_pVideoResolutionKey, -1);
	LinkControlEx(plist, "opVideoResolutionCombo", m_pVideoResolutionCombo, -1);
	LinkControlEx(plist, "opVideoResolutionButton", m_pVideoResolutionButton, GCI_OPP_B_VIDEO_RESOLUTION);
	LinkControlEx(plist, "opVideoResolutionPopup", m_pVideoResolutionPopup, GCI_OPP_L_VIDEO_RESOLUTION);
	LinkControlEx(plist, "opVideoColorKey", m_pVideoColorKey, -1);
	LinkControlEx(plist, "opVideoColorCombo", m_pVideoColorCombo, -1);
	LinkControlEx(plist, "opVideoColorButton", m_pVideoColorButton, GCI_OPP_B_VIDEO_COLOR);
	LinkControlEx(plist, "opVideoColorPopup", m_pVideoColorPopup, GCI_OPP_L_VIDEO_COLOR);
	LinkControlEx(plist, "opVideoScanRateKey", m_pVideoScanRateKey, -1);
	LinkControlEx(plist, "opVideoScanRateCombo", m_pVideoScanRateCombo, -1);
	LinkControlEx(plist, "opVideoScanRateButton", m_pVideoScanRateButton, GCI_OPP_B_VIDEO_SCAN_RATE);
	LinkControlEx(plist, "opVideoScanRatePopup", m_pVideoScanRatePopup, GCI_OPP_L_VIDEO_SCAN_RATE);
	LinkControlEx(plist, "opVideoAntiAliasingKey", m_pVideoAntiAliasingKey, -1);
	LinkControlEx(plist, "opVideoAntiAliasingCombo", m_pVideoAntiAliasingCombo, -1);
	LinkControlEx(plist, "opVideoAntiAliasingButton", m_pVideoAntiAliasingButton, GCI_OPP_B_VIDEO_ANTI_ALIASING);	
	LinkControlEx(plist, "opVideoAntiAliasingPopup", m_pVideoAntiAliasingPopup, GCI_OPP_L_VIDEO_ANTI_ALIASING);
	
	LinkControlEx(plist, "opVideoCaptureKey", m_pVideoCaptureKey, -1);
	LinkControlEx(plist, "opVideoCaptureCombo", m_pVideoCaptureCombo, -1);
	LinkControlEx(plist, "opVideoCaptureButton", m_pVideoCaptureButton, GCI_OPP_B_VIDEO_CAPTURE);
	LinkControlEx(plist, "opVideoCaptureTooltip", m_pVideoCaptureTooltip, GCI_OPP_C_VIDEO_CAPTURE);
	LinkControlEx(plist, "opVideoCapturePopup", m_pVideoCapturePopup, GCI_OPP_L_VIDEO_CAPTURE);
	
	LinkControlEx(plist, "opVideoWindowMode", m_pVideoWindowMode, GCI_OPP_C_VIDEO_WINDOW_MODE);
	LinkControlEx(plist, "opVideoScreenSync", m_pVideoScreenSync, GCI_OPP_C_VIDEO_SCREEN_SYNC);
	LinkControlEx(plist, "opVideoSmoothDistance", m_pVideoSmoothDistance, GCI_OPP_C_VIDEO_SMOOTH_DISTANCE);
	LinkControlEx(plist, "opVideoWindowModeKey", m_pVideoWindowModeKey, -1);
	LinkControlEx(plist, "opVideoScreenSyncKey", m_pVideoScreenSyncKey, -1);
	LinkControlEx(plist, "opVideoSmoothDistanceKey", m_pVideoSmoothDistanceKey, -1);
	LinkControlEx(plist, "opVideoGammaKey", m_pVideoGammaKey, -1);
	LinkControlEx(plist, "opVideoGammaPrev", m_pVideoGammaPrev, GCI_OPP_B_VIDEO_GAMMA_PREV);
	LinkControlEx(plist, "opVideoGammaNext", m_pVideoGammaNext, GCI_OPP_B_VIDEO_GAMMA_NEXT);
	LinkControlEx(plist, "opVideoGammaGage", m_pVideoGammaGage, -1);
	LinkControlEx(plist, "opVideoGammaNumber", m_pVideoGammaNumber, -1);
	LinkControlEx(plist, "opVideoAntiAliasing", m_pArrowVideoAntiAliasing, -1);
	LinkControlEx(plist, "opVideoColor", m_pArrowVideoColor, -1);
	LinkControlEx(plist, "opVideoScanRate", m_pArrowVideoScanRate, -1);	
	LinkControlEx(plist, "opVideoResolution", m_pArrowVideoResolution, -1);

	LinkControlEx(plist, "opVideoCapture", m_pArrowVideoCapture, -1);

	LinkControlEx(plist, "opPlayBlood2Key", m_pPlayBlood2Key, -1);
	LinkControlEx(plist, "opPlayBloodRadio1", m_pPlayBloodRadio1, GCI_OPP_R_PLAY_BLOOD1);
	LinkControlEx(plist, "opPlayBloodRadio2", m_pPlayBloodRadio2, GCI_OPP_R_PLAY_BLOOD2);

	LinkControlEx(plist, "opPlayBlood", m_pPlayBlood, GCI_OPP_C_PLAY_BLOOD);
	LinkControlEx(plist, "opPlayHit", m_pPlayHit, GCI_OPP_C_PLAY_HIT);
	LinkControlEx(plist, "opPlayHandType", m_pPlayHandType, -1);
	LinkControlEx(plist, "opPlayBloodKey", m_pPlayBloodKey, -1);
	LinkControlEx(plist, "opPlayHitKey", m_pPlayHitKey, -1);
	LinkControlEx(plist, "opPlayCrosshairKey", m_pPlayCrosshairKey, -1);
	LinkControlEx(plist, "opPlayHandTypeKey", m_pPlayHandTypeKey, -1);
	LinkControlEx(plist, "opPlayHandTypeCombo", m_pPlayHandTypeCombo, -1);
	LinkControlEx(plist, "opPlayHandTypeButton", m_pPlayHandTypeButton, GCI_OPP_B_PLAY_HANDTYPE_COMBO);
	LinkControlEx(plist, "opPlayHandTypePopup", m_pPlayHandTypePopup, GCI_OPP_B_PLAY_HANDTYPE_POPUP);
	LinkControlEx(plist, "opPlayCrosshairType1", m_pPlayCrosshairType1, -1);
	LinkControlEx(plist, "opPlayCrosshairType2", m_pPlayCrosshairType2, -1);
	LinkControlEx(plist, "opPlayCrosshairType3", m_pPlayCrosshairType3, -1);
	LinkControlEx(plist, "opPlayCrosshairType4", m_pPlayCrosshairType4, -1);

	// 주의 - 저장 데이터와 화면 출력순서가 다르다
	LinkControlEx(plist, "opPlayCrosshairRadio2", m_pPlayCrosshair1, GCI_OPP_R_PLAY_CROSSHAIR_1);
	LinkControlEx(plist, "opPlayCrosshairRadio1", m_pPlayCrosshair2, GCI_OPP_R_PLAY_CROSSHAIR_2);
	LinkControlEx(plist, "opPlayCrosshairRadio4", m_pPlayCrosshair3, GCI_OPP_R_PLAY_CROSSHAIR_3);
	LinkControlEx(plist, "opPlayCrosshairRadio3", m_pPlayCrosshair4, GCI_OPP_R_PLAY_CROSSHAIR_4);

	LinkControlEx(plist, "opInterfaceNicknameKey", m_pInterfaceNicknameKey, -1);
	LinkControlEx(plist, "opInterfaceMarkKey", m_pInterfaceMarkKey, -1);
	LinkControlEx(plist, "opInterfaceEffectKey", m_pInterfaceEffectKey, -1);
	LinkControlEx(plist, "opInterfaceChallengeKey", m_pInterfaceChallengeKey, -1);
	LinkControlEx(plist, "opInterface1VS1Key", m_pInterface1VS1Key, -1);
	LinkControlEx(plist, "opInterfaceNickname", m_pInterfaceNickname, GCI_OPP_C_INTERFACE_NICKNAME);
	LinkControlEx(plist, "opInterfaceMark", m_pInterfaceMark, GCI_OPP_C_INTERFACE_MARK);
	LinkControlEx(plist, "opInterfaceEffect", m_pInterfaceEffect, GCI_OPP_C_INTERFACE_EFFECT);
	LinkControlEx(plist, "opInterfaceChallenge", m_pInterfaceChallenge, GCI_OPP_C_INTERFACE_CHALLENGE);
	LinkControlEx(plist, "opInterface1VS1", m_pInterface1VS1, GCI_OPP_C_INTERFACE_1VS1);
	LinkControlEx(plist, "opInterfaceRadarKey", m_pInterfaceRadarKey, -1);
	LinkControlEx(plist, "opInterfaceRadarCombo", m_pInterfaceRadarCombo, -1);
	LinkControlEx(plist, "opInterfaceRadarButton", m_pInterfaceRadarButton, GCI_OPP_B_INTERFACE_RADAR);
	LinkControlEx(plist, "opInterfaceRadarPopup", m_pInterfaceRadarPopup, GCI_OPP_P_INTERFACE_RADAR);

	LinkControlEx(plist, "opSoundBgm", m_pSoundBgm, GCI_OPP_C_SOUND_BGM);
	LinkControlEx(plist, "opSoundBgmKey", m_pSoundBgmKey, -1);
	LinkControlEx(plist, "opSoundBgmValue", m_pSoundBgmValue, -1);
	LinkControlEx(plist, "opSoundBgmPrev", m_pSoundBgmPrev, GCI_OPP_B_SOUND_BGM_PREV);
	LinkControlEx(plist, "opSoundBgmNext", m_pSoundBgmNext, GCI_OPP_B_SOUND_BGM_NEXT);
	LinkControlEx(plist, "opSoundBgmGage", m_pSoundBgmGage, -1);
	LinkControlEx(plist, "opSoundEffect", m_pSoundEffect, GCI_OPP_C_SOUND_EFFECT);
	LinkControlEx(plist, "opSoundEffectKey", m_pSoundEffectKey, -1);
	LinkControlEx(plist, "opSoundEffectValue", m_pSoundEffectValue, -1);
	LinkControlEx(plist, "opSoundEffectPrev", m_pSoundEffectPrev, GCI_OPP_B_SOUND_EFFECT_PREV);
	LinkControlEx(plist, "opSoundEffectNext", m_pSoundEffectNext, GCI_OPP_B_SOUND_EFFECT_NEXT);
	LinkControlEx(plist, "opSoundEffectGage", m_pSoundEffectGage, -1);
	LinkControlEx(plist, "opSoundSpeakerKey", m_pSoundSpeakerKey, -1);
	LinkControlEx(plist, "opSoundSpeakerCombo", m_pSoundSpeakerCombo, -1);
	LinkControlEx(plist, "opSoundSpeakerButton", m_pSoundSpeakerButton, GCI_OPP_B_SOUND_SPEAKER_COMBO);
	LinkControlEx(plist, "opSoundSpeakerPopup", m_pSoundSpeakerPopup, GCI_OPP_B_SOUND_SPEAKER_POPUP);
	LinkControlEx(plist, "opSoundVoice", m_pSoundVoice, GCI_OPP_C_SOUND_VOICE);
	LinkControlEx(plist, "opSoundVoiceKey", m_pSoundVoiceKey, -1);
	LinkControlEx(plist, "opSoundVoiceCombo", m_pSoundVoiceCombo, -1);
	LinkControlEx(plist, "opSoundVoiceButton", m_pSoundVoiceButton, GCI_OPP_C_SOUND_VOICE_COMBO);
	LinkControlEx(plist, "opSoundVoicePopup", m_pSoundVoicePopup, GCI_OPP_C_SOUND_VOICE_POPUP);

	LinkControlEx(plist, "opCapacityBullet", m_pCapacityBullet, GCI_OPP_C_CAPACITY_BULLET);
	LinkControlEx(plist, "opCapacityBulletSmoke", m_pCapacityBulletSmoke, GCI_OPP_C_CAPACITY_BULLETSMOKE);
	LinkControlEx(plist, "opCapacityLight", m_pCapacityLight, GCI_OPP_C_CAPACITY_LIGHT);
	LinkControlEx(plist, "opCapacityNormal", m_pCapacityNormal, GCI_OPP_C_CAPACITY_NORMAL);
	LinkControlEx(plist, "opCapacityTerrain", m_pCapacityTerrain, GCI_OPP_C_CAPACITY_TERRAIN);
	LinkControlEx(plist, "opCapacityDepth", m_pCapacityDepth, GCI_OPP_C_CAPACITY_DEPTH);
	LinkControlEx(plist, "opCapacityBloom", m_pCapacityBloom, GCI_OPP_C_CAPACITY_BLOOM);
	LinkControlEx(plist, "opCapacityPhysX", m_pCapacityPhysX, GCI_OPP_C_CAPACITY_PHYSX);
	LinkControlEx(plist, "opCapacityBulletKey", m_pCapacityBulletKey, -1);
	LinkControlEx(plist, "opCapacityBulletSmokeKey", m_pCapacityBulletSmokeKey, -1);
	LinkControlEx(plist, "opCapacityLightKey", m_pCapacityLightKey, -1);
	LinkControlEx(plist, "opCapacityNormalKey", m_pCapacityNormalKey, -1);
	LinkControlEx(plist, "opCapacityTerrainKey", m_pCapacityTerrainKey, -1);
	LinkControlEx(plist, "opCapacityDepthKey", m_pCapacityDepthKey, -1);
	LinkControlEx(plist, "opCapacityBloomKey", m_pCapacityBloomKey, -1);
	LinkControlEx(plist, "opCapacityPhysXKey", m_pCapacityPhysXKey, -1);
	LinkControlEx(plist, "opCapacityShadowKey", m_pCapacityShadowKey, -1);
	LinkControlEx(plist, "opCapacityShadowCombo", m_pCapacityShadowCombo, -1);
	LinkControlEx(plist, "opCapacityShadowButton", m_pCapacityShadowButton, GCI_OPP_B_CAPACITY_SHADOW_COMBO);
	LinkControlEx(plist, "opCapacityShadowPopup", m_pCapacityShadowPopup, GCI_OPP_L_CAPACITY_SHADOW_POPUP);
	LinkControlEx(plist, "opCapacityTextureKey", m_pCapacityTextureKey, -1);
	LinkControlEx(plist, "opCapacityTextureCombo", m_pCapacityTextureCombo, -1);
	LinkControlEx(plist, "opCapacityTextureButton", m_pCapacityTextureButton, GCI_OPP_B_CAPACITY_TEXTURE_COMBO);
	LinkControlEx(plist, "opCapacityTexturePopup", m_pCapacityTexturePopup, GCI_OPP_L_CAPACITY_TEXTURE_POPUP);
	LinkControlEx(plist, "opCapacitySpecularKey", m_pCapacitySpecularKey, -1);
	LinkControlEx(plist, "opCapacitySpecularCombo", m_pCapacitySpecularCombo, -1);
	LinkControlEx(plist, "opCapacitySpecularButton", m_pCapacitySpecularButton, GCI_OPP_B_CAPACITY_SPECULAR_COMBO);
	LinkControlEx(plist, "opCapacitySpecularPopup", m_pCapacitySpecularPopup, GCI_OPP_L_CAPACITY_SPECULAR_POPUP);
	LinkControlEx(plist, "opCapacityEffectQualityKey", m_pCapacityEffectKey, -1);
	LinkControlEx(plist, "opCapacityEffectQualityCombo", m_pCapacityEffectCombo, -1);
	LinkControlEx(plist, "opCapacityEffectQualityButton", m_pCapacityEffectButton, GCI_OPP_B_CAPACITY_EFFECT_COMBO);
	LinkControlEx(plist, "opCapacityEffectQualityPopup", m_pCapacityEffectPopup, GCI_OPP_L_CAPACITY_EFFECT_POPUP);

	LinkControlEx(plist, "opCommunityNoteCombo", m_pCommunityNoteCombo, -1);
	LinkControlEx(plist, "opCommunityInviteCombo", m_pCommunityInviteCombo, -1);
	LinkControlEx(plist, "opCommunityWhisperCombo", m_pCommunityWhisperCombo, -1);
	LinkControlEx(plist, "opCommunityChatCombo", m_pCommunityChatCombo, -1);
	LinkControlEx(plist, "opCommunityNoteButton", m_pCommunityNoteButton, GCI_OPP_B_COMMUNITY_NOTE_COMBO);
	LinkControlEx(plist, "opCommunityInviteButton", m_pCommunityInviteButton, GCI_OPP_B_COMMUNITY_INVITE_COMBO);
	LinkControlEx(plist, "opCommunityWhisperButton", m_pCommunityWhisperButton, GCI_OPP_B_COMMUNITY_WHISPER_COMBO);
	LinkControlEx(plist, "opCommunityChatButton", m_pCommunityChatButton, GCI_OPP_B_COMMUNITY_CHAT_COMBO);
	LinkControlEx(plist, "opCommunityNotePopup", m_pCommunityNotePopup, GCI_OPP_L_COMMUNITY_NOTE_POPUP);
	LinkControlEx(plist, "opCommunityInvitePopup", m_pCommunityInvitePopup, GCI_OPP_L_COMMUNITY_INVITE_POPUP);
	LinkControlEx(plist, "opCommunityWhisperPopup", m_pCommunityWhisperPopup, GCI_OPP_L_COMMUNITY_WHISPER_POPUP);
	LinkControlEx(plist, "opCommunityChatPopup", m_pCommunityChatPopup, GCI_OPP_L_COMMUNITY_CHAT_POPUP);
	LinkControlEx(plist, "opCommunityNoteKey", m_pCommunityNoteKey, -1);
	LinkControlEx(plist, "opCommunityInviteKey", m_pCommunityInviteKey, -1);
	LinkControlEx(plist, "opCommunityWhisperKey", m_pCommunityWhisperKey, -1);
	LinkControlEx(plist, "opCommunityChatKey", m_pCommunityChatKey, -1);

	LinkControlEx(plist, "opMacroAllKey", m_pMacroAllKey, -1);
	LinkControlEx(plist, "opMacroTeamKey", m_pMacroTeamKey, -1);
	LinkControlEx(plist, "opMacroKey1", m_pMacroKey1, -1);
	LinkControlEx(plist, "opMacroKey2", m_pMacroKey2, -1);
	LinkControlEx(plist, "opMacroKey3", m_pMacroKey3, -1);
	LinkControlEx(plist, "opMacroKey4", m_pMacroKey4, -1);
	LinkControlEx(plist, "opMacroKey5", m_pMacroKey5, -1);
	LinkControlEx(plist, "opMacroValue1", m_pMacroValue1, -1);
	LinkControlEx(plist, "opMacroValue2", m_pMacroValue2, -1);
	LinkControlEx(plist, "opMacroValue3", m_pMacroValue3, -1);
	LinkControlEx(plist, "opMacroValue4", m_pMacroValue4, -1);
	LinkControlEx(plist, "opMacroValue5", m_pMacroValue5, -1);
	LinkControlEx(plist, "opMacroAll1", m_pMacroAll1, GCI_OPP_R_MACRO_ALL_1);
	LinkControlEx(plist, "opMacroAll2", m_pMacroAll2, GCI_OPP_R_MACRO_ALL_2);
	LinkControlEx(plist, "opMacroAll3", m_pMacroAll3, GCI_OPP_R_MACRO_ALL_3);
	LinkControlEx(plist, "opMacroAll4", m_pMacroAll4, GCI_OPP_R_MACRO_ALL_4);
	LinkControlEx(plist, "opMacroAll5", m_pMacroAll5, GCI_OPP_R_MACRO_ALL_5);
	LinkControlEx(plist, "opMacroTeam1", m_pMacroTeam1, GCI_OPP_R_MACRO_TEAM_1);
	LinkControlEx(plist, "opMacroTeam2", m_pMacroTeam2, GCI_OPP_R_MACRO_TEAM_2);
	LinkControlEx(plist, "opMacroTeam3", m_pMacroTeam3, GCI_OPP_R_MACRO_TEAM_3);
	LinkControlEx(plist, "opMacroTeam4", m_pMacroTeam4, GCI_OPP_R_MACRO_TEAM_4);
	LinkControlEx(plist, "opMacroTeam5", m_pMacroTeam5, GCI_OPP_R_MACRO_TEAM_5);
	LinkControlEx(plist, "opControlKeyboardKey", m_pControlKeyboardKey, -1);
	LinkControlEx(plist, "opControlSelect", m_pControlSelect, -1);
	LinkControlEx(plist, "opControlFocus", m_pControlFocus, -1);
	LinkControlEx(plist, "opControlList", m_pControlList, GCI_OPP_L_CONTROL_LIST);

	for(UINT32 i = 0; i < MAX_KEYMAP_SLOT; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "opControlSlotKey%d", i + 1);
		LinkControlEx(plist, strName, m_keymapSlot[i]._pKey, -1);

		sprintf_s(strName, "opControlSlotValue%d", i + 1);
		LinkControlEx(plist, strName, m_keymapSlot[i]._pValue, -1);
	}

	LinkControlEx(plist, "opControlReverse", m_pControlReverse, GCI_OPP_C_CONTROL_REVERSE);
	LinkControlEx(plist, "opControlReverseKey", m_pControlReverseKey, -1);
	LinkControlEx(plist, "opControlMouseKey", m_pControlMouseKey, -1);
	LinkControlEx(plist, "opControlMouseGagePrev", m_pControlMouseGagePrev, GCI_OPP_B_CONTROL_MOUSE_PREV);
	LinkControlEx(plist, "opControlMouseGageNext", m_pControlMouseGageNext, GCI_OPP_B_CONTROL_MOUSE_NEXT);
	LinkControlEx(plist, "opControlMouseGage", m_pControlMouseGage, -1);
	LinkControlEx(plist, "opControlFovKey", m_pControlFovKey, -1);
	LinkControlEx(plist, "opControlFovPrev", m_pControlFovPrev, GCI_OPP_B_CONTROL_FOV_PREV);
	LinkControlEx(plist, "opControlFovNext", m_pControlFovNext, GCI_OPP_B_CONTROL_FOV_NEXT);
	LinkControlEx(plist, "opControlFovGage", m_pControlFovGage, -1);
	LinkControlEx(plist, "opControlKeyButton", m_pControlKeyButton, GCI_OPP_B_CONTROL_BUTTON);
	LinkControlEx(plist, "opControlKeyDelete", m_pControlKeyDelete, GCI_OPP_B_CONTROL_DELETE);
	LinkControlEx(plist, "opControlMouseValue", m_pControlMouseValue, -1);
	LinkControlEx(plist, "opControlFovValue", m_pControlFovValue, -1);

	LinkControlEx(plist, "opRatingNotice", m_pRatingNotice, -1);
	LinkControlEx(plist, "opRatingNoticeBG", m_pRatingNoticeBG, -1);

	return FALSE;
}

void CPopupOption::OnInitControl(void)
{
	// 각 섹션 초기화
	OptionInitLoad();

	// 컨트롤 초기화
	_InitCommonControl();
	_InitGamePlay();
	_InitVideo();
	_InitInterface();
	_InitSound();
	_InitControl();
	_InitPerformance();
	_InitCommunity();
	_InitMacro();
	_InitVoiceChat();
	_InitRating();

	// 초기값 설정
	_ResetGamePlay();
	_ResetVideo();
	_ResetInterface();
	_ResetSound();
	_ResetControl();
	_ResetPerformance();
	_ResetCommunity();
	_ResetMacro();
	_ResetVoiceChat();

	CPopupBase::OnInitControl();
}

void CPopupOption::SetInputDisable(BOOL bDisable)
{
	m_pOptionGui->setInputDisable(bDisable);
}

void CPopupOption::SetPushedStateButton(INT32 menuButtonType,BOOL bPushed)
{
	if( !m_bInGame)
	{
		if( m_pParent->IsExactTypeOf( CStageReady::GetClassMeta()))
			CGuiNotifyReceiver::SetPushedStateButton( menuButtonType, bPushed);
	}
}

BOOL CPopupOption::GetInputDisable(void)
{
	return m_pOptionGui->getInputDisable();
}

void CPopupOption::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup();

	// 비디오 옵션 변경 중 게임이 시작되면 문제가 되기때문에, Ready 상태를 푼다.
	if( !m_bInGame)
	{
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
		if( pFrame->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
		{
			SLOT_INFO* info = g_pGameContext->getSlotInfo(g_pGameContext->getMySlotIdx());

			if (SLOT_STATE_READY == info->_State)
			{
				g_pGameContext->SetEvent(EVENT_READY_BATTLE);
			}
		}
	}

	m_pOptionGui->SetEnable(TRUE);

	UpdateInit();

	// Default View Sect
	_EnableExclusionSect(OS_PLAY);

	SetPushedStateButton(MBT_OPTION, TRUE);
}

void CPopupOption::OnClosePopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnClosePopup();

	// 저장되어있는 값들로 변수들을 설정합니다.
	OptionInitLoad();

	_ResetGamePlay();
	_ResetVideo();
	_ResetInterface();
	_ResetSound();
	_ResetControl();
	_ResetPerformance();
	_ResetCommunity();
	_ResetMacro();
	_ResetVoiceChat();

	if( m_pControlPopup )
	{
		GameUI::CloseMsgBox( m_pControlPopup);
		m_pControlPopup = NULL;
	}

	m_pOptionGui->SetEnable(FALSE);

	SetPushedStateButton(MBT_OPTION, FALSE);

	if( g_pFramework->GetUIMainframe() != NULL)
	{
		g_pFramework->GetUIMainframe()->ClosePopup( UPW_OPTION);
	}
	else if( g_pFramework->GetUIBattleFrame() != NULL)
	{

	}
}

BOOL CPopupOption::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			if( InputKeyboardEnter())
			{
				if (FALSE == _IsInputingIme())
				{
					if( !m_bEnableConfirm)
					{
						OnOptionConfirm();
						return TRUE;
					}
				}
			}
		}
	}

	return CPopupBase::OnEvent(event, pObj, param2, code);
}

void	CPopupOption::Process_Confirm()
{
	if (FALSE == _IsInputingIme())
	{
		if( !m_bEnableConfirm)
		{
			OnOptionConfirm();
		}
	}	
	
}

void	CPopupOption::Process_Cancel()
{
	if (FALSE == _IsInputingIme())
	{
		OnOptionCancel();
	}	
		
}



void CPopupOption::OnInGame(void)
{
	m_bInGame = TRUE;
}

void CPopupOption::OnPlay(void)
{
	_EnableExclusionSect(OS_PLAY);
}

void CPopupOption::OnVideo(void)
{
	_EnableExclusionSect(OS_VIDEO);
}

void CPopupOption::OnInterface(void)
{
	_EnableExclusionSect(OS_INTERFACE);
}

void CPopupOption::OnSound(void)
{
	_EnableExclusionSect(OS_SOUND);
}

void CPopupOption::OnControl(void)
{
	_EnableExclusionSect(OS_CONTROL);
}

void CPopupOption::OnCapacity(void)
{
	_EnableExclusionSect(OS_CAPACITY);
}

void CPopupOption::OnCommunity(void)
{
	//STRING_POPUP( GAME_STRING("STR_POPUP_PREPARE_PLAN"));	//	"차후 추가될 기능입니다."
	_EnableExclusionSect(OS_COMMUNITY);
}

void CPopupOption::OnMacro(void)
{
	_EnableExclusionSect(OS_MACRO);
}

void CPopupOption::OnVoice(void)
{
	//	"차후 추가될 기능입니다."
	UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_PREPARE_PLAN"));
	RePositionMsgBox( pBox);
	//_EnableExclusionSect(OS_VOICE);
}

void CPopupOption::OnRating(void)
{
	_EnableExclusionSect(OS_RATING);
}

void CPopupOption::OnPlayBloodType(UINT32 idxBlood)
{
	_SetGamePlayBloodType(idxBlood);
}

void CPopupOption::OnPlayBlood(void)
{
	_SetGamePlayBlood();
}

void CPopupOption::OnPlayHit(void)
{
	_SetGamePlayCrossHairEffect();
}

void CPopupOption::OnPlayHandTypeCombo(void)
{
	ExclusionPopup(m_pPlayHandTypePopup);
}

void CPopupOption::OnPlayHandTypePopup(UINT32 idx)
{
	m_nHandType = idx;

	_SetGamePlayHandType();
}

void CPopupOption::OnPlayCorsshair(UINT32 idxCrosshair)
{
	_SetGamePlayCrossHairType(idxCrosshair);
}

void CPopupOption::OnInterfaceNickname(void)
{
	_SetInterfaceNick();
}

void CPopupOption::OnInterfaceMark(void)
{
	_SetInterfaceMark();
}

void CPopupOption::OnInterfaceEffect(void)
{
	m_bEnableTextEffect = m_pInterfaceEffect->getCheckBoxState();
}

void CPopupOption::OnInterfaceChallenge(void)
{
	m_bEnableChallenge = m_pInterfaceChallenge->getCheckBoxState();
}

void CPopupOption::OnInterface1VS1(void)
{
	m_bEnable1VS1 = m_pInterface1VS1->getCheckBoxState();
}

void CPopupOption::OnInterfaceRadarButton(void)
{
	ExclusionPopup(m_pInterfaceRadarPopup);
}

void CPopupOption::OnInterfaceRadarPopup(UINT32 idxItem)
{
	if (m_pInterfaceRadarPopup->GetElementCount() > (INT32)idxItem)
	{
		m_pInterfaceRadarCombo->SetText((char*)m_pInterfaceRadarPopup->GetText(idxItem));
		m_bMinimapType = idxItem;
	}
}

void CPopupOption::OnSoundBgm(void)
{
	_SetSoundBG();

	//m_pSoundBgm->setCheckBoxState(FALSE);
	//STRING_POPUP("차후 추가될 기능입니다.");
}

void CPopupOption::OnSoundBgmPrev(void)
{
	if( m_fDelayLocalTime == 0.0f )
	{
		_SetSoundBG(-1.0f);
	}
	else
	{
		if( m_fDelayLocalTime > 0.3f)
		{
			_SetSoundBG(-1.0f);
		}
	}

	m_fDelayLocalTime += m_fElapsedTime;
}

void CPopupOption::OnSoundBgmNext(void)
{
	if( m_fDelayLocalTime == 0.0f )
	{
		_SetSoundBG(1.0f);
	}
	else
	{
		if( m_fDelayLocalTime > 0.3f)
		{
			_SetSoundBG(1.0f);
		}
	}

	m_fDelayLocalTime += m_fElapsedTime;
}

void CPopupOption::OnSoundEffect(void)
{
	_SetSoundFX();
}

void CPopupOption::OnSoundEffectPrev(void)
{
	if( m_fDelayLocalTime == 0.0f )
	{
		_SetSoundFX(-1.0f);
	}
	else
	{
		if( m_fDelayLocalTime > 0.3f)
		{
			_SetSoundFX(-1.0f);
		}
	}

	m_fDelayLocalTime += m_fElapsedTime;
}

void CPopupOption::OnSoundEffectNext(void)
{
	if( m_fDelayLocalTime == 0.0f )
	{
		_SetSoundFX(1.0f);
	}
	else
	{
		if( m_fDelayLocalTime > 0.3f)
		{
			_SetSoundFX(1.0f);
		}
	}

	m_fDelayLocalTime += m_fElapsedTime;
}

void CPopupOption::OnSoundSpeakerCombo(void)
{
	//	"차후 추가될 기능입니다."
	UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_PREPARE_PLAN"));
	RePositionMsgBox( pBox);
}

void CPopupOption::OnSoundSpeakerPopup(UINT32 idxSpeaker)
{
	m_nSpeakerType = idxSpeaker;

	_SetSoundSpeaker();
}

void CPopupOption::OnSoundVoice(void)
{
	_SetRadioMessage();
}

void CPopupOption::OnSoundVoiceCombo(void)
{
	ExclusionPopup(m_pSoundVoicePopup);
}

void CPopupOption::OnSoundVoicePopup(UINT32 idxVoice)
{
	m_nRadioMessageType = idxVoice;

	_SetRadioMessage();
}

void CPopupOption::OnCommunityNoteComb(void)
{
#ifdef ACTIVATE_COMMUNITY_STATE_NOTE
	// 쪽지 수신 허용, 차단 기능
	ExclusionPopup(m_pCommunityNotePopup);
#else
	/*차후 구현 예정입니다.*/
	UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_READY_UNDER_CONSTRUCTION"));
	RePositionMsgBox( pBox);
#endif
}

void CPopupOption::OnCommunityInviteComb(void)
{
	ExclusionPopup(m_pCommunityInvitePopup);
}

void CPopupOption::OnCommunityWhisperComb(void)
{
	ExclusionPopup(m_pCommunityWhisperPopup);
}

void CPopupOption::OnCommunityChatComb(void)
{
	ExclusionPopup(m_pCommunityChatPopup);
}

void CPopupOption::OnCommunityNotePopup(UINT32 idxItem)
{
#ifdef ACTIVATE_COMMUNITY_STATE_NOTE
	// 쪽지 수신 허용, 차단 기능
	_SetCommunityMessage(idxItem);
#else
	/*차후 구현 예정입니다.*/
	UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_READY_UNDER_CONSTRUCTION"));
	RePositionMsgBox( pBox);
#endif
}

void CPopupOption::OnCommunityInvitePopup(UINT32 idxItem)
{
	_SetCommunityInvite(idxItem);
}

void CPopupOption::OnCommunityWhisperPopup(UINT32 idxItem)
{
	_SetCommunityWhisper(idxItem);
}

void CPopupOption::OnCommunityChatPopup(UINT32 idxItem)
{
	_SetCommunityChat(idxItem);
}

void CPopupOption::OnCommunityMacro1(UINT32 idxItem)
{
	m_pMacroAll1->SetAllShapeEnable(FALSE);
	m_pMacroTeam1->SetAllShapeEnable(FALSE);

	UINT32 nAll = (idxItem - 1) * -2;	// 0 -> 2, 1 -> 0
	UINT32 nTeam = idxItem * 2;			// 0 -> 0, 1 -> 2

	m_pMacroAll1->SetShapeEnable(GUI_SHAPE_OFFSET + nAll, TRUE);
	m_pMacroTeam1->SetShapeEnable(GUI_SHAPE_OFFSET + nTeam, TRUE);

	_SetMacro(0, 0 != idxItem);
}

void CPopupOption::OnCommunityMacro2(UINT32 idxItem)
{
	m_pMacroAll2->SetAllShapeEnable(FALSE);
	m_pMacroTeam2->SetAllShapeEnable(FALSE);

	UINT32 nAll = (idxItem - 1) * -2;	// 0 -> 2, 1 -> 0
	UINT32 nTeam = idxItem * 2;			// 0 -> 0, 1 -> 2

	m_pMacroAll2->SetShapeEnable(GUI_SHAPE_OFFSET + nAll, TRUE);
	m_pMacroTeam2->SetShapeEnable(GUI_SHAPE_OFFSET + nTeam, TRUE);

	_SetMacro(1, 0 != idxItem);
}

void CPopupOption::OnCommunityMacro3(UINT32 idxItem)
{
	m_pMacroAll3->SetAllShapeEnable(FALSE);
	m_pMacroTeam3->SetAllShapeEnable(FALSE);

	UINT32 nAll = (idxItem - 1) * -2;	// 0 -> 2, 1 -> 0
	UINT32 nTeam = idxItem * 2;			// 0 -> 0, 1 -> 2

	m_pMacroAll3->SetShapeEnable(GUI_SHAPE_OFFSET + nAll, TRUE);
	m_pMacroTeam3->SetShapeEnable(GUI_SHAPE_OFFSET + nTeam, TRUE);

	_SetMacro(2, 0 != idxItem);
}

void CPopupOption::OnCommunityMacro4(UINT32 idxItem)
{
	m_pMacroAll4->SetAllShapeEnable(FALSE);
	m_pMacroTeam4->SetAllShapeEnable(FALSE);

	UINT32 nAll = (idxItem - 1) * -2;	// 0 -> 2, 1 -> 0
	UINT32 nTeam = idxItem * 2;			// 0 -> 0, 1 -> 2

	m_pMacroAll4->SetShapeEnable(GUI_SHAPE_OFFSET + nAll, TRUE);
	m_pMacroTeam4->SetShapeEnable(GUI_SHAPE_OFFSET + nTeam, TRUE);

	_SetMacro(3, 0 != idxItem);
}

void CPopupOption::OnCommunityMacro5(UINT32 idxItem)
{
	m_pMacroAll5->SetAllShapeEnable(FALSE);
	m_pMacroTeam5->SetAllShapeEnable(FALSE);

	UINT32 nAll = (idxItem - 1) * -2;	// 0 -> 2, 1 -> 0
	UINT32 nTeam = idxItem * 2;			// 0 -> 0, 1 -> 2

	m_pMacroAll5->SetShapeEnable(GUI_SHAPE_OFFSET + nAll, TRUE);
	m_pMacroTeam5->SetShapeEnable(GUI_SHAPE_OFFSET + nTeam, TRUE);

	_SetMacro(4, 0 != idxItem);
}

void CPopupOption::OnControlListFocus(UINT32 idxItem)
{
	m_pControlFocus->setPositionY((REAL32)(idxItem * 24 + 2));
	m_pControlFocus->SetEnable(TRUE);
}

void CPopupOption::OnControlListSelect(INT32 idxItem)
{
	UINT32 idxSlot = idxItem - m_pControlList->GetCurrentTop();

	m_pControlSelect->setPositionY((REAL32)(idxSlot * 24 + 2));
	m_pControlSelect->SetEnable(TRUE);

	m_nSelectedIdx = idxItem;
}

void CPopupOption::OnControlListChange(INT32 idxItem)
{
	UINT32 idxSlot = idxItem - m_pControlList->GetCurrentTop();

	m_nSelectedKeyMapIdx = idxSlot + 1;
	m_nCurKeyMapIdx = m_pControlList->GetCurrentTop();
}

void CPopupOption::OnControlListDrag(void)
{
	m_nSelectedKeyMapIdx = 0;
	m_nCurKeyMapIdx = m_pControlList->GetCurrentTop();

	UINT32 idxSlot = m_nSelectedIdx - m_pControlList->GetCurrentTop();

	if (0 <= idxSlot && idxSlot < MAX_KEYMAP_SLOT)
	{
		m_pControlSelect->setPositionY(idxSlot * 24.0f + 2.0f);
		m_pControlSelect->SetEnable(TRUE);
	}
}

void CPopupOption::OnControlReverse(void)
{
	_SetControlMouseInvert();
}

void CPopupOption::OnControlMousePrev(void)
{
	// 0.0001은 정도차이에의한 출력 오차를 줄이기위해
	if( m_fDelayLocalTime == 0.0f )
	{
		_SetControlMouseSense(-0.01f);
	}
	else
	{
		if( m_fDelayLocalTime > 0.3f)
		{
			_SetControlMouseSense(-0.01f);
		}
	}

	m_fDelayLocalTime += m_fElapsedTime;
}

void CPopupOption::OnControlMouseNext(void)
{
	// 0.0001은 정도차이에의한 출력 오차를 줄이기위해
	if( m_fDelayLocalTime == 0.0f )
	{
		_SetControlMouseSense(0.01f);
	}
	else
	{
		if( m_fDelayLocalTime > 0.3f)
		{
			_SetControlMouseSense(0.01f);
		}
	}

	m_fDelayLocalTime += m_fElapsedTime;
}

void CPopupOption::OnControlFovPrev(void)
{
	if( m_fDelayLocalTime == 0.0f )
	{
		_SetControlFov(-1.0f);
	}
	else
	{
		if( m_fDelayLocalTime > 0.3f)
		{
			_SetControlFov(-1.0f);
		}
	}

	m_fDelayLocalTime += m_fElapsedTime;
}

void CPopupOption::OnControlFovNext(void)
{
	if( m_fDelayLocalTime == 0.0f )
	{
		_SetControlFov(1.0f);
	}
	else
	{
		if( m_fDelayLocalTime > 0.3f)
		{
			_SetControlFov(1.0f);
		}
	}

	m_fDelayLocalTime += m_fElapsedTime;
}

void CPopupOption::OnControlButton(void)
{
	if (-1 != m_nSelectedIdx)
	{
		OnControlListChange(m_nSelectedIdx);
	}
	else
	{
		/*변경할 키를 먼저 선택해 주세요.*/
		UIMessageBox* pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_MODITYCATION_KEY"));
		RePositionMsgBox( pBox);
	}
}

void CPopupOption::OnControlDelete(void)
{
	if (-1 != m_nSelectedIdx)
	{
		_SetKeyMapValue(m_nSelectedIdx, I3I_KEY_UNKNOWN);
	}
	else
	{
		/*삭제할 키를 먼저 선택해 주세요.*/
		UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_DELETION_KEY"));
		RePositionMsgBox( pBox);
	}
}

void CPopupOption::_EnableExclusionSect(OptionSectType enableSect)
{
	m_sect[m_currentSect]._pButton->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_sect[m_currentSect]._pSect->SetEnable(FALSE);

	m_sect[enableSect]._pButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_sect[enableSect]._pSect->SetEnable(TRUE);

	m_currentSect = enableSect;

	// FrameCaption 변경
	char strCaption[MAX_STRING_COUNT];

	switch(m_currentSect)
	{
	case OS_PLAY:
		i3String::Copy(strCaption, GAME_STRING("STR_POPUP_MESSAGE_CHANGE_PART_FOR_PLAYING"), MAX_STRING_COUNT );//:플레이에 영향을 주는 부분을 변경합니다.
		break;
	case OS_VIDEO:
		i3String::Copy(strCaption, GAME_STRING("STBL_IDX_OPT_1"), MAX_STRING_COUNT );
		break;
	case OS_INTERFACE:
		i3String::Copy(strCaption, GAME_STRING("STR_POPUP_MESSAGE_USE_INTERFACE"), MAX_STRING_COUNT );//:인터페이스의 사용 여부를 결정합니다.
		break;
	case OS_SOUND:
		i3String::Copy(strCaption, GAME_STRING("STBL_IDX_OPT_2"), MAX_STRING_COUNT );
		break;
	case OS_CONTROL:
		i3String::Copy(strCaption, GAME_STRING("STBL_IDX_OPT_3"), MAX_STRING_COUNT );
		break;
	case OS_CAPACITY:
		i3String::Copy(strCaption, GAME_STRING("STBL_IDX_OPT_4"), MAX_STRING_COUNT );
		break;
	case OS_COMMUNITY:
		i3String::Copy(strCaption, GAME_STRING("STBL_IDX_OPT_5"), MAX_STRING_COUNT );
		break;
	case OS_MACRO:
		i3String::Copy(strCaption, GAME_STRING("STBL_IDX_OPT_6"), MAX_STRING_COUNT );
		break;
	case OS_VOICE:
		i3String::Copy(strCaption, GAME_STRING("STBL_IDX_OPT_1"), MAX_STRING_COUNT );
		break;
	case OS_RATING:
		i3String::Copy(strCaption, GAME_STRING(""), MAX_STRING_COUNT );
		break;
	}

	m_pFrameCaption->SetText(strCaption);
}

void CPopupOption::_ResetInterface(void)
{
	m_pInterfaceNickname->setCheckBoxState(m_bEnableFrindlyName);
	m_pInterfaceMark->setCheckBoxState(m_bEnableFrindlyMark);
	m_pInterfaceEffect->setCheckBoxState(m_bEnableTextEffect);
	m_pInterfaceChallenge->setCheckBoxState(m_bEnableChallenge);
	m_pInterface1VS1->setCheckBoxState(m_bEnable1VS1);

	m_pInterfaceRadarCombo->SetText((char*)m_pInterfaceRadarPopup->GetText(m_bMinimapType ? 1 : 0));
}

void CPopupOption::_InitCommonControl(void)
{
	m_pOptionGui->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pOptionCaption, GAME_STRING("STR_POPUP_ENV_SETTING"));/*환경설정*/

	INIT_CAPTION_BUTTON(m_sect[OS_PLAY]._pButton, GAME_STRING("STR_POPUP_GAME_PLAY"));/*게임플레이*/
	INIT_CAPTION_BUTTON(m_sect[OS_VIDEO]._pButton, GAME_STRING("STR_POPUP_VIDEO"));/*비디오*/
	INIT_CAPTION_BUTTON(m_sect[OS_INTERFACE]._pButton, GAME_STRING("STR_POPUP_OPTION_INTERFACE"));/*인터페이스*/
	INIT_CAPTION_BUTTON(m_sect[OS_SOUND]._pButton, GAME_STRING("STR_POPUP_SOUND"));/*사운드*/
	INIT_CAPTION_BUTTON(m_sect[OS_CONTROL]._pButton, GAME_STRING("STR_POPUP_OPTION_CONTROL"));/*컨트롤*/
	INIT_CAPTION_BUTTON(m_sect[OS_CAPACITY]._pButton, GAME_STRING("STR_POPUP_PERFORMANCE"));/*성능*/
	INIT_CAPTION_BUTTON(m_sect[OS_COMMUNITY]._pButton, GAME_STRING("STR_POPUP_COMMUNITY"));/*커뮤니티*/
	INIT_CAPTION_BUTTON(m_sect[OS_MACRO]._pButton, GAME_STRING("STR_POPUP_MACRO"));/*매크로*/
	INIT_CAPTION_BUTTON(m_sect[OS_VOICE]._pButton, GAME_STRING("STR_POPUP_VOICE_CHATTING"));/*음성채팅*/
	INIT_CAPTION_BUTTON(m_sect[OS_RATING]._pButton, GAME_STRING("STR_POPUP_RATING"));/*심의등급*/
	INIT_CAPTION_BUTTON(m_pInit, GAME_STRING("STR_POPUP_RESET"));/*초기화*/
	INIT_CAPTION_BUTTON(m_pOptimize, GAME_STRING("STR_POPUP_MINMAL_OPTION"));/*최소옵션*/
	INIT_CAPTION_BUTTON(m_pCofirm, GAME_STRING("STR_POPUP_OK"));	//	"확인"
	INIT_CAPTION_BUTTON(m_pCancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"취소"


	// 게임플레이 외 세션은 디스에이블
	m_sect[OS_VIDEO]._pSect->SetEnable(FALSE);
	m_sect[OS_INTERFACE]._pSect->SetEnable(FALSE);
	m_sect[OS_SOUND]._pSect->SetEnable(FALSE);
	m_sect[OS_CONTROL]._pSect->SetEnable(FALSE);
	m_sect[OS_CAPACITY]._pSect->SetEnable(FALSE);
	m_sect[OS_COMMUNITY]._pSect->SetEnable(FALSE);
	m_sect[OS_MACRO]._pSect->SetEnable(FALSE);
	m_sect[OS_RATING]._pSect->SetEnable(FALSE);

	// Voice Chatting관련은 화면에서 보이지 않게 처리합니다.
	m_sect[OS_VOICE]._pButton->SetEnable( FALSE);
	m_sect[OS_VOICE]._pSect->SetEnable(FALSE);

	// 옵션창의 심의등급 버튼...한국만 사용한다. -성민
#if !defined( LOCALE_KOREA)
	m_sect[OS_RATING]._pButton->SetEnable( FALSE);
#endif

	// 초기 세션은 게임플레이
	m_currentSect = OS_PLAY;
	m_sect[OS_PLAY]._pButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_sect[OS_PLAY]._pSect->SetEnable(TRUE);

	m_pFrameCaption->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFrameCaption->SetTextColor(GetColor(GCT_DEFAULT));
	m_pFrameCaption->SetText(GAME_STRING("STBL_IDX_OPT_1"));

	// In Game Option에서 사용되지 않는 항목들을 비활성화 합니다.
	if( m_bInGame)
	{
		COLOR col;
		i3Color::Set( &col, (UINT8) 180, 180, 180, 100);

		m_pInit->SetEnable( FALSE);
		m_pOptimize->SetEnable( FALSE);

		m_sect[OS_VIDEO]._pButton->setInputDisable( TRUE);
		m_sect[OS_VIDEO]._pButton->SetColor( I3GUI_CONTROL_STATE_NORMAL, &col);
		m_sect[OS_VIDEO]._pButton->SetCaptionColor( &col);
		m_sect[OS_VIDEO]._pSect->SetEnable( FALSE);

		m_sect[OS_COMMUNITY]._pButton->setInputDisable( TRUE);
		m_sect[OS_COMMUNITY]._pButton->SetColor( I3GUI_CONTROL_STATE_NORMAL, &col);
		m_sect[OS_COMMUNITY]._pButton->SetCaptionColor( &col);
		m_sect[OS_COMMUNITY]._pSect->SetEnable( FALSE);

		m_sect[OS_VOICE]._pButton->setInputDisable( TRUE);
		m_sect[OS_VOICE]._pButton->SetColor( I3GUI_CONTROL_STATE_NORMAL, &col);
		m_sect[OS_VOICE]._pButton->SetCaptionColor( &col);
		m_sect[OS_VOICE]._pSect->SetEnable( FALSE);
	}
}

void CPopupOption::_InitGamePlay(void)
{
	m_pPlayBloodKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPlayBlood2Key->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPlayHitKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPlayCrosshairKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPlayHandTypeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPlayBloodKey->SetTextColor(GetColor(GCT_STATIC));
	m_pPlayBlood2Key->SetTextColor(GetColor(GCT_STATIC));
	m_pPlayHitKey->SetTextColor(GetColor(GCT_STATIC));
	m_pPlayCrosshairKey->SetTextColor(GetColor(GCT_STATIC));
	m_pPlayHandTypeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pPlayBloodKey->SetText(GAME_STRING("STR_POPUP_DISPLAY_BLOOD"));/*혈흔 표시*/
	m_pPlayBlood2Key->SetText(GAME_STRING("STR_POPUP_DISPLAY_BLOOD2"));/*혈흔 표시*/ //STR_POPUP_DISPLAY_BLOOD2
	m_pPlayHitKey->SetText(GAME_STRING("STR_POPUP_HITTING_CROSSHAIR"));/*크로스헤어 타격 효과*/
	m_pPlayCrosshairKey->SetText(GAME_STRING("STR_POPUP_CROSSHAIR"));/*크로스헤어*/
	m_pPlayHandTypeKey->SetText(GAME_STRING("STR_POPUP_WEAPON_HAND"));/*무기 드는 손*/
	m_pPlayHandTypeCombo->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

#if defined( LOCALE_CHINA)
	m_pPlayBlood->SetVisible(FALSE);
	m_pPlayBlood->SetEnable(FALSE);
#else
	m_pPlayBloodRadio1->SetVisible(FALSE);
	m_pPlayBloodRadio1->SetEnable(FALSE);

	m_pPlayBloodRadio2->SetVisible(FALSE);
	m_pPlayBloodRadio2->SetEnable(FALSE);

	m_pPlayBlood2Key->SetEnable(FALSE);
	m_pPlayBlood2Key->SetVisible(FALSE);
#endif

	m_pPlayCrosshairType1->SetAllShapeEnable(FALSE);
	m_pPlayCrosshairType2->SetAllShapeEnable(FALSE);
	m_pPlayCrosshairType3->SetAllShapeEnable(FALSE);
	m_pPlayCrosshairType4->SetAllShapeEnable(FALSE);
	m_pPlayCrosshairType1->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);
	m_pPlayCrosshairType2->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
	m_pPlayCrosshairType3->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
	m_pPlayCrosshairType4->SetShapeEnable(GUI_SHAPE_OFFSET + 3, TRUE);

	m_pPlayHandTypePopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pPlayHandTypePopup->SetSelColor(100, 100, 100, 100);
	m_pPlayHandTypePopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pPlayHandTypePopup->SetExclusiveEnableControl(TRUE);
	m_pPlayHandTypePopup->SetTextAlign( TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPlayHandTypePopup->AddElement(GAME_STRING("STR_POPUP_RIGHT_HANDED"));/*오른손잡이*/
	m_pPlayHandTypePopup->AddElement(GAME_STRING("STR_POPUP_LEFT_HANDED"));/*왼손잡이*/
	m_pPlayHandTypePopup->SetEnable(FALSE);

	switch( m_nHandType)
	{
	case 0:
		m_pPlayHandTypeCombo->SetText(GAME_STRING("STR_POPUP_RIGHT_HANDED"));/*오른손잡이*/
		break;
	case 1:
		m_pPlayHandTypeCombo->SetText(GAME_STRING("STR_POPUP_LEFT_HANDED"));/*왼손잡이*/
		break;
	default:
		m_pPlayHandTypeCombo->SetText(GAME_STRING("STR_POPUP_RIGHT_HANDED"));/*오른손잡이*/
		break;
	}
}

void CPopupOption::_InitInterface(void)
{
	m_pInterfaceNicknameKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pInterfaceMarkKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pInterfaceEffectKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pInterfaceChallengeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pInterface1VS1Key->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pInterfaceNicknameKey->SetTextColor(GetColor(GCT_STATIC));
	m_pInterfaceMarkKey->SetTextColor(GetColor(GCT_STATIC));
	m_pInterfaceEffectKey->SetTextColor(GetColor(GCT_STATIC));
	m_pInterfaceChallengeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pInterface1VS1Key->SetTextColor(GetColor(GCT_STATIC));
	m_pInterfaceNicknameKey->SetText(GAME_STRING("STR_POPUP_DISPLAY_NICK_FOR_ALLY"));/*아군 닉네임 표시*/
	m_pInterfaceMarkKey->SetText(GAME_STRING("STR_POPUP_DISPLAY_MARK_FOR_ALLY"));/*아군 마크 표시*/
	m_pInterfaceEffectKey->SetText(GAME_STRING("STR_POPUP_DISPLAY_EFFECT"));/*텍스트 이펙트 표시*/
	m_pInterfaceChallengeKey->SetText(GAME_STRING("STR_POPUP_MISSION_WINDOW"));/*임무 알림창*/
	m_pInterface1VS1Key->SetText(GAME_STRING("STR_POPUP_VERSUS_HISTORY"));/*일대일 전적 기능*/

	m_pInterfaceRadarKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pInterfaceRadarCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pInterfaceRadarKey->SetTextColor(GetColor(GCT_STATIC));
	m_pInterfaceRadarCombo->SetTextColor(GetColor(GCT_STATIC));
	m_pInterfaceRadarKey->SetText(GAME_STRING("STR_POPUP_ROTATE_MINI_MAP"));/*미니맵 회전 여부*/

	INIT_COMBO_EX(m_pInterfaceRadarButton);

	m_pInterfaceRadarPopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pInterfaceRadarPopup->SetSelColor(100, 100, 100, 100);
	m_pInterfaceRadarPopup->SetTextColor(GetColor(GCT_STATIC));
	m_pInterfaceRadarPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pInterfaceRadarPopup->SetExclusiveEnableControl(TRUE);
	m_pInterfaceRadarPopup->AddElement(GAME_STRING("STR_POPUP_POPUP_ROTATION"));/*회전*/
	m_pInterfaceRadarPopup->AddElement(GAME_STRING("STR_POPUP_FIXED"));/*고정*/
	m_pInterfaceRadarPopup->SetEnable(FALSE);

	m_pInterfaceRadarCombo->SetText((char*)m_pInterfaceRadarPopup->GetText(0));
}

void CPopupOption::_InitSound(void)
{
	m_pSoundBgmKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSoundEffectKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSoundSpeakerKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSoundVoiceKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSoundBgmKey->SetTextColor(GetColor(GCT_STATIC));
	m_pSoundEffectKey->SetTextColor(GetColor(GCT_STATIC));
	m_pSoundSpeakerKey->SetTextColor(GetColor(GCT_STATIC));
	m_pSoundVoiceKey->SetTextColor(GetColor(GCT_STATIC));
	m_pSoundBgmKey->SetText(GAME_STRING("STR_POPUP_SETTING_BGM"));/*배경음 설정*/
	m_pSoundEffectKey->SetText(GAME_STRING("STR_POPUP_SET_SFX_VOLUME"));/*효과음 설정*/
	m_pSoundSpeakerKey->SetText(GAME_STRING("STR_POPUP_SETTING_SPEAKER"));/*스피커 설정*/
	m_pSoundVoiceKey->SetText(GAME_STRING("STR_POPUP_RADIO_MESSAGE"));/*라디오 메시지*/


	CREATE_IME_ELEMENT ime;

	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = 3;
	ime._StyleIME = IME_STYLE_NUMERIC_ONLY | IME_STYLE_IGNORE_KEYINPUT_IME_MODE;

	m_pSoundBgmValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pSoundBgmValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSoundBgmValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSoundBgmValue->SetAlphaNumericOnly( true);

	m_pSoundEffectValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pSoundEffectValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSoundEffectValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSoundEffectValue->SetAlphaNumericOnly( true);

	m_pSoundSpeakerCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSoundVoiceCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	m_pSoundEffectValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSoundSpeakerCombo->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSoundVoiceCombo->SetTextColor(GetColor(GCT_DEFAULT));

	INIT_REPEAT_COMBO(m_pSoundBgmPrev);
	INIT_REPEAT_COMBO(m_pSoundBgmNext);
	INIT_REPEAT_COMBO(m_pSoundEffectPrev);
	INIT_REPEAT_COMBO(m_pSoundEffectNext);
	INIT_COMBO_EX(m_pSoundSpeakerButton);
	INIT_COMBO_EX(m_pSoundVoiceButton);

	m_pSoundSpeakerPopup->CreateTextEx(1,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pSoundSpeakerPopup->SetSelColor(100, 100, 100, 100);
	m_pSoundSpeakerPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSoundSpeakerPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSoundSpeakerPopup->SetExclusiveEnableControl(TRUE);
	m_pSoundSpeakerPopup->AddElement( GAME_STRING("STR_POPUP_SOUND_2_CHANNEL"));	//	"2.1 채널"
	m_pSoundSpeakerPopup->SetEnable(FALSE);

	if( GetCodePage() == I3_LANG_CODE_KOREAN)
	{
		m_pSoundVoicePopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pSoundVoicePopup->AddElement(GAME_STRING("STR_POPUP_ENGLISH_LANGUAGE"));/*영문*/
		m_pSoundVoicePopup->AddElement(GAME_STRING("STR_POPUP_NATIVE_LANGUAGE"));/*한글*/

	}
	else
	{
#if defined (LOCALE_RUSSIA)
		m_pSoundVoicePopup->CreateTextEx(1,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pSoundVoicePopup->AddElement(GAME_STRING("STR_POPUP_RUSSIA_LANGUAGE"));
#elif defined(LOCALE_TURKEY)
		m_pSoundVoicePopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pSoundVoicePopup->AddElement(GAME_STRING("STR_POPUP_NATIVE_LANGUAGE"));/*터키어*/
		m_pSoundVoicePopup->AddElement(GAME_STRING("STR_POPUP_ENGLISH_LANGUAGE"));/*영문*/
#else 
		m_pSoundVoicePopup->CreateTextEx(1,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
		m_pSoundVoicePopup->AddElement(GAME_STRING("STR_POPUP_ENGLISH_LANGUAGE"));/*영문*/
#endif
	}

	m_pSoundVoicePopup->SetSelColor(100, 100, 100, 100);
	m_pSoundVoicePopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSoundVoicePopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSoundVoicePopup->SetExclusiveEnableControl(TRUE);
	m_pSoundVoicePopup->SetEnable(FALSE);

	// 아직 구현되지 않았습니다.
	m_pSoundSpeakerButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	if( m_bInGame)
	{
		COLOR col;
		i3Color::Set( &col, (UINT8) 180, 180, 180, 100);

		m_pSoundSpeakerButton->setInputDisable( TRUE);

		m_pSoundSpeakerCombo->SetTextColor( &col);

		m_pSoundSpeakerPopup->setInputDisable( TRUE);
		m_pSoundSpeakerPopup->SetTextColor( &col);

		m_pSoundSpeakerKey->SetTextColor( &col);
	}
}

void CPopupOption::_InitControl(void)
{
	m_pControlKeyboardKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pControlReverseKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pControlMouseKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pControlFovKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pControlKeyboardKey->SetTextColor(GetColor(GCT_STATIC));
	m_pControlReverseKey->SetTextColor(GetColor(GCT_STATIC));
	m_pControlMouseKey->SetTextColor(GetColor(GCT_STATIC));
	m_pControlFovKey->SetTextColor(GetColor(GCT_STATIC));
	m_pControlKeyboardKey->SetText(GAME_STRING("STR_POPUP_KEY_SETTING"));/*키 설정*/
	m_pControlReverseKey->SetText(GAME_STRING("STR_POPUP_INVERT_MOUSE"));/*마우스 반전*/
	m_pControlMouseKey->SetText(GAME_STRING("STR_POPUP_MOUSE_SENSITIVITY"));/*마우스 민감도*/
	m_pControlFovKey->SetText(GAME_STRING("STR_POPUP_FIELD_OF_VIEW"));/*시야각*/

	INIT_REPEAT_COMBO(m_pControlMouseGagePrev);
	INIT_REPEAT_COMBO(m_pControlMouseGageNext);
	INIT_REPEAT_COMBO(m_pControlFovPrev);
	INIT_REPEAT_COMBO(m_pControlFovNext);

	m_pControlSelect->SetEnable(FALSE);
	m_pControlFocus->SetEnable(FALSE);

	m_pControlList->CreateTextEx(MAX_KEYMAP_SLOT,  GetDefaultFontName(), 64, GAME_FONT_DEFAULT_SIZE);
	m_pControlList->SetSelColor(0, 0, 0, 0);

	INT32	nIndex	= 0;

	for(UINT32 i = 1; i < GAME_KEY_MAP_COUNT; i++)
	{
		// 해당 키맵을 출력하지 않게 하기 위해선 continue를 해주면 된다.
		if( i == GAME_KEY_MAP_VIDEOCAPTURE && LocaleValue::Enable("EnableVideoCapture") == FALSE ) continue;
		if( i == GAME_KEY_MAP_INGAME_SHOP && LocaleValue::Enable("FieldShop") == FALSE ) continue;

		m_pControlList->AddElement("");
		m_nListKeyIndex[ nIndex ]	= i;

		nIndex++;
	}

	for(UINT32 i = 0; i < MAX_KEYMAP_SLOT; i++)
	{
		m_keymapSlot[i]._pKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_keymapSlot[i]._pValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_keymapSlot[i]._pKey->SetTextColor(GetColor(GCT_STATIC));
		m_keymapSlot[i]._pValue->SetTextColor(GetColor(GCT_STATIC));

		m_keymapSlot[i]._pKey->SetOffsetPos(3.0f, 0.0f);
#if defined(GUI_ADJUST_RUSSIA)
		m_keymapSlot[i]._pKey->ReCreateTextEx(GetDefaultFontName(), FONT_COUNT_DEFAULT, GAME_FONT_DEFAULT_SIZE-1);
#endif
	}

	INIT_CAPTION_BUTTON(m_pControlKeyButton, GAME_STRING("STR_POPUP_MODIFY_KEY"));/*키변경*/
	INIT_CAPTION_BUTTON(m_pControlKeyDelete, GAME_STRING("STR_POPUP_DELETE_KEY"));/*키삭제*/


	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = 4;
	ime._StyleIME = IME_STYLE_NUMERIC_AND_NUMERICAL | IME_STYLE_IGNORE_KEYINPUT_IME_MODE;

	m_pControlMouseValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pControlMouseValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pControlMouseValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pControlMouseValue->SetAlphaNumericOnly( true);

	ime._nTextLimitCount = 3;
	ime._StyleIME = IME_STYLE_NUMERIC_ONLY | IME_STYLE_IGNORE_KEYINPUT_IME_MODE;

	m_pControlFovValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pControlFovValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pControlFovValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pControlFovValue->SetAlphaNumericOnly( true);
}

void CPopupOption::_InitCommunity(void)
{
	m_pCommunityNoteKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCommunityInviteKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCommunityWhisperKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCommunityChatKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCommunityNoteKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCommunityInviteKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCommunityWhisperKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCommunityChatKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCommunityNoteKey->SetText(GAME_STRING("STR_POPUP_RECEIVE_NOTE"));/*쪽지 수신*/
	m_pCommunityInviteKey->SetText(GAME_STRING("STR_POPUP_INVITE_MESSAGE"));/*초대 메시지*/
	m_pCommunityWhisperKey->SetText(GAME_STRING("STR_POPUP_WHISPER"));/*귓속말*/
	m_pCommunityChatKey->SetText(GAME_STRING("STR_POPUP_CHATTING"));/*채팅*/

	INIT_COMBO_EX(m_pCommunityNoteButton);
	INIT_COMBO_EX(m_pCommunityInviteButton);
	INIT_COMBO_EX(m_pCommunityWhisperButton);
	INIT_COMBO_EX(m_pCommunityChatButton);

	m_pCommunityNoteCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCommunityInviteCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCommunityWhisperCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCommunityChatCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCommunityNoteCombo->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCommunityInviteCombo->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCommunityWhisperCombo->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCommunityChatCombo->SetTextColor(GetColor(GCT_DEFAULT));

	m_pCommunityNotePopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pCommunityNotePopup->SetSelColor(100, 100, 100, 100);
	m_pCommunityNotePopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCommunityNotePopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCommunityNotePopup->SetExclusiveEnableControl(TRUE);
	m_pCommunityNotePopup->AddElement(GAME_STRING("STBL_IDX_CHAT_TYPE2"));	// 전체허용
	m_pCommunityNotePopup->AddElement(GAME_STRING("STBL_IDX_CHAT_TYPE3"));	// 전체차단
	//	m_pCommunityNotePopup->AddElement(GAME_STRING("STBL_IDX_CHAT_TYPE4"));	// 친구/클랜허용
	m_pCommunityNotePopup->SetEnable(FALSE);

	m_pCommunityInvitePopup->CreateTextEx(3,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pCommunityInvitePopup->SetSelColor(100, 100, 100, 100);
	m_pCommunityInvitePopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCommunityInvitePopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCommunityInvitePopup->SetExclusiveEnableControl(TRUE);
	m_pCommunityInvitePopup->AddElement(GAME_STRING("STBL_IDX_CHAT_TYPE2"));	// 전체허용
	m_pCommunityInvitePopup->AddElement(GAME_STRING("STBL_IDX_CHAT_TYPE3"));	// 전체차단
	m_pCommunityInvitePopup->AddElement(GAME_STRING("STBL_IDX_CHAT_TYPE4"));	// 친구/클랜허용
	m_pCommunityInvitePopup->SetEnable(FALSE);

	m_pCommunityWhisperPopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pCommunityWhisperPopup->SetSelColor(100, 100, 100, 100);
	m_pCommunityWhisperPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCommunityWhisperPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCommunityWhisperPopup->SetExclusiveEnableControl(TRUE);
	m_pCommunityWhisperPopup->AddElement(GAME_STRING("STBL_IDX_CHAT_TYPE2"));	// 전체허용
	m_pCommunityWhisperPopup->AddElement(GAME_STRING("STBL_IDX_CHAT_TYPE3"));	// 전체차단
	//	m_pCommunityWhisperPopup->AddElement(GAME_STRING("STBL_IDX_CHAT_TYPE4"));	// 친구/클랜허용
	m_pCommunityWhisperPopup->SetEnable(FALSE);

	m_pCommunityChatPopup->CreateTextEx(2,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pCommunityChatPopup->SetSelColor(100, 100, 100, 100);
	m_pCommunityChatPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCommunityChatPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCommunityChatPopup->SetExclusiveEnableControl(TRUE);
	m_pCommunityChatPopup->AddElement(GAME_STRING("STBL_IDX_CHAT_TYPE2"));	// 전체허용
	m_pCommunityChatPopup->AddElement(GAME_STRING("STBL_IDX_CHAT_TYPE3"));	// 전체차단
	//	m_pCommunityChatPopup->AddElement(GAME_STRING("STBL_IDX_CHAT_TYPE1"));	// 아군허용
	m_pCommunityChatPopup->SetEnable(FALSE);

	// 아직 구현 안된것들은 비활성화 합니다.
	{
		//		COLOR col;
		//		i3Color::Set( &col, 110, 110, 110, 190);
		//		m_pCommunityNoteCombo->SetTextColor( &col);
		//		m_pCommunityWhisperCombo->SetTextColor( &col);
		//		m_pCommunityChatCombo->SetTextColor( &col);
		//		m_pCommunityNoteButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		//		m_pCommunityWhisperButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		//		m_pCommunityChatButton->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	}
}

void CPopupOption::_InitMacro(void)
{
	m_pMacroAllKey->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMacroTeamKey->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMacroAllKey->SetTextColor(GetColor(GCT_STATIC));
	m_pMacroTeamKey->SetTextColor(GetColor(GCT_STATIC));
	m_pMacroAllKey->SetText(GAME_STRING("STR_POPUP_MACRO_ALL"));/*전체*/
	m_pMacroTeamKey->SetText(GAME_STRING("STR_POPUP_MACRO_TEAM"));/*팀*/

	m_pMacroKey1->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMacroKey2->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMacroKey3->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMacroKey4->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMacroKey5->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMacroKey1->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMacroKey2->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMacroKey3->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMacroKey4->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMacroKey5->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMacroKey1->SetText("Alt + 1");
	m_pMacroKey2->SetText("Alt + 2");
	m_pMacroKey3->SetText("Alt + 3");
	m_pMacroKey4->SetText("Alt + 4");
	m_pMacroKey5->SetText("Alt + 5");

	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = 58;
	ime._StyleIME = 0;

	m_pMacroValue1->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pMacroValue2->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pMacroValue3->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pMacroValue4->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pMacroValue5->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pMacroValue1->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMacroValue2->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMacroValue3->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMacroValue4->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMacroValue5->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMacroValue1->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMacroValue2->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMacroValue3->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMacroValue4->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMacroValue5->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMacroValue1->SetTextAutoWrap( FALSE);
	m_pMacroValue2->SetTextAutoWrap( FALSE);
	m_pMacroValue3->SetTextAutoWrap( FALSE);
	m_pMacroValue4->SetTextAutoWrap( FALSE);
	m_pMacroValue5->SetTextAutoWrap( FALSE);
	m_pMacroValue1->SetOutCharaProcess( TRUE);
	m_pMacroValue2->SetOutCharaProcess( TRUE);
	m_pMacroValue3->SetOutCharaProcess( TRUE);
	m_pMacroValue4->SetOutCharaProcess( TRUE);
	m_pMacroValue5->SetOutCharaProcess( TRUE);
}

void CPopupOption::_InitVoiceChat(void)
{
	// null
}

void CPopupOption::_InitRating(void)
{
	// 한국 NC 측에서 텍스트 내용을 빼달라고 요청받음.
#if defined( LOCALE_KOREA)
	m_pRatingNotice->SetEnable(FALSE);
	m_pRatingNoticeBG->SetEnable(FALSE);
#else
	m_pRatingNotice->SetText(GAME_STRING("STR_POPUP_RATING_NOTICE"));
#endif

	m_pRatingNotice->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRatingNotice->SetTextColor(GetColor(GCT_DEFAULT));

}

BOOL CPopupOption::_IsChangedOption(void)
{
	if( g_pEnvSet->m_bEnableFrindlyName != m_bEnableFrindlyName || g_pEnvSet->m_bEnableFrindlyMark != m_bEnableFrindlyMark ||
		g_pEnvSet->m_bEnableBlood != m_bEnableBlood || g_pEnvSet->m_bEnableBlood2 != m_bEnableBlood2 || m_nOldCrossHairType != m_nCrossHairType ||
		g_pEnvSet->m_bEnableBGM != m_bEnableBGM || g_pEnvSet->m_bEnableEffect != m_bEnableEffect ||
		g_pEnvSet->m_bEnableRadioMessage != m_bEnableRadioMessage || g_pEnvSet->m_nRadioMessageType != m_nRadioMessageType ||
		g_pEnvSet->m_fVolBGM != m_fVolBGM || g_pEnvSet->m_nHandType != m_nHandType ||
		g_pEnvSet->m_fVolEffect != m_fVolEffect || g_pEnvSet->m_bEnableMouseInvert != m_bEnableMouseInvert ||
		g_pEnvSet->m_rMouseSensivity != m_rMouseSensivity || g_pEnvSet->m_fFovValue != m_fFovValue ||
		g_pEnvSet->m_nChatType != m_nChatType || g_pEnvSet->m_nWhisperType != m_nWhisperType ||
		g_pEnvSet->m_nInviteType != m_nInviteType || g_pEnvSet->m_nFriendType != m_nFriendType ||
		g_pEnvSet->m_nMessageType != m_nMessageType || g_pEnvSet->m_nClanType != m_nClanType ||
		g_pEnvSet->m_bCrossHairEffect != m_bCrossHairEffect ||
		g_pEnvSet->m_bEnableFrindlyName != m_bEnableFrindlyName ||
		g_pEnvSet->m_bEnableFrindlyMark != m_bEnableFrindlyMark || g_pEnvSet->m_bTextEffect != m_bEnableTextEffect ||
		g_pEnvSet->m_bChallange != m_bEnableChallenge || g_pEnvSet->m_b1VS1 != m_bEnable1VS1 || g_pEnvSet->m_bMiniMapMode != m_bMinimapType ||
		g_pEnvSet->m_bFullScreen != m_bFullScreen || g_pEnvSet->m_fGammaVal != m_GammaVal || g_pEnvSet->m_bEnableTerrainEffect != m_bEnableTerrainEffect ||
		g_pEnvSet->m_bEnableBulletTrace != m_bEnableBulletTrace || g_pEnvSet->m_bEnableBulletSmoke != m_bEnableBulletSmoke ||
		g_pEnvSet->m_bDynamicLight != m_bDynamicLight || g_pEnvSet->m_bEnableBloom != m_bEnableBloom || g_pEnvSet->m_bEnablePhysX != m_bEnablePhysX || g_pEnvSet->m_nEffectQuality != m_nEffectQuality ||
		g_pEnvSet->m_bMacroEnableFriend[0] != m_bMacroEnableFriend[0] || g_pEnvSet->m_bMacroEnableFriend[1] != m_bMacroEnableFriend[1] ||
		g_pEnvSet->m_bMacroEnableFriend[2] != m_bMacroEnableFriend[2] || g_pEnvSet->m_bMacroEnableFriend[3] != m_bMacroEnableFriend[3] ||
		g_pEnvSet->m_bMacroEnableFriend[4] != m_bMacroEnableFriend[4] ||
		i3String::CompareCase( g_pEnvSet->m_szMacro[0], m_pMacroValue1->getIMETextBuf()) != 0 || i3String::CompareCase( g_pEnvSet->m_szMacro[1], m_pMacroValue2->getIMETextBuf()) != 0 ||
		i3String::CompareCase( g_pEnvSet->m_szMacro[2], m_pMacroValue3->getIMETextBuf()) != 0 || i3String::CompareCase( g_pEnvSet->m_szMacro[3], m_pMacroValue4->getIMETextBuf()) != 0 ||
		i3String::CompareCase( g_pEnvSet->m_szMacro[4], m_pMacroValue5->getIMETextBuf()) != 0
		)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CPopupOption::_IsInputingIme(void)
{
	if (m_pSoundBgmValue->getIsFocus())	return TRUE;
	if (m_pSoundEffectValue->getIsFocus()) return TRUE;
	if (m_pVideoGammaNumber->getIsFocus()) return TRUE;
	if (m_pControlMouseValue->getIsFocus()) return TRUE;
	if (m_pControlFovValue->getIsFocus()) return TRUE;

	return FALSE;
}

void CPopupOption::OptionInitLoad(void)
{
	INT32 i = 0;

	if( g_pEnvSet)
	{
		// GamePlay 관련 변수들
		m_bEnableBlood =		g_pEnvSet->m_bEnableBlood;
		m_bEnableBlood2 =		g_pEnvSet->m_bEnableBlood2;
		m_bCrossHairEffect =	g_pEnvSet->m_bCrossHairEffect;
		m_nHandType =			g_pEnvSet->m_nHandType;
		
		if( m_bEnableBlood ) m_nBloodType = 0;
		else if( m_bEnableBlood2 ) m_nBloodType = 1;

		m_nOldBloodType = m_nBloodType;

		if( i3String::Compare( g_pEnvSet->m_szCrosshairName, "Crosshair_Cross") == 0)
			m_nCrossHairType = 1;
		else if( i3String::Compare( g_pEnvSet->m_szCrosshairName, "Crosshair_CrossDot") == 0)
			m_nCrossHairType = 0;
		else if( i3String::Compare( g_pEnvSet->m_szCrosshairName, "Crosshair_Dot") == 0)
			m_nCrossHairType = 3;
		else if( i3String::Compare( g_pEnvSet->m_szCrosshairName, "Crosshair_Ring") == 0)
			m_nCrossHairType = 2;
		else
			m_nCrossHairType = 0;

		m_nOldCrossHairType = m_nCrossHairType;

		// Video관련 변수들
		m_bFullScreen =			g_pEnvSet->m_bFullScreen;
		m_nScreenWidth =		g_pEnvSet->m_nScreenWidth;
		m_nScreenHeight =		g_pEnvSet->m_nScreenHeight;
		m_nScreenHz	=			g_pEnvSet->m_nScreenHz;
		m_ScreenDepth =			g_pEnvSet->m_ScreenDepth;
		m_AA				=	g_pEnvSet->m_AA;
		m_AAQ				=	g_pEnvSet->m_AAQ;
		m_bVSync			=	g_pEnvSet->m_bVSync;
		m_bTriLinearFilter	=	g_pEnvSet->m_bTriLinearFilter;
		m_GammaVal =			g_pEnvSet->m_fGammaVal;
		m_nVideoResolution	=	g_pEnvSet->m_nVideoResolution;

		// Interface관련 변수들
		m_bEnableFrindlyName =	g_pEnvSet->m_bEnableFrindlyName;
		m_bEnableFrindlyMark =	g_pEnvSet->m_bEnableFrindlyMark;
		m_bEnableTextEffect	 =  g_pEnvSet->m_bTextEffect;
		m_bEnableChallenge	 =  g_pEnvSet->m_bChallange;
		m_bEnable1VS1		 =	g_pEnvSet->m_b1VS1;
		m_bMinimapType		 =  g_pEnvSet->m_bMiniMapMode;

		// Sound관련 변수들
		m_bEnableBGM =			g_pEnvSet->m_bEnableBGM;
		//m_bEnableBGM =			FALSE;								// dummy
		m_bEnableEffect =		g_pEnvSet->m_bEnableEffect;
		m_bEnableRadioMessage = g_pEnvSet->m_bEnableRadioMessage;
		m_nRadioMessageType =	g_pEnvSet->m_nRadioMessageType;
		m_fVolBGM =				g_pEnvSet->m_fVolBGM;
		m_fVolEffect =			g_pEnvSet->m_fVolEffect;
		m_nSpeakerType =		g_pEnvSet->m_nSpeakerType;

		// Control관련 변수들
		i3mem::Copy(m_KeyMapArray, g_pEnvSet->m_KeyMapArray,  sizeof(GAME_KEY_STRUCT)*GAME_KEY_MAP_COUNT);
		m_bEnableMouseInvert =	g_pEnvSet->m_bEnableMouseInvert;
		m_rMouseSensivity =		g_pEnvSet->m_rMouseSensivity;
		m_fFovValue =			g_pEnvSet->m_fFovValue;

		// Performance관련 변수들
		m_bEnableBulletTrace =	g_pEnvSet->m_bEnableBulletTrace;
		m_bEnableBulletSmoke =  g_pEnvSet->m_bEnableBulletSmoke;
		m_nShadowQualityType =	g_pEnvSet->m_nShadowQualityType;
		m_bDynamicLight =		g_pEnvSet->m_bDynamicLight;
		m_bEnableNormalMap =	g_pEnvSet->m_bEnableNormalMap;
		m_bEnableTerrainEffect = g_pEnvSet->m_bEnableTerrainEffect;
		m_nTextureQualityType =	g_pEnvSet->m_nTextureQualityType;
		m_b16BitsDepth		=	g_pEnvSet->m_b16BitsDepth;
		m_bEnableBloom		=	g_pEnvSet->m_bEnableBloom;
		m_bEnablePhysX		=	g_pEnvSet->m_bEnablePhysX;
		m_nEffectQuality	=	g_pEnvSet->m_nEffectQuality;
		m_nSpecularQualityType = g_pEnvSet->m_nSpecularQualityType;

		// Commnuity관련 변수들
		m_nChatType =			g_pEnvSet->m_nChatType;
		m_nWhisperType =		g_pEnvSet->m_nWhisperType;
		m_nInviteType =			g_pEnvSet->m_nInviteType;
		m_nFriendType =			g_pEnvSet->m_nFriendType;
		m_nMessageType =		g_pEnvSet->m_nMessageType;
		m_nClanType =			g_pEnvSet->m_nClanType;

		// Macro관련 변수들
		for(i=0; i<5; i++)
		{
			i3String::Copy( m_szMacro[i], g_pEnvSet->m_szMacro[i], MACRO_MAX_LENGHT );
			m_bMacroEnableFriend[i] = g_pEnvSet->m_bMacroEnableFriend[i];
		}
	}
}

void CPopupOption::UpdateInit(void)
{
	// 저장되어있는 값들로 변수들을 설정합니다.
	OptionInitLoad();

	// 공통 컨트롤 초기화에 성공하면 다른 모든 컨트롤을 초기화합니다.
	_ResetGamePlay();
	_ResetVideo();
	_ResetInterface();
	_ResetSound();
	_ResetControl();
	_ResetPerformance();
	_ResetCommunity();
	_ResetMacro();
	_ResetVoiceChat();
}

void CPopupOption::OptionSave(void)
{
	INT32 i = 0;


#if defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)
	if( !g_pConfig->m_bNetworking)
	{
		/*서버에 저장되는 정보가 있을경우, 오류가 있을 수 있으므로 저장하지 않습니다.\n게임에 로그인후 사용해 주세요.*/
		UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_CANNOT_SAVE_INFO"));
		RePositionMsgBox( pBox);
		return;
	}
#endif

	g_pGameContext->SetNetEnvSetBasicChange( 0);

	if( m_bOptionInit)
	{
		m_KeyMapArray[GAME_KEY_MAP_VERSION]._nState= GAME_KEY_VERSION_CUR;

		g_pGameContext->SetNetEnvSetBasicChange( g_pGameContext->GetNetEnvSetBasicChange() | 0x10000000);
		g_pGameContext->SetNetEnvSetBasicChange( g_pGameContext->GetNetEnvSetBasicChange() | 0x00000002);
		m_bOptionInit = FALSE;
	}

	if (_IsChangedOption())
	{
		g_pGameContext->SetNetEnvSetBasicChange( g_pGameContext->GetNetEnvSetBasicChange() | 0x00000001);
	}

	// GamePlay 관련 변수들
	g_pEnvSet->m_bEnableFrindlyName =		m_bEnableFrindlyName;
	g_pEnvSet->m_bEnableFrindlyMark =		m_bEnableFrindlyMark;
	g_pEnvSet->m_bEnableBlood =				m_bEnableBlood;
	g_pEnvSet->m_bEnableBlood2 =			m_bEnableBlood2;
	g_pEnvSet->m_bCrossHairEffect =			m_bCrossHairEffect;
	g_pEnvSet->m_nHandType =				m_nHandType;

	switch( m_nCrossHairType)
	{
	case 1 :	i3String::Copy( g_pEnvSet->m_szCrosshairName, "Crosshair_Cross",	256 );	break;
	case 2 :	i3String::Copy( g_pEnvSet->m_szCrosshairName, "Crosshair_Ring",		256 );	break;
	case 3 :	i3String::Copy( g_pEnvSet->m_szCrosshairName, "Crosshair_Dot",		256 );	break;
	default :	i3String::Copy( g_pEnvSet->m_szCrosshairName, "Crosshair_CrossDot",	256 );	break;
	}

	// Video관련 변수들
	g_pEnvSet->m_bFullScreen =				m_bFullScreen;
	g_pEnvSet->m_nScreenWidth =				m_nScreenWidth;
	g_pEnvSet->m_nScreenHeight =			m_nScreenHeight;
	g_pEnvSet->m_nScreenHz	=				m_nScreenHz;
	g_pEnvSet->m_ScreenDepth =				m_ScreenDepth;
	g_pEnvSet->m_AA =						m_AA;
	g_pEnvSet->m_AAQ =						m_AAQ;
	g_pEnvSet->m_bVSync	=					m_bVSync;
	g_pEnvSet->m_bTriLinearFilter =			m_bTriLinearFilter;
	g_pEnvSet->m_nVideoResolution =			m_nVideoResolution;
	g_pEnvSet->m_fGammaVal =				m_GammaVal;

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

	// Interface관련 변수들
	g_pEnvSet->m_bEnableFrindlyName = m_bEnableFrindlyName;
	g_pEnvSet->m_bEnableFrindlyMark = m_bEnableFrindlyMark;
	g_pEnvSet->m_bTextEffect = m_bEnableTextEffect;
	g_pEnvSet->m_bChallange = m_bEnableChallenge;
	g_pEnvSet->m_b1VS1 = m_bEnable1VS1;
	g_pEnvSet->m_bMiniMapMode = m_bMinimapType;

	// Sound관련 변수들
	g_pEnvSet->m_bEnableBGM =				m_bEnableBGM;
	g_pEnvSet->m_bEnableEffect =			m_bEnableEffect;
	g_pEnvSet->m_bEnableRadioMessage =		m_bEnableRadioMessage;
	g_pEnvSet->m_nRadioMessageType =		m_nRadioMessageType;
	g_pEnvSet->m_fVolBGM =					m_fVolBGM;
	g_pEnvSet->m_fVolEffect =				m_fVolEffect;
	g_pEnvSet->m_nSpeakerType =				m_nSpeakerType;

	// Control관련 변수들
	g_pEnvSet->m_bEnableMouseInvert =		m_bEnableMouseInvert;
	g_pEnvSet->m_rMouseSensivity =			m_rMouseSensivity;
	g_pEnvSet->m_fFovValue =				m_fFovValue;

	// Performance관련 변수
	g_pEnvSet->m_bEnableBulletTrace =		m_bEnableBulletTrace;
	g_pEnvSet->m_bEnableBulletSmoke =		m_bEnableBulletSmoke;
	g_pEnvSet->m_nShadowQualityType =		m_nShadowQualityType;
	g_pEnvSet->m_bDynamicLight =			m_bDynamicLight;
	g_pEnvSet->m_bEnableNormalMap =			m_bEnableNormalMap;
	g_pEnvSet->m_bEnableTerrainEffect =		m_bEnableTerrainEffect;
	g_pEnvSet->m_nTextureQualityType =		m_nTextureQualityType;
	g_pEnvSet->m_b16BitsDepth =				m_b16BitsDepth;
	g_pEnvSet->m_bEnableBloom =				m_bEnableBloom;
	g_pEnvSet->m_bEnablePhysX =				m_bEnablePhysX;
	g_pEnvSet->m_nEffectQuality =			m_nEffectQuality;
	g_pEnvSet->m_nSpecularQualityType =		m_nSpecularQualityType;

	switch( m_nSpecularQualityType)
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

	// Commnuity관련 변수들
	g_pEnvSet->m_nChatType		= m_nChatType;
	g_pEnvSet->m_nWhisperType	= m_nWhisperType;
	g_pEnvSet->m_nInviteType	= m_nInviteType;
	g_pEnvSet->m_nFriendType	= m_nFriendType;
	g_pEnvSet->m_nMessageType	= m_nMessageType;
	g_pEnvSet->m_nClanType		= m_nClanType;

	//// Macro관련 변수들
	i3GuiEditBox* pMacro = NULL;
	for(i=0; i<5; i++)
	{
		switch( i)
		{
		case 0: pMacro = m_pMacroValue1;	break;
		case 1: pMacro = m_pMacroValue2;	break;
		case 2: pMacro = m_pMacroValue3;	break;
		case 3: pMacro = m_pMacroValue4;	break;
		case 4: pMacro = m_pMacroValue5;	break;
		}


		GameGUI::RemoveColorKeyword(pMacro->getIMETextBuf(), m_szMacro[i], MACRO_MAX_LENGHT);


		if( i3String::CompareCase( g_pGameContext->GetMacro(i), m_szMacro[i]) != 0)
		{
			// GameContext에 변화되어진 Macro의 Index와 셋팅을 알려줍니다.
			UINT32 NetEnvSetBasicChange = g_pGameContext->GetNetEnvSetBasicChange();

			NetEnvSetBasicChange |= 0x0100 << i;
			NetEnvSetBasicChange |= 0x00000004;

			g_pGameContext->SetNetEnvSetBasicChange( NetEnvSetBasicChange);
		}

		i3String::Copy( g_pEnvSet->m_szMacro[i], m_szMacro[i], MACRO_MAX_LENGHT );

		if( g_pEnvSet->m_bMacroEnableFriend[i] != m_bMacroEnableFriend[i])
		{
			g_pGameContext->SetNetEnvSetBasicChange( g_pGameContext->GetNetEnvSetBasicChange() | 0x00000001);
			g_pEnvSet->m_bMacroEnableFriend[i] = m_bMacroEnableFriend[i];
		}
	}

	SetNewControlSetting();

	// 설정되어진 값을 옵션파일에 저장합니다.
	g_pEnvSet->Save();
	g_pEnvSet->Net_Save();

	if( g_pGameContext->GetEnvSetDefault() == 1)
	{
		// 최초 입장 유저입니다.
		UINT32 nChange = g_pGameContext->GetNetEnvSetBasicChange();

		nChange |= 0x00000001;
		nChange |= 0x00000002;
		nChange |= 0x00000004;

		g_pGameContext->SetNetEnvSetBasicChange( nChange);

		g_pGameContext->SetEnvSetDefault( 0);
	}

	g_pGameContext->SetEvent( EVENT_OPTION_SAVE_N );

#if defined( LOCALE_CHINA)
	if( m_nOldBloodType != m_nBloodType )
	{
		g_pEffectManager->NeetToInitAgeLimitEffect();
	}
#endif

	// In Game Option일 경우 변경된 값들을 게임에 바로 적용토록합니다.
	if( m_bInGame)	InGameOptionApply();
}

void CPopupOption::InGameOptionApply(void)
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	// 이 함수에서는 기존에 인게임 안에서 바로 적용되지 않고 누락된 것들에 대한 처리만 합니다.
	////////////////////////////////////////////////////////////////////////////////////////////////
	CStageBattle * pStage = g_pFramework->GetStageBattle();

	if( pStage != NULL)
	{
		//Camera FOV 적용.
		//무기 줌 상태가 아닌 경우만 적용 된다.	2011년 3월 23일 
		//이정우
//		if( g_pCameraManager->GetCurrentCamera()->getZoomRatio() == 1.0f )
//			g_pCameraManager->ApplyEnvSetFOV();
		if( CGameCameraManager::i()->GetCurrentCamera()->getZoomRatio() == 1.0f )
			CGameCameraManager::i()->ApplyEnvSetFOV();


		// Cross Hair 적용. 2012.01.02. 수빈.  옵션창에서 크로스헤어 변경시 적용 테스트를 한번은 거쳐야함. 
		CrosshairMgr::i()->ApplyEnvSetCrosshair();
		//pStage->ApplyEnvSetCrossHair();
		
		// 임무 알림창
		CHud::i()->SetEnableChallengeBoard( g_pEnvSet->m_bChallange);
		CHud::i()->ChangeChallenge();
		// TeamTag
		if( g_pCharaManager != NULL)
		{
			for( INT32 i=0; i<g_pCharaManager->getCharaCount(); i++)
			{
				CGameCharaBase * pChara = g_pCharaManager->getChara( i);

				if( pChara == NULL)
					continue;
				pChara->Cmd_Enable_TeamTag( (g_pEnvSet->m_bEnableFrindlyMark==TRUE));
			}
		}

#if defined( LOCALE_CHINA)
		if( m_nOldBloodType != m_nBloodType )
		{
			g_pEffectManager->NeetToInitAgeLimitEffect();

			g_pEffectManager->InitAgeLimitEffect();

			char filename[MAX_PATH] = "";
			if( g_pEnvSet->m_bEnableBlood2 )
				i3String::Copy( filename, "Gui/Ingame/blood_HitEffect.i3i", MAX_PATH );
			else
				i3String::Copy( filename, "Gui/Ingame/blood_green.i3i", MAX_PATH );

			CStageBattle::ChangeHudTexture(HUD_TEXTURE_DAMAGE_FILTER, filename);
			CHud::i()->SetDamageFilterInGame( 
				CStageBattle::LoadHudTexture(HUD_TEXTURE_DAMAGE_FILTER) );

		}
#endif
	}
}

void CPopupOption::OnUpdate( REAL32 rDeltaSeconds )
{
	CPopupBase::OnUpdate(rDeltaSeconds);

	_SetVideoOptionEnable();

	{
		m_fElapsedTime = rDeltaSeconds;

		UINT32 nState = g_pFramework->getMouse()->GetButtonState();

		if( (nState & I3I_MOUSE_LBUTTON) == 0)
		{
			m_fDelayLocalTime = 0.0f;
		}
	}

	if( m_bOldFullScreen != g_pEnvSet->m_bFullScreen)
	{
		_SetVideoMode();
		m_bOldFullScreen = g_pEnvSet->m_bFullScreen;
	}
	
	
	switch(m_currentSect)
	{
	case OS_PLAY:		UpdateGamePlay( rDeltaSeconds );		break;
	case OS_VIDEO:		UpdateVideo( rDeltaSeconds );			break;
	case OS_INTERFACE:	UpdateInterface(rDeltaSeconds);			break;
	case OS_SOUND:		UpdateSound( rDeltaSeconds );			break;
	case OS_CONTROL:	UpdateControl( rDeltaSeconds );			break;
	case OS_CAPACITY:	UpdatePerformance( rDeltaSeconds );		break;
	case OS_COMMUNITY:	UpdateCommunity( rDeltaSeconds );		break;
	case OS_MACRO:		UpdateMacro( rDeltaSeconds );			break;
	case OS_VOICE:		UpdateVoiceChat( rDeltaSeconds );		break;
	default:		break;
	}
}

void CPopupOption::OnPreGuiNotify(void)
{
	CPopupBase::OnPreGuiNotify();

	m_pControlFocus->SetEnable(FALSE);

	INT32 idxSlot = m_nSelectedIdx - m_pControlList->GetCurrentTop();

	if(idxSlot < 0 || idxSlot >= MAX_KEYMAP_SLOT)
	{
		m_pControlSelect->SetEnable(FALSE);
	}
}

void CPopupOption::OnExitKey(void)
{
	OnClosePopup();
}

void CPopupOption::OnWheelUpdate(INT32 scrollby)
{
	if (OS_CONTROL == m_currentSect)
	{
		if (0 < scrollby)
		{
			for(INT32 i = 0; i < scrollby; i++)
			{
				m_pControlList->MovePrev();
			}
		}
		else
		{
			for(INT32 i = 0; i > scrollby; i--)
			{
				m_pControlList->MoveNext();
			}
		}
	}

	UINT32 idxSlot = m_nSelectedIdx - m_pControlList->GetCurrentTop();

	if (0 <= idxSlot && idxSlot < MAX_KEYMAP_SLOT)
	{
		m_pControlSelect->setPositionY(idxSlot * 24.0f + 2.0f);
		m_pControlSelect->SetEnable(TRUE);
	}
}

void CPopupOption::_ResetGamePlay(void)
{
#if defined( LOCALE_CHINA)
	_SetGamePlayBloodType(m_nBloodType);
#else
	// 혈흔
	m_pPlayBlood->setCheckBoxState(m_bEnableBlood);
#endif

	// CrossHair 타격 이펙트
	m_pPlayHit->setCheckBoxState(m_bCrossHairEffect);

	// CrossHair
	_SetGamePlayCrossHairType(m_nCrossHairType);

	switch( m_nHandType)
	{
	case 0:
		m_pPlayHandTypeCombo->SetText(GAME_STRING("STR_POPUP_RIGHT_HANDED"));/*오른손잡이*/
		break;
	case 1:
		m_pPlayHandTypeCombo->SetText(GAME_STRING("STR_POPUP_LEFT_HANDED"));/*왼손잡이*/
		break;
	default:
		m_pPlayHandTypeCombo->SetText(GAME_STRING("STR_POPUP_RIGHT_HANDED"));/*오른손잡이*/
		break;
	}
}

void CPopupOption::_ResetSound(void)
{
	// 배경음
	m_pSoundBgm->setCheckBoxState(m_bEnableBGM);

	{
		char strValue[MAX_STRING_COUNT];

		sprintf_s(strValue, "%d", (INT32)m_fVolBGM);
		m_pSoundBgmValue->SetIMEText(strValue);
	}

	if( m_bEnableBGM)
	{
		m_pSoundBgmPrev->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pSoundBgmNext->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pSoundBgmValue->setInputDisable(FALSE);
	}
	else
	{
		m_pSoundBgmPrev->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundBgmNext->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundBgmValue->setInputDisable(TRUE);
	}

	// 효과음
	m_pSoundEffect->setCheckBoxState( m_bEnableEffect);

	{
		char strValue[MAX_STRING_COUNT];

		sprintf_s(strValue, "%d", (INT32)m_fVolEffect);
		m_pSoundEffectValue->SetIMEText(strValue);
	}

	if( m_bEnableEffect)
	{
		m_pSoundEffectPrev->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pSoundEffectNext->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pSoundEffectValue->setInputDisable(FALSE);
	}
	else
	{
		m_pSoundEffectPrev->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundEffectNext->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundEffectValue->setInputDisable(TRUE);
	}

	m_pSoundVoice->setCheckBoxState(m_bEnableRadioMessage);

	SetSoundSetting();

	_SetSoundSpeaker();
	_SetRadioMessage();

	i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
	if( pSndCtx == NULL)
	{
		// 배경음
		m_pSoundBgm->setInputDisable(TRUE);
		m_pSoundEffect->setInputDisable(TRUE);
		m_pSoundBgmValue->setInputDisable(TRUE);
		m_pSoundEffectValue->setInputDisable(TRUE);
		m_pSoundBgmPrev->setInputDisable(TRUE);
		m_pSoundBgmNext->setInputDisable(TRUE);
		m_pSoundEffectPrev->setInputDisable(TRUE);
		m_pSoundEffectNext->setInputDisable(TRUE);

		m_pSoundBgm->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundEffect->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundBgmPrev->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundBgmNext->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundEffectPrev->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundEffectNext->setControlState(I3GUI_CONTROL_STATE_DISABLED);

		return;
	}
}

void CPopupOption::_ResetControl(void)
{
	// 컨트롤 선택 변수 초기화
	m_nSelectedKeyMapIdx = 0;
	m_nCurKeyMapIdx = 0;
	m_nSelectedIdx = -1;
	m_nDuplicateIdx = -1;

	// 컨트롤 선택 테두리 초기화
	m_pControlSelect->SetEnable( FALSE);

	// 컨트롤 리스트 초기화
	m_pControlList->SetCurValue(0.0f);

	// 마우스 반전
	m_pControlReverse->setCheckBoxState(m_bEnableMouseInvert);

	// 마우스 감도
	{
		char szTemp[MAX_STRING_COUNT];

		sprintf_s( szTemp, "%.2f", m_rMouseSensivity);
		m_pControlMouseValue->SetIMEText( szTemp);
	}

	// FOV
	{
		char szTemp[MAX_STRING_COUNT];

		sprintf_s( szTemp, "%d", (INT32) m_fFovValue);
		m_pControlFovValue->SetIMEText( szTemp);
	}

	// Key Map Init
	_InitKeyboardSetting();
}

void CPopupOption::_ResetCommunity(void)
{
	// 채팅
	_SetCommunityChat(m_nChatType);

	// 귓속말
	_SetCommunityWhisper(m_nWhisperType);

	// 초대 메시지
	_SetCommunityInvite(m_nInviteType);

	// 쪽지 수신
	_SetCommunityMessage(m_nMessageType);
}

void CPopupOption::_ResetMacro(void)
{
	m_pMacroValue1->SetIMEText(m_szMacro[0]);
	m_pMacroValue2->SetIMEText(m_szMacro[1]);
	m_pMacroValue3->SetIMEText(m_szMacro[2]);
	m_pMacroValue4->SetIMEText(m_szMacro[3]);
	m_pMacroValue5->SetIMEText(m_szMacro[4]);

	m_pMacroAll1->SetAllShapeEnable(FALSE);
	m_pMacroAll2->SetAllShapeEnable(FALSE);
	m_pMacroAll3->SetAllShapeEnable(FALSE);
	m_pMacroAll4->SetAllShapeEnable(FALSE);
	m_pMacroAll5->SetAllShapeEnable(FALSE);
	m_pMacroTeam1->SetAllShapeEnable(FALSE);
	m_pMacroTeam2->SetAllShapeEnable(FALSE);
	m_pMacroTeam3->SetAllShapeEnable(FALSE);
	m_pMacroTeam4->SetAllShapeEnable(FALSE);
	m_pMacroTeam5->SetAllShapeEnable(FALSE);


	UINT32 nAll[5] = {0, };
	UINT32 nTeam[5] = {0, };

	for(UINT32 i = 0; i < 5; i++)
	{
		if (m_bMacroEnableFriend[i])
		{
			nTeam[i] = 2;
		}
		else
		{
			nAll[i] = 2;
		}
	}

	m_pMacroAll1->SetShapeEnable(GUI_SHAPE_OFFSET + nAll[0], TRUE);
	m_pMacroAll2->SetShapeEnable(GUI_SHAPE_OFFSET + nAll[1], TRUE);
	m_pMacroAll3->SetShapeEnable(GUI_SHAPE_OFFSET + nAll[2], TRUE);
	m_pMacroAll4->SetShapeEnable(GUI_SHAPE_OFFSET + nAll[3], TRUE);
	m_pMacroAll5->SetShapeEnable(GUI_SHAPE_OFFSET + nAll[4], TRUE);
	m_pMacroTeam1->SetShapeEnable(GUI_SHAPE_OFFSET + nTeam[0], TRUE);
	m_pMacroTeam2->SetShapeEnable(GUI_SHAPE_OFFSET + nTeam[1], TRUE);
	m_pMacroTeam3->SetShapeEnable(GUI_SHAPE_OFFSET + nTeam[2], TRUE);
	m_pMacroTeam4->SetShapeEnable(GUI_SHAPE_OFFSET + nTeam[3], TRUE);
	m_pMacroTeam5->SetShapeEnable(GUI_SHAPE_OFFSET + nTeam[4], TRUE);
}

void CPopupOption::_ResetVoiceChat(void)
{
	// null
}

void CPopupOption::OnOptionConfirm(void)
{
	m_bEnableConfirm = TRUE;

	INT32 i = 0;
	BOOL bNotReady = FALSE;

	// 게임키중에 아직 정의 되지 않은것이 있는지 체크합니다.
	for( i=1; i<GAME_KEY_MAP_COUNT; i++)
	{
		// 해당 옵션을 쓰지 않을경우 체크하지 않음
		if( i == GAME_KEY_MAP_VIDEOCAPTURE && LocaleValue::Enable("EnableVideoCapture") == FALSE ) continue;
		if( i == GAME_KEY_MAP_INGAME_SHOP && LocaleValue::Enable("FieldShop") == FALSE ) continue;

		if( m_KeyMapArray[i]._nState == I3I_KEY_UNKNOWN ||
			(m_KeyMapArray[i]._nDiviceType == 1 && m_KeyMapArray[i]._nState == 0) )
		{
			bNotReady = TRUE;
			break;
		}
	}

	// 정의되지 않은것이 있다면...
	if( bNotReady)
	{
		UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_STRING("STBL_IDX_EP_OPTION_NOTDEFINEKEY"), NULL, MAKE_CALLBACK(CbConfirm), this);
		RePositionMsgBox(pBox);
	}
	else
	{
		INT32 keyMapCompare = memcmp(g_pEnvSet->m_KeyMapArray, m_KeyMapArray, sizeof(GAME_KEY_STRUCT) * GAME_KEY_MAP_COUNT);

		// 변경된것이 있을때는 확인을 받는다.
		if (_NeedToUserVerify() || _IsChangedOption() || 0 != keyMapCompare)
		{
			UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_STRING("STBL_IDX_EP_OPTION_CONFIRM"), NULL, MAKE_CALLBACK(CbConfirm), this);
			RePositionMsgBox( pBox);
		}
		else
		{
			OnClosePopup();
			m_bEnableConfirm = FALSE;
		}
	}
}

void CPopupOption::OnOptionCancel(void)
{
	OnClosePopup();
}

void CPopupOption::OnOptionInitial(void)
{
	UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_STRING("STBL_IDX_EP_OPTION_INITIALIZE"), NULL, MAKE_CALLBACK(CbInit), this);
	RePositionMsgBox( pBox);
}

void CPopupOption::OnOptionOptimize(void)
{
	UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_STRING("STBL_IDX_EP_OPTION_OPTIMIZE"), NULL, MAKE_CALLBACK(CbOptimize), this);
	RePositionMsgBox( pBox);
}

void CPopupOption::CbConfirm(void* pThis,INT32 nParam)
{
	CPopupOption * pDlg = (CPopupOption *) pThis;

	//if( nParam == MBR_OK || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
	if( nParam == MBR_OK || pDlg->InputKeyboardEnter())
	{		
		// Shader 설정에 영향을 미치는 옵션이 변경되었다면...
		if( pDlg->m_bNeedToValidate)
		{
			if( pDlg->_ValidateShader() == false)
			{
				return;
			}
		}

		{
			// 해상도 등이 변경되었을 때, 직접 해상도를 변경한 후, 화면에 올바르게
			// 보이고 있는지를 사용자의 입력을 통해 확인받아야 한다.
			if( pDlg->_NeedToUserVerify())
			{
				pDlg->_PopupUserVerify();
			}
			else
			{
				// 사용자의 확인 불필요한 변경이라면 그냥 설정하고 나감.
				((CPopupOption*)pThis)->OptionSave();
				((CPopupOption*)pThis)->OnClosePopup();
				((CPopupOption*)pThis)->m_bEnableConfirm = FALSE;
			}
		}

		// In Game Option Disable
		if( pDlg->m_bInGame)	((CBattleGui*)pDlg->getParent())->SetEnablePopup();
	}
	else if( nParam == MBR_CANCEL || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ESC ))
	{
		((CPopupOption*)pThis)->m_bEnableConfirm = FALSE;
	}
}

void CPopupOption::CbInit(void* pThis,INT32 nParam)
{
	BOOL bSetVedio = FALSE;

	if( nParam == MBR_OK)
	{
		// 해상도를 변경합니다.
		if( ((CPopupOption*)pThis)->m_nScreenWidth != 1024 && ((CPopupOption*)pThis)->m_nScreenHeight != 768)
		{
			// 0을 기본해상도인것으로 합니다.
			if (((CPopupOption*)pThis)->m_pVideoResolutionPopup->GetElementCount() > 0)
			{
				((CPopupOption*)pThis)->_SetVideoResolution(0);
				bSetVedio = TRUE;
			}
		}

		// 주사율을 변경합니다.
		if (((CPopupOption*)pThis)->m_pVideoScanRatePopup->GetElementCount() > 0)
		{
			((CPopupOption*)pThis)->OnVideoScanRatePopup( 0);
			bSetVedio = TRUE;
		}

		// 안티앨리어싱을 변경합니다.
		if (((CPopupOption*)pThis)->m_pVideoAntiAliasingPopup->GetElementCount() > 0)
		{
			((CPopupOption*)pThis)->OnVideoAntiAliasingPopup( 0);
			bSetVedio = TRUE;
		}

		// 동영상 해상도 변경
		if (((CPopupOption*)pThis)->m_pVideoCapturePopup->GetElementCount() != 1)
		{
			((CPopupOption*)pThis)->OnVideoCapturePopup(1);
			bSetVedio = TRUE;
		}

		g_pEnvSet->LoadDefaultSave();			//EnvSet의 값 초기화
		((CPopupOption*)pThis)->UpdateInit();	//Env value -> PopupState

		((CPopupOption*)pThis)->m_bOptionInit = TRUE;
		((CPopupOption*)pThis)->OptionSave();	//다시 EnvSet에 저장(위에거가 변경되서?

		if( bSetVedio) ((CPopupOption*)pThis)->_SetVideo( false);


		((CPopupOption*)pThis)->OnClosePopup();

		// In Game Option Disable
		if( ((CPopupOption*)pThis)->m_bInGame)	((CBattleGui*)((CPopupOption*)pThis))->SetEnablePopup();
	}
}


void CPopupOption::CbOptimize(void* pThis,INT32 nParam)
{
	CPopupOption * pDlg = (CPopupOption *) pThis;
	
	//if( nParam == MBR_OK || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
	if( nParam == MBR_OK || pDlg->InputKeyboardEnter())
	{
		pDlg->m_bVSync = FALSE;
		pDlg->m_bDynamicLight = FALSE;
		pDlg->m_bEnableNormalMap = FALSE;
		pDlg->m_nTextureQualityType = 2;
		pDlg->m_nShadowQualityType = 0;
		pDlg->m_nSpecularQualityType = 0;
		pDlg->m_bTriLinearFilter = FALSE;
		pDlg->m_nEffectQuality = 0;
		pDlg->m_AA = 0;
		pDlg->m_AAQ = 0;

		pDlg->_ValidateShader();

		if( pDlg->_NeedToUserVerify())
		{
			// 설정한 해상도를 일단 변경
			pDlg->_SetVideo( false);

			pDlg->OptionSave();
			pDlg->OnClosePopup();
			pDlg->m_bEnableConfirm = FALSE;
		}
		else
		{
			// 사용자의 확인 불필요한 변경이라면 그냥 설정하고 나감.
			((CPopupOption*)pThis)->OptionSave();
			((CPopupOption*)pThis)->OnClosePopup();
			((CPopupOption*)pThis)->m_bEnableConfirm = FALSE;
		}

		((CPopupOption*)pThis)->OnClosePopup();
	}
}

void CPopupOption::CbDissolveImeInput(void* pThis,INT32 nParam)
{
	if (MBR_OK == nParam)
	{
		//i3GuiRoot::setFocusControl(NULL);
	}
}

////////////////////////////////////////////////////////////////
// 게임플레이 환경 설정
////////////////////////////////////////////////////////////////
void CPopupOption::_SetGamePlayBloodType( INT32 nType)
{
	m_nBloodType = nType;

	if( (nType + GCI_OPP_R_PLAY_BLOOD1) == GCI_OPP_R_PLAY_BLOOD1 )
	{
		m_bEnableBlood = TRUE;
		m_bEnableBlood2 = FALSE;
	}
	else if( (nType + GCI_OPP_R_PLAY_BLOOD1) == GCI_OPP_R_PLAY_BLOOD2 )
	{
		m_bEnableBlood = FALSE;
		m_bEnableBlood2 = TRUE;
	}

	m_pPlayBloodRadio1->SetAllShapeEnable(FALSE);
	m_pPlayBloodRadio2->SetAllShapeEnable(FALSE);

	UINT32 idxState[2] = {0, };
	idxState[nType] = 2;

	m_pPlayBloodRadio1->SetShapeEnable(GUI_SHAPE_OFFSET + idxState[0], TRUE);
	m_pPlayBloodRadio2->SetShapeEnable(GUI_SHAPE_OFFSET + idxState[1], TRUE);
}

void CPopupOption::_SetGamePlayBlood(void)
{
	m_bEnableBlood = m_pPlayBlood->getCheckBoxState();
}

void CPopupOption::_SetGamePlayCrossHairEffect(void)
{
	m_bCrossHairEffect = m_pPlayHit->getCheckBoxState();
}

void CPopupOption::_SetGamePlayCrossHairType( INT32 nType)
{
	m_nCrossHairType = nType;

	m_pPlayCrosshair1->SetAllShapeEnable(FALSE);
	m_pPlayCrosshair2->SetAllShapeEnable(FALSE);
	m_pPlayCrosshair3->SetAllShapeEnable(FALSE);
	m_pPlayCrosshair4->SetAllShapeEnable(FALSE);

	UINT32 idxState[4] = {0, };
	idxState[nType] = 2;

	m_pPlayCrosshair1->SetShapeEnable(GUI_SHAPE_OFFSET + idxState[0], TRUE);
	m_pPlayCrosshair2->SetShapeEnable(GUI_SHAPE_OFFSET + idxState[1], TRUE);
	m_pPlayCrosshair3->SetShapeEnable(GUI_SHAPE_OFFSET + idxState[2], TRUE);
	m_pPlayCrosshair4->SetShapeEnable(GUI_SHAPE_OFFSET + idxState[3], TRUE);
}

void CPopupOption::_SetGamePlayHandType(void)
{
	switch( m_nHandType)
	{
	case 0:
		m_pPlayHandTypeCombo->SetText(GAME_STRING("STR_POPUP_RIGHT_HANDED"));/*오른손잡이*/
		break;
	case 1:
		m_pPlayHandTypeCombo->SetText(GAME_STRING("STR_POPUP_LEFT_HANDED"));/*왼손잡이*/
		break;
	default:
		m_pPlayHandTypeCombo->SetText(GAME_STRING("STR_POPUP_RIGHT_HANDED"));/*오른손잡이*/
		break;
	}
}


////////////////////////////////////////////////////////////////
// 게임내에서의 Interface설정
////////////////////////////////////////////////////////////////
void CPopupOption::_SetInterfaceNick(void)
{
	m_bEnableFrindlyName = m_pInterfaceNickname->getCheckBoxState();
}

void CPopupOption::_SetInterfaceMark(void)
{
	m_bEnableFrindlyMark = m_pInterfaceMark->getCheckBoxState();
}

////////////////////////////////////////////////////////////////
// 게임내에서의 Control설정
////////////////////////////////////////////////////////////////
void	CPopupOption::_SetControlMouseInvert(void)
{
	m_bEnableMouseInvert = m_pControlReverse->getCheckBoxState();
}

void	CPopupOption::_InitKeyboardSetting(void)
{
	// 키 설정 관련 멤버 초기화
	i3InputDeviceManager	*	pMng	= g_pViewer->GetInputDeviceManager();
	m_pKeyboard	= pMng->GetKeyboard();
	m_pMouse = pMng->GetMouse();

	m_nMaxKeyMapIdx = GAME_KEY_MAP_COUNT - MAX_KEYMAP_SLOT;

	// 키 값 초기화
/*	UINT32 idxKeymap = m_pControlList->GetCurrentTop();

	for(UINT32 i = 0; i < MAX_KEYMAP_SLOT; i++)
	{
		m_keymapSlot[i]._pKey->SetText(_GetKeyboardExplain(idxKeymap + i + 1));
		m_keymapSlot[i]._pValue->SetText(_GetKeyboardValue(&m_KeyMapArray[idxKeymap + i + 1]));
	}
*/
	UINT32	nIndexTop	= m_pControlList->GetCurrentTop();
	UINT32	nIdxKeyMap; 

	for(UINT32 i = 0; i < MAX_KEYMAP_SLOT; i++)
	{
		nIdxKeyMap		= m_nListKeyIndex[ nIndexTop + i ];

		m_keymapSlot[i]._pKey->SetText(_GetKeyboardExplain( nIdxKeyMap ));
		m_keymapSlot[i]._pValue->SetText(_GetKeyboardValue(&m_KeyMapArray[ nIdxKeyMap ]));
	}
}

const char *	CPopupOption::_GetKeyboardExplain(INT32 idx)
{
	switch( idx)
	{
		// 이동
	case GAME_KEY_MAP_LEFT:					return 	GAME_STRING("STBL_IDX_GAMEKEY_LEFT");
	case GAME_KEY_MAP_RIGHT:				return 	GAME_STRING("STBL_IDX_GAMEKEY_RIGHT");
	case GAME_KEY_MAP_FORWARD:				return 	GAME_STRING("STBL_IDX_GAMEKEY_FORWARD");
	case GAME_KEY_MAP_BACKWARD:				return 	GAME_STRING("STBL_IDX_GAMEKEY_BACKWARD");
	case GAME_KEY_MAP_JUMP:					return 	GAME_STRING("STBL_IDX_GAMEKEY_JUMP");
	case GAME_KEY_MAP_FOOTSTEP:				return 	GAME_STRING("STBL_IDX_GAMEKEY_FOOTSTEP");
	case GAME_KEY_MAP_CROUCH:				return 	GAME_STRING("STBL_IDX_GAMEKEY_CROUCH");

		// 무기사용
	case GAME_KEY_MAP_ATTACK:				return 	GAME_STRING("STBL_IDX_GAMEKEY_ATTACK");
	case GAME_KEY_MAP_SUBFUNC:				return 	GAME_STRING("STBL_IDX_GAMEKEY_SUBFUNC");
	case GAME_KEY_MAP_RELOAD:				return 	GAME_STRING("STBL_IDX_GAMEKEY_RELOAD");
	case GAME_KEY_MAP_TURNBACK :			return	GAME_STRING("STBL_IDX_GAMEKEY_TURNBACK");
	case GAME_KEY_MAP_DROP :				return	GAME_STRING("STBL_IDX_GAMEKEY_DROP");
	case GAME_KEY_MAP_WEAPON_EXTENSION :	return	GAME_STRING("STBL_IDX_GAMEKEY_WEAPON_EXTENSION");

		// 무기교체
	case GAME_KEY_MAP_CHANGEWEAPON:			return	GAME_STRING("STBL_IDX_GAMEKEY_CHANGEWEPON");
	case GAME_KEY_MAP_CHANGEWEAPON_1:		return 	GAME_STRING("STBL_IDX_GAMEKEY_CHANGEWEPON_1");
	case GAME_KEY_MAP_CHANGEWEAPON_2:		return 	GAME_STRING("STBL_IDX_GAMEKEY_CHANGEWEPON_2");
	case GAME_KEY_MAP_CHANGEWEAPON_3:		return 	GAME_STRING("STBL_IDX_GAMEKEY_CHANGEWEPON_3");
	case GAME_KEY_MAP_CHANGEWEAPON_4:		return 	GAME_STRING("STBL_IDX_GAMEKEY_CHANGEWEPON_4");
	case GAME_KEY_MAP_CHANGEWEAPON_5:		return 	GAME_STRING("STBL_IDX_GAMEKEY_CHANGEWEPON_5");
	case GAME_KEY_MAP_CHANGEWEAPON_6:		return 	GAME_STRING("STBL_IDX_GAMEKEY_CHANGEWEPON_6");
	case GAME_KEY_MAP_CHANGEWEAPON_QUICK:	return 	GAME_STRING("STBL_IDX_GAMEKEY_CHANGEWEPON_QUICK");
	case GAME_KEY_MAP_CHANGEWEAPON_PREV:	return 	GAME_STRING("STBL_IDX_GAMEKEY_CHANGEWEPON_PREV");
	case GAME_KEY_MAP_CHANGEWEAPON_NEXT:	return 	GAME_STRING("STBL_IDX_GAMEKEY_CHANGEWEPON_NEXT");

		// 기타
	case GAME_KEY_MAP_SHOWSCORE:			return 	GAME_STRING("STBL_IDX_GAMEKEY_SHOWSCORE");
	case GAME_KEY_MAP_CHAT:					return 	GAME_STRING("STBL_IDX_GAMEKEY_CHAT");
	case GAME_KEY_MAP_VOICECHAT:			return 	GAME_STRING("STBL_IDX_GAMEKEY_VOICECHAT");
	case GAME_KEY_MAP_RADIOMESSAGE1:		return 	GAME_STRING("STBL_IDX_GAMEKEY_RADIOMESSAGE1");
	case GAME_KEY_MAP_RADIOMESSAGE2:		return 	GAME_STRING("STBL_IDX_GAMEKEY_RADIOMESSAGE2");
	case GAME_KEY_MAP_RADIOMESSAGE3:		return 	GAME_STRING("STBL_IDX_GAMEKEY_RADIOMESSAGE3");
	case GAME_KEY_MAP_SPECIALACTION:		return 	GAME_STRING("STBL_IDX_GAMEKEY_SPECIALACTION");
	case GAME_KEY_MAP_MOUSE_SENS_UP:		return 	GAME_STRING("STBL_IDX_GAMEKEY_MOUSESENEUP");
	case GAME_KEY_MAP_MOUSE_SENS_DOWN:		return 	GAME_STRING("STBL_IDX_GAMEKEY_MOUSESENEDOWN");

	case GAME_KEY_MAP_SCREENSHOT:			return 	GAME_STRING("STBL_IDX_GAMEKEY_SCREENSHOT");
	case GAME_KEY_MAP_VIDEOCAPTURE:			return 	GAME_STRING("STBL_IDX_GAMEKEY_VIDEOCAPTURE");

	case GAME_KEY_MAP_TOGGLE_MAP:			return 	GAME_STRING("STBL_IDX_GAMEKEY_TOGGLEMAP");
	case GAME_KEY_MAP_MINIMAP_ENLARGE:		return 	GAME_STRING("STBL_IDX_GAMEKEY_MINIMAP_ENLARGE");
	case GAME_KEY_MAP_MINIMAP_REDUCTION:	return 	GAME_STRING("STBL_IDX_GAMEKEY_MINIMAP_REDUCTION");
	case GAME_KEY_MAP_CHAT_ALL:				return 	GAME_STRING("STBL_IDX_GAMEKEY_CHAT_ALL");
	case GAME_KEY_MAP_CHAT_TEAM:			return 	GAME_STRING("STBL_IDX_GAMEKEY_CHAT_TEAM");
	case GAME_KEY_MAP_CHAT_CLAN:			return 	GAME_STRING("STBL_IDX_GAMEKEY_CHAT_CLAN");
	case GAME_KEY_MAP_CHAT_HISTORY:			return 	GAME_STRING("STBL_IDX_GAMEKEY_CHAT_HISTORY");

	// 야전 상점
	case GAME_KEY_MAP_INGAME_SHOP:			return GAME_STRING("STBL_IDX_GAMEKEY_INGAME_SHOP");
	}

	return NULL;
}

const char *		GetKeyboardString(INT8 device_type, UINT32 key_state)
{
	if (device_type == GAME_DEVICE_KEYBOARD)
	{
		switch(key_state)
		{
		case I3I_KEY_UNKNOWN:					return	GAME_STRING("STR_POPUP_NO_USE");/*[사용안함]*/

		case I3I_KEY_0:							return 	"0";
		case I3I_KEY_1:							return 	"1";
		case I3I_KEY_2:							return 	"2";
		case I3I_KEY_3:							return 	"3";
		case I3I_KEY_4:							return 	"4";
		case I3I_KEY_5:							return 	"5";
		case I3I_KEY_6:							return 	"6";
		case I3I_KEY_7:							return 	"7";
		case I3I_KEY_8:							return 	"8";
		case I3I_KEY_9:							return 	"9";

		case I3I_KEY_A:							return 	"A";
		case I3I_KEY_B:							return 	"B";
		case I3I_KEY_C:							return 	"C";
		case I3I_KEY_D:							return 	"D";
		case I3I_KEY_E:							return 	"E";
		case I3I_KEY_F:							return 	"F";
		case I3I_KEY_G:							return 	"G";
		case I3I_KEY_H:							return 	"H";
		case I3I_KEY_I:							return 	"I";
		case I3I_KEY_J:							return 	"J";
		case I3I_KEY_K:							return 	"K";
		case I3I_KEY_L:							return 	"L";
		case I3I_KEY_M:							return 	"M";
		case I3I_KEY_N:							return 	"N";
		case I3I_KEY_O:							return 	"O";
		case I3I_KEY_P:							return 	"P";
		case I3I_KEY_Q:							return 	"Q";
		case I3I_KEY_R:							return 	"R";
		case I3I_KEY_S:							return 	"S";
		case I3I_KEY_T:							return 	"T";
		case I3I_KEY_U:							return 	"U";
		case I3I_KEY_V:							return 	"V";
		case I3I_KEY_W:							return 	"W";
		case I3I_KEY_X:							return 	"X";
		case I3I_KEY_Y:							return 	"Y";
		case I3I_KEY_Z:							return 	"Z";

		case I3I_KEY_ESC:						return 	"ESC";
		case I3I_KEY_ENTER:						return 	"Enter";
		case I3I_KEY_LCTRL:						return 	"L-Ctrl";
		case I3I_KEY_RCTRL:						return 	"R-Ctrl";
		case I3I_KEY_LSHIFT:					return 	"L-Shift";
		case I3I_KEY_RSHIFT:					return 	"R-Shift";
		case I3I_KEY_LALT:						return 	"L-Alt";
		case I3I_KEY_RALT:						return 	"R-Alt";
		case I3I_KEY_SPACE:						return 	"Space";
		case I3I_KEY_LEFT:						return 	"Left";
		case I3I_KEY_RIGHT:						return 	"Right";
		case I3I_KEY_UP:						return 	"Up";
		case I3I_KEY_DOWN:						return 	"Down";
		case I3I_KEY_PGUP:						return 	"Page up";
		case I3I_KEY_PGDN:						return 	"Page down";
		case I3I_KEY_HOME:						return 	"Home";
		case I3I_KEY_END:						return 	"End";
		case I3I_KEY_DEL:						return 	"Delete";
		case I3I_KEY_INS:						return 	"Insert";
		case I3I_KEY_TAB:						return 	"Tab";
		case I3I_KEY_NUMLOCK:					return 	"Num Lock";
		case I3I_KEY_CAPSLOCK:					return 	"Caps Lock";
		case I3I_KEY_SCROLLLOCK:				return 	"Scroll Lock";
		case I3I_KEY_BREAK:						return 	"Break";
		case I3I_KEY_PRTSC:						return 	"Print Screen";
		case I3I_KEY_WIN:						return 	"Window Key";
		case I3I_KEY_BACK:						return 	"Back Space";

		case I3I_KEY_F1:						return 	"F1";
		case I3I_KEY_F2:						return 	"F2";
		case I3I_KEY_F3:						return 	"F3";
		case I3I_KEY_F4:						return 	"F4";
		case I3I_KEY_F5:						return 	"F5";
		case I3I_KEY_F6:						return 	"F6";
		case I3I_KEY_F7:						return 	"F7";
		case I3I_KEY_F8:						return 	"F8";
		case I3I_KEY_F9:						return 	"F9";
		case I3I_KEY_F10:						return 	"F10";
		case I3I_KEY_F11:						return 	"F11";
		case I3I_KEY_F12:						return 	"F12";

		case I3I_KEY_NUMPAD0:					return 	"Numpad 0";
		case I3I_KEY_NUMPAD1:					return 	"Numpad 1";
		case I3I_KEY_NUMPAD2:					return 	"Numpad 2";
		case I3I_KEY_NUMPAD3:					return 	"Numpad 3";
		case I3I_KEY_NUMPAD4:					return 	"Numpad 4";
		case I3I_KEY_NUMPAD5:					return 	"Numpad 5";
		case I3I_KEY_NUMPAD6:					return 	"Numpad 6";
		case I3I_KEY_NUMPAD7:					return 	"Numpad 7";
		case I3I_KEY_NUMPAD8:					return 	"Numpad 8";
		case I3I_KEY_NUMPAD9:					return 	"Numpad 9";
		case I3I_KEY_NUMPADDIVIDE:				return 	"Numpad Div";
		case I3I_KEY_NUMPADMULTIPLY:			return 	"Numpad Mul";
		case I3I_KEY_NUMPADMINUS:				return 	"Numpad Minus";
		case I3I_KEY_NUMPADPLUS:				return 	"Numpad Plus";
		case I3I_KEY_NUMPADENTER:				return 	"Numpad Enter";
		case I3I_KEY_NUMPADDEL:					return 	"Numpad Del";

		case I3I_KEY_MINUS:						return 	"-";
		case I3I_KEY_EQUAL:						return 	"=";
		case I3I_KEY_BACKSLASH:					return 	"\\";
		case I3I_KEY_LBRACKET:					return 	"[";
		case I3I_KEY_RBRACKET:					return 	"]";
		case I3I_KEY_SEMICOLON:					return 	";";
		case I3I_KEY_APOSTROPHE:				return 	"'";
		case I3I_KEY_COMMA:						return 	",";
		case I3I_KEY_PERIOD:					return 	".";
		case I3I_KEY_SLASH:						return 	"/";
		case I3I_KEY_GRAVE:						return	"~";
		default: break;
		}
	}
	else
	if (device_type == GAME_DEVICE_MOUSE)
	{
		switch(key_state)
		{
		case I3I_MOUSE_LBUTTON:						return GAME_STRING("STR_POPUP_MOUSE_LEFT_BUTTON");/*마우스 좌측버튼*/
		case I3I_MOUSE_RBUTTON:						return GAME_STRING("STR_POPUP_MOUSE_RIGHT_BUTTON");/*마우스 우측버튼*/
		case I3I_MOUSE_MBUTTON:						return GAME_STRING("STR_POPUP_MOUSE_WHEEL_BUTTON");/*마우스 휠버튼*/
		case I3I_MOUSE_WHEELUP:						return GAME_STRING("STR_POPUP_MOUSE_WHEEL_UP");/*마우스휠 올리기*/
		case I3I_MOUSE_WHEELDOWN:					return GAME_STRING("STR_POPUP_MOUSE_WHEEL_DOWN");/*마우스휠 내리기*/
		default: break;
		}
	}
	
	return GAME_STRING("STR_POPUP_NO_USE");/*[사용안함]*/
}

const char *		GetKeymapString(GAME_KEY_MAP key_map)
{
	GAME_KEY_STRUCT* gk = g_pEnvSet->getGameKeyMap() +  ptrdiff_t( key_map );
	return ::GetKeyboardString(gk->_nDiviceType, gk->_nState);
}

const char *	CPopupOption::_GetKeyboardValue(GAME_KEY_STRUCT *pGameKey)
{
	return ::GetKeyboardString(pGameKey->_nDiviceType, pGameKey->_nState);
}


void	CPopupOption::_SetKeyMapValue( INT32 idx, I3I_KEY key)
{
	INT32 i = 0;
	INT32 nKeyIdxInterval = 0;
	INT32	nKeyIndex	= m_nListKeyIndex[ idx ];

	//m_KeyMapArray[idx+1]._nDiviceType = GAME_DEVICE_KEYBOARD;
	//m_KeyMapArray[idx+1]._nState = key;
	m_KeyMapArray[ nKeyIndex ]._nDiviceType = GAME_DEVICE_KEYBOARD;
	m_KeyMapArray[ nKeyIndex ]._nState = key;

	if( m_nDuplicateIdx != -1)
	{
		INT32 nDuplKeyIndex		= m_nListKeyIndex[ m_nDuplicateIdx ];

		m_KeyMapArray[ nDuplKeyIndex ]._nState	= (UINT32)I3I_KEY_UNKNOWN;
		//m_KeyMapArray[m_nDuplicateIdx+1]._nState = (UINT32)I3I_KEY_UNKNOWN;

		// 중복된 Key가 있는곳으로 List의 위치를 변경합니다.
		{
			nKeyIdxInterval = m_nDuplicateIdx - m_nCurKeyMapIdx;
			if( nKeyIdxInterval > 0)
			{
				nKeyIdxInterval = i3Math::abs( nKeyIdxInterval);

				for( i=0; i<nKeyIdxInterval; i++)
				{
					I3ASSERT( m_pControlList != NULL);

					if( m_pControlList) m_pControlList->MoveNext();
				}
			}
			else
			{
				nKeyIdxInterval = i3Math::abs( nKeyIdxInterval);

				for( i=0; i<nKeyIdxInterval; i++)
				{
					I3ASSERT( m_pControlList != NULL);

					if( m_pControlList) m_pControlList->MovePrev();
				}
			}
		}

		OnControlListSelect( m_nDuplicateIdx);
		m_nCurKeyMapIdx = m_nDuplicateIdx;
		if( m_nCurKeyMapIdx > m_nMaxKeyMapIdx) m_nCurKeyMapIdx = m_nMaxKeyMapIdx;

		m_nDuplicateIdx = -1;
	}

	//	m_KeyMapArray[idx+1]._nMouseState = 0;
	m_nChangeKeyMapIdx = 0;
	m_ChangeKey = I3I_KEY_UNKNOWN;

	if( m_pControlPopup)
	{
		GameUI::CloseMsgBox( m_pControlPopup);
		m_pControlPopup = NULL;
	}
}

void	CPopupOption::_SetMouseMapValue( INT32 idx, UINT32 key)
{
	INT32 i = 0;
	INT32 nKeyIdxInterval = 0;

	m_KeyMapArray[idx+1]._nDiviceType = GAME_DEVICE_MOUSE;
	m_KeyMapArray[idx+1]._nState = key;

	if( m_nDuplicateIdx != -1)
	{
		m_KeyMapArray[m_nDuplicateIdx+1]._nState = 0;

		// 중복된 Key가 있는곳으로 List의 위치를 변경합니다.
		{
			nKeyIdxInterval = m_nDuplicateIdx - m_nCurKeyMapIdx;
			if( nKeyIdxInterval > 0)
			{
				nKeyIdxInterval = i3Math::abs( nKeyIdxInterval);

				for( i=0; i<nKeyIdxInterval; i++)
				{
					m_pControlList->MoveNext();
				}
			}
			else
			{
				nKeyIdxInterval = i3Math::abs( nKeyIdxInterval);

				for( i=0; i<nKeyIdxInterval; i++)
				{
					m_pControlList->MovePrev();
				}
			}
		}

		OnControlListSelect( m_nDuplicateIdx);
		m_nCurKeyMapIdx = m_nDuplicateIdx;
		if( m_nCurKeyMapIdx > m_nMaxKeyMapIdx) m_nCurKeyMapIdx = m_nMaxKeyMapIdx;

		m_nDuplicateIdx = -1;
	}

	//	m_KeyMapArray[idx+1]._nKeyboardState = I3I_KEY_UNKNOWN;
	m_nChangeKeyMapIdx = 0;
	m_ChangeMouse = 0;

	if( m_pControlPopup)
	{
		GameUI::CloseMsgBox( m_pControlPopup);
		m_pControlPopup = NULL;
	}
}

void	CPopupOption::_UpdateKeyboardValue(void)
{
	INT32 i = 0;
	//	COLOR col;
	static BOOL bOnce = FALSE;

	if( m_nSelectedKeyMapIdx != 0)
	{
		if( m_pControlPopup == NULL)
		{
			m_pControlPopup = GameUI::MsgBox( MSG_TYPE_GAME_WAIT, GAME_STRING("STBL_IDX_EP_OPTION_CHANGE_KEY"));	// 변경하실 키를 눌러주세요.
			RePositionMsgBox( m_pControlPopup);
		}

		{
			for( i=0; i<I3I_KEY_MAX; i++)
			{
				if( m_pKeyboard->GetStrokeState( (I3I_KEY)i))
				{
					switch( (I3I_KEY)i)
					{
						// 사용할수 없는 키입니다.
					case I3I_KEY_ESC:
					case I3I_KEY_BREAK:
					//case I3I_KEY_SCROLLLOCK:
					case I3I_KEY_WIN:
					case I3I_KEY_NUMLOCK:
					case I3I_KEY_PRTSC:
					case I3I_KEY_F1:
						continue;
						break;
					}

					m_ChangeKey = (I3I_KEY)i;

					
					//여러 키를 동시에 누르는 경우엔 m_nSelectedKeyMapIdx == 0 값이 오네..
					if( m_nSelectedKeyMapIdx == 0 )
						return;

					// 새로운 Key값을 적용합니다.
					m_nChangeKeyMapIdx = m_nCurKeyMapIdx + (m_nSelectedKeyMapIdx-1);

					if( _CheckKeyMap( m_ChangeKey, m_nChangeKeyMapIdx, &m_nDuplicateIdx, GAME_DEVICE_KEYBOARD) )
					{
						_SetKeyMapValue( m_nChangeKeyMapIdx, m_ChangeKey);
						m_nSelectedKeyMapIdx = 0;
					}
					else
					{
						if( m_pControlPopup)
						{
							GameUI::CloseMsgBox( m_pControlPopup);
							m_pControlPopup = NULL;
						}

						// 이미 같은키가 존재합니다.\n\n그래도 교체하시겠습니까?
						UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_STRING("STBL_IDX_EP_OPTION_DUPLICATE_KEY"), NULL, MAKE_CALLBACK(CbKeymap), this);
						RePositionMsgBox(pBox);
					
						m_nSelectedKeyMapIdx = 0;
					}
				}
			}

			if( m_pMouse->GetButtonState() == 0)
			{
				bOnce = TRUE;
			}

			if (bOnce)
			{
				m_ChangeMouse = m_pMouse->GetButtonState();
				if( m_ChangeMouse != 0)
				{
					m_nChangeKeyMapIdx = m_nCurKeyMapIdx + (m_nSelectedKeyMapIdx-1);

					if( _CheckKeyMap( m_ChangeMouse, m_nChangeKeyMapIdx, &m_nDuplicateIdx, GAME_DEVICE_MOUSE) )
					{
						_SetMouseMapValue( m_nChangeKeyMapIdx, m_ChangeMouse);
						m_nSelectedKeyMapIdx = 0;
					}
					else
					{
						if( m_pControlPopup)
						{
							GameUI::CloseMsgBox( m_pControlPopup);
							m_pControlPopup = NULL;
						}

						// 이미 같은키가 존재합니다.\n\n그래도 교체하시겠습니까?
						UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_STRING("STBL_IDX_EP_OPTION_DUPLICATE_KEY"), NULL, MAKE_CALLBACK(CbMousemap), this);
						RePositionMsgBox( pBox);
						m_nSelectedKeyMapIdx = 0;
					}
				}
			}
		}
	}
	else
	{
		bOnce = FALSE;

		if( m_pControlPopup)
		{
			GameUI::CloseMsgBox( m_pControlPopup);
			m_pControlPopup = NULL;
		}
	}
}

void CPopupOption::CbKeymap(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		((CPopupOption*)pThis)->_SetKeyMapValue( ((CPopupOption*)pThis)->m_nChangeKeyMapIdx, ((CPopupOption*)pThis)->m_ChangeKey);
	}
	else
	{
		// 중복된 키를해제합니다.
		((CPopupOption*)pThis)->m_nDuplicateIdx = -1;
	}
}

void CPopupOption::CbMousemap(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		((CPopupOption*)pThis)->_SetMouseMapValue( ((CPopupOption*)pThis)->m_nChangeKeyMapIdx, ((CPopupOption*)pThis)->m_ChangeMouse);
	}
	else
	{
		// 중복된 키를해제합니다.
		((CPopupOption*)pThis)->m_nDuplicateIdx = -1;
	}
}

BOOL	CPopupOption::_CheckKeyMap( I3I_KEY key, INT32 exception, INT32* DuplicateIdx,INT8 deviceType)
{
	INT32 i = 0;
	INT32	nCount	= m_pControlList->GetElementCount();
	INT32	nKeyIndex;
/*
	for( i=1; i<GAME_KEY_MAP_COUNT; i++)
	{
		if (deviceType == m_KeyMapArray[i]._nDiviceType &&
			m_KeyMapArray[i]._nState == (UINT32)key &&
			exception+1 != i)
		{
			*DuplicateIdx = i-1;
			return FALSE;
		}
	}
*/
	for( i = 0; i < nCount; i++ )
	{
		nKeyIndex	= m_nListKeyIndex[i];
	
		if( deviceType == m_KeyMapArray[ nKeyIndex ]._nDiviceType
			&& m_KeyMapArray[ nKeyIndex ]._nState == (UINT32)key
			&& exception != i )
		{
			*DuplicateIdx = i;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL	CPopupOption::_CheckKeyMap( UINT32 key, INT32 exception, INT32* DuplicateIdx,INT8 deviceType)
{
	INT32 i = 0;

	for( i=1; i<GAME_KEY_MAP_COUNT; i++)
	{
		if (deviceType == m_KeyMapArray[i]._nDiviceType &&
			m_KeyMapArray[i]._nState == key &&
			exception+1 != i)
		{
			*DuplicateIdx = i-1;
			return FALSE;
		}
	}

	return TRUE;
}

void	CPopupOption::_SetControlKeySlider(INT32 nVal)
{

}

void	CPopupOption::_UpdateKeyMap(void)
{
	//UINT32	idxKeymap = m_pControlList->GetCurrentTop();
	UINT32	nIndexTop	= m_pControlList->GetCurrentTop();
	UINT32	nIdxKeyMap; 

	for(UINT32 i = 0; i < MAX_KEYMAP_SLOT; i++)
	{
		//m_keymapSlot[i]._pKey->SetText(_GetKeyboardExplain(idxKeymap + i + 1));
		//m_keymapSlot[i]._pValue->SetText(_GetKeyboardValue(&m_KeyMapArray[idxKeymap + i + 1]));

		nIdxKeyMap		= m_nListKeyIndex[ nIndexTop + i ];

		m_keymapSlot[i]._pKey->SetText(_GetKeyboardExplain( nIdxKeyMap ));
		m_keymapSlot[i]._pValue->SetText(_GetKeyboardValue(&m_KeyMapArray[ nIdxKeyMap ]));
	}

	// 만약 키값이 변경가능 상태라면 Key값을 변경합니다.
	_UpdateKeyboardValue();
}

void	CPopupOption::_SetControlKeyChange(void)
{

}
void	CPopupOption::_SetControlKeyDelete(void)
{

}
void	CPopupOption::_SetControlMouseSense( REAL32 fVal)
{
	char szTemp[MAX_PATH] = {0,};

	m_rMouseSensivity += fVal;
	if( m_rMouseSensivity > MAX_MOUSE_SENSIVITY) m_rMouseSensivity = MAX_MOUSE_SENSIVITY;
	else if( m_rMouseSensivity < MIN_MOUSE_SENSIVITY) m_rMouseSensivity = MIN_MOUSE_SENSIVITY;

	sprintf_s( szTemp, "%.2f", m_rMouseSensivity);
	m_pControlMouseValue->SetIMEText( szTemp);

	// 사운드 문제로 인해서 일단 집어 넣습니다 - praptor
	m_pControlMouseGagePrev->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pControlMouseGageNext->setControlState(I3GUI_CONTROL_STATE_NORMAL);
}
void	CPopupOption::_SetControlMouseSenseSlider(void)
{
	//char szTemp[MAX_PATH] = {0,};

	//REAL32 fSliderValue = m_pSliderControl[GUI_CONTROL_SLIDER_MOUSESENSE]->getCurValue();
	//m_rMouseSensivity = MIN_MOUSE_SENSIVITY + (fSliderValue / (100.0f / (MAX_MOUSE_SENSIVITY-MIN_MOUSE_SENSIVITY)));
	//sprintf_s( szTemp, "%.2f", m_rMouseSensivity);
	//m_pEditBoxControl[GUI_CONTROL_EDITBOX_MOUSESENE]->SetText( szTemp);

}
void	CPopupOption::_SetControlFov( REAL32 fVal)
{
	char szTemp[MAX_PATH] = {0,};

	m_fFovValue += fVal;
	if( m_fFovValue > MAX_CAM_FOV) m_fFovValue = MAX_CAM_FOV;
	else if( m_fFovValue < MIN_CAM_FOV) m_fFovValue = MIN_CAM_FOV;

	sprintf_s( szTemp, "%d", (INT32) m_fFovValue);
	m_pControlFovValue->SetIMEText( szTemp);

	// 사운드 문제로 인해서 일단 집어 넣습니다 - raja
	m_pControlFovPrev->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pControlFovNext->setControlState(I3GUI_CONTROL_STATE_NORMAL);
}
void	CPopupOption::_SetControlFovSlider(void)
{
	//char szTemp[MAX_PATH] = {0,};

	//REAL32 fSliderValue = m_pSliderControl[GUI_CONTROL_SLIDER_FOV]->getCurValue();
	//m_fFovValue = MIN_CAM_FOV + (fSliderValue / (100.0f / (MAX_CAM_FOV - MIN_CAM_FOV)));
	//sprintf_s( szTemp, "%d", (INT32)m_fFovValue);
	//m_pEditBoxControl[GUI_CONTROL_EDITBOX_FOV]->SetText( szTemp);

}
void	CPopupOption::SetDefaultControlSetting(void)
{
	g_pEnvSet->InitGameKeyMap();
}

void	CPopupOption::SetNewControlSetting(void)
{
	INT32 i = 0;

	for( i=0; i< GAME_KEY_MAP_COUNT; i++)
	{
		if( g_pEnvSet->m_KeyMapArray[i]._nDiviceType != m_KeyMapArray[i]._nDiviceType ||
			g_pEnvSet->m_KeyMapArray[i]._nState != m_KeyMapArray[i]._nState )
		{
			// GameContext에 변화되어진 Key의 셋팅을 알려줍니다.
			g_pGameContext->SetNetEnvSetBasicChange( g_pGameContext->GetNetEnvSetBasicChange() | 0x00000002);
		}
	}

	i3mem::Copy( g_pEnvSet->m_KeyMapArray, m_KeyMapArray,sizeof(GAME_KEY_STRUCT)*GAME_KEY_MAP_COUNT);
}



////////////////////////////////////////////////////////////////
// 게임내에서의 Sound설정
////////////////////////////////////////////////////////////////
void	CPopupOption::_SetSoundSpeaker(void)
{
	switch( m_nSpeakerType)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		m_pSoundSpeakerCombo->SetText( GAME_STRING("STBL_IDX_SPEAKER_TYPE1"));	// 2.1 채널
		break;
	default:
		break;
	}
}
void	CPopupOption::_SetSoundBG(REAL32 fVal)
{
	char szTemp[MAX_PATH] = {0,};

	m_bEnableBGM = m_pSoundBgm->getCheckBoxState();

	if( m_bEnableBGM)
	{
		m_pSoundBgmPrev->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pSoundBgmNext->setControlState(I3GUI_CONTROL_STATE_NORMAL);

		m_fVolBGM += fVal;
		if( m_fVolBGM>100.0f) m_fVolBGM = 100.0f;
		else if( m_fVolBGM <0.0f) m_fVolBGM = 0.0f;
		sprintf_s( szTemp, "%d", (INT32) m_fVolBGM);

		m_pSoundBgmValue->SetIMEText( szTemp);

		m_pSoundBgmValue->setInputDisable(FALSE);
	}
	else
	{
		m_pSoundBgmPrev->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundBgmNext->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundBgmValue->setInputDisable(TRUE);
	}

	SetSoundSetting();
}

void	CPopupOption::_SetSoundFX(REAL32 fVal)
{
	char szTemp[MAX_PATH] = {0,};

	m_bEnableEffect = m_pSoundEffect->getCheckBoxState();

	if( m_bEnableEffect)
	{
		m_pSoundEffectPrev->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pSoundEffectNext->setControlState(I3GUI_CONTROL_STATE_NORMAL);

		m_fVolEffect += fVal;
		if( m_fVolEffect>100.0f) m_fVolEffect = 100.0f;
		else if( m_fVolEffect <0.0f) m_fVolEffect = 0.0f;
		sprintf_s( szTemp, "%d", (INT32) m_fVolEffect);

		m_pSoundEffectValue->SetIMEText(szTemp);
		m_pSoundEffectValue->setInputDisable(FALSE);
	}
	else
	{
		m_pSoundEffectPrev->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundEffectNext->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pSoundEffectValue->setInputDisable(TRUE);
	}

	SetSoundSetting();
}


void	CPopupOption::_SetRadioMessage(void)
{
	m_bEnableRadioMessage = m_pSoundVoice->getCheckBoxState();

	if( GetCodePage() == I3_LANG_CODE_KOREAN)
	{
		switch( m_nRadioMessageType)
		{
		case 0:
			m_pSoundVoiceCombo->SetText(GAME_STRING("STR_POPUP_ENGLISH_LANGUAGE"));/*영문*/
			break;
		case 1:
			m_pSoundVoiceCombo->SetText(GAME_STRING("STR_POPUP_NATIVE_LANGUAGE"));/*한글*/
			break;
		default:
			m_pSoundVoiceCombo->SetText(GAME_STRING("STR_POPUP_ENGLISH_LANGUAGE"));/*영문*/
			break;
		}
	}
	else
	{
		switch( m_nRadioMessageType)
		{
#if defined (LOCALE_RUSSIA)
		case 0:
			m_pSoundVoiceCombo->SetText(GAME_STRING("STR_POPUP_RUSSIA_LANGUAGE"));
			break;
		default:
			m_pSoundVoiceCombo->SetText(GAME_STRING("STR_POPUP_RUSSIA_LANGUAGE"));
			break;
#elif defined(LOCALE_TURKEY)
			case 0:
				m_pSoundVoiceCombo->SetText(GAME_STRING("STR_POPUP_NATIVE_LANGUAGE"));/*터키어*/
				break;
			case 1:
				m_pSoundVoiceCombo->SetText(GAME_STRING("STR_POPUP_ENGLISH_LANGUAGE"));/*영문*/
				break;
			default:
				m_pSoundVoiceCombo->SetText(GAME_STRING("STR_POPUP_NATIVE_LANGUAGE"));/*터키어*/
				break;
#else
		case 0:
			m_pSoundVoiceCombo->SetText(GAME_STRING("STR_POPUP_ENGLISH_LANGUAGE"));/*영문*/
			break;
		default:
			m_pSoundVoiceCombo->SetText(GAME_STRING("STR_POPUP_ENGLISH_LANGUAGE"));/*영문*/
			break;
#endif
		}
	}
}
void	CPopupOption::SetSoundSetting(void)
{
	i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
	if( pSndCtx == NULL)
		return ;

	if(g_pSndMng->IsPlayBGM() == FALSE && !g_pFramework->IsBattleStage())
	{
		g_pSndMng->StartBGM("Sound/UI/PB_15ver_BGM_1.ogg", true, true, 1.f);
		
		::InitSound();
	}

	if( m_bEnableBGM)
	{
		pSndCtx->SetTypeVolume( I3SND_PROP_TYPE_BGM,  (UINT8)m_fVolBGM);
	}
	else
	{
		pSndCtx->SetTypeVolume( I3SND_PROP_TYPE_BGM,  (UINT8)0);
	}

	if( m_bEnableEffect)
	{
		pSndCtx->SetTypeVolume( I3SND_PROP_TYPE_EFFECT, (UINT8)m_fVolEffect);
		pSndCtx->SetTypeVolume( I3SND_PROP_TYPE_VOICE, (UINT8)m_fVolEffect);
	}
	else
	{
		pSndCtx->SetTypeVolume( I3SND_PROP_TYPE_EFFECT, (UINT8)0);
		pSndCtx->SetTypeVolume( I3SND_PROP_TYPE_VOICE, (UINT8)0);
	}
}

////////////////////////////////////////////////////////////////
// 게임내에서의 커뮤니티 설정
////////////////////////////////////////////////////////////////
void	CPopupOption::_SetCommunityChat(UINT32 idxItem)
{
	m_nChatType = idxItem;

	const char * pText = NULL;

	switch( m_nChatType)
	{
	case COMMUNITY_PERMISSION_ALL	:	pText	= GAME_STRING("STBL_IDX_CHAT_TYPE2");		break;
	case COMMUNITY_PERMISSION_NO	:	pText	= GAME_STRING("STBL_IDX_CHAT_TYPE3");		break;
	case COMMUNITY_PERMISSION_FRIEND:	pText	= GAME_STRING("STBL_IDX_CHAT_TYPE1");		break;
	}

	if ( pText )
		m_pCommunityChatCombo->SetText((char*)pText);
}


const char *	GetCommunityPermissionString(COMMUNITY_PERMISSION Type)
{
	switch(Type)
	{
	case COMMUNITY_PERMISSION_ALL		:	return GAME_STRING("STBL_IDX_CHAT_TYPE2");
	case COMMUNITY_PERMISSION_NO		:	return GAME_STRING("STBL_IDX_CHAT_TYPE3");
	case COMMUNITY_PERMISSION_FRIEND	:	return GAME_STRING("STBL_IDX_CHAT_TYPE4");
	}
	return "";
}

void	CPopupOption::_SetCommunityWhisper(UINT32 idxItem)
{
	m_nWhisperType = idxItem;

	const char * pText = GetCommunityPermissionString((COMMUNITY_PERMISSION)m_nWhisperType);
	m_pCommunityWhisperCombo->SetText((char*)pText);
}


void	CPopupOption::_SetCommunityInvite(UINT32 idxItem)
{
	m_nInviteType = idxItem;

	const char * pText = GetCommunityPermissionString((COMMUNITY_PERMISSION)m_nInviteType);
	m_pCommunityInviteCombo->SetText((char*)pText);
}


void	CPopupOption::_SetCommunityMessage(UINT32 idxItem)
{
	m_nMessageType = idxItem;

	const char * pText = GetCommunityPermissionString((COMMUNITY_PERMISSION)m_nMessageType);
	m_pCommunityNoteCombo->SetText((char*)pText);
}

////////////////////////////////////////////////////////////////
// 게임내에서의 매크로 설정
////////////////////////////////////////////////////////////////
void	CPopupOption::_SetMacro(INT32 nType, BOOL bFlag)
{
	m_bMacroEnableFriend[nType] = bFlag;
}

void	CPopupOption::UpdateGamePlay( REAL32 rDeltaSeconds )
{

}

void CPopupOption::UpdateInterface(REAL32 rDeltaSeconds)
{

}

void	CPopupOption::UpdateSound( REAL32 rDeltaSeconds )
{
	i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
	if( pSndCtx == NULL)
		return;

	_UpdateBgmValue(rDeltaSeconds);
	_UpdateEffectValue(rDeltaSeconds);

	if (0 == m_fVolBGM)
	{
		m_pSoundBgmGage->SetEnable(FALSE);
	}
	else
	{
		REAL32 rateX = 0.0f;

		if( m_bInGame)
		{
			rateX = 1.0f;
		}
		else
		{
			rateX = i3GuiRoot::getViewer()->GetViewWidth() * 0.9765625e-3f;
		}

		REAL32 rTotal = m_pSoundBgmNext->getPositionX() * rateX;
		rTotal -= m_pSoundBgmNext->getWidth();

		i3Sprite2D* pSprite = m_pSoundBgmGage->getSprite();

		m_pSoundBgmGage->setSize( (INT32)((rTotal * m_fVolBGM) / 100), (INT32) (m_pSoundBgmGage->getHeight()));
		pSprite->SetRect(GUI_SHAPE_OFFSET, m_pSoundBgmGage->getTotalPosX(), m_pSoundBgmGage->getTotalPosY(), (rTotal * m_fVolBGM) / 100.0f, (REAL32)m_pSoundBgmGage->getHeight());
		m_pSoundBgmGage->SetEnable(TRUE);
	}

	if (0 == m_fVolEffect)
	{
		m_pSoundEffectGage->SetEnable(FALSE);
	}
	else
	{
		REAL32 rateX = 0.0f;

		if( m_bInGame)
		{
			rateX = 1.0f;
		}
		else
		{
			rateX = i3GuiRoot::getViewer()->GetViewWidth() * 0.9765625e-3f;
		}
		REAL32 rTotal = m_pSoundEffectNext->getPositionX() * rateX;
		rTotal -= m_pSoundEffectNext->getWidth();

		i3Sprite2D* pSprite = m_pSoundEffectGage->getSprite();

		m_pSoundEffectGage->setSize( (INT32)((rTotal * m_fVolEffect) / 100), (INT32)(m_pSoundEffectGage->getHeight()));
		pSprite->SetRect(GUI_SHAPE_OFFSET, m_pSoundEffectGage->getTotalPosX(), m_pSoundEffectGage->getTotalPosY(), (rTotal * m_fVolEffect) / 100.0f, (REAL32)m_pSoundEffectGage->getHeight());
		m_pSoundEffectGage->SetEnable(TRUE);
	}
}

void	CPopupOption::UpdateControl( REAL32 rDeltaSeconds )
{
	_UpdateMouseValue(rDeltaSeconds);
	_UpdateFovValue(rDeltaSeconds);

	if (0 == m_rMouseSensivity)
	{
		m_pControlMouseGage->SetEnable(FALSE);
	}
	else
	{
		REAL32 rateX  = 0.0f;

		if( m_bInGame)
		{
			rateX = 1.0f;
		}
		else
		{
			rateX = i3GuiRoot::getViewer()->GetViewWidth() * 0.9765625e-3f;
		}
		REAL32 rTotal = m_pControlMouseGageNext->getPositionX() * rateX;
		rTotal -= m_pControlMouseGageNext->getWidth();

		i3Sprite2D* pSprite = m_pControlMouseGage->getSprite();

		m_pControlMouseGage->setSize((INT32)((rTotal * m_rMouseSensivity) / 1.2f), m_pControlMouseGage->getHeight());
		pSprite->SetRect(GUI_SHAPE_OFFSET, m_pControlMouseGage->getTotalPosX(), m_pControlMouseGage->getTotalPosY(), (rTotal * m_rMouseSensivity) / 1.2f, (REAL32)m_pControlMouseGage->getHeight());
		m_pControlMouseGage->SetEnable(TRUE);
	}


	if (0 == m_fFovValue)
	{
		m_pControlFovGage->SetEnable(FALSE);
	}
	else
	{
		REAL32 rateX = 0.0f;

		if( m_bInGame)
		{
			rateX = 1.0f;
		}
		else
		{
			rateX = i3GuiRoot::getViewer()->GetViewWidth() * 0.9765625e-3f;
		}
		REAL32 rTotal = m_pControlFovNext->getPositionX() * rateX;
		rTotal -= m_pControlFovNext->getWidth();

		i3Sprite2D* pSprite = m_pControlFovGage->getSprite();

		m_pControlFovGage->setSize((INT32)((rTotal * (m_fFovValue - MIN_CAM_FOV)) / (MAX_CAM_FOV - MIN_CAM_FOV)), m_pControlFovGage->getHeight());
		pSprite->SetRect(GUI_SHAPE_OFFSET, m_pControlFovGage->getTotalPosX(), m_pControlFovGage->getTotalPosY(), (rTotal * (m_fFovValue - MIN_CAM_FOV)) / (MAX_CAM_FOV - MIN_CAM_FOV), (REAL32)m_pControlFovGage->getHeight());
		m_pControlFovGage->SetEnable(TRUE);
	}


	// Update KeyMap
	_UpdateKeyMap();
}

void	CPopupOption::UpdatePerformance( REAL32 rDeltaSeconds )
{

}
void	CPopupOption::UpdateCommunity( REAL32 rDeltaSeconds )
{

}
void	CPopupOption::UpdateMacro( REAL32 rDeltaSeconds )
{

}
void	CPopupOption::UpdateVoiceChat( REAL32 rDeltaSeconds )
{

}

void CPopupOption::_UpdateBgmValue(REAL32 rDeltaSeconds)
{
	if (m_pSoundBgmValue->getIsFocus())
	{
		m_pSoundBgmValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	}
	else
	{
		char temp[MAX_STRING_COUNT];

		INT32 value = i3String::ToInt(m_pSoundBgmValue->getIMETextBuf());
		if (0 > value || 100 < value)
		{
			/*배경음은 0에서 100사이로 설정하여 주시기 바랍니다.*/
			UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_BGM_VOLUME"));
			RePositionMsgBox( pBox);
		}

		sprintf_s(temp, "%d", (INT32)m_fVolBGM);
		m_pSoundBgmValue->SetIMEText(temp);
		m_pSoundBgmValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	}

	if (GUI_IME_CHANGING(m_pSoundBgmValue))
	{
		if (m_pSoundBgmValue->getIsFocus())
		{
			const char* text = m_pSoundBgmValue->getIMETextBuf();
			INT32 value = i3String::ToInt(text);

			// 유효범위 (0, 100)
			// 단, ToInt()의 에러값이 0이기 때문에, 0만 별도로 검사한다.
			if ((0 <= value && 100 >= value) || 0 == i3String::Compare(text, "0"))
			{
				m_fVolBGM = (REAL32)value;
				SetSoundSetting();
			}
		}
	}

	if (GUI_IME_CONFIRM(m_pSoundBgmValue))
	{
		if (m_pSoundBgmValue->getIsFocus())
		{
			const char* text = m_pSoundBgmValue->getIMETextBuf();
			INT32 value = i3String::ToInt(text);

			m_pSoundBgmValue->InputDone();

			// 유효범위 (0, 100)
			// 단, ToInt()의 에러값이 0이기 때문에, 0만 별도로 검사한다.
			if ((0 <= value && 100 >= value) || 0 == i3String::Compare(text, "0"))
			{
				m_fVolBGM = (REAL32)value;
				SetSoundSetting();
			}

			m_pSoundBgmValue->SetFocus(FALSE);
			i3GuiRoot::setFocusControl(NULL);
		}
		else
		{
			m_pSoundBgmValue->InputDone();
		}
	}
}

void CPopupOption::_UpdateEffectValue(REAL32 rDeltaSeconds)
{
	if (m_pSoundEffectValue->getIsFocus())
	{
		m_pSoundEffectValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	}
	else
	{
		char temp[MAX_STRING_COUNT];

		INT32 value = i3String::ToInt(m_pSoundEffectValue->getIMETextBuf());
		if (0 > value || 100 < value)
		{
			/*효과음은 0에서 100사이로 설정하여 주시기 바랍니다.*/
			UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_SFX_VOLUME"));
			RePositionMsgBox( pBox);
		}

		sprintf_s(temp, "%d", (INT32)m_fVolEffect);
		m_pSoundEffectValue->SetIMEText(temp);
		m_pSoundEffectValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	}

	if (GUI_IME_CHANGING(m_pSoundEffectValue))
	{
		if (m_pSoundEffectValue->getIsFocus())
		{
			const char* text = m_pSoundEffectValue->getIMETextBuf();

			if( i3String::Length( text) > 0)
			{
				INT32 value = i3String::ToInt(text);

				// 유효범위 (0, 100)
				// 단, ToInt()의 에러값이 0이기 때문에, 0만 별도로 검사한다.
				if ((0 <= value && 100 >= value) || 0 == i3String::Compare(text, "0"))
				{
					m_fVolEffect = (REAL32)value;
					SetSoundSetting();
				}
			}
		}
	}

	if (GUI_IME_CONFIRM(m_pSoundEffectValue))
	{
		if (m_pSoundEffectValue->getIsFocus())
		{
			const char* text = m_pSoundEffectValue->getIMETextBuf();

			if( i3String::Length( text) > 0)
			{
				INT32 value = i3String::ToInt(text);

				m_pSoundEffectValue->InputDone();

				// 유효범위 (0, 100)
				// 단, ToInt()의 에러값이 0이기 때문에, 0만 별도로 검사한다.
				if ((0 <= value && 100 >= value) || 0 == i3String::Compare(text, "0"))
				{
					m_fVolEffect = (REAL32)value;
					SetSoundSetting();
				}

				m_pSoundEffectValue->SetFocus(FALSE);
				i3GuiRoot::setFocusControl(NULL);
			}
		}
		else
		{
			m_pSoundEffectValue->InputDone();
		}
	}
}

void CPopupOption::_UpdateMouseValue(REAL32 rDeltaSeconds)
{
	if (m_pControlMouseValue->getIsFocus())
	{
		m_pControlMouseValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	}
	else
	{
		char temp[MAX_STRING_COUNT];

		//REAL32 value = (REAL32)atof(m_pControlMouseValue->getTextNodeDX()->getText());
		if (0.1f > m_rMouseSensivity || 1.2f < m_rMouseSensivity)
		{
			/*마우스 민감도는 0.10에서 1.20사이로 설정하여 주시기 바랍니다.*/
			UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_MOUSE_SENSITIVITY"));
			RePositionMsgBox( pBox);
		
			if( 0.1f > m_rMouseSensivity)	m_rMouseSensivity = 0.1f;
			else if( 1.2f < m_rMouseSensivity)	m_rMouseSensivity = 1.2f;
		}

		i3String::Format( temp, sizeof( temp), "%.2f", m_rMouseSensivity);

		m_pControlMouseValue->getTextNodeDX()->SetText( temp);
		m_pControlMouseValue->SetIMEText(temp);
		m_pControlMouseValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	}

	if (GUI_IME_CHANGING(m_pControlMouseValue))
	{
		if (m_pControlMouseValue->getIsFocus())
		{
			const char* text = m_pControlMouseValue->getIMETextBuf();
			REAL32 value = (REAL32)atof(text);

			// 유효범위 (0, 100)
			// 단, ToInt()의 에러값이 0이기 때문에, 0만 별도로 검사한다.
			if (0.1f <= value && 1.2f >= value)
			{
				m_rMouseSensivity = value;
			}
		}
	}

	if (GUI_IME_CONFIRM(m_pControlMouseValue))
	{
		if (m_pControlMouseValue->getIsFocus())
		{
			const char* text = m_pControlMouseValue->getIMETextBuf();
			REAL32 value = (REAL32)atof(text);

			m_pControlMouseValue->InputDone();

			// 유효범위 (0.10, 1.20)
			// 단, ToInt()의 에러값이 0이기 때문에, 0만 별도로 검사한다.
			if (0.1f <= value && 1.2f >= value)
			{
				m_rMouseSensivity = value;
			}
			else if (0.1f > value)
			{
				/*마우스 민감도는 0.10에서 1.20사이로 설정하여 주시기 바랍니다.*/
			UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_MOUSE_SENSITIVITY"));
			RePositionMsgBox( pBox);
			m_rMouseSensivity = 0.1f;
			}
			else if (1.2f < value)
			{
				/*마우스 민감도는 0.10에서 1.20사이로 설정하여 주시기 바랍니다.*/
			UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_MOUSE_SENSITIVITY"));
			RePositionMsgBox( pBox);
				m_rMouseSensivity = 1.2f;
			}

			m_pControlMouseValue->SetFocus(FALSE);
			i3GuiRoot::setFocusControl(NULL);
		}
		else
		{
			m_pControlMouseValue->InputDone();
		}
	}
}

void CPopupOption::_UpdateFovValue(REAL32 rDeltaSeconds)
{
	if (m_pControlFovValue->getIsFocus())
	{
		m_pControlFovValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	}
	else
	{
		char temp[MAX_STRING_COUNT];

		INT32 value = i3String::ToInt(m_pControlFovValue->getIMETextBuf());
		// 유효범위 (MIN_CAM_FOV, MAX_CAM_FOV)
		if (MIN_CAM_FOV > (REAL32)value || MAX_CAM_FOV < (REAL32)value)
		{
			char message[MAX_STRING_COUNT];

			/*시야각은 %d에서 %d사이로 설정하여 주시기 바랍니다.*/
			i3String::Format( message, sizeof( message), GAME_STRING("STR_POPUP_MESSAGE_SET_TO_FIELD_OF_VIEW"), (INT32)MIN_CAM_FOV, (INT32)MAX_CAM_FOV);
			UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, message);
			RePositionMsgBox( pBox);
		}

		sprintf_s(temp, "%d", (INT32)m_fFovValue);
		m_pControlFovValue->SetIMEText(temp);
		m_pControlFovValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	}

	if (GUI_IME_CHANGING(m_pControlFovValue))
	{
		if (m_pControlFovValue->getIsFocus())
		{
			const char* text = m_pControlFovValue->getIMETextBuf();
			INT32 value = i3String::ToInt(text);

			// 유효범위 (MIN_CAM_FOV, MAX_CAM_FOV)
			if (MIN_CAM_FOV <= value && MAX_CAM_FOV >= value)
			{
				m_fFovValue = (REAL32)value;
			}
		}
	}

	if (GUI_IME_CONFIRM(m_pControlFovValue))
	{
		if (m_pControlFovValue->getIsFocus())
		{
			const char* text = m_pControlFovValue->getIMETextBuf();
			REAL32 value = (REAL32)i3String::ToInt(text);

			m_pControlFovValue->InputDone();

			if( value > MAX_CAM_FOV || value < MIN_CAM_FOV )
			{
				char message[MAX_STRING_COUNT];

				/*시야각은 %d에서 %d사이로 설정하여 주시기 바랍니다.*/
				i3String::Format( message, sizeof(message), GAME_STRING("STR_POPUP_MESSAGE_SET_TO_FIELD_OF_VIEW"), (INT32)MIN_CAM_FOV, (INT32)MAX_CAM_FOV);

				UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, message);
				RePositionMsgBox( pBox);
			}

			// 유효범위 (MIN_CAM_FOV, MAX_CAM_FOV)
			m_fFovValue = MINMAX( MIN_CAM_FOV, value, MAX_CAM_FOV);

			char temp[MAX_STRING_COUNT];
			sprintf_s(temp, "%d", (INT32)m_fFovValue);

			m_pControlFovValue->SetIMEText(temp);
			m_pControlFovValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

			m_pControlFovValue->SetFocus(FALSE);
			i3GuiRoot::setFocusControl(NULL);
		}
		else
		{
			m_pControlFovValue->InputDone();
		}
	}
}
