#include "stdafx.h"
#include "PickTarget.h"
#include "Util.h"
#include "resource.h"

PickTarget::PickTarget(void)
{
}

void PickTarget::Reset(void)
{
	m_SelNodeList.clear();
}

BOOL PickTarget::ValidTarget( INode * pNode, bool bErrorMsg)
{
	Interface * ip = GetCOREInterface();
	
	for(size_t i = 0; i < m_SelNodeList.size(); i++)
	{
		
		if (i3::vu::int_index_of(m_SelNodeList, pNode) != -1)
		{
			if( bErrorMsg)
			{
				MessageBox( ip->GetMAXHWnd(), "선택한 노드는 동일 Node이므로 Proxy로 설정될 수 없습니다.", "오류", MB_OK);
			}

			return FALSE;
		}
	}

	if( GetProxyProperty( pNode))
	{
		if( bErrorMsg)
		{
			MessageBox( ip->GetMAXHWnd(), "이미 Proxy 설정을 가지고 있으며, Collision으로 설정된 Node이므로 Proxy로 설정될 수 없습니다.", "오류", MB_OK);
		}

		return FALSE;
	}

	return TRUE;
}

BOOL PickTarget::Filter(INode *node)
{
	if( node)
	{
		return ValidTarget( node, false);
	}

	return FALSE;
}

BOOL PickTarget::HitTest( IObjParam *ip, HWND hWnd, ViewExp *vpt,IPoint2 m,int flags)
{
	INode *node = ip->PickNode(hWnd,m,this);

	if( node)
	{
		return ValidTarget( node, false);
	}

	return FALSE;
}

void PickTarget::SetProxy( INode * pPicked)
{
	char * pszName = NULL;
	ICustButton * pBtn = GetICustButton( m_hwndBtn);

	if( pPicked != NULL)
	{
		pszName = pPicked->GetName();

		pBtn->SetText( pszName);
	}
	else
	{
		pszName = "";

		pBtn->SetText( "(None)");
	}

	// Proxy로 설정.
	for( size_t i = 0; i < m_SelNodeList.size(); i++)
	{
		INode * pNode = m_SelNodeList[i];

		pNode->SetUserPropString( TSTR( "Proxy_Geometry"), pszName);
	}

	ReleaseICustButton( pBtn);
}

BOOL PickTarget::Pick(IObjParam *ip,ViewExp *vpt)
{
	INode *node = vpt->GetClosestHit();
	assert(node);
	INode * pPicked;

	if( ValidTarget( node, true))
	{
		pPicked = node;
	}
	else
	{
		pPicked  = NULL;
	}

	SetProxy( pPicked);

	ip->RedrawViews( ip->GetTime());

	return TRUE;
}

void PickTarget::EnterMode(IObjParam *ip)
{
	ICustButton *iBut = GetICustButton( m_hwndBtn);

	if( iBut != NULL)
	{
		iBut->SetCheck(TRUE);
		iBut->SetText( "(Pick Proxy)");

		ReleaseICustButton(iBut);
	}
}

void PickTarget::ExitMode(IObjParam *ip)
{
	ICustButton *iBut = GetICustButton( GetDlgItem( m_hwndDlg, IDC_BTN_PROXY));

	if( iBut != NULL)
	{
		iBut->SetCheck( FALSE);
		//iBut->SetCheckHighlight( FALSE);

		ReleaseICustButton(iBut);
	}
}

