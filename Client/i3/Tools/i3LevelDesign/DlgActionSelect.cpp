// DlgActionSelect.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgActionSelect.h"
#include "DlgEnterEventName.h"
#include ".\dlgactionselect.h"
#include "GlobalVariable.h"
#include "MainFrm.h"

#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/contain_string.h"

// CDlgActionSelect dialog

IMPLEMENT_DYNAMIC(CDlgActionSelect, CDialog)
CDlgActionSelect::CDlgActionSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgActionSelect::IDD, pParent)
{
	m_pSelEvent = NULL;
}

CDlgActionSelect::~CDlgActionSelect()
{
	RemoveAllEvents();
}

void CDlgActionSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ACTION_SEL, m_EventListCtrl);
	DDX_Control(pDX, ID_BTN_NEWEVENT, m_ctrlNewEvent);
	DDX_Control(pDX, IDC_STT_SCENE_PATH, m_stActionPath);
}


BEGIN_MESSAGE_MAP(CDlgActionSelect, CDialog)
	ON_BN_CLICKED(ID_BTN_NEWEVENT, OnBnClickedBtnNewevent)
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_BTN_DELETE_EVENT, OnBnClickedBtnDeleteEvent)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_ACTION_SEL, OnLvnEndlabeleditListActionSel)
END_MESSAGE_MAP()


// CDlgActionSelect message handlers
bool CDlgActionSelect::Execute( i3LevelResEvent * pAction)
{
	m_pSelEvent = pAction;

	if( DoModal() == IDCANCEL)
		return false;

	return true;
}

void CDlgActionSelect::RemoveAllEvents(void)
{
	for( UINT32 i = 0; i < m_EventList.size(); i++)
	{
		i3LevelResEvent * pRes = m_EventList[i];

		I3_MUST_RELEASE( pRes);
	}

	m_EventList.clear();

	if( ::IsWindow( m_EventListCtrl.m_hWnd))
		m_EventListCtrl.DeleteAllItems();
}

BOOL CDlgActionSelect::EventProc( INT32 Level, char * pszPath, WIN32_FIND_DATA * pFileInfo, void * pUserData)
{
	char szFull[ MAX_PATH], szName[ MAX_PATH];
	i3::vector< i3LevelResEvent *> * pList = (i3::vector< i3LevelResEvent *> *) pUserData;

	i3LevelResEvent * pRes = i3LevelResEvent::new_object();

	// Loading
	{
		sprintf_s( szFull, _countof( szFull)- 1, "%s/%s", pszPath, pFileInfo->cFileName);

		pRes->Load( szFull);
	}

	// Registration
	{
		i3::extract_filetitle( pFileInfo->cFileName, szName);

		pRes->SetName( szName);
	}

	pList->push_back( pRes);

	return TRUE;
}

void CDlgActionSelect::UpdateAllEvents(void)
{
	char szPath[ MAX_PATH];

	sprintf( szPath, "%s/Scene/%s/_Generated", i3ResourceFile::GetWorkingFolder(), g_pScene->GetName());

	RemoveAllEvents();

	i3System::LookForFiles( szPath, "*.i3Evt", (FINDFILEPROC) EventProc, &m_EventList);

	for( UINT32 i = 0; i < m_EventList.size(); i++)
	{
		i3LevelResEvent * pRes = m_EventList[i];

		INT32 idx = m_EventListCtrl.InsertItem( i, pRes->GetName(), 0);

		UpdateEvent( idx, pRes);
	}
}

void CDlgActionSelect::UpdateEvent( INT32 idx, i3LevelResEvent * pEvent)
{
	m_EventListCtrl.SetItemData( idx, (DWORD_PTR) pEvent);

	char szType[ 512], szCmt[128];
	i3TimeEventGen * pEventGen = pEvent->getTimeEvent();

	szType[0] = 0;

	for( UINT32 i = 0; i < pEventGen->getEventCount(); i++)
	{
		i3TimeEventInfo * pInfo = pEventGen->getEvent( i);

		i3Action * pAction = pInfo->getAction();

		pAction->GetInfoString( szCmt, true, _countof( szCmt)-1);

		strcat_s( szType, _countof( szType)-1, szCmt);

		if( i < (pEventGen->getEventCount()-1))
			strcat_s( szType, _countof( szType)-1, ", ");
	}

	m_EventListCtrl.SetItemText( idx, 1, szType);
}

BOOL CDlgActionSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	i3LevelScene * pScene = i3Level::GetScene();
	I3ASSERT( pScene != NULL);

	if( pScene->hasName() == false)
		return FALSE;

	{
		m_ImageList.Create( 64, 64, ILC_COLOR32, 1, 1);

		m_EventListCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL);
	}

	{
		m_EventListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 100);
		m_EventListCtrl.InsertColumn( 1, "Events", LVCFMT_LEFT, 200);

		m_EventListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	{
		char szPath[ MAX_PATH];
		sprintf( szPath, "Scene/%s/_Generated/*.i3Evt", g_pScene->GetName());

		{
			m_stActionPath.SetWindowText( szPath);
		}

		UpdateAllEvents();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgActionSelect::OnBnClickedBtnNewevent()
{
	// TODO: Add your control notification handler code here
	CDlgEnterEventName dlg;

	if( dlg.DoModal() == IDCANCEL)
	{
		m_pSelEvent = NULL;
		return;
	}
	else
	{
		char conv[MAX_PATH];
		i3::string_ncopy_nullpad( conv, dlg.getEventName(), MAX_PATH -1);

		if( conv[0] == 0)
		{
			::MessageBox( m_hWnd, "  이벤트의 이름을 입력해주세요\n\n 리소스 데이터베이스 구조상 noname은 성립되지 않습니다.", "알림", MB_OK);
			return;
		}

		i3LevelResEvent * pRes;

		for( UINT32 i = 0; i < m_EventList.size(); i++)
		{
			pRes = m_EventList[i];

			if( i3::generic_is_iequal( pRes->GetName(), conv) )
			{
				::MessageBox( m_hWnd, "같은 종류의 리소스가 존재합니다.", "알림", MB_OK);
				return;
			}

		}

		{
			char szPath[ MAX_PATH];

			sprintf( szPath, "Scene/%s/_Generated/%s.i3Evt", g_pScene->GetName(), conv);

			i3TimeEventGen * pGen = i3TimeEventGen::new_object_ref();
			i3LevelResEvent * pEvent = i3LevelResEvent::new_object();
			pEvent->setTimeEvent( pGen);
			pEvent->setPath( szPath);
			pEvent->SetName( conv);

			pEvent->addResState( I3LV_RES_STATE_MODIFIED | I3LV_RES_STATE_LOADED | I3LV_RES_STATE_BRIEF_LOADED);

			g_pResDB->AddRes( pEvent);

			pEvent->OnSerialize();

			m_EventList.push_back( pEvent);

			m_pSelEvent = pEvent;

			INT32 idx = m_EventListCtrl.InsertItem( m_EventListCtrl.GetItemCount(), pEvent->GetName(), 0);

			UpdateEvent( idx, pEvent);

			m_EventListCtrl.EnsureVisible( idx, FALSE);
		}
	}
}

void CDlgActionSelect::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
}

void CDlgActionSelect::OnOK()
{
	POSITION pos = m_EventListCtrl.GetFirstSelectedItemPosition();
	// TODO: Add your specialized code here and/or call the base class
	if( pos != NULL)
	{
		INT32 idx = m_EventListCtrl.GetNextSelectedItem( pos);

		m_pSelEvent = (i3LevelResEvent *) m_EventListCtrl.GetItemData( idx);

		{
			// Resource DB에 Update
			i3LevelRes * pRes = g_pResDB->FindResByPath( m_pSelEvent->getPath());
			
			if( pRes == NULL)
			{
				// 추가되어야 한다.
				g_pResDB->AddRes( m_pSelEvent);
			}
		}
	}

	CDialog::OnOK();
}

void CDlgActionSelect::OnBnClickedBtnDeleteEvent()
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_EventListCtrl.GetFirstSelectedItemPosition();
	if( pos != NULL)
	{
		INT32 idx = m_EventListCtrl.GetNextSelectedItem( pos);

		i3LevelResEvent * pRes = (i3LevelResEvent*)m_EventListCtrl.GetItemData( idx);

		char szPath[MAX_PATH];
		
		sprintf( szPath, "%s/%s", g_pProfile->m_szResPath, pRes->getPath());
		::DeleteFile( szPath);

		sprintf( szPath, "%s/%s", g_pProfile->m_szWorkingDir, pRes->getPath());
		::DeleteFile( szPath);

		// 현재 Scene에서 해당 Resource를 사용하는 모든 Element들을 구하여 삭제한다.
		{
			i3LevelScene * pScene = i3Level::GetScene();
			i3::vector<i3LevelElement3D*> list;

			pScene->FindElementByResource( list, pRes);

			if( list.size() > 1)	i3LevelUndo::BeginBatch();

			for( size_t i = 0; i < list.size(); i++)
			{
				i3LevelElement3D * pElm = list[i];

				pScene->RemoveElement( pElm);
			}

			if( list.size() > 1)	i3LevelUndo::EndBatch();
		}

		// Resource DB에서 제거
		g_pResDB->RemoveRes( pRes);

		i3::vu::remove( m_EventList, pRes);

		m_EventListCtrl.DeleteItem( idx);

		I3_MUST_RELEASE( pRes);

		CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();
		pMainWnd->m_PanelRes.BuildResTree( I3LV_RESDB_TYPE_EVENT);
	}
}

void CDlgActionSelect::OnLvnEndlabeleditListActionSel(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	INT32 idx = pDispInfo->item.iItem;

	i3LevelResEvent * pEvent = (i3LevelResEvent *) m_EventListCtrl.GetItemData( idx);

	I3ASSERT( i3::same_of<i3LevelResEvent*>(pEvent)); //->IsExactTypeOf( i3LevelResEvent::static_meta()));
	
	pEvent->SetName( pDispInfo->item.pszText);

	m_EventListCtrl.SetItemText( idx, 0, pDispInfo->item.pszText);

	*pResult = 0;
}
