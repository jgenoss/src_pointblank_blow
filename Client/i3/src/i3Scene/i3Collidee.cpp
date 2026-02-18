#include "i3SceneDef.h"
#include "i3Collidee.h"

I3_CLASS_INSTANCE( i3Collidee);

struct _TerrianTypeMetaInfo
{
	I3_TERRAIN_TYPE		m_type;
	const char*			m_name;
	const char*			m_nodePrefix;
};

static _TerrianTypeMetaInfo gs_terMetaInfo[] = 
{
	{ I3_TERRAIN_NA				,  "I3_TERRAIN_NA"				, 	"$NULL$"			},
	{ I3_TERRAIN_CONCRETE		,  "I3_TERRAIN_CONCRETE"		,	"CCR_"				},
	{ I3_TERRAIN_STEEL			,  "I3_TERRAIN_STEEL"			,	"MET_"				},
	{ I3_TERRAIN_GROUND			,  "I3_TERRAIN_GROUND"			,	"GND_"				},
	{ I3_TERRAIN_WOOD			,  "I3_TERRAIN_WOOD"			,	"WOOD_"				},
	{ I3_TERRAIN_SNOW			,  "I3_TERRAIN_SNOW"			,	"SNOW_"				},
	{ I3_TERRAIN_WATER_DEEP		,  "I3_TERRAIN_WATER_DEEP"		,	"WTD_"				},
	{ I3_TERRAIN_WATER_SHALLOW	,  "I3_TERRAIN_WATER_SHALLOW"	,   "WTS_"				},
	{ I3_TERRAIN_WET_CLAY		,  "I3_TERRAIN_WET_CLAY"		,	"CLAY_"				},
	{ I3_TERRAIN_GRASS			,  "I3_TERRAIN_GRASS"			,	"GRASS_"			},
	{ I3_TERRAIN_MARBLE			,  "I3_TERRAIN_MARBLE"			,	"MARB_"				},
	{ I3_TERRAIN_FALLLEAF		,  "I3_TERRAIN_FALLLEAF"		,	"LEAF_"				},	
	{ I3_TERRAIN_CONCRETE_THIN	,  "I3_TERRAIN_CONCRETE_THIN"	,	"TH_CCR_"			},
	{ I3_TERRAIN_STEEL_THIN		,  "I3_TERRAIN_STEEL_THIN"		,	"TH_MET_"			},
	{ I3_TERRAIN_WOOD_THIN		,  "I3_TERRAIN_WOOD_THIN"		,	"TH_WOOD_"			},
	{ I3_TERRAIN_MARBLE_THIN	,  "I3_TERRAIN_MARBLE_THIN"		,	"TH_MARB_"			},
	{ I3_TERRAIN_PAPER			,  "I3_TERRAIN_PAPER"			,	"PAP_"				},
	{ I3_TERRAIN_GLASS			,  "I3_TERRAIN_GLASS"			,	"GLASS_"			},
	{ I3_TERRAIN_PLASTIC		,  "I3_TERRAIN_PLASTIC"			,	"PLASTIC_"			},
	{ I3_TERRAIN_RUBBER			,  "I3_TERRAIN_RUBBER"			,	"RUB_"				},
	{ I3_TERRAIN_CLOTH			,  "I3_TERRAIN_CLOTH"			,	"CLOTH_"			},
	{ I3_TERRAIN_GROUND_THIN	,  "I3_TERRAIN_GROUND_THIN"		,	"TH_GND_"			},
	{ I3_TERRAIN_SNOW_THIN		,  "I3_TERRAIN_SNOW_THIN"		,	"TH_SNOW_"			},
	{ I3_TERRAIN_WET_CLAY_THIN	,  "I3_TERRAIN_WET_CLAY_THIN"	,	"TH_CLAY_"			},
	{ I3_TERRAIN_GRASS_THIN		,  "I3_TERRAIN_GRASS_THIN"		,	"TH_GRASS_"			},
	{ I3_TERRAIN_PAPER_THIN		,  "I3_TERRAIN_PAPER_THIN"		,	"TH_PAP_"			},
	{ I3_TERRAIN_FALLLEAF_THIN	,  "I3_TERRAIN_FALLLEAF_THIN"	,	"TH_LEAF_"			},
	{ I3_TERRAIN_GLASS_THIN		,  "I3_TERRAIN_GLASS_THIN"		,	"TH_GLASS_"			},
	{ I3_TERRAIN_PLASTIC_THIN	,  "I3_TERRAIN_PLASTIC_THIN"	,	"TH_PLASTIC_"		},
	{ I3_TERRAIN_RUBBER_THIN	,  "I3_TERRAIN_RUBBER_THIN"		,	"TH_RUB_"			},
	{ I3_TERRAIN_CLOTH_THIN		,  "I3_TERRAIN_CLOTH_THIN"		,	"TH_CLOTH_"			},
	{ I3_TERRAIN_BARBEDWIRE		,  "I3_TERRAIN_BARBEDWIRE"		,	"TH_BARBEDWIRE_"	},
	{ I3_TERRAIN_BLOOD			,  "I3_TERRAIN_BLOOD"			,	"BLOOD_"			},
	{ I3_TERRAIN_TEMP1			,  "I3_TERRAIN_TEMP1"			,	"TEMP1_"			},
	{ I3_TERRAIN_TEMP2			,  "I3_TERRAIN_TEMP2"			,	"TEMP2_"			},
	{ I3_TERRAIN_CLOTH_NO_DECAL	,  "I3_TERRAIN_CLOTH_NO_DECAL"	,	"ND_CLOTH_"			},
	{ I3_TERRAIN_STEEL_WIRE		,  "I3_TERRAIN_STEEL_WIRE"		,	"WR_MET_"			},
	{ I3_TERRAIN_WOOD_WIRE		,  "I3_TERRAIN_WOOD_WIRE"		,	"WR_WOORD_"			},
	{ I3_TERRAIN_PLASTIC_WIRE	,  "I3_TERRAIN_PLASTIC_WIRE"	,	"WR_PLASTIC_"		},
	{ I3_TERRAIN_NONE			,  "I3_TERRAIN_NONE"			,	"NONE_"				},
};

static const INT32 gs_numTerMetaInfo = sizeof(gs_terMetaInfo) / sizeof(gs_terMetaInfo[0]);


I3_EXPORT_SCENE const char* i3TerrainTypeToNodeNamePrefix(I3_TERRAIN_TYPE type)
{
	if(type >= gs_numTerMetaInfo)
		return nullptr;

	return gs_terMetaInfo[type].m_nodePrefix;
}

I3_EXPORT_SCENE I3_TERRAIN_TYPE i3TerrainTypeNameToType(const char* typeName)
{
	for(INT32 i=0; i<gs_numTerMetaInfo; i++)
	{
		if(!stricmp(typeName, gs_terMetaInfo[i].m_name))
			return (I3_TERRAIN_TYPE)i;
	}

	return I3_TERRAIN_NA;
}

I3_EXPORT_SCENE const char* i3TerrainTypeNameToNodeNamePrefix(const char* typeName)
{
	for(INT32 i=0; i<gs_numTerMetaInfo; i++)
	{
		if(!stricmp(typeName, gs_terMetaInfo[i].m_name))
			return gs_terMetaInfo[i].m_nodePrefix;	
	}

	return nullptr;
}

I3_EXPORT_SCENE const char* i3TerrainTypeToStr(I3_TERRAIN_TYPE type)
{
	if(type >= gs_numTerMetaInfo)
		return nullptr;
	
	return gs_terMetaInfo[type].m_name;
}
