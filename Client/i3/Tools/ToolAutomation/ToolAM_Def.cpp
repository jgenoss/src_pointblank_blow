
#include "ToolAMPCH.h"
#include "ToolAM_Def.h"


namespace ToolAM
{
	static const char* gs_heroName[2][ECHR_HERO_MAX] = 
	{
		{ "SWAT_Male", "Bella", "Chou", "D-Fox", "Hide", "SWAT_Female",	"Leopard", "REBEL_Male", "REBEL_Female", "ViperRed" }, // current name
		{ "AcidPol",   "Bella", "Chou", "D-Fox", "Hide", "KeenEyes",	"Leopard", "RedBulls",	 "Tarantula",	 "ViperRed" }, // old name
	};

	const char* GetHeroName(DWORD hero, bool oldName) 
	{ 
		return gs_heroName[oldName ? 1 : 0][hero]; 
	}

	const char* GetOldHeroNameByName(const char* heroName)
	{	
		for(INT32 i=0; i<ECHR_HERO_MAX; i++)
		{
			if(!stricmp(gs_heroName[0][i], heroName))
				return gs_heroName[1][i];
		}
		return NULL;
	}

} // end of namespace ToolAM
