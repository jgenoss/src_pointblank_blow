#pragma once

#include "EquipData.h"
// CPanel_Part
#include "Avatar.h"

class CPanel_Part : public CDockablePane
{
	DECLARE_DYNAMIC(CPanel_Part)

public:
	CPanel_Part();
	virtual ~CPanel_Part();

	void AdjustLayout();

protected:
	CFont m_fntPropList;
	CMFCPropertyGridCtrl m_wndPropList;
	CMFCButton m_btnSave;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);
	afx_msg void OnDestroy();
	afx_msg void OnUpdateButton(CCmdUI *pCmdUI);
	afx_msg void OnBtnClick();
	DECLARE_MESSAGE_MAP()


protected:
	void InitPropList();
	void SetPropListFont();

protected:
	void GetPropItem( CMFCPropertyGridProperty* pProp );
	void SetPropItem( CMFCPropertyGridProperty* pProp );

	void InitPropItem( CMFCPropertyGridProperty* pProp );
	void InitProp();


public:
	void UpdatePart( EQUIP::ePART ePart, const char * pszPartsName, const char * pszFullPath, TEAM_TYPE team );
	void UpdateData();

	void LoadEquipData();

	CEquipData * GetEquipData()			{ return &m_EquipData; }

protected:
	void Init();
	bool CheckAvatar();
	void SetEquip( const char * pszName );

	bool IsLoad();
	bool IsAdjust();

protected:
// Tool buffer 값
//////////////////////////////////////////////////////////////////////////
	char			m_szChara[MAX_PATH];	// 캐릭터
	char			m_szEquip[MAX_PATH];	// 장비
	EQUIP::ePART	m_ePart;				// 파츠
	VEC4D			m_vTransform;			// 변경값
	INT32			m_HeadType;		// 머리 종류, 변경되는 값
//////////////////////////////////////////////////////////////////////////

// 데이터
	CEquipData		m_EquipData;
	bool m_bLoad;
	bool m_bAdjust;
};


