// PaneSpec.cpp : implementation file
//

#include "stdafx.h"
#include "i3RSCGen.h"
#include "PaneSpec.h"
#include ".\panespec.h"

#include "i3Base/string/ext/utf16_to_mb.h"

// CPaneSpec dialog
IMPLEMENT_DYNAMIC(CPaneSpec, CDialog)
CPaneSpec::CPaneSpec(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneSpec::IDD, pParent)
{
	m_pNode			= NULL;
}

CPaneSpec::~CPaneSpec()
{
}

void CPaneSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SPEC, m_ctrlList);
}


BEGIN_MESSAGE_MAP(CPaneSpec, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_SPEC, OnNMClickListSpec)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
//	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


void CPaneSpec::_ClearList()
{
	m_ctrlList.DeleteAllItems();
}

void CPaneSpec::SetNode( RSCNode * pNode)
{
	m_pNode = pNode;

	_SetNodeInfo( m_pNode);
}

void CPaneSpec::_SetNodeInfo( RSCNode * pNode)
{
	_ClearList();

	if( pNode == NULL)	return ;
	////////////////////////////////////////////////////
	UINT32 nNodeStyle = pNode->GetStyle();

	if( nNodeStyle & I3RSC_ATTR_FOLDER)
	{
		return;
	}
	else
	{
		RSC_DATA * pData = pNode->GetData();

		if( pData == NULL)
			return ;

		if( pData->m_spField == NULL)
			return ;

		CFieldPack * pField = pData->m_spField.get();

		const INT32 nCount = pField->GetFieldCount();
		
		for(INT32 i = 0; i< nCount; ++i)
		{
			_SetField( pField->GetField(i));
		}
	}
}

void CPaneSpec::_SetField( RSC_FIELD_INFO * pData)
{
	if( pData == NULL)	return;

	INT32 idx;
//	char	conv[MAX_PATH];
	i3::rc_string conv;
	
	idx = m_ctrlList.InsertItem( m_ctrlList.GetItemCount(), pData->m_strFieldName.c_str(), 0);

	if( pData->m_nFieldStyle & I3RSC_FIELD_TYPE_STRING)
	{
		const i3::rc_string* strVal = i3::unsafe_any_cast<i3::rc_string>(&pData->m_FieldData);
		m_ctrlList.SetItemText( idx, 1, strVal->c_str() );
	}
	else if( pData->m_nFieldStyle & I3RSC_FIELD_TYPE_INDEX)
	{
		INT32 N = *i3::unsafe_any_cast<int>(&pData->m_FieldData);

		if( i3::generic_is_iequal( pData->m_strFieldName, "Type") )
		{
			conv = g_RSCStyleDef[N].m_strFieldPackName;
		}
		else if( i3::generic_is_iequal( pData->m_strFieldName, "Layer") )
		{
			char szTemp[MAX_PATH];
			N += 1;

			if( g_pRegistry != NULL)
			{
				i3RegKey * pKey = FIND_REG_KEY( "SgLayer");
				if( pKey != NULL)
				{
					sprintf( szTemp, "Layer%d", N);

					i3::rc_wstring wstrReg;
					FIND_REG_DATA( pKey, szTemp, wstrReg );
					i3::utf16_to_mb(wstrReg, conv);
					// conv = szVal;
				}
			}
			else
			{
				conv = g_LayerTypeDef[N];
			}
		}
		else
		{
			i3::sprintf( conv, "%d", N);
			m_ctrlList.SetItemText( idx, 1, conv.c_str());
		}
		
		m_ctrlList.SetItemText( idx, 1, conv.c_str());
	}
	else if( pData->m_nFieldStyle & I3RSC_FIELD_TYPE_VALUE)
	{
		INT32 * N =	i3::unsafe_any_cast<int>(&pData->m_FieldData);	//	(INT32*)pData->m_pFieldData;
		i3::sprintf( conv, "%d", *N);
		m_ctrlList.SetItemText( idx, 1, conv.c_str());
	}
	else if( pData->m_nFieldStyle & I3RSC_FIELD_TYPE_REALVALUE)
	{
		REAL32 * L = i3::unsafe_any_cast<float>(&pData->m_FieldData);
		i3::sprintf( conv, "%f", *L);
		m_ctrlList.SetItemText( idx, 1, conv.c_str());
	}
}

void CPaneSpec::_EditItemValue( INT32 nItem, INT32 nCol)
{
	I3ASSERT( m_pNode != NULL);

	CFieldPack * pField = m_pNode->GetData()->m_spField.get();
	I3ASSERT( pField != NULL);

	RSC_FIELD_INFO * pInfo = pField->GetField( nItem);

	if( pInfo->m_nFieldStyle & I3RSC_FIELD_ATTR_UNEDITABLE)
		return;

	if( pInfo->m_nFieldStyle & I3RSC_FIELD_TYPE_STRING)
	{
		m_ctrlList.EditSubLabel( pInfo, nItem, nCol);
	}
	else if( pInfo->m_nFieldStyle & I3RSC_FIELD_TYPE_INDEX)
	{
		CStringList list;
		INT32 i;
		char szString[36];

		INT32 N = *i3::unsafe_any_cast<int>(&pInfo->m_FieldData);

		if( i3::generic_is_iequal( pInfo->m_strFieldName, "Type") )
		{
			for( i =0; i< I3RSC_TYPE_COUNT; ++i)
			{
				i3::sprintf(szString, g_RSCStyleDef[i].m_strFieldPackName);
				list.AddTail((LPCTSTR)szString);
			}
		}
		else if( i3::generic_is_iequal( pInfo->m_strFieldName, "Layer") )
		{
			if( g_pRegistry != NULL)
			{
				i3RegKey * pKey = FIND_REG_KEY( REG_LAYER_ROOT);
				if( pKey != NULL)
				{	
					i3::rc_wstring	wstrReg;
					i3::string str;

					INT32 nLayerCount = pKey->GetDataCount();
					for( i =0; i< nLayerCount; ++i)
					{
						char szName[MAX_PATH];
						sprintf( szName, "Layer%d", i);

						FIND_REG_DATA( pKey, szName, wstrReg );
						i3::utf16_to_mb(wstrReg, str);

						list.AddTail(str.c_str());
					}
				}
				else
				{
					for( i =0; i< I3RSC_LAYER_TYPECOUNT; ++i)
					{
						sprintf(szString, g_LayerTypeDef[i]);
						list.AddTail((LPCTSTR)szString);
					}
				}
			}
			else
			{
				for( i =0; i< I3RSC_LAYER_TYPECOUNT; ++i)
				{
					sprintf(szString, g_LayerTypeDef[i]);
					list.AddTail((LPCTSTR)szString);
				}
			}
		}
		else
		{
			for( i =0; i< 10; ++i)
			{
				sprintf(szString, "%d", i);
				list.AddTail((LPCTSTR)szString);
			}
		}

		m_ctrlList.SelectValue( pInfo, nItem, nCol, list, N);
	}
	else if( pInfo->m_nFieldStyle & I3RSC_FIELD_TYPE_VALUE)
	{
		m_ctrlList.EditSubLabel( pInfo, nItem, nCol);
	}
	else if( pInfo->m_nFieldStyle & I3RSC_FIELD_TYPE_REALVALUE)
	{
		m_ctrlList.EditSubLabel( pInfo, nItem, nCol);
	}
}
/////////////////////////////////////////////////////////////////////////////////
// CPaneSpec message handlers
void CPaneSpec::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_ctrlList))
	{
		m_ctrlList.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
		m_ctrlList.SetColumnWidth( 0, cx >> 2);
		m_ctrlList.SetColumnWidth( 1, cx - (cx >> 2) - 10);
	}
}

BOOL CPaneSpec::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	DWORD dwStyle = m_ctrlList.GetExtendedStyle();

	m_ctrlList.SetExtendedStyle( dwStyle | LVS_EX_GRIDLINES | LVS_EX_TRACKSELECT | LVS_EX_SUBITEMIMAGES  );

	m_ctrlList.InsertColumn(0, "Content", LVCFMT_LEFT, 64);
	m_ctrlList.InsertColumn(1, "Value", LVCFMT_LEFT, 400);

	i3TDK::RegisterUpdate( m_hWnd, RSCNode::static_meta(), I3_TDK_UPDATE_SELECT);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneSpec::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( pInfo->m_pObject != NULL)
		{
			if( i3::same_of<RSCNode*>(pInfo->m_pObject)) //->IsExactTypeOf( RSCNode::static_meta()) )
			{
				SetNode( (RSCNode*)pInfo->m_pObject);
			}
		}
		else
		{
			SetNode( NULL);
		}
	}
}

LRESULT CPaneSpec::WindowProc( UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc( message, wParam, lParam);
}


void CPaneSpec::OnNMClickListSpec(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CPoint point;

	//INT32 nRow, nColumn;
	GetCursorPos( &point);
	m_ctrlList.ScreenToClient( &point);

	LVHITTESTINFO lvhti;

	lvhti.pt = point;
	m_ctrlList.SubItemHitTest(&lvhti);

	if( lvhti.flags & LVHT_ONITEM)
	{
//		UINT flag = LVIS_FOCUSED;			바로 아래에 쓰이지만, 주석되어서 같이 주석처리..
		if(/* (m_ctrlList.GetItemState( lvhti.iItem, flag) & flag) == flag && */lvhti.iSubItem > 0)
		{
			if( GetWindowLong( m_ctrlList.m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
			{
				_EditItemValue( lvhti.iItem, lvhti.iSubItem);
			}
		}
		else
		{
			m_ctrlList.SetItemState( lvhti.iItem, LVIS_SELECTED | LVIS_FOCUSED,	LVIS_SELECTED | LVIS_FOCUSED);
		}
	}

	*pResult = 0;
}

void CPaneSpec::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
}

void CPaneSpec::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
}
