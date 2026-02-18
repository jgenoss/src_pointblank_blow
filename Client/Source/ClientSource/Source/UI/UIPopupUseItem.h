#if !defined( __UI_POPUP_USEITEM_H__)
#define __UI_POPUP_USEITEM_H__

#include "UIPopupBase.h"
#if defined( ENABLE_UI2)
//	아이템사용 팝업 타입
enum USE_ITEM_POPUP_TYPE
{
	USE_ITEM_UNKNOWN = -1,
	USE_ITEM_QUERY = 0,				//	사용자 사용 문의
	USE_ITEM_COMPLETE,				//	사용 완료

	USE_ITEM_CHANGE_NICK,			//	닉네임 변경
	USE_ITEM_CHANGE_CLAN_NAME,		//	클랜이름 변경
	USE_ITEM_COLOR_NICK,			//	컬러 닉네임
	USE_ITEM_COLOR_CLAN_NAME,		//	컬러 클랜이름
	USE_ITEM_COLOR_CROSSHAIR,		//	컬러 크로스헤어
	USE_ITEM_DISGUISE_NICK,			//	임시 닉네임
	USE_ITEM_DISGUISE_RANK,			//	위장 계급

};

enum USE_ITEM_STYLE
{
	USE_ITEM_STYLE_NORMAL = 0,
	USE_ITEM_STYLE_BUYING,		//	'인벤토리에서 확인할 수 있습니다' - 사용 취소하면 메시지 뜸.
	USE_ITEM_STYLE_RESULT,		//	아이템 사용 결과 창
};

#define SELECT_COLOR_BOX_COUNT					10	//	변경 색상 샘플 갯수
#define SELECT_CROSSHAIR_COLOR_BOX_COUNT		10	//	변경 색상 샘플 갯수

class UIPopupUseItem : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupUseItem);

public:
	UIPopupUseItem();
	virtual ~UIPopupUseItem();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnExitKey(void);

	virtual void ProcessGameEvent( INT32 event, INT32 arg);
	virtual bool OnEntranceStart(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual bool OnExitStart();

	void OnCancel(void);
	void OnUseItem(void);

	void OnOpenChangeNick(void);
	BOOL OnChangeNameDuplicate(void);
	BOOL OnChangeNameOk(void);

	void OnChangeColorSelect(INT32 idx);
	void OnChangeCrosshairColorSelect(INT32 idx);

	void OnChangeRankPopupButton(void);
	void OnChangeRankPopupList(UINT32 idxItem);

	static void CbQueryOk(void * pThis, INT32 nParam);
	static void CbChangeNickWarnning(void * pThis, INT32 nParam);		//	닉네임 변경시 임시 닉네임 사용중이면 경고 팝업 처리

protected:
	bool	_Open( USE_ITEM_POPUP_TYPE type);
	void _InitQueryPopup(void);
	void _InitCompletePopup(void);
	void _UseItem(INT32 nItemFlag);
	void _SetInputLock(BOOL bValue);
	void _SetEnablePopup(USE_ITEM_POPUP_TYPE type);
	BOOL _SetCurrentUseItemName(i3GuiEditBox * pControl);
	BOOL _CheckItem(INT32 nItemFlag);

	void _QueryOK(void);
	void _UseItemOk(void);

	void _InitChangeNickPopup(void);
	void _ChangeNickOk(void);
	void _ChangeNickCheck(void);

	void _InitChangeClanNamePopup(void);
	void _ChangeClanNameOk(void);
	void _ChangeClanNameCheck(void);

	void _InitColorNickPopup(void);
	void _ChangeColorNickOk(void);

	void _InitColorClanNamePopup(void);
	void _ChangeColorClanNameOk(void);

	void _InitColorCrosshairPopup(void);
	void _ChangeColorCrosshairOk(void);

	void _InitDisguiseNickPopup(void);
	void _ChangeDisguiseNickOk(void);

	void _InitDisguiseRankPopup(void);
	void _ChangeDisguiseRankOk(void);



protected:
	i3GuiStatic * m_pRoot;

	//	아이템사용 팝업
	i3GuiStatic * m_pQueryRoot;
	i3GuiEditBox * m_pQueryCaption;		//	'아이템사용'
	i3GuiButton * m_pQueryButtonOk;		//	'확인'
	i3GuiButton * m_pQueryButtonCancel;	//	'취소'
	i3GuiStatic * m_pQueryItemImage;	//	사용 아이템 이미지
	i3GuiEditBox * m_pQueryItemStatic;	//	'사용 아이템 :'
	i3GuiEditBox * m_pQueryItemValue;	//	사용 아이템 이름
	i3GuiEditBox * m_pQueryAttrStatic;	//	'속성 :'
	i3GuiEditBox * m_pQueryAttrValue;	//	사용 아이템 속성
	i3GuiEditBox * m_pQueryNotiveValue;		//	사용 아이템 내용
	i3GuiEditBox * m_pQueryStatic;			//	'아이템을 사용 하시겠습니까?'

	//	닉네임 변경 팝업
	i3GuiStatic * m_pChangeNameRoot;
	i3GuiEditBox * m_pChangeNameCaption;
	i3GuiButton * m_pChangeNameButtonOk;
	i3GuiButton * m_pChangeNameButtonCancel;
	i3GuiEditBox * m_pChangeNameItemStatic;
	i3GuiEditBox * m_pChangeNameItemValue;
	i3GuiEditBox * m_pChangeNameBeforeStatic;
	i3GuiEditBox * m_pChangeNameBeforeValue;
	i3GuiEditBox * m_pChangeNameUserStatic;
	i3GuiEditBox * m_pChangeNameUserEdit;
	i3GuiEditBox * m_pChangeNameCheckStatic;
	i3GuiButton * m_pChangeNameCheck;
	i3GuiEditBox * m_pChangeNameNoticeValue;
	BOOL m_bChangeNameChecked;

	//	컬러 변경 팝업
	i3GuiStatic * m_pChangeColorRoot;
	i3GuiEditBox * m_pChangeColorCaption;
	i3GuiButton * m_pChangeColorButtonOk;
	i3GuiButton * m_pChangeColorButtonCancel;
	i3GuiEditBox * m_pChangeColorItemStatic;
	i3GuiEditBox * m_pChangeColorItemValue;
	i3GuiEditBox * m_pChangeColorCurrentStatic;
	i3GuiEditBox * m_pChangeColorSelectStatic;
	i3GuiEditBox * m_pChangeColorApplyStatic;
	i3GuiEditBox * m_pChangeColorApplyValue;
	i3GuiEditBox * m_pChangeColorNoticeValue;
	i3GuiStatic * m_pChangeColorCurrentColorBox;
	i3GuiStatic * m_pChangeColorSelectColorBoxFrame;
	i3GuiStatic * m_pChangeColorSelectColorBox[SELECT_COLOR_BOX_COUNT];

	//	계급 변경 팝업
	i3GuiStatic * m_pChangeRankRoot;
	i3GuiEditBox * m_pChangeRankCaption;
	i3GuiButton * m_pChangeRankButtonOk;
	i3GuiButton * m_pChangeRankButtonCancel;
	i3GuiEditBox * m_pChangeRankItemStatic;
	i3GuiEditBox * m_pChangeRankItemValue;
	i3GuiEditBox * m_pChangeRankCurrentStatic;
	i3GuiEditBox * m_pChangeRankCurrentValue;
	i3GuiEditBox * m_pChangeRankSelectStatic;
	i3GuiEditBox * m_pChangeRankSelectCombo;
	i3GuiEditBox * m_pChangeRankNoticeValue;
	i3GuiButton * m_pChangeRankSelectButton;
	i3GuiPopupList * m_pChangeRankSelectList;

	//	컬러 크로스헤어 팝업
	i3GuiStatic * m_pChangeCrosshairColorRoot;
	i3GuiEditBox * m_pChangeCrosshairColorCaption;
	i3GuiButton * m_pChangeCrosshairColorButtonOk;
	i3GuiButton * m_pChangeCrosshairColorButtonCancel;
	i3GuiEditBox * m_pChangeCrosshairColorItemStatic;
	i3GuiEditBox * m_pChangeCrosshairColorItemValue;
	i3GuiEditBox * m_pChangeCrosshairColorCurrentStatic;
	i3GuiEditBox * m_pChangeCrosshairColorSelectStatic;
	i3GuiEditBox * m_pChangeCrosshairColorNoticeValue;
	i3GuiStatic * m_pChangeCrosshairColorCurrentColorBox;
	i3GuiStatic * m_pChangeCrosshairColorSelectColorBoxFrame;
	i3GuiStatic * m_pChangeCrosshairColorSelectColorBox[SELECT_CROSSHAIR_COLOR_BOX_COUNT];
	i3GuiStatic * m_pChangeCrosshairColorCrossImage;
	i3GuiStatic * m_pChangeCrosshairColorCrossDotImage;
	i3GuiStatic * m_pChangeCrosshairColorDotImage;
	i3GuiStatic * m_pChangeCrosshairColorCircleImage;

	//	사용 확인 팝업
	i3GuiStatic * m_pCompleteRoot;
	i3GuiEditBox * m_pCompleteCaption;		//	아이템사용
	i3GuiButton * m_pCompleteButtonOk;		//	'확인'
	i3GuiEditBox * m_pCompleteResultStatic;	//	사용 아이템 이름
	i3GuiEditBox * m_pCompleteResultValue;	//	아이템 사용후 적용된 결과
	i3GuiEditBox * m_pCompleteNoticeValue;	//	아이템 사용후 결과
	i3GuiStatic * m_pCompleteCrosshair_Cross;
	i3GuiStatic * m_pCompleteCrosshair_CrossDot;
	i3GuiStatic * m_pCompleteCrosshair_Dot;
	i3GuiStatic * m_pCompleteCrosshair_Circle;

	USE_ITEM_POPUP_TYPE m_Type;
	INT32				m_ItemID;
	INT64				m_AuthWareDBIndex;
	USE_ITEM_STYLE		m_Style;
	INT32				m_nSelected;

	char m_szChangeName[MAX_STRING_COUNT];
	CStringFilter *		m_pStringFilter;
	CStringFilter *		m_pStringFilter2;
};

#endif

#endif	// ENABLE_UI2