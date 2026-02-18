#pragma once
#if defined(USE_EDITDLG)
#include "afxcmn.h"
#include "i3Framework.h"
#include "resource.h"
#include "i3Framework/i3GameObjPropertyDialog.h"
// ObjSpec_BattleUseItem 대화 상자입니다.

class CGameObjectBattleUseItem;

class ObjSpec_BattleUseItem : public i3GameObjPropertyDialog
{
	DECLARE_DYNAMIC(ObjSpec_BattleUseItem)
public:
	struct ITEMLIST
	{
		T_ItemID	itemID;
		char		itemName[256];
	};

protected:
	enum { MAX_ITEM_COUNT = 5, };
	
	CEdit			m_ED_StartTime;
	CEdit			m_ED_RespawnTime;

	CComboBox		m_CB_ItemType[MAX_ITEM_COUNT];
	CComboBox		m_CB_ItemName[MAX_ITEM_COUNT];
	CEdit			m_ED_ItemProbability[MAX_ITEM_COUNT];

	CEdit			m_ED_ProbText;

	bool			m_bItemListUp;

	i3::vector<ITEMLIST*> itemList_primary;
	i3::vector<ITEMLIST*> itemList_secondary;
	i3::vector<ITEMLIST*> itemList_item;

public:
	ObjSpec_BattleUseItem(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ObjSpec_BattleUseItem();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PB_SPEC_BATTLEUSEITEM};
	
	virtual void	SetObject(i3ElementBase * pObj) override;

private:
	void		_SumProbability(CGameObjectBattleUseItem * pObject);

	// Type 설정
	void		_SetType();

	bool		_isListUpType(INT32 type);
	void		_ComboBoxClear(CComboBox * pCB);

	void		_CBAddPrimaryString(CComboBox * pCB);
	void		_CBAddSecondaryString(CComboBox * pCB);
	void		_CBAddItemString(CComboBox * pCB);

	T_ItemID	_GetPrimaryItemIDbyIdx(INT32 idx);
	T_ItemID	_GetSecondaryItemIDbyIdx(INT32 idx);
	T_ItemID	_GetItemItemIDbyIdx(INT32 idx);

	INT32		_SetItemType(INT32 type, CComboBox* pCB);
	void		_SetItemName(INT curSel, T_ItemID itemID, CComboBox* pCB);

	INT32		_GetPrimaryListIdxByItemID(T_ItemID itemID);
	INT32		_GetSecondaryListIdxByItemID(T_ItemID itemID);
	INT32		_GetItemListIdxByItemID(T_ItemID itemID);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog() override;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnKillfocusEdSttime();
	afx_msg void OnEnKillfocusEdRestime();
	afx_msg void OnEnKillfocusEdProb();
	afx_msg void OnEnKillfocusCBType(UINT32 id);
	afx_msg void OnEnKillfocusCBName(UINT32 id);
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnBnClickedButton1();
};
#endif
