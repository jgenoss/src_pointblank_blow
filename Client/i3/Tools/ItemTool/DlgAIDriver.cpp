// cDlgAIDriver.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "DlgAIDriver.h"

// cDlgAIDriver 대화 상자입니다.

static const COLORREF colSel1		= RGB(240,247,249);
static const COLORREF colSel2		= RGB(229,232,239);
static const COLORREF colDisable1	= RGB(128,255,255);
static const COLORREF colDisable2	= RGB(200,255,255);

IMPLEMENT_DYNAMIC(cDlgAIDriver, CDialog)

cDlgAIDriver::cDlgAIDriver(CWnd* pParent /*=NULL*/)
	: CDialog(cDlgAIDriver::IDD, pParent)
	,	m_eSearchType(E_SEARCH_AIDRIVE)
{

}

cDlgAIDriver::~cDlgAIDriver()
{
}

void cDlgAIDriver::SetAIDriverData(mAIDriverCon* pAIDriverCon)
{
	m_mAIDriverData = *pAIDriverCon;
}

void cDlgAIDriver::SetAIEnum(vAIEnumCon* pvAIEnum)
{
	m_vAIEnumData = *pvAIEnum;
}

void cDlgAIDriver::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_AIDRIVER, m_list_aidriver);
	DDX_Control(pDX, IDC_LIST_AIDRIVER_WEAPON, m_list_aidriver_weapon);
	DDX_Control(pDX, IDC_LIST_AIDRIVER_TABLE, m_list_aidriver_table);
	DDX_Control(pDX, IDC_RADIO_AIDRIVER_SEARCH, m_btn_aidriver_search);
	DDX_Control(pDX, IDC_RADIO_WEAPON_SEARCH, m_btn_weapon_search);
	DDX_Control(pDX, IDC_RADIO_TABLE_SEARCH, m_btn_table_search);
}


BEGIN_MESSAGE_MAP(cDlgAIDriver, CDialog)
	ON_BN_CLICKED(IDC_RADIO_AIDRIVER_SEARCH, &cDlgAIDriver::OnBnClickedRadioAidriverSearch)
	ON_BN_CLICKED(IDC_RADIO_WEAPON_SEARCH, &cDlgAIDriver::OnBnClickedRadioWeaponSearch)
	ON_BN_CLICKED(IDC_RADIO_TABLE_SEARCH, &cDlgAIDriver::OnBnClickedRadioTableSearch)
	ON_BN_CLICKED(IDC_ADD_WEAPON, &cDlgAIDriver::OnBnClickedAddWeapon)
END_MESSAGE_MAP()


BOOL cDlgAIDriver::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_HwndDlgAIDriver = GetSafeHwnd();

	m_list_aidriver.Init("AIDriver List");
	m_list_aidriver_weapon.Init("Weapon List");
	m_list_aidriver_table.Init("Table List");

	m_list_aidriver.SetExtendedStyle(LVS_EX_FLATSB  | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_list_aidriver_weapon.SetExtendedStyle(LVS_EX_FLATSB  | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );
	m_list_aidriver_table.SetExtendedStyle(LVS_EX_FLATSB  | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

	m_list_aidriver_weapon.m_colRow1 = RGB(128,255,255);
	m_list_aidriver_weapon.m_colRow2 = RGB(200,255,255);

	m_list_aidriver_table.m_colRow1 = RGB(128,255,255);
	m_list_aidriver_table.m_colRow2 = RGB(200,255,255);

	std::vector<std::string > vTmp1;
	std::vector<std::string > vTmp2;

	mAIDriverCon::iterator iter = m_mAIDriverData.begin();

	m_vAIDriver.resize(m_mAIDriverData.size());
	INT32 nAIDriverCnt = 0;
	for (;iter != m_mAIDriverData.end(); ++iter, ++nAIDriverCnt)
	{
		std::string strName = iter->first;	//AIDriver Name;
		m_list_aidriver.InsertItem(nAIDriverCnt, strName.c_str());
		
		cAIDriverData* pAIData = iter->second;
		m_vAIDriver[nAIDriverCnt] = pAIData;
		m_vAIDriver[nAIDriverCnt]->SetName(strName);

		std::vector<std::string> weaponList;
		std::vector<std::string> procList;

		pAIData->GetWepaonList(weaponList);
		pAIData->GetProcList(procList);

		for (size_t ui = 0; ui < weaponList.size(); ++ui)
		{
			m_mWeaponList[weaponList[ui].c_str()] = strName;
		}

		for (size_t ui = 0; ui < procList.size(); ++ui)
		{
			m_mvProcList[procList[ui].c_str()].push_back(strName);
		}
	}

	//std::sort(vTmp1.begin(),vTmp1.end());
	//std::sort(vTmp2.begin(),vTmp2.end());
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

LRESULT cDlgAIDriver::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		int nClick = static_cast<int>(wParam);
		if (nClick == 0)
		{
			int nSel = static_cast<int>(lParam);
			
			I3ASSERT(nSel != -1);

			if (m_vAIDriver.size() > (UINT32)nSel)
			{
				cAIDriverData* pAIData = m_vAIDriver[nSel];

				if (m_eSearchType == E_SEARCH_AIDRIVE)
				{
					std::vector<std::string> weaponList;
					std::vector<std::string> procList;

					pAIData->GetWepaonList(weaponList);
					pAIData->GetProcList(procList);

					m_list_aidriver_weapon.DeleteAllItems();
					m_list_aidriver_table.DeleteAllItems();
					for (size_t ui = 0; ui < weaponList.size(); ++ui)
					{
						m_list_aidriver_weapon.InsertItem(ui, weaponList[ui].c_str());
					}

					for (size_t ui = 0; ui < procList.size(); ++ui)
					{
						m_list_aidriver_table.InsertItem(ui, procList[ui].c_str());
					}
				}
				//else// 검색해서 찾은 값이다... 추가될 대상..
				{
					//int nCnt = m_list_aidriver.GetItemCount();
					//m_list_aidriver.GetSelectedColumn();
					m_strCurrAIDriver = m_list_aidriver.GetItemText(nSel,0);
				//	m_mAIDriverData[szSelAIDriver];
					//m_strCurrAIDriver = nSel;
				}
			}
		}
		if (nClick == 1)
		{
			int nSel = static_cast<int>(lParam);
			I3ASSERT(nSel != -1);

			if (m_eSearchType == E_SEARCH_WEAPON)
			{
				m_list_aidriver.DeleteAllItems();
				m_list_aidriver_table.DeleteAllItems();

				mStringListCon::iterator iter = m_mWeaponList.begin();

				int nIdx = 0;
				for (; iter != m_mWeaponList.end(); ++iter, ++nIdx)
				{
					if (nIdx == nSel)
					{
						m_list_aidriver.InsertItem(0, (iter->second).c_str());
					}
				}
			}
			else
			{

			}
		}
		if (nClick == 2)
		{
			int nSel = static_cast<int>(lParam);
			I3ASSERT(nSel != -1);

			if (m_eSearchType == E_SEARCH_TABLE)
			{
				m_list_aidriver.DeleteAllItems();
				m_list_aidriver_weapon.DeleteAllItems();

				mvecStringListCon::iterator iter = m_mvProcList.begin();

				int nIdx = 0;
				for (; iter != m_mvProcList.end(); ++iter, ++nIdx)
				{
					if (nIdx == nSel)
					{
						std::vector<std::string> vAIDriverList = iter->second;
						for (size_t ui = 0; ui < vAIDriverList.size(); ++ui)
							m_list_aidriver.InsertItem(ui, vAIDriverList[ui].c_str());
					}
				}
			}
			else
			{

			}
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void cDlgAIDriver::OnBnClickedRadioAidriverSearch()
{
	m_eSearchType = E_SEARCH_AIDRIVE;

	m_btn_aidriver_search.SetCheck(1);
	m_btn_weapon_search.SetCheck(0);
	m_btn_table_search.SetCheck(0);
	
	m_list_aidriver.DeleteAllItems();
	m_list_aidriver_weapon.DeleteAllItems();
	m_list_aidriver_table.DeleteAllItems();

	m_list_aidriver.m_colRow1 = colSel1;
	m_list_aidriver.m_colRow2 = colSel2;

	m_list_aidriver_weapon.m_colRow1 = colDisable1;
	m_list_aidriver_weapon.m_colRow2 = colDisable2;

	m_list_aidriver_table.m_colRow1 = colDisable1;
	m_list_aidriver_table.m_colRow2 = colDisable2;


	mAIDriverCon::iterator iter = m_mAIDriverData.begin();

	int nIdx = 0;
	for (;iter != m_mAIDriverData.end(); ++iter, ++nIdx)
	{
		m_list_aidriver.InsertItem(nIdx, (iter->first).c_str());
	}


	Invalidate(FALSE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void cDlgAIDriver::OnBnClickedRadioWeaponSearch()
{
	m_eSearchType = E_SEARCH_WEAPON;

	m_btn_aidriver_search.SetCheck(0);
	m_btn_weapon_search.SetCheck(1);
	m_btn_table_search.SetCheck(0);
	
	m_list_aidriver.DeleteAllItems();
	m_list_aidriver_weapon.DeleteAllItems();
	m_list_aidriver_table.DeleteAllItems();

	m_list_aidriver.m_colRow1 = colDisable1;
	m_list_aidriver.m_colRow2 = colDisable2;

	m_list_aidriver_weapon.m_colRow1 = colSel1;
	m_list_aidriver_weapon.m_colRow2 = colSel2;

	m_list_aidriver_table.m_colRow1 = colDisable1;
	m_list_aidriver_table.m_colRow2 = colDisable2;


	mStringListCon::iterator iter = m_mWeaponList.begin();

	int nIdx = 0;
	for (; iter != m_mWeaponList.end(); ++iter, ++nIdx)
	{
		m_list_aidriver_weapon.InsertItem(nIdx, (iter->first).c_str());
	}


	Invalidate(FALSE);
}

void cDlgAIDriver::OnBnClickedRadioTableSearch()
{
	m_eSearchType = E_SEARCH_TABLE;

	m_btn_aidriver_search.SetCheck(0);
	m_btn_weapon_search.SetCheck(0);
	m_btn_table_search.SetCheck(1);

	m_list_aidriver.DeleteAllItems();
	m_list_aidriver_weapon.DeleteAllItems();
	m_list_aidriver_table.DeleteAllItems();

	m_list_aidriver.m_colRow1 = colDisable1;
	m_list_aidriver.m_colRow2 = colDisable2;

	m_list_aidriver_weapon.m_colRow1 = colDisable1;
	m_list_aidriver_weapon.m_colRow2 = colDisable2;

	m_list_aidriver_table.m_colRow1 = colSel1;
	m_list_aidriver_table.m_colRow2 = colSel2;


	mvecStringListCon::iterator iter = m_mvProcList.begin();

	int nIdx = 0;
	for (; iter != m_mvProcList.end(); ++iter, ++nIdx)
	{
		m_list_aidriver_table.InsertItem(nIdx, (iter->first).c_str());
	}


	Invalidate(FALSE);
	
}

void cDlgAIDriver::OnBnClickedAddWeapon()
{
	if (m_strCurrAIDriver == -1)
		return;
	I3ASSERT(m_vAIDriver.size() != 0);

	CString strAIDriver = "AIDriver" + m_strCurrAIDriver;
	CString strOutMessage;
	strOutMessage.Format("%s를 추가합니다.",strAIDriver);
	if (IDOK == AfxMessageBox(strOutMessage,MB_OKCANCEL))
	{
		//현재 선택된 AIDriver
		cAIDriverData* pAIDriverData = m_mAIDriverData[(LPTSTR)(LPCTSTR)m_strCurrAIDriver];

		if (g_pChara->getAICount() > 0)
			g_pChara->RemoveAI(0);

		//새로운 AI를 추가한다.
		AI* pAI		= AI::new_object();
		pAI->SetName("Default");

		i3AI* pi3Ai = i3AI::new_object();
		pi3Ai->setDriverMetaName(strAIDriver);

		pAI->SetAI(pi3Ai);
		g_pChara->AddAI(NULL,pAI);

		vStrAIDriveListCon vTableList;
		pAIDriverData->GetProcList(vTableList);

		for (size_t ui = 0; ui < vTableList.size(); ++ui)
		{
			std::string strTable = vTableList[ui];

			i3AIState * pState = i3AIState::new_object_ref();
			pState->SetName(strTable.c_str());

			pAI->getAI()->AddAIState(pState);

			GICShapeAI * pNewShape = GICShapeAI::new_object_ref(); 
			pNewShape->SetName(strTable.c_str());
			pNewShape->setAIState( pState);

			pAI->getDiagram()->AddShape( NULL, pNewShape);
		}

		I3_TDK_UPDATE_INFO Data;
		Data.m_Event = I3_TDK_UPDATE_EDIT;
		::SendMessage(g_HwndDlgWeaponEditor, WM_TDK_UPDATE, (WPARAM) &Data, NULL);

	}
}
