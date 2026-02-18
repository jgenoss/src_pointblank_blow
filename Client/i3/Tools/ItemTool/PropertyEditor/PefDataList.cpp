#include "StdAfx.h"
#include "PefDataList.h"
#include "..\MainFrm.h"
#include "i3TDKRegistryCtrl_Impl2.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_fileext.h"

I3_CLASS_INSTANCE(cPefDataList); //, i3GameNode);

//cPefDataList::m_sItemTreeDef[ MAX_PEFLIST ];

cPefDataList::cPefDataList(void)
:	m_pRegCtrl(NULL)
,	m_CurrPefType(E_PEF_NULL)
,	m_LocaleMax(-1)
{
	m_vPefKeyLists.resize(MAX_PEFLIST);
}

cPefDataList::~cPefDataList(void)
{
	for (int i = 0; i < MAX_PEFLIST; ++i)
	{
		I3_SAFE_DELETE(m_pPefTreeList[i]);
	}
}


bool cPefDataList::Create( void )
{
 	for (UINT32 ui = 0; ui < MAX_PEFLIST; ++ui)
 	{
 		i3RegKey* pKey = m_RegList[ui].getRoot();
 		pKey->SetName( "Registry Root");
 		pKey->setNodeStyle(I3REG_STYLE_ROOT);
 
 		m_RegList[ui].SetCodePage( I3_LANG_CODE_ANSI );
 	}

	for (int i = 0; i < MAX_PEFLIST; ++i)
	{
		m_pPefTreeList[i] = new cPefTreeData();
	}

	//PEF파일의 트리구조를 정의한다.

	//====================== 1.0 =======================
	//Weapon.pef
	*m_pPefTreeList[E_PEF_WEAPON]	+= cItemTreeDef("WeaponList",	E_SEARCH_NONE);
	*m_pPefTreeList[E_PEF_WEAPON]	+= cItemTreeDef("Weapon",		E_SEARCH_CATEGORY_CHILD,
		std::string("DinoWeapon/")+
		std::string("Bomb/")+
		std::string("BombTrigger/")+
		std::string("wooden_arrow/MissionTrigger/MissionSentrygunTrigger/M197"));

	//Good.pef
	*m_pPefTreeList[E_PEF_GOODS]	+= cItemTreeDef("ShopItem",		E_SEARCH_CHILD);

	//Equipment.pef
	*m_pPefTreeList[E_PEF_EQUIPMENT] += cItemTreeDef("Equipment",	E_SEARCH_CATEGORY_CHILD);

	//Character.pef
	*m_pPefTreeList[E_PEF_CHARA]	+= cItemTreeDef("Character",	E_SEARCH_CHILD);

	return true;
}

void cPefDataList::OnUpdatePefList()
{
	for (int i = 0; i < MAX_PEFLIST; ++i)
	{
		m_vPefKeyLists[i].clear();
		if (false == SetPefList((ePefType)i))
		{
			I3ASSERT("UpdatePefList Failed");
		}
	}
}


i3RegKey* cPefDataList::getRoot(void)
{
	if( _TypeCheck( m_CurrPefType ) == false )
		return NULL;

	return m_RegList[m_CurrPefType].getRoot();
}

i3RegKey* cPefDataList::getRoot(ePefType eType)
{
	if( _TypeCheck( eType ) == false )
		return NULL;

	return m_RegList[eType].getRoot();
}


void cPefDataList::SetCodePage(INT32 nCodePage)
{
	for( int i = 0; i < MAX_PEFLIST; i++ )
		m_RegList[i].SetCodePage(nCodePage);
}

void cPefDataList::SetRegistryCtrl(i3TDKRegistryCtrl_Impl* pRegCtrl)
{
	m_pRegCtrl	= pRegCtrl;

	if( m_pRegCtrl )
	{
		for( int i = 0; i < MAX_PEFLIST; i++ )
			m_RegList[i].SetListCtrlPtr( pRegCtrl->GetListCtrlPtr() );

		m_LocaleMax	= m_pRegCtrl->GetListCtrlPtr()->GetNationListRef()->size();

		//_SetLocale();
	}
}



bool cPefDataList::LoadFile( const char * pFilePath, ePefType type)
{
	if( pFilePath == NULL )
		return false;

	if( _TypeCheck(type) == false )
		return false;


	m_CurrPefType = type;

	char	szExt[MAX_PATH];
//	i3String::SplitFileExt(pFilePath, szExt, MAX_PATH);
	i3::extract_fileext(pFilePath, szExt);

	if(	i3::generic_is_iequal(szExt, "PEF")   || 
		i3::generic_is_iequal(szExt, "i3Reg") ||
		i3::generic_is_iequal(szExt, "env") )
	{
		return _LoadPef(pFilePath, REGISTRY_FILE_BINARY);
	}
	else if(i3::generic_is_iequal(szExt, "xml") ||
		i3::generic_is_iequal(szExt, "i3RegXML")  )
	{
		return _LoadPef(pFilePath, REGISTRY_FILE_XML_BY_UTF8);
	}


	return false;
}


bool cPefDataList::_LoadPef(LPCSTR pFilePath, REGISTRY_FILE_TYPE FileType)
{
	ePefType eType = m_CurrPefType;

	m_CurFilePath[eType].Empty();
	
	//_ClearLocale();
	_SetLocale();

	INT32	ret	= m_RegList[eType].LoadRegistrySet(pFilePath, FileType);

	//_SetLocale();

	if( ret == STREAM_ERR )
		return false;

	if (false == SetPefList(eType))
	{
		I3ASSERT("Pef List Setting  Failed");
		return false;
	}

	m_CurFilePath[eType]	= pFilePath;

	return true;
}

bool cPefDataList::SetPefList(const ePefType& eType)
{
	i3RegKey* pRoot				= m_RegList[eType].getRoot();
	KEY_LIST& eCurrentKey		= m_vPefKeyLists[eType];
	cPefTreeData* pCurrentData	= m_pPefTreeList[eType];

	SetItemTree(pRoot, eCurrentKey, *pCurrentData, 0);

	return true;
}



bool cPefDataList::SaveFile(void)
{
	if( _TypeCheck( m_CurrPefType ) == false )
		return false;

	if( m_CurFilePath[m_CurrPefType].IsEmpty() )
	{
		AfxMessageBox("로드된 파일이 없습니다.");
		return false;
	}

	_ClearLocale();

	bool	rv	= _SavePef(m_CurFilePath[m_CurrPefType]);

	_SetLocale();

	return rv;
}


bool cPefDataList::_SavePef(const char * pszFileName)
{
	m_RegList[m_CurrPefType].SetCodePage( I3_LANG_CODE_ANSI );

	char szFileName[MAX_PATH] = ""; 
	char szExt[MAX_PATH] = "";

	i3::safe_string_copy( szFileName, pszFileName, MAX_PATH );
//	i3String::SplitFileExt( pszFileName, szExt, sizeof( szExt));
	i3::extract_fileext(pszFileName, szExt);

	// Sorting
	if( m_pRegCtrl )
	{
		m_pRegCtrl->_GenerateTimeStamp();
		m_pRegCtrl->_Sort( SORT_TYPE_KEY );
		m_pRegCtrl->SortReg();
	}

	//	저장 형식이 i3RegXML이면 먼저 Binary(pef)로 저장
	if( i3::generic_is_iequal( szExt, "i3RegXML") )
	{
		i3::change_file_ext_copy(szFileName, pszFileName, "pef");
	}

	//	Binary(pef)로 저장
	if( m_RegList[m_CurrPefType].SaveRegistrySet( szFileName, REGISTRY_FILE_BINARY ) == STREAM_ERR)
		return false;

	i3::change_file_ext_copy(szFileName, pszFileName, "i3RegXML");

	//	i3RegXML로 저장
	if( m_RegList[m_CurrPefType].SaveRegistrySet( szFileName, REGISTRY_FILE_XML_BY_UTF8 ) == STREAM_ERR)
		return false;

	return true;
}

// 
// bool cPefDataList::_SetItemList( i3RegKey* pRoot, KEY_LIST& eKey, const char* pszNodeName )
// {
// 	if( pRoot == NULL )
// 		return false;
// 
// 	i3TreeNode*	pList		= pRoot->FindChildByName(pszNodeName);
// 	if( pList == NULL )
// 		return false;
// 
// 	INT32		nCount		= pList->getChildCount();
// 	for( INT32 i = 0; i < nCount; i++ )
// 	{
// 		i3TreeNode*		pNode	= pList->getChild(i);
// 		if( pNode == NULL )
// 			continue;
// 
// 	//	eKey.push_back( static_cast<i3RegKey*>(pNode) );
// 	}
// 
// 	return true;
// }

bool cPefDataList::CheckExistResFile(i3RegKey* pRegKey)
{
	char pszName[256];
	memset(pszName,0,256);

	i3::rc_wstring wstr;
	i3::string str;

	FIND_REG_DATA( pRegKey, "_ResName", wstr );

	i3::utf16_to_mb( wstr, str);
	i3::generic_string_copy( pszName, str);

	CString strTmp = pszName;
	strTmp.MakeLower();

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (NULL == pFrame)
		return false;
	
	cDlgFileLoader* pLoader = pFrame->GetFileLoader();
	if (NULL == pLoader)
		return false;

	std::string strCharaEditorName = pLoader->GetFileFolderName((LPTSTR)(LPCTSTR)strTmp);

	return strCharaEditorName.empty() == false;
}

bool cPefDataList::SetItemTree(i3RegKey* pRoot, KEY_LIST& eKey, cPefTreeData& data, int nDepth)
{
	eKey.clear();

	if (NULL == pRoot)
		return false;

	cItemTreeDef* pTreeDef = data.GetTreeProperty(nDepth++);

	if (NULL != pTreeDef)
	{
		i3TreeNode* pList = pRoot->FindChildByName(pTreeDef->m_strName.c_str());
		switch(pTreeDef->m_nSearchType)
		{
		case E_SEARCH_NONE:
			{
				SetItemTree((i3RegKey*)pList, eKey, data, nDepth);
			}
			break;
		case E_SEARCH_CHILD:
			{
				for (int i = 0; i < pList->getChildCount(); ++i)
				{
					i3RegKey* pChild = (i3RegKey*)pList->getChild(i);

					sKeyDate data;
					data.pKey = pChild;
					data.strCategory = pList->GetName();
					if (E_PEF_WEAPON == m_CurrPefType)
					{
						if (CheckExistResFile(pChild))
							data.pKey = pChild;
					}
					else
					{
						data.pKey = pChild;
					}
					eKey.push_back(data);
				}
			}
			break;
		case E_SEARCH_CATEGORY_CHILD:
			{
				for (int nCategory = 0; nCategory < pList->getChildCount(); ++nCategory)
				{
					i3TreeNode* pCategoryNode = pList->getChild(nCategory);

					if (pTreeDef->CheckIgnoreWord(pCategoryNode->GetName()))
						continue;
					
					if (NULL == pCategoryNode)
						continue;

					for (int nChildIdx = 0; nChildIdx < pCategoryNode->getChildCount(); ++nChildIdx)
					{
						i3RegKey* pChild = (i3RegKey*)pCategoryNode->getChild(nChildIdx);
						
						//std::map<std::string, 
						if (NULL == pChild)
							continue;

						sKeyDate data;
						data.pKey = pChild;
						data.strCategory = pCategoryNode->GetName();

						//아 무기일 때만.. 리소스가 존재하는지 체크한다..ㅜㅜ
						if (E_PEF_WEAPON == m_CurrPefType)
						{
							if (CheckExistResFile(pChild))
							{
								data.pKey = pChild;
							}
						}
						else
						{
							data.pKey = pChild;
						}

						eKey.push_back(data);
					}
				}
			}
			break;
		}
	}
	return true;
}
// 
// bool cPefDataList::SetWeaponItem(i3RegKey* pRoot, KEY_LIST& eKey)
// {
// 	eKey.clear();
// 
// 	if( pRoot == NULL )
// 		return false;
// 
// 	i3TreeNode*	pList			= pRoot->FindChildByName("WeaponList");
// 	if( pList == NULL )
// 		return false;
// 
// 	i3TreeNode*	pWeaponList		= pList->FindChildByName("Weapon");
// 	if( pWeaponList == NULL )
// 		return false;
// 
// 
// 	INT32		nWeaponCount	= pWeaponList->getChildCount();
// 
// 	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
// 	cDlgFileLoader* pLoader = pFrame->GetFileLoader();
// 
// 	for( INT32 i = 0; i < nWeaponCount; i++ )
// 	{
// 		i3TreeNode*	pSubList	= pWeaponList->getChild(i);
// 		if( pSubList == NULL )
// 			continue;
// 
// 		//아! 예외 추기들을 걸러내야한다...
// 		if (i3String::Compare(pSubList->GetName(), "DinoWeapon") == 0)
// 			continue;
// 
// 		INT32		nSubCount	= pSubList->getChildCount();
// 		for( INT32 j = 0; j < nSubCount; j++ )
// 		{
// 			i3RegKey*	pWeapon		= (i3RegKey*)pSubList->getChild(j);
// 			if( pWeapon == NULL )
// 				continue;
// 
// 			i3RegData* pData = pWeapon->FindDataByName("_ResName");
// 			if (pData)
// 			{
// 				if( pData->getDataType() == I3REG_TYPE_ARRAY )
// 				{
// 					i3RegArray * pArrayData = (i3RegArray*)pData;
// 
// 					size_t size = pArrayData->getSize();
// 					for(size_t i =0; i<size; i++)
// 					{
// 						i3RegString* Data = (i3RegString*)(*pArrayData)[i];
// 						char* szValue = Data->getValue();
// 					}
// 				}
// 			}
// 
// 			char pszName[256];
// 			FIND_REG_DATA(  (i3RegKey*)pWeapon, "_ResName", pszName, 256 );
// 
// 			CString strTmp = pszName;
// 			strTmp.MakeLower();
// 
//  			std::string strCharaEditorName = pLoader->GetFileFolderName((LPTSTR)(LPCTSTR)strTmp);
//  			if (strCharaEditorName.empty())
// 			{
// 				CString strContinue;
// 				strContinue.Format("Continue : %s\n",pWeapon->GetName());
// 				I3TRACE("================================\n");
// 				I3TRACE(strContinue);
// 				I3TRACE("================================\n");
// 				continue;
// 			}
// 
// 			CString strAdd;
// 			strAdd.Format("ADD : %s\n",pWeapon->GetName());
// 			I3TRACE(strAdd);
// 			eKey.push_back( (i3RegKey*)pWeapon );
// 		}
// 	}
// 
// 	return true;
// }
// 
// 
// bool cPefDataList::SetGoodsItem(i3RegKey* pRoot, KEY_LIST& eKey)
// {
// 	eKey.clear();
// 
// 	i3TreeNode*	pList			= pRoot->FindChildByName("ShopItem");
// 	for (int i = 0; i < pList->getChildCount(); ++i)
// 	{
// 		i3TreeNode*	pSubList	= pList->getChild(i);
// 
// 		eKey.push_back( (i3RegKey*)pSubList );
// 	}
// 
// 	return true;
// }
// 
// bool cPefDataList::SetEquipmentItem(i3RegKey* pRoot, KEY_LIST& eKey)
// {
// 	eKey.clear();
// 
// 	i3TreeNode*	pList = pRoot->FindChildByName("Equipment");
// 	for (int nEquipIdx = 0; nEquipIdx < pList->getChildCount(); ++nEquipIdx)
// 	{
// 		i3TreeNode*	pSubList	= pList->getChild(nEquipIdx);
// 		for (int nItem = 0; nItem < pSubList->getChildCount(); ++nItem)
// 		{
// 			i3TreeNode* pItemList = pSubList->getChild(nItem);
// 			//eKey.push_back( (i3RegKey*)pItemList );	
// 		}
// 	}
// 
// 	return true;
// }
// 
// bool cPefDataList::SetCharacterItem(i3RegKey* pRoot, KEY_LIST& eKey)
// {
// 	eKey.clear();
// 
// 	if( pRoot == NULL )
// 		return false;
// 
// 
// 	// version 1.5 - Character와 Dino로 나뉘어 있다.
// 	// version 1.0 or 1.1 - 전캐릭터가 Character노드에 있다
// 	//if( _SetItemList(pRoot, eKey, "Character") == false )
// 		return false;
// 
// 	//if( _SetItemList(pRoot, eKey, "Dino") == false )
// 	{
// 		I3TRACE("Dino가 없습니다. 1.5버전일 경우 포함되어 있어야합니다.");
// 	}
// 
// 	return true;
// }


i3RegKey* cPefDataList::GetItemKey( UINT32 nItemIndex )
{
	if(nItemIndex >= GetCurrKeyList()->size())
		return NULL;

	return GetCurrentKey(nItemIndex);
}

INT32 cPefDataList::GetItemCount( void )
{
	return GetCurrKeyList()->size();
}


INT32 cPefDataList::GetItemUIIndex( UINT32 nItemIndex )
{
	if(nItemIndex >= GetCurrKeyList()->size())
		return -1;

	INT32		nIndex	= -1;

	FIND_REG_DATA( GetCurrentKey(nItemIndex), "_UIShapeIndex", &nIndex );

	return nIndex;
}


bool cPefDataList::GetItemUIIndex( UINT32 nItemIndex, INT32& Type, INT32& TexIndex, INT32& ShapeIndex )
{
	if(nItemIndex >= GetCurrKeyList()->size())
		return false;

	FIND_REG_DATA( GetCurrentKey(nItemIndex), "_Type", &Type );
	FIND_REG_DATA( GetCurrentKey(nItemIndex), "_UITexIndex", &TexIndex );
	FIND_REG_DATA( GetCurrentKey(nItemIndex), "_UIShapeIndex", &ShapeIndex );

	return true;
}



void cPefDataList::GetItemResName( UINT32 nItemIndex, char* pszName, UINT32 nNameMax )
{
	if(nItemIndex >= GetCurrKeyList()->size())
		return;

	i3::rc_wstring wstr;
	i3::string str;

	FIND_REG_DATA( GetCurrentKey(nItemIndex), "_ResName", wstr);
	i3::utf16_to_mb( wstr, str);
	i3::generic_string_copy( pszName, str);
}

const char* cPefDataList::GetItemNodeName( UINT32 nItemIndex )
{
	if(nItemIndex >= GetCurrKeyList()->size())
		return NULL;

	return GetCurrentKey(nItemIndex)->GetName();
}

void cPefDataList::GetItemClassIndex( UINT32 nItemIndex, INT32& nClassIdx )
{
	if(nItemIndex >= GetCurrKeyList()->size())
	{
		return;
	}
	FIND_REG_DATA( GetCurrentKey(nItemIndex), "_ClassType", &nClassIdx );
}

void cPefDataList::GetItemIndex( UINT32 nItemIndex, const std::string& strDataName, INT32& nClassIdx )
{
	if(nItemIndex >= GetCurrKeyList()->size())
	{
		return;
	}
	FIND_REG_DATA( GetCurrentKey(nItemIndex), strDataName.c_str(), &nClassIdx );
}

void cPefDataList::SetNodePropertyEditor( UINT32 nItemIndex )
{
	if(NULL == GetCurrKeyList() || nItemIndex >= GetCurrKeyList()->size())
		return;

	i3RegKey* pKey = GetCurrentKey(nItemIndex);
	std::string strName = pKey->GetName();

	m_RegList[m_CurrPefType].SelectData( pKey );
}

KEY_LIST* cPefDataList::GetCurrKeyList()
{
	if ( _TypeCheck( m_CurrPefType ) == false )
		return NULL;

	return &m_vPefKeyLists[m_CurrPefType];
}

i3RegKey* cPefDataList::GetCurrentKey(int nIdx)
{
	return m_vPefKeyLists[m_CurrPefType][nIdx].pKey;
}

std::string cPefDataList::GetCategory(int nIdx)
{
	if (m_vPefKeyLists[m_CurrPefType].size() <= (UINT32)nIdx)
		return "";
	return m_vPefKeyLists[m_CurrPefType][nIdx].strCategory;
}

