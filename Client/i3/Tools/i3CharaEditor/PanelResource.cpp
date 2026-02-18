// PanelResource.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "PanelResource.h"
#include ".\panelresource.h"
#include "GlobalVar.h"
#include "DlgRenameFolder.h"
#include "DlgAddAIStateToChilds.h"

#include "i3Base/string/ext/extract_filename.h"

// CPanelResource dialog

IMPLEMENT_DYNAMIC(CPanelResource, CDialog)
CPanelResource::CPanelResource(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelResource::IDD, pParent)
{
	m_bDragging = false;
	m_pDragObject = NULL;
	m_pTreeCtrlImageList = NULL;
}

CPanelResource::~CPanelResource()
{
	if( m_pTreeCtrlImageList != NULL)
	{
		delete m_pTreeCtrlImageList;
		m_pTreeCtrlImageList = NULL;
	}
}

void CPanelResource::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RSC_TREE, m_TreeCtrl);
}

HTREEITEM CPanelResource::getRootItem( i3ResourceObject * pRes)
{
	if( i3::kind_of<CChara*>(pRes)) //->IsTypeOf( CChara::static_meta()))
	{
		return m_hRootItem[ RSC_CHARA];
	}
	else if( i3::same_of<i3SceneGraphInfo*>(pRes)) //->IsExactTypeOf( i3SceneGraphInfo::static_meta()))
	{
		return m_hRootItem[ RSC_SCENEGRAPH];
	}
	else if( i3::kind_of<CLOD*>(pRes)) //->IsTypeOf( CLOD::static_meta()))
	{
		return m_hRootItem[ RSC_LOD] ;
	}
	else if( i3::kind_of<i3Animation*>(pRes)) //->IsTypeOf( i3Animation::static_meta()))
	{
		return m_hRootItem[ RSC_ANIMATION];
	}
	else if( i3::kind_of<AI*>(pRes)) //->IsTypeOf( AI::static_meta()))
	{
		AI * pAI = (AI *) pRes;
		i3GameNode * pParent = pAI->getAIContext()->getParent();

		if( pParent)
		{
			if( i3::kind_of<i3AIContext*>(pParent)) //->IsTypeOf( i3AIContext::static_meta()))
			{
				INT32 idxParent = g_pChara->FindAIByAIContext( (i3AIContext *) pParent); 
				I3ASSERT( idxParent != -1);

				AI * pParentAI = g_pChara->getAI( idxParent);

				// Parent를 찾는다.
				HTREEITEM hParent = i3TDK::FindTreeItemByData( &m_TreeCtrl, pParentAI, m_hRootItem[ RSC_AI]);
				I3ASSERT( hParent != NULL);

				return hParent;
			}
		}

		return m_hRootItem[ RSC_AI] ;
	}
	else if( i3::kind_of<i3Sound*>(pRes)) //->IsTypeOf( i3Sound::static_meta()))
	{
		return m_hRootItem[ RSC_SOUND];
	}

	return TVI_ROOT;
}

void	CPanelResource::GetInfoString( char * pszBuf, i3ResourceObject * pRes)
{
	if( i3::same_of<i3SceneGraphInfo*>(pRes)) //->IsExactTypeOf( i3SceneGraphInfo::static_meta()))
	{
	//	i3String::SplitFileName( pRes->GetName(), pszBuf, FALSE);
		i3::extract_filetitle( pRes->GetName(), pszBuf);
	}
	else if( i3::same_of<CLOD*>(pRes)) //->IsExactTypeOf( CLOD::static_meta()))
	{
		sprintf( pszBuf, "Lv %d [%s]", g_pChara->FindLOD( (CLOD *) pRes), pRes->GetName());
	}
	else if( i3::kind_of<i3Animation*>(pRes)) //->IsTypeOf( i3Animation::static_meta()))
	{
		strcpy( pszBuf, pRes->GetName());
	}
	else
	{
	//	i3String::SplitFileName( pRes->GetName(), pszBuf, FALSE);
		i3::extract_filetitle(pRes->GetName(), pszBuf);
	}
}

void CPanelResource::UpdateRes( HTREEITEM hItem, i3ResourceObject * pRes)
{
	char szName[ MAX_PATH];

	GetInfoString( szName, pRes);

	m_TreeCtrl.SetItemText( hItem, szName);
}

HTREEITEM	CPanelResource::AddRes( HTREEITEM hParentItem, i3ResourceObject * pRes)
{
	HTREEITEM hItem;
	TVINSERTSTRUCT item;

	if( hParentItem == NULL)
	{
		hParentItem = getRootItem( pRes);
	}

	memset( &item, 0, sizeof(item));
	item.hInsertAfter = TVI_LAST;
	item.hParent = hParentItem;
	item.item.lParam = (LPARAM) pRes;
	item.item.mask = TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT;
	item.item.pszText = "";

	hItem = m_TreeCtrl.InsertItem( &item);
	UpdateRes( hItem, pRes);

	// Root Item의 + 표시 Update
	{
		TVITEM info;

		info.mask = TVIF_CHILDREN | TVIF_HANDLE;
		info.hItem = hParentItem;
		info.cChildren = 1;

		m_TreeCtrl.SetItem( &info);
	}

	m_TreeCtrl.EnsureVisible( hItem);
	//m_TreeCtrl.SelectItem( hItem);

	return hItem;
}

HTREEITEM	CPanelResource::FindRes( i3ResourceObject * pRes)
{
	HTREEITEM hRoot;

	hRoot = getRootItem( pRes);

	return i3TDK::FindTreeItemByData( &m_TreeCtrl, pRes, hRoot);
}

void	CPanelResource::RemoveRes( i3ResourceObject * pRes)
{
	HTREEITEM hItem;

	hItem = FindRes( pRes);
	//I3ASSERT( hItem != NULL);			새캐릭터 또는 종료시 다운 버그 막기 by KOMET

	if( hItem != NULL)
		RemoveRes( hItem);
}

void	CPanelResource::RemoveRes( HTREEITEM hItem)
{
	m_TreeCtrl.DeleteItem( hItem);
}

static BOOL _RemoveAllResProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	i3ClassMeta * pSrcMeta = (i3ClassMeta *) pData;
	i3ResourceObject * pRes;

	pRes = (i3ResourceObject *) pCtrl->GetItemData( hItem);
	if( pRes == NULL)
		return TRUE;

	if( pSrcMeta != NULL)
	{
		if( pRes->kind_of( pSrcMeta))
			pCtrl->DeleteItem( hItem);
	}
	else
	{
		pCtrl->DeleteItem( hItem);
	}

	return TRUE;
}

void	CPanelResource::RemoveAllRes( i3ClassMeta * pType)
{
	m_TreeCtrl.SetItemData( m_hRootItem[ RSC_CHARA], NULL);

	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _RemoveAllResProc, pType);

	m_TreeCtrl.SetItemData( m_hRootItem[ RSC_CHARA], (DWORD_PTR) g_pChara);
}

void	CPanelResource::UpdateAllRes(void)
{
	INT32 i;

	LockWindowUpdate();

	RemoveAllRes( NULL);

	// Scene Graph
	for( i = 0; i < g_pChara->getSceneGraphCount(); i++)
	{
		i3SceneGraphInfo * pInfo = g_pChara->getSceneGraph( i);

		AddRes( NULL, pInfo);
	}

	// LOD
	for( i = 0; i < g_pChara->getLODCount(); i++)
	{
		CLOD * pLOD = g_pChara->getLOD( i);

		AddRes( NULL, pLOD);
	}

	// Animation
	for( i = 0; i < g_pChara->getAnimCount(); i++)
	{
		i3Animation * pAnim = g_pChara->getAnim( i);

		AddRes( NULL, pAnim);
	}

	// Sound
	for( i = 0; i < g_pChara->getSoundCount(); i++)
	{
		i3Sound * pSound = g_pChara->getSound( i);

		AddRes( NULL, pSound);
	}

	// Texture

	// AI
	UpdateAllAI();

	UpdateChara();

	this->UnlockWindowUpdate();
}

void	CPanelResource::UpdateChara(void)
{
	HTREEITEM hItem = m_hRootItem[ RSC_CHARA];

	m_TreeCtrl.SetItemData( hItem, (DWORD_PTR) g_pChara);

	if( g_pChara->hasName() == false)
	{
		m_TreeCtrl.SetItemText( hItem, "<NONAME : Character>");
	}
	else
	{
		m_TreeCtrl.SetItemText( hItem, g_pChara->GetName());
	}
}

void	CPanelResource::UpdateAllLOD(void)
{
	INT32 i;

	RemoveAllRes( CLOD::static_meta());

	// LOD
	for( i = 0; i < g_pChara->getLODCount(); i++)
	{
		CLOD * pLOD = g_pChara->getLOD( i);

		AddRes( NULL, pLOD);
	}
}

bool CPanelResource::_AITraverseProc( i3GameNode * pNode, i3GameNodeTraversal * pTr, void * pUserData)
{
	CPanelResource * pPanel = (CPanelResource *) pUserData;

	if( !i3::kind_of<i3AIContext*>(pNode)) //->IsTypeOf( i3AIContext::static_meta()) == FALSE)
		return true;

	INT32 idx = g_pChara->FindAIByAIContext( (i3AIContext *) pNode);
	I3ASSERT( idx != -1);

	AI * pAI = g_pChara->getAI( idx);

	pPanel->AddRes( NULL, pAI);

	return true;
}

void CPanelResource::_Rec_UpdateAI( HTREEITEM hParentItem, i3GameNode * pNode)
{
	HTREEITEM hItem = hParentItem;

	if( i3::kind_of<i3AIContext*>(pNode)) //->IsTypeOf( i3AIContext::static_meta()))
	{
		i3AIContext * pCtx = (i3AIContext *) pNode;
		AI * pAI;
		INT32 idx;

		idx = g_pChara->FindAIByAIContext( pCtx);
		I3ASSERT( idx != -1);

		pAI = g_pChara->getAI( idx);

		hItem = AddRes( hParentItem, pAI);
	}

	i3GameNode * pChild = pNode->getFirstChild();

	while( pChild != NULL)
	{
		_Rec_UpdateAI( hItem, pChild);

		pChild = pChild->getNext();
	}
}

void CPanelResource::UpdateAllAI(void)
{
	_Rec_UpdateAI( m_hRootItem[ RSC_AI], g_pChara->geti3Chara());
}

BEGIN_MESSAGE_MAP(CPanelResource, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_RSC_TREE, OnTvnSelchangedRscTree)
	ON_NOTIFY(NM_RCLICK, IDC_RSC_TREE, OnNMRclickRscTree)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_RSC_TREE, OnTvnBeginlabeleditRscTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_RSC_TREE, OnTvnEndlabeleditRscTree)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_RSC_TREE, OnTvnBegindragRscTree)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CPanelResource message handlers

struct _tagRootInfo
{
	char m_szName[32];
	INT32	m_Image;
} s_RootInfo[] =
{
	{	"Scene Graph",	10	},
	{	"LOD",			0	},
	{	"Animation",	0	},
	{	"Sound",		14	},
	{	"Texture",		16	},
	{	"AI",			0	}
};

BOOL CPanelResource::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_hCursor[0] = ::LoadCursor( NULL, IDC_ARROW);
		m_hCursor[1] = ::LoadCursor( NULL, IDC_NO);
	}

	{
		INT32 i;

		m_hRootItem[0] = m_TreeCtrl.InsertItem( "Character", 0, 0);

		for( i = 1; i < RSC_TYPECOUNT; i++)
		{
			m_hRootItem[i] = m_TreeCtrl.InsertItem( s_RootInfo[i - 1].m_szName, s_RootInfo[i - 1].m_Image, 0, m_hRootItem[0]);
		}

		m_TreeCtrl.Expand( m_hRootItem[0], TVE_EXPAND);

		m_TreeCtrl.SetBkColor( g_pTDKGlobalRes->getColor( I3TDK_COLOR_FORE_APP_BKGND));
		m_TreeCtrl.SetTextColor( g_pTDKGlobalRes->getColor( I3TDK_COLOR_TEXT_NORMAL));

		{
			m_pTreeCtrlImageList = new CImageList;

			m_pTreeCtrlImageList->Create( 16, 16, ILC_COLOR32, 32, 12);

			i3TDK::AddTDKImageListTo( m_pTreeCtrlImageList);

			m_TreeCtrl.SetImageList( m_pTreeCtrlImageList, TVSIL_NORMAL);
		}
	}

	i3TDK::RegisterUpdate( m_hWnd, CChara::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, i3SceneGraphInfo::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, CLOD::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, i3Animation::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, AI::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, i3Sound::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CPanelResource::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelResource::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( i3::same_of<CChara*>(pInfo->m_pMeta)) //->IsExactTypeOf( CChara::static_meta()))
	{
		if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
		{
			RemoveAllRes();
		}
		else if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
		{
			UpdateAllRes();
		}
		else if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
		{

		}
	}
	else if( i3::same_of<i3SceneGraphInfo*>(pInfo->m_pMeta)) //->IsExactTypeOf( i3SceneGraphInfo::static_meta()))
	{
		if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
		{
			AddRes( NULL, (i3ResourceObject *) pInfo->m_pObject);
		}
		else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
		{
			if( pInfo->m_pObject == NULL)
			{
				RemoveAllRes( i3SceneGraphInfo::static_meta());
			}
			else
			{
				RemoveRes( (i3ResourceObject *) pInfo->m_pObject);
			}
		}
	}
	else if( i3::same_of<CLOD*>(pInfo->m_pMeta)) //->IsExactTypeOf( CLOD::static_meta()))
	{
		if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
		{
			UpdateAllLOD();
		}
		else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
		{
			UpdateAllLOD();
		}
	}
	else if( i3::kind_of<i3Animation*>(pInfo->m_pMeta)) //->IsTypeOf( i3Animation::static_meta()))
	{
		if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
		{
			AddRes( NULL, (i3ResourceObject *) pInfo->m_pObject);
		}
		else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
		{
			if( pInfo->m_pObject == NULL)
			{
				RemoveAllRes( i3Animation::static_meta());
			}
			else
			{
				RemoveRes( (i3ResourceObject *) pInfo->m_pObject);
			}
		}
		else if( pInfo->m_Event & I3_TDK_UPDATE_EDIT )
		{
			RemoveRes( (i3ResourceObject *) pInfo->m_pObject);
			AddRes( NULL, (i3ResourceObject *) pInfo->m_pObject);
		}
	}
	else if( i3::kind_of<AI*>(pInfo->m_pMeta)) //->IsTypeOf( AI::static_meta()))
	{
		if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
		{
			AddRes( NULL, (i3ResourceObject *) pInfo->m_pObject);
		}
		else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
		{
			if( pInfo->m_pObject == NULL)
			{
				RemoveAllRes( AI::static_meta());
			}
			else
			{
				RemoveRes( (i3ResourceObject *) pInfo->m_pObject);
			}
		}
	}
	else if( i3::kind_of<i3Sound*>(pInfo->m_pMeta)) //->IsTypeOf( i3Sound::static_meta()))
	{
		if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
		{
			AddRes( NULL, (i3ResourceObject *) pInfo->m_pObject);
		}
		else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
		{
			if( pInfo->m_pObject == NULL)
			{
				RemoveAllRes( i3Sound::static_meta());
			}
			else
			{
				RemoveRes( (i3ResourceObject *) pInfo->m_pObject);
			}
		}
	}

	if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
	{
		I3ASSERT( pInfo->m_pObject != NULL);

		HTREEITEM hItem = FindRes( (i3ResourceObject *) pInfo->m_pObject);

		if( hItem != NULL)
		{
			UpdateRes( hItem, (i3ResourceObject *) pInfo->m_pObject);
		}
	}
}


LRESULT CPanelResource::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CPanelResource::OnOK()
{
}

void CPanelResource::OnCancel()
{
}


void CPanelResource::OnTvnSelchangedRscTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	i3ElementBase * pObj;

	if( pNMTreeView->itemOld.hItem != NULL)
	{
		pObj = (i3ElementBase *) pNMTreeView->itemOld.lParam;

		if( pObj != NULL)
		{
			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, pObj);
		}
	}

	if( pNMTreeView->itemNew.hItem != NULL)
	{
		pObj = (i3ElementBase *) pNMTreeView->itemNew.lParam;

		if( pObj != NULL)
		{
			if( i3::kind_of<CLOD*>(pObj)) //->IsTypeOf( CLOD::static_meta()))
			{
				CLOD* pCurrentLOD = (CLOD*)pObj;
				g_pChara->getBody()->removeStyle(I3BS_LOD_AUTO);
				for (int i = 0; i < g_pChara->getLODCount(); ++i)
				{
					CLOD* pLOD = g_pChara->getLOD(i);
					if (i3::generic_is_iequal(pLOD->GetName(), pCurrentLOD->GetName()) )
					{
						g_pChara->getBody()->setCurrentLevel(i);
					}
				}
				//해당 LOD선택시.. 자동LOD를 없애고
				//수동으로 LOD를 조절
// 				const char* szName = pLOD->GetName();
// 				INT32 nPos = i3String::Contain(szName,"LOD");
// 				INT32 nNum = atoi(&szName[nPos + 3]);
// 				g_pChara->getBody()->removeStyle(I3BS_LOD_AUTO);
// 				g_pChara->getBody()->setCurrentLevel(nNum);
			}

			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pObj);
		}
	}

	*pResult = 0;
}

void	CPanelResource::OnDelete( HTREEITEM hItem, i3ElementBase * pObj)
{
	if( pObj != NULL)
	{
		if( i3::kind_of<CChara*>(pObj)) //->IsTypeOf( CChara::static_meta()))
		{
		}
		else if( i3::kind_of<i3SceneGraphInfo*>(pObj)) //->IsTypeOf( i3SceneGraphInfo::static_meta()))
		{
			int rv = AfxMessageBox( "Scene-Graph를 사용하는 LOD도 함께 제거될 수 있다. 그래도 진행하시겠습니까?", MB_YESNO);

			if( rv == IDYES)
			{
				g_pChara->RemoveSceneGraph( (i3SceneGraphInfo *) pObj);
			}
		}
		else if( i3::kind_of<CLOD*>(pObj)) //->IsTypeOf( CLOD::static_meta()))
		{
			g_pChara->RemoveLOD( (CLOD *) pObj);
		}
		else if( i3::kind_of<i3Animation*>(pObj)) //->IsTypeOf( i3Animation::static_meta()))
		{
			g_pChara->RemoveAnim( (i3Animation *) pObj);
		}
		else if( i3::kind_of<AI*>(pObj)) //->IsTypeOf( AI::static_meta()))
		{
			g_pChara->RemoveAI( (AI *) pObj);
		}
		else if( i3::kind_of<i3Sound*>(pObj)) //->IsTypeOf( i3Sound::static_meta()))
		{
			g_pChara->RemoveSound( (i3Sound *) pObj);
		}
	}
	else
	{
		if( hItem == m_hRootItem[RSC_CHARA])
		{
		}
		else if( hItem == m_hRootItem[RSC_SCENEGRAPH])
		{
			int rv = AfxMessageBox( "Scene-Graph를 사용하는 LOD도 함께 제거될 수 있다. 그래도 진행하시겠습니까?", MB_YESNO);

			if( rv == IDYES)
			{
				g_pChara->RemoveAllSceneGraph();
			}
		}
		else if( hItem == m_hRootItem[RSC_LOD])
		{
			g_pChara->RemoveAllLOD();
		}
		else if( hItem == m_hRootItem[RSC_ANIMATION])
		{
			g_pChara->RemoveAllAnim();
		}
		else if( hItem == m_hRootItem[RSC_SOUND])
		{
			g_pChara->RemoveAllSound();
		}
		else if( hItem == m_hRootItem[RSC_TEXTURE])
		{
		}
		else if( hItem == m_hRootItem[RSC_AI])
		{
			g_pChara->RemoveAllAI();
		}
	}
}

void	CPanelResource::OnNewAI( HTREEITEM hItem, i3ElementBase * pObj)
{
	AI * pParentAI;

	if(!i3::kind_of<AI*>(pObj)) //->IsTypeOf( AI::static_meta()) == FALSE)
	{
		pParentAI = NULL;
	}
	else
	{
		pParentAI = (AI *) pObj;
	}

	AI * pAI = AI::new_object_ref();

	pAI->SetName( "Nonamed");
	g_pChara->AddAI( pParentAI, pAI);
}

void CPanelResource::OnAddLODFromSg( i3ElementBase * pObj)
{
	I3ASSERT( i3::kind_of<i3SceneGraphInfo*>(pObj)); //->IsTypeOf( i3SceneGraphInfo::static_meta()));
	i3SceneGraphInfo * pInfo = (i3SceneGraphInfo *) pObj;

	{
		i3::vector<CLOD*> List;

		g_pChara->FindLODBySg( pInfo, List);

		if( List.size() > 0)
		{
			int rv = AfxMessageBox( "이미 해당 Scene-Graph를 사용하는 LOD가 있다. 그래도 만들래?", MB_YESNO);
			if( rv == IDNO)
				return;
		}

		CLOD * pLOD = CLOD::new_object_ref();

		pLOD->SetName( pInfo->GetName());
		pLOD->SetSceneGraph( pInfo, 0, true);

		g_pChara->AddLOD( pLOD);
	}
}

void CPanelResource::OnLODMoveUp( HTREEITEM hItem)
{
	CLOD * pLOD;
	INT32 idx;

	pLOD = (CLOD *) m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( i3::kind_of<CLOD*>(pLOD)); //->IsTypeOf( CLOD::static_meta()));

	idx = g_pChara->FindLOD( pLOD);
	I3ASSERT( idx != -1);

	g_pChara->MoveLOD( idx, -1);

	UpdateAllLOD();
}

void CPanelResource::OnLODMoveDown( HTREEITEM hItem)
{
	CLOD * pLOD;
	INT32 idx;

	pLOD = (CLOD *) m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( i3::kind_of<CLOD*>(pLOD)); //->IsTypeOf( CLOD::static_meta()));

	idx = g_pChara->FindLOD( pLOD);
	I3ASSERT( idx != -1);

	g_pChara->MoveLOD( idx, 1);

	UpdateAllLOD();
}

void CPanelResource::OnLODSort(void)
{
	g_pChara->SortLOD();

	UpdateAllLOD();
}

bool _BuildAIList( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
//	List * pList = (List *) pUserData;
	i3::vector<i3PersistantElement*>& List = *reinterpret_cast<i3::vector<i3PersistantElement*>*>(pUserData);

	if( i3::kind_of<i3AIContext*>(pNode)) //->IsTypeOf( i3AIContext::static_meta()))
	{
		INT32 idx;

		// AI context에 대응하는 AI Class를 찾아 등록한다.
		idx = g_pChara->FindAIByAIContext( (i3AIContext *) pNode);
		if( idx != -1)
		{
			List.push_back( g_pChara->getAI( idx));
		}
	}

	return true;
}

static void _MakeName( char * pszStr)
{
	while( *pszStr != 0)
	{
		if( (*pszStr == ':') || (*pszStr == '/'))
			*pszStr = '_';
		
		pszStr++;
	}
}

void CPanelResource::OnAIExport( HTREEITEM hItem, i3ElementBase * pObj)
{
	if( ! i3::kind_of<AI*>(pObj)) //->IsTypeOf( AI::static_meta()))
		return;

	char szPath[ MAX_PATH];
	AI * pAI = (AI *) pObj;

	pAI->GetFullAIPath( szPath);

	_MakeName( szPath);

	CFileDialog	Dlg( FALSE, "i3AI", szPath, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3AI File(*.i3AI)|*.i3AI||", this);

	if( Dlg.DoModal() != IDOK) return;
	
	i3ResourceFile file;
	i3::vector<i3PersistantElement*> list;
	

	{
		i3GameNodeTraversal tr;

		tr.setUserData( &list);
		tr.setUserProc( _BuildAIList);
		tr.Traverse( pAI->getAIContext());
	}

	file.setKeyObject( pAI);
	file.SetObjects( list);

	if( file.Save( LPCTSTR( Dlg.GetPathName())) == STREAM_ERR)
	{
		CharaEditorNotifyBox( m_hWnd, "%s AI 파일을 저장할 수 없음.", LPCTSTR( Dlg.GetPathName()));
		return;
	}
}

void CPanelResource::OnAIImport( HTREEITEM hItem, i3ElementBase * pObj)
{

	CFileDialog	Dlg( TRUE, "i3AI", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3AI File(*.i3AI)|*.i3AI||", this);

	if( Dlg.DoModal() != IDOK) return;

	i3ResourceFile file;

	if( file.Load( LPCTSTR( Dlg.GetPathName())) == STREAM_ERR)
	{
		CharaEditorNotifyBox( m_hWnd, "%s AI 파일을 읽을 수 없음.", LPCTSTR( Dlg.GetPathName()));
		return;
	}

	INT32 i;
	i3PersistantElement * pPersistObj;
	AI * pAI, * pParentAI = (AI *) pObj;

	for( i = 0; i < file.GetObjectCount(); i++)
	{
		pPersistObj = file.GetObject( i);

		if( i3::kind_of<AI*>(pPersistObj)) //->IsTypeOf( AI::static_meta()))
		{
			pAI = (AI *) pPersistObj;

			if( file.getKeyObject() == pPersistObj)
			{
				g_pChara->AddAI( pParentAI, pAI, true);
			}
			else
			{
				g_pChara->AddAI( NULL, pAI, false);
			}
		}
	}

	pAI = (AI *) file.getKeyObject();

	//_Rec_UpdateAI( hItem, pAI->getAIContext());
}

INT32 CPanelResource::DumpAIContext( i3AIContext * pContext, i3::vector<i3::rc_string>& List)
{
	if( pContext == NULL )		return 0;

	i3AIContext * pCurContext = pContext;
	INT32 cnt = g_pChara->DumpAIState( pCurContext, List);
	
	i3GameNode * pNode = pCurContext->getFirstChild();
	while( pNode != NULL)
	{
		if( i3::same_of<i3AIContext*>(pNode)) //->IsExactTypeOf( i3AIContext::static_meta()))
		{
			cnt += DumpAIContext( (i3AIContext*) pNode, List);
		}

		pNode = pNode->getNext();
	}

	return cnt;	//	AIState count
}

void CPanelResource::OnDumpAI( HTREEITEM hItem, i3ElementBase * pObj)
{
	char temp[256] = "";
	AI * pAI = (AI*) pObj;

	//	루트면 통째로
	if(hItem == m_hRootItem[RSC_AI])
	{
		g_pChara->DumpAI();
		return;
	}

	if( pAI == NULL)		return;

	//	선택된 AI 이름
	char * pszName = "(null)";
	if( pAI->getAIContext()->hasName())
		pszName = (char*) pAI->getAIContext()->GetName();

	INT32 nAIStateCount = 0;

	//	덤프 파일 생성
	i3FileStream file;
	DeleteFile("DumpAI.txt");
	if( !file.Create("DumpAI.txt", STREAM_READ | STREAM_WRITE))
	{		
		sprintf( temp, "Dump에 실패했습니다. DumpAI.txt 파일을 생성할 수 없습니다.");

		I3PRINTLOG(I3LOG_WARN, temp);
		AfxMessageBox( temp);

		return;
	}

	//	AIContext를 재귀호출해서 Dump 스트링 수집
	i3::vector<i3::rc_string> list;
	nAIStateCount = DumpAIContext(pAI->getAIContext(), list);

	for(size_t cnt=0; cnt<list.size(); cnt++)
	{
		file.WriteLine( list[cnt].c_str());
	}

	list.clear();

	file.WriteLine("=======================================================\r\n");	

	sprintf( temp, "Name : %s\r\n", pszName);
	file.WriteLine(temp);
	
	sprintf( temp, "Total AIState count : %d\r\n", nAIStateCount);
	file.WriteLine(temp);

	file.WriteLine("=======================================================\r\n");

	file.Close();

	//	덤프파일을 메모장에 띄운다.
	WinExec("NotePad.exe DumpAI.txt", SW_SHOWNORMAL);
}

void CPanelResource::OnRenameFolder( )
{
	CDlgRenameFolder dlg;
	
	dlg.DoModal();
}

void CPanelResource::OnAddAIStateToChilds( HTREEITEM hItem, i3ElementBase * pObj)
{
	CDlgAddAIStateToChilds dlg;
	dlg.setObj( pObj);
	dlg.DoModal();
}

void CPanelResource::_AdjustPopupMenu( CMenu * pMenu, HTREEITEM hItem, i3ElementBase * pObj)
{
	if( pObj != NULL)
	{
		if( i3::kind_of<CChara*>(pObj)) //->IsTypeOf( CChara::static_meta()))
		{
		}
		else if( i3::kind_of<i3SceneGraphInfo*>(pObj)) //->IsTypeOf( i3SceneGraphInfo::static_meta()))
		{
			pMenu->AppendMenu( MF_STRING, ID_RSC_ADDLOD, "Add LOD...");
		}
		else if( i3::kind_of<CLOD*>(pObj)) //->IsTypeOf( CLOD::static_meta()))
		{
			pMenu->AppendMenu( MF_STRING, ID_RSC_LOD_MOVEUP, "Move up");
			pMenu->AppendMenu( MF_STRING, ID_RSC_LOD_MOVEDOWN, "Move down");
			pMenu->AppendMenu( MF_STRING, ID_RSC_LOD_SORT, "Sort by triangle count");
		}
		else if( i3::kind_of<i3Animation*>(pObj)) //->IsTypeOf( i3Animation::static_meta()))
		{
			pMenu->AppendMenu( MF_STRING, ID_RSC_OPTIMIZEANIM, "Optimize Animations");
			pMenu->AppendMenu( MF_STRING, ID_RSC_RENAMEFOLDER, "Rename Folder");
		}
		else if( i3::kind_of<AI*>(pObj)) //->IsTypeOf( AI::static_meta()))
		{
			pMenu->AppendMenu( MF_STRING, ID_RSC_NEWAI, "Add AI...");
			pMenu->AppendMenu( MF_STRING, ID_RSC_AI_EXPORT,	"Export...");
			pMenu->AppendMenu( MF_STRING, ID_RSC_AI_IMPORT,	"Import...");
			pMenu->AppendMenu( MF_STRING, ID_RSC_AI_DUMP,	"Dump AI...");
			pMenu->AppendMenu( MF_STRING, ID_RSC_ADDAISTATETOCHILDS, "AddAIStateToChilds");
		}
	}
	else
	{
		pMenu->EnableMenuItem( ID_RSC_DELETE, MF_BYCOMMAND | MF_DISABLED);

		if( hItem == m_hRootItem[RSC_CHARA])
		{
		}
		else if( hItem == m_hRootItem[RSC_SCENEGRAPH])
		{
			if( g_pChara->getSceneGraphCount())
				pMenu->EnableMenuItem( ID_RSC_DELETE, MF_BYCOMMAND | MF_ENABLED);
		}
		else if( hItem == m_hRootItem[RSC_LOD])
		{
			pMenu->AppendMenu( MF_STRING, ID_RSC_LOD_SORT, "Sort by triangle count");

			if( g_pChara->getLODCount())
				pMenu->EnableMenuItem( ID_RSC_DELETE, MF_BYCOMMAND | MF_ENABLED);
		}
		else if( hItem == m_hRootItem[RSC_ANIMATION])
		{
			pMenu->AppendMenu( MF_STRING, ID_RSC_OPTIMIZEANIM, "Optimize Animations");
			pMenu->AppendMenu( MF_STRING, ID_RSC_RENAMEFOLDER, "Rename Folder");

			if( g_pChara->getAnimCount())
				pMenu->EnableMenuItem( ID_RSC_DELETE, MF_BYCOMMAND | MF_ENABLED);
		}
		else if( hItem == m_hRootItem[RSC_SOUND])
		{
			if( g_pChara->getSoundCount())
				pMenu->EnableMenuItem( ID_RSC_DELETE, MF_BYCOMMAND | MF_ENABLED);
		}
		else if( hItem == m_hRootItem[RSC_TEXTURE])
		{
		}
		else if( hItem == m_hRootItem[RSC_AI])
		{
			pMenu->AppendMenu( MF_STRING, ID_RSC_NEWAI, "Add AI...");
			pMenu->AppendMenu( MF_STRING, ID_RSC_AI_IMPORT,	"Import...");
			pMenu->AppendMenu( MF_STRING, ID_RSC_AI_DUMP,	"Dump AI...");			

			if( g_pChara->getAICount())
				pMenu->EnableMenuItem( ID_RSC_DELETE, MF_BYCOMMAND | MF_ENABLED);
		}
	}
}

void CPanelResource::OnNMRclickRscTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hSel;
	CPoint pt, pt2;
	UINT flag, cmd;
	CMenu menu, * pPopup;
	i3ElementBase * pObj = NULL;

	GetCursorPos( &pt);

	// 오른쪽으로 선택된 Item을 선택시킨다.
	{
		pt2 = pt;
		m_TreeCtrl.ScreenToClient( &pt2);

		hSel = m_TreeCtrl.HitTest( pt2, &flag);

		if ((hSel != NULL) && (TVHT_ONITEM & flag))
		{
			m_TreeCtrl.Select( hSel, TVGN_CARET);

			pObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hSel);
		}
	}

	{
		menu.LoadMenu( IDR_POPUP_RSC);
		pPopup = menu.GetSubMenu( 0);
	}

	// 선택 Object의 종류에 따라 Menu Item을 추가..
	{
		_AdjustPopupMenu( pPopup, hSel, pObj);
	}

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_RSC_DELETE :		
			OnDelete( hSel, pObj);
			break;

		case ID_RSC_ADDLOD :
			OnAddLODFromSg( pObj);
			break;

		case ID_RSC_NEWAI :
			OnNewAI( hSel, pObj);
			break;

		case ID_RSC_LOD_MOVEUP :
			OnLODMoveUp( hSel);
			break;

		case ID_RSC_LOD_MOVEDOWN :
			OnLODMoveDown( hSel);
			break;

		case ID_RSC_LOD_SORT :
			OnLODSort();
			break;

		case ID_RSC_AI_EXPORT :
			OnAIExport( hSel, pObj);
			break;

		case ID_RSC_AI_IMPORT :
			OnAIImport( hSel, pObj);
			break;

		case ID_RSC_AI_DUMP:
			OnDumpAI( hSel, pObj);
			break;

		case ID_RSC_OPTIMIZEANIM :
			g_pChara->OptimizeAnim();
			break;

		case ID_RSC_RENAMEFOLDER :
			OnRenameFolder();
			break;

		case ID_RSC_ADDAISTATETOCHILDS :
			OnAddAIStateToChilds( hSel, pObj);
			break;
	}

	*pResult = 0;
}

void CPanelResource::OnTvnBeginlabeleditRscTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	i3ElementBase * pObj = (i3ElementBase *) pTVDispInfo->item.lParam;

	*pResult = TRUE;		// Canceling label editting.

	if( pObj != NULL)
	{
		if( i3::kind_of<CChara*>(pObj)) //->IsTypeOf( CChara::static_meta()))
		{
			*pResult = 0;
		}
		else if( i3::kind_of<AI*>(pObj)) //->IsTypeOf( AI::static_meta()))
		{
			*pResult = 0;
		}
	}
}

void CPanelResource::OnTvnEndlabeleditRscTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	i3ElementBase * pObj = (i3ElementBase *) pTVDispInfo->item.lParam;

	*pResult = 0;

	if((pObj != NULL) && (pTVDispInfo->item.pszText != NULL))
	{
		if( i3::kind_of<CChara*>(pObj)) //->IsTypeOf( CChara::static_meta()))
		{
			CChara * pChara = (CChara *) pObj;

			pChara->geti3Chara()->SetName( pTVDispInfo->item.pszText);
		}

		if( i3::kind_of<i3NamedElement*>(pObj)) //->IsTypeOf( i3NamedElement::static_meta()))
		{
			i3NamedElement * pNamedObj = (i3NamedElement *) pObj;

			pNamedObj->SetName( pTVDispInfo->item.pszText);
		}

		*pResult = TRUE;
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pObj);
	}
}

void CPanelResource::OnTvnBegindragRscTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	i3ElementBase * pObj;

	pObj = (i3ElementBase *) pNMTreeView->itemNew.lParam;

	if( i3::kind_of<AI*>(pObj)) //->IsTypeOf( AI::static_meta()))
	{
		CPoint pt = pNMTreeView->ptDrag;
		int cxCursor, cyCursor;

		cxCursor = 10;
		cyCursor = 10;

		ClientToScreen( &pt);

		SetCapture();

		m_bDragging = true;
		m_pDragObject = pObj;

		m_pDragImage = m_TreeCtrl.CreateDragImage( pNMTreeView->itemNew.hItem);
		m_pDragImage->BeginDrag( 0, CPoint( -cxCursor, -cyCursor));
		m_pDragImage->DragShowNolock( TRUE);
		m_pDragImage->DragEnter( &m_TreeCtrl, pt);

		m_hTargetItem = NULL;
		m_hDragItem = pNMTreeView->itemNew.hItem;
	}

	*pResult = 0;
}

void CPanelResource::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging)
	{
		CDC * pDC = m_TreeCtrl.GetDC();
		
		m_pDragImage->DragMove( point);

		m_TreeCtrl.ReleaseDC( pDC);

		// Cursor
		HTREEITEM hTarget = m_TreeCtrl.HitTest( point);
		HCURSOR hNewCursor = m_hCursor[1];

		if( m_hTargetItem != NULL)
		{
			m_TreeCtrl.SetItemState( m_hTargetItem, 0, TVIS_DROPHILITED);
			m_hTargetItem = NULL;
		}

		if( hTarget != NULL)
		{
			i3ElementBase * pObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hTarget);

			if(i3::kind_of<AI*>(pObj)) //->IsTypeOf( AI::static_meta()) )
			{
				// OK
				if( pObj != m_pDragObject)
				{
					hNewCursor = m_hCursor[0];
					m_hTargetItem = hTarget;
				}
			}
			else
			{
				CString str = m_TreeCtrl.GetItemText( hTarget);

				if( str == "AI")
				{
					// OK
					hNewCursor = m_hCursor[0];
					m_hTargetItem = hTarget;
				}
			}
		}

		if( m_hTargetItem != NULL)
		{
			m_TreeCtrl.SetItemState( m_hTargetItem, 0xFFFFFFFF, TVIS_DROPHILITED);
		}

		i3TDK::SetCursor( m_hWnd, hNewCursor);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CPanelResource::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging)
	{
		m_bDragging = false;
		ReleaseCapture();

		m_pDragImage->DragLeave( &m_TreeCtrl);
		m_pDragImage->EndDrag();

		delete m_pDragImage;

		if( m_hTargetItem != NULL)
		{
			CMenu menu, * pPopup;
			UINT32 cmd;

			menu.LoadMenu( MAKEINTRESOURCE( IDR_POPUP_AIDRAG));
			pPopup = menu.GetSubMenu( 0);

			ClientToScreen( &point);

			cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				point.x, point.y, (CWnd *) this, NULL);

			///////////////////////////////////////
				// AI에 대한 Drag & Drop만 지원한다.
				i3ElementBase * pObj = (i3ElementBase *) m_TreeCtrl.GetItemData( m_hTargetItem);

				I3ASSERT( m_pDragObject != NULL);
				AI * pSrcAI = (AI *) m_pDragObject;
				i3AIContext * pSrcCtx = pSrcAI->getAIContext();
			//////////////////////////////////////////////

			switch( cmd)
			{
			case  ID_AI_MOVE :
				{
					// Dragging하는 AI를 이동시킨다.
					i3TDK::MoveTreeItem( &m_TreeCtrl, m_hTargetItem, m_hDragItem);

					i3GameNode * pParent = pSrcCtx->getParent();
					I3ASSERT( pParent != NULL);

					pParent->RemoveChild( pSrcCtx);

					if( pObj == NULL)
					{
						// Chara에 붙인다.
						g_pChara->geti3Chara()->AddChild( pSrcCtx);
					}
					else 
					{
						I3ASSERT( i3::kind_of<AI*>(pObj)); //->IsTypeOf( AI::static_meta()));

						AI * pTargetAI = (AI *) pObj;
						i3AIContext * pTargetCtx = pTargetAI->getAIContext();

						pTargetCtx->AddChild( pSrcCtx);
					}
				}
				break;
			default:
				{
					// Dragging한 AI의 Clone을 생성해, Target AI에 붙인다.
					//i3AIContext * pNewCtx = _CreateCloneAITree( pSrcCtx);

					/*
					if( pObj == NULL)
					{
						// Chara에 붙인다.
						g_pChara->geti3Chara()->AddChild( pNewCtx);
					}
					else 
					{
						I3ASSERT( i3::kind_of<AI*>(pObj)); //->IsTypeOf( AI::static_meta()));

						AI * pTargetAI = (AI *) pObj;
						i3AIContext * pTargetCtx = pTargetAI->getAIContext();

						pTargetCtx->AddChild( pNewCtx);
					}

					UpdateAllAI();
					*/
				}
				break;
			}
		}
	}

	CDialog::OnLButtonUp(nFlags, point);
}

i3AIContext *	CPanelResource::_CreateCloneAITree( i3AIContext * pCtx)
{
	//i3AIContext * pNewCtx = i3AIContext::new_object_ref();

	//pCtx->CopyTo( pNewCtx);
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
#ifdef _USE_TOOLAM_
//////////////////////////////////////////////////////////////////////////

#include "ToolAM.h"
#include "ToolAM_Def_CharTool.h"

using namespace ToolAM::CharTool;
using namespace ToolAM::Util;


BOOL CPanelResource::AM_I3CHR_SetName(const char* name)
{
	HTREEITEM rt = m_TreeCtrl.GetRootItem();
	m_TreeCtrl.SetItemText(rt, name);

	if(g_pChara)
	{
		i3::rc_string nameStr;

		if(name)
			nameStr = name;

		g_pChara->geti3Chara()->SetName(nameStr);
		g_pChara->SetName(nameStr);
	}

	return TRUE;
}

// resource
BOOL CPanelResource::AM_I3CHR_AddI3SLOD_HERO(
	const char* workpath,  // c:\\media
	const char* subpath,   // equip/mask
	const char* name,	   // mask_soccer_Canary
	DWORD lodFlag,
	std::vector<std::string>* heroNames // AcidPol, Bella ..
)
{
	std::string path = GeneratePath(workpath, subpath, "Resource");
	std::string file = GenerateLODFileNameWithHero(name, "AcidPol", lodFlag);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
#endif // end of _USE_TOOLAM_
//////////////////////////////////////////////////////////////////////////