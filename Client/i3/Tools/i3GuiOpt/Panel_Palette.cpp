// Panel_Palette.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiOpt.h"
#include "Panel_Palette.h"
#include "GlobalVar.h"

// CPanel_Palette dialog

IMPLEMENT_DYNAMIC(CPanel_Palette, i3TDKDialogBase)

CPanel_Palette::CPanel_Palette(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanel_Palette::IDD, pParent)
{

}

CPanel_Palette::~CPanel_Palette()
{
}

void CPanel_Palette::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_OPT, m_TREE_OPT);
}

void CPanel_Palette::StartProcessing(void)
{
	m_TREE_OPT.EnableWindow( FALSE);
}

void CPanel_Palette::EndProcessing(void)
{
	m_TREE_OPT.EnableWindow( TRUE);
}

BEGIN_MESSAGE_MAP(CPanel_Palette, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_OPT, &CPanel_Palette::OnNMDblclkTreeOpt)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_OPT, &CPanel_Palette::OnTvnBegindragTreeOpt)
	ON_NOTIFY(TVN_GETINFOTIP, IDC_TREE_OPT, &CPanel_Palette::OnTvnGetInfoTipTreeOpt)
END_MESSAGE_MAP()


// CPanel_Palette message handlers

BOOL CPanel_Palette::OnInitDialog()
{


	i3TDKDialogBase::OnInitDialog();

	{
		CImageList * pImageList = g_pTDKGlobalRes->m_pTDKImageList;

		m_TREE_OPT.SetImageList( pImageList, TVSIL_NORMAL);

		for(INT32 i = 0; i < i3SceneOptimizer::CLASS_MAX; i++)
		{
			m_hRoot[ g_ClassTbl[i].m_Class]		= m_TREE_OPT.InsertItem( g_ClassTbl[i].m_szName,	g_ClassTbl[i].m_idxImage, g_ClassTbl[i].m_idxImage, TVI_ROOT);
		}
	}

	{
		i3::vector<i3ClassMeta*> list;
		i3ClassMeta * pMeta;

		i3SceneOptimizer::static_meta()->get_all_derived_metas( list);

		for(size_t i = 0; i < list.size(); i++)
		{
			const char * pszName;
			pMeta = list[i];

			if( pMeta->get_class_meta_style() == i3::cms_abstract)	// IsAbstractClass())
				continue;

			i3SceneOptimizer * pOpt = (i3SceneOptimizer *) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
			pOpt->AddRef();

			if( pOpt->isInternalOnly() == false)
			{
				pszName = pOpt->getDescName();

				INT32 iClass = pOpt->getClass();
				INT32 idx = FindClassTblByClass( iClass);
				I3ASSERT( idx != -1);

				if( idx == 6)
				{
					I3TRACE("%s---------------\n", pszName);
				}

				HTREEITEM hItem = m_TREE_OPT.InsertItem( pszName, g_ClassTbl[idx].m_idxImage, g_ClassTbl[idx].m_idxImage, m_hRoot[ iClass]);

				m_TREE_OPT.SetItemData( hItem, (DWORD_PTR) pMeta);
			}

			pOpt->Release();
		}

		// 전체 Root node들을 Expand 해둔다.
		for( INT32 i = 0; i < i3SceneOptimizer::CLASS_MAX; i++)
		{
			m_TREE_OPT.Expand( m_hRoot[ i], TVE_EXPAND);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanel_Palette::OnOK()
{
}

void CPanel_Palette::OnCancel()
{
}

void CPanel_Palette::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TREE_OPT.m_hWnd))
	{
		m_TREE_OPT.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanel_Palette::OnNMDblclkTreeOpt(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem = m_TREE_OPT.GetSelectedItem();

	if( hItem == NULL)
		return;

	i3ClassMeta * pMeta = (i3ClassMeta *) m_TREE_OPT.GetItemData( hItem);

	if( pMeta != NULL)
	{
		i3SceneOptimizer * pOpt = (i3SceneOptimizer *) pMeta->create_instance();
			// CREATEINSTANCE( pMeta);

		g_pOptConfig->addOpt( pOpt);
	}

	*pResult = 0;
}

void CPanel_Palette::OnTvnBegindragTreeOpt(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CPanel_Palette::OnTvnGetInfoTipTreeOpt(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVGETINFOTIP pTip = reinterpret_cast<LPNMTVGETINFOTIP>(pNMHDR);

	i3SceneOptimizer * pOpt;
	i3ClassMeta * pMeta;

	pMeta = (i3ClassMeta *) pTip->lParam;
	if( pMeta == NULL)
		return;

	pOpt = (i3SceneOptimizer *) pMeta->create_instance();	// CREATEINSTANCE( pMeta);
	if( pOpt == NULL)
		return;

	pOpt->AddRef();

	const char * pszHelp = pOpt->getHelpText();

	if( pszHelp != NULL)
	{
		i3::string_ncopy_nullpad( pTip->pszText, pszHelp, pTip->cchTextMax);
	}

	pOpt->Release();
	
	*pResult = 0;
}
