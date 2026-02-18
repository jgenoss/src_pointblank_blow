#pragma once

#include <baseTsd.h>
#include <string>
#include <map>


namespace EQUIP{

	//	장비 부위
	enum ePART
	{
		ePART_UNKNOWN = -1,

		ePART_VEST,		// 상체
		ePART_PANT,		// 하체
		ePART_GLOVE,	// 손
		ePART_CHARA,	// 캐릭터 얼굴

		ePART_BODY,

		ePART_HEADGEAR = ePART_BODY,	// 머리 장비품
		ePART_FACEGEAR,	// 얼굴
		ePART_HOLSTER,	// 총집
		ePART_BELT,		// 벨트

		ePART_COUNT
	};

	enum eABILITY 
	{
		eABILITY_C4SPEEDKIT = 0,
		eABILITY_CEBONUS,
		eABILITY_CROSSHAIRMIN,
		eABILITY_DAMAGE,
		eABILITY_DAMAGERATE,		
		eABILITY_DEFENSE_ARM,
		eABILITY_DEFENSE_BODY,
		eABILITY_DEFENSE_FOOT,
		eABILITY_DEFENSE_HAND,
		eABILITY_DEFENSE_LEG,
		eABILITY_DETECTC5,
		eABILITY_FALLDAMAGE,
		eABILITY_HPINCREASE,
		eABILITY_PROTECTION_RATE,
		eABILITY_INVINCIBLE_TIME,
		eABILITY_MOVESPEED_BACK,		// 0~1 %
		eABILITY_MOVESPEED_DIAGONAL,	// 0~1 %
		eABILITY_MOVESPEED_FRONT,		// 0~1 %
		eABILITY_MOVESPEED_SIDE,		// 0~1 %
		eABILITY_PROTECTIONTIME,
		eABILITY_RECOILHORZCOUNT,
		eABILITY_RESPAWNTIME,
		eABILITY_STBONUS,
		eABILITY_MOVESOUND_VOL,
		eABILITY_ACCURACY,
		eABILITY_FIREDELAY,
		eABILITY_MAXBULLET,
		eABILITY_PENETRATE,
		eABILITY_RANGE,
		eABILITY_RELOADTIME,
		eABILITY_SWAPTIME,
		eABILITY_THROWSPEED,
		eABILITY_THROWCOUNT,
		

		eABILITY_COUNT
	};
};

//////////////////////////////////////////////////////////////////////////////////
//
// A Equipment Information
//
//////////////////////////////////////////////////////////////////////////////////

struct EquipmentInfo
{
	T_ItemID	m_TItemID;

	REAL32		m_ar32Ability [ EQUIP::eABILITY_COUNT ];

	EquipmentInfo() { ClearAll(); }

	void ClearAll(void)
	{
		m_TItemID = 0;
		ClearAbility();
	}

	void ClearAbility(void)
	{
		i3mem::FillZero(m_ar32Ability, sizeof(m_ar32Ability) );
	}

	REAL32	GetAbilityValue( EQUIP::eABILITY ability) const
	{
		I3ASSERT(ability < EQUIP::eABILITY_COUNT);
		if(ability < EQUIP::eABILITY_COUNT)
			return m_ar32Ability[ability];
		return 0.0f;
	}

	REAL32 GetDamageReduceRate(CHARA_HIT_PART eHitPart)
	{
		switch( eHitPart )
		{
		case CHARA_HIT_NECK	:
		case CHARA_HIT_LCLAVICLE :
		case CHARA_HIT_RCLAVICLE :
		case CHARA_HIT_BODY1 :
		case CHARA_HIT_BODY2 :
		case CHARA_HIT_BODY3 :
		case CHARA_HIT_ROOT	:
			return m_ar32Ability[EQUIP::eABILITY_DEFENSE_BODY];
			break;

		case CHARA_HIT_RUARM:
		case CHARA_HIT_RDARM:
		case CHARA_HIT_LUARM:
		case CHARA_HIT_LDARM:
			return m_ar32Ability[EQUIP::eABILITY_DEFENSE_ARM];
			break;

		case CHARA_HIT_LULEG:
		case CHARA_HIT_LDLEG:
		case CHARA_HIT_RULEG:
		case CHARA_HIT_RDLEG:
			return m_ar32Ability[EQUIP::eABILITY_DEFENSE_LEG];
			break;
		case CHARA_HIT_LFOOT:
		case CHARA_HIT_RFOOT:			//	발 데미지
			return m_ar32Ability[EQUIP::eABILITY_DEFENSE_FOOT];
			break;

		case CHARA_HIT_LHAND:
		case CHARA_HIT_RHAND:			//	손 데미지
			return m_ar32Ability[EQUIP::eABILITY_DEFENSE_HAND];
			break;
		}

		return 0.0f;
	}
};

//////////////////////////////////////////////////////////////////////////////////
//
// Equipment Table manager class
//
//////////////////////////////////////////////////////////////////////////////////

struct IDispatch; // forward definition
class EquipmentTable
{
	static EquipmentTable m_Instance;

	// All loaded equipment data
	std::map<UINT32, EquipmentInfo>  m_mapEquipmentTable[2];
	
	// National Code on config file
	UINT8 m_ui8NationalCode;

	// National Code String name to parse xml column 
	std::string m_strNationalColumnName; 

public:
	// Constructor / Destructor
	EquipmentTable(UINT8 iNationalCode = 1);
	~EquipmentTable(void);

public:
	// Get singleton Instance
	static EquipmentTable&		GetInstance() { return m_Instance; }

public:
// external interface
	// Find a equipment item using key value
	bool			Find(const UINT32 ui32ItemID, EquipmentInfo& Info, PB_VER ePBVer = PB_VER_V15 );
	EquipmentInfo*	Find(const UINT32 ui32ItemID, PB_VER ePBVer = PB_VER_V15 );

	// Parse xml file to load weapon damage value
	void ParseEquipmentTable(IDispatch*	pNode);

	// Set national Code
	void SetNationalCode(UINT8 ui8NationalCode) { m_ui8NationalCode = ui8NationalCode; 
										SetNationalColumnName(ui8NationalCode); } 

private:
// internal function
	// Insert a character damage information to the map
	bool Insert( UINT32 ui32ItemID, EquipmentInfo pInfo, PB_VER ePBVer );
	
	// To parse specify national data
	void SetNationalColumnName(const UINT8 ui8NationalCode);
};

extern std::string s_strEquipAbilityField[ EQUIP::eABILITY_COUNT];