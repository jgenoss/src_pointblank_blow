#ifndef __POPUP_DELETE_ITEM_H
#define __POPUP_DELETE_ITEM_H

#if !defined( ENABLE_UI2)

#include "PopupBase.h"

//	아이템사용 팝업 타입
enum DELETE_ITEM_POPUP_TYPE
{
	DELETE_ITEM_UNKNOWN = -1,
	DELETE_ITEM_QUERY = 0,				//	사용자 사용 문의
	DELETE_ITEM_COMPLETE,				//	사용 완료

};

enum DELETE_ITEM_STYLE
{
	DELETE_ITEM_STYLE_NORMAL = 0,
	DELETE_ITEM_STYLE_BUYING,		//	'인벤토리에서 확인할 수 있습니다' - 사용 취소하면 메시지 뜸.
	DELETE_ITEM_STYLE_RESULT,		//	아이템 사용 결과 창
};

#define SELECT_COLOR_BOX_COUNT					10	//	변경 색상 샘플 갯수
#define SELECT_CROSSHAIR_COLOR_BOX_COUNT		10	//	변경 색상 샘플 갯수

class CPopupDeleteItem : public CPopupBase
{
	I3_CLASS_DEFINE( CPopupDeleteItem);

public:
	CPopupDeleteItem(void);
	virtual ~CPopupDeleteItem(void);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnExitKey(void);
	virtual void OnGameEvent( INT32 event, INT32 arg);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	void OnCancel(void);
	void OnDeleteItem(void);

protected:
	BOOL _Open( DELETE_ITEM_POPUP_TYPE type);
	void _InitQueryPopup(void);
	void _InitCompletePopup(void);
	void _DeleteItem(INT32 nItemFlag);
	void _SetInputLock(BOOL bValue);
	void _SetEnablePopup(DELETE_ITEM_POPUP_TYPE type);
	BOOL _SetCurrentDeleteItemName(i3GuiEditBox * pControl);
	BOOL _CheckItem(INT32 nItemFlag);
	void _QueryOK(void);
	void _DeleteItemOk(void);
	void _GetInvenUseDay(UINT32 Arg, char * pOut);
	
	static void CbQueryOk(void * pThis, INT32 nParam);


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
	i3GuiEditBox * m_pQueryStatic2;			//	'경고 삭제된 아이템은 복구되지 않습니다.'

	DELETE_ITEM_POPUP_TYPE m_Type;
	INT32				m_ItemID;
	INT64				m_AuthWareDBIndex;
	DELETE_ITEM_STYLE		m_Style;
	INT32				m_nSelected;

	char m_szChangeName[MAX_STRING_COUNT];
	CStringFilter *		m_pStringFilter;
	CStringFilter *		m_pStringFilter2;
};

#endif	// ENABLE_UI2

#endif
