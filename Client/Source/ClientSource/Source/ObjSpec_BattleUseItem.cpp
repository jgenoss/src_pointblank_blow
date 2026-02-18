// ObjSpec_BattleUseItem.cpp : 구현 파일입니다.
//
#include "pch.h"
#if defined(USE_EDITDLG)
#include "ObjSpec_BattleUseItem.h"
#include "GameObject.h"
#include "i3Level/i3LevelElement3D.h"
#include "DB/Ado.h"

#define ON_CBN_SELCHANGE_EX(id, memberFxn)    \
      { WM_COMMAND, (WORD)CBN_SELCHANGE, (WORD)id, (WORD)id, AfxSigCmd_RANGE,   \
       (AFX_PMSG)(void (AFX_MSG_CALL CCmdTarget::*)(UINT32))memberFxn },

// ObjSpec_BattleUseItem 대화 상자입니다.
IMPLEMENT_DYNAMIC(ObjSpec_BattleUseItem, i3GameObjPropertyDialog)

ObjSpec_BattleUseItem::ObjSpec_BattleUseItem(CWnd* pParent /*=NULL*/)
	: i3GameObjPropertyDialog(ObjSpec_BattleUseItem::IDD, pParent)
{
	m_bItemListUp = false;
}

ObjSpec_BattleUseItem::~ObjSpec_BattleUseItem()
{
	size_t count = itemList_primary.size();
	for (size_t i = 0; i < count; i++)
	{
		delete itemList_primary[i];
	}

	count = itemList_secondary.size();
	for (size_t i = 0; i < count; i++)
	{
		delete itemList_secondary[i];
	}

	count = itemList_item.size();
	for (size_t i = 0; i < count; i++)
	{
		delete itemList_item[i];
	}

	itemList_primary.clear();
	itemList_secondary.clear();
	itemList_item.clear();
}

void ObjSpec_BattleUseItem::SetObject(i3ElementBase * pObj)
{
	i3GameObjPropertyDialog::SetObject(pObj);

	i3LevelElement3D * pElm = (i3LevelElement3D *)getObject();
	if (pElm == NULL)
		return;

	CGameObjectBattleUseItem * pItemBox = (CGameObjectBattleUseItem *)pElm->CreateGameInstance();
	
	// 여기서 내가 가지고 있는 것들을 세팅합니다
	char conv[128];

	// 1. start Time
	i3::ftoa(pItemBox->GetStartTime(), conv);
	m_ED_StartTime.SetWindowText(conv);

	// 2. Respawn Time
	i3::ftoa(pItemBox->GetRespawnTime(), conv);
	m_ED_RespawnTime.SetWindowText(conv);

	if (m_bItemListUp == true)
	{
		for (INT32 i = 0; i < MAX_ITEM_COUNT; i++)
		{
			if (pItemBox->GetItemId(i) != 0)
			{
				// 3. ItemType
				INT32 curSel = _SetItemType(GET_ITEM_TYPE(pItemBox->GetItemId(i)), &m_CB_ItemType[i]);

				// 4. Item Name
				_SetItemName(curSel, pItemBox->GetItemId(i), &m_CB_ItemName[i]);
			}
			else
			{
				m_CB_ItemType[i].SetCurSel(-1);
				_ComboBoxClear(&m_CB_ItemName[i]);
			}
		}
	}
	else
	{
		m_CB_ItemName[0].AddString(" Item List Up 버튼을 눌러주세요");
		m_CB_ItemName[0].SetCurSel(0);
	}
	
	// 5. Probability
	for (INT32 i = 0; i < MAX_ITEM_COUNT; i++)
	{
		i3::ftoa(pItemBox->GetItemProbability(i), conv);
		m_ED_ItemProbability[i].SetWindowText(conv);
	}
	
	_SumProbability(pItemBox);

	I3_MUST_RELEASE(pItemBox);
}

void ObjSpec_BattleUseItem::_SumProbability(CGameObjectBattleUseItem * pObject)
{
	char conv[128];
	int sum = 0;
	for (int i = 0; i < MAX_ITEM_COUNT; i++)
	{
		m_ED_ItemProbability[i].GetWindowText(conv, sizeof(conv) - 1);
		int prob = atoi(conv);
		sum += prob;
		pObject->SetItemProbability(i, prob);
	}
	
	itoa(sum, conv, 10);
	m_ED_ProbText.SetWindowTextA(conv);
}

void ObjSpec_BattleUseItem::_SetType()
{
	for (INT32 i = 0; i < MAX_ITEM_COUNT; i++)
	{
		m_CB_ItemType[i].AddString("PRIMARY");
		m_CB_ItemType[i].AddString("SECONDARY");
		m_CB_ItemType[i].AddString("ITEM");
	}
}

bool ObjSpec_BattleUseItem::_isListUpType(INT32 type)
{
	switch (type)
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_WRAP_COUNT:
		return true;
	}

	return false;
}

void ObjSpec_BattleUseItem::_ComboBoxClear(CComboBox * pCB)
{
	// Delete every item from the combo box.
	for (int i = pCB->GetCount() - 1; i >= 0; i--)
	{
		pCB->DeleteString(i);
	}
	pCB->SetCurSel(0);
}

void ObjSpec_BattleUseItem::_CBAddPrimaryString(CComboBox * pCB)
{
	size_t listCount = itemList_primary.size();
	for (size_t i = 0; i < listCount; i++)
	{
		pCB->AddString(itemList_primary[i]->itemName);
	}
}

void ObjSpec_BattleUseItem::_CBAddSecondaryString(CComboBox * pCB)
{
	size_t listCount = itemList_secondary.size();
	for (size_t i = 0; i < listCount; i++)
	{
		pCB->AddString(itemList_secondary[i]->itemName);
	}
}

void ObjSpec_BattleUseItem::_CBAddItemString(CComboBox * pCB)
{
	size_t listCount = itemList_item.size();
	for (size_t i = 0; i < listCount; i++)
	{
		pCB->AddString(itemList_item[i]->itemName);
	}
}

T_ItemID ObjSpec_BattleUseItem::_GetPrimaryItemIDbyIdx(INT32 idx)
{
	if (idx < (INT32)itemList_primary.size())
	{
		return itemList_primary[idx]->itemID;
	}

	return 0;
}

T_ItemID ObjSpec_BattleUseItem::_GetSecondaryItemIDbyIdx(INT32 idx)
{
	if (idx < (INT32)itemList_secondary.size())
	{
		return itemList_secondary[idx]->itemID;
	}

	return 0;
}

T_ItemID ObjSpec_BattleUseItem::_GetItemItemIDbyIdx(INT32 idx)
{
	if (idx < (INT32)itemList_item.size())
	{
		return itemList_item[idx]->itemID;
	}

	return 0;
}

INT32 ObjSpec_BattleUseItem::_SetItemType(INT32 type, CComboBox * pCB)
{
	INT32 idx = -1;
	switch (type)
	{
	case 1:
		idx = 0;
		break;
	case 2:
		idx = 1;
		break;
	case 18:
		idx = 2;
		break;
	}

	if(pCB != NULL)
		pCB->SetCurSel(idx);

	return idx;
}

void ObjSpec_BattleUseItem::_SetItemName(INT curSel, T_ItemID itemID, CComboBox * pCB)
{
	if (pCB != NULL)
	{
		// 클리어 후
		_ComboBoxClear(pCB);
		
		// 종류 파악 후 리스트 업 한 뒤 Sel 선택
		switch (curSel)
		{
		case 0:
		{
			_CBAddPrimaryString(pCB);
			pCB->SetCurSel(_GetPrimaryListIdxByItemID(itemID));
		}
		break;
		case 1:
		{
			_CBAddSecondaryString(pCB);
			pCB->SetCurSel(_GetSecondaryListIdxByItemID(itemID));
		}
		break;
		case 2:
		{
			_CBAddItemString(pCB);
			pCB->SetCurSel(_GetItemListIdxByItemID(itemID));
		}
		break;
		}
	}
}

INT32 ObjSpec_BattleUseItem::_GetPrimaryListIdxByItemID(T_ItemID itemID)
{
	INT32 i = 0;
	for (i = 0; i < (INT32)itemList_primary.size(); i++)
	{
		if (itemID == itemList_primary[i]->itemID) break;
	}

	return i;
}

INT32 ObjSpec_BattleUseItem::_GetSecondaryListIdxByItemID(T_ItemID itemID)
{
	INT32 i = 0;
	for (i = 0; i < (INT32)itemList_secondary.size(); i++)
	{
		if (itemID == itemList_secondary[i]->itemID) break;
	}

	return i;
}

INT32 ObjSpec_BattleUseItem::_GetItemListIdxByItemID(T_ItemID itemID)
{
	INT32 i = 0;
	for (i = 0; i < (INT32)itemList_item.size(); i++)
	{
		if (itemID == itemList_item[i]->itemID) break;
	}

	return i;
}

void ObjSpec_BattleUseItem::DoDataExchange(CDataExchange* pDX)
{
	i3GameObjPropertyDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ED_STTIME, m_ED_StartTime);
	DDX_Control(pDX, IDC_ED_RESTIME, m_ED_RespawnTime);

	// 우선 하드 코딩으로 잡아 두어야 합니다.
	// Add 시점을 어떻게 해야 할까 생각해봐야 합니다...
	DDX_Control(pDX, IDC_TYPECOMBO1, m_CB_ItemType[0]);
	DDX_Control(pDX, IDC_TYPECOMBO2, m_CB_ItemType[1]);
	DDX_Control(pDX, IDC_TYPECOMBO3, m_CB_ItemType[2]);
	DDX_Control(pDX, IDC_TYPECOMBO4, m_CB_ItemType[3]);
	DDX_Control(pDX, IDC_TYPECOMBO5, m_CB_ItemType[4]);

	DDX_Control(pDX, IDC_CONTENT1, m_CB_ItemName[0]);
	DDX_Control(pDX, IDC_CONTENT2, m_CB_ItemName[1]);
	DDX_Control(pDX, IDC_CONTENT3, m_CB_ItemName[2]);
	DDX_Control(pDX, IDC_CONTENT4, m_CB_ItemName[3]);
	DDX_Control(pDX, IDC_CONTENT5, m_CB_ItemName[4]);

	DDX_Control(pDX, IDC_ED_PROB1, m_ED_ItemProbability[0]);
	DDX_Control(pDX, IDC_ED_PROB2, m_ED_ItemProbability[1]);
	DDX_Control(pDX, IDC_ED_PROB3, m_ED_ItemProbability[2]);
	DDX_Control(pDX, IDC_ED_PROB4, m_ED_ItemProbability[3]);
	DDX_Control(pDX, IDC_ED_PROB5, m_ED_ItemProbability[4]);

	DDX_Control(pDX, IDC_Prob, m_ED_ProbText);
}


BEGIN_MESSAGE_MAP(ObjSpec_BattleUseItem, i3GameObjPropertyDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_EN_KILLFOCUS(IDC_ED_STTIME, OnEnKillfocusEdSttime)
	ON_EN_KILLFOCUS(IDC_ED_RESTIME, OnEnKillfocusEdRestime)
	ON_EN_KILLFOCUS(IDC_ED_PROB1, OnEnKillfocusEdProb)
	ON_EN_KILLFOCUS(IDC_ED_PROB2, OnEnKillfocusEdProb)
	ON_EN_KILLFOCUS(IDC_ED_PROB3, OnEnKillfocusEdProb)
	ON_EN_KILLFOCUS(IDC_ED_PROB4, OnEnKillfocusEdProb)
	ON_EN_KILLFOCUS(IDC_ED_PROB5, OnEnKillfocusEdProb)
	
	// 어떻게 5개를 하드코딩해야 하는가...
	ON_CBN_SELCHANGE_EX(IDC_TYPECOMBO1, OnEnKillfocusCBType)
	ON_CBN_SELCHANGE_EX(IDC_TYPECOMBO2, OnEnKillfocusCBType)
	ON_CBN_SELCHANGE_EX(IDC_TYPECOMBO3, OnEnKillfocusCBType)
	ON_CBN_SELCHANGE_EX(IDC_TYPECOMBO4, OnEnKillfocusCBType)
	ON_CBN_SELCHANGE_EX(IDC_TYPECOMBO5, OnEnKillfocusCBType)

	ON_CBN_SELCHANGE_EX(IDC_CONTENT1, OnEnKillfocusCBName)
	ON_CBN_SELCHANGE_EX(IDC_CONTENT2, OnEnKillfocusCBName)
	ON_CBN_SELCHANGE_EX(IDC_CONTENT3, OnEnKillfocusCBName)
	ON_CBN_SELCHANGE_EX(IDC_CONTENT4, OnEnKillfocusCBName)
	ON_CBN_SELCHANGE_EX(IDC_CONTENT5, OnEnKillfocusCBName)

	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// ObjSpec_BattleUseItem 메시지 처리기입니다.

BOOL ObjSpec_BattleUseItem::OnInitDialog()
{
	i3GameObjPropertyDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void ObjSpec_BattleUseItem::OnSize(UINT nType, int cx, int cy)
{
	i3GameObjPropertyDialog::OnSize(nType, cx, cy);
}

int ObjSpec_BattleUseItem::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3GameObjPropertyDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void ObjSpec_BattleUseItem::OnEnKillfocusEdSttime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	i3LevelElement3D * pElm = (i3LevelElement3D *)getObject();
	if (pElm == NULL)
		return;

	CGameObjectBattleUseItem * pItemBox = (CGameObjectBattleUseItem *)pElm->CreateGameInstance();

	char conv[128];
	if (m_ED_StartTime.GetModify())
	{
		m_ED_StartTime.GetWindowText(conv, sizeof(conv) - 1);

		pItemBox->SetStartTime((REAL32)atof(conv));

		m_ED_StartTime.SetModify(FALSE);
	}

	I3_MUST_RELEASE(pItemBox);
}

void ObjSpec_BattleUseItem::OnEnKillfocusEdRestime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	i3LevelElement3D * pElm = (i3LevelElement3D *)getObject();
	if (pElm == NULL)
		return;

	CGameObjectBattleUseItem * pItemBox = (CGameObjectBattleUseItem *)pElm->CreateGameInstance();

	char conv[128];
	if (m_ED_RespawnTime.GetModify())
	{
		m_ED_RespawnTime.GetWindowText(conv, sizeof(conv) - 1);

		pItemBox->SetRespawnTime((REAL32)atof(conv));

		m_ED_RespawnTime.SetModify(FALSE);
	}

	I3_MUST_RELEASE(pItemBox);
}

void ObjSpec_BattleUseItem::OnEnKillfocusEdProb()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 확률을 계산해서 Text로 출력.
	i3LevelElement3D * pElm = (i3LevelElement3D *)getObject();
	if (pElm == NULL)
		return;

	CGameObjectBattleUseItem * pItemBox = (CGameObjectBattleUseItem *)pElm->CreateGameInstance();

	_SumProbability(pItemBox);

	I3_MUST_RELEASE(pItemBox);
}

void ObjSpec_BattleUseItem::OnEnKillfocusCBType(UINT32 id)
{
	// 여기에서 Name들을 골라내야 합니다.

	CComboBox * pComboType = NULL;
	INT32 i = 0;
	for (i = 0; i < MAX_ITEM_COUNT; i++)
	{
		if (id == (UINT32)m_CB_ItemType[i].GetDlgCtrlID())
		{
			pComboType = &m_CB_ItemType[i];
			break;
		}
	}

	if (pComboType != NULL)
	{
		CComboBox * pComboName = &m_CB_ItemName[i];
		if (pComboName != NULL)
		{
			_ComboBoxClear(pComboName);
			switch (pComboType->GetCurSel())
			{
			case 0:
				{
					_CBAddPrimaryString(pComboName);
				}break;
			case 1:
				{
					_CBAddSecondaryString(pComboName);
				}break;
			case 2:
				{
					_CBAddItemString(pComboName);
				}break;

			default:
				break;
			}
		}
	}
}

void ObjSpec_BattleUseItem::OnEnKillfocusCBName(UINT32 id)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	i3LevelElement3D * pElm = (i3LevelElement3D *)getObject();
	if (pElm == NULL)
		return;

	if (m_bItemListUp == false) return;

	CGameObjectBattleUseItem * pItemBox = (CGameObjectBattleUseItem *)pElm->CreateGameInstance();

	CComboBox * pComboName = NULL;
	INT32 i = 0;
	for (i = 0; i < MAX_ITEM_COUNT; i++)
	{
		if (id == (UINT32)m_CB_ItemName[i].GetDlgCtrlID())
		{
			pComboName = &m_CB_ItemName[i];
			break;
		}
	}

	if (pComboName != NULL)
	{
		CComboBox * pComboType = &m_CB_ItemType[i];
		if (pComboType != NULL)
		{
			pComboType->GetCurSel();
			switch (pComboType->GetCurSel())
			{
			case 0:
			{
				pItemBox->SetItemID(i, _GetPrimaryItemIDbyIdx(pComboName->GetCurSel()));
			}break;
			case 1:
			{
				pItemBox->SetItemID(i, _GetSecondaryItemIDbyIdx(pComboName->GetCurSel()));
			}break;
			case 2:
			{
				pItemBox->SetItemID(i, _GetItemItemIDbyIdx(pComboName->GetCurSel()));
			}break;

			default:
				break;
			}
		}
	}


	I3_MUST_RELEASE(pItemBox);
}

void ObjSpec_BattleUseItem::OnOK()
{
	OnEnKillfocusEdSttime();
	OnEnKillfocusEdRestime();
	OnEnKillfocusEdProb();
}

void ObjSpec_BattleUseItem::OnCancel()
{
}

void ObjSpec_BattleUseItem::OnBnClickedButton1()
{
	// DB를 만들고 붙입니다.
	if (m_bItemListUp == true) return;

	CADODatabase * mDB = new CADODatabase;

	// DB 연결
	if (FALSE == mDB->Connect("192.168.2.161,17002", "PBOption", "pb", "wpvpxh12#$"))
	{
		return;
	}

	char strTemp[256];
	strTemp[0] = '\0';

	// Query 실행
	strcpy_s(strTemp, 256, "SELECT ItemID, Name FROM PB_BT_SShopItem ORDER BY Name");
	if (FALSE == mDB->ExecuteSP(strTemp))
	{
		return;
	}

	// 데이터 입력
	INT32 count = 0;
	while (FALSE == mDB->IsEOF())
	{
		if (_isListUpType( GET_ITEM_TYPE( mDB->GetINT32Data(0) ) ) )
		{
			ITEMLIST * list = new ITEMLIST;
			list->itemID = mDB->GetINT32Data(0);

			list->itemName[0] = '\0';
			mDB->GetTextData(1, list->itemName, 256);

			// 리스트 추가
			switch (GET_ITEM_TYPE(list->itemID))
			{
			case ITEM_TYPE_PRIMARY:
				itemList_primary.push_back(list);
				break;
			case ITEM_TYPE_SECONDARY:
				itemList_secondary.push_back(list);
				break;
			case ITEM_TYPE_WRAP_COUNT:
				itemList_item.push_back(list);
				break;
			}
			count++;
		}
		mDB->MoveNext();
	}

	delete mDB;
	m_bItemListUp = true;

	_SetType();
}

#endif