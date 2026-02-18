#include "StdAfx.h"
#include "WeaponResourceList.h"
#include "DlgWeaponEditor.h"

#include "i3sound/i3SoundGlobal.h"
#include "i3sound/i3SoundFMOD.h"
#include "i3sound/i3SoundPlayStateFMOD.h"
#include "i3sound/i3SoundContext.h"
#include "i3sound/i3SoundContextFMOD.h"
#include "i3Base/string/ext/extract_filename.h"


IMPLEMENT_DYNAMIC(cWeaponResourceList, CTreeCtrl)

BEGIN_MESSAGE_MAP(cWeaponResourceList, CTreeCtrl)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &cWeaponResourceList::OnTvnSelchanged)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, &cWeaponResourceList::OnTvnKeydown)
END_MESSAGE_MAP()

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

cWeaponResourceList::cWeaponResourceList()
{

}
void cWeaponResourceList::DoDataExchange(CDataExchange* pDX)
{
	CTreeCtrl::DoDataExchange(pDX);
}

void cWeaponResourceList::OnSetFocus(CWnd* pOldWnd)
{
	CTreeCtrl::OnSetFocus(pOldWnd);
	m_pParent->OnManualSetFocus();

	I3TRACE("cWeaponResourceList SerFocus\n");
}

void cWeaponResourceList::Create()
{
	g_HwndWeaponResList = GetSafeHwnd();

	m_hRootItem[0] = InsertItem( "Character", 0, 0);

	for( int i = 1; i < RSC_TYPECOUNT; i++)
	{
		m_hRootItem[i] = InsertItem( s_RootInfo[i - 1].m_szName, s_RootInfo[i - 1].m_Image, 0, m_hRootItem[0]);
	}

	Expand( m_hRootItem[0], TVE_EXPAND);
}

int cWeaponResourceList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	

	m_hRootItem[0] = InsertItem( "Character", 0, 0);

	return 0;
}

void cWeaponResourceList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CTreeCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void cWeaponResourceList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
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

void	cWeaponResourceList::RemoveAllRes( i3ClassMeta * pType)
{
	SetItemData( m_hRootItem[ RSC_CHARA], NULL);

	i3TDK::TreeCtrlTraverse( this, _RemoveAllResProc, pType);

	SetItemData( m_hRootItem[ RSC_CHARA], (DWORD_PTR) g_pChara);
}

void	cWeaponResourceList::UpdateAllRes(void)
{
	INT32 i;

	DeleteAllItems();

	Create();

	this->LockWindowUpdate();

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

	SetScrollPos(SB_VERT,0);
	SetScrollPos(SB_HORZ,0);
	this->UnlockWindowUpdate();
}



HTREEITEM	cWeaponResourceList::AddRes( HTREEITEM hParentItem, i3ResourceObject * pRes)
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

	hItem = InsertItem( &item);
	UpdateRes( hItem, pRes);

	// Root Item의 + 표시 Update
	{
		TVITEM info;

		info.mask = TVIF_CHILDREN | TVIF_HANDLE;
		info.hItem = hParentItem;
		info.cChildren = 1;

		SetItem( &info);
	}

	EnsureVisible( hItem);


	//m_TreeCtrl.SelectItem( hItem);

	return hItem;
}

void cWeaponResourceList::UpdateAllAI(void)
{
	_Rec_UpdateAI( m_hRootItem[ RSC_AI], g_pChara->geti3Chara());
}


void	cWeaponResourceList::UpdateChara(void)
{
	HTREEITEM hItem = m_hRootItem[ RSC_CHARA];

	SetItemData( hItem, (DWORD_PTR) g_pChara);

	if( g_pChara->hasName() == false)
	{
		SetItemText( hItem, "<NONAME : Character>");
	}
	else
	{
		SetItemText( hItem, g_pChara->GetName());
	}
}


void cWeaponResourceList::UpdateRes( HTREEITEM& hItem, i3ResourceObject * pRes)
{
	char szName[ MAX_PATH];

	GetInfoString( szName, pRes);

	I3TRACE("%s\n",szName);
	SetItemText( hItem, szName);
}


void cWeaponResourceList::_Rec_UpdateAI( HTREEITEM hParentItem, i3GameNode * pNode)
{
	HTREEITEM hItem = hParentItem;

	if( i3::kind_of<i3AIContext*>(pNode)) //->IsTypeOf( i3AIContext::GetClassMeta()))
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

void	cWeaponResourceList::GetInfoString( char * pszBuf, i3ResourceObject * pRes)
{
	if( i3::same_of<i3SceneGraphInfo*>(pRes)) //->IsExactTypeOf( i3SceneGraphInfo::GetClassMeta()))
	{
	//	i3String::SplitFileName( pRes->GetName(), pszBuf, FALSE);
		i3::extract_filetitle(pRes->GetName(), pszBuf);
	}
	else if( i3::same_of<CLOD*>(pRes)) //->IsExactTypeOf( CLOD::GetClassMeta()))
	{
		sprintf( pszBuf, "Lv %d [%s]", g_pChara->FindLOD( (CLOD *) pRes), pRes->GetName());
	}
	else if( i3::kind_of<i3Animation*>(pRes)) //->IsTypeOf( i3Animation::GetClassMeta()))
	{
		strcpy( pszBuf, pRes->GetName());
	}
	else
	{
	//	i3String::SplitFileName( pRes->GetName(), pszBuf, FALSE);
		i3::extract_filetitle(pRes->GetName(), pszBuf);
	}
}


HTREEITEM cWeaponResourceList::getRootItem( i3ResourceObject * pRes)
{
	if( i3::kind_of<CChara*>(pRes)) //->IsTypeOf( CChara::GetClassMeta()))
	{
		return m_hRootItem[ RSC_CHARA];
	}
	else if( i3::same_of<i3SceneGraphInfo*>(pRes)) //->IsExactTypeOf( i3SceneGraphInfo::GetClassMeta()))
	{
		return m_hRootItem[ RSC_SCENEGRAPH];
	}
	else if( i3::kind_of<CLOD*>(pRes)) //->IsTypeOf( CLOD::GetClassMeta()))
	{
		return m_hRootItem[ RSC_LOD] ;
	}
	else if( i3::kind_of<i3Animation*>(pRes)) //->IsTypeOf( i3Animation::GetClassMeta()))
	{
		return m_hRootItem[ RSC_ANIMATION];
	}
	else if( i3::kind_of<AI*>(pRes)) //->IsTypeOf( AI::GetClassMeta()))
	{
		AI * pAI = (AI *) pRes;
		i3GameNode * pParent = pAI->getAIContext()->getParent();

		if( pParent)
		{
			if( i3::kind_of<i3AIContext*>(pParent)) //->IsTypeOf( i3AIContext::GetClassMeta()))
			{
				INT32 idxParent = g_pChara->FindAIByAIContext( (i3AIContext *) pParent); 
				I3ASSERT( idxParent != -1);

				AI * pParentAI = g_pChara->getAI( idxParent);

				// Parent를 찾는다.
				HTREEITEM hParent = i3TDK::FindTreeItemByData( this, pParentAI, m_hRootItem[ RSC_AI]);
				I3ASSERT( hParent != NULL);

				return hParent;
			}
		}

		return m_hRootItem[ RSC_AI] ;
	}
	else if( i3::kind_of<i3Sound*>(pRes)) //->IsTypeOf( i3Sound::GetClassMeta()))
	{
		return m_hRootItem[ RSC_SOUND];
	}

	return TVI_ROOT;
}

HTREEITEM	cWeaponResourceList::FindRes( i3ResourceObject * pRes)
{
	HTREEITEM hRoot;

	hRoot = getRootItem( pRes);

	CTreeCtrl* pTree = this;
	return i3TDK::FindTreeItemByData( pTree, pRes, hRoot);
}

LRESULT cWeaponResourceList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_TDK_UPDATE)
	{
		OnUpdate((I3_TDK_UPDATE_INFO*) wParam);
	}

	return CTreeCtrl::WindowProc(message, wParam, lParam);
}


void cWeaponResourceList::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if (NULL == pInfo->m_pMeta)
	{
		UpdateAllRes();
		return;
	}

	if( i3::same_of<CChara*>(pInfo->m_pMeta)) //->IsExactTypeOf( CChara::GetClassMeta()))
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
	else if( i3::same_of<i3SceneGraphInfo*>(pInfo->m_pMeta)) //->IsExactTypeOf( i3SceneGraphInfo::GetClassMeta()))
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
	else if( i3::same_of<CLOD*>(pInfo->m_pMeta)) //->IsExactTypeOf( CLOD::GetClassMeta()))
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
	else if( i3::kind_of<i3Animation*>(pInfo->m_pMeta)) //->IsTypeOf( i3Animation::GetClassMeta()))
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
	else if( i3::kind_of<AI*>(pInfo->m_pMeta)) //->IsTypeOf( AI::GetClassMeta()))
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
	else if( i3::kind_of<i3Sound*>(pInfo->m_pMeta)) //->IsTypeOf( i3Sound::GetClassMeta()))
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


void	cWeaponResourceList::UpdateAllLOD(void)
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

void	cWeaponResourceList::RemoveRes( i3ResourceObject * pRes)
{
	HTREEITEM hItem;

	hItem = FindRes( pRes);
	//I3ASSERT( hItem != NULL);			새캐릭터 또는 종료시 다운 버그 막기 by KOMET

	if( hItem != NULL)
		RemoveRes( hItem);
}

void	cWeaponResourceList::RemoveRes( HTREEITEM hItem)
{
	DeleteItem( hItem);
}

void cWeaponResourceList::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM item = GetSelectedItem();
	i3ElementBase* pObject = (i3ElementBase*)GetItemData(item);
	
	if (pObject == NULL)
		return;

	if( i3::kind_of<i3Animation*>(pObject)) //->IsTypeOf( i3Animation::GetClassMeta()))
	{
		i3Animation* pAnim = (i3Animation*)pObject;
		g_pChara->geti3Chara()->PlayAnim(pAnim,I3_ANIM_PLAY_MODE_CLAMP);
	}
	else if( i3::kind_of<i3Sound*>(pObject)) //->IsTypeOf( i3Sound::GetClassMeta()))
	{
		if (i3::same_of<i3SoundFMOD*>(pObject)) //->IsExactTypeOf(i3SoundFMOD::GetClassMeta()))
		{
			i3SoundFMOD* pSound = (i3SoundFMOD*)pObject;
			
			FMOD::Sound* pFMSound = pSound->Get3DSound(true);
			pFMSound->setMode(FMOD_LOOP_OFF);

			FMOD::Channel *		m_pChannel = NULL;
			i3SoundContextFMOD* pFMOD = (i3SoundContextFMOD*)g_pViewer->GetSoundContext();
			
			FMOD::System* pFMODSystem = pFMOD->getFMODSystem();
			pFMODSystem->playSound(FMOD_CHANNEL_FREE, pFMSound, false, &m_pChannel);
		}
	}


	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void cWeaponResourceList::OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	HTREEITEM item = GetSelectedItem();
	i3ElementBase* pObject = (i3ElementBase*)GetItemData(item);
	
	if (pObject == NULL)
		return;

	if( i3::kind_of<i3Animation*>(pObject)) //->IsTypeOf( i3Animation::GetClassMeta()))
	{
		i3Animation* pAnim = (i3Animation*)pObject;
		g_pChara->geti3Chara()->PlayAnim(pAnim,I3_ANIM_PLAY_MODE_CLAMP);
	}
	else if( i3::kind_of<i3Sound*>(pObject)) //->IsTypeOf( i3Sound::GetClassMeta()))
	{
		if (i3::same_of<i3SoundFMOD*>(pObject)) //->IsExactTypeOf(i3SoundFMOD::GetClassMeta()))
		{
			i3SoundFMOD* pSound = (i3SoundFMOD*)pObject;
			FMOD::Sound* pFMSound = pSound->Get3DSound(true);

			FMOD::Channel *		m_pChannel;
			i3SoundContextFMOD* pFMOD = (i3SoundContextFMOD*)g_pViewer->GetSoundContext();

			FMOD::System* pFMODSystem = pFMOD->getFMODSystem();
			pFMODSystem->playSound(FMOD_CHANNEL_FREE, pFMSound, true, &m_pChannel);
		}
	}

	switch(pTVKeyDown->wVKey)
	{
	case VK_DELETE:
		{
			bool bDelete = false;
			if( i3::kind_of<CLOD*>(pObject)) //->IsTypeOf(CLOD::GetClassMeta()))
				bDelete = true;

			OnDelete(item, pObject);

			if (false == bDelete)
				DeleteItem(item);
		}
		break;
	}
	
	*pResult = 0;
}

void cWeaponResourceList::OnDelete( HTREEITEM hItem, i3ElementBase * pObj)
{
	if( pObj != NULL)
	{
		if( i3::kind_of<CChara*>(pObj)) //->IsTypeOf( CChara::GetClassMeta()))
		{
		}
		else if( i3::kind_of<i3SceneGraphInfo*>(pObj)) //->IsTypeOf( i3SceneGraphInfo::GetClassMeta()))
		{
 			int rv = AfxMessageBox( "Scene-Graph를 사용하는 LOD도 함께 제거될 수 있다. 그래도 진행하시겠습니까?", MB_YESNO);
 
 			if( rv == IDYES)
 			{
 				g_pChara->RemoveSceneGraph( (i3SceneGraphInfo *) pObj);
				
 			}
		}
		else if( i3::kind_of<CLOD*>(pObj)) //->IsTypeOf( CLOD::GetClassMeta()))
		{
			g_pChara->RemoveLOD( (CLOD *) pObj);
		}
		else if( i3::kind_of<i3Animation*>(pObj)) //->IsTypeOf( i3Animation::GetClassMeta()))
		{
			g_pChara->geti3Chara()->StopAnimation((i3Animation*)pObj);
			g_pChara->RemoveAnim( (i3Animation *) pObj);
		}
		else if( i3::kind_of<AI*>(pObj)) //->IsTypeOf( AI::GetClassMeta()))
		{
			g_pChara->RemoveAI( (AI *) pObj);
		}
		else if( i3::kind_of<i3Sound*>(pObj)) //->IsTypeOf( i3Sound::GetClassMeta()))
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