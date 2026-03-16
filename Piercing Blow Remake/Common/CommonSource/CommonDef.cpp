#include "pch.h"			
#include "CommonDef.h"

const UINT32	g_ExpValueV2[] = {0,			1200,		3600,		7300,		12200,		18400,		25400,		33400,		42400,		53400,
								65400,		78400,		93400,		115400,		139400,		165400,		202400,		242400,		285400,		337400,
								393400,		453400,		544400,		641400,		745400,		855400,		994400,		1141400,	1297400,	1462400,	
								1649400,	1847400,	2055400,	2274400,	2548400,	2836400,	3138400,	3454400,	3843400,	4251400,	
								4678400,	5124400,	5629400,	6158400,	6710400,	7285400,	8169400,	9098400,	10072400,	11110400,	
								12173400,	100000000,
};

const UINT32	g_ExpValueV1[] = {0,			2000,		5000,		9000,		14000,		21000,		30000,		41000,		55000,		71000, \
								89000,		110000,		134000,		161000,		191000,		224000,		260000,		301000,		347000,		398000, \
								454000,		516000,		584000,		658000,		738000,		824000,		917000,		1017000,	1124000,	1238000, \
								1359000,	1510000,	1691000,	1902000,	2143000,	2414000,	2725000,	3076000,	3467000,	3898000, \
								4369000,	4890000,	5461000,	6082000,	6753000,	7474000,	8255000,	9106000,	10037000,	11058000, \
                                12179000,	100000000,
 };

const INT32 g_shufflevalue[][2] = {	{14,29},{645,32},{45,435},{234,2345},{45,2456},{432,2345},{864,12341},{123,879},{15,98},{25,5687},\
									{3245,2345},{4567,1234},{345,9870},{967,2123},{1234,87},{435,43},{26789,548},{9868,234},{45,65},{678,1234}\
}; 


const INT32 g_InitialRankUpPoint = 50000;		// �ϻ��̻� �ʵ����� ���� ����Ʈ

//�ܺο��� ���� -��ȹ�ǵ� 
const UINT32	g_TeamTimeTable[]		= {180, 300, 420, 300, 600, 900, 1200, 1500, 1800, 65536, 20 };

const UINT32	g_TeamKillCountTable[]	= {60, 80, 100, 120, 140, 160, 65536};

const UINT32	g_MissionRoundCountTable[]	= {3, 2, 3, 5, 7, 9, 65536}; //0�� ������� ���� 
//const UINT32	g_MissionRoundCountTable[]	= {0, 2, 3, 5, 7, 9, 65536}; //0�� ������� ���� 

namespace Weapon
{
	ITEM_TYPE	GetItemTypeFromClass(WEAPON_CLASS_TYPE WeaponClass)
	{
		switch(WeaponClass)
		{
		case WEAPON_CLASS_KNIFE:			return ITEM_TYPE_MELEE;
		case WEAPON_CLASS_HANDGUN:			return ITEM_TYPE_SECONDARY;
		case WEAPON_CLASS_ASSAULT:			return ITEM_TYPE_PRIMARY;
		case WEAPON_CLASS_SMG:				return ITEM_TYPE_PRIMARY;
		case WEAPON_CLASS_SNIPER:			return ITEM_TYPE_PRIMARY;
		case WEAPON_CLASS_SHOTGUN:			return ITEM_TYPE_PRIMARY;
		case WEAPON_CLASS_THROWING_GRENADE:	return ITEM_TYPE_THROWING1;
		case WEAPON_CLASS_THROWING_CONTAIN:	return ITEM_TYPE_THROWING2;
		case WEAPON_CLASS_MISSION:			return ITEM_TYPE_MISSION;
		case WEAPON_CLASS_MG:				return ITEM_TYPE_PRIMARY;
		case WEAPON_CLASS_GRENADESHELL:		return ITEM_TYPE_THROWING1;
		case WEAPON_CLASS_CIC:				return ITEM_TYPE_SECONDARY;
		case WEAPON_CLASS_DUALHANDGUN:		return ITEM_TYPE_SECONDARY;
		case WEAPON_CLASS_DUALKNIFE:		return ITEM_TYPE_MELEE;
		case WEAPON_CLASS_ROCKET_LAUNCHER:	return ITEM_TYPE_PRIMARY;
		case WEAPON_CLASS_OBJECT_GUN:		return ITEM_TYPE_PRIMARY;
		case WEAPON_CLASS_DUALSMG:			return ITEM_TYPE_PRIMARY;
		case WEAPON_CLASS_KNUCKLE:			return ITEM_TYPE_MELEE;
		case WEAPON_CLASS_BOW:				return ITEM_TYPE_SECONDARY;
		case WEAPON_CLASS_DINO:				return ITEM_TYPE_PRIMARY;
		case WEAPON_CLASS_THROWING_ATTACK:	return ITEM_TYPE_THROWING2;
		case WEAPON_CLASS_THROWING_HEAL:	return ITEM_TYPE_THROWING2;
		case WEAPON_SUBCLASS_ASSAULT:
		case WEAPON_SUBCLASS_SHOTGUN:
		case WEAPON_SUBCLASS_SNIPER:
		case WEAPON_SUBCLASS_SMG:
		case WEAPON_SUBCLASS_G_LAUNCHER:
		case WEAPON_SUBCLASS_BOW:			return ITEM_TYPE_SECONDARY;
		case WEAPON_CLASS_DUALSHOTGUN :		return ITEM_TYPE_PRIMARY;
		case WEAPON_CLASS_UNKNOWN:
		default					:			return ITEM_TYPE_UNKNOWN;
		}
	}

	WEAPON_SLOT_TYPE	GetUsageFromItemID(UINT32 ui32ItemID)
	{
		return GetUsageFromItemType( GET_ITEM_TYPE(ui32ItemID) );
	}

	WEAPON_SLOT_TYPE	GetUsageFromItemType(INT32 ItemType)
	{
		switch( ItemType )
		{
		case ITEM_TYPE_PRIMARY:		return WEAPON_SLOT_PRIMARY;
		case ITEM_TYPE_SECONDARY:	return WEAPON_SLOT_SECONDARY;
		case ITEM_TYPE_MELEE:		return WEAPON_SLOT_MELEE;
		case ITEM_TYPE_THROWING1:	return WEAPON_SLOT_THROWING1;
		case ITEM_TYPE_THROWING2:	return WEAPON_SLOT_THROWING2;
		case ITEM_TYPE_MISSION:		return WEAPON_SLOT_MISSION;
		default:					return WEAPON_SLOT_UNKNOWN;
		}
	}

};

namespace CashItem
{
	CASHITEM_USAGE_TYPE GetUsageFromItemType(INT32 ItemType)
	{
		switch( ItemType )
		{
		case ITEM_TYPE_MAINTENANCE	:
		case ITEM_TYPE_WRAP_PERIOD	:	return CASHITEM_USAGE_MAINTENANCE;
		case ITEM_TYPE_WRAP_COUNT	:	return CASHITEM_USAGE_EXPENDABLES;
		default						:	return CASHITEM_USAGE_UNKNOWN;
		}
	}

	CASHITEM_USAGE_TYPE GetUsageFromItemID( T_ItemID ItemID )
	{
		return GetUsageFromItemType( GET_ITEM_TYPE(ItemID) );
	}	
};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// ȣĪ �ý��� �Ӽ� ���̺�
const USER_TITLE_CONTEXT g_UserTitleContext[MAX_USER_TITLE] =
{
	{0,},
	{UTC_ALL,			0,						0},				// �� ���� �ܰ� ���� ȣĪ A
	{UTC_ALL,			0,						0},				// �� ���� �ܰ� ���� ȣĪ B
	{UTC_ALL,			0,						0},				// �� ���� �ܰ� ���� ȣĪ C
	{UTC_ALL,			0,						0},				// �� ���� �ܰ� ���� ȣĪ D

	{UTC_ASSAULT,		0/*G-36C Ext.*/,		0},				// 1�� ���� �ܰ� ȣĪ A
	{UTC_RIFLE,			0/*Dragunov G*/,		0},				// 1�� ���� �ܰ� ȣĪ B
	{UTC_SMG,			0/*Spectre W*/,			0},				// 1�� ���� �ܰ� ȣĪ C

	{UTC_ASSAULT,		0,						0},				// 2�� ���� �ܰ� ȣĪ A (Assault Rifle)
	{UTC_ASSAULT,		0/*G-36C*/,				0},				// 2�� ���� �ܰ� ȣĪ B (Assault Rifle)
	{UTC_ASSAULT,		0,						0},				// 2�� ���� �ܰ� ȣĪ C (Assault Rifle)
	{UTC_ASSAULT,		0/*K-201 S*/,			0},				// 2�� ���� �ܰ� ȣĪ D (Assault Rifle)
	{UTC_ASSAULT,		0/*K-201 S*/,			0},				// ������ (Assault Rifle)
	{UTC_ASSAULT,		0,						0/*��ĳ*/},		// ����� (Assault Rifle)

	{UTC_RIFLE,			0,						0},				// 2�� ���� �ܰ� ȣĪ A (Rifle)
	{UTC_RIFLE,			0/*Dragunov G*/,		0},				// 2�� ���� �ܰ� ȣĪ B (Rifle)
	{UTC_RIFLE,			0,						0},				// 2�� ���� �ܰ� ȣĪ C (Rifle)
	{UTC_RIFLE,			0/*L115A1*/,			0},				// 2�� ���� �ܰ� ȣĪ D (Rifle)
	{UTC_RIFLE,			0/*L115A1*/,			0},				// ������ (Rifle)
	{UTC_RIFLE,			0,						0/*��ĳ*/},		// ����� (Rifle)

	{UTC_SMG,			0,						0},				// 2�� ���� �ܰ� ȣĪ A (SMG)
	{UTC_SMG,			0/*Spectre W*/,			0},				// 2�� ���� �ܰ� ȣĪ B (SMG)
	{UTC_SMG,			0,						0},				// 2�� ���� �ܰ� ȣĪ C (SMG)
	{UTC_SMG,			0/*P90*/,				0},				// 2�� ���� �ܰ� ȣĪ D (SMG)
	{UTC_SMG,			0/*P90*/,				0},				// ������ (SMG)
	{UTC_SMG,			0,						0/*��ĳ*/},		// ����� (SMG)

	{UTC_KNIFE,			0,						0},				// 2�� ���� �ܰ� ȣĪ A (Knife)
	{UTC_KNIFE,			0/*Dual Knife*/,		0},				// 2�� ���� �ܰ� ȣĪ B (Knife)
	{UTC_KNIFE,			0,						0},				// 2�� ���� �ܰ� ȣĪ C (Knife)
	{UTC_KNIFE,			0/*Dual Knife*/,		0/*��ĳ*/},		// ������ (Knife)

	{UTC_HANDGUN,		0/*Dual Hand Gun*/,		0},				// 2�� ���� �ܰ� ȣĪ A (Hand Gun)
	{UTC_HANDGUN,		0,						0},				// 2�� ���� �ܰ� ȣĪ B (Hand Gun)
	{UTC_HANDGUN,		0/*Dual Hand Gun*/,		0},				// 2�� ���� �ܰ� ȣĪ C (Hand Gun)
	{UTC_HANDGUN,		0/*CIC*/,				0},				// ������ (Hand Gun)
	{UTC_HANDGUN,		0/*CIC*/,				0/*��ĳ*/},		// ����� (Hand Gun)

	{UTC_SHOTGUN,		0/*870MCS W*/,			0},				// 2�� ���� �ܰ� ȣĪ A (Shot Gun)
	{UTC_SHOTGUN,		0,						0},				// 2�� ���� �ܰ� ȣĪ B (Shot Gun)
	{UTC_SHOTGUN,		0/*870MCS W*/,			0},				// 2�� ���� �ܰ� ȣĪ C (Shot Gun)
	{UTC_SHOTGUN,		0/*Franchi SPAS-15*/,	0},				// ������ (Shot Gun)
	{UTC_SHOTGUN,		0/*Franchi SPAS-15*/,	0/*��ĳ*/},		// ����� (Shot Gun)

	{UTC_COMPOSITION,	0,						0},				// 2�� ���� �ܰ� ȣĪ A (Composition)
	{UTC_COMPOSITION,	0/*C-5*/,				0},				// 2�� ���� �ܰ� ȣĪ B (Composition)
	{UTC_COMPOSITION,	0/*C-5*/,				0},				// 2�� ���� �ܰ� ȣĪ C (Composition)
	{UTC_COMPOSITION,	0/*C-5*/,				0 },			// 2�� ���� �ܰ� ȣĪ D (Composition)
	{UTC_COMPOSITION,	0/*C-5*/,				0/*��ĳ*/},		// ������ (Composition)
};

const USER_TITLE_REQUIREMENT g_UserTitleRequirement[MAX_USER_TITLE] =
{	
	{0,},
	// �ʿ� �޴� 1,		�ʿ� �޴� 2,		�ʿ� �޴� 3				�ʿ��������޴�, ���
	{{{UTR_RIBBON,	1},	{UTR_NOTHING,	0},	{UTR_NOTHING,	0}},	{0,		0},		1},		// 1 �� ���� �ܰ� ���� ȣĪ A �Ϲݴ��
	{{{UTR_RIBBON,	2},	{UTR_NOTHING,	0},	{UTR_NOTHING,	0}},	{1,		0},		2},		// 2 �� ���� �ܰ� ���� ȣĪ B ���Ӵ��
	{{{UTR_RIBBON,	3},	{UTR_NOTHING,	0},	{UTR_NOTHING,	0}},	{2,		0},		3},		// 3 �� ���� �ܰ� ���� ȣĪ C Ư����
	{{{UTR_RIBBON,	4},	{UTR_NOTHING,	0},	{UTR_NOTHING,	0}},	{3,		0},		4},		// 4 �� ���� �ܰ� ���� ȣĪ D Ư�����

	{{{UTR_MASTER,	1},	{UTR_ENSIGN,	2},	{UTR_NOTHING,	0}},	{4,		0},		5},		// 5 1�� ���� �ܰ� ȣĪ A	���� �δ��
	{{{UTR_MASTER,	2},	{UTR_ENSIGN,	2},	{UTR_NOTHING,	0}},	{4,		0},		5},		// 6 1�� ���� �ܰ� ȣĪ B	���� �δ��
	{{{UTR_MASTER,	1},	{UTR_ENSIGN,	2},	{UTR_NOTHING,	0}},	{4,		0},		5},		// 7 1�� ���� �ܰ� ȣĪ C ���� �δ��

	{{{UTR_MASTER,	1},	{UTR_ENSIGN,	3},	{UTR_NOTHING,	0}},	{5,		0},		8},		// 8 2�� ���� �ܰ� ȣĪ A (Assault Rifle) �Ϲ� ���ݴ��
	{{{UTR_MASTER,	2},	{UTR_MEDAL,		6},	{UTR_NOTHING,	0}},	{8,		0},		12},	// 9 2�� ���� �ܰ� ȣĪ B (Assault Rifle) ���� ���ݴ��
	{{{UTR_MASTER,	4},	{UTR_MEDAL,		14},{UTR_NOTHING,	0}},	{9,		0},		17},	// 10 2�� ���� �ܰ� ȣĪ C (Assault Rifle) ���� ���ݴ��
	{{{UTR_MASTER,	3},	{UTR_MEDAL,		13},{UTR_NOTHING,	0}},	{10,	28},	21},	// 11 2�� ���� �ܰ� ȣĪ D (Assault Rifle) Ư�� ���� �屳
	{{{UTR_MASTER,	6},	{UTR_MEDAL,		21},{UTR_NOTHING,	0}},	{11,	0},		26},	// 12 ������ (Assault Rifle) ���� ���ְ�
	{{{UTR_MASTER,	7},	{UTR_MEDAL,		26},{UTR_NOTHING,	0}},	{11,	0},		31},	// 13 ����� (Assault Rifle) ������ �ͼ�

	{{{UTR_MASTER,	4},	{UTR_ENSIGN,	9},	{UTR_NOTHING,	0}},	{6,		0},		8},		// 14 2�� ���� �ܰ� ȣĪ A (Rifle)	�Ϲ� ���ݺ�
	{{{UTR_MASTER,	2},	{UTR_ENSIGN,	6},	{UTR_ENSIGN,	2}},	{14,	0},		12},	// 15 2�� ���� �ܰ� ȣĪ B (Rifle) ���� ���ݺ�
	{{{UTR_MASTER,	4},	{UTR_MEDAL,		14},{UTR_NOTHING,	0}},	{15,	0},		17},	// 16 2�� ���� �ܰ� ȣĪ C (Rifle) ���� ���ݺ�
	{{{UTR_MASTER,	3},	{UTR_MEDAL,		13},{UTR_NOTHING,	0}},	{16,	32},	21},	// 17 2�� ���� �ܰ� ȣĪ D (Rifle) Ư�� �����屳
	{{{UTR_MASTER,	6},	{UTR_MEDAL,		21},{UTR_NOTHING,	0}},	{17,	0},		26},	// 18 ������ (Rifle) ���� ���ְ�
	{{{UTR_MASTER,	3},	{UTR_MEDAL,		26},{UTR_NOTHING,	0}},	{18,	0},		31},	// 19 ����� (Rifle) ������ �ϻ���

	{{{UTR_MASTER,	1},	{UTR_ENSIGN,	6},	{UTR_NOTHING,	0}},	{7,		0},		8},		// 20 2�� ���� �ܰ� ȣĪ A (SMG) �Ϲ� ���ݴ��
	{{{UTR_MASTER,	4},	{UTR_MEDAL,		20},{UTR_NOTHING,	0}},	{20,	0},		12},	// 21 2�� ���� �ܰ� ȣĪ B (SMG) ���� ���ݴ��
	{{{UTR_MASTER,	4},	{UTR_MEDAL,		20},{UTR_NOTHING,	0}},	{21,	0},		17},	// 22 2�� ���� �ܰ� ȣĪ C (SMG) ���� ���ݴ��
	{{{UTR_MASTER,	6},	{UTR_ENSIGN,	13},{UTR_NOTHING,	0}},	{22,	42},	21},	// 23 2�� ���� �ܰ� ȣĪ D (SMG) Ư�� ���ݴ��
	{{{UTR_MASTER,	6},	{UTR_MEDAL,		28},{UTR_NOTHING,	0}},	{23,	0},		26},	// 24 ������ (SMG) ���� ���ְ�
	{{{UTR_MASTER,	7},	{UTR_MEDAL,		28},{UTR_NOTHING,	0}},	{24,	0},		31},	// 25 ����� (SMG) ��Ȱ�� ħ����

	{{{UTR_MEDAL,	1},	{UTR_ENSIGN,	9},	{UTR_NOTHING,	0}},	{5,		0},		8},		// 26 2�� ���� �ܰ� ȣĪ A (Knife) �Ϲ� ħ����
	{{{UTR_MASTER,	2},	{UTR_MEDAL,		6},	{UTR_NOTHING,	0}},	{26,	0},		12},	// 27 2�� ���� �ܰ� ȣĪ B (Knife) ���� ħ����
	{{{UTR_MASTER,	3},	{UTR_MEDAL,		14},{UTR_NOTHING,	0}},	{27,	0},		17},	// 28 2�� ���� �ܰ� ȣĪ C (Knife) Ư�� ħ���屳
	{{{UTR_MASTER,	6},	{UTR_MEDAL,		12},{UTR_NOTHING,	0}},	{28,	0},		31},	// 29 ������ (Knife) ħ�� ���ְ�

	{{{UTR_MASTER,	1},	{UTR_ENSIGN,	9},	{UTR_NOTHING,	0}},	{6,		0},		8},		// 30 2�� ���� �ܰ� ȣĪ A (Hand Gun) �Ϲ� ���
	{{{UTR_MASTER,	2},	{UTR_MEDAL,		6},	{UTR_NOTHING,	0}},	{30,	0},		12},	// 31 2�� ���� �ܰ� ȣĪ B (Hand Gun) ���� ���
	{{{UTR_MASTER,	3},	{UTR_MEDAL,		14},{UTR_NOTHING,	0}},	{31,	0},		17},	// 32 2�� ���� �ܰ� ȣĪ C (Hand Gun) Ư�� ����屳
	{{{UTR_MASTER,	4},	{UTR_MEDAL,		12},{UTR_NOTHING,	0}},	{32,	0},		26},	// 33 ������ (Hand Gun) ��� ���ְ�
	{{{UTR_MASTER,	6},	{UTR_MEDAL,		14},{UTR_NOTHING,	0}},	{33,	0},		31},	// 34 ����� (Hand Gun) ������ �����

	{{{UTR_MASTER,	1},	{UTR_ENSIGN,	6},	{UTR_NOTHING,	0}},	{7,		0},		8},		// 35 2�� ���� �ܰ� ȣĪ A (Shot Gun) �Ϲ� Ÿ�ݴ��
	{{{UTR_MASTER,	2},	{UTR_MEDAL,		8},	{UTR_NOTHING,	0}},	{35,	0},		12},	// 36 2�� ���� �ܰ� ȣĪ B (Shot Gun) ���� Ÿ�ݴ��
	{{{UTR_MASTER,	5},	{UTR_ENSIGN,	10},{UTR_NOTHING,	0}},	{36,	0},		17},	// 37 2�� ���� �ܰ� ȣĪ C (Shot Gun) Ư�� Ÿ�ݴ��
	{{{UTR_MASTER,	5},	{UTR_ENSIGN,	13},{UTR_NOTHING,	0}},	{37,	0},		26},	// 38 ������ (Shot Gun) Ÿ�� ���ְ�
	{{{UTR_MASTER,	6},	{UTR_MEDAL,		15},{UTR_NOTHING,	0}},	{38,	0},		31},	// 39 ����� (Shot Gun) ������ �ı���

	{{{UTR_MASTER,	1},	{UTR_MEDAL,		6},	{UTR_NOTHING,	0}},	{7,		0},		8},		// 40 2�� ���� �ܰ� ȣĪ A (Composition) �Ϲ� ���Ĵ��
	{{{UTR_MASTER,	1},	{UTR_MEDAL,		7},	{UTR_NOTHING,	0}},	{40,	0},		12},	// 41 2�� ���� �ܰ� ȣĪ B (Composition) ���� ���Ĵ��
	{{{UTR_MASTER,	3},	{UTR_ENSIGN,	10},{UTR_NOTHING,	0}},	{41,	0},		17},	// 42 2�� ���� �ܰ� ȣĪ C (Composition) ���� ���Ĵ��
	{{{UTR_MASTER,	5},	{UTR_ENSIGN,	13},{UTR_NOTHING,	0}},	{42,	0},		21},	// 43 2�� ���� �ܰ� ȣĪ D (Composition) Ư�� �����屳
	{{{UTR_MASTER,	6},	{UTR_MEDAL,		15},{UTR_NOTHING,	0}},	{43,	0},		31},	// 44 ������ (Composition) ���� ���ְ�
};



//#ifdef I3_WINDOWS // for BitRotateEncript()

// ������ �����ϸ� ����
INT16 g_Encryption(char * pPacket, INT16 iPacketsize, INT16 iBuffersize )
{

	N_GAME_HEADER	* pHeader = (N_GAME_HEADER *)pPacket; 
	UINT8			* pStartPos = (UINT8*)&pPacket[sizeof(N_GAME_HEADER)]; 

	UINT32 key = pHeader->_Size % 6; 
	BitRotateEncript( pStartPos, (iPacketsize - sizeof(N_GAME_HEADER)), key + 1);

	return iPacketsize; 
}

INT16 g_Decryption(char * pPacket, INT16 iPacketsize, INT16 iBuffersize)
{
	N_GAME_HEADER	* pHeader = (N_GAME_HEADER *)pPacket; 
	UINT8			* pStartPos = (UINT8*)&pPacket[sizeof(N_GAME_HEADER)]; 

	UINT32 key = pHeader->_Size % 6; 
	BitRotateDecript( pStartPos, (iPacketsize - sizeof(N_GAME_HEADER)), key + 1);

	return iPacketsize; 
}

void ShuffleHitPart ( UINT8* pOut, UINT32 ui32Key ) // ũ��� CHARA_HIT_MAXCOUNT�� �����Ǿ� ����
{	
	//1 ���� ���� �մϴ�. 
	UINT32 ui32StartPos =  ui32Key % CHARA_HIT_MAXCOUNT;
	for( UINT32 i = 0; i < CHARA_HIT_MAXCOUNT; i++ ) 
	{
		pOut[i] = (UINT8)ui32StartPos;	

		ui32StartPos++; 
		if(ui32StartPos >= CHARA_HIT_MAXCOUNT)ui32StartPos = 0; 
	}

	//2 ��Ģ��� ���� �̵� �մϴ�. 
	if( ui32Key > 10)
	{
		INT32  iRotate =  ui32Key / 10;
		iRotate = iRotate % SHUFFLE_VALUE_MAX;

		for(INT32 i = 0; i < iRotate; i++)
		{
			INT32 iSwap1 = g_shufflevalue[i][0] % CHARA_HIT_MAXCOUNT; 
			INT32 iSwap2 = g_shufflevalue[i][1] % CHARA_HIT_MAXCOUNT; 
			
			UINT8 iTemp  = pOut[iSwap1]; 
			pOut[iSwap1] = pOut[iSwap2]; 
			pOut[iSwap2] = iTemp;
		}
	}

	return; 
}

UINT32	CalcHeadShotDamage( PBYTE pShuffledPartsArray, UINT32 ui32SizeOfShuffledParts,  UINT32 ui32MyRespawnCnt, UINT32 ui32SlotIdx )
{
	I3ASSERT( ui32SizeOfShuffledParts == CHARA_HIT_MAXCOUNT );
	I3ASSERT( ui32SizeOfShuffledParts != 0 );

	return 200 + pShuffledPartsArray[ ui32MyRespawnCnt % ui32SizeOfShuffledParts ] - pShuffledPartsArray[ (ui32SlotIdx + (ui32MyRespawnCnt*2))% ui32SizeOfShuffledParts ];
}

UINT32	CalcAnimObjPath(  PBYTE pShuffledPartsArray, UINT32 ui32RoundNum,INT32 i32TeamIdx ,INT32 i32OldPath)
{
	UINT32 i32Group = ui32RoundNum%2;
	UINT32 ui32Key = pShuffledPartsArray[ui32RoundNum]%2;
	UINT32 ui32Path = 0;
	
	if (i32OldPath < 1)
	{
		if (i32Group == 0) //¦������
		{
			if (i32TeamIdx == 0)			
			{//Team RED
				ui32Path = ui32Key +1;		//RESPAWN_HELICOPTER_PATH_RED1  1
			}								//RESPAWN_HELICOPTER_PATH_RED2  2
			else							
			{//Team BLUD
				ui32Path = ui32Key +3;		//RESPAWN_HELICOPTER_PATH_BLUE1 3
			}								//RESPAWN_HELICOPTER_PATH_BLUE2 4
		}
		else
		{
			if (i32TeamIdx == 0)			
			{//Team RED
				ui32Path = ui32Key +5;		//RESPAWN_HELICOPTER_PATH_RED1  5
			}								//RESPAWN_HELICOPTER_PATH_RED2  6
			else							
			{//Team BLUE
				ui32Path = ui32Key +7;		//RESPAWN_HELICOPTER_PATH_BLUE1 7
			}								//RESPAWN_HELICOPTER_PATH_BLUE2 8
		}
	}
	else
	{
		INT32 Key = i32OldPath%2;
		if (Key == 0)
		{
			ui32Path = i32OldPath -1;
		}
		else
		{
			ui32Path = i32OldPath +1;
		}
	}

	return ui32Path;
}

//#endif // I3_WINDOWS