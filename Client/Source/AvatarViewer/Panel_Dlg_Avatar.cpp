// Panel_Dlg_Avatar.cpp : implementation file
//

#include "pch.h"
#include "AvatarViewer.h"
#include "Panel_Dlg_Avatar.h"
#include "Avatar.h"
#include "GlobalVar.h"
#include "MainFrm.h"

#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/substr.h"

// CPanel_Dlg_Avatar dialog

IMPLEMENT_DYNAMIC(CPanel_Dlg_Avatar, CDialog)

CPanel_Dlg_Avatar::CPanel_Dlg_Avatar(CWnd* pParent /*=NULL*/)
	: CDialog(CPanel_Dlg_Avatar::IDD, pParent)
{

}

CPanel_Dlg_Avatar::~CPanel_Dlg_Avatar()
{
	//ReleasePartsList();
	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		mapParts::iterator it = m_PartPath[i].begin();

		while( it != m_PartPath[i].end())
		{
			I3MEM_SAFE_FREE( it->second);
			it++;
		}

		m_PartPath[i].clear();
	}

	// m_TreeCtrl.DeleteAllItems()가 소멸자에서 호출되면 근처에서 그냥 조용이 종료되고, 릭만 왕창생김...따라서..
	// 여기서 이 함수를 호출하면 안될것 같다....(2012.09.12.수빈)
}

void CPanel_Dlg_Avatar::ReleasePartsList( void)
{
	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		mapParts::iterator it = m_PartPath[i].begin();

		while( it != m_PartPath[i].end())
		{
			I3MEM_SAFE_FREE( it->second);
			it++;
		}

		m_PartPath[i].clear();
	}
	m_TreeCtrl.DeleteAllItems();
}

void CPanel_Dlg_Avatar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_TreeCtrl);
}

bool CPanel_Dlg_Avatar::UpdatePart( Avatar * pAvatar, EQUIP::ePART part, INT32 lod, vPartsList &partlist)
{
	CMainFrame * pMainFrm = (CMainFrame*)(::AfxGetMainWnd());
	I3ASSERT( pMainFrm != NULL);
	TEAM_TYPE team = pMainFrm->GetTeamType();
	CEquipData * pEquipData = pMainFrm->m_Panel_Part.GetEquipData();

	i3::rc_string &str = pAvatar->getPartName( part);

	// check team
	const char * pszTeamPart = pEquipData->GetTeamPart( str.c_str(), team);
	if( pszTeamPart != NULL)
	{
		const char * pszFullPath = FindFullPathByName( part, pszTeamPart);

		if( pszFullPath != NULL)
		{
			partlist.push_back( pszTeamPart);
			return true;
		}
	}

	return false;
}

void CPanel_Dlg_Avatar::UpdateAll( Avatar * pAvatar)
{
	INT32 i;
	vPartsList list;

	for( i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		if( UpdatePart( pAvatar, (EQUIP::ePART) i, 0, list) == false)
		{
			i3::string str = g_pAvatar->getPartName( (EQUIP::ePART) i);
			list.push_back( str);
		}
	}

	g_pFramework->ReloadParts( list, g_pFramework->GetTeam());
}

void CPanel_Dlg_Avatar::Log( EQUIP::ePART part, INT32 lod, void * pData, const char * pszMsg)
{
	/*HTREEITEM hLOD;
	CPanel_Dlg_Avatar * pDlg = (CPanel_Dlg_Avatar *) pData;*/

	//hLOD = pDlg->m_hLOD[ part][lod];

	//HTREEITEM hParent = pDlg->m_TreeCtrl.GetChildItem( hLOD);

	//HTREEITEM hItem = pDlg->m_TreeCtrl.InsertItem( pszMsg, hParent);

	//pDlg->m_TreeCtrl.Expand( hParent, TVE_EXPAND);
}


BEGIN_MESSAGE_MAP(CPanel_Dlg_Avatar, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY( NM_DBLCLK, IDC_TREE, OnNMDBClickPartTree)
END_MESSAGE_MAP()


// CPanel_Dlg_Avatar message handlers

void CPanel_Dlg_Avatar::_InsertPartsList( HTREEITEM hRoot, const char * filter)
{
	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		const char * pszName = AVATAR::getPartName( (EQUIP::ePART) i).c_str();
		m_hRoots[i] = m_TreeCtrl.InsertItem( pszName, hRoot);

		mapParts::iterator it = m_PartPath[i].begin();

		CMainFrame* pMainFrm = (CMainFrame*)(AfxGetMainWnd());
		CEquipData * pEquipData = pMainFrm->m_Panel_Part.GetEquipData();

		INT32 j = 0;
		while( it != m_PartPath[ i].end())
		{
			I3ASSERT( it->second != NULL);
			if( (filter == NULL ||
				it->first.c_str()[0] == (char)*filter) && (it->second->_flag == false) )
			{
				// 이미 등록되어 있는 페어를 찾아본다.
				CEquipData::tagPartInfo * pPartInfo = pEquipData->ExistPart( it->first.c_str());

				if( pPartInfo == NULL)
				{// pef에 없는 res
					it->second->_hItem = m_TreeCtrl.InsertItem( it->first.c_str(), m_hRoots[i]);
					it->second->_flag = true;
				}
				else
				{
					if( pPartInfo->_hResItem == NULL)
					{
						pPartInfo->_hResItem = it->second->_hItem = m_TreeCtrl.InsertItem( it->first.c_str(), m_hRoots[i]);
						it->second->_flag = true;
					}
				}

				j++;
			}

			it++;
		}

		if( j == 0)
		{
			m_TreeCtrl.DeleteItem( m_hRoots[i]);
		}
	}
}

void CPanel_Dlg_Avatar::CreatePartsList( void)
{
	::BOOL bDir = SetCurrentDirectory(i3ResourceFile::GetWorkingFolder());
	if (FALSE == bDir)
	{
		i3ResourceFile::SetWorkingFolder("");
		return;
	}

	ReleasePartsList();

	if( i3::generic_string_size( i3ResourceFile::GetWorkingFolder()) > 0)
	{
		HTREEITEM hRoot_Female = m_TreeCtrl.InsertItem( "Female");
		HTREEITEM hRoot_Male = m_TreeCtrl.InsertItem( "Male");
		HTREEITEM hRoot_ETC = m_TreeCtrl.InsertItem( "ETC");

		for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
		{
			const char * pszName = AVATAR::getPartName( (EQUIP::ePART) i).c_str();
			
			// 각 장비 폴더를 뒤져보자
			cAvatarManager::FindPartsFromFolder( pszName, m_PartPath[ i]);
		}

		_InsertPartsList( hRoot_Female, "F");
		_InsertPartsList( hRoot_Male, "M");
		_InsertPartsList( hRoot_ETC, NULL);

		//m_TreeCtrl.Expand( m_hRoots[i], TVE_EXPAND);
	}
}

BOOL CPanel_Dlg_Avatar::OnInitDialog()
{
	CDialog::OnInitDialog();

	CreatePartsList();

	if( g_pAvatar != NULL)
	{
		UpdateAll( g_pAvatar);
	}

	i3TDK::RegisterUpdate( m_hWnd, Avatar::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanel_Dlg_Avatar::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

const char * CPanel_Dlg_Avatar::FindFullPathByName( EQUIP::ePART part, const char * pszName)
{
	mapParts::iterator it = m_PartPath[ part].find( pszName);

	if( it != m_PartPath[ part].end())
		return (const char*)it->second->_szFullPath;

	return NULL;
}

void CPanel_Dlg_Avatar::OnNMDBClickPartTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	//char szFullPath[ MAX_PATH];
	HTREEITEM hSel = NULL;

	POINT pt;
	GetCursorPos( &pt);

	// Select
	{
		POINT cpt;
		UINT flag;

		cpt = pt;

		m_TreeCtrl.ScreenToClient( &cpt);

		hSel = m_TreeCtrl.HitTest( cpt, &flag);

		if ((hSel != NULL) && (TVHT_ONITEM & flag))
		{	
			m_TreeCtrl.Select( hSel, TVGN_CARET);

			HTREEITEM hParent = m_TreeCtrl.GetParentItem( hSel);
			if( hParent != NULL)
			{
				CString partsName = m_TreeCtrl.GetItemText( hParent);

				CMainFrame * pMainFrm = (CMainFrame*)(::AfxGetMainWnd());
				I3ASSERT( pMainFrm != NULL);
				TEAM_TYPE team = pMainFrm->GetTeamType();
				CEquipData * pEquipData = pMainFrm->m_Panel_Part.GetEquipData();

				INT32 i;
				for( i = 0; i < EQUIP::ePART_COUNT; i++)
				{
					if( AVATAR::getPartName( (EQUIP::ePART)i) == partsName.GetString())
					{
						CString str = m_TreeCtrl.GetItemText( hSel);
						const char * pszName = (const char *) str.GetString();
						if( pszName != NULL)
						{
							// check team
							const char * pszTeamPart = pEquipData->GetTeamPart( pszName, team);
							if( pszTeamPart != NULL)
							{
								const char * pszFullPath = FindFullPathByName( (EQUIP::ePART) i, pszTeamPart);

								if( pszFullPath != NULL)
									pMainFrm->ChangePart( (EQUIP::ePART)i, pszTeamPart, pszFullPath );
							}
							else
							{
								const char * pszFullPath = FindFullPathByName( (EQUIP::ePART) i, pszName);

								if( pszFullPath != NULL)
									pMainFrm->ChangePart( (EQUIP::ePART)i, pszName, pszFullPath );
							}
						}
					}
				}
			}
		}
	}
}

void CPanel_Dlg_Avatar::OnNMRClickPartTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu Menu, * pPopup;
	POINT pt;
	UINT32 cmd;
	HTREEITEM hSel = NULL;
	GetCursorPos( &pt);

	// Select
	{
		POINT cpt;
		UINT flag;

		cpt = pt;

		m_TreeCtrl.ScreenToClient( &cpt);

		hSel = m_TreeCtrl.HitTest( cpt, &flag);

		if ((hSel != NULL) && (TVHT_ONITEM & flag))
		{
			m_TreeCtrl.Select( hSel, TVGN_CARET);
		}
	}

	{
		HINSTANCE hOld = AfxGetResourceHandle();

		AfxSetResourceHandle( g_hInstTDK);

		Menu.LoadMenu( IDR_POPUP_EDIT);

		AfxSetResourceHandle( hOld);
	}

	pPopup = Menu.GetSubMenu( 0);

	pPopup->EnableMenuItem( ID_EDIT_CUT, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	pPopup->EnableMenuItem( ID_EDIT_COPY, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	pPopup->EnableMenuItem( ID_EDIT_PASTE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);

	if( hSel == NULL)
	{	
		pPopup->EnableMenuItem( ID_EDIT_SETPOSITIONSCALE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	}

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);


	if( cmd == ID_EDIT_SETPOSITIONSCALE)
	{


	}
	
	*pResult = 1;
}

void	CPanel_Dlg_Avatar::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( i3::same_of<Avatar*>(pInfo->m_pMeta)) //->IsExactTypeOf( Avatar::GetClassMeta()))
	{
		if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
		{
			I3ASSERT( pInfo->m_pObject != NULL);

			UpdateAll( (Avatar *) pInfo->m_pObject);
		}
	}
}

LRESULT CPanel_Dlg_Avatar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

