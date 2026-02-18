#pragma once

class i3Quad;

enum WEAPON_TAG_TYPE
{ 
	WEP_TAG_SKILL_SHOP = 0, 
	WEP_TAG_DUMMY, 
	WEP_TAG_SENTYGUN, 
	WEP_TAG_OBJ_HP, 
	WEP_TAG_OBJ_AMMO,

	//hp hud
	TAG_OBJ_HP,

	WEP_TAG_NONE
};

bool	SetTagToWeapon( i3Quad*& pQuad, WEAPON_TAG_TYPE type );

