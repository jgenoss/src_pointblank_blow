// DlgAIEnumList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "DlgAIEnumList.h"


// cDlgAIEnumList 대화 상자입니다.

IMPLEMENT_DYNAMIC(cDlgAIEnumList, CDialog)

cDlgAIEnumList::cDlgAIEnumList(CWnd* pParent /*=NULL*/)
	: CDialog(cDlgAIEnumList::IDD, pParent)
	,	m_nSelectIndex(0)
{

}

cDlgAIEnumList::~cDlgAIEnumList()
{
}

void cDlgAIEnumList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_AI_ENUM_LIST, m_list_ai_enum);
	DDX_Control(pDX, IDC_EDIT_ENUM_EXPLAIN, m_edit_enum_explain);
	DDX_Control(pDX, IDC_LIST_AI_ENUM_LIST2, m_list_enum_current_weapon);
}


BEGIN_MESSAGE_MAP(cDlgAIEnumList, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_LIST_AI_ENUM_LIST, &cDlgAIEnumList::OnNMClickListAiEnumList)
	ON_BN_CLICKED(IDC_BUTTON_ADD_ENUM, &cDlgAIEnumList::OnBnClickedButtonAddEnum)
END_MESSAGE_MAP()


// cDlgAIEnumList 메시지 처리기입니다.

BOOL cDlgAIEnumList::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_HwndDlgAIEnumList = GetSafeHwnd();

	m_list_ai_enum.SetExtendedStyle(LVS_EX_FLATSB  | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );	
	m_list_ai_enum.Init("AI Enum");
	m_list_ai_enum.InsertColumn(1,"AI Name",LVCFMT_LEFT);

	m_list_enum_current_weapon.SetExtendedStyle(LVS_EX_FLATSB  | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );
	m_list_enum_current_weapon.Init("Current Weapon List");
	
	CRect r;
	m_list_ai_enum.GetClientRect(r);
	m_list_ai_enum.SetColumnWidth (0,r.right/2);
	m_list_ai_enum.SetColumnWidth (1,r.right/2);

	m_list_enum_current_weapon.GetClientRect(r);
	m_list_enum_current_weapon.SetColumnWidth(0, r.right);

	for (size_t ui = 0; ui < m_vAIEnumData.size(); ++ui)
	{
		cAIDriverEnum* pAIEnum = m_vAIEnumData[ui];
		pAIEnum->GetAIEnum();
		pAIEnum->GetAIName();

		m_list_ai_enum.InsertItem(ui,pAIEnum->GetAIEnum().c_str());
		m_list_ai_enum.SetItemText(ui,1,pAIEnum->GetAIName().c_str());
	}

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

LRESULT cDlgAIEnumList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		int nSel = static_cast<int>(lParam);
		m_nSelectIndex = nSel;

		cAIDriverEnum* pAIEnum = m_vAIEnumData[nSel];

		CString strEnumContent = "해당 AIEnum의 설명 'ㅂ'b ";
		CString strExplain;
		strExplain.Format("%s\r\n%s",pAIEnum->GetAIName().c_str(),strEnumContent);

		mAIDriverCon::iterator iter = m_mAIDriverData.begin();
		
		std::vector<std::string> vEnumWeaponList;
		for (; iter != m_mAIDriverData.end(); ++iter)
		{
			cAIDriverData* pAIData = iter->second;

			std::vector<std::string> EnumList;
			std::vector<std::string> weaponList;

			pAIData->GetProcList(EnumList);
			pAIData->GetWepaonList(weaponList);

			for (size_t ui = 0; ui < EnumList.size(); ++ui)
			{
				if (EnumList[ui] == pAIEnum->GetAIName())
				{
					for (size_t ui = 0; ui < weaponList.size(); ++ui)
					{
						vEnumWeaponList.push_back(weaponList[ui].c_str());
					}
				}
			}
		}

		std::sort(vEnumWeaponList.begin(), vEnumWeaponList.end());

		m_list_enum_current_weapon.DeleteAllItems();
		for (size_t ui = 0;ui < vEnumWeaponList.size(); ++ui)
		{
			m_list_enum_current_weapon.InsertItem(ui, vEnumWeaponList[ui].c_str());
		}

		m_edit_enum_explain.SetWindowText(strExplain);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void cDlgAIEnumList::SetAIDriverData(mAIDriverCon* pAIDriverCon)
{
	m_mAIDriverData = *pAIDriverCon;
}

void cDlgAIEnumList::SetAIEnum(vAIEnumCon* pvAIEnum)
{
	m_vAIEnumData = *pvAIEnum;
}

void cDlgAIEnumList::OnNMClickListAiEnumList(NMHDR *pNMHDR, LRESULT *pResult)
{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void cDlgAIEnumList::OnBnClickedButtonAddEnum()
{
	i3AIState * pState = i3AIState::new_object_ref();

	std::string strAIName = m_vAIEnumData[m_nSelectIndex]->GetAIName();
	pState->SetName(strAIName.c_str());

	if (g_pChara->getAICount() == 0)
	{
		AI* pAI = AI::new_object();
		i3AI* pi3Ai = i3AI::new_object();
		pAI->SetAI(pi3Ai);
		g_pChara->AddAI(NULL,pAI);
		//return;
	}

	AI* pAI = g_pChara->getAI(0);
	pAI->getAI()->AddAIState(pState);

	GICShapeAI * pNewShape =	GICShapeAI::new_object_ref();  // (GICShapeAI *) CREATEINSTANCE( GICShapeAI::static_meta());
	pNewShape->SetName(strAIName.c_str());
	pNewShape->setAIState( pState);
	pAI->getDiagram()->AddShape( NULL, pNewShape);
	

	//GICShapeAI* pShape = (GICShapeAI*)pAI->getDiagram()->FindShapeByName(strAIName.c_str());

	I3_TDK_UPDATE_INFO Data;
	Data.m_Event = I3_TDK_UPDATE_EDIT;
	::SendMessage(g_HwndDlgWeaponEditor, WM_TDK_UPDATE, (WPARAM) &Data, NULL);

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
