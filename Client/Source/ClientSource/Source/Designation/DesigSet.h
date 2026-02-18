#pragma once

#define DESIG_EFFECT_COUNT	2

struct DesigSet : public i3::class_object_pool<DesigSet>
{
	i3::rc_wstring			wstrName;
	i3::rc_wstring			wstrNameE;
	i3::rc_wstring			wstrExplain;

	DESIGNATION_APPLY_TYPE	applyType = DESIGNATION_APPLY_TYPE_ALL;
	INT32					desigSetUIIndex = 0;
	INT32					desigSetType = 0;

	UINT8					addEffect[DESIG_EFFECT_COUNT] = { 0 };
	UINT8					plusEffect[DESIG_EFFECT_COUNT] = { 0 };
	UINT8					minusEffect[DESIG_EFFECT_COUNT] = { 0 };
};

void	InitDesigSet(DesigSet* desigSet);
