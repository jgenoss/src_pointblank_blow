#include "stdafx.h"
#include "GlobalInstance.h"
#include "MainFrm.h"
#include "i3Base/string/ext/make_relative_path.h"

CGlobalInstance::CGlobalInstance()
{
	m_pUIAnimation = NULL;
}


CGlobalInstance::~CGlobalInstance()
{
	for (INT32 i = 0; i < m_vecUVItem.size(); i++)
	{
		UVITEM* pItem = m_vecUVItem[i];
		I3_SAFE_RELEASE(pItem->_pControl);
		I3_SAFE_DELETE(pItem);
	}

	I3_SAFE_RELEASE(m_pUIAnimation);
}

UVITEM* CGlobalInstance::GetUVItem(INT32 nIndex)
{
	if (m_vecUVItem.size() <= nIndex) return NULL;

	return m_vecUVItem[nIndex];
}

INT32 CGlobalInstance::GetUVItem(i3UIControl* pControl)
{
	for (INT32 i = 0; i < m_vecUVItem.size(); i++)
	{
		if (m_vecUVItem[i]->_pControl == pControl)
		{
			return i;
		}
	}

	return -1;
}

void CGlobalInstance::AddUVItem(i3Texture* pTexture, CRect rect)
{
	if (pTexture == NULL) return;

	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	LVITEM item;
	item.mask = LVIF_TEXT | LVIF_IMAGE | LVCFMT_LEFT;
	item.iSubItem = 0;
	item.pszText = "UVImage";

	UVITEM* pItem = new UVITEM;
	pItem->_pTexture = pTexture;

	pItem->_pControl = i3UIControl::new_object();
	pItem->_pControl->setPos(rect.left, rect.top);
	pItem->_pControl->setSize(rect.right, rect.bottom);
	pItem->_pControl->removeStyle(I3UI_STYLE_SIZE_RATIO);

	m_vecUVItem.push_back(pItem);

	pMainWnd->GetPieceListPane()->AddItem(pItem, &item);
	pMainWnd->GetTextureView()->AddItem(pItem);
	pMainWnd->GetPropertys()->Update(pItem);
}

void CGlobalInstance::UpdateUVItem(i3UIControl* pControl)
{
	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	for (INT32 i = 0; i < (INT32)m_vecUVItem.size(); i++)
	{
		UVITEM* pItem = GetUVItem(i);
		if (pItem->_pControl == pControl)
		{
			pItem->_pControl->setPos(pControl->getPos());
			pItem->_pControl->setSize(pControl->getSize());

			pMainWnd->GetPieceListPane()->UpdateUVItem(pItem, i);
		}
	}
}

void CGlobalInstance::RemoveUVItem(UVITEM* pItem)
{
	if (pItem == NULL) return;
	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();
	i3::vector<UVITEM*>::iterator it = m_vecUVItem.begin();

	for ( it = m_vecUVItem.begin(); it != m_vecUVItem.end(); ++it)
	{
		if ((*it)->_pControl == pItem->_pControl)
		{
			I3_SAFE_RELEASE((*it)->_pControl);
			delete *it;
			m_vecUVItem.erase(it);
			break;
		}
	}

	pMainWnd->GetTextureView()->RemoveAllItem();
	pMainWnd->GetPieceListPane()->RemoveAllItem();

	i3::vector<UVITEM*> vecTemp;
	vecTemp.clear();

	for (INT32 i=0; i < (INT32)m_vecUVItem.size(); i++)
	{
		LVITEM item;
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVCFMT_LEFT;
		item.iSubItem = 0;
		item.pszText = "UVImage";

		pMainWnd->GetPieceListPane()->AddItem(m_vecUVItem[i], &item);
		pMainWnd->GetTextureView()->AddItem(m_vecUVItem[i]);

		vecTemp.push_back(m_vecUVItem[i]);
	}

	m_vecUVItem = vecTemp;

	i3Texture* pTexture = pMainWnd->GetTextureView()->GetImage();

	if (pTexture)
		pMainWnd->GetTextureView()->DisplayTex(pTexture->GetName());
}

void CGlobalInstance::AddAnimationItem(char* pchTitle, REAL32 fLifeTime)
{
	char chAnimationName[MAX_PATH];
	sprintf_s(chAnimationName, "%s", pchTitle );
	INT32 nIndex = 1;
	while (true)
	{
		if (m_pUIAnimation->IsAnimation(chAnimationName))
		{
			sprintf_s(chAnimationName, "%s%d", pchTitle, nIndex++);
		}
		else
		{
			break;
		}
	}


	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	i3UIAnimation2DSprite* pAnimation = i3UIAnimation2DSprite::new_object();

	pAnimation->SetSpriteCount(m_vecUVItem.size());
	pAnimation->CreateSprite(pAnimation->GetSpriteCount());
	pAnimation->SetName(chAnimationName);
	
	for (INT32 i = 0; i < pAnimation->GetSpriteCount(); i++)
	{
		pAnimation->SetUVInfo(i, m_vecUVItem[i]->_pTexture->GetName(),
			CRect(m_vecUVItem[i]->_pControl->getPos()->x, m_vecUVItem[i]->_pControl->getPos()->y,
				m_vecUVItem[i]->_pControl->getWidth(), m_vecUVItem[i]->_pControl->getHeight()));
	}

	pAnimation->SetLifeTime( fLifeTime);

	m_pUIAnimation->AddAnimation(pAnimation);

	pMainWnd->GetAnimationListPane()->Update();

	RemoveUVItems();
}

void CGlobalInstance::RemoveUVItems()
{
	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	pMainWnd->GetTextureView()->RemoveAllItem();
	pMainWnd->GetPieceListPane()->RemoveAllItem();
	pMainWnd->GetPropertys()->RemoveAllProperty();
	pMainWnd->GetTextureView()->UpdateInvaliDate();

	for (INT32 i = 0; i < m_vecUVItem.size(); i++)
	{
		UVITEM* pItem = m_vecUVItem[i];
		I3_SAFE_RELEASE(pItem->_pControl);
		I3_SAFE_DELETE(pItem);
	}

	m_vecUVItem.clear();
}

i3UIAnimation2DSprite* CGlobalInstance::GetAnimation(i3::string name)
{
	return m_pUIAnimation->GetAnimation(name);
}

void CGlobalInstance::RemoveAnimation(i3::string name)
{
	m_pUIAnimation->RemoveAnimation(name);
}

bool CGlobalInstance::SaveAnimationList()
{

	OPENFILENAME Ofn;
	memset(&Ofn, 0, sizeof(OPENFILENAME));

	char lpstrFile[1024] = "";
	Ofn.lStructSize = sizeof(OPENFILENAME);
	Ofn.lpstrFilter = "AnimationFile(*.i3Animation)\0*.i3Animation\0葛电颇老(*.*)\0*.*\0";
	Ofn.nFilterIndex = 1;
	Ofn.lpstrFile = lpstrFile;
	Ofn.nMaxFile = 1024;
	Ofn.lpstrDefExt = "i3Animation";
	Ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&Ofn) != 0)
	{
		i3XMLFile File;
		File.setKeyObject(m_pUIAnimation);

		if (File.Save(Ofn.lpstrFile) == false)
		{
			I3PRINTLOG(I3LOG_WARN, "failed save file: %s\n", Ofn.lpstrFile);
			return false;
		}
	}
	
}

bool CGlobalInstance::OpenAnimationList()
{
	OPENFILENAME Ofn;
	memset(&Ofn, 0, sizeof(OPENFILENAME));

	char lpstrFile[1024] = "";
	Ofn.lStructSize = sizeof(OPENFILENAME);
	Ofn.lpstrFilter = "AnimationFile(*.i3Animation)\0*.i3Animation\0葛电颇老(*.*)\0*.*\0";
	Ofn.nFilterIndex = 1;
	Ofn.lpstrFile = lpstrFile;
	Ofn.nMaxFile = 1024;
	Ofn.lpstrDefExt = "i3Animation";

	if (GetOpenFileName(&Ofn) != 0)
	{
		i3XMLFile File;

		if (File.Load(Ofn.lpstrFile) == false)
		{
			I3PRINTLOG(I3LOG_WARN, "failed Open file: %s\n", Ofn.lpstrFile);
		}
		else
		{
			I3_SAFE_RELEASE(m_pUIAnimation);
			m_pUIAnimation = (i3UIAnimationMgr*)File.getKeyObject();
			I3_SAFE_ADDREF(m_pUIAnimation);

			CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();
			pMainWnd->GetAnimationListPane()->Update();
		}
	}

	return false;
}

void CGlobalInstance::NewAnimationFile()
{
	I3_SAFE_RELEASE(m_pUIAnimation);
	m_pUIAnimation = i3UIAnimationMgr::new_object();
	SaveAnimationList();
}