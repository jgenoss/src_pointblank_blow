// Prop_LvRes_Animation.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvRes_Animation.h"
#include ".\prop_lvres_animation.h"

// CProp_LvRes_Animation dialog

IMPLEMENT_DYNAMIC(CProp_LvRes_Animation, i3TDKDialogBase)
CProp_LvRes_Animation::CProp_LvRes_Animation(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvRes_Animation::IDD, pParent)
{
	m_pResAnimation = NULL;
}

CProp_LvRes_Animation::~CProp_LvRes_Animation()
{
	I3_SAFE_RELEASE( m_pResAnimation);
}

void CProp_LvRes_Animation::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_PROP_LVRES_ANIMATION, m_listAnimInfo);
}


BEGIN_MESSAGE_MAP(CProp_LvRes_Animation, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CProp_LvRes_Animation message handlers
void CProp_LvRes_Animation::SetObject( i3ElementBase * pObj)
{
	if( pObj != NULL && !i3::same_of<i3LevelResAnimation*>(pObj)) //->IsExactTypeOf( i3LevelResAnimation::static_meta()) == FALSE)
	{
		I3TRACE( "pObj is not a Instance of i3LevelResAnimation\n");

		I3_SAFE_RELEASE( m_pResAnimation);
	}
	else
	{
		I3_REF_CHANGE( m_pResAnimation, (i3LevelResAnimation *)pObj);
	}

	_UpdateResInfo();
}

void CProp_LvRes_Animation::_UpdateResInfo( void)
{
	INT32	nIndex				= 0;
	char	conv[MAX_PATH];

	if( m_pResAnimation == NULL)
	{
		sprintf( conv, "%s", "");
		m_listAnimInfo.SetItemText( nIndex++, 1, conv);
		m_listAnimInfo.SetItemText( nIndex++, 1, conv);
		m_listAnimInfo.SetItemText( nIndex++, 1, conv);
		m_listAnimInfo.SetItemText( nIndex++, 1, conv);
		m_listAnimInfo.SetItemText( nIndex++, 1, conv);
		m_listAnimInfo.SetItemText( nIndex++, 1, conv);

		return ;
	}

	sprintf( conv, "%s", m_pResAnimation->getPath());
	m_listAnimInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "0x%08X", (unsigned int)m_pResAnimation->getHashCode());
	m_listAnimInfo.SetItemText( nIndex++, 1, conv);

	{
		UINT32 nResType = m_pResAnimation->getResType();
		switch( nResType)
		{
			case	I3LV_TYPE_PRIMITIVE:	sprintf( conv, "%s", "I3LV_TYPE_PRIMITIVE");	break;
			case	I3LV_TYPE_CHARA:		sprintf( conv, "%s", "I3LV_TYPE_CHARA");		break;
			case	I3LV_TYPE_WORLD:		sprintf( conv, "%s", "I3LV_TYPE_WORLD");		break;
			case	I3LV_TYPE_OBJECT:		sprintf( conv, "%s", "I3LV_TYPE_OBJECT");		break;
			case	I3LV_TYPE_EFFECT:		sprintf( conv, "%s", "I3LV_TYPE_EFFECT");		break;
			case	I3LV_TYPE_LIGHT:		sprintf( conv, "%s", "I3LV_TYPE_LIGHT");		break;
			case	I3LV_TYPE_CAMERA:		sprintf( conv, "%s", "I3LV_TYPE_CAMERA");		break;
			case	I3LV_TYPE_TEXTURE:		sprintf( conv, "%s", "I3LV_TYPE_TEXTURE");		break;
			case	I3LV_TYPE_ANIMATION:	sprintf( conv, "%s", "I3LV_TYLE_ANIMATION");	break;
			case	I3LV_TYPE_SOUND:		sprintf( conv, "%s", "I3LV_TYPE_SOUND");		break;
			case	I3LV_TYPE_SCENEGRAPH:	sprintf( conv, "%s", "I3LV_TYPE_SCENEGRAPH");	break;
			default:
				sprintf( conv, "%s", "Unknown Type");	break;

		}

		m_listAnimInfo.SetItemText( nIndex++, 1, conv);
	}

	sprintf( conv, "%08X", m_pResAnimation->getResState());
	m_listAnimInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%d", m_pResAnimation->getTrackCount());
	m_listAnimInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "%08f", m_pResAnimation->getDuration());
	m_listAnimInfo.SetItemText( nIndex++, 1, conv);
}

BOOL CProp_LvRes_Animation::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	m_listAnimInfo.InsertColumn( 0, "Content", 0, 128);
	m_listAnimInfo.InsertColumn( 1, "Value", 0, 512);

	// TODO:  Add extra initialization here
	INT32	nIndex = 0;

	nIndex = m_listAnimInfo.InsertItem( nIndex, "파일 경로", 0);
	nIndex = m_listAnimInfo.InsertItem( nIndex + 1, "해쉬 코드", 0);
	nIndex = m_listAnimInfo.InsertItem( nIndex + 1, "리소스 종류", 0);
	nIndex = m_listAnimInfo.InsertItem( nIndex + 1, "리소스 상태", 0);
	nIndex = m_listAnimInfo.InsertItem( nIndex + 1, "트랙수", 0);
	nIndex = m_listAnimInfo.InsertItem( nIndex + 1, "총 시간(초)", 0);

	_UpdateResInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProp_LvRes_Animation::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_listAnimInfo.m_hWnd))
	{
		m_listAnimInfo.SetWindowPos( NULL, 4, 4, cx - 8, cy - 8, SWP_NOACTIVATE | SWP_NOZORDER);
		m_listAnimInfo.SetColumnWidth( 1, cx - 8 - 128);
	}
}

int CProp_LvRes_Animation::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	RECT rt;
	rt.left = rt.top = 4;
	rt.right = rt.bottom = 768;

	m_listAnimInfo.Create( 0, rt, this, 101);
	return 0;
}
