// i3RegistryCtrl.cpp : implementation file
//

#include "stdafx.h"

#include "GlobalVariable.h"

#include "ListSubCtrl.h"
#include "i3TDKRegistryCtrl_Impl.h"
#include "i3TDKUtilShell.h"

#include "WeaponContextDialog.h"
#include "i3Base/itl/algorithm/sort.h"
#include "i3Base/string/algorithm/to_lower.h"
#include "i3Base/string/ext/mb_to_utf16.h"

//***********************************************//
/*
static INT32	CompareNameProc( void* pData1, void* pData2 )
{
	i3RegKey* pRegKey1 =  static_cast<i3RegKey*>(pData1);
	i3RegKey* pRegKey2 =  static_cast<i3RegKey*>(pData2);

	if( pRegKey1->hasName() == false)
		return 1;

	if( pRegKey2->hasName() == false)
		return -1;

	return strcmp( pRegKey1->GetName(), pRegKey2->GetName() );
}
*/
bool CompareNameProc(const i3RegKey* p1, const i3RegKey* p2 )
{
	if( p1->hasName() == false) return false;

	if( p2->hasName() == false) return true;

	return strcmp( p1->GetName(), p2->GetName() ) < 0;
}


/*
static INT32	CompareTimeProc( void* pData1, void* pData2 )
{
	i3RegKey* pRegKey1 =  static_cast<i3RegKey*>(pData1);
	i3RegKey* pRegKey2 =  static_cast<i3RegKey*>(pData2);

	if( pRegKey1->hasName() == false)
		return 1;

	if( pRegKey2->hasName() == false)
		return -1;

	i3RegArray* pArraryRegData1 = (i3RegArray*)pRegKey1->FindDataByName( "__Make Time" );
	i3RegArray* pArraryRegData2 = (i3RegArray*)pRegKey2->FindDataByName( "__Make Time" );

	i3RegString* pRegString1 = (i3RegString*)(*pArraryRegData1)[0];
	i3RegString* pRegString2 = (i3RegString*)(*pArraryRegData2)[0];

	if (pRegString1 == NULL || pRegString2 == NULL )
	{
		I3TRACE( "CompareTimeProc : NULL ");
		return 0;
	}

	return strcmp( pRegString2->getValue(), pRegString1->getValue() );
}
*/
bool	CompareTimeProc(const i3RegKey* pRegKey1, const i3RegKey* pRegKey2 )
{
	if( pRegKey1->hasName() == false)
		return false;

	if( pRegKey2->hasName() == false)
		return true;

	i3RegArray* pArraryRegData1 = (i3RegArray*)pRegKey1->FindDataByName( "__Make Time" );
	i3RegArray* pArraryRegData2 = (i3RegArray*)pRegKey2->FindDataByName( "__Make Time" );

	i3RegString* pRegString1 = (i3RegString*)(*pArraryRegData1)[0];
	i3RegString* pRegString2 = (i3RegString*)(*pArraryRegData2)[0];

	if (pRegString1 == NULL || pRegString2 == NULL )
	{
		I3TRACE( "CompareTimeProc : NULL ");
		return false;
	}
	return  pRegString2->getValue() < pRegString1->getValue();
}



i3RegArray*	 _CreateRegArrayStringData( const char* szName, const char * val, size_t size)
{
	i3RegArray * pNewArrayData = i3RegArray::new_object();
	pNewArrayData->SetName( szName );
	pNewArrayData->setElementType( I3REG_TYPE_STRING );
	
	i3::rc_wstring wstrValue;		i3::mb_to_utf16(val, wstrValue);

	for(size_t i = 0; i < size; i++)
	{
		i3RegString * regData = i3RegString::new_object();
		*regData = wstrValue;
		pNewArrayData->addRegData( regData );
	}

	return pNewArrayData;
}

void _CopyRegData(i3RegArray * pArrayData, int DataIndex, int Begin, int End)
{
	switch( pArrayData->getElementType() )
	{
	case	I3REG_TYPE_INT32:
		{
			INT32 Value;
			GET_REG_SINGLE_DATA((*pArrayData)[DataIndex], &Value);

			for(int i = Begin; i < End; ++i)
				pArrayData->replaceRegData(i, Value);
		}
		break;
	case	I3REG_TYPE_REAL32:
		{
			REAL32 Value;
			GET_REG_SINGLE_DATA((*pArrayData)[DataIndex], &Value);
			for(int i = Begin; i < End; ++i)
				pArrayData->replaceRegData(i, Value);
		}
		break;
	case	I3REG_TYPE_STRING:	
		{
			i3::rc_wstring wstrValue;
			GET_REG_SINGLE_DATA( (*pArrayData)[DataIndex], wstrValue );
			for(int i = Begin; i < End; ++i)
				pArrayData->replaceRegData(i, wstrValue);
		}
		break;
	case	I3REG_TYPE_VEC2D:
		{
			VEC2D Value;
			GET_REG_SINGLE_DATA((*pArrayData)[DataIndex], &Value);
			for(int i = Begin; i < End; ++i)
				pArrayData->replaceRegData(i, Value);
		}
		break;
	case	I3REG_TYPE_VEC3D:
		{
			VEC3D Value;
			GET_REG_SINGLE_DATA((*pArrayData)[DataIndex], &Value);
			for(int i = Begin; i < End; ++i)
				pArrayData->replaceRegData(i, Value);
		}
		break;
	case	I3REG_TYPE_VEC4D:
		{
			VEC4D Value;
			GET_REG_SINGLE_DATA((*pArrayData)[DataIndex], &Value);
			for(int i = Begin; i < End; ++i)
				pArrayData->replaceRegData(i, Value);
		}
		break;
	case	I3REG_TYPE_COLOR:
		{
			I3COLOR Value;
			GET_REG_SINGLE_DATA((*pArrayData)[DataIndex], &Value);
			for(int i = Begin; i < End; ++i)
				pArrayData->replaceRegData(i, Value);
		}
		break;
	case	I3REG_TYPE_MATRIX:
		{
			MATRIX Value;
			GET_REG_SINGLE_DATA((*pArrayData)[DataIndex], &Value);
			for(int i = Begin; i < End; ++i)
				pArrayData->replaceRegData(i, Value);
		}
		break;
	default:
		break;
	}
}


//***********************************************//

void	i3TDKRegistryCtrl_Impl::CommandRClickTree( UINT32 Cmd, HTREEITEM hSel )
{
	switch( Cmd )
	{
	case 	ID_MENU_ADDKEY:			_AddNewKey(*m_FocusTreeCtrl, hSel, false);						break;
	case	ID_MENU_ADDKEY_MULTINATION: _AddNewKey(*m_FocusTreeCtrl, hSel, true);			break;
	case 	ID_MENU_DELETEKEY:		_DeleteKey(*m_FocusTreeCtrl, hSel);						break;
	case 	ID_MENU_RENAMEKEY:		_RenameKey(*m_FocusTreeCtrl, hSel);						break;

	case	ID_MENU_COPYKEY:		_CopyKey(*m_FocusTreeCtrl, hSel);						break;
	case	ID_MENU_PASTEKEY:		_PasteKey(*m_FocusTreeCtrl, hSel);						break;
	
	case 	ID_MENU_FINDKEY:		_FindKey();								break;

	case	ID_MENU_SORT_KEY:		_Sort(SORT_TYPE_KEY );					break;
	case	ID_MENU_SORT_TIME:		_Sort(SORT_TYPE_TIME );				break;
	
	case 	ID_MENU_COLLAPSENODE:	_CollapseNode(*m_FocusTreeCtrl, hSel);	break;
	case 	ID_MENU_EXPANDNODE:		_ExpandNode(*m_FocusTreeCtrl, hSel);	break;
	
	case	ID_MENU_IMPORTKEY:		_ImportKey(*m_FocusTreeCtrl,  hSel);	break;
	case	ID_MENU_EXPORTKEY:		_ExportKey(*m_FocusTreeCtrl,  hSel);	break;

	case	ID_MENU_ADD_SELKEYS:    _AddSelectedKeyToList(*m_FocusTreeCtrl); break;
	case	ID_MENU_SHOW_SELKEYS:	_ShowSelectedKey(*m_FocusTreeCtrl); break;
	case	ID_MENU_REMOVEALL_SELKEYS: _RemoveSelectedKeyList(); break;
	case	ID_MENU_IMPORT_SELKEYS:	_ImportKeyEx(*m_FocusTreeCtrl, hSel); break;
	case	ID_MENU_EXPORT_SELKEYS:	_ExportKeyEx(*m_FocusTreeCtrl); break;

	
	case	ID_MENU_EXPORT_SQLCREATE: _ExprotSqlCreate(*m_FocusTreeCtrl, hSel); break; 
	case	ID_MENU_EXPORT_SQLUPDATE: _ExprotSqlUpdate(*m_FocusTreeCtrl, hSel); break; 


	case	ID_MENU_DTYPE_INT32:	_AddNewData(hSel,	I3REG_TYPE_INT32);	break;
	case	ID_MENU_DTYPE_REAL32:	_AddNewData(hSel,	I3REG_TYPE_REAL32);	break;
	case	ID_MENU_DTYPE_STRING:	_AddNewData(hSel,	I3REG_TYPE_STRING);	break;
	case	ID_MENU_DTYPE_VEC2D:	_AddNewData(hSel,	I3REG_TYPE_VEC2D);	break;
	case	ID_MENU_DTYPE_VEC3D:	_AddNewData(hSel,	I3REG_TYPE_VEC3D);	break;
	case	ID_MENU_DTYPE_VEC4D:	_AddNewData(hSel,	I3REG_TYPE_VEC4D);	break;
	case	ID_MENU_DTYPE_COLOR:	_AddNewData(hSel,	I3REG_TYPE_COLOR);	break;
	case	ID_MENU_DTYPE_MATRIX:	_AddNewData(hSel,	I3REG_TYPE_MATRIX);	break;
	case	ID_MENU_PASTEDATA:		_PasteData(m_ItemListViewCtrl);	break;
	default:
		break;
	}
}

void	i3TDKRegistryCtrl_Impl::CommandRClickList( UINT32 Cmd, LVHITTESTINFO lvh)
{
	switch( Cmd )
	{
	case 	ID_MENU_DELETEDATA:		_DeleteData(*m_FocusListCtrl);		break;
//	case 	ID_MENU_RENAMEDATA:		_RenameData(lvh.iItem);		break;
	case	ID_MENU_COPYDATA:		_CopyData(*m_FocusListCtrl);		break;
	case	ID_MENU_PASTEDATA:		_PasteData(*m_FocusListCtrl);				break;

	case	ID_MENU_DTYPE_INT32:	_AddNewData(*m_FocusListCtrl, m_pCurItem, I3REG_TYPE_INT32);	break;
	case	ID_MENU_DTYPE_REAL32:	_AddNewData(*m_FocusListCtrl, m_pCurItem, I3REG_TYPE_REAL32);	break;
	case	ID_MENU_DTYPE_STRING:	_AddNewData(*m_FocusListCtrl, m_pCurItem,	I3REG_TYPE_STRING);	break;
	case	ID_MENU_DTYPE_VEC2D:	_AddNewData(*m_FocusListCtrl, m_pCurItem,	I3REG_TYPE_VEC2D);	break;
	case	ID_MENU_DTYPE_VEC3D:	_AddNewData(*m_FocusListCtrl, m_pCurItem, I3REG_TYPE_VEC3D);	break;
	case	ID_MENU_DTYPE_VEC4D:	_AddNewData(*m_FocusListCtrl, m_pCurItem, I3REG_TYPE_VEC4D);	break;
	case	ID_MENU_DTYPE_COLOR:	_AddNewData(*m_FocusListCtrl, m_pCurItem, I3REG_TYPE_COLOR);	break;
	case	ID_MENU_DTYPE_MATRIX:	_AddNewData(*m_FocusListCtrl, m_pCurItem, I3REG_TYPE_MATRIX);	break;

	case ID_MENU_DTYPE_WEAPON: _AddNewWeaponData(*m_FocusListCtrl, m_pCurItem);	break;

	case	ID_MENU_DATA_COPY_LEFT:	_CopyDataLeft(*m_FocusListCtrl, m_pCurItem, lvh.iItem, lvh.iSubItem);	break;
	case	ID_MENU_DATA_COPY_RIGHT:	_CopyDataRight(*m_FocusListCtrl, m_pCurItem, lvh.iItem, lvh.iSubItem);	break;
	case	ID_MENU_DATA_COPY_WHOLE:	_CopyDataWhole(*m_FocusListCtrl, m_pCurItem, lvh.iItem, lvh.iSubItem);	break;

	case	ID_MENU_DATA_COMPARE:		m_FocusListCtrl->AnalysisItem(lvh.iItem, lvh.iSubItem, DAT_COMPARE);		break;
	case	ID_MENU_DATA_MATCH:			m_FocusListCtrl->AnalysisItem(lvh.iItem, lvh.iSubItem, DAT_MATCH);		break;

	default:
		break;
	}
}


void	i3TDKRegistryCtrl_Impl::SetTreeInfo( TVITEM * pInfo, i3RegKey * pKey)
{
	memset( pInfo, 0, sizeof( TVITEM));

	pInfo->mask				= TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;
	pInfo->cChildren		= 0;

	if( pKey->getNodeStyle() & I3REG_STYLE_ROOT)
	{
		pInfo->iImage			= m_nIndexOfFirstTDKImage + I3ICON_NODE;
		pInfo->iSelectedImage	= m_nIndexOfFirstTDKImage + I3ICON_NODE + 1;
	}
	else
	{
		pInfo->iImage			= m_nIndexOfFirstTDKImage + I3ICON_GEOMETRY;
		pInfo->iSelectedImage	= m_nIndexOfFirstTDKImage + I3ICON_GEOMETRY + 1;
	}

	pInfo->lParam = ( LPARAM) pKey;
	pInfo->pszText= (LPSTR)pKey->GetName();

	
}

HTREEITEM	i3TDKRegistryCtrl_Impl::AddTree( CTreeCtrl & TreeCtrl, HTREEITEM hParent, i3RegKey * pKey)
{
	TVINSERTSTRUCT	data;
	HTREEITEM		hItem;

	data.hParent		= hParent;
	data.hInsertAfter	= TVI_LAST;

	SetTreeInfo( &data.item, pKey);

	hItem	= TreeCtrl.InsertItem( &data);
	TreeCtrl.SetItemText(hItem, pKey->GetName());
	
	sHitemData HitemData;
	HitemData.hItem = hItem;
	HitemData.tvData = data;
	HitemData.treeCtrl = &TreeCtrl;

	char szClone[256];
	i3::safe_string_copy(szClone,data.item.pszText,256);
	i3::to_lower(szClone);//, strlen(data.item.pszText),ToLower)

	HTREEITEM hParentItem = TreeCtrl.GetParentItem(hItem);
	
	bool bItemListItem = false;
	if (hParentItem)
	{
		i3RegKey* pParentKey = (i3RegKey*)TreeCtrl.GetItemData(hParentItem);
		if (pKey)
		{
			if (i3::generic_is_iequal("ItemList", pParentKey->GetName()))
			{
				m_mWeaponItemList[szClone] = HitemData;
				bItemListItem = true;
			}
		}
	}

	if (false == bItemListItem)
		m_mHitemList[szClone] = HitemData;

	if( hParent != TVI_ROOT)
	{
		TVITEM item;
		item.hItem = hParent;
		item.mask	= TVIF_CHILDREN;

		if( TreeCtrl.GetItem( &item))
		{
			item.cChildren = 1;
			TreeCtrl.SetItem(&item);
		}
	}

	return hItem;
}

void	i3TDKRegistryCtrl_Impl::Rec_AddTree( CTreeCtrl & TreeCtrl, HTREEITEM hParent, i3RegKey * pKey)
{
	HTREEITEM hItem = NULL;
	if( GetItemWeaponPef() && TreeCtrl == m_TreeCtrl )
	{
		if( i3::generic_is_iequal( pKey->GetName(), "ClassType" )  )
			Rec_AddClassType(hParent, pKey);
		else if( i3::generic_is_iequal( pKey->GetName(), "ItemList" ) )
			Rec_AddItem(hParent, pKey);
		else if( i3::generic_is_iequal( pKey->GetName(), "WeaponList" )  )
			Rec_AddWeapon(hParent, pKey);
	}
	else
	{
		hItem = AddTree( TreeCtrl, hParent, pKey);
		if( GetItemWeaponPef() && TreeCtrl == m_SubTree )
		{
			if( hParent == TVI_ROOT )
				m_pSubTreeFirstItem = hItem;
		}
	}

	for (INT32 i =0; i < pKey->getChildCount(); i++)
	{
		i3RegKey * pChild = (i3RegKey*)pKey->getChild( i);
		Rec_AddTree( TreeCtrl, hItem, pChild);
	}
}

void	i3TDKRegistryCtrl_Impl::Rec_SortTree( i3RegKey* pKey, REGISTRYCTRL_SORT_TYPE sort_type )
{
	i3RegKey* pChild = NULL;
  
	for ( int i =0; i < pKey->getChildCount(); i++ )
	{
		pChild = (i3RegKey*)pKey->getChild( i );
		
		if ( pChild->getChildCount() > 0 )
		{
			Rec_SortTree( pChild, sort_type );
		}
	}

	pChild = (i3RegKey*)pChild->getParent();

	if ( pChild != NULL )
	{
		i3::vector<i3RegKey*>& ChildList = reinterpret_cast< i3::vector<i3RegKey*>&  >(pChild->getChildList());
	

		if ( sort_type == SORT_TYPE_KEY )
		{
			i3::sort(ChildList.begin(), ChildList.end(), CompareNameProc);
		//	pChildList->Sort( (COMPAREPROC)CompareNameProc );
		}
		else if( sort_type == SORT_TYPE_TIME )
		{
			i3::sort(ChildList.begin(), ChildList.end(), CompareTimeProc);
		//	pChildList->Sort( (COMPAREPROC)CompareTimeProc );						
		}
	}
}

void	i3TDKRegistryCtrl_Impl::Rec_CheckTimeStampNormal( i3RegKey* pKey )
{
	i3RegKey* pChild = NULL;
  
	for ( int i =0; i < pKey->getChildCount(); i++ )
	{
		pChild = (i3RegKey*)pKey->getChild( i );

		if ( pChild->getChildCount() > 0 )
		{
			Rec_CheckTimeStampNormal( pChild );
		}

		if ( pChild->FindDataByName( "__Make Time" ) == NULL || pChild->FindDataByName( "__Update Time" ) == NULL )
		{
 			if (m_ItemListViewCtrl.GetPefFileType() == PFT_MULTI)
 			{
 				size_t NationSize = m_ItemListViewCtrl.GetNationListRef()->size();
 
 				i3RegArray * pNewArrayData = NULL;
 
 				pNewArrayData = _CreateRegArrayStringData("__Make Time", "0", NationSize);
 				pChild->AddData(pNewArrayData);
 
 				pNewArrayData = _CreateRegArrayStringData("__Update Time", "0", NationSize);
 				pChild->AddData(pNewArrayData);
 			}
 			else
 			{
 				pChild->AddData("__Make Time", L"0");
 				pChild->AddData("__Update Time", L"0");
 			}
			
		}
		_UpdateDataTimeStamp(pChild);
 		//i3RegData* pmakeTime = pChild->FindDataByName("__Make Time");
 		//i3RegData* pupdateTime = pChild->FindDataByName("__Update Time");
		
// 		if (pmakeTime)
// 		{
// 			pChild->RemoveData(pmakeTime);
// 		}
// 		if (pupdateTime)
// 		{
// 			pChild->RemoveData(pupdateTime);
// 		}
	}
}

void	i3TDKRegistryCtrl_Impl::Rec_CheckTimeStampSingle( i3RegKey* pKey )
{
	i3RegKey* pChild = NULL;
  
	for ( int i =0; i < pKey->getChildCount(); i++ )
	{
		pChild = (i3RegKey*)pKey->getChild( i );

		if ( pChild->getChildCount() > 0 )
		{
			Rec_CheckTimeStampSingle( pChild );
		}

 		if ( pChild->FindDataByName( "__Make Time" ) == NULL || pChild->FindDataByName( "__Update Time" ) == NULL )
 		{
 			pChild->AddData( "__Make Time", L"0" );
 			pChild->AddData( "__Update Time", L"0" );
 		}

// 		i3RegData* pmakeTime = pChild->FindDataByName("__Make Time");
// 		i3RegData* pupdateTime = pChild->FindDataByName("__Update Time");
// 		if (pmakeTime)
// 		{
// 			pChild->RemoveData(pmakeTime);
// 		}
// 		if (pupdateTime)
// 		{
// 			pChild->RemoveData(pupdateTime);
// 		}
	}
}

void	i3TDKRegistryCtrl_Impl::Rec_CheckTimeStampMulti( i3RegKey* pKey )
{
	i3RegKey* pChild = NULL;
  
	for ( int i =0; i < pKey->getChildCount(); i++ )
	{
		pChild = (i3RegKey*)pKey->getChild( i );

		if ( pChild->getChildCount() > 0 )
		{
			Rec_CheckTimeStampMulti( pChild );
		}

		if (pChild->FindDataByName("__Make Time") == NULL || pChild->FindDataByName("__Update Time") == NULL)
		{
			size_t NationSize = m_WeaponListViewCtrl.GetNationListRef()->size();

			i3RegArray * pNewArrayData = NULL;

			pNewArrayData = _CreateRegArrayStringData("__Make Time", "0", NationSize);
			pChild->AddData(pNewArrayData);

			pNewArrayData = _CreateRegArrayStringData("__Update Time", "0", NationSize);
			pChild->AddData(pNewArrayData);
		}

// 		i3RegData* pmakeTime = pChild->FindDataByName("__Make Time");
// 		i3RegData* pupdateTime = pChild->FindDataByName("__Update Time");
// 		if (pmakeTime)
// 		{
// 			pChild->RemoveData(pmakeTime);
// 		}
// 		if (pupdateTime)
// 		{
// 			pChild->RemoveData(pupdateTime);
// 		}
	}
}

void	i3TDKRegistryCtrl_Impl::_AddNewKey(CTreeCtrl & TreeCtrl, HTREEITEM hParentItem, bool bMultiNation)
{
	i3RegKey * pParent = (i3RegKey *)TreeCtrl.GetItemData( hParentItem);

	CString NewKwyName = GetNewKeyName(pParent);

	if( GetItemWeaponPef() && TreeCtrl == m_TreeCtrl )
	{
		++m_MaxClassTypeIndex;

		CString Message;
		Message.Format( "%s 키 값에 대한 ClassType는 [%d] 이다.\n"
			"Client Source - CommonDef.h 소스에 \n"
			"enum WEAPON_CLASS_TYPE 타입에 추가 및 _CalssType이 동일해야 한다.",
			NewKwyName,
			m_MaxClassTypeIndex);

		if( AfxMessageBox( Message,  IDOK | MB_DEFBUTTON1) == IDOK )
			AddClassType( NewKwyName.GetString(), m_MaxClassTypeIndex, true);
	}

//	i3RegKey * pParent = (i3RegKey *)TreeCtrl.GetItemData( hParentItem);
	I3ASSERT( pParent != NULL);

	i3RegKey * pNew = i3RegKey::new_object_ref();
	pNew->SetName( NewKwyName.GetString() );

	pParent->AddChild( pNew);

	
	HTREEITEM hCurItem = AddTree( TreeCtrl, hParentItem, pNew);
	
	this->SelectItemInTreeCtrl(hCurItem);

	i3TDKListCtrl_Impl* updateListCtrl = NULL;
	
	if( *m_FocusTreeCtrl == m_TreeCtrl )
	{
		updateListCtrl = &m_ItemListViewCtrl;
	}
	else if ( *m_FocusTreeCtrl == m_SubTree )
	{
		updateListCtrl = &m_WeaponListViewCtrl;
	}

	// 생성시간 추가
	
	if (updateListCtrl)
	{
		UINT currPFT = updateListCtrl->GetPefFileType();
		UINT tgtPFT  = (bMultiNation) ? PFT_MULTI : PFT_SINGLE;

		if (currPFT != tgtPFT)
		{
			if (tgtPFT == PFT_SINGLE)
			{
				updateListCtrl->SetSingleNation();
			}
			else
			{

				if (updateListCtrl->IsNationFileListEmpty())
				{
					updateListCtrl->SetSingleNation();
				}
				else
				{
					updateListCtrl->SetMultiNation(CListSubCtrl::ACT_CHANGEKEY);
				}
			}
		}
		_AddTimeStampData( *updateListCtrl, hCurItem );
	}

}

void	i3TDKRegistryCtrl_Impl::_DeleteKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem)
{
	i3RegKey * pKey = (i3RegKey *)TreeCtrl.GetItemData( hItem);
	I3ASSERT( pKey != NULL);

	if( pKey->getNodeStyle() & I3REG_STYLE_ROOT)
	{
		AfxMessageBox( "제거할수 없는 노드입니다.", MB_OK, 0);
		return;
	}

	if( GetItemWeaponPef() && TreeCtrl == m_TreeCtrl )
	{
		DelClassType( m_CurrentWeaponType );
		m_CurrentWeaponType = "";
	}


	i3RegKey * pParent = (i3RegKey*)pKey->getParent();
	I3ASSERT( pParent != NULL);

	pParent->RemoveChild( pKey);
	TreeCtrl.DeleteItem( hItem);
}

void	i3TDKRegistryCtrl_Impl::_RenameKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem)
{
	i3RegKey * pKey = (i3RegKey *)TreeCtrl.GetItemData( hItem);
	I3ASSERT( pKey != NULL);

	::SendMessage( TreeCtrl.m_hWnd, TVM_EDITLABEL, 0, (LPARAM)hItem);
}

void	i3TDKRegistryCtrl_Impl::_CopyKey( CTreeCtrl & TreeCtrl, HTREEITEM hItem)
{
	m_SrcItem = hItem;
}

void i3TDKRegistryCtrl_Impl::_UpdateDataTimeStamp(i3RegKey* pKey)
{
	i3RegKey * pChild;
	i3RegData *pData;

	//만든 시간을 검색해서 있는 경우 수정한다.
	pData = pKey->FindDataByName("__Make Time");
	if (pData)
	{
		UpdateTimeStamp();

		i3::rc_wstring	wstrTimeStamp;
		i3::mb_to_utf16(m_szTimeStamp, wstrTimeStamp);

		if (pData->getDataType() == I3REG_TYPE_ARRAY)
		{
			i3RegArray * pArrayData = (i3RegArray*)pData;


			size_t size = pArrayData->getSize();
			for (size_t i = 0; i < size; i++)
			{
				i3RegString* Data = (i3RegString*)(*pArrayData)[i];
				Data->setValue(wstrTimeStamp);
			}
		}
		else
		{
			i3RegString* Data = (i3RegString*)pData;
			if (I3REG_TYPE_STRING == Data->getDataType())
				Data->setValue(wstrTimeStamp);
		}

		pData = pKey->FindDataByName("__Update Time");
		if (pData)
		{
			if (pData->getDataType() == I3REG_TYPE_ARRAY)
			{
				i3RegArray * pArrayData = (i3RegArray*)pData;

				size_t size = pArrayData->getSize();
				for (size_t i = 0; i < size; i++)
				{
					i3RegString* Data = (i3RegString*)(*pArrayData)[i];
					Data->setValue(wstrTimeStamp);
				}
			}
			else
			{
				i3RegString* Data = (i3RegString*)pData;
				if (I3REG_TYPE_STRING == Data->getDataType())
					Data->setValue(wstrTimeStamp);
			}
		}
	}

	for (INT32 i = 0; i < pKey->getChildCount(); i++)
	{
		pChild = (i3RegKey*)pKey->getChild(i);
		//다시 호출한다.
		_UpdateDataTimeStamp(pChild);
	}
}

void	i3TDKRegistryCtrl_Impl::_PasteKey(CTreeCtrl & TreeCtrl,  HTREEITEM hItem)
{
	I3ASSERT( m_SrcItem != NULL);
	I3ASSERT( hItem != NULL);

	i3RegKey * pKey = (i3RegKey *)TreeCtrl.GetItemData( hItem);
	I3ASSERT( pKey != NULL);

	i3RegKey * pSrcKey = ( i3RegKey*)TreeCtrl.GetItemData( m_SrcItem);
	I3ASSERT( pSrcKey != NULL);

	i3RegKey * pNew = (i3RegKey*)i3RegKey::CreateClone( pSrcKey, I3_COPY_INSTANCE);
	CString strTemp;
	strTemp.Format("[TEMP] %s", pSrcKey->GetName());
	pNew->SetName((const char*)strTemp);

	_UpdateDataTimeStamp(pNew);

	i3RegKey* attachKey = pKey;
	HTREEITEM attachHItem = hItem;

	// 타겟과 자신이 동일할경우 부모의 자식으로 추가
	if(m_SrcItem == hItem)
	{
		attachKey = (i3RegKey*)pKey->getParent();
		attachHItem = TreeCtrl.GetParentItem(hItem);
	}

	attachKey->AddChild(pNew);
	Rec_AddTree(TreeCtrl, attachHItem, pNew);
}

void	i3TDKRegistryCtrl_Impl::_FindKey()
{

}

void	i3TDKRegistryCtrl_Impl::_Sort( REGISTRYCTRL_SORT_TYPE sorttype )
{
	Rec_SortTree( m_pRoot, sorttype );
	UpdateAll();
}

void	i3TDKRegistryCtrl_Impl::_GenerateTimeStamp()
{
//	Rec_CheckTimeStamp( m_pRoot );

	if( GetItemWeaponPef() )
	{
		int Count = m_pRoot->getChildCount();
		for( int i = 0; i<Count; ++i )
		{
			i3RegKey* pKey = (i3RegKey*)m_pRoot->getChild( i );
			if( i3::generic_is_iequal( pKey->GetName(), "ClassType" )  )
				Rec_CheckTimeStampSingle(pKey);
			else if( i3::generic_is_iequal( pKey->GetName(), "ItemList" )  )
				Rec_CheckTimeStampSingle(pKey);
			else if( i3::generic_is_iequal( pKey->GetName(), "WeaponList" ) )
				Rec_CheckTimeStampMulti(pKey);
		}
	}
	else
		Rec_CheckTimeStampNormal( m_pRoot );
}




void	i3TDKRegistryCtrl_Impl::_AddNewWeaponData(i3TDKListCtrl_Impl & ListCtrl_Impl, HTREEITEM hParentItem)
{
	if( m_CurrentClassTypeIndex == 0 )
	{
		AfxMessageBox( "Could not create New Weapon Data in hear!" );
		return;
	}

	CWeaponContextDialog Dlg;

	Dlg.SetClassType( m_CurrentClassTypeIndex );
	Dlg.SetNumber( m_ItemListViewCtrl.GetAbleWeaponNumber() );
	Dlg.SetRegRoot( m_pWeaponRoot );

	i3RegKey * pParent = (i3RegKey *)GetFriendTreePtr()->GetItemData( m_pTreeCurItem );
	Dlg.SetBaseRegKey( pParent );

	if( Dlg.DoModal() == IDOK )
	{
		m_ItemListViewCtrl.AddNewData( I3REG_TYPE_INT32, Dlg.GetNumber()  );

		//	새로운 아이템을 등록 및 m_WeaponDictionary 에 등록 한다.
		AddWeaponDictionary(
			m_CurrentWeaponType.c_str(),
			Dlg.GetNumber());

		CString NewRegistryName = Dlg.GetRegistryName();

		//현재 선택한 Registry 위치
		i3RegKey* pRegKey = Dlg.GetSelectedRegistry();
		INT32 BaseDataIndex = Dlg.GetSelectedBaseDataIndex();

		//자식을 하나 만든다.
		i3RegKey * pNewKey = NULL;

		if( BaseDataIndex == -1 )
		{
			pNewKey = i3RegKey::new_object_ref();

			m_WeaponListViewCtrl._AddNewData( pNewKey, "_ClassType", m_CurrentClassTypeIndex );
			m_WeaponListViewCtrl._AddNewData( pNewKey, "_Number", Dlg.GetNumber() );
		}
		else
		{
			WeaponRegBank* pRegBank = _GetWeaponRegKey(BaseDataIndex);

			if( pRegBank->empty() && pRegBank->size() != 1 )
			{
				AfxMessageBox("Base Data Registry invalided");
				return;
			}

			i3RegArray * pNumberArrayData = (i3RegArray*)i3Registry::FindData( pRegBank->at(0), "_Number" );

			if( pNumberArrayData )
			{
				pNewKey = (i3RegKey*)i3RegKey::CreateClone( pRegBank->at(0), I3_COPY_INSTANCE);

				INT32 Number = (INT32)Dlg.GetNumber();
				size_t size = pNumberArrayData->getSize();
				for(size_t i = 0; i<size; ++i)
				{
					pNumberArrayData->replaceRegData(i, Number);
				}
			}
			else
			{
				AfxMessageBox("There are No ClassType & No Number\n Base Data Registry invalided! ");
				return;
			}

			_UpdateDataTimeStamp(pNewKey);

			pNewKey->SetName( NewRegistryName.GetString() );
			pRegKey->AddChild( pNewKey);

			RegistryWeaponDictionary( 
				Dlg.GetClassType(),
				Dlg.GetNumber(),
				pNewKey );
		}
	}
}


void	i3TDKRegistryCtrl_Impl::_AddNewData(HTREEITEM hParentItem, INT32 nNewDataType)
{
	i3TDKListCtrl_Impl* pListCtrl = GetFriendListPtr();

	if( pListCtrl )
		_AddNewData(*pListCtrl, hParentItem, nNewDataType);
}


void	i3TDKRegistryCtrl_Impl::_AddNewData(i3TDKListCtrl_Impl & ListCtrl_Impl, HTREEITEM hParentItem, INT32 nNewDataType)
{
	i3RegKey * pParent = (i3RegKey *)GetFriendTreePtr()->GetItemData( hParentItem);
	I3ASSERT( pParent != NULL);

	ListCtrl_Impl.SetRegKey( pParent, CListSubCtrl::ACT_CHANGEKEY);
	ListCtrl_Impl.AddNewData( nNewDataType );

	UpdateTimeStamp();

	ListCtrl_Impl.UpdateTimeStampData( "__Update Time", m_szTimeStamp, m_szTimeStampString );
}

void	i3TDKRegistryCtrl_Impl::_CopyDataLeft(i3TDKListCtrl_Impl & ListCtrl_Impl, HTREEITEM hParentItem, int nRow, int nCol)
{
	ITR_ITEM _iterBegin = m_conSelected.begin();
	ITR_ITEM _iterEnd = m_conSelected.end();

	i3RegKey * pParent = (i3RegKey *)GetFriendTreePtr()->GetItemData( hParentItem);
	I3ASSERT( pParent != NULL);
	nCol = nCol - 2;
	while( _iterEnd != _iterBegin )
	{
		i3RegData* pParentData = pParent->GetData((*_iterBegin));
		if( pParentData->getDataType() == I3REG_TYPE_ARRAY ) 
		{
			_CopyRegData( (i3RegArray *)pParentData, nCol, 0, nCol );
		}
		++_iterBegin;
	}

	ListCtrl_Impl.SetRegKey( pParent, CListSubCtrl::ACT_COPYCELL);
}

void	i3TDKRegistryCtrl_Impl::_CopyDataRight(i3TDKListCtrl_Impl & ListCtrl_Impl, HTREEITEM hParentItem, int nRow, int nCol)
{
	ITR_ITEM _iterBegin = m_conSelected.begin();
	ITR_ITEM _iterEnd = m_conSelected.end();

	i3RegKey * pParent = (i3RegKey *)GetFriendTreePtr()->GetItemData( hParentItem);
	I3ASSERT( pParent != NULL);

	nCol = nCol - 2;
	while( _iterEnd != _iterBegin )
	{
		i3RegData* pParentData = pParent->GetData((*_iterBegin));
		if( pParentData->getDataType() == I3REG_TYPE_ARRAY ) 
		{
			i3RegArray * pArrayRegData = (i3RegArray *)pParentData;
			_CopyRegData(pArrayRegData, nCol, nCol, pArrayRegData->getSize() );
		}
		++_iterBegin;
	}

	ListCtrl_Impl.SetRegKey( pParent, CListSubCtrl::ACT_COPYCELL);
}

void	i3TDKRegistryCtrl_Impl::_CopyDataWhole(i3TDKListCtrl_Impl & ListCtrl_Impl, HTREEITEM hParentItem, int nRow, int nCol)
{
	ITR_ITEM _iterBegin = m_conSelected.begin();
	ITR_ITEM _iterEnd = m_conSelected.end();

	i3RegKey * pParent = (i3RegKey *)GetFriendTreePtr()->GetItemData( hParentItem);
	I3ASSERT( pParent != NULL);

	nCol = nCol - 2;
	while( _iterEnd != _iterBegin )
	{
		i3RegData* pParentData = pParent->GetData((*_iterBegin));
		if( pParentData->getDataType() == I3REG_TYPE_ARRAY ) 
		{
			i3RegArray * pArrayRegData = (i3RegArray *)pParentData;
			_CopyRegData( (i3RegArray *)pParentData, nCol, 0, pArrayRegData->getSize() );
		}
		++_iterBegin;
	}
	
	ListCtrl_Impl.SetRegKey( pParent, CListSubCtrl::ACT_COPYCELL);
}


void	i3TDKRegistryCtrl_Impl::_AddTimeStampData(i3TDKListCtrl_Impl & ListCtrl_Impl, HTREEITEM hParentItem )
{
	i3RegKey * pParent = (i3RegKey *)GetFriendTreePtr()->GetItemData( hParentItem );
	I3ASSERT( pParent != NULL);

	UpdateTimeStamp();

 	ListCtrl_Impl.AddTimeStampData( pParent, "__Make Time", m_szTimeStamp, m_szTimeStampString );
 	ListCtrl_Impl.AddTimeStampData( pParent, "__Update Time", m_szTimeStamp, m_szTimeStampString );
 
 	ListCtrl_Impl.SetRegKey( pParent, CListSubCtrl::ACT_CHANGEKEY);
}

void i3TDKRegistryCtrl_Impl::_CopyData(i3TDKListCtrl_Impl & ListCtrl_Impl)
{

	_clearTemp();

	ITR_ITEM _iter = m_conSelected.begin();
	while( m_conSelected.end() != _iter )
	{
		INT32 nItem = (*_iter);
		i3RegData * pSrcData = (i3RegData *)ListCtrl_Impl.GetItemData( nItem );
		if( pSrcData )
		{
			m_conTempRegData.push_back( pSrcData->create_instance() );
			pSrcData->CopyTo( m_conTempRegData.back(), I3_COPY_INSTANCE );
			m_conTempRegData.back()->AddRef();
		}
		++_iter;
	}
}

void i3TDKRegistryCtrl_Impl::_PasteData(i3TDKListCtrl_Impl & ListCtrl_Impl)
{
	if( !m_conTempRegData.empty() && m_pCurItem != NULL)
	{
		ITR_REG_DATA _iterBegin = m_conTempRegData.begin();
		ITR_REG_DATA _iterEnd = m_conTempRegData.end();

		INT32 numPasteCount = 0;

		while( _iterEnd != _iterBegin )
		{
			ListCtrl_Impl.PasteData( (*_iterBegin) );
			++_iterBegin;

			numPasteCount++;
		}

		if(m_FocusListCtrl)
			m_FocusListCtrl->_AddPasteRowToCustomDraw(numPasteCount);
	}
}

//void	i3TDKRegistryCtrl_Impl::_RenameData( CTreeCtrl & TreeCtrl, INT32 nRow)
//{
//	::SendMessage( TreeCtrl.m_hWnd, LVM_EDITLABEL, (WPARAM)nRow, 0);
//}

void	i3TDKRegistryCtrl_Impl::_DeleteData(i3TDKListCtrl_Impl & ListCtrl_Impl)
{
	bool bWeaponPef =  ( GetItemWeaponPef() && ListCtrl_Impl == m_ItemListViewCtrl ) ? true : false;
	CON_ITEM::reverse_iterator _iterBegin = m_conSelected.rbegin();
	CON_ITEM::reverse_iterator _iterEnd = m_conSelected.rend();
	while( _iterEnd != _iterBegin )
	{
		INT32 nItem = (*_iterBegin);
		if( bWeaponPef )
		{
			i3RegData * pData = (i3RegData*)ListCtrl_Impl.GetDataPtr( nItem );
			if( pData )
			{
				INT32 Value = 0;
				if( pData->getDataType() == I3REG_TYPE_INT32 )
				{
					((i3RegINT32*)pData)->GetValue( &Value );
				}
				DelWeaponData( m_CurrentWeaponType, Value );
			}
		}
		ListCtrl_Impl.DeleteData( nItem );
		++_iterBegin;
	}
}

void	i3TDKRegistryCtrl_Impl::_CollapseNode( CTreeCtrl & TreeCtrl, HTREEITEM item)
{
	TreeCtrl.Expand( item, TVE_COLLAPSE);
}

void	i3TDKRegistryCtrl_Impl::_ExpandNode(CTreeCtrl & TreeCtrl, HTREEITEM item)
{
	TreeCtrl.Expand( item, TVE_EXPAND);
}

void	i3TDKRegistryCtrl_Impl::_ExprotSqlUpdate( CTreeCtrl & TreeCtrl, HTREEITEM item )
{
	if( item == NULL)	return;

	CFileDialog	Dlg( FALSE, "sql", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"sql File(*.sql)|*.sql||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	CString FileName = Dlg.GetPathName();

	i3RegKey * pKey = (i3RegKey*)TreeCtrl.GetItemData( item);
	I3ASSERT( pKey != NULL);

	i3RegKeyFile RegKeyFile;
	RegKeyFile.setRoot( pKey);
	RegKeyFile.OnExportSQLUpdate( FileName ); 

	return; 
}

void	i3TDKRegistryCtrl_Impl::_ExprotSqlCreate( CTreeCtrl & TreeCtrl, HTREEITEM item )
{
	if( item == NULL)	return;

	CFileDialog	Dlg( FALSE, "sql", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"sql File(*.sql)|*.sql||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	CString FileName = Dlg.GetPathName();

	i3RegKey * pKey = (i3RegKey*)TreeCtrl.GetItemData( item);
	I3ASSERT( pKey != NULL);

	i3RegKeyFile RegKeyFile;
	RegKeyFile.setRoot( pKey);
	RegKeyFile.OnExportSQLCreate( FileName ); 

	return; 
}

void	i3TDKRegistryCtrl_Impl::_ExportKey( CTreeCtrl & TreeCtrl, HTREEITEM item)
{
	if( item == NULL)	return;

	CFileDialog	Dlg( FALSE, "i3Rkf", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"i3RegKey File(*.i3Rkf)|*.i3Rkf||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	CString FileName = Dlg.GetPathName();

	i3RegKey * pKey = (i3RegKey*)TreeCtrl.GetItemData( item);
	I3ASSERT( pKey != NULL);

	i3RegKeyFile* RegKeyFile = i3RegKeyFile::new_object_ref();
	RegKeyFile->setRoot( pKey);

	i3ResourceFile File;
	i3::vector<i3PersistantElement*> list;
	list.push_back( RegKeyFile);

	File.SetObjects( list);
	if( File.Save( FileName) == STREAM_ERR)
	{
		I3TRACE("Could not save Regkeyfile\n");
		return;
	}
}

void i3TDKRegistryCtrl_Impl::_ExportKeyEx(CTreeCtrl & TreeCtrl)
{
	std::vector<HTREEITEM>& items = m_selectedTreeItem;
	if(items.empty()) return;

	CFileDialog	Dlg( FALSE, "Multiple export i3Rkf", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"i3RegKey File(*.i3Rkf)|*.i3Rkf||", this);

	if( Dlg.DoModal() != IDOK)
		return;

	CString FileName = Dlg.GetPathName();
	char pathOnly[MAX_PATH] = {0};
	strcpy_s(pathOnly, MAX_PATH, FileName.GetBuffer());
	char* p = strrchr(pathOnly, '\\');

	if(p)
		*p = NULL;

	for(INT32 i=0; i<(INT32)items.size(); i++)
	{
		char savePath[MAX_PATH] = {0};

		i3RegKey * pKey = (i3RegKey*)TreeCtrl.GetItemData(items[i]);
		I3ASSERT( pKey != NULL);

		sprintf_s(savePath, MAX_PATH, "%s\\%s.i3Rkf",pathOnly, pKey->GetName());

		i3RegKeyFile* RegKeyFile = i3RegKeyFile::new_object_ref();
		RegKeyFile->setRoot( pKey);

		i3ResourceFile File;
		i3::vector<i3PersistantElement*> list;
		list.push_back( RegKeyFile);

		File.SetObjects(list);

		if( File.Save(savePath) == STREAM_ERR)
		{
			I3TRACE("ERR! - Could not save Regkeyfile \n");
		}
	}
}

void i3TDKRegistryCtrl_Impl::_AddSelectedKeyToList(CTreeCtrl& TreeCtrl)
{
	HTREEITEM hItem = TreeCtrl.GetSelectedItem();

	if(!hItem)
		return;

	m_selectedTreeItem.push_back(hItem);
}

void i3TDKRegistryCtrl_Impl::_ShowSelectedKey(CTreeCtrl & TreeCtrl)
{
	std::string keyNames;
	char name[MAX_PATH]={0};

	for(INT32 i=0; i<(INT32)m_selectedTreeItem.size(); i++)
	{
		HTREEITEM curTreeItem = m_selectedTreeItem[i];
		i3RegKey* pKey = (i3RegKey*)TreeCtrl.GetItemData(curTreeItem);
		sprintf_s(name, MAX_PATH, "[%s] %s", pKey->GetName(), (i%5 == 0) ? " " : "\n");
		keyNames += std::string(name);					
	}
	MessageBox(keyNames.c_str(), "Selected List", MB_OK);
}

void i3TDKRegistryCtrl_Impl::_RemoveSelectedKeyList()
{
	m_selectedTreeItem.clear();
}

void	i3TDKRegistryCtrl_Impl::_ImportKeyEx(CTreeCtrl & TreeCtrl, HTREEITEM item)
{

	CFileDialog	Dlg( TRUE, 
		"i3Rkf", 
		NULL, 
		OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"i3RegKey File(*.i3Rkf)|*.i3Rkf||", 
		this);

	CString fileList;
	INT32 numMaxFile = 200;
	Dlg.GetOFN().lpstrFile = fileList.GetBuffer(numMaxFile*(_MAX_PATH+1)+1);
	Dlg.GetOFN().nMaxFile =numMaxFile*(_MAX_PATH+1)+1;
	
	if( Dlg.DoModal() != IDOK)
		return;

	// x:\\path file1 file2 ...

	char curPath[MAX_PATH] = {0};
	char curFile[MAX_PATH] = {0};
	const char* curPos = fileList.GetBuffer();
	bool first = true;

	do
	{
		curFile[0] = NULL;
		strcpy_s(curFile, MAX_PATH, curPos);

		char loadFilePath[MAX_PATH] = {0};

		// skip first
		if(first)
		{
			strcpy_s(curPath, MAX_PATH, curFile);
			first = false;

			curPos += strlen(curFile) + 1;

			if(!(*curPos))
			{
				char* p = strrchr(curPath, '\\');
				if(p) *p = NULL;
				strcpy_s(curFile, MAX_PATH, p+1);
			}
			else
				continue;
		}

		sprintf_s(loadFilePath, MAX_PATH, "%s\\%s", curPath, curFile);

		i3ResourceFile File;

		if(File.Load(loadFilePath) == STREAM_ERR)
		{
			I3TRACE("Can't Load RegkeyFile\n");
			fileList.ReleaseBuffer();
			return;
		}

		INT32 nCount = File.GetObjectCount();
		for( INT32 i = 0; i < nCount; ++i)
		{
			i3PersistantElement * pElem = File.GetObject( i);
			if( i3::same_of<i3RegKeyFile*>(pElem)) //->IsExactTypeOf( i3RegKeyFile::static_meta()) )
			{
				i3RegKeyFile * pRegKeyFile = (i3RegKeyFile*)pElem;
				i3RegKey * pRoot = pRegKeyFile->getRoot();
				if( pRoot != NULL)
				{
					i3RegKey * pParentKey = (i3RegKey*)TreeCtrl.GetItemData( item);
					I3ASSERT( pParentKey != NULL);

					pParentKey->AddChild( pRoot);
					Rec_AddTree( TreeCtrl, item, pRoot);

					pRegKeyFile->AddRef();
					I3_SAFE_RELEASE( pRegKeyFile);
				}
			}
		}

		curPos += strlen(curFile) + 1;

	} while(*curPos);


	fileList.ReleaseBuffer();

}

void	i3TDKRegistryCtrl_Impl::_ImportKey( CTreeCtrl & TreeCtrl, HTREEITEM item)
{
	CFileDialog	Dlg( TRUE, "i3Rkf", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3RegKey File(*.i3Rkf)|*.i3Rkf||", this);
	
	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	CString FileName = Dlg.GetPathName();

	i3ResourceFile File;
	if( File.Load( FileName) == STREAM_ERR)
	{
		I3TRACE("Can't Load RegkeyFile\n");
		return;
	}

	INT32 nCount = File.GetObjectCount();
	for( INT32 i = 0;i < nCount; ++i)
	{
		i3PersistantElement * pElem = File.GetObject( i);
		if( i3::same_of<i3RegKeyFile*>(pElem)) //->IsExactTypeOf( i3RegKeyFile::static_meta()) )
		{
			i3RegKeyFile * pRegKeyFile = (i3RegKeyFile*)pElem;
			i3RegKey * pRoot = pRegKeyFile->getRoot();
			if( pRoot != NULL)
			{
				i3RegKey * pParentKey = (i3RegKey*)TreeCtrl.GetItemData( item);
				I3ASSERT( pParentKey != NULL);

				pParentKey->AddChild( pRoot);
				Rec_AddTree( TreeCtrl, item, pRoot);

				pRegKeyFile->AddRef();
				I3_SAFE_RELEASE( pRegKeyFile);
			}
		}
	}
}

void i3TDKRegistryCtrl_Impl::UpdateTimeStamp()
{
	time_t osBinaryTime;  // C run-time time (defined in <time.h>)
	time( &osBinaryTime );

	CTime ct(osBinaryTime);
	_i64toa( osBinaryTime, m_szTimeStamp, 10 );

	int y,m,d,h,min,s;
	
	y=ct.GetYear();
	m=ct.GetMonth();
	d=ct.GetDay();
	h=ct.GetHour();
	min=ct.GetMinute();
	s=ct.GetSecond();

	//sprintf( m_szTimeStampString, "%04d-%02d-%02d %02d:%02d:%02d", y, m, d, h, min, s );
	sprintf(m_szTimeStamp, "0");
	sprintf(m_szTimeStampString, "0");
}