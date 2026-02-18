#if !defined( _POPUP_CLAN_CREATE_H__)
#define	_POPUP_CLAN_CREATE_H__

#include "PopupBase.h"

#define CREATE_CLANMARK_COMBINATION_MAX		2
#define CREATE_CLANMARK_SHAPE_SLOT_MAX		5
#define CREATE_CLANMARK_COLOR_SLOT_MAX		10

class CPopupClanCreate : public CPopupBase
{
	I3_CLASS_DEFINE( CPopupClanCreate);

	enum { MAX_STRING = 1024 };

protected:

	//Create Popup
	i3GuiStatic *	m_pClanCreateRoot;
	i3GuiStatic	*	m_pClanCreate;

	//	클랜 생성전 안내 
	i3GuiEditBox	*	m_pClanCreateCaption;				// 클랜 생성
	i3GuiButton		*	m_pClanCreateOk;					// 클랜생성 확인
	i3GuiButton		*	m_pClanCreateCancel;				// 클랜생성 취소
	i3GuiEditBox	*	m_pClanCreateGuideValue;			// 클랜생성 도움말

	//	클랜 정보 입력
	i3GuiStatic	*	m_pClanInputInfo;						// 클랜정보 입력
	i3GuiEditBox		*	m_pClanInputInfoCaption;		// 클랜 기본정보 입력
	i3GuiButton		*	m_pClanInputInfoOk;					// 클랜 기본정보 입력 확인
	i3GuiButton		*	m_pClanInputInfoCancel;				// 클랜 기본정보 입력 취소

	i3GuiEditBox	*	m_pClanInputInfoClanName;			// 클랜명
	i3GuiEditBox	*	m_pClanInputInfoClanNameEdit;		// 클랜명 입력
	i3GuiButton		*	m_pClanInputInfoClanNameCheck;		// 클랜명 중복 체크
	i3GuiEditBox	*	m_pClanInputInfoClanNameGuide;		// (한글 8자, 영문 16자)
	i3GuiEditBox	*	m_pClanInputInfoClanIntroduce;		// 클랜소개( 한글 120자, ...)
	i3GuiEditBox	*	m_pClanInputInfoClanIntroduceValue;	// 클랜소개글 입력
	i3GuiEditBox	*	m_pClanInputInfoClanAddressGuideStatic;		//	클랜 아지트 주소 입력 안내
	i3GuiEditBox	*	m_pClanInputInfoClanAddressStatic;	//	클랜 아지트 기본 주소
	i3GuiEditBox	*	m_pClanInputInfoClanAddressEdit;	//	클랜 아지트 유저 주소 입력
	i3GuiStatic     *   m_pClanInputInfoClanAddressBox;
	i3GuiButton		*	m_pClanInputInfoClanAddressCheck;	//	클랜 아지트 주소 중복 검사 버튼
		
	//	클랜 마크 등록
	i3GuiStatic * m_pCreateClanMarkGui;						//	클랜 마크 팝업
	i3GuiEditBox	* m_pCreateClanMarkCaption;				//	'클랜 마크 선택'

	i3GuiEditBox	* m_pCreateClanMarkSelectStatic;					//	'SELECT'
	i3GuiEditBox	* m_pCreateClanMarkBackColorStatic;					//	'바탕 색상'
	i3GuiEditBox	* m_pCreateClanMarkShapeColorStatic;				//	'모양 색상'

	i3GuiButton		* m_pCreateClanMarkButtonOk;						//	'확인'
	i3GuiButton		* m_pCreateClanMarkButtonCancel;					//	'취소'
	i3GuiButton		* m_pCreateClanMarkButtonOverlappingCheck;			//	'중복검사'

	i3GuiStatic		* m_pCreateClanMarkConfirmImage;					//	최종 마크 이미지

	i3GuiButton		* m_pCreateClanMarkPrev[CREATE_CLANMARK_COMBINATION_MAX];		//	이미지 이전 버튼
	i3GuiButton		* m_pCreateClanMarkNext[CREATE_CLANMARK_COMBINATION_MAX];		//	이미지 다음 버튼

	i3GuiStatic		* m_pCreateClanMarkSelectImage[CREATE_CLANMARK_COMBINATION_MAX][CREATE_CLANMARK_SHAPE_SLOT_MAX];	//	마크 조합 이미지 
	i3GuiStatic		* m_pCreateClanMarkBackColorBox[CREATE_CLANMARK_COLOR_SLOT_MAX];		//	이미지 배경 색상
	i3GuiStatic		* m_pCreateClanMarkShapeColorBox[CREATE_CLANMARK_COLOR_SLOT_MAX];		//	이미지 모양 색상
	i3GuiStatic		* m_pCreateClanMarkBackColorSelected;							//	배경 색상 선택 포커스
	i3GuiStatic		* m_pCreateClanMarkShapeColorSelected;							//	모양 색상 선택 포커스

	i3Texture *		m_pTexClanMarkFinal;
	i3Texture *		m_pTexClanMarkSelect[CREATE_CLANMARK_COMBINATION_MAX][CREATE_CLANMARK_SHAPE_SLOT_MAX];

	INT32 m_nClanMarkBackImageIdx;		//	1 base (0 unknown)
	INT32 m_nClanMarkShapeImageIdx;		//	1 base (0 unknown)
	INT32 m_nClanMarkBackColorIdx;		//	1 base (0 unknown)
	INT32 m_nClanMarkShapeColorIdx;		//	1 base (0 unknown)

	//	클랜 이름 중복 검사
    i3GuiStatic	*	m_pOverlappingCheck;
	i3GuiEditBox	*	m_pOverlappingCheckCaption;			// 클랜명 중복 검사
	i3GuiButton		*	m_pOverlappingCheckOk;				// 클랜명 중복검사 확인
	i3GuiButton		*	m_pOverlappingCheckCancel;			// 클랜명 중복검사 취소
	i3GuiButton		*	m_pOverlappingCheckCheck;			// 중복검사 버튼

	i3GuiEditBox	*	m_pOverlappingCheckInput;			// 다른 클랜명 입력
	i3GuiEditBox	*	m_pOverlappingCheckInsertedName;	// 중복으로 판명된 클랜명
	i3GuiEditBox	*	m_pOverlappingCheckInsertedNameGuide;	// 는 이미 사용중이거나 사용할수 없는 이름입니다....
	i3GuiEditBox	*	m_pOverlappingCheckInputEdit;		// 다른 클랜명 입력

	//	클랜 주소 중복 검사
	i3GuiStatic	*	m_pOverlappingAddress;					//	아지트 주소 중복 검사 팝업
	i3GuiEditBox	*	m_pOverlappingAddressCaption;		// 아지트 중복 검사
	i3GuiButton		*	m_pOverlappingAddressOk;			// 아지트 중복검사 확인
	i3GuiButton		*	m_pOverlappingAddressCancel;		// 아지트 중복검사 취소
	i3GuiButton		*	m_pOverlappingAddressCheck;			// 중복검사 버튼

	i3GuiEditBox	*	m_pOverlappingAddressInputStatic;	// 다른 아지트 입력
	i3GuiEditBox	*	m_pOverlappingAddressInsertedName;	// 중복으로 판명된 아지트
	i3GuiEditBox	*	m_pOverlappingAddressInsertedNameGuide;	// 는 이미 사용중이거나 사용할수 없는 아지트입니다....
	i3GuiEditBox	*	m_pOverlappingAddressURL;			// http://cafe.plaync.co.kr.ncpb
	i3GuiEditBox	*	m_pOverlappingAddressInputEdit;		// 다른 아지트 입력

	//	클랜 가입
	i3GuiStatic	*	m_pSubScription;
	i3GuiEditBox	*	m_pSubScriptionCaption;				// 클랜가입요청
	i3GuiButton		*	m_pSubScriptionOk;					// 클랜 가입 확인
	i3GuiButton		*	m_pSubScriptionCancel;				// 클랜 가입 취소

	i3GuiEditBox	*	m_pSubScriptionReceiver;			// To. 
	i3GuiEditBox	*	m_pSubScriptionReceiverValue;		// 대상 클랜명
	i3GuiEditBox	*	m_pSubScriptionSenderNick;			// 닉네임
	i3GuiStatic		*	m_pSubScriptionSenderRank;			// 발신자 등급
	i3GuiEditBox	*	m_pSubScriptionSenderNickValue;		// 발신자 닉네임 

	i3GuiEditBox	*	m_pSubScriptionSenderWin;			// 승수 :
	i3GuiEditBox	*	m_pSubScriptionSenderWinValue;		// 발신자 승수
	i3GuiEditBox	*	m_pSubScriptionSenderScore;			// 전적	:
	i3GuiEditBox	*	m_pSubScriptionSenderScoreValue;	// 발신자 전적

	i3GuiEditBox	*	m_pSubScriptMessageStatic;			// 가입 신청 메시지*( 100자이내)

	i3GuiEditBox	*	m_pSubScriptMessageValue;			// 가입신청 메시지 내용 입력

	CStringFilter *		m_pStringFilter;
	CStringFilter *		m_pStringFilter2;

public:
	CPopupClanCreate(void);
	virtual ~CPopupClanCreate();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnWheelUpdate(INT32 scrollby);
	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual void OnGameEvent( INT32 event, INT32 arg);
	virtual void OnExitKey(void);
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);
	
	void	CreateClanMarkOverlappingConfirm(void);
	void	CreateClanMarkNoSignConfirm(void);

	void	StepToCreateClan(void);

	//	콜백
	static void cbSuccessCreateClan(void* pThis, INT32 nParam);
	static void cbQueryCreateClan(void* pThis, INT32 nParam);
	static void cbQuaryJoinClan(void* pThis, INT32 nParam);
	static void cbCancelRequest(void* pThis, INT32 nParam);
	static void cbCreateClanMarkOverlapping(void * pThis, INT32 nParam);
	static void cbCreateClanMarkNoSign(void * pThis, INT32 nParam);

protected:
	BOOL	m_bWaitingConfirmSubscripting;
	BOOL	m_bWaitingConfirmCreateClan;
	BOOL	m_bWaitingIsDuplicateName;
	BOOL	m_bWaitingIsDuplicateAddress;
	BOOL	m_bWaitingIsDuplicateMark;
	BOOL	m_bCheckingName;
	BOOL	m_bCheckingAddress;
	BOOL	m_bCheckedName;
	BOOL	m_bCheckedAddress;
	BOOL	m_bCheckedClanMark;
	BOOL	m_bCheckedClanIntro;
	BOOL	m_bCreateConfirm;
	BOOL	m_bJumpToClanMarkStep;	

	INT32	m_nCurClanIndex;

	void	_CleanupTexture(void);
	void	_CreateConfirm(void);
	void	_CreateCancel(void);
	void	_InputOK(void);
	void	_InputCancel(void);
	void	_InputCheckNick(void);
	void	_InputCheckClanAddress(void);	
	void	_OverLappingOK(void);
	void	_OverLappingCancel(void);
	void	_OverLappingCheck(void);
	void	_OverLappingAddressOK(void);
	void	_OverLappingAddressCancel(void);
	void	_OverLappingAddressCheck(void);
	void	_SubscriptionOK(void);
	void	_SubScriptionCancel(void);
	void	_SubscriptionSendJoin(void);
	void	_SubscriptionCancelRequest(void);
	void	_CreateClanMarkOK(void);
	void	_CreateClanMarkCancel(void);
	void	_CreateClanMarkOverlappingCheck(void);
	void	_CreateClanMarkPrevBackImage(void);
	void	_CreateClanMarkNextBackImage(void);
	void	_CreateClanMarkPrevShapeImage(void);
	void	_CreateClanMarkNextShapeImage(void);
	void	_CreateClanMarkUserSelectImage(INT32 nShapeIdx, INT32 nSlotIdx, INT32 nImageIdx, INT32 nColorIdx);
	void	_CreateClanMarkUserFinalImage(void);
	void	_CreateClanMarkUserSelectImageUpdate(INT32 nShapeIdx);
	void	_CreateClanMarkChangeBackColor(INT32 nColorIdx);
	void	_CreateClanMarkChangeShapeColor(INT32 nColorIdx);

	BOOL	_CheckNickNameOverLap( const char * pszText);
	BOOL	_CheckClanAddressOverLap(const char * pszText);
	BOOL	_CheckClanAddressValidate(const char * pszText);

	void	_SendCreateClan(void);

	//
	void	_ResetSubscriptionPage();
	void	_ResetClanCreatePage();

	//
	void	_OnPopupSubScriptionPage(void);
	void	_OnPopupClanCreate( void);
	void	_OnPopupInputInfo(void);
	void	_OnPopupCheckOverlapping( const char * pszName, BOOL bDuplicate);
	void	_OnPopupCheckOverlappingAddress( const char * pszName, BOOL bDuplicate);
	void	_OnPopupCreateClanMark(void);

	void	_OnEventDuplicateName( INT32 Arg);
	void	_OnEventDuplicateAddress( INT32 Arg);
	void	_OnEventSubScriptionPage( INT32 Arg);
	void	_OnEventCreateClan( INT32 Arg);
};

#endif	// #define	_POPUP_CLAN_CREATE_H__
