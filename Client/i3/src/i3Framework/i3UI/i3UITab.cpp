#include "i3FrameworkPCH.h"
#include "i3UITab.h"
#include "i3UIManager.h"	
#include "i3UITemplate_Tab.h"
#include "../i3Framework.h"
#include "i3Base/string/compare/generic_is_iequal.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

I3_CLASS_INSTANCE( i3UITab);

i3UITab::i3UITab()
{
	if( i3UI::getToolDevMode())
		SetCaption( L"Tab");
	
	for (int i = 0 ; i < I3UI_TAB_STATE_COUNT ; ++i)
		m_aTextColor[i].SetRGBA(0xFFFFFFFF);
}

void i3UITab::GetTabsInTheSameGroup( i3::vector<i3UITab*>& Out)
{
	//tab의 부모컨트롤이 framewindow말고 다른것도 될수 있나? 
	i3GameNode* pParent = getParent();
	while( pParent != nullptr)
	{
		if( i3::same_of<i3UIFrameWnd*>(pParent))
			break;

		pParent = pParent->getParent();
	}

	if( pParent != nullptr)
	{
		I3ASSERT( i3::same_of<i3UIFrameWnd*>(pParent));

		i3::vector<i3UIControl*> childList;
		
		i3UIFrameWnd* pParentFrm = (i3UIFrameWnd*)pParent;
		pParentFrm->GetChildControls( childList, true);

		for( size_t i=0; i< childList.size(); i++)
		{
			i3UIControl* pChild = childList[i];
			if( i3::same_of<i3UITab*>(pChild))
			{
				i3UITab* pTab = (i3UITab*)pChild;
				if( pTab->getGroupID() == m_nGroupID)
				{
					Out.push_back( pTab);	//this도 포함
				}
			}
		}
	}
}

//탭이 선택되었을 때: 자신의 buddy frame window만 enable시키고
//같은 그룹에 속한 다른 탭들의 buddy frame window들은 모두 disable시킨다. 
void i3UITab::OnTabSelected( void)
{
	i3::vector<i3UITab*> tabList;
	
	GetTabsInTheSameGroup( tabList);

	for( size_t i=0; i< tabList.size(); i++)
	{
		i3UITab* pTab = tabList[i];
		
		if( pTab == this)
		{
			pTab->addState( I3UI_STATE_SELECTED);
			pTab->EnableBuddyFrame( true);
		}
		else
		{
			pTab->RemoveTabSelect();
		}
	}

	setModifyState(true);
}

void i3UITab::RemoveTabSelect( void)
{
	removeState( I3UI_STATE_SELECTED);
	EnableBuddyFrame( false);
	setModifyState( true);
}

void i3UITab::InitBuddy( void)
{
	m_pBuddyFrame = nullptr;

	if( !m_strBuddyName.empty())
	{
		i3UIControl* pBuddy = i3UI::FindControlByName( nullptr, m_strBuddyName.c_str());
		if( i3::same_of<i3UIFrameWnd*>(pBuddy))
		{
			m_pBuddyFrame = (i3UIFrameWnd*)pBuddy;		//addref ? 

			m_pBuddyFrame->EnableCtrl( m_bDefault);

			addState( I3UI_STATE_NEED_UPDATE);
		}
	}
}

void i3UITab::EnableBuddyFrame( bool bEnable)
{
	if( m_pBuddyFrame != nullptr)
	{
		m_pBuddyFrame->EnableCtrl( bEnable, true);
	}
}

void i3UITab::_UpdateShapeState( void)
{
	bool bDisabled = !isActivated();
	bool bSelected = isSelected();	// LClick
	bool bOnMouse = isState( I3UI_STATE_ONMOUSE);	// Enter

	INT32 start = I3UI_TAB_SHAPE_TOPLEFT;
	
	I3UI_TAB_STATE tabState = I3UI_TAB_STATE_NORMAL;

	if( bDisabled)
	{
		start = I3UI_TAB_SHAPE_TOPLEFT_DISABLE;
		tabState = I3UI_TAB_STATE_DISABLED;
	}
	else
	{
		if( bSelected)	// OnMouse 상태보다 우선한다.
		{
			start = I3UI_TAB_SHAPE_TOPLEFT_CLICK;
			tabState = I3UI_TAB_STATE_PUSHED;
		}
		else
		if( bOnMouse)
		{
			start = I3UI_TAB_SHAPE_TOPLEFT_ONMOUSE;
			tabState = I3UI_TAB_STATE_ONMOUSE;
		}
	}

	INT32 end = start + I3UI_TAB_SHAPE_COUNT_PER_SET;

	for( INT32 i=0; i< I3UI_TAB_SHAPE_COUNT; i++)
	{
		if( i >= start && i < end)
			setShapeVisible( i, true);
		else
			setShapeVisible( i, false);
	}

	i3UICaptionControl::SetTextColor(&m_aTextColor[tabState]);

	i3UICaptionControl::_UpdateShapeState();
}


void i3UITab::OnEnabled( bool bEnable)
{
	i3UICaptionControl::OnEnabled( bEnable);
}

void i3UITab::OnLButtonDown( UINT32 nFlag, POINT point)
{
	if(isActivated() == false)
		return;

	i3UICaptionControl::OnLButtonDown( nFlag, point);

	//addState( I3UI_STATE_SELECTED);

	Notify( I3UI_EVT_BTN_PUSHED, 0, 0 );		//  이 상태가 루아 수준에서 꼭 필요하므로, 이것으로 대체..(11.11.21.수빈)
//	ParentNotify( I3UI_EVT_BTN_PUSHED, 0, 0);	//I3UI_EVT_CHANGED ? 

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UITab::OnLButtonUp(UINT32 nFlag, POINT point)
{
	if(isActivated() == false)
		return;

	// 마우스포인터가 컨트롤에 위치할때만 탭을 활성화시킵니다. (2013.02.21 - 양승천)
	if(point.x >= m_AccPos.x && point.x <= m_AccPos.x + m_Size.x &&	point.y >= m_AccPos.y && point.y <= m_AccPos.y + m_Size.y)
	{
		OnTabSelected();
	}

	i3UICaptionControl::OnLButtonUp( nFlag, point);

	addState( I3UI_STATE_NEED_UPDATE);
}


void i3UITab::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UICaptionControl::OnEnterMouse( x, y, keys);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UITab::OnLeaveMouse(void)
{
	i3UICaptionControl::OnLeaveMouse();

	addState( I3UI_STATE_NEED_UPDATE);
}


void i3UITab::OnSize( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::OnSize( cx, cy);

	addState( I3UI_STATE_NEED_UPDATE);
}


namespace
{
	UINT32& FileIDToUINT(UINT8 (&strID)[4] ) {  return reinterpret_cast<UINT32&>(strID); }
	const UINT32& FileIDToUINT(const char (&strID)[5] ) {  return reinterpret_cast<const UINT32&>(strID); }

	const UINT32& UTB1 = FileIDToUINT("UTB1");
	const UINT32& UTB2 = FileIDToUINT("UTB2");
	const UINT32& UTB3 = FileIDToUINT("UTB3");
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 UITAB
	{
		UINT8		m_ID[4] = { 'U', 'T', 'B', '2' };
		UINT32		m_textColor_Normal = 0;
		UINT32		m_textColor_MouseOver = 0;
		UINT32		m_textColor_Selected = 0;
		UINT32		m_textColor_Disable = 0;
		UINT32		m_GroupID = 0;
		UINT32		pad[27] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3UITab::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UITAB data;

	Result = i3UICaptionControl::OnSave( pResFile);
	I3_CHKIO( Result);

	//i3mem::Copy( data.m_ID, &UTB2, 4);	// 초기화에서 설정합니다. 주석처리. 2017.2.14. soon9
	
	data.m_GroupID =  m_nGroupID;
	data.m_textColor_Normal = GetTextColorByStateUINT32(I3UI_TAB_STATE_NORMAL);
	data.m_textColor_MouseOver = GetTextColorByStateUINT32(I3UI_TAB_STATE_ONMOUSE);
	data.m_textColor_Selected = GetTextColorByStateUINT32(I3UI_TAB_STATE_PUSHED);
	data.m_textColor_Disable  = GetTextColorByStateUINT32(I3UI_TAB_STATE_DISABLED);

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

UINT32 i3UITab::OnLoad( i3ResourceFile * pResFile)
{
i3_prof_func();

	UINT32 Rc, Result = 0;
	pack::UITAB data;

	Result = i3UICaptionControl::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	
	Result += Rc;
	
	UINT32 myID = FileIDToUINT(data.m_ID);

	m_nGroupID = data.m_GroupID;

	if (myID == UTB2)
	{
		SetTextColorByState(I3UI_TAB_STATE_NORMAL, data.m_textColor_Normal);
		SetTextColorByState(I3UI_TAB_STATE_ONMOUSE, data.m_textColor_MouseOver);
		SetTextColorByState(I3UI_TAB_STATE_PUSHED, data.m_textColor_Selected);
		SetTextColorByState(I3UI_TAB_STATE_DISABLED, data.m_textColor_Disable);
	}
	else
	{
		UINT32 uColor = getTextColor();
		SetTextColorByStateAll(uColor);
	}

	return Result;
}

bool i3UITab::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	pXML->addAttr( "GroupID", m_nGroupID);
	pXML->addAttr( "BuddyName", m_strBuddyName.c_str());
	pXML->addAttr( "Default", m_bDefault ? 1 : 0);

	I3COLOR clr = m_aTextColor[ I3UI_TAB_STATE_NORMAL];
	pXML->addAttr("TabText_Normal_ColorR", (UINT32)i3Color::GetR(&clr));
	pXML->addAttr("TabText_Normal_ColorG", (UINT32)i3Color::GetG(&clr));
	pXML->addAttr("TabText_Normal_ColorB", (UINT32)i3Color::GetB(&clr));
	pXML->addAttr("TabText_Normal_ColorA", (UINT32)i3Color::GetA(&clr));
	
	clr = m_aTextColor[I3UI_TAB_STATE_ONMOUSE];
	pXML->addAttr("TabText_MouseOver_ColorR", (UINT32)i3Color::GetR(&clr));
	pXML->addAttr("TabText_MouseOver_ColorG", (UINT32)i3Color::GetG(&clr));
	pXML->addAttr("TabText_MouseOver_ColorB", (UINT32)i3Color::GetB(&clr));
	pXML->addAttr("TabText_MouseOver_ColorA", (UINT32)i3Color::GetA(&clr));
	
	clr = m_aTextColor[I3UI_TAB_STATE_PUSHED];
	pXML->addAttr("TabText_Selected_ColorR", (UINT32)i3Color::GetR(&clr));
	pXML->addAttr("TabText_Selected_ColorG", (UINT32)i3Color::GetG(&clr));
	pXML->addAttr("TabText_Selected_ColorB", (UINT32)i3Color::GetB(&clr));
	pXML->addAttr("TabText_Selected_ColorA", (UINT32)i3Color::GetA(&clr));

	clr = m_aTextColor[I3UI_TAB_STATE_DISABLED];
	pXML->addAttr("TabText_Disable_ColorR", (UINT32)i3Color::GetR(&clr));
	pXML->addAttr("TabText_Disable_ColorG", (UINT32)i3Color::GetG(&clr));
	pXML->addAttr("TabText_Disable_ColorB", (UINT32)i3Color::GetB(&clr));
	pXML->addAttr("TabText_Disable_ColorA", (UINT32)i3Color::GetA(&clr));
	
	return bResult;
}

bool i3UITab::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;

	pXML->getAttr( "GroupID", &m_nGroupID);

	const char* pszTemp = pXML->getAttr( "BuddyName");
	if( pszTemp != nullptr)	
		m_strBuddyName = pszTemp; 

	INT32 temp;
	pXML->getAttr( "Default", &temp);
	m_bDefault = temp > 0 ? true : false;

	UINT32 r = 255, g = 255, b = 255, a = 255;

	if (pXML->getAttr("TabText_Normal_ColorR", &r) )
	{
		I3COLOR clr;

		pXML->getAttr("TabText_Normal_ColorG", &g);
		pXML->getAttr("TabText_Normal_ColorB", &b);
		pXML->getAttr("TabText_Normal_ColorA", &a);
			
		i3Color::Set(&clr, (UINT8)r, (UINT8)g, (UINT8)b, (UINT8)a);
		m_aTextColor[I3UI_TAB_STATE_NORMAL] = clr;

		pXML->getAttr("TabText_MouseOver_ColorR", &r);
		pXML->getAttr("TabText_MouseOver_ColorG", &g);
		pXML->getAttr("TabText_MouseOver_ColorB", &b);
		pXML->getAttr("TabText_MouseOver_ColorA", &a);
		
		i3Color::Set(&clr, (UINT8)r, (UINT8)g, (UINT8)b, (UINT8)a);
		m_aTextColor[I3UI_TAB_STATE_ONMOUSE] = clr;

		pXML->getAttr("TabText_Selected_ColorR", &r);
		pXML->getAttr("TabText_Selected_ColorG", &g);
		pXML->getAttr("TabText_Selected_ColorB", &b);
		pXML->getAttr("TabText_Selected_ColorA", &a);

		i3Color::Set(&clr, (UINT8)r, (UINT8)g, (UINT8)b, (UINT8)a);
		m_aTextColor[I3UI_TAB_STATE_PUSHED] = clr;

		pXML->getAttr("TabText_Disable_ColorR", &r);
		pXML->getAttr("TabText_Disable_ColorG", &g);
		pXML->getAttr("TabText_Disable_ColorB", &b);
		pXML->getAttr("TabText_Disable_ColorA", &a);

		i3Color::Set(&clr, (UINT8)r, (UINT8)g, (UINT8)b, (UINT8)a);
		m_aTextColor[I3UI_TAB_STATE_DISABLED] = clr;
	}
	else
	{
		UINT32 clr = i3UICaptionControl::getTextColor();
		SetTextColorByStateAll(clr);
	}

	return bResult;
}

void i3UITab::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UICaptionControl::CopyTo( pDest, method );

	I3ASSERT( i3::same_of<i3UITab*>(pDest));

	i3UITab* pTab = (i3UITab*)pDest;

	pTab->setGroupID( m_nGroupID);
	pTab->m_bDefault = m_bDefault;

	pTab->m_strBuddyName = m_strBuddyName;
	
	for (INT32 i = 0 ; i < I3UI_TAB_STATE_COUNT; ++i)
	{
		pTab->m_aTextColor[i] = this->m_aTextColor[i];
	}
}


UINT32 i3UITab::GetTextColorByStateUINT32( I3UI_TAB_STATE state) const
{
	const COLOR& clr = i3UITab::GetTextColorByState(state);
	return i3Color::ConvertRGBA32(const_cast<COLOR*>(&clr));
}

const COLOR& i3UITab::GetTextColorByState(I3UI_TAB_STATE state) const
{
	return m_aTextColor[state];
}

void i3UITab::SetTextColorByState(  I3UI_TAB_STATE state, UINT32 clr)
{
	COLOR cr; cr.SetRGBA(clr);
	i3UITab::SetTextColorByState(state, cr);
}

void i3UITab::SetTextColorByState(  I3UI_TAB_STATE state, const COLOR& clr)
{
	m_aTextColor[state] = clr;
	this->addState(I3UI_STATE_NEED_UPDATE);
}

void i3UITab::SetTextColorByStateAll( UINT32 clr)
{
	COLOR cr; cr.SetRGBA(clr);
	i3UITab::SetTextColorByStateAll(cr);
}

void i3UITab::SetTextColorByStateAll(const COLOR& clr)
{
	for (INT32 i = 0 ; i < I3UI_TAB_STATE_COUNT ; ++i)
	{
		m_aTextColor[i] = clr;
	}
	addState( I3UI_STATE_NEED_UPDATE);
}

// 오버로딩 가림 처리..
void i3UITab::SetTextColor(I3UI_TAB_STATE state, UINT32 clr )
{
	i3UITab::SetTextColorByState(state, clr);
}

void i3UITab::SetTextColor(I3UI_TAB_STATE state, const COLOR& clr)
{
	i3UITab::SetTextColorByState(state, clr);
}

#if defined( I3_DEBUG)
void i3UITab::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	i3UICaptionControl::GetTDKGridProperties( List);

	AddMFCProperty_BeginGroup( List, "i3UITab");
	AddMFCProperty_Int32( List, &m_nGroupID, "Group ID");
	AddMFCProperty_Bool( List, &m_bDefault, "Default", "초기화시에 이 탭이 선택됩니다.");
	AddMFCProperty_RcString( List, m_strBuddyName, "Buddy Window", "name of a frame window to be showed when this tab is pushed");

	AddMFCProperty_Color(List, &m_aTextColor[I3UI_TAB_STATE_NORMAL], "Text Color NORMAL");
	AddMFCProperty_Color(List, &m_aTextColor[I3UI_TAB_STATE_ONMOUSE], "Text Color MOUSEOVER");
	AddMFCProperty_Color(List, &m_aTextColor[I3UI_TAB_STATE_PUSHED], "Text Color SELECTED");
	AddMFCProperty_Color(List, &m_aTextColor[I3UI_TAB_STATE_DISABLED], "Text Color DISABLE");

	AddMFCProperty_EndGroup( List);
}

void i3UITab::OnTDKPropertyChanged( const char* szName)
{
	if(i3::generic_is_iequal( szName, "Default") )
	{
		if (i3::generic_is_equal(szName, "Text Color NORMAL") || 
			i3::generic_is_equal(szName, "Text Color MOUSEOVER") ||
			i3::generic_is_equal(szName, "Text Color SELECTED") ||
			i3::generic_is_equal(szName, "Text Color DISABLE") )
		{
			setModifyState();	// 이거 갖고 안되면 UpdateShapeState()를 호출해야됨..
		}
		else
		if( m_bDefault == true)
		{
			i3::vector<i3UITab*> tabList;
		
			GetTabsInTheSameGroup( tabList);

			for( size_t i=0; i< tabList.size(); i++)
			{
				i3UITab* pTab = tabList[i];

				if( pTab != this)
				{
					pTab->SetDefault( false);
				}
			}
		}
	}
	else
	{
		i3UICaptionControl::OnTDKPropertyChanged( szName);
	}
}
#endif	// I3_DEBUG


/////////////////////////////////////////////////////////////////////////////////////
extern "C" int i3UITab_init( lua_State* L)
{
	i3UITab* pTab = (i3UITab*)i3Lua::GetUserDataArg( L, 1);
	if( pTab != nullptr)
	{
		pTab->InitBuddy();
	}
	return 0;
}

extern "C" int i3UITab_SetTabState( LuaState * L)
{
	i3UITab * pTab = (i3UITab*) i3Lua::GetUserDataArg( L, 1);
	if( pTab != nullptr)
	{
//		I3UI_TAB_STATE state = (I3UI_TAB_STATE) i3Lua::GetIntegerArg( L, 2);
		I3PRINTLOG(I3LOG_FATAL, "SetTabState Function deleted because of not working. So, DO NOT USE this RIGHT NOW");
	}
	return 0;
}

extern "C" int i3UITab_Selected( LuaState * L)
{
	i3UITab * pTab = (i3UITab*) i3Lua::GetUserDataArg( L, 1);
	if( pTab != nullptr)
	{
		bool bPush = i3Lua::GetBooleanArg( L, 2)?true:false;

		if( bPush)
			pTab->OnTabSelected();
		else
			pTab->RemoveTabSelect();
	}

	return 0;
}

LuaFuncReg i3UITabGlue[] = 
{
	{"uiTab_init",						i3UITab_init},
	{"uiTab_setTabState",				i3UITab_SetTabState},
	{"uiTab_selected",					i3UITab_Selected},
	
	{nullptr, nullptr}
};

void i3UITab::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UITabGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UITabGlue[i].name, i3UITabGlue[i].func);
	}
}