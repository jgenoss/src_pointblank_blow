#pragma once

#include "PropertyEditor/i3RegistrySet_Impl2.h"

struct sKeyDate
{
	i3RegKey*	pKey;
	std::string strCategory;
};

typedef std::vector<sKeyDate>	KEY_LIST;
typedef std::vector<KEY_LIST>	vPefKeyListCon;


enum ePefType
{
	E_PEF_NULL = -1,
	E_PEF_WEAPON,
	E_PEF_GOODS,
	E_PEF_EQUIPMENT,
	E_PEF_CHARA,

	MAX_PEFLIST
};

enum eTreeSearchType
{
	E_SEARCH_NONE,				//검사x
	E_SEARCH_CHILD,				//차일드 검사
	E_SEARCH_CATEGORY_CHILD,	//하위항목을 카테고리로 설정하고 차일드 검사
};

class cItemTreeDef
{
public:
	cItemTreeDef(std::string str, eTreeSearchType nType = E_SEARCH_NONE, std::string strIgnore = "")
		:m_strName(str), m_nSearchType(nType)
	{
		if (!strIgnore.empty())
			FindIgnoreList(strIgnore);
	}

	void FindIgnoreList(std::string strIgnore)
	{
		int nPos = strIgnore.rfind('/');
		if (nPos == -1)
			m_vStrIgnore.push_back(strIgnore);
		else
		{
			std::string strWord = strIgnore.substr(nPos+1, strIgnore.length());
			strIgnore.erase(nPos, strIgnore.length());
			m_vStrIgnore.push_back(strWord);
			if (!strIgnore.empty())
				FindIgnoreList(strIgnore);;
		}
	}

	bool CheckIgnoreWord(const std::string strWord)
	{
		for (size_t ui = 0; ui < m_vStrIgnore.size(); ++ui)
		{
			if (m_vStrIgnore[ui] == strWord)
				return true;
		}
		return false;
	}


	eTreeSearchType m_nSearchType;
	std::string m_strName;
	std::vector<std::string> m_vStrIgnore;
};

class cPefTreeData
{
public:
	cPefTreeData()
	{
	}

	void operator+=(const cItemTreeDef& rhs)
	{
		m_vTreeDefs.push_back(rhs);
	}

	cItemTreeDef* GetTreeProperty(int nDepth)
	{
		if (nDepth >= (int)m_vTreeDefs.size())
			return NULL;

		return &m_vTreeDefs[nDepth];
	}
	std::vector<cItemTreeDef> m_vTreeDefs;
};

class i3TDKRegistryCtrl_Impl;
class cPefDataList : public i3GameNode
{
	I3_CLASS_DEFINE(cPefDataList, i3GameNode);

private:
	vPefKeyListCon				m_vPefKeyLists;
	//typedef vector<i3RegistrySet_Impl2>	vRegSetCon;
	//vRegSetCon					m_vRegList;
	i3RegistrySet_Impl2			m_RegList[MAX_PEFLIST];
	CString						m_CurFilePath[MAX_PEFLIST];

	i3TDKRegistryCtrl_Impl*		m_pRegCtrl;

	ePefType					m_CurrPefType;
	INT32						m_LocaleMax;

	cPefTreeData* m_pPefTreeList[MAX_PEFLIST];
	
public:
	cPefDataList(void);
	virtual ~cPefDataList(void);

	bool		Create(void);
	void		OnUpdatePefList();

	i3RegKey*	getRoot(void);
	i3RegKey*	getRoot(ePefType eType);

	void		SetCodePage(INT32 nCodePage);
	void		SetRegistryCtrl(i3TDKRegistryCtrl_Impl* pRegCtrl);

	void		SetNodePropertyEditor(UINT32 nItemIndex);

	i3RegKey*	GetItemKey(UINT32 nItemIndex);
	INT32		GetItemCount(void);

	INT32		GetItemUIIndex(UINT32 nItemIndex);
	bool		GetItemUIIndex(UINT32 nItemIndex, INT32& Type, INT32& TexIndex, INT32& ShapeIndex);

	const char*	GetItemNodeName(UINT32 nItemIndex);
	void		GetItemResName(UINT32 nItemIndex, char* pszName, UINT32 nNameMax);
	void		GetItemClassIndex(UINT32 nItemIndex, INT32& nClassIdx);
	void		GetItemIndex( UINT32 nItemIndex, const std::string& strDataName, INT32& nClassIdx );

	bool		LoadFile(const char * pFilePath, ePefType type);
	bool		SaveFile(void);

	KEY_LIST*	GetCurrKeyList();
	i3RegKey*	GetCurrentKey(int nIdx);
	std::string GetCategory(int nIdx);

	void		SetCurrentType(ePefType type)	{ if( _TypeCheck( type ) ) m_CurrPefType	= type;		}
	ePefType	GetCurrentType(void) const		{ return m_CurrPefType;		}


private:
	inline bool	_TypeCheck(ePefType type)
	{
		if( type <= E_PEF_NULL || type >= MAX_PEFLIST )
			return false;

		return true;
	}

	inline void	_ClearLocale(void)			{ i3Registry::Set_LocationIndex( 0, 0 );			}
	inline void	_SetLocale(void)			{ i3Registry::Set_LocationIndex(0, m_LocaleMax );	}

	bool		_LoadPef(LPCSTR pFilePath, REGISTRY_FILE_TYPE FileType);
	bool		_LoadPefList(LPCSTR pFilePath, REGISTRY_FILE_TYPE FileType);
	bool		_SavePef(const char * pszFileName);
	bool		SetPefList(const ePefType& eType);

	//내부 트리 구조가 틀리므로 ...각각 셋팅한다..
	bool		CheckExistResFile(i3RegKey* pRegKey);
	bool		SetItemTree(i3RegKey* pRoot, KEY_LIST& eKey, cPefTreeData& data, int nDepth);
// 	bool		SetWeaponItem(i3RegKey* pRoot, KEY_LIST& eKey);
// 	bool		SetGoodsItem(i3RegKey* pRoot, KEY_LIST& eKey);
// 	bool		SetEquipmentItem(i3RegKey* pRoot, KEY_LIST& eKey);
// 	bool		SetCharacterItem(i3RegKey* pRoot, KEY_LIST& eKey);

	//bool		_SetItemList(i3RegKey* pRoot, KEY_LIST& eKey, const char* pszNodeName);
};