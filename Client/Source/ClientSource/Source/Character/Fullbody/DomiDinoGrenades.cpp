#include "pch.h"
#include "DomiDinoGrenades.h"


DomiDinoGrenades::~DomiDinoGrenades() 
{ 
	DeleteAll(); 
}

void DomiDinoGrenades::Add(GAMEINFO_THROW_WEAPON_DOMI_DINO* obj)
{
	m_grenades.push_back(obj);
}

DomiDinoGrenades::iterator DomiDinoGrenades::Delete( iterator i) 
{  
	I3_SAFE_DELETE(*i);
	I3TRACE("[Domi] REMAIN POISON IN LIST %d\n", m_grenades.size() - 1);
	return m_grenades.erase(i);
}

void DomiDinoGrenades::DeleteAll()
{
	if (m_grenades.empty()) return;

	list<GAMEINFO_THROW_WEAPON_DOMI_DINO*>::iterator it;
	for (it = m_grenades.begin(); it != m_grenades.end(); it++)
		I3_SAFE_DELETE(*it);
	m_grenades.clear();
}

DomiDinoGrenades::iterator DomiDinoGrenades::begin() 
{ 
	return m_grenades.begin(); 
}

DomiDinoGrenades::iterator DomiDinoGrenades::end() 
{ 
	return m_grenades.end(); 
}

GAMEINFO_THROW_WEAPON_DOMI_DINO* DomiDinoGrenades::Find(INT32 grenade_slot) 
{
	struct fn 
	{
		fn(INT32 idx) : idx(idx){} INT32 idx;
		bool operator()(const GAMEINFO_THROW_WEAPON_DOMI_DINO* p) const { return p->getSlotIndex() == idx ;}
	};
	iterator it = std::find_if(m_grenades.begin(), m_grenades.end(), fn(grenade_slot));
	return ( it != m_grenades.end() ) ? *it : nullptr;
}

bool DomiDinoGrenades::empty() const 
{ 
	return m_grenades.empty(); 
}	

