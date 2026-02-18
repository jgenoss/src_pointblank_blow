#pragma once

#include "mmc.h"

#include "ListSubCtrl.h"
#include "i3TDKListCtrl_Impl.h"

#include "i3TDKRegKeyCtrl.h"
#include "i3TDKRegDataCtrl.h"

#include "i3TDKImageViewCtrl_Impl.h"


class i3TDKRegistryCtrl_Impl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKRegistryCtrl_Impl)

protected:
	DECLARE_MESSAGE_MAP()

public:
	i3TDKRegistryCtrl_Impl();
	virtual ~i3TDKRegistryCtrl_Impl();

protected:
	DWORD				m_dwStyle;

	CImageList *		m_pImageList;
	INT32				m_nIndexOfFirstTDKImage;	//TDK res에서 add된 icon의 첫번째 index

	i3TDKRegDataCtrl	m_ListDataCtrl;	//사용안함.

	i3TDKListCtrl_Impl		m_WeaponListViewCtrl;
	i3TDKListCtrl_Impl		m_ItemListViewCtrl;
	CTreeCtrl m_TreeCtrl, m_SubTree;

	CImageViewCtrl		m_ImageViewCtrl;
	i3RenderContext *	m_pContext;

	i3TDKListCtrl_Impl* m_FocusListCtrl;
	CTreeCtrl* m_FocusTreeCtrl;

	i3RegKey *		m_pRoot;

	HTREEITEM m_pSubTreeFirstItem;
	HTREEITEM m_pCurItem;
	HTREEITEM m_pTreeCurItem;

	INT32	m_nCurRow;
	INT32 m_nCurCol;

	HTREEITEM			m_SrcItem;
	//i3RegData	*		m_SrcData;
	CToolTipCtrl m_SubTreeToolTip;
	char				m_szTimeStamp[64];
	char				m_szTimeStampString[65];



	struct sHitemData 
	{
		CTreeCtrl* treeCtrl = nullptr;
		TVINSERTSTRUCT tvData;
		HTREEITEM hItem = nullptr;
	};
	typedef std::map<std::string, sHitemData> mHitemListCom;
	mHitemListCom m_mHitemList;
	
	//무기전용 
	mHitemListCom m_mWeaponItemList;
	HACCEL m_hAccel;
	
public:
	BOOL			Create( DWORD dwStyle,const RECT &rect, CWnd * pParentWnd, UINT nID);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnNMRClickTree( NMHDR * pNMHDR, LRESULT * pResult);
	
	afx_msg void	OnNMRClickListCtrl( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void	OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void	OnTVNEndLabelEdit( NMHDR * pNMHDR, LRESULT * pResult);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	void		SetRoot( i3RegKey * pRoot);
	i3RegKey *	GetRoot() { return m_pRoot; }

	i3TDKListCtrl_Impl* GetFriendListPtr();
	CTreeCtrl*	GetFriendTreePtr();

	void		SetTreeInfo( TVITEM * pInfo, i3RegKey * pKey);
	void		ReSizeControl();

	void		UpdateAll( void);

	i3TDKListCtrl_Impl* GetListCtrlPtr() {	return &m_ItemListViewCtrl;	}

	void	NotifyRegFileNation();
	
	void	AddRegNation(const std::string& NationName);
	void	DelRegNation(const std::string& NationName);
	void	CopyRegNation(const std::string& FromNation, const std::string& ToNation, bool BeginRoot);
	void	CopyRegNation(size_t FromNation, size_t ToNation, bool BeginRoot);
	void	InsertRegNation(const std::string& AddNation, const std::string& BaseNation);
	void	ResetRegNation(const std::string& NationName, bool BeginRoot);
	void	SortReg();
	int		GetRegNation(const std::string& NationName);

	void	DirectCallCommandRClickList(i3TDKListCtrl_Impl* listCtrl, UINT32 Cmd, LVHITTESTINFO* lvh=NULL);
	void	UpdateSelectedListCtrlItem(bool* rstStamp=NULL);

protected:
	void				CommandRClickTree(UINT32 Cmd, HTREEITEM hSel);
	void				CommandRClickList(UINT32 Cmd, LVHITTESTINFO lvh);

	virtual HTREEITEM	AddTree( CTreeCtrl & TreeCtrl,HTREEITEM hParent, i3RegKey * pKey);
	void				Rec_AddTree( CTreeCtrl & TreeCtrl,HTREEITEM hParent, i3RegKey * pKey);
	void				Rec_SortTree( i3RegKey * pKey, REGISTRYCTRL_SORT_TYPE sort_type );

	//일반 형식
	void				Rec_CheckTimeStampNormal( i3RegKey* pKey );
	//WeaponPef - Single
	void				Rec_CheckTimeStampSingle( i3RegKey* pKey );
	//WeaponPef - Multi
	void				Rec_CheckTimeStampMulti( i3RegKey* pKey );

	//Menu Function
	virtual void		_AddNewKey(CTreeCtrl & TreeCtrl, HTREEITEM hParentItem, bool bMultiNation);
	virtual void		_DeleteKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem);
	virtual void		_RenameKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem);
	virtual void		_CopyKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem);
	virtual void		_PasteKey(CTreeCtrl & TreeCtrl,  HTREEITEM hItem);
	
	void				_FindKey();

	void				_CollapseNode(CTreeCtrl & TreeCtrl, HTREEITEM item);	
	void				_ExpandNode(CTreeCtrl & TreeCtrl, HTREEITEM item);

	void				_ImportKey( CTreeCtrl & TreeCtrl, HTREEITEM item);
	void				_ExportKey( CTreeCtrl & TreeCtrl, HTREEITEM item);

	void				_AddSelectedKeyToList(CTreeCtrl & TreeCtrl);
	void				_ShowSelectedKey( CTreeCtrl & TreeCtrl);
	void				_RemoveSelectedKeyList();
	void				_ImportKeyEx( CTreeCtrl & TreeCtrl, HTREEITEM item);
	void				_ExportKeyEx( CTreeCtrl & TreeCtrl);

	void				_ExprotSqlCreate( CTreeCtrl & TreeCtrl, HTREEITEM item ); 
	void				_ExprotSqlUpdate( CTreeCtrl & TreeCtrl, HTREEITEM item ); 

	//Menu Function
	void				_AddNewData(HTREEITEM hParentItem, INT32 nNewDataType);
	void				_AddNewData(i3TDKListCtrl_Impl & ListCtrl_Impl, HTREEITEM hParentItem, INT32 nNewDataType);
	void				_AddNewWeaponData(i3TDKListCtrl_Impl & ListCtrl_Impl, HTREEITEM hParentItem);
	void				_CopyData(i3TDKListCtrl_Impl & ListCtrl_Impl);
	void				_PasteData(i3TDKListCtrl_Impl & ListCtrl_Impl);
	void				_DeleteData(i3TDKListCtrl_Impl & ListCtrl_Impl);

	void				_CopyDataLeft(i3TDKListCtrl_Impl & ListCtrl_Impl, HTREEITEM hParentItem, int nRow, int nCol);
	void				_CopyDataRight(i3TDKListCtrl_Impl & ListCtrl_Impl, HTREEITEM hParentItem, int nRow, int nCol);
	void				_CopyDataWhole(i3TDKListCtrl_Impl & ListCtrl_Impl, HTREEITEM hParentItem, int nRow, int nCol);

//	void				_DataAnalysis(HTREEITEM hParentItem, int nRow, int nCol, int nType);
	//void				_RenameData(INT32 nRow);


	void				_AddTimeStampData( i3TDKListCtrl_Impl & ListCtrl_Impl, HTREEITEM hParentItem );


	//Pef 파일안에 데이터 복사시에 만든 시간 갱신한다.
	//현재 시간으로 갱신한다.
	void				_UpdateDataTimeStamp(i3RegKey* pKey);
	void				UpdateTimeStamp();

	virtual BOOL		PreTranslateMessage(MSG* pMsg) override;
	virtual BOOL		OnCommand(WPARAM wParam, LPARAM lParam) override;

public:
	void	 _GenerateTimeStamp();
	void	 _Sort( REGISTRYCTRL_SORT_TYPE sorttype );

	// opt=0:포함, 1:prefix, 2:suffix
	struct StrTreeItem
	{
		std::string			m_str;
		HTREEITEM			m_item = nullptr;
	};

	// only for m_treeCtrl
	void CollectTreeItemByStr(std::vector<StrTreeItem>& rst, const char* str, 
		bool findInPrefixOnly=false, HTREEITEM findFrom=TVI_ROOT);
	void SelectItemInTreeCtrl(HTREEITEM hTreeItem);

	void AdjustKeyRegDataCntToNationCount(std::vector<std::string>* rstAdjustedLog);

	HTREEITEM GetSelectdTreeItem();

protected:
	bool m_bItemWeaponPef;	//Item & Weapon  파일 유무
	std::string m_CurrentWeaponType;

	//***********************************************//

	i3RegKey * m_pClassTypeRoot;
	i3RegKey * m_pItemListRoot;
	i3RegKey * m_pWeaponRoot;

	UINT m_MaxClassTypeIndex;
	UINT m_CurrentClassTypeIndex;


	//무기 리스트 목차
	//Client Source에 CommonDef.h 소스에서
	//enum WEAPON_CLASS_TYPE 과 동일해야 한다.
	//size_t : ClassType Index
	//std::string : ClassType Name ( ItemList에 항목 이름과 일치 해야한다. )
	typedef std::map< size_t, std::string > ClassTypeLookup;
	ClassTypeLookup m_ClassTypeLookup;

	bool _DuplicateClassTypeRegistry(const std::string & ClassType, size_t Index);
	void _AddClassTypeRegistry(const std::string & ClassType, size_t Index);
	void _DelClassTypeRegistry(const std::string & ClassType);
	void _RenameClassTypeRegistry(const std::string & ClassType, const std::string & NewClassType);

	bool _DuplicateClassTypeLookup(const std::string & ClassType, size_t Index);
	void _AddClassTypeLookup(const std::string & ClassType, size_t Index);
	void _DelClassTypeLookup(const std::string & ClassType);
	void _RenameClassTypeLookup(const std::string & ClassType, const std::string & NewClassType);

	void AddClassType(const std::string & ClassType, size_t Index, bool ApplyRegistry = false);
	void DelClassType(const std::string & ClassType, bool ApplyRegistry = true);
	void RenameClassType(const std::string & ClassType, const std::string & NewClassType, bool ApplyRegistry = true);

//	void AddClassType(const std::string & ClassType, size_t Index, bool ApplyRegistry = false);
	void DelWeaponData(const std::string & ClassType, INT32 WeaponNumber);
//	void RenameClassType(const std::string & ClassType, const std::string & NewClassType, bool ApplyRegistry = true);

	//0이 나오면 안된다.
	size_t GetClassTypeLookupIndex(const std::string & ClassType) const;




	//***********************************************//
	//size_t : Weapon 의 Number
	//i3RegKey* : Weapon Registry
	//std::string : ItemList에 항목 이름
	typedef std::vector< i3RegKey* > WeaponRegBank;
	typedef std::map< size_t, WeaponRegBank > WeaponRegDictionary;
	typedef std::map< std::string, WeaponRegDictionary* > WeaponDictionary;
	WeaponDictionary m_WeaponDictionary;

	void Rec_AddClassType( HTREEITEM hParent, i3RegKey * pKey);
	void Rec_AddItem( HTREEITEM hParent, i3RegKey * pKey);
	void Rec_AddWeapon( HTREEITEM hParent, i3RegKey * pKey);

	void AddWeaponDictionary(const char* Name, INT32 Value);
	void RegistryWeaponDictionary(INT32 ClassType, INT32 WeaponNumber, i3RegKey * pKey);

public:
	WeaponRegBank* _GetWeaponRegKey(size_t Number);

	void ClearWeaponDictionary();
	void DisplayRegistryWeaponDictionary(size_t Number);
	void DisplayWeaponImage(i3RegKey* pKey);
	void DisplayWeaponRegString(i3RegKey* pKey);

	void SetItemWeaponPef(bool ItemWeaponPef);
	bool GetItemWeaponPef() const {	return m_bItemWeaponPef;	}

	void SelectSubTreeFirstItem();

protected:
		CString GetNewKeyName(i3RegKey * pRegKey);

//////////////////////////////////////////////////////////////////////////
// Multi-Select
//////////////////////////////////////////////////////////////////////////
private:
	typedef std::vector< INT32 >	CON_ITEM;
	typedef CON_ITEM::iterator		ITR_ITEM;

	CON_ITEM m_conSelected;

	void _initSelected();

private:
	typedef std::vector< i3RegData* >	CON_REG_DATA;
	typedef CON_REG_DATA::iterator		ITR_REG_DATA;

	CON_REG_DATA m_conTempRegData;

	void _initTemp();
	void _clearTemp();

protected:
	std::vector<HTREEITEM>				m_selectedTreeItem;

public:
	static bool							ms_useRenderContext;

};


	//void				UpdateTree( HTREEITEM hItem, i3RegKey * pKey);
//	void				OnDelete( CTreeCtrl & TreeCtrl, HTREEITEM hItem);



//	void 

	//void AddItemDictionary(i3RegKey * pKey);
	//void AddWeaponDictionary(i3RegKey * pKey);

//	std::map