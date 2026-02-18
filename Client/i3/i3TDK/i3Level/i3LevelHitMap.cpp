#include "stdafx.h"
#include "i3LevelHitMap.h"
#include "i3LevelGlobalVariable.h"

#define DEF_COLOOR_RED COLOR(255, 0, 0, 255)
#define DEF_COLOOR_YELLOW COLOR(247, 234, 110, 255)
#define DEF_COLOOR_GRAY COLOR(116, 116, 116, 255)

I3_CLASS_INSTANCE(i3LevelHitMap);

i3LevelHitMap::i3LevelHitMap()
{
	setInstanceClassName("i3GameHiptMap");

	AddStyle(I3_LEVEL_STYLE_UNSELECT | I3_LEVEL_STYLE_NOMOVE | I3_LEVEL_STYLE_NOSCALE |
		I3_LEVEL_STYLE_NOROTATE_X | I3_LEVEL_STYLE_NOROTATE_Y | I3_LEVEL_STYLE_NOROTATE_Z);

	m_HitMapList.clear();
}


i3LevelHitMap::~i3LevelHitMap()
{
	for (size_t i = 0; i < m_HitMapList.size(); i++)
	{
		I3_MUST_RELEASE(m_HitMapList[i]->_pKillerOj);
		I3_MUST_RELEASE(m_HitMapList[i]->_pTargetOj);
		I3_MUST_RELEASE(m_HitMapList[i]->_pPathLink);

		I3_SAFE_DELETE(m_HitMapList[i]);
	}

	m_HitMapList.clear();
}

void i3LevelHitMap::AddHitPoint(UINT64 uiUserID, char* killerName, char* deathName, VEC3D killerPos, VEC3D deathPos)
{
	//int nIndex = m_HitMapList.size();
	i3BoundBox bb;
	VEC3D vecMin;
	VEC3D vecMax;

	HITMAPINFO* pHitmap = new HITMAPINFO;
	pHitmap->_uiIUserID = uiUserID;
	COLOR color = DEF_COLOOR_GRAY;
	{
		pHitmap->_pKillerOj = i3LevelHitPoint::new_object();
		pHitmap->_pKillerOj->Create();
		pHitmap->_pKillerOj->setLayer(g_pScene->getLayer(g_pScene->FindLayerByName("HitMap")));
		pHitmap->_pKillerOj->SetOwner(this);

		g_pScene->AddElement(pHitmap->_pKillerOj, killerName);
		pHitmap->_pKillerOj->setPos(&killerPos);
		pHitmap->_pKillerOj->SetName(killerName, &killerPos);
		
		pHitmap->_pKillerOj->SetColor(&color);
		
		pHitmap->_pKillerOj->GetTransformedBoundBox(&vecMin, &vecMax);
		bb.SetMinMax(&vecMin, &vecMax);
		bb.GetCenter(&killerPos);
		pHitmap->_pKillerOj->setPos(&killerPos);
	}
	
	{
		pHitmap->_pTargetOj = i3LevelHitPoint::new_object();
		pHitmap->_pTargetOj->Create();
		pHitmap->_pTargetOj->setLayer(g_pScene->getLayer(g_pScene->FindLayerByName("HitMap")));
		pHitmap->_pTargetOj->SetOwner(this);

		g_pScene->AddElement(pHitmap->_pTargetOj, deathName);
		pHitmap->_pTargetOj->setPos(&deathPos);
		pHitmap->_pTargetOj->SetName(deathName, &deathPos);
		pHitmap->_pTargetOj->SetColor(&color);

		pHitmap->_pTargetOj->GetTransformedBoundBox(&vecMin, &vecMax);
		bb.SetMinMax(&vecMin, &vecMax);
		bb.GetCenter(&deathPos);
		pHitmap->_pTargetOj->setPos(&deathPos);
	}
	
	{
		pHitmap->_pPathLink = i3LevelHitPathLink::new_object();
		pHitmap->_pPathLink->SetOwner(this);
		pHitmap->_pPathLink->SetOffsetPos(VEC3D(0, 0.7f, 0));
		pHitmap->_pPathLink->SetStartElement(pHitmap->_pKillerOj);
		pHitmap->_pPathLink->SetEndElement(pHitmap->_pTargetOj);
		pHitmap->_pPathLink->setBidirectional(false);
		pHitmap->_pPathLink->SetColor(&color);

		//Path Object는 _pKillerOj의 자식으로 삼는다
		pHitmap->_pKillerOj->GetRoot()->AddChild(pHitmap->_pPathLink->GetRoot());
	}

	m_HitMapList.push_back(pHitmap);
}

void i3LevelHitMap::Update()
{
	for (int i = 0; i < (int)m_HitMapList.size(); i++)
	{
		HITMAPINFO* pHitmap = m_HitMapList[i];

		pHitmap->_pKillerOj->UpdateNode();
		pHitmap->_pTargetOj->UpdateNode();

		pHitmap->_pPathLink->UpdateLink();					//경로 update
	}
}

void i3LevelHitMap::SetColorChange(UINT64 uiUserID)
{
	COLOR color = DEF_COLOOR_RED;
	for (int i = 0; i < (int)m_HitMapList.size(); i++)
	{
		HITMAPINFO* pHitmap = m_HitMapList[i];
		
		if (pHitmap->_uiIUserID == uiUserID)
		{
			color = DEF_COLOOR_RED;
			pHitmap->_pKillerOj->SetColor(&color);
			color = DEF_COLOOR_YELLOW;
			pHitmap->_pTargetOj->SetColor(&color);
			color = DEF_COLOOR_RED;
			pHitmap->_pPathLink->SetColor(&color);
		}
		else
		{
			color = DEF_COLOOR_GRAY;
			pHitmap->_pKillerOj->SetColor(&color);
			pHitmap->_pTargetOj->SetColor(&color);
			pHitmap->_pPathLink->SetColor(&color);
		}
	}
}

void i3LevelHitMap::DeleteAllPoint()
{
	for (size_t i = 0; i < m_HitMapList.size(); i++)
	{
		i3Level::GetScene()->RemoveElement(m_HitMapList[i]->_pKillerOj);
		i3Level::GetScene()->RemoveElement(m_HitMapList[i]->_pTargetOj);

		I3_MUST_RELEASE(m_HitMapList[i]->_pKillerOj);
		I3_MUST_RELEASE(m_HitMapList[i]->_pTargetOj);
		I3_MUST_RELEASE(m_HitMapList[i]->_pPathLink);

		I3_SAFE_DELETE(m_HitMapList[i]);
	}

	m_HitMapList.clear();
}