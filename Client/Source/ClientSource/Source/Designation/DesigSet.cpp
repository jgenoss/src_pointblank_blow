#include "pch.h"
#include "DesigSet.h"



void	InitDesigSet(DesigSet* desigSet)
{
	desigSet->applyType = DESIGNATION_APPLY_TYPE_ALL;
	desigSet->desigSetUIIndex = 0;
	desigSet->desigSetType = -1;

	::memset(desigSet->addEffect, 0, sizeof(UINT8) * DESIG_EFFECT_COUNT);
	::memset(desigSet->plusEffect, 0, sizeof(UINT8) * DESIG_EFFECT_COUNT);
	::memset(desigSet->minusEffect, 0, sizeof(UINT8) * DESIG_EFFECT_COUNT);
}

