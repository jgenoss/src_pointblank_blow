// CPanel_Part.cpp : implementation file
//

#include "pch.h"
#include "AvatarViewer.h"
#include "Panel_Part.h"
#include "GlobalVar.h"
#include "MainFrm.h"

#include "AvatarDefine.h"

#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/substr.h"

#define CSZ_PROP_PART					("Part")
#define CSZ_PROP_PART_MASK				("Character")
#define CSZ_PROP_PART_NAME				("Name")
#define CSZ_PROP_PART_TYPE				("Type")
#define CSZ_PROP_DESCR_PART_MASK		("Character")
#define CSZ_PROP_DESCR_PART_NAME		("Part Name")
#define CSZ_PROP_DESCR_PART_TYPE		("Equip Type")
#define CSZ_PROP_TRANSFORM				("Transform")
#define CSZ_PROP_SCALE					("Scale")
#define CSZ_PROP_TRANS_X				("Translation X")
#define CSZ_PROP_TRANS_Y				("Translation Y")
#define CSZ_PROP_TRANS_Z				("Translation Z")
#define CSZ_PROP_DESCR_SCALE			("Scale Value")
#define CSZ_PROP_DESCR_TRANS_X			("Translation X Value")
#define CSZ_PROP_DESCR_TRANS_Y			("Translation Y Value")
#define CSZ_PROP_DESCR_TRANS_Z			("Translation Z Value")
#define CSZ_PROP_DEFAULT				("none")
#define CSZ_PROP_HEADTYPE				("HeadType")
#define CSZ_PROP_DESC_HEADTYPE			("0=Normal,1=Half,2=Open,3=Nothing")
#define CSZ_PROP_ETC					("ETC")

// CPanel_Part

IMPLEMENT_DYNAMIC(CPanel_Part, CDockablePane)

CPanel_Part::CPanel_Part()
{

}

CPanel_Part::~CPanel_Part()
{
}


BEGIN_MESSAGE_MAP(CPanel_Part, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_UPDATE_COMMAND_UI(3, OnUpdateButton)
	ON_BN_CLICKED(3,OnBtnClick)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CPanel_Part::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	INT32 nBtnHeight = 30;

	m_btnSave.SetWindowPos(NULL, rectClient.left, rectClient.top , rectClient.Width(), nBtnHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top+nBtnHeight, rectClient.Width(), rectClient.Height()-nBtnHeight, SWP_NOACTIVATE | SWP_NOZORDER);
}


int CPanel_Part::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	if( !m_btnSave.Create( "Save", WS_VISIBLE | WS_CHILD, rectDummy, this, 3 ) )
	{
		TRACE0("버튼을 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	Init();
	InitPropList();
	AdjustLayout();

	m_bLoad = m_EquipData.Load();

	return 0;
}

void CPanel_Part::OnDestroy()
{
// 	m_EquipData.Save();
// 	m_EquipData.Save(false);
	CDockablePane::OnDestroy();
}
void CPanel_Part::OnUpdateButton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsLoad()&&IsAdjust());
}
void CPanel_Part::OnBtnClick()
{
	m_EquipData.Save();
	m_EquipData.Save(false);
}

void CPanel_Part::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

LRESULT CPanel_Part::OnPropertyChanged(WPARAM wparam, LPARAM lparam)
{
	CMFCPropertyGridProperty * pProp = (CMFCPropertyGridProperty*)lparam;

	if (pProp == NULL ) return 0;

	if( !IsLoad() ) return 0;

	GetPropItem( pProp );
	
	m_EquipData.SetPart( m_ePart, m_szEquip, m_szChara, m_vTransform, m_HeadType );

	if( g_pFramework )
	{
		CMainFrame* pMainFrm = (CMainFrame*)(AfxGetMainWnd());
		CPanel_Dlg_Avatar * pAvatarDlg = pMainFrm->m_Panel_Avatar.getAvatarDlg();

		char szName[ 260];
		i3::sprintf( szName, "%s.I3S", m_szEquip);
		
		const char * pszFullPath = pAvatarDlg->FindFullPathByName( m_ePart, szName);

		g_pFramework->ChangeHeadPart( m_HeadType, m_ePart, m_vTransform, pszFullPath );
	}

	if( !IsAdjust() )
		m_bAdjust = true;

	return 0;
}


void CPanel_Part::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroupParts = new CMFCPropertyGridProperty(CSZ_PROP_PART);
	CMFCPropertyGridProperty* pGroupTrans = new CMFCPropertyGridProperty(CSZ_PROP_TRANSFORM);

	m_wndPropList.AddProperty(pGroupParts);
	m_wndPropList.AddProperty(pGroupTrans);

	CMFCPropertyGridProperty* pGroupETC = new CMFCPropertyGridProperty(CSZ_PROP_ETC);
	m_wndPropList.AddProperty(pGroupETC);

	CMFCPropertyGridProperty* pProp = NULL;

	// Parts
	pProp = new CMFCPropertyGridProperty(CSZ_PROP_PART_MASK,CSZ_PROP_DEFAULT, CSZ_PROP_DESCR_PART_MASK);

	pProp->Enable(FALSE);
	pGroupParts->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(CSZ_PROP_PART_TYPE,CSZ_PROP_DEFAULT, CSZ_PROP_DESCR_PART_TYPE);
	
	pProp->Enable(FALSE);
	pGroupParts->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(CSZ_PROP_PART_NAME,CSZ_PROP_DEFAULT, CSZ_PROP_DESCR_PART_NAME);
	pProp->Enable(FALSE);
	pGroupParts->AddSubItem(pProp);


	// Transform
	pProp = new CMFCPropertyGridProperty(CSZ_PROP_TRANS_X,(_variant_t) 0.0f, CSZ_PROP_DESCR_TRANS_X);
	pProp->Enable(FALSE);
	pGroupTrans->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(CSZ_PROP_TRANS_Y,(_variant_t) 0.0f, CSZ_PROP_DESCR_TRANS_Y);
	pProp->Enable(FALSE);
	pGroupTrans->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(CSZ_PROP_TRANS_Z,(_variant_t) 0.0f, CSZ_PROP_DESCR_TRANS_Z);
	pProp->Enable(FALSE);
	pGroupTrans->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(CSZ_PROP_SCALE,(_variant_t) 1.0f, CSZ_PROP_DESCR_SCALE);
	pProp->Enable(FALSE);
	pGroupTrans->AddSubItem(pProp);

	// etc
	pProp = new CMFCPropertyGridProperty( CSZ_PROP_HEADTYPE, (_variant_t) "None", CSZ_PROP_DESC_HEADTYPE);
	pProp->AddOption( "None");
	pProp->AddOption( "Normal");
	pProp->AddOption( "Half");
	pProp->AddOption( "Open");
	pProp->AddOption( "Nothing");
	pProp->AllowEdit(FALSE);
	pProp->Enable(FALSE);
	pGroupETC->AddSubItem( pProp);

}


void CPanel_Part::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}

void CPanel_Part::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPanel_Part::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPanel_Part::GetPropItem( CMFCPropertyGridProperty* pProp )
{
	if( NULL == pProp )return;
	CString temp = pProp->GetDescription();
	COleVariant rVariant = pProp->GetValue();	
	if( i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_SCALE ) ) m_vTransform.w  = rVariant.fltVal;
	else if( i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_TRANS_X ) ) m_vTransform.x  = rVariant.fltVal;
	else if( i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_TRANS_Y ) ) m_vTransform.y  = rVariant.fltVal;
	else if( i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_TRANS_Z ) ) m_vTransform.z  = rVariant.fltVal;
	else if( i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESC_HEADTYPE))
	{
		CString val = pProp->FormatProperty();
		if( val == "None")
			m_HeadType = -1;
		else if( val == "Normal")
			m_HeadType = 0;
		else if( val == "Half")
			m_HeadType = 1;
		else if( val == "Open")
			m_HeadType = 2;
		else
			m_HeadType = 3;
	}
}

void CPanel_Part::SetPropItem( CMFCPropertyGridProperty* pProp )
{
	if( NULL == pProp )return;
	CString temp = pProp->GetDescription();
	pProp->Enable(TRUE);
	if(i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_PART_MASK ) ) { pProp->SetValue( (_variant_t)m_szChara );	pProp->Enable(FALSE); }
	else if(i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_PART_NAME ) ) {pProp->SetValue( (_variant_t)AVATAR::getPartName((EQUIP::ePART)m_ePart).c_str() );pProp->Enable(FALSE); }
	else if(i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_PART_TYPE ) ) {pProp->SetValue( (_variant_t)m_szEquip ); pProp->Enable(FALSE);}
	else if(i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_SCALE ) ) pProp->SetValue( (_variant_t)m_vTransform.w );
	else if(i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_TRANS_X ) ) pProp->SetValue( (_variant_t)m_vTransform.x );
	else if(i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_TRANS_Y ) ) pProp->SetValue( (_variant_t)m_vTransform.y );
	else if(i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_TRANS_Z ) ) pProp->SetValue( (_variant_t)m_vTransform.z );
	else if(i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESC_HEADTYPE))
	{
		if( m_HeadType == -1)
		{
			COleVariant val( "None");
			pProp->SetValue( val );
		}
		else if( m_HeadType == 0)
		{
			COleVariant val( "Normal");
			pProp->SetValue( val );
		}
		else if( m_HeadType == 1)
		{
			COleVariant val( "Half");
			pProp->SetValue( val );
		}
		else if( m_HeadType == 2)
		{
			COleVariant val( "Open");
			pProp->SetValue( val );
		}
		else
		{
			COleVariant val( "Nothing");
			pProp->SetValue( val );
		}
	}
}

void CPanel_Part::InitPropItem( CMFCPropertyGridProperty* pProp )
{
	if( NULL == pProp )return;
	CString temp = pProp->GetDescription();
	if(i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_PART_TYPE ) )		{pProp->SetValue( (_variant_t)CSZ_PROP_DEFAULT ); }
	else if(i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_PART_NAME ) ) {pProp->SetValue( (_variant_t)CSZ_PROP_DEFAULT ); }
	else if(i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_PART_MASK ) ) {pProp->SetValue( (_variant_t)CSZ_PROP_DEFAULT ); }
	else if(i3::generic_is_iequal( temp.GetString(), CSZ_PROP_DESCR_SCALE ) ) pProp->SetValue( (_variant_t)m_vTransform.w );
	else if(i3::generic_is_iequal( temp.GetString(),  CSZ_PROP_DESCR_TRANS_X ) ) pProp->SetValue( (_variant_t)m_vTransform.x );
	else if(i3::generic_is_iequal( temp.GetString(),  CSZ_PROP_DESCR_TRANS_Y ) ) pProp->SetValue( (_variant_t)m_vTransform.y );
	else if(i3::generic_is_iequal( temp.GetString(),  CSZ_PROP_DESCR_TRANS_Z ) ) pProp->SetValue( (_variant_t)m_vTransform.z );
	else if(i3::generic_is_iequal( temp.GetString(),  CSZ_PROP_DESC_HEADTYPE ) )
	{
		if( m_HeadType == -1)
		{
			COleVariant val( "None");
			pProp->SetValue( val );
		}
		else if( m_HeadType == 0)
		{
			COleVariant val( "Normal");
			pProp->SetValue( val );
		}
		else if( m_HeadType == 1)
		{
			COleVariant val( "Half");
			pProp->SetValue( val );
		}
		else if( m_HeadType == 2)
		{
			COleVariant val( "Open");
			pProp->SetValue( val );
		}
		else
		{
			COleVariant val( "Nothing");
			pProp->SetValue( val );
		}
	}
	pProp->Enable(FALSE);
}

void CPanel_Part::InitProp()
{
	const char * pszDefault = CSZ_PROP_DEFAULT;
	m_ePart = EQUIP::ePART_UNKNOWN;
	i3::safe_string_copy(m_szEquip , pszDefault, sizeof(m_szEquip) );
	i3::safe_string_copy(m_szChara , pszDefault, sizeof(m_szChara) );
	m_vTransform.x = 0.f;
	m_vTransform.y = 0.f;
	m_vTransform.z = 0.f;
	m_vTransform.w = 1.f;
	m_HeadType = -1;

	int nPropCnt = m_wndPropList.GetPropertyCount();
	for( int nProp = 0 ; nProp < nPropCnt ; ++nProp )
	{
		CMFCPropertyGridProperty * pProp = m_wndPropList.GetProperty( nProp );
		int nSubItemCnt = pProp->GetSubItemsCount();
		for( INT32 nSubItem = 0; nSubItem < nSubItemCnt; ++nSubItem )
		{
			CMFCPropertyGridProperty * pSubItem = pProp->GetSubItem( nSubItem );
			InitPropItem( pSubItem );
		}
	}
}

void CPanel_Part::UpdatePart( EQUIP::ePART ePart, const char * pszPartName, const char * pszFullPath, TEAM_TYPE team )
{
	switch( ePart )
	{
	case EQUIP::ePART_HEADGEAR:	
	case EQUIP::ePART_FACEGEAR:
	case EQUIP::ePART_HOLSTER:
	case EQUIP::ePART_BELT:
	case EQUIP::ePART_SKIN:
		{
			if( !IsLoad() ) return ;
			/*if( ePart != EQUIP::ePART_SKIN && !CheckAvatar() )
				return ;*/

			if( CheckAvatar() )
			{
				m_ePart = ePart;
				SetEquip(pszPartName);
				if( !m_EquipData.GetPart( m_ePart, m_szEquip, m_szChara, m_vTransform, m_HeadType ) )
				{
					m_vTransform.x = 0.f;
					m_vTransform.y = 0.f;
					m_vTransform.z = 0.f;
					m_vTransform.w = 1.f;
					m_EquipData.SetPart( m_ePart, m_szEquip, m_szChara, m_vTransform, m_HeadType );
				}

				UpdateData();
			}

			if( g_pFramework )
			{
				if( g_pFramework->ChangeHeadPart( m_HeadType, m_ePart, m_vTransform, pszFullPath) == false)
				{	// 스킨이지만 로딩이 안된 상태라면 로딩
					if( ePart == EQUIP::ePART_SKIN)
					{
						g_pFramework->LoadPart( pszFullPath, team);
					}
				}
			}
		}
		break;
	
	default:
		g_pFramework->LoadPart( pszFullPath, team);
		InitProp();
		break;
	}	
}


void CPanel_Part::UpdateData()
{
	int nPropCnt = m_wndPropList.GetPropertyCount();
	for( int nProp = 0 ; nProp < nPropCnt ; ++nProp )
	{
		CMFCPropertyGridProperty * pProp = m_wndPropList.GetProperty( nProp );
		int nSubItemCnt = pProp->GetSubItemsCount();
		for( INT32 nSubItem = 0; nSubItem < nSubItemCnt; ++nSubItem )
		{
			CMFCPropertyGridProperty * pSubItem = pProp->GetSubItem( nSubItem );
			if( pSubItem != NULL)
				SetPropItem( pSubItem );
		}
	}
}

void CPanel_Part::Init()
{
	i3::safe_string_copy(m_szChara,CSZ_PROP_DEFAULT, sizeof(m_szChara));
	i3::safe_string_copy(m_szEquip,CSZ_PROP_DEFAULT, sizeof(m_szEquip));
	m_ePart = EQUIP::ePART_UNKNOWN;
	m_vTransform.x = 0.f;
	m_vTransform.y = 0.f;
	m_vTransform.z = 0.f;
	m_vTransform.w = 1.f;
	m_HeadType = -1;
	m_bLoad = false;
	m_bAdjust = false;
}

bool CPanel_Part::CheckAvatar()
{
	if( g_pFramework )
	{
		if( g_pFramework->getAvatar() )
		{
			const char * pszChara = g_pFramework->getAvatar()->getPartName( EQUIP::ePART_CHARA ).c_str();
			char szName[ MAX_PATH];
			i3::safe_string_copy( szName, pszChara, sizeof( szName));

			INT32 pos = i3::contain_string( szName,"_X");
			if( pos != -1)
				szName[pos] = 0;
			pos = i3::contain_string( szName,"_Y");
			if( pos != -1)
				szName[pos] = 0;
			pos = i3::contain_string( szName,"_Z");
			if( pos != -1)
				szName[pos] = 0;

			i3::safe_string_copy( m_szChara, szName, sizeof(m_szChara) );
			return true;
		}
	}
	return false;
}

void CPanel_Part::SetEquip( const char * pszName )
{
	char szTemp[MAX_PATH];
	i3::safe_string_copy( szTemp, pszName, sizeof(szTemp) );
	char szExt[ 16];
	i3::extract_fileext( szTemp, szExt);
	i3::to_upper( szExt);
	if( i3::generic_is_iequal( szExt, "I3S") )
	{
		INT32 pos = i3::contain_string( szTemp,".I3S");

		if (pos != -1)
			i3::substr( m_szEquip, szTemp, 0 ,pos);
		else
			i3::safe_string_copy( m_szEquip, szTemp, sizeof( m_szEquip));
	}
}


void CPanel_Part::LoadEquipData()
{
	InitProp();
	m_bLoad = m_EquipData.Load(false);
	m_bAdjust = false;
}

bool CPanel_Part::IsLoad()
{
	return m_bLoad;
}

bool CPanel_Part::IsAdjust()
{
	return m_bAdjust;
}
