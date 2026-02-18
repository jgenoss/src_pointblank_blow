#include "pch.h"
#include "Prop_LvElm_HitMap.h"
#include "i3LevelDesign.h"
#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/atof.h"

IMPLEMENT_DYNAMIC(CProp_LvElm_HitMapBuild, i3TDKDialogBase)
BEGIN_MESSAGE_MAP(CProp_LvElm_HitMapBuild, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_BUTTON_BUILD, &CProp_LvElm_HitMapBuild::OnBnClickedButtonBuild)
END_MESSAGE_MAP()


CProp_LvElm_HitMapBuild::CProp_LvElm_HitMapBuild(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvElm_HitMapBuild::IDD, pParent)
{
}


CProp_LvElm_HitMapBuild::~CProp_LvElm_HitMapBuild()
{
}

BOOL CProp_LvElm_HitMapBuild::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;
}

void CProp_LvElm_HitMapBuild::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PROGRESS_BUILD, m_BuildProgressCtrl);
}

void CProp_LvElm_HitMapBuild::OnBnClickedButtonBuild()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame * pMain = theApp.GetMainFrm();
	std::map<UINT64, KILLERUSER> vecKiller = pMain->m_pHitMapMng->GetKillerUserList();
	std::map<UINT64, KILLERUSER>::iterator iter;

	pMain->m_pFramework->GetLevelHitMap()->DeleteAllPoint();

	char pchKillID[MAX_PATH];
	char pchDeathID[MAX_PATH];
	REAL32 startTime;
	REAL32 endTime;
	REAL32 playTime;
	int nIndex;

	for (iter = vecKiller.begin(); iter != vecKiller.end(); ++iter)
	{
		nIndex = 0;
		m_BuildProgressCtrl.SetRange(0, (short)iter->second.veckill.size());
		for (UINT32 i = 0; i < iter->second.veckill.size(); i++)
		{
			m_TimeSeqCtrl->GetSeqTime(startTime, endTime);
			playTime = iter->second.veckill[i].m_ui32PlayTime / 60.0f;
			if (playTime >= startTime && playTime <= endTime)			//시간 체크
			{
				sprintf_s(pchKillID, MAX_PATH, "Killer :%I64d (Time:%d)", iter->second.veckill[i].m_ui64KillerUID, iter->second.veckill[i].m_ui32PlayTime);
				sprintf_s(pchDeathID, MAX_PATH, "Target :%I64d (Time:%d)", iter->second.veckill[i].m_ui64DeathUID, iter->second.veckill[i].m_ui32PlayTime);
		
				pMain->m_pFramework->GetLevelHitMap()->AddHitPoint(
					iter->second.ui64KillerUID,
					pchKillID,
					pchDeathID,
					VEC3D(iter->second.veckill[i].m_vec3KillPos.x, iter->second.veckill[i].m_vec3KillPos.y, iter->second.veckill[i].m_vec3KillPos.z),
					VEC3D(iter->second.veckill[i].m_vec3DeathPos.x, iter->second.veckill[i].m_vec3DeathPos.y, iter->second.veckill[i].m_vec3DeathPos.z)
					);
			}
			m_BuildProgressCtrl.SetPos(++nIndex);
		}
	}
}

IMPLEMENT_DYNAMIC(CProp_LvElm_HitMap, i3TDKDialogBase)
BEGIN_MESSAGE_MAP(CProp_LvElm_HitMap, i3TDKDialogBase)

	ON_CBN_SELCHANGE(IDC_CB_ROOMSELECT, &CProp_LvElm_HitMap::OnCbnSelchangeCbRoomselect)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PLAYER, &CProp_LvElm_HitMap::OnLvnItemchangedListPlayer)
	ON_BN_CLICKED(IDC_RADIO_ETC, &CProp_LvElm_HitMap::OnBnClickedRadioEtc)
	ON_BN_CLICKED(IDC_RADIO_MELEE, &CProp_LvElm_HitMap::OnBnClickedRadioMelee)
	ON_BN_CLICKED(IDC_RADIO_ALL, &CProp_LvElm_HitMap::OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_RADIO_PRIMARY, &CProp_LvElm_HitMap::OnBnClickedRadioPrimary)
	ON_BN_CLICKED(IDC_RADIO_SECONDARY, &CProp_LvElm_HitMap::OnBnClickedRadioSecondary)
	ON_BN_CLICKED(IDC_RADIO_THROWING, &CProp_LvElm_HitMap::OnBnClickedRadioThrowing)
	ON_EN_CHANGE(IDC_EDIT_STARTTIME, &CProp_LvElm_HitMap::OnEnChangeEditStarttime)
	ON_EN_CHANGE(IDC_EDIT_ENDTIME, &CProp_LvElm_HitMap::OnEnChangeEditEndtime)


END_MESSAGE_MAP()


CProp_LvElm_HitMap::CProp_LvElm_HitMap(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvElm_HitMap::IDD, pParent)
{
}


CProp_LvElm_HitMap::~CProp_LvElm_HitMap()
{
}

void CProp_LvElm_HitMap::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CB_ROOMSELECT, m_RoomSelectCtrl);
	DDX_Control(pDX, IDC_LIST_PLAYER, m_PlayerListCtrl);
	DDX_Control(pDX, IDC_EDIT_STARTTIME, m_StartTimeEdit);
	DDX_Control(pDX, IDC_EDIT_ENDTIME, m_EndTimeEdit);
}
BOOL CProp_LvElm_HitMap::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	m_RoomSelectCtrl.AddString("(NONE)");
	m_RoomSelectCtrl.SetCurSel(0);

	m_PlayerListCtrl.InsertColumn(0, "ID", LVCFMT_LEFT, 100);
	m_PlayerListCtrl.InsertColumn(1, "Name", LVCFMT_LEFT, 200);

	m_PlayerListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL);
	pButton->SetCheck(true);

	return TRUE; 
}


void CProp_LvElm_HitMap::ResetRoomList()
{
	CMainFrame * pMain = theApp.GetMainFrm();

	if (pMain)
	{
		m_RoomSelectCtrl.ResetContent();
		PLAY_ROOM_LIST* pList = pMain->m_pHitMapMng->GetRoomList();

		//char pchID[MAX_PATH];
		char strRoomItem[ MAX_PATH ];
		char strMapName[ MAX_PATH ];
		char strDate[ MAX_PATH ];

		for (int i = 0; i < pMain->m_pHitMapMng->GetRoomListCount(); i++)
		{
			//i3::itoa(pList[i].m_ui64BattleUID, pchID, _countof(pchID));

			// Battle Time
			sprintf_s( strDate, MAX_PATH, "%04d/%02d/%02d %02d:%02d", 
				pList[i].m_dtBattleStartDate.GetYear(), pList[i].m_dtBattleStartDate.GetMonth(), pList[i].m_dtBattleStartDate.GetDay(), 
				pList[i].m_dtBattleStartDate.GetHour(), pList[i].m_dtBattleStartDate.GetMinute() );
			// StageName
			//getStageDisplayName( strMapName, getStageId( pList[i].m_ui32MapID ) );

			sprintf_s( strRoomItem, MAX_PATH, "%s [%s]", strDate, strMapName );

			m_RoomSelectCtrl.AddString( strRoomItem );
		}

		m_PlayerListCtrl.DeleteAllItems();
	}
}

void CProp_LvElm_HitMap::ReLoadKillList(WEAPON_SLOT_TYPE slotType)
{
	CMainFrame * pMain = theApp.GetMainFrm();
	PLAY_ROOM_LIST* pRoomList = pMain->m_pHitMapMng->GetRoomList();

	if (!pRoomList) return;
	UINT64 ui64BattleUID = pRoomList[m_RoomSelectCtrl.GetCurSel()].m_ui64BattleUID;
	pMain->m_pHitMapMng->GetKillList(ui64BattleUID, slotType);

	std::map<UINT64, KILLERUSER> vecKiller = pMain->m_pHitMapMng->GetKillerUserList();
	m_PlayerListCtrl.DeleteAllItems();

	char pchID[MAX_PATH];
	std::map<UINT64, KILLERUSER>::iterator iter;
	int Index = 0;
	for (iter = vecKiller.begin(); iter != vecKiller.end(); ++iter)
	{
		i3::itoa(iter->first, pchID, _countof(pchID));
		m_PlayerListCtrl.InsertItem(Index, pchID);
		//m_PlayerListCtrl.SetItemText(i, 1, "김기원");
		Index++;
	}
}


void CProp_LvElm_HitMap::OnCbnSelchangeCbRoomselect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame * pMain = theApp.GetMainFrm();
	PLAY_ROOM_LIST* pRoomList = pMain->m_pHitMapMng->GetRoomList();
	
	if (!pRoomList) return;

	ReLoadKillList(WEAPON_SLOT_TYPE::WEAPON_SLOT_ALL);;

	std::map<UINT64, KILLERUSER> vecKiller = pMain->m_pHitMapMng->GetKillerUserList();

	//playtime 설정
	if (vecKiller.size() > 0)
	{
		UINT32 nPlayTime = RoomSetting::GetBattleTimeToSec(pRoomList[m_RoomSelectCtrl.GetCurSel()].m_ui32RoomSetting);
		if (0x80000000 != nPlayTime)
		{
			nPlayTime = nPlayTime / 60;	//초를 분으로 변경
			m_TimeSeqCtrl->SetTimeSequence((REAL32)nPlayTime);

			char chPlayTime[MAX_PATH];
			i3::itoa(nPlayTime, chPlayTime, 10);
			m_StartTimeEdit.SetWindowText("0");
			m_EndTimeEdit.SetWindowText(chPlayTime);
		}
		else
		{
			m_TimeSeqCtrl->DeleteTimeSequence();
		}
	}
	else
	{
		m_TimeSeqCtrl->DeleteTimeSequence();
	}
}


void CProp_LvElm_HitMap::OnLvnItemchangedListPlayer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame * pMain = theApp.GetMainFrm();
	CString UserID = m_PlayerListCtrl.GetItemText(pNMLV->iItem, 0);
	pMain->m_pFramework->GetLevelHitMap()->SetColorChange(atoi(UserID));

	*pResult = 0;
}

void CProp_LvElm_HitMap::OnBnClickedRadioAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ReLoadKillList(WEAPON_SLOT_TYPE::WEAPON_SLOT_ALL);
}

void CProp_LvElm_HitMap::OnBnClickedRadioPrimary()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ReLoadKillList(WEAPON_SLOT_TYPE::WEAPON_SLOT_PRIMARY);
}

void CProp_LvElm_HitMap::OnBnClickedRadioSecondary()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ReLoadKillList(WEAPON_SLOT_TYPE::WEAPON_SLOT_SECONDARY);
}

void CProp_LvElm_HitMap::OnBnClickedRadioThrowing()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ReLoadKillList(WEAPON_SLOT_TYPE::WEAPON_SLOT_THROWING1);
}

void CProp_LvElm_HitMap::OnBnClickedRadioEtc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ReLoadKillList(WEAPON_SLOT_TYPE::WEAPON_SLOT_COUNT);
}

void CProp_LvElm_HitMap::OnBnClickedRadioMelee()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ReLoadKillList(WEAPON_SLOT_TYPE::WEAPON_SLOT_MELEE);
}


void CProp_LvElm_HitMap::OnEnChangeEditStarttime()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// i3TDKDialogBase::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	TCHAR conv[256];
	m_StartTimeEdit.GetWindowText(conv, _countof(conv));
	REAL32 fStartTime = (REAL32)i3::atof(conv);

	m_EndTimeEdit.GetWindowText(conv, _countof(conv));
	REAL32 fEndTime = (REAL32)i3::atof(conv);
	m_TimeSeqCtrl->SetStartTime(fStartTime);

	if (fEndTime - fStartTime > 0)
	{
		m_TimeSeqCtrl->SetDuration(fEndTime - fStartTime);
	}
}


void CProp_LvElm_HitMap::OnEnChangeEditEndtime()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// i3TDKDialogBase::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	TCHAR conv[256];

	m_StartTimeEdit.GetWindowText(conv, _countof(conv));
	REAL32 fStartTime = (REAL32)i3::atof(conv);

	m_EndTimeEdit.GetWindowText(conv, _countof(conv));
	REAL32 fEndTime = (REAL32)i3::atof(conv);

	if (fEndTime - fStartTime > 0)
	{
		m_TimeSeqCtrl->SetDuration(fEndTime - fStartTime);
	} 
}

BOOL CProp_LvElm_HitMap::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if( pMsg->wParam == VK_RETURN)
		{
			return false;
		}
	}

	return i3TDKDialogBase::PreTranslateMessage(pMsg);
}

void CProp_LvElm_HitMap::SetSeqTime(char* chStartTime, char* chEndTime)
{
	m_StartTimeEdit.SetWindowText(chStartTime);
	m_EndTimeEdit.SetWindowText(chEndTime);
}