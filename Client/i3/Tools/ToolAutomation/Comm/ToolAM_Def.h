
#ifndef _TOOLAM_DEF_H_
#define _TOOLAM_DEF_H_

#include <string>
#include <vector>


namespace ToolAM
{

	enum ECHR_HERO
	{
		ECHR_HERO_SWATMALE,			// ACIDPOL,
		ECHR_HERO_BELLA,
		ECHR_HERO_CHOU,
		ECHR_HERO_DFOX,				//D-FOX
		ECHR_HERO_HIDE,
		ECHR_HERO_SWATFEMALE,		// KEENEYES,
		ECHR_HERO_LEOPARD,
		ECHR_HERO_REBEL_MALE,		// REDBULLS,
		ECHR_HERO_REBEL_FEMALE,		// TARANTULA,
		ECHR_HERO_VIPERRED,
		ECHR_HERO_MAX
	};

	const char* GetHeroName(DWORD hero, bool oldName=false);
	const char* GetOldHeroNameByName(const char* heroName);

	typedef std::string						Str;
	typedef std::vector<Str>				StrVec;


} // end of namespace ToolAM


#endif

