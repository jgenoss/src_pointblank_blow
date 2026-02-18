#include "StdAfx.h"
#include "i3TDKRegistryCtrl_Impl2.h"

#include "../../PropertyEditor/GlobalVariable.h"
#include "../MainFrm.h"



i3TDKRegistryCtrl_Impl2::i3TDKRegistryCtrl_Impl2(void)
{
	m_pContext	= NULL;
}

i3TDKRegistryCtrl_Impl2::~i3TDKRegistryCtrl_Impl2(void)
{
}



BEGIN_MESSAGE_MAP(i3TDKRegistryCtrl_Impl2, i3TDKRegistryCtrl_Impl)
	ON_WM_CREATE()
END_MESSAGE_MAP()



int i3TDKRegistryCtrl_Impl2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect( &rect);

	{
		if( m_TreeCtrl.Create( m_dwStyle | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VISIBLE | 
			TVS_HASBUTTONS | TVS_HASLINES | TVS_EDITLABELS | TVS_SHOWSELALWAYS |
			TVS_INFOTIP | TVS_LINESATROOT,
			rect, this, REG_TREECTRL) == FALSE)
		{
			I3FATAL( "Could not Create TreeCtrl");
		}

		if( m_SubTree.Create( m_dwStyle | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VISIBLE | 
			TVS_HASBUTTONS | TVS_HASLINES | TVS_EDITLABELS | TVS_SHOWSELALWAYS |
			TVS_INFOTIP | TVS_LINESATROOT,
			CRect(0,0,0,0), this, REG_TREECTRL) == FALSE)
		{
			I3FATAL( "Could not Create SubTreeCtrl");
		}
		m_SubTree.ShowWindow( FALSE );
	}

	{
		if( m_ItemListViewCtrl.Create( m_dwStyle | WS_BORDER | WS_VISIBLE , rect, this, REG_ITEM_LISTCTRL) == FALSE)
		{
			I3FATAL( "Could not Create ItemListView");
		}
		m_ItemListViewCtrl.SetUseCellLClickList( true );
		m_ItemListViewCtrl.Set_Name("List1");
		m_ItemListViewCtrl.ShowWindow( TRUE );

		if( m_WeaponListViewCtrl.Create( m_dwStyle | WS_BORDER | WS_VISIBLE , CRect(0,0,0,0), this, REG_WEAPON_LISTCTRL) == FALSE)
		{
			I3FATAL( "Could not Create WeaponListView");
		}
		m_WeaponListViewCtrl.SetUseCellLClickList( false );
		m_WeaponListViewCtrl.Set_Name("List2");
		m_WeaponListViewCtrl.ShowWindow( FALSE );
	}

	{
		if( m_ImageViewCtrl.Create( m_dwStyle | WS_BORDER, this, REG_IMAGE_VIEW) == FALSE)
		{
			I3FATAL( "Could not Create ImageView");
		}
		m_ImageViewCtrl.ShowWindow( FALSE );
	}
	{
		::SetCurrentDirectory( g_pProfile->m_WorkDirectoryPath );

		//m_pContext	= AfxGetMainFrame()->GetMainRenderView().GetViewerCtrl().getViewer()->GetRenderContext();
		//m_pContext->AddRef();
	}

	{
		m_pImageList			= i3TDK::GetSystemImageList();
		m_nIndexOfFirstTDKImage = i3TDK::AddTDKImageListTo( m_pImageList);

		m_TreeCtrl.SetImageList( m_pImageList, TVSIL_NORMAL);
	}

	i3TDK::RegisterUpdate( m_hWnd, i3RegData::static_meta(), I3_TDK_UPDATE_EDIT);


	{
		if( m_SubTreeToolTip.Create(this) == FALSE)
		{
			I3FATAL( "Could not Create Subtree Tooltip");
		}
	}

	return 0;
}

void i3TDKRegistryCtrl_Impl2::_AddNewKey(CTreeCtrl & TreeCtrl, HTREEITEM hParentItem)
{
	i3TDKRegistryCtrl_Impl::_AddNewKey(TreeCtrl, hParentItem);
	g_pPefList->OnUpdatePefList();

//	::SendMessage(g_HwndDockPropertyWnd)
}
void i3TDKRegistryCtrl_Impl2::_DeleteKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem)
{
	i3TDKRegistryCtrl_Impl::_DeleteKey(TreeCtrl, hItem);
	g_pPefList->OnUpdatePefList();
}
void i3TDKRegistryCtrl_Impl2::_RenameKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem)
{
	i3TDKRegistryCtrl_Impl::_RenameKey(TreeCtrl, hItem);

}
void i3TDKRegistryCtrl_Impl2::_CopyKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem)
{
	i3TDKRegistryCtrl_Impl::_CopyKey(TreeCtrl, hItem);
}

void i3TDKRegistryCtrl_Impl2::_PasteKey(CTreeCtrl & TreeCtrl, HTREEITEM hItem)
{
	i3TDKRegistryCtrl_Impl::_PasteKey(TreeCtrl, hItem);

	g_pPefList->OnUpdatePefList();
}

void i3TDKRegistryCtrl_Impl2::_PasteKey(CTreeCtrl & TreeCtrl,  HTREEITEM hItem,
										const CString& strClonedName, INT32 nNumber)
{
	//i3TDKRegistryCtrl_Impl::_PasteKey(TreeCtrl, hItem);
	
	I3ASSERT( m_SrcItem != NULL);
	I3ASSERT( hItem != NULL);

	i3RegKey * pKey = (i3RegKey *)TreeCtrl.GetItemData( hItem);
	I3ASSERT( pKey != NULL);

	i3RegKey * pSrcKey = ( i3RegKey*)TreeCtrl.GetItemData( m_SrcItem);
	I3ASSERT( pSrcKey != NULL);

	i3RegKey * pNew = (i3RegKey*)i3RegKey::CreateClone( pSrcKey, I3_COPY_INSTANCE);

	if (!strClonedName.IsEmpty())
		pNew->SetName((const char*)strClonedName);

	i3RegData* pData = pNew->FindDataByName("_ResName");
	if (pData && !strClonedName.IsEmpty())
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray * pArrayData = (i3RegArray*)pData;

			size_t size = pArrayData->getSize();
			for(size_t i =0; i<size; i++)
			{
				i3RegString* pCurrData = (i3RegString*)(*pArrayData)[i];
				i3::rc_wstring str;
				i3::mb_to_utf16( strClonedName.GetString(), str);
				pCurrData->setValue( str );
			}
		}
	}

	pData = pNew->FindDataByName("_Number");
	if (pData)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray * pArrayData = (i3RegArray*)pData;

			size_t size = pArrayData->getSize();
			for(size_t i =0; i<size; i++)
			{
				i3RegINT32* pCurrData = (i3RegINT32*)(*pArrayData)[i];
				pCurrData->setValue(nNumber);
			}
		}
	}

	_UpdateDataTimeStamp(pNew);

	pKey->AddChild( pNew);

	Rec_AddTree( TreeCtrl, hItem, pNew);

	g_pPefList->OnUpdatePefList();
}

void i3TDKRegistryCtrl_Impl2::GetHitemFullPath(CString& resPath, HTREEITEM item)
{
	if (!resPath.IsEmpty())
		resPath = m_TreeCtrl.GetItemText(item) + "/" + resPath;
	else
		resPath = m_TreeCtrl.GetItemText(item);

	HTREEITEM hParent = m_TreeCtrl.GetParentItem(item);
	if (hParent)
		GetHitemFullPath(resPath, hParent);
}

INT32 i3TDKRegistryCtrl_Impl2::InsertItemList(INT32 nClassIndex, const CString& strCloneName)
{
	i3RegKey* pRegKey		= g_pPefList->getRoot(E_PEF_WEAPON);
	i3TreeNode* pItemList	= pRegKey->FindChildByName("ItemList");
	i3RegKey* pClassList	= (i3RegKey*)pRegKey->FindChildByName("ClassType");

	for (int nClassCnt = 0; nClassCnt < pClassList->GetDataCount(); ++nClassCnt)
	{
		i3RegData* pData = pClassList->GetData(nClassCnt);
		INT32 nValue = 0;
		pData->GetValue(&nValue);

		//타입 값으로 클래스 이름을 검출
		if (nValue == nClassIndex)
		{
			const char* szDataName = pData->GetName();
			i3RegKey* pSelRegList = (i3RegKey*)pItemList->FindChildByName(szDataName);
			if (pSelRegList)
			{
				INT32 nMaxNum = 0;
				for (int nSelList = 0; nSelList < pSelRegList->GetDataCount(); ++nSelList)
				{
					i3RegData* pSelData = pSelRegList->GetData(nSelList);
					pSelData->GetValue(&nValue);

					if (nMaxNum < nValue)
						nMaxNum = nValue;
				}
				CString strDataName = CString(szDataName) +"_"+ strCloneName;
				strDataName.MakeUpper();

				pSelRegList->AddData(strDataName,(INT32)++nMaxNum);
				m_ItemListViewCtrl.SetRegKey(pSelRegList, CListSubCtrl::ACT_COPYCELL);

				return nMaxNum;
			}
			break;
		}
	}

	return -1;
}

void i3TDKRegistryCtrl_Impl2::OnSelectItem(const char* szName)
{
	HTREEITEM hItem = GetHItem(szName);

	if (hItem)
		m_TreeCtrl.SelectItem(hItem);

}

void i3TDKRegistryCtrl_Impl2::OnCloneItem(const char* szName, const char* szClonedName)
{
	mHitemListCom::iterator iter = m_mHitemList.find(szName);
	if (iter != m_mHitemList.end())
	{
		CString strClonedName = szClonedName;
		strClonedName.MakeUpper();

		_CopyKey(*m_mHitemList[szName].treeCtrl, m_mHitemList[szName].hItem);

		i3RegKey * pSrcKey = (i3RegKey*)m_TreeCtrl.GetItemData(m_SrcItem);
		
		INT32 nIndex = -1;
		FIND_REG_DATA(pSrcKey,"_ClassType",&nIndex);
		
		INT32 nNumber = InsertItemList(nIndex, strClonedName);

		if (nNumber == -1)
		{
			I3TRACE("Add Number To ItemList Failed\n");
		}

		//m_TreeCtrl.SetItemText(m_SrcItem,szClonedName);

		_PasteKey(*m_mHitemList[szName].treeCtrl, m_mHitemList[szName].tvData.hParent, strClonedName, nNumber);

		CString strFullPath;
		GetHitemFullPath(strFullPath, m_SrcItem);
		
		strFullPath = "Pef 데이터가 추가 되었습니다.\n위치:" + strFullPath;

		AfxMessageBox(strFullPath);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		cDlgFileLoader* pLoader = pFrame->GetFileLoader();
		pLoader->DoModal();

		g_pPefList->OnUpdatePefList();
	}
	else
	{
		I3ASSERT("PEF 복사에 실패 하였습니다.");
	}
}
