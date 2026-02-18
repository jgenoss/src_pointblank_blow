#include "stdafx.h"
#include "PropertyPane.h"
#include "resource.h"
#include "MainFrm.h"
#include "i3TDKGlobalVariable.h"
#include "GlobalInstance.h"
#include "i3Base/string/ext/atoi.h"
#include "ADDAnimationDlg.h"

#define DEFALUT_UV_IMAGESIZE	64
#define DEFALUT_UV_IMAGEPOS		100

#define ID_PROPERTY 1
#define ID_PROPERTY_POS_X 2
#define ID_PROPERTY_POS_Y 3
#define ID_PROPERTY_SIZE_W 4
#define ID_PROPERTY_SIZE_H 5

#define ID_WND_ADDUV			1
#define ID_WND_MFCPROPERTY		2
#define ID_WND_ADDANIMATION		3


IMPLEMENT_DYNAMIC(CPropertyPane, CPaneBase)

BEGIN_MESSAGE_MAP(CPropertyPane, CPaneBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_WND_ADDUV, &CPropertyPane::OnUpdateButton)
	ON_UPDATE_COMMAND_UI(ID_WND_ADDANIMATION, &CPropertyPane::OnUpdateButton)
	ON_BN_CLICKED(ID_WND_ADDUV, &CPropertyPane::OnBnClickedADDUV)
	ON_BN_CLICKED(ID_WND_ADDANIMATION, &CPropertyPane::OnBnClickedADDAnimation)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()


CPropertyPane::CPropertyPane()
{
}


CPropertyPane::~CPropertyPane()
{
}

int CPropertyPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPaneBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rt;
	m_btADDUV.Create("ADDUV", WS_CHILD | WS_VISIBLE, rt, this, ID_WND_ADDUV);
	m_btADDAnimation.Create("ADDAnimation", WS_CHILD | WS_VISIBLE, rt, this, ID_WND_ADDANIMATION);
	m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rt, this, ID_WND_MFCPROPERTY);

	AdjustLayout();

	return 0;
}

void CPropertyPane::Init()
{
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pBaseList = new CMFCPropertyGridProperty(_T("Propertys"));
	pBaseList->SetData(ID_PROPERTY);

	m_wndPropList.AddProperty(pBaseList);
}

void CPropertyPane::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	INT32 nHeight = 20;

	//백그라운드
	m_btADDUV.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), nHeight, SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndPropList.SetWindowPos(NULL, rectClient.left , rectClient.top + nHeight, rectClient.Width() , rectClient.Height() - 40, SWP_NOACTIVATE | SWP_NOZORDER);

	m_btADDAnimation.SetWindowPos(NULL, rectClient.left, rectClient.Height() - nHeight, rectClient.Width(), nHeight, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPropertyPane::OnSize(UINT nType, int cx, int cy)
{
	CPaneBase::OnSize(nType, cx, cy);

	AdjustLayout();
}



void CPropertyPane::OnBnClickedADDUV()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	CGlobalInstance::getSingletonPtr()->AddUVItem(pMainWnd->GetTextureView()->GetImage(), 
		CRect(DEFALUT_UV_IMAGEPOS, DEFALUT_UV_IMAGEPOS, DEFALUT_UV_IMAGESIZE, DEFALUT_UV_IMAGESIZE));
}

void CPropertyPane::OnBnClickedADDAnimation()
{
	CADDAnimationDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		if (strlen(dlg.m_chTitle) <= 0)
		{
			sprintf_s(dlg.m_chTitle, "Animation");
		}

		if ( dlg.m_fLifeTime <= 0)
		{
			dlg.m_fLifeTime = 5;
		}

		CGlobalInstance::getSingletonPtr()->AddAnimationItem(dlg.m_chTitle, dlg.m_fLifeTime);
	}
}


void CPropertyPane::OnUpdateButton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CPropertyPane::Update(UVITEM* pItem)
{
	CMFCPropertyGridProperty* pProperty = m_wndPropList.FindItemByData(ID_PROPERTY);

	RemoveAllProperty(pProperty);

	char str[1024];
	INT32 nIndex = CGlobalInstance::getSingletonPtr()->GetUVItem(pItem->_pControl);
	sprintf_s(str, "%d", nIndex);
	CMFCPropertyGridProperty* pChildProperty = new CMFCPropertyGridProperty(_T("Index"), str);
	pChildProperty->Enable(false);
	pProperty->AddSubItem(pChildProperty);

	sprintf_s(str, "%d", (INT32)pItem->_pControl->getPos()->x);
	pChildProperty = new CMFCPropertyGridProperty(_T("PosX"), str);
	pChildProperty->SetData(ID_PROPERTY_POS_X);
	pProperty->AddSubItem(pChildProperty);

	sprintf_s(str, "%d", (INT32)pItem->_pControl->getPos()->y);
	pChildProperty = new CMFCPropertyGridProperty(_T("PosY"), str);
	pChildProperty->SetData(ID_PROPERTY_POS_Y);
	pProperty->AddSubItem(pChildProperty);

	sprintf_s(str, "%d", (INT32)pItem->_pControl->getSize()->x);
	pChildProperty = new CMFCPropertyGridProperty(_T("Width"), str);
	pChildProperty->SetData(ID_PROPERTY_SIZE_W);
	pProperty->AddSubItem(pChildProperty);

	sprintf_s(str, "%d", (INT32)pItem->_pControl->getSize()->y);
	pChildProperty = new CMFCPropertyGridProperty(_T("Height"), str);
	pChildProperty->SetData(ID_PROPERTY_SIZE_H);
	pProperty->AddSubItem(pChildProperty);

	pChildProperty = new CMFCPropertyGridProperty(_T("ImagePath"), pItem->_pTexture->GetName());
	pChildProperty->Enable(false);
	pProperty->AddSubItem(pChildProperty);

	m_wndPropList.ExpandAll();

	m_nCurControlIndex = nIndex;
}

void CPropertyPane::RemoveAllProperty(CMFCPropertyGridProperty* pProperty)
{
	if (pProperty->GetSubItemsCount() > 0)
	{
		CMFCPropertyGridProperty *pSubProperty = pProperty->GetSubItem(0);
		while (true)
		{
			pProperty->RemoveSubItem(pSubProperty);

			if (pProperty->GetSubItemsCount() == 0)
				break;
			else
				pSubProperty = pProperty->GetSubItem(0);
		}
	}
}

void CPropertyPane::RemoveAllProperty()
{
	CMFCPropertyGridProperty* pProperty = m_wndPropList.FindItemByData(ID_PROPERTY);

	RemoveAllProperty(pProperty);

	m_wndPropList.ExpandAll();
}

LRESULT CPropertyPane::OnPropertyChanged(WPARAM wparam, LPARAM lparam)
{
	CMFCPropertyGridProperty *pProp = (CMFCPropertyGridProperty*)lparam;
	if (!pProp) return 0;

	UpdateUVItem(pProp);

	return S_OK;
}

void CPropertyPane::UpdateUVItem(CMFCPropertyGridProperty* pProperty)
{
	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();
	UVITEM* pItem = CGlobalInstance::getSingletonPtr()->GetUVItem(m_nCurControlIndex);
	if (pItem == NULL) return;
	INT32 nValue = 0;

	nValue = GetPropertyValue(pProperty);
	VEC3D vPos = *pItem->_pControl->getPos();
	VEC2D vSize = *pItem->_pControl->getSize();

	switch (pProperty->GetData())
	{
	case ID_PROPERTY_POS_X:
		{
			vPos.x = nValue;
			pItem->_pControl->setPos(&vPos);
		}
		break;
	case ID_PROPERTY_POS_Y:
		{
			vPos.y = nValue;
			pItem->_pControl->setPos(&vPos);
		}
		break;
	case ID_PROPERTY_SIZE_W:
		vSize.x = nValue;
		pItem->_pControl->setSize(&vSize);
		break;
	case ID_PROPERTY_SIZE_H:
		vSize.y = nValue;
		pItem->_pControl->setSize(&vSize);
		break;
	}

	CGlobalInstance::getSingletonPtr()->UpdateUVItem(pItem->_pControl);
	pMainWnd->GetTextureView()->UpdateInvaliDate();
}

INT32 CPropertyPane::GetPropertyValue(CMFCPropertyGridProperty* pProperty)
{
	COleVariant rVariant = pProperty->GetValue();
	VARIANT var = rVariant.Detach();
	i3::string val = "";
	INT32 nVal = 0;
	switch (var.vt)
	{
		case VT_BSTR:
		{
			i3::utf16_to_mb(var.bstrVal, val);
			nVal = i3::atoi(val);
		}
		break;
	}

	return nVal;
}