#include "StdAfx.h"
#include "Equip_Pane.h"
#include "LiveManager.h"
#include "GlobalInstance.h"
#include "ExecuteSVN.h"
#include "PackScriptContents.h"
IMPLEMENT_DYNAMIC(CEquip_Pane, CDockablePane)


BEGIN_MESSAGE_MAP(CEquip_Pane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

CEquip_Pane::CEquip_Pane(void)
{
/*	 = i3RegistrySet_Impl::new_object();*/
}

CEquip_Pane::~CEquip_Pane(void)
{
	//delete m_pEquipMent;
	i3::vector<BYTE> buff;
	i3::string strCmd;
	strCmd = "svn.exe unlock ";
	strCmd += m_sv1PefFullPath;
	//svn::LoadDataFromPipeSync(strCmd, buff);
}
void CEquip_Pane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}


void CEquip_Pane::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top+20, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CEquip_Pane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

 	CRect rectDummy;
 
 	if (!m_wndObjectCombo.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rectDummy, this, 111))
 	{
		TRACE0("속성 콤보 상자를 만들지 못했습니다. \n");
 		return -1;      // 만들지 못했습니다.
 	}
 
 	m_wndObjectCombo.SetFont(CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT)));
 	m_wndObjectCombo.SetCurSel(0);
 
 	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 22))
 	{
 		TRACE0("속성 표를 만들지 못했습니다. \n");
 		return -1;      // 만들지 못했습니다.
 	}

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	AdjustLayout();

	return 0;
}

bool CEquip_Pane::LoadRegistry()
{
	if( m_v1Registry.GetNationCount() <= 0 )
	{
		char str[MAX_PATH] = {0};
		sprintf_s(str, MAX_PATH, "%s\\Config\\Equipment.pef", theApp.GetV1MediaPath());

		m_sv1PefFullPath = str;

		m_v1Registry.SetCodePage( I3_LANG_CODE_ANSI );
		UINT32	Rv = m_v1Registry.LoadRegistrySet(str, REGISTRY_FILE_BINARY);

		i3::vector<BYTE> buff;
		i3::string strCmd;
		strCmd = "svn.exe lock ";
		strCmd += str;
// 		if( !svn::LoadDataFromPipeSync(strCmd, buff) )
// 		{
// 			MessageBox(L"Lock걸기 실패", L"Svn");
// 			return false;
// 		}
// 		else
// 		{
// 			i3::string rng = i3::string(reinterpret_cast<char*>(buff.begin()), 
// 				reinterpret_cast<char*>(buff.end()));
// 
// 			CGlobalInstance::getSingleton().cprintf(rng.c_str());
// 		}


		sprintf_s(str, MAX_PATH, "%s\\Config\\Nation.nlf", theApp.GetV1MediaPath());
		m_v1Registry.LoadNationFile(CString(str));

		i3RegKey *pChild = (i3RegKey *)i3TreeNode::FindNodeByName( m_v1Registry.getRoot(), "Equipment");
		for(int i = 0; i<pChild->getChildCount(); i++)
		{	
			m_wndObjectCombo.AddString(CString(pChild->getChild(i)->GetName()));
		}

		m_wndObjectCombo.SetCurSel(0);

		
	}

	return true;
}

void CEquip_Pane::Update()
{
	LoadRegistry();

	//combobox 업데이트
	INT32 nCurSel = m_wndObjectCombo.GetCurSel();
	if( m_nComboBoxCurSel != nCurSel)
	{
		CString sStringName;
		m_wndObjectCombo.GetLBText(nCurSel, sStringName);

		if( !m_wndObjectCombo.GetDroppedState() )
		{
			//장비정보 콤보박스형태로 등록
			if( sStringName != "" )
			{	
				AddEquipList(sStringName);
				m_nComboBoxCurSel = nCurSel;
			}
		}
	}
}

void CEquip_Pane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();

	CGlobalInstance::getSingleton().m_CurFocusWindow = this;
}

void CEquip_Pane::AddEquipList(CString sAttribute)
{
	m_wndPropList.RemoveAll();

	CMFCPropertyGridProperty* pBaseList = new CMFCPropertyGridProperty(_T("장비선택"));
	CMFCPropertyGridProperty* pBaseProperty = new CMFCPropertyGridProperty(_T("BaseEquip"), _T("장비를를 선택하세요"), _T(""));

	i3RegKey* pKey = m_v1Registry.getRoot();

	pBaseProperty->SetData(ID_PROPERTY);

	i3RegKey *pChild = (i3RegKey *)i3TreeNode::FindNodeByName( m_v1Registry.getRoot(), "Equipment");

	if( pChild )
	{
		char chName[256];
		ZeroMemory(chName, 256);
		CGlobalInstance::getSingleton().StringToChar(sAttribute, chName, 256);

		pChild = (i3RegKey *)i3TreeNode::FindNodeByName( pChild, chName);

		for(INT32 i =0; i<pChild->getChildCount(); i++)
		{
			i3RegKey * ptempChild = (i3RegKey*)pChild->getChild(i);
			pBaseProperty->AddOption(CString(ptempChild->GetName()));
		}

		pBaseProperty->AllowEdit(FALSE);
		pBaseList->AddSubItem(pBaseProperty);
	}

	m_wndPropList.AddProperty(pBaseList);
}

LRESULT CEquip_Pane::OnPropertyChanged(WPARAM wparam, LPARAM lparam)
{
	CMFCPropertyGridProperty *pProp = (CMFCPropertyGridProperty*)lparam;
	if (!pProp) return 0;

	switch(pProp->GetData())
	{
	case ID_PROPERTY:
		{
			AddBaseInfo(pProp);
		}
		break;

	}

	return S_OK;
}

void CEquip_Pane::AddBaseInfo(CMFCPropertyGridProperty *pProp)
{
	char chbuffer[4092] = {0,};

	CString val = GetPropertyValue(pProp);

	i3RegKey* pKey = m_v1Registry.getRoot();
	char chValue[256] = {0,};
	strcpy(chValue,CT2A(val));

	CMFCPropertyGridProperty* pBaseProperty = m_wndPropList.FindItemByData(ID_PROPERTY_BASEINFO);
	if( pBaseProperty )
	{
		m_wndPropList.DeleteProperty(pBaseProperty);
	}

	CMFCPropertyGridProperty* pChangeProperty = m_wndPropList.FindItemByData(ID_PROPERTY_NEWINFO);
	if( pChangeProperty )
	{
		m_wndPropList.DeleteProperty(pChangeProperty);
	}

	pKey = (i3RegKey *)i3TreeNode::FindNodeByName( m_v1Registry.getRoot(), chValue);	//아이템 key얻기

	m_sBaseItemName = chValue;

	if( pKey )
	{
		pChangeProperty = new CMFCPropertyGridProperty(_T("NewItem"));
		pChangeProperty->SetData(ID_PROPERTY_NEWINFO);
		m_wndPropList.AddProperty(pChangeProperty);

		pBaseProperty = new CMFCPropertyGridProperty(_T("BaseItem"));
		pBaseProperty->SetData(ID_PROPERTY_BASEINFO);

		
		pChangeProperty->AddSubItem(new CPropertyGridPropertyWeapon(L"Item Name", (_variant_t) CString("")));
		

		i3RegArray* pRegArray = NULL;


		//아이템이 가지고있는 Base필드 등록
		m_vecCheckBoxProperty.clear();


		AddBaseInfoToProperty(pKey, pBaseProperty);
		m_wndPropList.AddProperty(pBaseProperty);

		for( INT32 j = 0; j < m_nBaseChildCount+1; j++)
		{
			CMFCPropertyGridProperty* pchild = m_wndPropList.FindItemByData(ID_PROPERTY_BASECHILDINFO + j);
			if( pchild )
				m_wndPropList.DeleteProperty(pchild);
		}

		//자식 노드가 가진 녀석들도 등록
		for( INT32 j = 0; j < pKey->getChildCount(); j++)
		{
			i3RegKey* pChildKey = (i3RegKey*)pKey->getChild(j);
			CMFCPropertyGridProperty* pChildProperty = new CMFCPropertyGridProperty(CString(pChildKey->GetName()));
			pChildProperty->SetData(ID_PROPERTY_BASECHILDINFO+j);
			AddBaseInfoToProperty(pChildKey, pChildProperty);
			m_wndPropList.AddProperty(pChildProperty);
			m_nBaseChildCount = j;
		}

	
	}
}

BOOL CEquip_Pane::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->message == WM_LBUTTONDOWN )
	{
		CGlobalInstance::getSingleton().m_CurFocusWindow = this;
	}

	return CDockablePane::PreTranslateMessage(pMsg);
}

bool CEquip_Pane::AddRegistry()
{
	char chName[256];
	ZeroMemory(chName, 256);

	if( CGlobalInstance::getSingleton().IsSave() ) 
	{
		CGlobalInstance::getSingleton().cprintf("pef저장중입니다 잠시후에 시도하세요");
		return false;
	}

	CMFCPropertyGridProperty* pChangeProperty = m_wndPropList.FindItemByData(ID_PROPERTY_NEWINFO);
	if( !pChangeProperty ) return false;;

	m_wndPropList.UpdateData(FALSE);

	CString sAttribute;
	char chValue[256];

	m_wndObjectCombo.GetLBText(m_wndObjectCombo.GetCurSel(), sAttribute);
	CGlobalInstance::getSingleton().StringToChar(sAttribute, chValue, 256);

	i3RegKey *pKey = (i3RegKey *)i3TreeNode::FindNodeByName( m_v1Registry.getRoot(), chValue);	//아이템 key얻기
	pKey = (i3RegKey *)i3TreeNode::FindNodeByName(pKey, (char*)m_sBaseItemName.c_str());

	if( !pKey ) return false;

	i3RegKey * pNew = i3RegKey::new_object();
	pKey->CopyTo(pNew, I3_COPY_REF);

	//자식노드들 복사
	for(int i = 0; i<pKey->getChildCount(); i++)
	{
		i3RegKey* pNode = (i3RegKey*)pKey->getChild(i);
		i3RegKey * pNewNode = i3RegKey::new_object();
		pNode->CopyTo(pNewNode, I3_COPY_REF);
		pNew->AddChild(pNewNode);
	}

	for(int i = 0; i< pChangeProperty->GetSubItemsCount(); i++)
	{
		if( pChangeProperty->GetSubItem(i)->IsInPlaceEditing() )
			pChangeProperty->GetSubItem(i)->OnUpdateValue();
	}

	CString val = GetPropertyValue(pChangeProperty->GetSubItem(0));
	CGlobalInstance::getSingleton().StringToChar(val, chName, 256);

	//같은 이름으로 중복 등록하지 않도록 하긔
	i3RegKey *tempkey = (i3RegKey *)i3TreeNode::FindNodeByName( m_v1Registry.getRoot(), chValue);
	tempkey = (i3RegKey *)i3TreeNode::FindNodeByName(tempkey, chName);
	if( tempkey )	
	{
		MessageBox(L"중복되는 아이템 이름이 존재합니다", L"추가실패");
		return false;
	}

	pNew->SetName( i3::rc_string(chName) );

	ITEMSUBINFO info;

	AddKey(pNew, chName, UPDATEFIELD_TYPE_NONE, &info);

	char szWeaponName[MAX_PATH] = ""; 
	i3::safe_string_copy( szWeaponName, (char*)m_sBaseItemName.c_str(), MAX_PATH );
	info.sItemName = CString(pNew->GetName());
	info.sBaseItemName = CString(szWeaponName);

	CString sValue=L"";
	CMFCPropertyGridProperty* pProperty = GetChildPropertyName(pChangeProperty, "", "ResPostName");
	if( pProperty )
	{
		sValue = GetPropertyValue(pProperty);
	}

	info.sNewResName = sValue;
	info.sItemListName = pKey->getParent()->GetName();

	//무기추가 list에 등록
	if( !CGlobalInstance::getSingleton().GetItemListPane()->AddWeapon(info) )
	{
		pNew->Release();
		return false;
	}

	//WeaponList에 추가
	i3RegKey * pParent = (i3RegKey*)pKey->getParent();
	pParent->AddChild( pNew);
	return true;
}

std::string CEquip_Pane::AddKey(i3RegKey * pNew, char* pchKeyName, UPDATAFIELD type, ITEMSUBINFO* ITEMSUBINFO)
{
	std::string sitemlistname = "";

	CMFCPropertyGridProperty* pChangeProperty = m_wndPropList.FindItemByData(ID_PROPERTY_NEWINFO);
	if( pChangeProperty )
	{
		i3RegArray* pRegArray = NULL;
		char chbuffer[256] = {0,};

		for(int i = 0;  i < pChangeProperty->GetSubItemsCount(); i++)
		{
			CMFCPropertyGridProperty* pProperty = pChangeProperty->GetSubItem(i);

			FIELDCHANGEINFO fieldInfo;
			fieldInfo.sFieldName = pProperty->GetName();
			fieldInfo.sValue = pProperty->GetValue();

			ITEMSUBINFO->vecFieldInfo.push_back(fieldInfo);
			if( type == UPDATEFIELD_TYPE_NONE )
			{
				AdjustRegistry(pNew, pProperty);

				for(INT32 j = 0; j < pNew->getChildCount(); j++)
				{
					i3RegKey* pChildKey = (i3RegKey*)pNew->getChild(j);
					AdjustRegistry(pChildKey, pProperty, true);
				}
			}
		}
	}
	return sitemlistname;
}

void CEquip_Pane::Save()
{
	if( CGlobalInstance::getSingleton().IsSave() )
	{
		if( CGlobalInstance::getSingleton().GetItemListPane()->GetEquipListSize() > 0 )
		{
			CGlobalInstance::getSingleton().SetPrograssBar(0);
			SaveRegistry();
			
			/*CGlobalInstance::getSingleton().AddStatusBarMsg("CharaExport...");
			for(INT32 i = 0; i < CGlobalInstance::getSingleton().GetItemListPane()->GetEquipListSize(); i++)
			{
				ITEMSUBINFO info = CGlobalInstance::getSingleton().GetItemListPane()->GetItemListInfo(i, ID_EQUIP_PANE);
				if( info.sItemListName == "Mask" )
				{
					SaveCharaEditor(info);
					SaveRSC(info);
				}
			}*/
			
			CGlobalInstance::getSingleton().SetPrograssBar(100);

			//CGlobalInstance::getSingleton().GetItemListPane()->ClearSubinfo(ID_EQUIP_PANE);
			CGlobalInstance::getSingleton().AddStatusBarMsg("Pef등록완료");
		}

		CGlobalInstance::getSingleton().SetSave(false);
	}
}

void CEquip_Pane::DeleteRegistry()
{
	if( !CGlobalInstance::getSingleton().IsSave() )
	{
		ITEMSUBINFO* Iteminfo = CGlobalInstance::getSingleton().GetItemListPane()->GetSelectItem();
		if( !Iteminfo ) return;
		if( Iteminfo->sItemName != "")
		{
			char chbuffer[256]={0,};
			CGlobalInstance::getSingleton().StringToChar(Iteminfo->sItemName, chbuffer, 256);

			i3RegKey *tempkey = (i3RegKey *)i3TreeNode::FindNodeByName( m_v1Registry.getRoot(), "WeaponList");
			i3RegKey *ItemKey = (i3RegKey *)i3TreeNode::FindNodeByName(tempkey, chbuffer);
			if( ItemKey )
			{
				ItemKey->getParent()->RemoveChild(ItemKey);
			}

		}
		CGlobalInstance::getSingleton().GetItemListPane()->DeleteWeapon();
	}
	else
	{
		CGlobalInstance::getSingleton().cprintf("pef저장중입니다 잠시후에 시도하세요");
	}
}

void CEquip_Pane::SaveCharaEditor(ITEMSUBINFO info)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	wchar_t wcstr[512]={0,};
	std::string sMaskArgument = "-GenerateMask ";
	sMaskArgument += theApp.GetV1MediaPath();	//Media 경로
	sMaskArgument += " ";
	for(size_t i=0; i<info.vecFieldInfo.size(); i++)
	{
		if( info.vecFieldInfo[i].sFieldName == "[NewItem] ResLocalPath")
		{
			char chName[256];
			CString temp = info.vecFieldInfo[i].sValue;
			temp.Delete(0, info.vecFieldInfo[i].sValue.Find(L"/")+1);
			CGlobalInstance::getSingleton().StringToChar(temp, chName, 256);
			sMaskArgument += chName;	//추출할 폴더 경로
			break;
		}
	}
	
	mbstowcs(wcstr, (char*)sMaskArgument.c_str(), sMaskArgument.size()+1);
	wchar_t wcCurDir[256];
	GetModuleFileName(NULL, wcCurDir, 256);

	CString path = wcCurDir;
	if (0 < path.ReverseFind('\\'))
	{
		path = path.Left(path.ReverseFind('\\'));
	}

	wchar_t wcProcessName[512];
	wsprintf(wcProcessName, L"%s\\i3CharaEditor.exe", path);
	//CGlobalInstance::getSingleton().cprintf("i3CharaEditor backgound진행\n");
	CreateProcess(wcProcessName, wcstr, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	//createprocess한게 종료되면 다음으로 넘어간다
	WaitForSingleObject( pi.hProcess, INFINITE );

	char chLog[512];
	CGlobalInstance::getSingleton().StringToChar(info.sItemName, chLog, 512);
	CGlobalInstance::getSingleton().cprintf("i3CharaEditor.. .%s와 연관된 i3chr&.i3i&.I3CharaEditor Export", chLog);
}

void CEquip_Pane::SaveRSC(ITEMSUBINFO info)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	char szFileName[256]={0,};
	CString sRSCArgument = L"-SaveRSC ";
	sRSCArgument += theApp.GetV1MediaPath();
	sRSCArgument += " ";
	sRSCArgument += "Equip/Equip.RSG ";

	for(size_t i=0; i<info.vecFieldInfo.size(); i++)
	{
		if( info.vecFieldInfo[i].sFieldName == "[NewItem] ResLocalPath")
		{
			sRSCArgument += CString(theApp.GetV1MediaPath()) + CString("/") + CString("Equip/") + info.vecFieldInfo[i].sValue;	
			break;
		}
	}
	sRSCArgument += " *.i3chr";
	//i3::change_file_ext_copy(szFileName, temp.c_str(), L"I3CHR");

	wchar_t wcstr[512] = {0,};
	char chArgument[512];
	CGlobalInstance::getSingleton().StringToChar(sRSCArgument, chArgument, 512);
	mbstowcs(wcstr, chArgument, strlen(chArgument));
	wchar_t wcCurDir[256];
	GetModuleFileName(NULL, wcCurDir, 256);

	CString path = wcCurDir;
	if (0 < path.ReverseFind('\\'))
	{
		path = path.Left(path.ReverseFind('\\'));
	}

	wchar_t wcProcessName[512];
	wsprintf(wcProcessName, L"%s\\i3RSCGen.exe", path);
	CreateProcess(wcProcessName, wcstr, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	char chLog[512];
	CGlobalInstance::getSingleton().StringToChar(info.sItemName, chLog, 512);
	CGlobalInstance::getSingleton().cprintf("i3RSCGen.. %s와 연관된 i3Chr 저장완료", chLog);

	sRSCArgument = L"-SaveRSC ";
	sRSCArgument += theApp.GetV1MediaPath();
	sRSCArgument += " ";
	sRSCArgument += "Equip/EquipTexture.RSG ";
	for(size_t i=0; i<info.vecFieldInfo.size(); i++)
	{
		if( info.vecFieldInfo[i].sFieldName == "[NewItem] ResLocalPath")
		{
			sRSCArgument += CString(theApp.GetV1MediaPath()) + CString("/") + CString("Equip/") + info.vecFieldInfo[i].sValue;	
			break;
		}
	}
	sRSCArgument += " *.i3i";
	CGlobalInstance::getSingleton().StringToChar(sRSCArgument, chArgument, 512);
	ZeroMemory(wcstr, sizeof(wcstr));
	mbstowcs(wcstr, chArgument, strlen(chArgument));
	CreateProcess(wcProcessName, wcstr, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	WaitForSingleObject( pi.hProcess, INFINITE );

	CGlobalInstance::getSingleton().StringToChar(info.sItemName, chLog, 512);
	CGlobalInstance::getSingleton().cprintf("i3RSCGen.. %s와 연관된 i3i 저장완료", chLog);
}

void CEquip_Pane::_CopyRight(char *chbuffer, size_t sizeOfString, i3RegArray* pRegArray)
{
	for(int i = 0; i < m_v1Registry.GetNationCount(); i++)
	{
		CGlobalInstance::getSingleton()._String2Convert(chbuffer, sizeOfString, (*pRegArray)[i]);
	}
}
void CEquip_Pane::SaveRegistry()
{
	CGlobalInstance::getSingleton().AddStatusBarMsg("V1 Equipment 저장중...");
	CGlobalInstance::getSingleton().cprintf("V1 Equip.pef 저장시작");
	CGlobalInstance::getSingleton().SetPrograssBar(10);

	char szFileName[MAX_PATH] = ""; 
	i3::safe_string_copy( szFileName, (char*)m_sv1PefFullPath.c_str(), MAX_PATH );
	m_v1Registry.SetCodePage( I3_LANG_CODE_ANSI);

	if( m_v1Registry.SaveRegistrySet( szFileName, REGISTRY_FILE_BINARY ) == STREAM_ERR)
	{
		MessageBox(L"V1 Equip.pef 저장실패", L"pef");
	}
	else
	{
		CGlobalInstance::getSingleton().cprintf("V1 Equip.pef 저장성공 : %s\n", szFileName);
	}

	CGlobalInstance::getSingleton().SetPrograssBar(20);

	i3::change_file_ext_copy(szFileName, szFileName, "i3RegXML");

	CGlobalInstance::getSingleton().AddStatusBarMsg("V1 Equip.xml 저장중...");

	//	i3RegXML로 저장
	if( m_v1Registry.SaveRegistrySet( szFileName, REGISTRY_FILE_XML_BY_UTF8 ) == STREAM_ERR)
	{
		MessageBox(L"V1 Equip.xml 저장실패", L"i3RegXML");
	}
	else
	{
		CGlobalInstance::getSingleton().cprintf("V1 Equip.xml 저장성공 : %s\n", szFileName);
	}

	CGlobalInstance::getSingleton().SetPrograssBar(40);
	CGlobalInstance::getSingleton().cprintf("V1 Equipment 저장완료");
}

void CEquip_Pane::ChangeField(CPropertyGridCheckBox* pProperty, i3RegData *pData)
{
	if( strcmp( pData->GetName(), "_Type" ) == 0 
		||strcmp( pData->GetName(), "ResLocalPath" ) == 0
		||strcmp( pData->GetName(), "ResPostName" ) == 0 
		||strcmp( pData->GetName(), "UIShapeIndex" ) == 0
		||strcmp( pData->GetName(), "DiffuseTexture" ) == 0
		||strcmp( pData->GetName(), "NormalTexture" ) == 0)
	{
		pProperty->OnEdit(NULL);
	}
}