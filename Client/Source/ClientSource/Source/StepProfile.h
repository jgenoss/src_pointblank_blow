#ifndef _StepProfile_h
#define _StepProfile_h

#include "StepBase.h"


#define MAX_ENSIGN_SLOT			4
#define MAX_RIBBON_SLOT			4
#define MAX_DESIGNATION_SLOT	MAX_EQUIP_USER_TITLE	// 호칭 슬롯 갯수(내정보 & 호칭변경 팝업)


enum ProfileType 
{
	PT_START = 0,

	PT_LOCKER = PT_START,
	PT_CHALLENGE,
	PT_HISTORY,

	MAX_PT
};

enum DetailSlot
{
	DS_RANK = 0,
	DS_WIN,
	DS_WIN_RATE,
	DS_KILL,
	DS_KILL_DEATH,
	DS_HIT_RATE,
	DS_HEADSHOT,

	MAX_DS
};

enum PrizeTextType
{
	PTT_NAME = 0,
	PTT_DESCRIPTION,
	PTT_GAIN_WAY,

	PTT_MAX
};

enum CardTextType
{
	CTT_CARDSET_TITLE = 0,
	CTT_CARDSET_NAME,
	CTT_CARD_NAME,

	CTT_MAX
};



class CQuestManager;
class CStepProfile_PrivateInfoBG;
class CStepProfile_CardShopWin;
class CStepProfile_CompensateWin;
class CStepProfile_Designation;
class CStepProfile_Challenge;



class CStepProfile: public CStepBase
{
	I3_CLASS_DEFINE(CStepProfile);
public:
	CStepProfile();
	virtual ~CStepProfile();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnStart(void);
	virtual void OnEnd(void);
	virtual void OnInitRun(void);
	virtual void OnPreGuiNotify(void);
	virtual void OnExitKey(void);
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);

	void EnterInfo(void);
	void LeaveInfo(void);

	void OnLeave(void);
	void OnChallengeTab(void);
	void OnDesignationTab(void);

	void	EnableInputProfileWin( void);
	void	DisableInputProfileWin( void);

	CStepProfile_Designation *	GetDesignation()	{return m_pDesignation;}

private:	
	void _InitCommon(void);
	void _InitChallenge(void);
	void _InitCardComplate(void);

	void _EnableExclusionProfile(ProfileType changeProfile);


	BOOL _IsPossibilityActiveCard(UINT32 cardIdx);

	// 카드 구입/삭제
	void _BuyCardSet(UINT8 cardSetId);
	void _DeleteCardSet(UINT8 cardSetIdx);
	BOOL _IsActiveViewCardSet(void);	
	void _ChangeActiveCardSet(UINT8 cardSetId);
	static void CbDeleteCardSet(void* pThis,INT32 nParam);

private:
	i3GuiStatic* m_pProfileGui;
	i3GuiStatic* m_pProfileWin;
	i3GuiEditBox* m_pProfileCaption;
	i3GuiButton* m_pLeave;	

	i3GuiButton* m_pChallengeTab;
	i3GuiButton* m_pDesignationTab;


	i3GuiStatic* m_pChallenge;

	CStepProfile_Challenge * m_pChallengeWin;

	// 개인정보창 BG
	CStepProfile_PrivateInfoBG * m_pPrivateInfoBG;

	// Designation (호칭 시스템)
	CStepProfile_Designation * m_pDesignation;
	
	// 튜토리얼 보상창
	CStepProfile_CompensateWin * m_pCompensate;


	ProfileType m_currentProfile;


	bool	m_bIsMyInfoView;
};

#endif //_Stepprofile_h
