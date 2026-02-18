#include "pch.h"
#include "AvatarDefine.h"


static AVT_PARTINFO s_PartInfo[ EQUIP::ePART_COUNT] =
{
	//							// Res folder	// BoneRef name			// Property name
	{	AVT_PART_STYLE_SKIN,	"Upper",		"",						"Upper"},	// ePART_VEST,		// 상체
	{	AVT_PART_STYLE_SKIN,	"Lower",		"",						"Lower"},	// ePART_PANT,		// 하체
	{	AVT_PART_STYLE_SKIN,	"Glove",		"",						"Glove"},	// ePART_GLOVE,		// 손
	{	AVT_PART_STYLE_EXCHANGE,"Mask",			"",						""},		// ePART_CHARA,		// 캐릭터	// 캐릭터는 CharaInfo에서 찾는다.
	{	AVT_PART_STYLE_EQUIP,	"Headgear",		"Bone_HeadGearAttach",	"Head"},	// ePART_HEADGEAR,		// 머리 장비품
	{	AVT_PART_STYLE_EQUIP,	"Facegear",		"Bone_FaceGearAttach",	"Facegear"},	// ePART_FACEGEAR,		// 얼굴 장비
	{	AVT_PART_STYLE_EQUIP,	"Holster",		"Bone_HolsterAttach",	"Holster"},	// ePART_HOLSTER,	// 총집
	{	AVT_PART_STYLE_EQUIP,	"Belt",			"Bone_BeltAttach",		"Belt"},	// ePART_BELT,		// 벨트
	{	AVT_PART_STYLE_FULLSKIN,"Skin",			"",						"Skin"},	// ePART_SKIN
	{	AVT_PART_STYLE_EQUIP,	"Beret",		"Bone_HeadGearAttach",	"Beret"},	// ePART_BERET
};

static AVATAR::TEXINFO	s_PartTexInfo1P[ EQUIP::ePART_COUNT] =
{
	{ EQUIP::ePART_VEST,		0,	1024,		1024,	512},	// Upper (1인칭)
	{ EQUIP::ePART_PANT,		0,	0,			0,		0},		// Lower
	{ EQUIP::ePART_GLOVE,		1024, 1024,		1024,	512},	// Glove (3인칭)
	{ EQUIP::ePART_CHARA,		0,	0,			0,		0},		// Mask
	{ EQUIP::ePART_HEADGEAR,	0,	0,			0,		0},		// Headgear
	{ EQUIP::ePART_FACEGEAR,	0,	0,			0,		0},		// Facegear
	{ EQUIP::ePART_HOLSTER,		0,	0,			0,		0},		// Holster
	{ EQUIP::ePART_BELT,		0,	0,			0,		0},		// Belt
	{ EQUIP::ePART_SKIN,		0,	0,			0,		0},		// Skin
	{ EQUIP::ePART_BERET,		0,	0,			0,		0},		// Beret
};

static AVATAR::TEXINFO	s_PartTexInfo3P[ EQUIP::ePART_COUNT] =
{
	{ EQUIP::ePART_VEST,		0,	0,			1024, 1024},	// Upper
	{ EQUIP::ePART_PANT,		1024, 0,		1024, 1024},	// Lower
	{ EQUIP::ePART_GLOVE,		1024, 1024,		1024, 512},		// Glove (3인칭)
	{ EQUIP::ePART_CHARA,		0,	1536,		1024, 512},		// Mask
	{ EQUIP::ePART_HEADGEAR,	1024, 1536,		512, 512},		// Headgear
	{ EQUIP::ePART_FACEGEAR,	1536, 1536,		256, 256},		// Facegear
	{ EQUIP::ePART_HOLSTER,		1792, 1536,		256, 256},		// Holster
	{ EQUIP::ePART_BELT,		1536, 1792,		256, 256},		// Belt
	{ EQUIP::ePART_SKIN,		0,	0,			0,		0},		// Skin
	{ EQUIP::ePART_BERET,		0,	0,			0,		0},		// Beret
};

AVT_PART_STYLE AVATAR::getPartStyle( EQUIP::ePART part)
{
	I3_BOUNDCHK( part, EQUIP::ePART_COUNT);

	return s_PartInfo[ part].m_Style;
}

const i3::fixed_string&	AVATAR::getPartName( EQUIP::ePART part) 
{
	I3_BOUNDCHK( part, EQUIP::ePART_COUNT);

	return s_PartInfo[ part].m_strName;
}

const AVT_PARTINFO *	AVATAR::getPartInfo( EQUIP::ePART part)
{
	I3_BOUNDCHK( part, EQUIP::ePART_COUNT);

	return &s_PartInfo[ part];
}

const AVATAR::TEXINFO *	AVATAR::getPartTexInfo( VIEW view, EQUIP::ePART part)
{
	TEXINFO * pTexInfo;

	I3_BOUNDCHK( view, 2);
	I3_BOUNDCHK( part, EQUIP::ePART_COUNT);

	if( view == VIEW_1P)
		pTexInfo = s_PartTexInfo1P;
	else
		pTexInfo = s_PartTexInfo3P;

	return &pTexInfo[ part];
}

const i3::fixed_string& AVATAR::getDataBaseName( EQUIP::ePART part)
{
	I3_BOUNDCHK( part, EQUIP::ePART_COUNT);
	return s_PartInfo[ part].m_strDataBaseName;
}

void AVATAR::SetTexturePathName(const char* szPathName, VIEW view, EQUIP::ePART part)
{
	I3_BOUNDCHK( view, 2);
	I3_BOUNDCHK( part, EQUIP::ePART_COUNT);

	if( view == VIEW_1P)
		s_PartTexInfo1P[part].m_strPathName = szPathName;
	else
		s_PartTexInfo3P[part].m_strPathName = szPathName;
}
