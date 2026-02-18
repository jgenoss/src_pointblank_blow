#pragma once
#include "resource.h"
#include "PropertyGridCheckBox.h"
#include "ItemList_Pane.h"

class CPropertyGridCheckBox;

enum UPDATAFIELD{
	UPDATEFIELD_TYPE_NONE = 0,
	UPDATEFIELD_TYPE_DUMMY,
	UPDATEFIELD_TYPE_DUAL,
	UPDATEFIELD_TYPE_GRENADESHELL,
	UPDATEFIELD_TYPE_SHOTGUN,
};

class CWeapon_PaneToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};


class i3RegistrySet_Impl :	public i3RegistrySet
{
	I3_CLASS_DEFINE( i3RegistrySet_Impl, i3RegistrySet );

public:

	void LoadNationFile(const CString & Path);
	INT32 GetNationCount();
	std::vector< std::string > m_NationList;

protected:

protected:
	virtual void _XML_ConvertDataToXML_Name(UINT32 I3Reg_Type, 
		size_t Index, const char* szPreName,
		char * _DstBuf, size_t _SizeInBytes);

};

class CWeapon_Pane : public CDockablePane
{
 	DECLARE_DYNAMIC(CWeapon_Pane)
 	DECLARE_MESSAGE_MAP()

	i3RegistrySet_Impl m_v2Registry;
	i3RegistrySet_Impl m_v1Registry;

	CPropertyGridCtrlEx m_wndPropList;
	CComboBox m_wndObjectCombo;
	std::vector<CPropertyGridCheckBox*> m_vecCheckBoxProperty;

	INT32 m_nComboBoxCurSel;

	std::string m_sBaseItemName;
	std::string m_sMediaPath;
	std::string m_sNewCharaEditor;
	INT32 m_nBaseChildCount;

	std::string m_sv2PefFullPath;
	std::string m_sv1PefFullPath;
	//bool m_bSave;
public:
	std::string getBaseItemName(){return m_sBaseItemName;}
public:
	CWeapon_Pane(void);
	virtual ~CWeapon_Pane(void);


	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:

	
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);

	virtual void Update();
	virtual void AdjustLayout();
	virtual void InitPropList(char *pchPefPath);
	virtual bool LoadRegistry();
	virtual void AddBaseInfo(CMFCPropertyGridProperty *pProp);
	virtual bool AddRegistry();
	virtual std::string AddKey(i3RegKey * pNew, char* pchKeyName, UPDATAFIELD type, ITEMSUBINFO* ITEMSUBINFO);
	virtual void Save();
	virtual void SaveRegistry();
	virtual void _CopyRight(char *chbuffer, size_t sizeOfString, i3RegArray* pRegArray);
	virtual void ChangeField(CPropertyGridCheckBox* pProperty, i3RegData *pData);
	virtual void SaveCharaEditor(ITEMSUBINFO info);
	virtual void SaveRSC(ITEMSUBINFO info);
	virtual void DeleteRegistry();

	void AddWeaponList(CString sAttribute);
	void AddWeapon( CMFCPropertyGridProperty* pGroup, i3RegKey* pKey);
	void AddBaseInfoToProperty(i3RegKey* pKey, CMFCPropertyGridProperty* pBaseProperty);
	bool UpdateField(i3RegKey * pNew, char* pchChildTab, char* FieldName, char* pchValue);
	i3RegKey* FindWeaponKey(std::string sItemName, std::string sDeleteKeyward);
	
	void StartSave();
	void MergePef();
	void CopyCharaEditor(ITEMSUBINFO info);

	void AdjustRegistry(i3RegKey *pNew, CMFCPropertyGridProperty* pProperty, bool bTabNameCheck = false);

	CString GetPropertyValue(CMFCPropertyGridProperty* pProperty);
	CMFCPropertyGridProperty* GetChildPropertyName(CMFCPropertyGridProperty* pProperty, char* pchChildTab, char* pchName);
	i3RegData* GetItemListKey(i3RegKey* pKey, std::string &sItemName);
	CPropertyGridCtrlEx* GetPropList(){ return &m_wndPropList;}
	i3RegData* FindData(i3RegKey* pKey, char* chDataName, std::string sChildNode = "", INT32 nDataType = I3REG_TYPE_ARRAY);

	INT32 GetNationCount(){ return m_v2Registry.GetNationCount(); }
	i3RegData * _AddNewData( i3RegKey* pRegKey, char* Name, i3::rc_wstring Value , int nType);
	//void AddItemList(char* pchKeyName);

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

};

