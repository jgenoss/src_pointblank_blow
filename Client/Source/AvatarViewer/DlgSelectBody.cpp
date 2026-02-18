#include "pch.h"
#include "DlgSelectBody.h"
#include "Panel_Dlg_Avatar.h"

#include "ConvertBlendIndex.h"

#include "i3Base/string/ext/contain_string.h"

IMPLEMENT_DYNAMIC(CDlg_SelectBody, CDialog)

CDlg_SelectBody::CDlg_SelectBody( CWnd * pParent /*= NULL*/)
	: CDialog(CDlg_SelectBody::IDD, pParent)
{
	
}

CDlg_SelectBody::~CDlg_SelectBody()
{
}

BOOL CDlg_SelectBody::OnInitDialog()
{
	BOOL bRv = CDialog::OnInitDialog();

	CreateBodyList();

	m_ExcludeList.InsertColumn( 0, "Name", LVCFMT_LEFT, 240);
	m_IncludeList.InsertColumn( 0, "Name", LVCFMT_LEFT, 240);

	return bRv;
}

void CDlg_SelectBody::OnOK( void)
{
	INT32 idx = m_CmbBodyList.GetCurSel();
	if( idx == -1)
	{
		::MessageBox( this->m_hWnd, "Sample Body를 설정하지 않았습니다. sample body를 설정하세요!!", "Warning!", MB_OK);
		return;
	}

	if( m_IncludePartsList.size() == 0)
	{
		::MessageBox( this->m_hWnd, "sampling을 할 Parts를 선택하지 않았습니다.", "Warning!", MB_OK);
		return;
	}

	// Convert를 실행
	ConvertBlendIndex converter;
	
	CString str;
	m_CmbBodyList.GetLBText( idx, str);
	const char * pszFrameName;
	if( str == "Male")	pszFrameName = "O_U_RedBulls_Ori";	// Male은 Redbull을 기준으로
	else				pszFrameName = "O_U_Tarantula_Ori";	// Female은 Tarantula를 기준으로

	//converter.LoadFrameSkel( pszFrameName, 0);

	mapParts::iterator it = m_IncludePartsList.begin();
	while( it != m_IncludePartsList.end())
	{
		converter.ConvertSG( pszFrameName, it->second->_szFullPath);
		it++;
	}

	::MessageBox( this->m_hWnd, "Convert가 완료되었습니다.", "Complate", MB_OK);
}

void CDlg_SelectBody::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_BODYLISt, m_CmbBodyList);
	DDX_Control(pDX, IDC_EXCLUDE_LIST, m_ExcludeList);
	DDX_Control(pDX, IDC_INCLUDE_LIST, m_IncludeList);
}

BEGIN_MESSAGE_MAP(CDlg_SelectBody, CDialog)
	ON_CBN_SELCHANGE(IDC_CMB_BODYLISt, OnCbSelectSampleBody)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_ADD, &CDlg_SelectBody::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_REMOVE, &CDlg_SelectBody::OnBnClickedBtnRemove)
END_MESSAGE_MAP()

void CDlg_SelectBody::OnDestroy()
{

	mapParts::iterator it = m_ExcludePartsList.begin();
	while( it != m_ExcludePartsList.end())
	{
		I3MEM_SAFE_FREE( it->second);
		it++;
	}
	m_ExcludePartsList.clear();

	it = m_IncludePartsList.begin();
	while( it != m_IncludePartsList.end())
	{
		I3MEM_SAFE_FREE( it->second);
		it++;
	}
	m_IncludePartsList.clear();

	CDialog::OnDestroy();
}

void CDlg_SelectBody::OnCbSelectSampleBody( void)
{
	INT32 idx = m_CmbBodyList.GetCurSel();

	if( idx != -1)
	{
		CString str;
		m_CmbBodyList.GetLBText( idx, str);

		mapParts::iterator it = m_ExcludePartsList.begin();
		while( it != m_ExcludePartsList.end())
		{
			I3MEM_SAFE_FREE( it->second);
			it++;
		}
		m_ExcludePartsList.clear();

		for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
		{
			const char * pszName = AVATAR::getPartName( (EQUIP::ePART) i).c_str();
			cAvatarManager::FindPartsFromFolder( pszName, m_ExcludePartsList);
		}

		UpdateList();

		if( str == "Male")
		{
			ExcludeBySexType( 1);
		}
		else if( str == "Female")
		{
			ExcludeBySexType( 2);
		}
	}
}

void CDlg_SelectBody::CreateBodyList( void)
{
	BOOL bDir = SetCurrentDirectory(i3ResourceFile::GetWorkingFolder());
	if( bDir == FALSE)
	{
		i3ResourceFile::SetWorkingFolder("");
		return;
	}

	m_CmbBodyList.Clear();
	m_CmbBodyList.AddString( "Male");
	m_CmbBodyList.AddString( "Female");
	m_CmbBodyList.SetCurSel( -1);

	mapParts list;
	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		const char * pszName = AVATAR::getPartName( (EQUIP::ePART) i).c_str();
		cAvatarManager::FindPartsFromFolder( pszName, list);
	}

	m_ExcludePartsList = list;

	UpdateList();
}

void CDlg_SelectBody::UpdateList()
{
	m_ExcludeList.DeleteAllItems();

	mapParts::iterator it = m_ExcludePartsList.begin();

	INT32 i = 0;
	while( it != m_ExcludePartsList.end())
	{
		INT32 idx = m_ExcludeList.InsertItem( i, it->first.c_str());
		m_ExcludeList.SetItemData( idx, (DWORD_PTR) it->second);
		it++;
	}

	m_IncludeList.DeleteAllItems();

	it = m_IncludePartsList.begin();
	i = 0;
	while( it != m_IncludePartsList.end())
	{
		INT32 idx = m_IncludeList.InsertItem( i, it->first.c_str());
		m_IncludeList.SetItemData( idx, (DWORD_PTR) it->second);
		it++;
	}
}

void CDlg_SelectBody::ExcludeBySexType( INT32 type)
{
	if( type > 2)
		return;

	mapParts::iterator it = m_ExcludePartsList.begin();

	while( it != m_ExcludePartsList.end())
	{
		bool bDelete = false;

		char szUpper[ 260];
		i3::generic_string_copy( szUpper, it->first.c_str());
		i3::to_upper( szUpper);

		if( type == 1)
		{
			if( i3::contain_string( szUpper, "A_") == 0)
				bDelete = true;

			// 앞자에 F가 있는 경우 검색
			if( i3::contain_string( szUpper, "F_") == 0)
				bDelete = true;

			// 특정 캐릭명 검색
			if( i3::contain_string( szUpper, "TARANTULA") > 0)
				bDelete = true;

			if( i3::contain_string( szUpper, "KEENEYES") > 0)
				bDelete = true;

			if( i3::contain_string( szUpper, "HIDE") > 0)
				bDelete = true;

			if( i3::contain_string( szUpper, "VIPERRED") > 0)
				bDelete = true;
		}
		else if( type == 2)
		{
			if( i3::contain_string( szUpper, "A_") == 0)
				bDelete = true;

			// 앞자에 M가 있는 경우 검색
			if( i3::contain_string( szUpper, "M_") == 0)
				bDelete = true;

			// 특정 캐릭명 검색
			if( i3::contain_string( szUpper, "REDBULLS") > 0)
				bDelete = true;

			if( i3::contain_string( szUpper, "ACIDPOL") > 0)
				bDelete = true;

			if( i3::contain_string( szUpper, "DFOX") > 0)
				bDelete = true;

			if( i3::contain_string( szUpper, "D-FOX") > 0)
				bDelete = true;

			if( i3::contain_string( szUpper, "LEOPARD") > 0)
				bDelete = true;
		}

		if( bDelete)
		{
			LVFINDINFO info;
			info.flags = LVFI_PARTIAL|LVFI_STRING;
			info.psz = _T( it->first.c_str());
			INT32 idx = m_ExcludeList.FindItem( &info);
			if( idx != -1)
				m_ExcludeList.DeleteItem(idx);

			I3MEM_SAFE_FREE( it->second);
			it = m_ExcludePartsList.erase( it);
		}
		else
			it++;
	}
}


void CDlg_SelectBody::OnBnClickedBtnAdd()
{
	// TODO: Add your control notification handler code here
	char szText[ 260];

	INT32 cnt = 0;
	for( INT32 i = 0; i < m_ExcludeList.GetItemCount(); i++)
	{
		UINT32 state = m_ExcludeList.GetItemState( i, LVIS_SELECTED);
		
		if( state == LVIS_SELECTED)
		{
			m_ExcludeList.GetItemText( i, 0, szText, 260);
			mapParts::iterator it = m_ExcludePartsList.find( szText);
			if( it != m_ExcludePartsList.end())
			{
				m_IncludePartsList.insert( mapParts::value_type( szText, it->second) );
				INT32 idx = m_IncludeList.InsertItem( cnt, it->first.c_str());
				cnt++;
				m_IncludeList.SetItemData( idx, (DWORD_PTR) it->second);

				m_ExcludePartsList.erase( it);
				m_ExcludeList.DeleteItem( i);
				i--;
			}
		}
	}
}

void CDlg_SelectBody::OnBnClickedBtnRemove()
{
	// TODO: Add your control notification handler code here
	char szText[ 260];

	INT32 cnt = 0;
	for( INT32 i = 0; i < m_IncludeList.GetItemCount(); i++)
	{
		UINT32 state = m_IncludeList.GetItemState( i, LVIS_SELECTED);
		
		if( state == LVIS_SELECTED)
		{
			m_IncludeList.GetItemText( i, 0, szText, 260);
			mapParts::iterator it = m_IncludePartsList.find( szText);
			if( it != m_IncludePartsList.end())
			{
				m_ExcludePartsList.insert( mapParts::value_type( szText, it->second) );
				INT32 idx = m_ExcludeList.InsertItem( cnt, it->first.c_str());
				cnt++;
				m_ExcludeList.SetItemData( idx, (DWORD_PTR) it->second);

				m_IncludePartsList.erase( it);
				m_IncludeList.DeleteItem( i);
				i--;
			}
		}
	}
}
