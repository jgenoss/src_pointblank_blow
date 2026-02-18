// PanelAIGraph.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "PanelAIGraph.h"
#include "GICShapeAI.h"
#include "Dlg_Replace.h"
#include "i3Base/string/ext/resolve_path_token.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/generic_string_replace_all.h"
#include "i3Base/string/compare/generic_is_equal.h"

// CPanelAIGraph dialog

IMPLEMENT_DYNAMIC(CPanelAIGraph, i3TDKDialogBase)
CPanelAIGraph::CPanelAIGraph(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelAIGraph::IDD, pParent)
{
	m_szSrc[0] = 0;
	m_szDest[0] = 0;
}

CPanelAIGraph::~CPanelAIGraph()
{
}

void CPanelAIGraph::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_SELMODE, m_SelCtrl);
	DDX_Control(pDX, IDC_BTN_CREATEAI, m_CreateAICtrl);
	DDX_Control(pDX, IDC_BTN_CREATENEXT, m_LinkAICtrl);
	DDX_Control(pDX, IDC_ED_NAME, m_NameCtrl);
	DDX_Control(pDX, IDC_CB_AISTATE, m_AIStateCtrl);
}

void CPanelAIGraph::SetEditMode( EDIT_MODE mode)
{
	m_EditMode = mode;

	m_SelCtrl.SetState( FALSE);
	m_CreateAICtrl.SetState( FALSE);
	m_LinkAICtrl.SetState( FALSE);

	switch( m_EditMode)
	{
		case EDIT_MODE_SELECT :		
			m_SelCtrl.SetState( TRUE);			
			m_View.SetMouseMode( GIC_MOUSEMODE_NONE);
			break;

		case EDIT_MODE_CREATE_AI :	
			m_CreateAICtrl.SetState( TRUE);		
			m_View.SetMouseMode( GIC_MOUSEMODE_CREATE);
			m_View.SetMetaToCreate( GICShapeAI::static_meta());
			break;

		case EDIT_MODE_LINK :		
			m_LinkAICtrl.SetState( TRUE);
			break;
	}
}

void CPanelAIGraph::SetAI( AI * pAI)
{
	BOOL bFlag = FALSE;
	INT32 i, idx;

	m_View.SetAI( pAI);

	if( pAI != NULL)
	{
		bFlag = TRUE;

		m_NameCtrl.SetWindowText( pAI->GetName());

		m_AIStateCtrl.ResetContent();
		for( i = 0; i < pAI->getAI()->getAIStateCount(); i++)
		{
			i3AIState * pState = pAI->getAI()->getAIState( i);

			idx = m_AIStateCtrl.AddString( pState->GetName());
			m_AIStateCtrl.SetItemData( idx, (DWORD_PTR) pState);
		}
	}
	else
	{
		m_NameCtrl.SetWindowText( "");
		m_AIStateCtrl.ResetContent();
	}
	
	m_NameCtrl.EnableWindow( bFlag);
	m_SelCtrl.EnableWindow( bFlag);
	m_CreateAICtrl.EnableWindow( bFlag);
	m_LinkAICtrl.EnableWindow( bFlag);
}


BEGIN_MESSAGE_MAP(CPanelAIGraph, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_CREATEAI, OnBnClickedBtnCreateai)
	ON_BN_CLICKED(IDC_BTN_CREATENEXT, OnBnClickedBtnCreatenext)
	ON_BN_CLICKED(IDC_BTN_SELMODE, OnBnClickedBtnSelmode)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CPanelAIGraph::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CHECK_TEMPLATE, &CPanelAIGraph::OnBnClickedBtnCheckTemplate)
	ON_BN_CLICKED(IDC_BTN_ADJUST, &CPanelAIGraph::OnBnClickedBtnAdjust)
	ON_BN_CLICKED(IDC_BTN_REPLACE, &CPanelAIGraph::OnBnClickedBtnReplace)
END_MESSAGE_MAP()


// CPanelAIGraph message handlers

BOOL CPanelAIGraph::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		HICON hIcon;
		HINSTANCE hInst;

		hInst = AfxGetApp()->m_hInstance;

		hIcon = (HICON) LoadImage( hInst, MAKEINTRESOURCE( IDI_SELMODE), IMAGE_ICON, 16, 16, LR_VGACOLOR);
		m_SelCtrl.SetIcon( hIcon);

		hIcon = (HICON) LoadImage( hInst, MAKEINTRESOURCE( IDI_CREATEAI), IMAGE_ICON, 16, 16, LR_VGACOLOR);
		m_CreateAICtrl.SetIcon( hIcon);

		hIcon = (HICON) LoadImage( hInst, MAKEINTRESOURCE( IDI_CREATENEXT), IMAGE_ICON, 16, 16, LR_VGACOLOR);
		m_LinkAICtrl.SetIcon( hIcon);
	}

	{
		CRect rect;

		GetClientRect( &rect);

		m_View.Create( "", WS_CHILD | WS_VISIBLE, rect, this, 100);
	}

	SetAI( NULL);
	SetEditMode( EDIT_MODE_SELECT);

	i3TDK::RegisterUpdate( m_hWnd, CChara::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, AI::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, GICShapeAI::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CPanelAIGraph::OnCancel()
{
}

void CPanelAIGraph::OnOK()
{
}

void CPanelAIGraph::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( i3::kind_of<AI*>(pInfo->m_pMeta)) //->IsTypeOf( AI::static_meta()))
		{
			if( pInfo->m_pObject != NULL)
			{
				SetAI( (AI *) pInfo->m_pObject);
			}
		}
	}
	else if( pInfo->m_Event & (I3_TDK_UPDATE_UNSELECT | I3_TDK_UPDATE_REMOVE))
	{
		if( i3::kind_of<GICShapeAI*>(pInfo->m_pMeta)) //->IsTypeOf( GICShapeAI::static_meta()))
		{
			GICShapeAI * pShape = (GICShapeAI *) pInfo->m_pObject;

			if(( pShape == NULL))
			{
				SetAI( NULL);
			}
		}
		else if( i3::kind_of<CChara*>(pInfo->m_pMeta)) //->IsTypeOf( CChara::static_meta()))
		{
			SetAI( NULL);
		}
		else if( i3::kind_of<AI*>(pInfo->m_pMeta)) //->IsTypeOf( AI::static_meta()))
		{
			if( (pInfo->m_pObject == NULL) || ( m_View.getAI() == pInfo->m_pObject))
			{
				SetAI( NULL);
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
	{
		m_View.Invalidate();
	}
}

LRESULT CPanelAIGraph::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CPanelAIGraph::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_View.m_hWnd))
	{
		m_View.SetWindowPos( NULL, 37, 30, cx - 37 - 3, cy - 30 - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelAIGraph::OnBnClickedBtnCreateai()
{
	SetEditMode( EDIT_MODE_CREATE_AI);
}

void CPanelAIGraph::OnBnClickedBtnCreatenext()
{
	SetEditMode( EDIT_MODE_LINK);
}

void CPanelAIGraph::OnBnClickedBtnSelmode()
{
	SetEditMode( EDIT_MODE_SELECT);
}

void CPanelAIGraph::OnBnClickedBtnSave()
{
	CFileDialog	Dlg( FALSE, "TXT", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"AI-State Dump File(*.TXT)|*.TXT||", this);

	if( Dlg.DoModal() != IDOK) return;

	AI * pAI = m_View.getAI();

	pAI->DumpTXT( LPCTSTR( Dlg.GetPathName()));
}

struct CHARA_INFO
{
	char		m_szName[128];
	bool		m_bFemale;
	bool		m_bDefault;
};

static CHARA_INFO	s_CharaList[] =
{
	{	"RB_DFx",				false,		false} ,
	{	"SW_Hide",				true,		false} ,
	{	"SW_Lprd",				false,		false} ,
	{	"RB_Trntl",				true,		false}, 
	{	"RB_VprRd",				true,		false},
	{	"RB_RdBls",				false,		false},
	{	"Base_FM_KnEys",		true,		true},
	{	"Base_M_AcdPl",			false,		true},
	{	"",						false,		false }
};


struct REPLACE_SYMBOL
{
	char		m_szSrc[128];
	char		m_szDest[128];
};

static REPLACE_SYMBOL s_ReplaceSymbols[] =
{
	{ "ShotGun",				"Shot_Gun" },
	{ "AssultRifle",			"Assault_Rifle" },
	{ "DualHandGun",			"Dual_Hand_Gun" },
	{ "DualKnife",				"Dual_Knife" },
	{ "DualSMG",				"Dual_SMG" },
	{ "HandGun",				"Hand_Gun" },
	{ "SniperRifle",			"Sniper_Rifle" },
	{ "Brass_Knuckle",			"Knife_Brass_Knuckle" },
	{ "DualGrenade",			"Throw_Candy_Grenade" },

	{ "MOVE_ZOOMIDLE.i3A",		"Zoom_Move.i3a" },
	{ "JUMPSTART_ZOOMIDLE.I3A",	"Zoom_Jump_Start.i3a" },
	{ "JUMPEND_ZOOMIDLE.I3A",	"Zoom_Jump_End.i3a" },
	{ "DAMAGE_ZOOMIDLE.I3A",	"Zoom_Damage.i3a" },
	{ "ZOOMIDLE.I3A",			"Zoom_Idle.i3a" },
	{ "ZOOMATTACK.I3A",			"Zoom_Attack.i3a" },

	{ "",						"" }
};

void CPanelAIGraph::AdjustAnimPath( i3AIState * pState)
{

	i3::vector<i3::rc_string>	List;

	i3::resolve_path_token( pState->getAnimName(), List);

	if( List.size() < 2)
		return;

	if( i3::generic_is_iequal( List[0].c_str(), "Chara") )
	{
		// 처음 경로가 "Chara"다..."Anim"으로 변경해야 한다.
		List.erase(List.begin());
		List.insert(List.begin(), "Anim");				// List.Insert( 0, "Anim");
	}

	// 기존의 "1PV" 또는 "3PV"가 있다면 위치를 옮겨준다.
	{
		size_t sidx = i3::vu::index_of(List, "1PV" );
		INT32 idx = (sidx != List.size()) ? INT32(sidx) : -1;

		char szStore[ 32];

		szStore[0] = 0;

		if( idx != -1)
		{
			i3::safe_string_copy( szStore, "1PV", 32 );
			List.erase(List.begin() + idx);
		}
		else
		{
			idx = i3::vu::int_index_of(List, "3PV");

			if( idx != -1)
			{
				i3::safe_string_copy( szStore, "3PV", 32 );
				List.erase(List.begin() + idx);
			}
		}

		if( szStore[0] != 0)
		{
			// 시점 경로를 뒤에 추가.
			//List.Insert( List.GetCount() - 1, szStore);
			List.insert(List.end() - 1, szStore);
		}
	}

	// 치환이 필요한 여타 문장들의 처리.
	{
		INT32 j;
		char szTemp[256];
		bool bReplace;

		szTemp[0] = 0;

		for(size_t i = 0; i < List.size(); i++)
		{
			i3::safe_string_copy( szTemp, List[i], 256 );

			bReplace = false;

			if( i3::generic_is_iequal( szTemp, "UI") )
			{
				i3::safe_string_copy( szTemp, "Common", 256 );
				bReplace = true;
			}

			for( j = 0; s_ReplaceSymbols[j].m_szSrc[0] != 0; j++)
			{
			//	if( i3String::ReplaceNoCase( szTemp, sizeof(szTemp), s_ReplaceSymbols[j].m_szSrc, s_ReplaceSymbols[j].m_szDest))
				i3::generic_string_ireplace_all( szTemp, s_ReplaceSymbols[j].m_szSrc, s_ReplaceSymbols[j].m_szDest);
				
			}

			bReplace = !i3::generic_is_equal(szTemp, List[i]);

			if( bReplace)
			{
				//i3::vu::erase_index(List, i);	//	List.Delete( i);
				//List.insert(List.begin() + i, szTemp);	// List.Insert( i, szTemp);
				List[i] = szTemp;		// 그냥 대입이 더 나을것 같구만...
			}
		}
	}

	// 완성
	{
		char szTemp[256];

		szTemp[0] = 0;

		for(size_t i = 0; i < List.size(); i++)
		{
			i3::generic_string_cat( szTemp, List[i].c_str());

			if( i < (List.size() - 1))
			{
				i3::generic_string_cat( szTemp, "/");
			}
		}

		pState->setAnimName( szTemp);
		I3TRACE( "PATH : %s\n", szTemp);
	}
}

void CPanelAIGraph::OnBnClickedBtnAdjust()
{
	INT32 i;
	AI * pAI = m_View.getAI();
	GICDiagramAI * pDiagram = pAI->getDiagram();

	for( i = 0; i < pDiagram->getShapeCount(); i++)
	{
		GICShapeAI * pShape = (GICShapeAI *) pDiagram->getShape( i);
		i3AIState * pState = pShape->getAIState();

		if( (pState->getStyle() & I3_AISTATE_STYLE_ANIM_TEMPLATE) == 0)
			continue;

		AdjustAnimPath( pState);
	}

	m_View.Invalidate();
}


void CPanelAIGraph::OnBnClickedBtnCheckTemplate()
{
	AI * pAI = m_View.getAI();
	INT32 i, j, k, cntChara;
	char szFull[ MAX_PATH];
	GICDiagramAI * pDiagram = pAI->getDiagram();
	CHARA_INFO * pDefFemale = NULL, * pDefMale = NULL;
	bool bFind;

	// 기본 남/녀 캐릭터 정보를 찾는다.
	for( i = 0, cntChara = 0; s_CharaList[i].m_szName[0] != 0; i++, cntChara++)
	{
		if( s_CharaList[i].m_bDefault)
		{
			if( s_CharaList[i].m_bFemale)
				pDefFemale = &s_CharaList[i];
			else
				pDefMale = &s_CharaList[i];
		}
	}

	I3ASSERT( pDefFemale != NULL);
	I3ASSERT( pDefMale != NULL);


	for( i = 0; i < pDiagram->getShapeCount(); i++)
	{
		GICShapeAI * pShape = (GICShapeAI *) pDiagram->getShape( i);
		i3AIState * pState = pShape->getAIState();

		if( (pState->getStyle() & I3_AISTATE_STYLE_ANIM_TEMPLATE) == 0)
		{
			pShape->setErrorState( false);
			continue;
		}

		for( j = 0; j < cntChara; j++)
		{
			bFind = false;

			for( k = 0; k < 2; k++)
			{
				if( i3System::isAbsolutePath( pState->getAnimName()))
					i3::snprintf( szFull, sizeof(szFull), "%s", pState->getAnimName());
				else
					i3::snprintf( szFull, sizeof(szFull), "%s/%s", i3ResourceFile::GetWorkingFolder(), pState->getAnimName());

				if( k == 0)
					i3::generic_string_ireplace_all( szFull, "%c", s_CharaList[j].m_szName);
				else
				{
					if( s_CharaList[j].m_bFemale)
						i3::generic_string_ireplace_all( szFull, "%c", pDefFemale->m_szName);
					else
						i3::generic_string_ireplace_all( szFull, "%c", pDefMale->m_szName);
				}

				if( i3System::IsFile( szFull))
				{
					I3TRACE( "OK : %s\n", szFull);
					bFind = true;
					break;
				}
			}

			if( bFind == false)
			{
				pShape->setErrorState( true);

				I3TRACE( "--- ERROR : %s\n", szFull);
			}
			else
			{
				pShape->setErrorState( false);
			}
		}
	}

	m_View.Invalidate();
}

void CPanelAIGraph::OnBnClickedBtnReplace()
{
	CDlg_Replace dlg;

	if( dlg.Execute( (const char *) m_szSrc, (const char *) m_szDest) == false)
		return;

	i3::safe_string_copy( m_szSrc, dlg.getSrcString(), 256 );
	i3::safe_string_copy( m_szDest, dlg.getDestString(), 256 );


	INT32 i;
	AI * pAI = m_View.getAI();
	GICDiagramAI * pDiagram = pAI->getDiagram();

	for( i = 0; i < pDiagram->getShapeCount(); i++)
	{
		GICShapeAI * pShape = (GICShapeAI *) pDiagram->getShape( i);
		i3AIState * pState = pShape->getAIState();

		if( (pState->getStyle() & I3_AISTATE_STYLE_ANIM_TEMPLATE) == 0)
			continue;

		char conv[512];

		i3::safe_string_copy( conv, pState->getAnimName(), 512 );
		i3::generic_string_ireplace_all( conv, m_szSrc, m_szDest);

		if( !i3::generic_is_equal(conv, pState->getAnimName() ) )
		{
			pState->setAnimName( conv);
		}
	}

	m_View.Invalidate();
}
