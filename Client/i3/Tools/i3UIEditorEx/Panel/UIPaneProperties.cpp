// UIProperties.cpp : implementation file
//

#include "stdafx.h"
#include "../i3UIEditor.h"
#include "UIPaneProperties.h"
#include "../UIGlobalRes.h"
#include "../MainFrm.h"

// CUIProperties

IMPLEMENT_DYNAMIC(CUIPaneProperties, CUIPaneBase)

CUIPaneProperties::CUIPaneProperties()
{

}

CUIPaneProperties::~CUIPaneProperties()
{
}


BEGIN_MESSAGE_MAP(CUIPaneProperties, CUIPaneBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CUIProperties message handlers



int CUIPaneProperties::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect clientRt;
	GetClientRect( &clientRt);

	{
		if (!m_wndPropList.Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, clientRt, this, (UINT)-1))
		{
			TRACE0("Failed to create Properties Grid \n");
			return -1;      // fail to create
		}

		CFont * pFont = CFont::FromHandle( UIGlobalRes::getDefFont());

		m_wndPropList.SetFont( pFont);
	}

	InitPropList();

	AdjustLayout();

	SetVSDotNetLook( TRUE);

	return 0;
}

void CUIPaneProperties::InitPropList()
{
	m_wndPropList.EnableHeaderCtrl( TRUE, "Name", "Value");
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();
	
	i3TDK::RegisterUpdate( m_hWnd, i3ElementBase::static_meta(), I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_UNSELECT | I3_TDK_UPDATE_REMOVE);
	i3TDK::RegisterUpdate( m_hWnd, i3UIScene::static_meta(), I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_UNSELECT | I3_TDK_UPDATE_BEFORE_PROP_CHANGE | I3_TDK_UPDATE_SET);
}

void CUIPaneProperties::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CUIPaneProperties::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( (pInfo->m_Event & I3_TDK_UPDATE_SELECT) || 
		(pInfo->m_Event & I3_TDK_UPDATE_UNSELECT) )
	{
		CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

		//SELCET이든 UNSELECT이든 현재 gripList에 있는 Obj들을 보고 결정

		i3::vector<i3UIControl*> objList;		//i3ElementBase*

		pMainFrm->getSelectedObjList( objList);
		
		INT32 nSelectedObjCount = (INT32)objList.size();

		//I3TRACE( "CUIPaneProperties::OnUpdate: %d\n", nSelectedObjCount);

		if( nSelectedObjCount > 1)
		{
			m_wndPropList.ClearProps();
			m_wndPropList.clearNotifyList();

			bool bAllSame = true;
			i3ClassMeta* pMetaToCompare = ((i3ElementBase*)objList[nSelectedObjCount-1])->meta();

			for( INT32 i=nSelectedObjCount-2; i >= 0; i--)
			{
				i3ElementBase* pObj = objList[i];
				if( pObj->same_of( pMetaToCompare) == false)
				{
					bAllSame = false;
					break;
				}
			}

			if( bAllSame)
			{
				for( INT32 i=nSelectedObjCount-1; i >= 0; i--)
				{
					i3ElementBase* pObj = objList[i];
					if( i == nSelectedObjCount-1)
					{
						m_wndPropList.AddProp( pObj);
					}
					else
					{
						//I3TRACE("addToNotifyList: %p\n", pObj);
						m_wndPropList.addToNotifyList( pObj);
					}
				}
			}
		}
		else
		{
			if( pInfo->m_pObject != NULL)
			{
				m_wndPropList.ClearProps();
				m_wndPropList.clearNotifyList();
				m_wndPropList.AddProp( pInfo->m_pObject);
			}
		}
	}
	else if( pInfo->m_Event == I3_TDK_UPDATE_SET)
	{
		m_wndPropList.ClearProps();
		m_wndPropList.clearNotifyList();
	}
	else if( pInfo->m_Event == I3_TDK_UPDATE_REMOVE)
	{
		if( pInfo->m_pObject == m_wndPropList.getBaseObj())
		{
			m_wndPropList.ClearProps();
			m_wndPropList.clearNotifyList();
		}
	}
}

LRESULT CUIPaneProperties::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	if (i3UI::IsTDKPropertyUpdate())
	{
		i3ElementBase* pObj = m_wndPropList.getBaseObj();
		m_wndPropList.ClearProps();
		m_wndPropList.clearNotifyList();
		m_wndPropList.AddProp(pObj);

		i3UI::setTDKPropertyUpdate(false);
	}

	return CDockablePane::WindowProc(message, wParam, lParam);
}

void CUIPaneProperties::Update( void)
{
	if( m_wndPropList.IsWindowEnabled())
	{
		m_wndPropList.UpdateToProperties();
		//i3ElementBase* pBase = m_wndPropList.GetBase();
		//if( pBase != NULL)
		//{
		//	//비효율적이지만 mfcpropertygridctrl에 값만 Update해주는 함수가 없다면.. 일단 이렇게라도.. 
		//	m_wndPropList.RemoveAll();
		//	m_wndPropList.AddProp( pBase);
		//}
	}
}

void CUIPaneProperties::SendMessagesToPropertyGrid( const i3::vector<PROPERTYMSG*>& msgList)
{
	if( m_wndPropList.IsWindowEnabled())
	{
		m_wndPropList.ProcessMessages( msgList);
	}
}

BOOL CUIPaneProperties::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CUIPaneBase::PreTranslateMessage(pMsg);
}

void CUIPaneProperties::DragTemplate(i3UITemplate* pTemplate)
{
	CPoint point;

	::GetCursorPos(&point);
	this->ScreenToClient(&point);
	i3TDKPropertyGridProperty* pProperty = (i3TDKPropertyGridProperty*)m_wndPropList.HitTest(point);

	if (pProperty && i3::contain_string(((CMFCPropertyGridProperty*)pProperty)->GetName(), "TemplateName_") >= 0)
	{
		pProperty->SetValue(pTemplate->GetName());
		pProperty->UpdateFromProperty();

		m_wndPropList.getBaseObj()->OnTDKPropertyChanged(((CMFCPropertyGridProperty*)pProperty)->GetName());
	}
}