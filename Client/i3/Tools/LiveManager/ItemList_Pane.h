#pragma once

struct FIELDCHANGEINFO{
	CString sFieldName;
	CString sValue;
};

struct ITEMSUBINFO{
	CString sItemName;
	CString sBaseResName;
	CString sBaseItemName;
	CString sNewResName;
	CString sItemListName;
	std::vector<FIELDCHANGEINFO> vecFieldInfo;
	std::vector<FIELDCHANGEINFO> vecItemList;
};

class CItemList_Pane : public CDockablePane
{
	DECLARE_DYNAMIC(CItemList_Pane)
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX) override;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void AdjustLayout();

	CDialogBar				m_DlgBar;
	std::vector<ITEMSUBINFO> m_vecWeaonList;
	std::vector<ITEMSUBINFO> m_vecEquipList;

public:
	CItemList_Pane(void);
	virtual ~CItemList_Pane(void);

	bool AddWeapon(ITEMSUBINFO subinfo);
	void DeleteWeapon();
	INT32 GetWeaponListSize();
	INT32 GetEquipListSize();
	ITEMSUBINFO GetItemListInfo(INT32 Index, INT32 nID);
	void ClearSubinfo(int nID);
	ITEMSUBINFO* GetSelectItem();
};
