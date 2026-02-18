#include "StdAfx.h"
#include "MainFrm.h"
#include "resource.h"

#include "ItemTabManager.h"

#include "ItemTabBase.h"
#include "ItemTabWeapon.h"
#include "ItemTabGoods.h"
#include "ItemTabEquipment.h"
#include "ItemTabCharacter.h"
#include "ItemTabAbility.h"



I3_CLASS_INSTANCE(cItemTabManager); //, i3ElementBase);

cItemTabManager::cItemTabManager(void)
:	m_eCurrentTAB(E_TAB_WEAPON)
{
}

cItemTabManager::~cItemTabManager(void)
{
	mItemTabCon::iterator iter = m_mItemTab.begin();

	for (;iter != m_mItemTab.end(); ++iter)
	{
		i3::destroy_instance(iter->second);		//	I3_SAFE_DELETE(iter->second);
	}
}

UINT cItemTabManager::GetMFCTabID(E_ITEM_TABLIST eTab)
{
	switch(eTab)
	{
	case E_TAB_WEAPON:
		return ID_VIEW_WEAPON_TAB;
		break;
	case E_TAB_GOODS:
		return ID_VIEW_GOODS_TAB;
		break;
	case E_TAB_EQUIPMENT:
		return ID_VIEW_EQUIPMENT_TAB;
		break;
	case E_TAB_CHARACTER:
		return ID_VIEW_CHARACTER_TAB;
		break;
	case E_TAB_ABILITY:
		return ID_VIEW_ABILITY_TAB;
		break;
	}
	return 0;
}

void cItemTabManager::InitTab(CWnd* pMainWnd)
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	m_wndRibbonBar.Create(pMain);

	//탭별로 클래스및 리본카테고리를 생성한다.
	if (!CreateTab(cItemTabWeapon::static_meta()))
		I3ASSERT(0);

	if (!CreateTab(cItemTabGoods::static_meta()))
		I3ASSERT(0);

	if (!CreateTab(cItemTabEquipment::static_meta()))
		I3ASSERT(0);

	if (!CreateTab(cItemTabCharacter::static_meta()))
		I3ASSERT(0);

	if (!CreateTab(cItemTabAbility::static_meta()))
		I3ASSERT(0);

	//탭 페널 업데이트
	m_wndRibbonBar.UpdateRibbonChildPanel();

	//RootPath 패널
	m_wndRibbonBar.AddPathEditorPanel();

	//Utility 패널
	//m_wndRibbonBar.AddUtilityPanel();


}

void cItemTabManager::OnUpdate(RT_REAL32 timeDelta)
{
	switch(m_eCurrentTAB)
	{
	case E_TAB_WEAPON:
		m_wndRibbonBar.OnVisibleWeaponEdit(true);
		break;
	default:
		m_wndRibbonBar.OnVisibleWeaponEdit(false);
		break;

	}
}

bool cItemTabManager::CreateTab(i3ClassMeta* pMeta)
{
	if (i3::same_of<cItemTabWeapon*>(pMeta)) //->IsExactTypeOf(cItemTabWeapon::GetClassMeta()))	
	{
		InsertTab(E_TAB_WEAPON, pMeta);
	}
	else if (i3::same_of<cItemTabGoods*>(pMeta)) //->IsExactTypeOf(cItemTabGoods::GetClassMeta()))	
	{
		InsertTab(E_TAB_GOODS, pMeta);
	}
	else if (i3::same_of<cItemTabEquipment*>(pMeta)) //->IsExactTypeOf(cItemTabEquipment::GetClassMeta()))	
	{
		InsertTab(E_TAB_EQUIPMENT, pMeta);
	}
	else if (i3::same_of<cItemTabCharacter*>(pMeta)) //->IsExactTypeOf(cItemTabCharacter::GetClassMeta()))	
	{
		InsertTab(E_TAB_CHARACTER, pMeta);
	}
	else if (i3::same_of<cItemTabAbility*>(pMeta)) //->IsExactTypeOf(cItemTabAbility::GetClassMeta()))	
	{
		InsertTab(E_TAB_ABILITY, pMeta);
	}
	else
		return false;

	return true;
}

bool cItemTabManager::InsertTab(E_ITEM_TABLIST eTab, i3ClassMeta* pMeta)
{
	cItemTabBase* pItemTab = (cItemTabBase*) pMeta->create_instance();	// CREATEINSTANCE(pMeta);
	if (!pItemTab)
		return false;

	m_mItemTab[eTab] = pItemTab;
	m_wndRibbonBar.AddRibbonPanel(GetMFCTabID(eTab));

	return true;
}

