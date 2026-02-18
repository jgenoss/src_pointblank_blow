// WeaponContextDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyEditor.h"
#include "WeaponContextDialog.h"


// CWeaponContextDialog dialog

IMPLEMENT_DYNAMIC(CWeaponContextDialog, CDialog)

CWeaponContextDialog::CWeaponContextDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWeaponContextDialog::IDD, pParent)
{
	m_pBaseRegKey = NULL;
	m_pSelectedRegistry = NULL;
	m_pRoot = NULL;
	m_uiClassType = 0;
	m_uiNumber = 0;
}

CWeaponContextDialog::~CWeaponContextDialog()
{
}

void CWeaponContextDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox_BaseAttribute);
}


BEGIN_MESSAGE_MAP(CWeaponContextDialog, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CWeaponContextDialog::OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDOK, &CWeaponContextDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CWeaponContextDialog message handlers

BOOL CWeaponContextDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	Rec_TreeItem(TVI_ROOT, m_pRoot);
	AddBaseRegKey();

	SetDlgItemInt(IDC_EDIT1, m_uiClassType, FALSE);
	SetDlgItemInt(IDC_EDIT2, m_uiNumber, FALSE);
	SetDlgItemText(IDC_EDIT3, m_RegistryName);

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

HTREEITEM CWeaponContextDialog::AddTree(HTREEITEM hParent, i3RegKey * pKey)
{
	TVINSERTSTRUCT	data;
	HTREEITEM		hItem;

	data.hParent = hParent;
	data.hInsertAfter	= TVI_LAST;
	data.item.mask = TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;
	data.item.cChildren = 0;
	data.item.lParam = (LPARAM)pKey;
	data.item.pszText= (LPSTR)pKey->GetName();

	hItem	= m_TreeCtrl.InsertItem( &data);


	if( hParent != TVI_ROOT)
	{
		TVITEM item;
		item.hItem = hParent;
		item.mask	= TVIF_CHILDREN;

		if( m_TreeCtrl.GetItem( &item))
		{
			item.cChildren = 1;
			m_TreeCtrl.SetItem(&item);
		}
	}

	return hItem;
}

void	CWeaponContextDialog::Rec_TreeItem( HTREEITEM hParent, i3RegKey * pKey)
{
	HTREEITEM hItem = hParent;
	if( strcmp( pKey->GetName(), "WeaponList" ) != 0 )
		hItem = AddTree( hParent, pKey);

	for (INT32 i =0; i < pKey->getChildCount(); i++)
	{
		i3RegKey * pChild = (i3RegKey*)pKey->getChild( i);

		Rec_TreeItem( hItem, pChild);
	}
}

void	CWeaponContextDialog::AddBaseRegKey()
{
	m_ComboBox_BaseAttribute.ResetContent();

	std::deque<i3RegData*>* pList = m_pBaseRegKey->GetValueList();

	i3RegData *pData = NULL;

	size_t size = pList->size();	size_t SkipCount = 0;
	for(size_t i=0; i<size; ++i)
	{
		pData = (*pList).at( i );

		const char* Name = pData->GetName();

		if( Name[0] == '_' && Name[1] == '_' )
		{
			++SkipCount;
			continue;
		}

		m_ComboBox_BaseAttribute.AddString( Name );
		m_ComboBox_BaseAttribute.SetItemData( i - SkipCount, (DWORD_PTR)pData );
	}
}

void CWeaponContextDialog::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	m_pSelectedRegistry = (i3RegKey*)m_TreeCtrl.GetItemData( pNMTreeView->itemNew.hItem );

	*pResult = 0;
}

CString _MakeReverseChainString( i3RegKey* pRegKey, TCHAR Delimiter)
{
	CString Ret;
	Ret.Insert( 0, pRegKey->GetName() );
	Ret.Insert( 0, Delimiter );

	i3RegKey* pParent = (i3RegKey*)pRegKey->getParent();
	while( pParent )
	{
		Ret.Insert( 0, pParent->GetName() );
		Ret.Insert( 0, Delimiter );

		pParent = (i3RegKey*)pParent->getParent();
	}
	int Index = Ret.Find( CString("Weapon/") );
	if( Index == -1 )
		Ret = "Weapon";
	else
		Ret.Delete( 0, Index );
	return Ret;
}

void CWeaponContextDialog::OnBnClickedOk()
{
	if( !m_pSelectedRegistry )
	{
		AfxMessageBox("You did not select The Weapon Registry Tree!");
		return;
	}

	GetDlgItemText(IDC_EDIT3, m_RegistryName);
	if( m_RegistryName.IsEmpty() )
	{
		AfxMessageBox("You did not insert The New Registry Name!");
		return;
	}

	int Index = m_ComboBox_BaseAttribute.GetCurSel();

	CString BaseRegName;
	if( Index == -1 )
	{
		m_SelectedBaseDataIndex = (size_t)-1;
		BaseRegName = "Base Default";
	}
	else
	{
		i3RegData *pRegData = (i3RegData *)m_ComboBox_BaseAttribute.GetItemData( Index );

		if( pRegData->getDataType() == I3REG_TYPE_INT32 )
		{
			((i3RegINT32*)pRegData)->GetValue( &m_SelectedBaseDataIndex );
		}

		BaseRegName = pRegData->GetName();
	}

	BOOL ret;
	m_uiClassType = GetDlgItemInt(IDC_EDIT1, &ret, FALSE);
	m_uiNumber = GetDlgItemInt(IDC_EDIT2, &ret, FALSE);

	if( m_uiNumber == 0 )
	{
		AfxMessageBox("You can not use the New Weapon Registry Number of 0(Zero)!");
		return;
	}

	CString RegistryString = _MakeReverseChainString( m_pSelectedRegistry );
	RegistryString.Append("/");
	RegistryString.Append( m_RegistryName );

	CString Message;
	Message.Format( "ClassType : %d, Number : %d\n"
		"Base Attribute [ %s ]\n"
		"Registry [ %s ]\n"
		"Is New Weapon Contexts correct?"
		, m_uiClassType, m_uiNumber, 
		BaseRegName.GetString(),
		RegistryString);

	if( AfxMessageBox(Message, MB_YESNO | MB_ICONQUESTION ) == IDYES )
		OnOK();
}
