#if !defined(__UI_POPUP_CLAN_CHANGE_MARK_OLD_H__)
#define __UI_POPUP_CLAN_CHANGE_MARK_OLD_H__

#if legacy_gui_scene

#include "UIPopupClanBase.h"
#include "ClanDef.h"

#define CHANGE_CLANMARK_COMBINATION_MAX		2
#define CHANGE_CLANMARK_SHAPE_SLOT_MAX		5
#define CHANGE_CLANMARK_COLOR_SLOT_MAX		10

class UIPopup_Clan_ChangeMark_Old : public UIPopupClanBase
{
	I3_CLASS_DEFINE( UIPopup_Clan_ChangeMark_Old, UIPopupClanBase);

public:
	UIPopup_Clan_ChangeMark_Old(void);
	virtual ~UIPopup_Clan_ChangeMark_Old(void);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify) override;
	virtual bool OnLinkControl(const i3::vector_set<STRCINFO*>* plist, i3GameNode * pControl) override;
	virtual void OnInitControl(void) override;
	virtual void OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void OnPreGuiNotify(void) override;
	virtual void OnExitKey(void);
	virtual bool OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code) override;
	virtual void ProcessGameEvent( GAME_EVENT evt, INT32 Arg, const i3::user_data& UserData) override;
	virtual void SetInputDisable(bool bDisable) override;
	virtual bool GetInputDisable(void);

	/*
	param[in] pArg1 INT64* m_nAuthFlag
	param[in] pArg2 UINT32* nOldFlag
	*/
	virtual bool OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool OnExitStart() override;

	void OnOK(void);
	void OnCancel(void);

	virtual bool	OnKey_Enter() { OnOK(); return true; }
	virtual bool	OnKey_Escape() { OnCancel(); return true; } 

	void OnOverlappingCheck(void);
	void OnPrevBackImage(void);
	void OnNextBackImage(void);
	void OnPrevShapeImage(void);
	void OnNextShapeImage(void);
	void OnChangeBackColor(INT32 nColorIdx);
	void OnChangeShapeColor(INT32 nColorIdx);

protected:
	i3GuiStatic * m_pGui;
	i3GuiEditBox * m_pCaption;		//	'클랜 마크 선택'

	i3GuiEditBox * m_pSelectStatic;		//	'SELECT'
	i3GuiEditBox * m_pBackColorStatic;	//	'바탕 색상'
	i3GuiEditBox * m_pShapeColorStatic;	//	'모양 색상'

	i3GuiButton * m_pButtonOk;			//	'확인'
	i3GuiButton * m_pButtonCancel;		//	'취소'
	i3GuiButton * m_pButtonOverlappingCheck;	//	'중복검사'

	i3GuiStatic * m_pClanMarkConfirmImage;		//	최종 마크 이미지

	i3GuiButton * m_pClanMarkPrev[CHANGE_CLANMARK_COMBINATION_MAX];		//	이미지 이전 버튼
	i3GuiButton * m_pClanMarkNext[CHANGE_CLANMARK_COMBINATION_MAX];		//	이미지 다음 버튼

	i3GuiStatic * m_pClanMarkSelectImage[CHANGE_CLANMARK_COMBINATION_MAX][CHANGE_CLANMARK_SHAPE_SLOT_MAX];	//	마크 조합 이미지 
	i3GuiStatic * m_pBackColorBox[CHANGE_CLANMARK_COLOR_SLOT_MAX];			//	이미지 배경 색상
	i3GuiStatic * m_pShapeColorBox[CHANGE_CLANMARK_COLOR_SLOT_MAX];		//	이미지 모양 색상
	i3GuiStatic * m_pBackColorSelected;								//	배경 색상 선택 포커스
	i3GuiStatic * m_pShapeColorSelected;							//	모양 색상 선택 포커스

	INT64 m_nAuthFlag;

	i3Texture *		m_pTexClanMarkFinal;
	i3Texture *		m_pTexClanMarkSelect[CHANGE_CLANMARK_COMBINATION_MAX][CHANGE_CLANMARK_SHAPE_SLOT_MAX];

	INT32 m_nBackImageIdx;		//	1 base (0 unknown)
	INT32 m_nShapeImageIdx;		//	1 base (0 unknown)
	INT32 m_nBackColorIdx;		//	1 base (0 unknown)
	INT32 m_nShapeColorIdx;		//	1 base (0 unknown)

	bool m_bWaitingForCheck;

protected:
	void _InitPopup(UINT32 nFlag);
	void _CleanupTexture(void);
	void _ClanMarkUserSelectImageUpdate(INT32 nShapeIdx);
	void _ClanMarkUserSelectImage(INT32 LayerType, INT32 nSlotIdx, INT32 nImageIdx, INT32 nColorIdx);
	void _ClanMarkUserFinalImage(void);
};

#endif

#endif
