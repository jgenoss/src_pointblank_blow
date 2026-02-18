#ifndef _DOMI_DINO_GRENADES_H_
#define _DOMI_DINO_GRENADES_H_

#include <list>

class DomiDinoGrenades
{
public:
	typedef std::list<GAMEINFO_THROW_WEAPON_DOMI_DINO*>::iterator	iterator;

	DomiDinoGrenades() {}
	~DomiDinoGrenades();

	void		Add(GAMEINFO_THROW_WEAPON_DOMI_DINO* obj);
	iterator	Delete( iterator i);
	void		DeleteAll();
	iterator	begin();
	iterator	end();
	bool		empty() const;
	GAMEINFO_THROW_WEAPON_DOMI_DINO* Find(INT32 grenade_slot);
	
private:
	std::list<GAMEINFO_THROW_WEAPON_DOMI_DINO*> m_grenades; 
};


#endif