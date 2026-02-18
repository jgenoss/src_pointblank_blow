// DlgMapSectorEdit.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgMapSectorEdit.h"
#include "i3Base/string/ext/make_relative_path.h" 

static CDlgMapSectorEdit *		s_pDlgInst = NULL;

// CDlgMapSectorEdit dialog

IMPLEMENT_DYNAMIC(CDlgMapSectorEdit, CDialog)
CDlgMapSectorEdit::CDlgMapSectorEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMapSectorEdit::IDD, pParent)
{
	m_pTable = NULL;
	m_pWorld = NULL;
	m_pRes = NULL;

	s_pDlgInst = this;
}

CDlgMapSectorEdit::~CDlgMapSectorEdit()
{
	I3_SAFE_RELEASE( m_pTable);
	I3_SAFE_RELEASE( m_pWorld);
	I3_SAFE_RELEASE( m_pRes);

	s_pDlgInst = NULL;

	_RemoveAllSector();
	RemoveAllRes();
}

void CDlgMapSectorEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAPSECTOR, m_listMapSector);
	DDX_Control(pDX, IDC_EDIT_SECTORNAME, m_edSectorName);
	DDX_Control(pDX, IDC_EDIT_FLOORINDEX, m_edFloorIndex);
	DDX_Control(pDX, IDC_LIST_TRIGGER, m_LIST_Trigger);
}


BEGIN_MESSAGE_MAP(CDlgMapSectorEdit, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_MAPSECTOR, OnNMRclickListMapsector)
	ON_EN_CHANGE(IDC_EDIT_FLOORINDEX, OnEnChangeEditFloorindex)
	ON_EN_CHANGE(IDC_EDIT_SECTORNAME, OnEnChangeEditSectorname)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MAPSECTOR, OnNMClickListMapsector)
	ON_BN_CLICKED(IDC_BTN_EXPORTSECTOR, OnBnClickedBtnExportsector)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TRIGGER, &CDlgMapSectorEdit::OnNMClickListTrigger)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TRIGGER, &CDlgMapSectorEdit::OnNMDblclkListTrigger)
END_MESSAGE_MAP()


// CDlgMapSectorEdit message handlers

BOOL CDlgMapSectorEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_listMapSector.InsertColumn( 0, "Sector", LVCFMT_LEFT, 50);
		m_listMapSector.InsertColumn( 1, "Floor", LVCFMT_LEFT, 40);
		m_listMapSector.InsertColumn( 2, "Name", LVCFMT_LEFT, 180);
		m_listMapSector.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	{
		HICON hIcon;

		m_ImageList_Checkbox.Create( 16, 16, ILC_COLOR32 | ILC_MASK, 0, 2);

		hIcon = (HICON) ::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_ICO_BLANK), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		m_ImageList_Checkbox.Add( hIcon);

		hIcon = (HICON) ::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_ICO_SEL), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		m_ImageList_Checkbox.Add( hIcon);
	}

	{
		m_LIST_Trigger.SetExtendedStyle( LVS_EX_FULLROWSELECT);
		m_LIST_Trigger.InsertColumn( 0, "Name", LVCFMT_LEFT, 160);
		m_LIST_Trigger.InsertColumn( 1, "Info", LVCFMT_LEFT, 160);
		m_LIST_Trigger.InsertColumn( 2, "Events", LVCFMT_LEFT, 160);

		m_LIST_Trigger.SetImageList( &m_ImageList_Checkbox, LVSIL_SMALL);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CDlgMapSectorEdit::Execute( i3LevelWorld * pWorld, i3LevelResWorld * pRes, i3MapSectorTable * pTable)
{
	if( s_pDlgInst == NULL)
	{
		s_pDlgInst = new CDlgMapSectorEdit;

		s_pDlgInst->Create( CDlgMapSectorEdit::IDD, AfxGetMainWnd());
	}

	s_pDlgInst->Config( pWorld, pRes, pTable);

	s_pDlgInst->ShowWindow( SW_SHOW);

	return true;
}

void CDlgMapSectorEdit::Destroy(void)
{
	if( s_pDlgInst != NULL)
	{
		delete s_pDlgInst;
		s_pDlgInst = NULL;
	}
}

void CDlgMapSectorEdit::Config( i3LevelWorld * pWorld, i3LevelResWorld * pRes, i3MapSectorTable * pTable)
{
	I3_REF_CHANGE( m_pWorld, pWorld);
	I3_REF_CHANGE( m_pRes, pRes);
	I3_REF_CHANGE( m_pTable, pTable);

	m_bEditted = FALSE;

	ScanEventRes();

	_AssignValue();
}

void CDlgMapSectorEdit::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_listMapSector.m_hWnd))
	{
		m_listMapSector.SetColumnWidth( 0, 60);
		m_listMapSector.SetColumnWidth( 0, 60);
		m_listMapSector.SetColumnWidth( 0, cx - 120);
	}
}

INT32 CDlgMapSectorEdit::_getSelectedSector(void)
{
	POSITION pos = m_listMapSector.GetFirstSelectedItemPosition();

	if( pos != NULL)
	{
		INT32 idx = m_listMapSector.GetNextSelectedItem( pos);

		return idx;
	}

	return -1;
}

void CDlgMapSectorEdit::_OnNewSector(void)
{
	i3::pack::MAP_SECTOR_INFO * pInfo;
	INT32 cnt = m_listMapSector.GetItemCount();

	pInfo = new i3::pack::MAP_SECTOR_INFO;

	i3mem::FillZero( pInfo, sizeof(i3::pack::MAP_SECTOR_INFO));

	sprintf( pInfo->m_szName, "Sector%d", cnt);

	char conv[128];

	sprintf( conv, "%d", cnt);

	INT32 idx = m_listMapSector.InsertItem( cnt, conv, 0);

	_UpdateSector( idx, pInfo);

	_SelectSector( idx);

	_UpdateAllSector();

	m_bEditted = TRUE;
}

void CDlgMapSectorEdit::_OnDeleteSector()
{
	INT32 idx = _getSelectedSector();
	i3::pack::MAP_SECTOR_INFO * pInfo;

	if( idx == -1)
		return;

	pInfo = (i3::pack::MAP_SECTOR_INFO *) m_listMapSector.GetItemData( idx);

	delete pInfo;

	m_listMapSector.DeleteItem( idx);

	if( m_idxSelect == idx)
	{
		_SelectSector( -1);
	}

	_UpdateAllSector();

	m_bEditted = TRUE;
}

void CDlgMapSectorEdit::_SelectSector( INT32 idx)
{
	if( m_idxSelect != -1)
	{
		m_listMapSector.SetItemState( m_idxSelect, LVIS_SELECTED, 0);
	}

	m_idxSelect = idx;

	if( m_idxSelect != -1)
	{
		m_listMapSector.SetItemState( m_idxSelect, LVIS_SELECTED, 0xFFFFFFFF);
		m_listMapSector.EnsureVisible( m_idxSelect, FALSE);

		{
			char conv[512];
			i3::pack::MAP_SECTOR_INFO * pInfo = (i3::pack::MAP_SECTOR_INFO *) m_listMapSector.GetItemData( idx);

			sprintf( conv, "%d", pInfo->m_Floor);
			m_edFloorIndex.SetWindowText( conv);
			m_edSectorName.SetWindowText( pInfo->m_szName);
		}
	}

	RefreshTriggerImage();
}

void CDlgMapSectorEdit::_UpdateAllSector(void)
{
	INT32 count = m_listMapSector.GetItemCount();
	for( INT32 i = 0;i < count; ++i)
	{
		_UpdateSector( i);
	}
}

void CDlgMapSectorEdit::_UpdateSector( INT32 idx, i3::pack::MAP_SECTOR_INFO * pInfo)
{
	m_listMapSector.SetItemData( idx, (DWORD_PTR) pInfo);

	_UpdateSector( idx);
}

void CDlgMapSectorEdit::_UpdateSector( INT32 idx)
{
	i3::pack::MAP_SECTOR_INFO * pInfo;

	pInfo = (i3::pack::MAP_SECTOR_INFO *) m_listMapSector.GetItemData( idx);

	char conv[512];

	// Map Sector Index
	sprintf( conv, "%d", idx);
	m_listMapSector.SetItemText( idx, 0, conv);

	// Floor
	sprintf( conv, "%d", pInfo->m_Floor);
	m_listMapSector.SetItemText( idx, 1, conv);

	// Name
	m_listMapSector.SetItemText( idx, 2, pInfo->m_szName);
}

void CDlgMapSectorEdit::GetAllTrigger( i3::vector< i3LevelPrimitive *> & elmList)
{
	if( g_pScene == NULL)
		return;

	for( INT32 i = 0; i < g_pScene->GetElementCount(); i++)
	{
		i3LevelElement3D * pElm = g_pScene->GetElement( i);

		if( pElm->kind_of( i3LevelPrimitive::static_meta()) && !pElm->kind_of( i3LevelRespawn::static_meta()))
		{
			i3LevelPrimitive * pPrim = (i3LevelPrimitive *) pElm;

			//if( IsActionMapSector( -1, pPrim))
			{
				// -1 인자는 어떤 종류이든지 i3ActionSetMapSector를 가지고 있는 경우에 해당.
				elmList.push_back( pPrim);
			}
		}
	}
}

void CDlgMapSectorEdit::FindAllRelatedElement( INT32 idxMapSector, i3::vector< i3LevelPrimitive *> & elmList)
{
	if( g_pScene == NULL)
		return;

	for( INT32 i = 0; i < g_pScene->GetElementCount(); i++)
	{
		i3LevelElement3D * pElm = g_pScene->GetElement( i);

		if( pElm->kind_of( i3LevelPrimitive::static_meta()))
		{
			i3LevelPrimitive * pPrim = (i3LevelPrimitive *) pElm;

			// 해당 TimeEventGen이 MapSector Action을 가지고 있는지 확인.
			if( IsActionMapSector( idxMapSector, pPrim))
			{
				elmList.push_back( pPrim);
			}
		}
	}
}

bool CDlgMapSectorEdit::IsSameMapSector( INT32 idxMapSector, i3TimeEventGen * pEventGen)
{
	for( UINT32 i = 0; i < pEventGen->getEventCount(); i++)
	{
		i3TimeEventInfo * pInfo = pEventGen->getEvent( i);

		i3Action * pAction = pInfo->getAction();

		if( (pAction != NULL) && (pAction->kind_of( i3ActionSetMapSector::static_meta())))
		{
			i3ActionSetMapSector * pTemp = (i3ActionSetMapSector *) pAction;

			if( idxMapSector == -1)
				return true;

			if( pTemp->getMapSector() == idxMapSector)
				return true;
		}
	}

	return false;
}

bool CDlgMapSectorEdit::IsActionMapSector( INT32 idxMapSector, i3LevelPrimitive * pPrim)
{
	if( pPrim->isTrigger() == false)
		return false;

	i3LevelResEvent * pEvent = pPrim->getEventRes();
	if( pEvent == NULL)
		return false;

	i3TimeEventGen * pTimeEvent = pEvent->getTimeEvent();
	if( pTimeEvent == NULL)
		return false;

	return IsSameMapSector( idxMapSector, pTimeEvent);
}

void CDlgMapSectorEdit::_RemoveAllSector(void)
{
	INT32 i;
	i3::pack::MAP_SECTOR_INFO * pInfo;

	for( i = 0; i < m_listMapSector.GetItemCount(); i++)
	{
		pInfo = (i3::pack::MAP_SECTOR_INFO *) m_listMapSector.GetItemData( i);

		delete pInfo;
	}

	m_listMapSector.DeleteAllItems();
}

void CDlgMapSectorEdit::UpdateAllTrigger(void)
{
	m_LIST_Trigger.DeleteAllItems();

	if( g_pScene == NULL)
		return;

	i3::vector< i3LevelPrimitive *> elmList;

	GetAllTrigger( elmList);

	for( UINT32 i = 0; i < elmList.size(); i++)
	{
		i3LevelPrimitive * pTrigger = elmList[i];		

		INT32 idx = m_LIST_Trigger.InsertItem( i, pTrigger->GetName(), 0);

		UpdateTrigger( idx, pTrigger);
	}
}

void CDlgMapSectorEdit::UpdateTrigger( INT32 idx, i3LevelPrimitive * pTrigger)
{
	m_LIST_Trigger.SetItemData( idx, (DWORD_PTR) pTrigger);

	// Image
	{
		INT32 idxImage = IsActionMapSector( m_idxSelect, pTrigger) ? 1 : 0;
		LVITEM item;

		memset( &item, 0, sizeof(item));

		item.mask = LVIF_IMAGE;
		item.iItem = idx;
		item.iImage = idxImage;

		m_LIST_Trigger.SetItem( &item);
	}

	// Info
	{
		char szEvent[512], szCmt[256], szRes[MAX_PATH];

		szEvent[0] = 0;
		szRes[0] = 0;

		i3LevelResEvent * pRes = pTrigger->getEventRes();
		if( pRes != NULL)
		{
			i3TimeEventGen * pEventGen = pRes->getTimeEvent();

			if( pEventGen != NULL)
			{
				for( UINT32 i = 0; i < pEventGen->getEventCount(); i++)
				{
					i3TimeEventInfo * pEvent = pEventGen->getEvent( i);

					i3Action * pAction = pEvent->getAction();

					if( pAction != NULL)
					{
						pAction->GetInfoString( szCmt, true, _countof(szCmt)-1);

						if( szEvent[0] != 0)
						{
							strcat_s( szEvent, _countof( szEvent)-1, ", ");
						}

						strcat_s( szEvent, _countof(szEvent)-1, szCmt);

						if( pAction->kind_of( i3ActionSetMapSector::static_meta()))
						{
							i3::string_ncopy_nullpad( szRes, pRes->GetName(), _countof( szRes) -1);
						}
					}
				}
			}			
		}

		m_LIST_Trigger.SetItemText( idx, 1, szEvent);

		m_LIST_Trigger.SetItemText( idx, 2, szRes);
	}
}

void CDlgMapSectorEdit::RefreshTriggerImage(void)
{
	INT32 i, cnt;

	cnt = m_LIST_Trigger.GetItemCount();

	for( i = 0; i < cnt; i++)
	{
		i3LevelPrimitive * pPrim = (i3LevelPrimitive *) m_LIST_Trigger.GetItemData( i);

		UpdateTrigger( i, pPrim);
	}
}

void CDlgMapSectorEdit::_AssignValue(void)
{
	INT32 i, idx;
	i3::pack::MAP_SECTOR_INFO * pSrc, * pDest;
	char conv[128];

	_RemoveAllSector();

	// Map Sectors
	for( i = 0; i < m_pTable->getCount(); i++)
	{
		pSrc = m_pTable->getMapSector( i);

		pDest = new i3::pack::MAP_SECTOR_INFO;

		memcpy( pDest, pSrc, sizeof(i3::pack::MAP_SECTOR_INFO));

		sprintf( conv, "%d", i);

		idx = m_listMapSector.InsertItem( i, conv, 0);

		_UpdateSector( idx, pDest);
	}

	// Triggers
	UpdateAllTrigger();
}

void CDlgMapSectorEdit::OnNMRclickListMapsector(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CMenu	Menu, *  pPopup;
	UINT32 cmd;
	CPoint pt;

	GetCursorPos( &pt);

	Menu.LoadMenu( IDR_POPUP_WORLDSECTION);

	pPopup = Menu.GetSubMenu( 0);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
			pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_WORLDSECTION_NEW :
			_OnNewSector();
			break;

		case ID_WORLDSECTION_DELETE :
			_OnDeleteSector();
			break;

		default:
			break;
	}

	*pResult = 1;
}

void CDlgMapSectorEdit::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	// World-Section Table의 재구성
	if( m_bEditted == TRUE && m_pWorld != NULL)
	{
		i3World * pWorld = m_pWorld->getWorld();
		I3ASSERT( pWorld != NULL);
		if( pWorld->getMapSectorTable() != NULL)
		{
			I3ASSERT( pWorld->getMapSectorTable() == m_pTable);
		}

		i3LevelUndo::ADD_Edit( m_pWorld);
	}

	if( m_pTable != NULL)
	{
		INT32 i, cnt;

		cnt = m_listMapSector.GetItemCount();

		m_pTable->SetSectorCount( cnt);

		for( i = 0; i < cnt; i++)
		{
			i3::pack::MAP_SECTOR_INFO * pInfo = (i3::pack::MAP_SECTOR_INFO *) m_listMapSector.GetItemData( i);

			i3mem::Copy( m_pTable->getMapSector( i), pInfo, sizeof(i3::pack::MAP_SECTOR_INFO));
		}
	}

	m_pRes->addResState( I3LV_RES_STATE_MODIFIED);

	m_bEditted	= FALSE;
	I3_SAFE_RELEASE( m_pWorld);
	I3_SAFE_RELEASE( m_pTable);
	I3_SAFE_RELEASE( m_pRes);

	CDialog::OnOK();
}

void CDlgMapSectorEdit::OnCancel()
{
	m_bEditted	= FALSE;
	I3_SAFE_RELEASE( m_pWorld);
	I3_SAFE_RELEASE( m_pTable);
	I3_SAFE_RELEASE( m_pRes);

	CDialog::OnCancel();
}

void CDlgMapSectorEdit::OnEnChangeEditFloorindex()
{
	char conv[256];
	i3::pack::MAP_SECTOR_INFO * pInfo;
	INT32 selIdx =	_getSelectedSector();

	if( selIdx == -1)
		return;

	m_edFloorIndex.GetWindowText( conv, sizeof(conv) - 1);
	if( i3::generic_string_size( conv) > 0)
	{
		pInfo = (i3::pack::MAP_SECTOR_INFO *) m_listMapSector.GetItemData( selIdx);
		
		pInfo->m_Floor = atoi( conv);

		m_bEditted = TRUE;

		_UpdateSector( selIdx);
	}
}

void CDlgMapSectorEdit::OnEnChangeEditSectorname()
{
	char conv[256];
	i3::pack::MAP_SECTOR_INFO * pInfo;
	INT32 selIdx =	_getSelectedSector();

	if( selIdx == -1)
		return;

	m_edSectorName.GetWindowText( conv, sizeof(conv) - 1);
	if( i3::generic_string_size( conv) > 0)
	{
		pInfo = (i3::pack::MAP_SECTOR_INFO *) m_listMapSector.GetItemData( selIdx);
		
		i3::string_ncopy_nullpad( pInfo->m_szName, conv, 127);

		m_bEditted = TRUE;

		_UpdateSector( selIdx);
	}
}

void CDlgMapSectorEdit::OnNMClickListMapsector(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT32 idx = _getSelectedSector();

	if( idx != m_idxSelect)
	{
		_SelectSector( idx);
	}

	*pResult = 0;
}

void CDlgMapSectorEdit::OnBnClickedBtnExportsector()
{
	// TODO: Add your control notification handler code here
	CFileDialog	Dlg( FALSE, "txt", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"MapSector string table(*.txt)|*.TXT||", this);

	if( Dlg.DoModal() != IDOK )
	{
		//Message 
		return; 
	}

	if( m_pTable != NULL)
	{
		m_pTable->SaveSectorString( LPCTSTR( Dlg.GetPathName()));
	}
}

void CDlgMapSectorEdit::OnNMClickListTrigger(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pInfo = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	{
		UINT flag;

		INT32 idx = m_LIST_Trigger.HitTest( pInfo->ptAction, &flag);

		//I3TRACE( "FLAG : %08X\n", flag);
		if( idx != -1)
		{
			if( flag == LVHT_ONITEMICON)		// Bit Flag 계산으로는 정확하게 Icon만 선택되지 않고, SubItem 영역도 포함된다.
			{
				if( m_idxSelect == -1)
				{
					AfxMessageBox( "먼저 Map Sector를 선택해야 합니다.");
					return;
				}

				ToggleTrigger( idx);
			}
		}
	}
	
	*pResult = 0;
}

void CDlgMapSectorEdit::OnNMDblclkListTrigger(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pInfo = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	INT32 idx = pInfo->iItem;

	if( idx != -1)
	{
		i3LevelFramework * pFramework = (i3LevelFramework *) i3Framework::getCurrentFramework();
		i3LevelPrimitive * pPrim = (i3LevelPrimitive *) m_LIST_Trigger.GetItemData( idx);

		pFramework->Fit( pPrim);
	}
	
	*pResult = 0;
}

void CDlgMapSectorEdit::ToggleTrigger( INT32 idx)
{
	bool bSelect = false;

	if( m_idxSelect == -1)
		return;

	i3LevelPrimitive * pPrim = (i3LevelPrimitive *) m_LIST_Trigger.GetItemData( idx);
	I3ASSERT( pPrim != NULL);

	// 선택인지 먼저 확인.
	{
		LVITEM item;

		item.mask = LVIF_IMAGE;
		item.iItem = idx;
		item.iSubItem = 0;

		m_LIST_Trigger.GetItem( &item);

		if( item.iImage == 1)
			bSelect = true;
	}

	if( bSelect == false)
	{
		// 해당 Map Sector로 설정.
		SetTrigger_MapSector( pPrim, m_idxSelect);
	}
	else
	{
		// 해제
		ClearTrigger_MapSector( pPrim);
	}

	UpdateTrigger( idx, pPrim);
}

i3LevelResEvent * CDlgMapSectorEdit::PrepareEventRes(void)
{
	char szPath[ MAX_PATH], szName[MAX_PATH];
	i3LevelResEvent * pRes;

	if( m_idxSelect == -1)
	{
		AfxMessageBox( "먼저 Map Sector를 선택해야 합니다.");
		return NULL;
	}

	sprintf_s( szName, _countof( szName)-1, "MapSector_%d", m_idxSelect);
	sprintf( szPath, "Scene/%s/_Generated/%s.i3Evt", g_pScene->GetName(), szName);

	pRes = (i3LevelResEvent *) g_pResDB->FindResByPath( szPath);

	if( pRes == NULL)
	{
		pRes = i3LevelResEvent::new_object();
		pRes->setPath( szPath);
		pRes->SetName( szName);

		pRes->addResState( I3LV_RES_STATE_MODIFIED | I3LV_RES_STATE_LOADED | I3LV_RES_STATE_BRIEF_LOADED);

		g_pResDB->AddRes( pRes);
	}

	return pRes;
}

void CDlgMapSectorEdit::SetTrigger_MapSector( i3LevelPrimitive * pPrim, INT32 idxMapSector)
{
	i3LevelResEvent * pRes;

	if( m_idxSelect == -1)
	{
		AfxMessageBox( "먼저 Map Sector가 선택되어야 합니다.");
		return;
	}

	if( pPrim->isTrigger() == false)
		pPrim->setTrigger( true);

	// 먼저 해당 Resource가 있는지 검색.
	pRes = FindEventRes( m_idxSelect);
	
	if( pRes == NULL)
	{
		// 그래도 없다면 생성.
		pRes = PrepareEventRes();

		m_ResList.push_back( pRes);
	}

	pPrim->setEventRes( pRes);

	I3ASSERT( pRes != NULL);

	i3TimeEventGen * pGen = pRes->getTimeEvent();
	if( pGen == NULL)
	{
		pGen = i3TimeEventGen::new_object_ref();

		pRes->setTimeEvent( pGen);
	}

	// 먼저 i3ActionSetMapSector Action이 존재하는지 확인.
	i3ActionSetMapSector * pAction = NULL;

	for( UINT32 i = 0; i < pGen->getEventCount(); i++)
	{
		i3TimeEventInfo * pEvent = pGen->getEvent( i);

		i3Action * pTemp = pEvent->getAction();

		if( pTemp == NULL)
			continue;

		if( pTemp->kind_of( i3ActionSetMapSector::static_meta()) == false)
			continue;

		pAction = (i3ActionSetMapSector *) pTemp;
	}

	if( pAction == NULL)
	{
		// Action이 없다면 추가해야 한다.
		i3TimeEventInfo * pInfo = i3TimeEventInfo::new_object_ref();

		pAction = i3ActionSetMapSector::new_object_ref();

		pInfo->SetName( "Map Sector");
		pInfo->setAction( pAction);
		pInfo->setStartTime( 0.0f);

		pGen->AddEvent( pInfo);
	}

	I3ASSERT( pAction != NULL);

	pAction->setMapSector( m_idxSelect);
}

void CDlgMapSectorEdit::ClearTrigger_MapSector( i3LevelPrimitive * pPrim)
{
	pPrim->setEventRes( NULL);
}

void CDlgMapSectorEdit::ScanEventRes(void)
{
	RemoveAllRes();

	i3Level::LoadAllEventRes( g_pScene->GetName(), m_ResList);
}

void CDlgMapSectorEdit::RemoveAllRes(void)
{
	m_ResList.clear();
}

i3LevelResEvent * CDlgMapSectorEdit::FindEventRes( INT32 idxMap)
{
	for( UINT32 i = 0; i < m_ResList.size(); i++)
	{
		i3LevelResEvent * pRes = m_ResList[i];

		i3TimeEventGen * pGen = pRes->getTimeEvent();
		if( pGen == NULL)
			continue;

		if( IsSameMapSector( m_idxSelect, pGen))
			return pRes;
	}

	return NULL;
}