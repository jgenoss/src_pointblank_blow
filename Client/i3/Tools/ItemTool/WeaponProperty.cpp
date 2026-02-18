// WeaponProperty.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "WeaponProperty.h"

#include "i3Base/string/ext/generic_string_ncopy.h"
#include "i3Base/string/ext/extract_directoryname_filename.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/ext/sprintf.h"

std::map<CString, void*> g_mPropListCon;

// cWeaponProperty

IMPLEMENT_DYNCREATE(cWeaponProperty, CDialog)
IMPLEMENT_DYNAMIC(cMyMFCPropertyGridCtrl, CMFCPropertyGridCtrl)
IMPLEMENT_DYNAMIC(cMyMFCPropertyGridProperty, CMFCPropertyGridProperty)

cMyMFCPropertyGridProperty::cMyMFCPropertyGridProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr,
													   DWORD_PTR dwData, LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate,
													   LPCTSTR lpszValidChars)
: CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
{
	g_mPropListCon[strName] = this;
}

cMyMFCPropertyGridProperty::cMyMFCPropertyGridProperty(const CString& strGroupName, DWORD_PTR dwData, BOOL bIsValueList) 
:	CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList)
{
 
}

cMyMFCPropertyGridProperty::~cMyMFCPropertyGridProperty()
{

}

// void cMyMFCPropertyGridCtrl::OnPropertyChanged(CMFCPropertyGridProperty* pProp) const
// {
// 	CString str = pProp->GetName();
// 	
// }

cWeaponProperty::cWeaponProperty()
:	m_pCurrAiState(NULL)
,	m_bDisplayPath(true)
,	m_bOnlyName(true)
,	m_fBlendTime(0.0f)
,	m_bLoop(true)
,	m_bIsAnimationEndtoBlend(true)
,	m_bIsBlendTimeLimit(true)
,	m_fBlendtimeLimit(0.0f)
,	m_nSelectedState(-1)
{

}

cWeaponProperty::~cWeaponProperty()
{
}

void cWeaponProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(cWeaponProperty, CDialog)
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

void cWeaponProperty::Init()
{
	//대분류
	//name
	//animation
	//condition
	//nextstate
	//script
	//source
	//comment
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 콤보 상자를 만듭니다.
	//const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return ;      // 만들지 못했습니다.
	}

	InitPropList();

	AdjustLayout();

	g_HwndWeaponProp = m_hWnd;
}

void cWeaponProperty::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	AdjustLayout();
}

void cWeaponProperty::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	int cyCmb = rectCombo.Size().cy;
	int cyTlb = 0;

	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}
 
 void cWeaponProperty::UpdateAnimList()
 {
 	m_vAnimationList.clear();
 
 	char conv[MAX_PATH];
 //	char	conv2[MAX_PATH];
 	BOOL	bShowPath;
 	INT32 i = 0;
 
 	bShowPath = TRUE;
 
 	{
 		i3Animation * pAnim;
 
 		for( i = 0; i < g_pChara->getAnimCount(); i++)
 		{
 			pAnim = g_pChara->getAnim( i);
 			I3ASSERT( pAnim->hasName());
 
 
 			if( bShowPath)
 			{
 				strcpy( conv, pAnim->GetName());
 			}
 			else
 			{
				//
				// 상대경로의 최하단 디렉토리1개와 파일타이틀명만 수집하는 루틴으로 보인다.
				// 너무 지저분해서, 다시 정리해야 한다.
				//
				i3::stack_string title;
				i3::stack_string dir;

				i3::extract_directoryname_filename(pAnim->GetName(), dir, title);
				i3::extract_filename(dir, dir);		//	// 최하단 디렉토리만 추출..
				i3::remove_ext(title);				//	// 파일명에서 확장자 제거..
				i3::sprintf(conv, "%s\\%s", dir, title );

				/*
 				char * pName = i3String::SplitFileName( pAnim->GetName(), conv, FALSE);
				// pName은 함수호출 리턴값으로는 널값이 불가능한 상태임..
 				if( pName != NULL )
 				{
 					INT32 nLength = i3::generic_string_size( pName );
 					if( nLength > 0 )
 					{
 						char temp[MAX_PATH] = {0,};
 		//				i3::insert_string( temp, 0, (char*)pAnim->GetName(), i3::generic_string_size(pAnim->GetName()) - (nLength+1) );
						i3::generic_string_ncopy( temp, pAnim->GetName(), 
							i3::generic_string_size(pAnim->GetName()) - (nLength+1) );
						
 						i3String::SplitFileName( temp, conv2, FALSE );
 						strcpy( temp, conv );
 						sprintf( conv, "%s\\%s", conv2, temp );
 					}
 				}
				*/
 			}
 
 			m_vAnimationList.push_back(conv);
 		}
 	}
 }

void cWeaponProperty::UpdateAll(int nIdx)
{
	if (nIdx == -1)
		return;

	m_nSelectedState = nIdx;
	UpdateAnimList();

	i3AI* pCurrAI		= g_pChara->getAI(0)->getAI();
	m_pCurrAiState		= pCurrAI->getAIState(nIdx);
	i3Animation* pAnim	= m_pCurrAiState->getAnim();

	m_strFileName	= m_pCurrAiState->GetName();
	
	char conv[256] = "(null)";
	CString strAniName;
	if (pAnim)
	{
		strAniName = pAnim->GetName();
	}

//	i3String::SplitFileName( strAniName, conv, FALSE);
	i3::extract_filetitle( (const char*)strAniName, conv);

	m_strAniName = conv;

	if (pAnim)
		m_strSelectedAnimFileName = pAnim->GetName();
	else
		m_strSelectedAnimFileName = "(null)";

	m_fBlendtimeLimit = m_pCurrAiState->getDuration();

	m_wndPropList.RemoveAll();

	InitPropList();
	AdjustLayout();
}


LRESULT cWeaponProperty::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		UpdateAll(wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


void cWeaponProperty::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	SetPropMain();
	SetPropAnimation();
	SetPropBlending();
	SetPropConditions();
	SetPropNextState();
}

void cWeaponProperty::SetPropMain()
{
	cMyMFCPropertyGridProperty* pGroup = new cMyMFCPropertyGridProperty(_T("Title"));

	cMyMFCPropertyGridProperty* pProp = new cMyMFCPropertyGridProperty(_T("Animation Name"), (_variant_t) m_strAniName, _T("애니메이션의 이름"), eSubAnimName);
	pProp->AllowEdit(FALSE);
	pProp->Enable(FALSE);
	pGroup->AddSubItem(pProp);

	pProp = new cMyMFCPropertyGridProperty(_T("Class Name"), (_variant_t) _T(m_strFileName), _T("파일 이름"), eSubClassName);
	pProp->AllowEdit(FALSE);
	pProp->Enable(FALSE);
	pGroup->AddSubItem(pProp);

	m_wndPropList.AddProperty(pGroup);

}
void cWeaponProperty::SetPropAnimation()
{
	cMyMFCPropertyGridProperty* pGroup = new cMyMFCPropertyGridProperty(_T("Animation"));

	cMyMFCPropertyGridProperty* pProp = NULL;
// 	cMyMFCPropertyGridProperty* pProp = new cMyMFCPropertyGridProperty(_T("Filter"), _T("File Only"), _T("파일 이름을 필터링 합니다."));
// 	pProp->AddOption(_T("Full Path"));
// 	pProp->AddOption(_T("File Only"));
// 	pProp->AllowEdit(FALSE);
// 	pGroup->AddSubItem(pProp);
// 
// 	pProp = new cMyMFCPropertyGridProperty(_T("View Animation Path"), (_variant_t) true, _T("애니메이션 파일의 경로를 표시 여부를 결정합니다"));
// 	pGroup->AddSubItem(pProp);
// 
// 	pProp = new cMyMFCPropertyGridProperty(_T("View Animation Name"), (_variant_t) true, _T("애니메이션의 이름 표시 여부를 결정합니다."));
// 	pGroup->AddSubItem(pProp);

	pProp = new cMyMFCPropertyGridProperty(_T("Anim List"), _T(m_strSelectedAnimFileName), _T("애니메이션을 지정합니다."), eSubAnimList);

	for (size_t i = 0; i < m_vAnimationList.size(); ++i)
	{
		pProp->AddOption(m_vAnimationList[i]);
	}

	pProp->AllowEdit(FALSE);
	pGroup->AddSubItem(pProp);

	char conv[256] = "(null)";
	CString aniName = m_strSelectedAnimFileName;
//	i3String::SplitFileName( aniName, conv, TRUE);
	i3::extract_filename((const char*)aniName, conv);

	pProp = new cMyMFCPropertyGridProperty(_T("Anim File Name"), _T(conv), _T("필터링된 이름이 표시됩니다."), eSubAnimFileName);
	pProp->AllowEdit(FALSE);
	pProp->Enable(FALSE);
	pGroup->AddSubItem(pProp);

	m_wndPropList.AddProperty(pGroup);
}

void cWeaponProperty::SetPropBlending()
{
	cMyMFCPropertyGridProperty* pGroup = new cMyMFCPropertyGridProperty(_T("Blend"));

	cMyMFCPropertyGridProperty* pProp = new cMyMFCPropertyGridProperty(_T("Time"), (_variant_t) m_fBlendTime, _T("블렌딩 시간"), eSubBlendTime);
	pGroup->AddSubItem(pProp);

	pProp = new cMyMFCPropertyGridProperty(_T("Loop"), (_variant_t) true, _T("애니메이션 루프 여부를 결정합니다"), eSubBlendLoop);
	pGroup->AddSubItem(pProp);

	m_wndPropList.AddProperty(pGroup);
}

void cWeaponProperty::SetPropConditions()
{
	cMyMFCPropertyGridProperty* pGroup = new cMyMFCPropertyGridProperty(_T("Condition"));

	cMyMFCPropertyGridProperty* pProp = new cMyMFCPropertyGridProperty(_T("Is Anim End"),
		(_variant_t) false, _T("애니메이션의 종료와 같이 종료 여부를 결정합니다."), eSubIsAnimEnd);
	pGroup->AddSubItem(pProp);

	pProp = new cMyMFCPropertyGridProperty(_T("Is Time Limit"), (_variant_t) true, _T("시간제 여부를 결정합니다."), eSubIsTimeLimit);
	pGroup->AddSubItem(pProp);

	pProp = new cMyMFCPropertyGridProperty(_T("Set Time Limit"), (_variant_t) m_fBlendtimeLimit, _T("시간을 정합니다."), eSubSetTimeLimit);
	pGroup->AddSubItem(pProp);

	m_wndPropList.AddProperty(pGroup);
}

void cWeaponProperty::SetPropNextState()
{
	cMyMFCPropertyGridProperty* pGroup = new cMyMFCPropertyGridProperty(_T("NextState"));
	
	cMyMFCPropertyGridProperty* pProp = new cMyMFCPropertyGridProperty(_T("Animation"), _T(""), _T("다음 처리할 애니메이션을 지정합니다."));
	pProp->AllowEdit(FALSE);

	pGroup->AddSubItem(pProp);

	m_wndPropList.AddProperty(pGroup);
}

// void cWeaponProperty::OnSetFocus(CWnd* pOldWnd)
// {
// 	CDockablePane::OnSetFocus(pOldWnd);
// 	m_wndPropList.SetFocus();
// }
// 
// void cWeaponProperty::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
// {
// 	CDockablePane::OnSettingChange(uFlags, lpszSection);
// 	SetPropListFont();
// }

void cWeaponProperty::SetPropListFont()
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

LRESULT cWeaponProperty::OnPropertyChanged(WPARAM wparam, LPARAM lparam)
{
	CMFCPropertyGridProperty * pProp = ( CMFCPropertyGridProperty * ) lparam;
	DWORD dwData = pProp->GetData();

#pragma warning(disable:4800)
	switch(dwData)
	{
	case eSubAnimName:
		m_strAniName = pProp->FormatProperty();
		break;
	case eSubClassName:
		m_strFileName = pProp->FormatProperty();
		break;
	case eSubAnimList:
		{
			m_strSelectedAnimFileName = pProp->FormatProperty();
			bool bFind = false;

			for (int k = 0; k < (int)m_vAnimationList.size(); ++k)
			{
				if (m_strSelectedAnimFileName == m_vAnimationList[k])
				{
					m_pCurrAiState->SetAnim(g_pChara->getAnim(k));
					bFind = true;
					break;
				}
			}

			UpdateAll(m_nSelectedState);

			I3_TDK_UPDATE_INFO Data;
			Data.m_Event = I3_TDK_UPDATE_EDIT;
			::SendMessage(g_HwndDlgWeaponEditor, WM_TDK_UPDATE, (WPARAM) &Data, NULL);
			break;
		}
	case eSubAnimFileName:
		m_strFilterName = pProp->FormatProperty();
		break;
	case eSubBlendTime:
		{
			COleVariant rVariant = pProp->GetValue();
			VARIANT v = rVariant.Detach();
			VARTYPE vt = v.vt;
			assert(vt == VT_R4);
			m_fBlendTime = (float)v.fltVal;
		}
		break;
	case eSubBlendLoop:
		{
			COleVariant rVariant = pProp->GetValue();
			VARIANT v = rVariant.Detach();
			VARTYPE vt = v.vt;
			assert(vt == VT_BOOL);
			m_bLoop = (bool)v.boolVal;
		}
		break;
	case eSubIsAnimEnd:
		{
			COleVariant rVariant = pProp->GetValue();
			VARIANT v = rVariant.Detach();
			VARTYPE vt = v.vt;
			assert(vt == VT_BOOL);
			m_bIsAnimationEndtoBlend = (bool)v.boolVal;
		}
		break;
	case eSubIsTimeLimit:
		{
			COleVariant rVariant = pProp->GetValue();
			VARIANT v = rVariant.Detach();
			VARTYPE vt = v.vt;
			assert(vt == VT_BOOL);
			m_bIsBlendTimeLimit = (bool)v.boolVal;
		}
		break;
	case eSubSetTimeLimit:
		{
			COleVariant rVariant = pProp->GetValue();
			VARIANT v = rVariant.Detach();
			VARTYPE vt = v.vt;
			assert(vt == VT_R4);
			m_fBlendtimeLimit = (float)v.fltVal;
		}
		break;
	}
#pragma warning(default:4800)
	return TRUE;
} 



