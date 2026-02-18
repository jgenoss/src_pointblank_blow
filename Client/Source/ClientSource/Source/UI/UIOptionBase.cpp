#include "pch.h"

#include "UIOptionBase.h"

I3_CLASS_INSTANCE( UIOptionBase);

UIOptionBase::UIOptionBase()
{
	init_key_list();
}

bool UIOptionBase::g_bKeyItemInit;

UIOptionBase::~UIOptionBase()
{
}
void UIOptionBase::init_key_list()
{	
	if(g_bKeyItemInit == true)  return; 
	g_bKeyItemInit = true;

	base_none_key._nDiviceType = -1;
	base_none_key._nState = 0;
}

void UIOptionBase::entrance_scn()
{
	OnEntranceStart();
}

void UIOptionBase::show_scn()
{
	SetVisible(true);
	GetScene(0)->Attach(false);
}

void UIOptionBase::hide_scn()
{
	SetVisible(false);
}

void UIOptionBase::exit_scn()
{
	OnExitStart();
}

bool UIOptionBase::OnEntranceStart( void * pArg1, void * pArg2)
{
	SetEnable( true );
	return true;
}

bool UIOptionBase::OnExitStart()
{
	SetEnable( false );
	return true;
}

//////////////////////////////////////////////////////////////////////////

void KeyItem::set_default() 
{	
	if(type == ITEM)	
		value = g_pEnvSet->GetDefaultGameKeyMap(menu);	
}

void KeyItem::load_value()	
{	
	value = g_pEnvSet->m_KeyMapArray[ menu ];	
}

void KeyItem::save_value()
{	
	g_pEnvSet->m_KeyMapArray[ menu ] = value;
}

bool KeyItem::match_value()
{ 
	const GAME_KEY_STRUCT & src = g_pEnvSet->m_KeyMapArray[ menu ];
	return ( value._nDiviceType == src._nDiviceType && value._nState == src._nState );
}

bool KeyItem::duplicate_keyItem(UINT32 key, INT8 device_type)
{
	if(key == value._nState && device_type == value._nDiviceType){
		return true;
	}
	return false;
}