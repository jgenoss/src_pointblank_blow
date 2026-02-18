#pragma once

static const CString& STR_WEAPON_BTN	= "Weapon";
static const CString& STR_GOODS_BTN		= "Goods";
static const CString& STR_EQUIPMENT_BTN	= "Equiment";
static const CString& STR_CHARACTER_BTN	= "Character";
static const CString& STR_ABILITY_BTN	= "Ability";
static const CString& STR_ROOT_PATH_BTN	= "RootPath";
static const CString& STR_UTILITY_BTN	= "Utility";

// ItemRibbonBar
enum ePANEL
{
	E_PANEL_HOME,
	MAX_PANEL
};
class cRibbonBarItem : public CMFCRibbonBar
{
	DECLARE_DYNAMIC(cRibbonBarItem)

public:
	cRibbonBarItem();
	virtual ~cRibbonBarItem();

protected:
	DECLARE_MESSAGE_MAP()

private:
	typedef std::vector<UINT> vPanelCon;
	typedef std::map<UINT, CString> mIndexByNameCon;

	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages	m_PanelImages;
	CMFCRibbonCategory* m_pCategoryHome;
	
	vPanelCon		m_vPanelList;
	mIndexByNameCon m_mPanelIDByName;

	int m_nCategoryCnt;

	CMFCRibbonButton* m_pBtnWeaponDlg;

	CMFCRibbonCategory* m_pPanelCatecory[MAX_PANEL];
public:
	void AddRibbonPanel(const UINT& uiTabID);
	void AddPathEditorPanel();
	void AddUtilityPanel();

	void OnVisibleWeaponEdit(const bool& bVisible);

	CMFCRibbonCategory* UpdateRibbonPanel(const UINT& uiCategoryID);
	void UpdateRibbonChildPanel();

public:
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnFileNew();
	void OnFileOpen();
	afx_msg void OnCbnSelchangeComboPannelCategory();
	afx_msg void OnCbnEditchangeComboPannelCategory();
};


