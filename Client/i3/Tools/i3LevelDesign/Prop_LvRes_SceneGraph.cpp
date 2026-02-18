// Prop_LvRes_SceneGraph.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvRes_SceneGraph.h"
#include ".\prop_lvres_scenegraph.h"


// CProp_LvRes_SceneGraph dialog

IMPLEMENT_DYNAMIC(CProp_LvRes_SceneGraph, i3TDKDialogBase)
CProp_LvRes_SceneGraph::CProp_LvRes_SceneGraph(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvRes_SceneGraph::IDD, pParent)
{
	m_pResSceneGraph = NULL;
}

CProp_LvRes_SceneGraph::~CProp_LvRes_SceneGraph()
{
	I3_SAFE_RELEASE( m_pResSceneGraph);
}

void CProp_LvRes_SceneGraph::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_PROP_LVRES_SCENEGRAPH, m_listSceneGraphInfo);
}


BEGIN_MESSAGE_MAP(CProp_LvRes_SceneGraph, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CProp_LvRes_SceneGraph message handlers
void CProp_LvRes_SceneGraph::SetObject( i3ElementBase * pObj)
{
	if( (pObj != NULL) && !i3::same_of<i3LevelResSceneGraph*>(pObj)) //->IsExactTypeOf( i3LevelResSceneGraph::static_meta()) == FALSE )
	{
		I3TRACE("pObj is not a Instance of i3LevelResSceneGraph\n");

		I3_SAFE_RELEASE( m_pResSceneGraph);
	}
	else
	{
		I3_REF_CHANGE( m_pResSceneGraph, (i3LevelResSceneGraph *)pObj);
	}

	_UpdateResInfo();
}

void CProp_LvRes_SceneGraph::_UpdateResInfo( void)
{
	INT32	nIndex				= 0;
	char	conv[MAX_PATH];

	if( m_pResSceneGraph == NULL)
	{
		sprintf( conv, "%s", "");
		m_listSceneGraphInfo.SetItemText( nIndex++, 1, conv);
		m_listSceneGraphInfo.SetItemText( nIndex++, 1, conv);
		m_listSceneGraphInfo.SetItemText( nIndex++, 1, conv);
		m_listSceneGraphInfo.SetItemText( nIndex++, 1, conv);
		m_listSceneGraphInfo.SetItemText( nIndex++, 1, conv);
		return ;
	}

	sprintf( conv, "%s", m_pResSceneGraph->getPath());
	m_listSceneGraphInfo.SetItemText( nIndex++, 1, conv);

	sprintf( conv, "0x%08X", (unsigned int)m_pResSceneGraph->getHashCode());
	m_listSceneGraphInfo.SetItemText( nIndex++, 1, conv);

	{
		UINT32 nResType = m_pResSceneGraph->getResType();
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

		m_listSceneGraphInfo.SetItemText( nIndex++, 1, conv);
	}

	sprintf( conv, "%08X", m_pResSceneGraph->getResState());
	m_listSceneGraphInfo.SetItemText( nIndex++, 1, conv);

	{
		UINT32 nSgType = m_pResSceneGraph->getSgType();
		switch( nSgType)
		{
			case	I3LV_SG_TYPE_NORMAL:	sprintf( conv, "%s", "I3LV_SG_TYPE_NORMAL");	break;
			case	I3LV_SG_TYPE_BODY:		sprintf( conv, "%s", "I3LV_SG_TYPE_BODY");		break;
			case	I3LV_SG_TYPE_EFFECT:	sprintf( conv, "%s", "I3LV_SG_TYPE_EFFECT");	break;
			default:
				sprintf( conv, "%s", "Unknown SceneGraph Type");	break;
		}

		m_listSceneGraphInfo.SetItemText( nIndex++, 1, conv);
	}
}

BOOL CProp_LvRes_SceneGraph::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	INT32	nIndex = 0;

	m_listSceneGraphInfo.InsertColumn( 0, "Content", 0, 128);
	m_listSceneGraphInfo.InsertColumn( 1, "Value", 0, 512);

	nIndex = m_listSceneGraphInfo.InsertItem( nIndex, "파일 경로", 0);
	nIndex = m_listSceneGraphInfo.InsertItem( nIndex + 1, "해쉬 코드", 0);
	nIndex = m_listSceneGraphInfo.InsertItem( nIndex + 1, "리소스 종류", 0);
	nIndex = m_listSceneGraphInfo.InsertItem( nIndex + 1, "리소스 상태", 0);
	nIndex = m_listSceneGraphInfo.InsertItem( nIndex + 1, "SceneGraph Type", 0);

	_UpdateResInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProp_LvRes_SceneGraph::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_listSceneGraphInfo.m_hWnd))
	{
		m_listSceneGraphInfo.SetWindowPos( NULL, 4, 4, cx - 8, cy - 8, SWP_NOACTIVATE | SWP_NOZORDER);
		m_listSceneGraphInfo.SetColumnWidth( 1, cx - 8 - 128);
	}
}

int CProp_LvRes_SceneGraph::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	RECT rt;
	rt.left = rt.top = 4;
	rt.right = rt.bottom = 768;

	m_listSceneGraphInfo.Create( 0, rt, this, 101);
	return 0;
}
