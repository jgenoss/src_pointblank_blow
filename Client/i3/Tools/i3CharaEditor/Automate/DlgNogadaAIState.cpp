
#include "stdafx.h"

#ifdef _USE_TOOLAM_

#include "resource.h"
#include "AIStateEditor_Ctl.h"
#include "DlgNogadaAIState.h"


namespace AIStateEdit
{

IMPLEMENT_DYNAMIC(DlgNogadaAIState, CDialog)

BEGIN_MESSAGE_MAP(DlgNogadaAIState, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_AIED_BT_FIND_UPPER_WEAPON_BY_NAME, OnBtn_FindUpperWeaponName)
	ON_BN_CLICKED(IDC_AIED_BT_RESCAN_WEAPON_CLASSANDNAME, OnBtn_RescanUpperWeaponClassAndName)
	ON_BN_CLICKED(IDC_AIED_BT_QUERY_UPPER_WEAPON, OnBtn_QueryUpperWeaponName)
END_MESSAGE_MAP()


DlgNogadaAIState::DlgNogadaAIState(CWnd* pParent /*=NULL*/, AIStateEditCallBack* callback)
: CDialog(DlgNogadaAIState::IDD, pParent)
{
//	m_listView = NULL;
	m_callback = callback;
}

DlgNogadaAIState::~DlgNogadaAIState()
{
}

void DlgNogadaAIState::RescanUpperWeaponCalssAndName()
{
	m_callback->AIStateEditCB_ScanUpperWeaponClassName(&m_upperWeaponClassScan);

	m_upperWeaponClassScan.Analyze();
}

//
// overrides
//

void DlgNogadaAIState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL DlgNogadaAIState::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitCtl();

	CRect rcFrame;
	GetDlgItem(IDC_AIED_ST_AREA_AISTATE)->GetWindowRect(&rcFrame);
	ScreenToClient(&rcFrame);

	m_lv.MoveWindow(&rcFrame, TRUE);
	m_lv.RefreshAIState();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void DlgNogadaAIState::OnCancel()
{
	EndDialog(1);
}

void DlgNogadaAIState::OnOK()
{
	EndDialog(1);
}

void DlgNogadaAIState::PostNcDestroy()
{
	CDialog::PostNcDestroy();

	//if(m_listView)
	//{
	//	delete m_listView;
	//	m_listView = NULL;
	//}
}

void DlgNogadaAIState::InitCtl()
{
	memset(m_ctl, 0, sizeof(m_ctl));

	m_ctl[BT_FIND_UPPER_WEAPON_NAME				] = IDC_AIED_BT_FIND_UPPER_WEAPON_BY_NAME;;
	m_ctl[EB_FIND_UPPER_WEAPON_NAME				] = IDC_AIED_EB_FIND_UPPER_WEAPON_NAME;

	m_ctl[CB_UPPER_WEAPON_CLASS					] = IDC_AIED_CB_UPPER_WEAPON_CLASS;
	m_ctl[CB_UPPER_WEAPON_NAME					] = IDC_AIED_CB_UPPER_WEAPON_NAME;
	m_ctl[BT_RESCAN_UPPER_WEAPON_CLASSANDNAME	] = IDC_AIED_BT_RESCAN_WEAPON_CLASSANDNAME;
	m_ctl[BT_QUERY_UPPER_WEAPON_NAME			] = IDC_AIED_BT_QUERY_UPPER_WEAPON;

	m_ctl[ST_AREA_UPPER_AISTATE					] = IDC_AIED_ST_AREA_AISTATE;
}


//
// message map handeler
//

int DlgNogadaAIState::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CDialog::OnCreate(lpCreateStruct);

	m_lv.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_AIED_LV_AISTATE);
	m_lv.SetImgListState(IDB_AISTATE_STATEIMG, 16, 1, RGB(255,0,0));

	return TRUE;
}

void DlgNogadaAIState::OnBtn_FindUpperWeaponName()
{
	
}

void DlgNogadaAIState::OnBtn_RescanUpperWeaponClassAndName()
{
	RescanUpperWeaponCalssAndName();
}

void DlgNogadaAIState::OnBtn_QueryUpperWeaponName()
{

}


} // end of namespace AIStateEdit

#endif // _USE_TOOLAM_