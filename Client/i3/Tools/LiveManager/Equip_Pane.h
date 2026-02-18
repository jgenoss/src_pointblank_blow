#pragma once
#include "Weapon_Pane.h"
class CEquip_Pane : public CWeapon_Pane
{
	DECLARE_DYNAMIC(CEquip_Pane)
	DECLARE_MESSAGE_MAP()

	
public:
	CEquip_Pane(void);
	~CEquip_Pane(void);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);
	afx_msg void OnSetFocus(CWnd* pOldWnd);


	virtual BOOL PreTranslateMessage(MSG* pMsg) override;


	virtual void Update() override;
	virtual void AdjustLayout() override;
	virtual bool LoadRegistry() override;
	virtual void AddBaseInfo(CMFCPropertyGridProperty *pProp) override;
	virtual bool AddRegistry() override;
	virtual std::string AddKey(i3RegKey * pNew, char* pchKeyName, UPDATAFIELD type, ITEMSUBINFO* ITEMSUBINFO) override;
	virtual void Save() override;
	virtual void _CopyRight(char *chbuffer, size_t sizeOfString, i3RegArray* pRegArray) override;
	virtual void SaveRegistry() override;
	virtual void ChangeField(CPropertyGridCheckBox* pProperty, i3RegData *pData) override;
	virtual void SaveCharaEditor(ITEMSUBINFO info) override;
	virtual void SaveRSC(ITEMSUBINFO info) override;
	virtual void DeleteRegistry() override;

	void AddEquipList(CString sAttribute);
	
};
