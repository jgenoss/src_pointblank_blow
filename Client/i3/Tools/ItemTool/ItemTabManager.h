#pragma once

#include "RibbonBarItem.h"


enum E_ITEM_TABLIST
{
	E_TAB_WEAPON,
	E_TAB_GOODS,
	E_TAB_EQUIPMENT,
	E_TAB_CHARACTER,
	E_TAB_ABILITY,
	E_MAX
};



class cItemTabBase;
class cItemTabManager : public i3ElementBase
{
public:
	I3_CLASS_DEFINE(cItemTabManager, i3ElementBase);

	cItemTabManager(void);
	virtual ~cItemTabManager(void);

private:
	typedef std::map<E_ITEM_TABLIST, cItemTabBase*> mItemTabCon;
	mItemTabCon m_mItemTab;

	cRibbonBarItem	m_wndRibbonBar;
		
	CMFCRibbonApplicationButton m_MainButton;
	
	E_ITEM_TABLIST m_eCurrentTAB;
	
public:
	void InitTab(CWnd* pMainWnd);
	void OnUpdate(RT_REAL32 timeDelta);

	E_ITEM_TABLIST GetCurrentTAB() const { return m_eCurrentTAB; }
	void SetCurrentTAB(E_ITEM_TABLIST val) 
	{ 
		m_eCurrentTAB = val; 
	}

	cRibbonBarItem* GetWndRibbonBar() { return &m_wndRibbonBar; }

	UINT GetMFCTabID(E_ITEM_TABLIST eTab);

protected:
	bool CreateTab(i3ClassMeta* pMeta);
	bool InsertTab(E_ITEM_TABLIST eTab, i3ClassMeta* pMeta);
};
