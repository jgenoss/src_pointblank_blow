#pragma once

#include <baseTsd.h>
#include <string>
#include <map>

#define DS_MAX_DINO_WEAPON		3	// Primary, Secondary , Extension(Dash, Trample..)

//////////////////////////////////////////////////////////////////////////////////
//
// A Character damage rate information
//
//////////////////////////////////////////////////////////////////////////////////
struct CharacterInfo
{
	// Character ItemID / HP
	T_ItemID	m_TItemID;
	UINT16		m_ui16HP;

	// Dino Weapon ItemID
	T_ItemID	m_TDinoWeaponID[DS_MAX_DINO_WEAPON];

	// Mode damage rate
	enum Mode
	{
		UndefinedMode = 0,
		HeadHunter,
		SuddenDeath,
	};
	
	Mode		m_eMode;

	// Gender Type
	enum Gender
	{
		Male = 0, 
		Female = 1,
		Dinosaur = 2,
		Skinny_Male = 3,
		UndefinedGender = 4,
	};
	Gender		m_eGenderType;

	// Damage rate to the hit position
	REAL32 		m_fArm;
	REAL32 		m_fBody;
	REAL32 		m_fFoot;
	REAL32		m_fHand;
	REAL32		m_fHead;
	REAL32		m_fNeck;
	REAL32		m_fLeg;
	REAL32		m_fOther;	// dino only

	// Character Speed
	REAL32		m_fFrontSpeed;

	// cam IK Hieght
	VEC4D		m_CamIKHeight;

	// Constructor
	CharacterInfo()  {	Clear();	}
	
	REAL32		m_fSpaceFrontSpeed;
	
	// To initialize in recursive call
	void Clear() 
	{
		m_TItemID	= 0;
		m_ui16HP	= 0;
		i3mem::FillZero( m_TDinoWeaponID, sizeof(m_TDinoWeaponID) );
		
		m_eMode = UndefinedMode;
		m_eGenderType = UndefinedGender;
		m_fArm = m_fBody = m_fFoot = m_fHand = m_fHead = m_fNeck = m_fLeg = m_fOther = 0.0f;
		m_fFrontSpeed = 0.0f;
		m_fSpaceFrontSpeed = 0.0f;
		i3Vector::Set( &m_CamIKHeight, 0.0f, 0.0f, 0.0f, 0.0f );
	}

	REAL32 GetHitPositionRate(CHARA_HIT_PART eCharHitPart);

	bool CheckDinoWeapon(T_ItemID TDinoWeaponID)
	{
		for(INT32 i = 0 ; i < DS_MAX_DINO_WEAPON ; ++i)
		{
			if(m_TDinoWeaponID[i] == TDinoWeaponID)
				return true;
		}

		return false;
	}
};

//////////////////////////////////////////////////////////////////////////////////
//
// Character Table manager class
//
//////////////////////////////////////////////////////////////////////////////////

struct IDispatch; // forward definition
class CharacterTable
{
	static CharacterTable    m_Instance;
	
	// All loaded Character data
	std::map<UINT32, CharacterInfo>  m_mapCharacterTable;

	// National Code on config file
	UINT8 m_ui8NationalCode;

	// National Code String name to parse xml column 
	std::string m_strNationalColumnName; 

	// Mode Damage Rate
	CharacterInfo	m_HeadHunterModeDamageRate;
	CharacterInfo	m_SuddenDeathModeDamageRate;

public:
	// Constructor / Destructor
	CharacterTable(UINT8 iNationalCode = 1);
	~CharacterTable(void);

public:
// Get singleton Instance
	static CharacterTable&		GetInstance() { return m_Instance;	}

public:
// external interface
	// Find a character damage rate using key value
	bool			Find(const UINT32 ui32CharacterIdx, CharacterInfo& Info);
	CharacterInfo*	Find(const UINT32 ui32CharacterIdx);	

	// Parse xml file to load weapon damage value
	void ParseCharacterTable(IDispatch*	pNode);

	// Set national Code
	void SetNationalCode(UINT8 ui8NationalCode) { m_ui8NationalCode = ui8NationalCode; 
										SetNationalColumnName(ui8NationalCode); } 

	// Get Hit Position Rate about the character hit part
	double GetHitPositionRate(CHARA_HIT_PART eCharacterHitPart); 

	// Check Dino Weapon
	bool CheckDinoWeapon(const UINT32 ui32CharaID, const UINT32 ui32WeaponID );

	// Get damage rate in play mode
	CharacterInfo	GetHeadHunterModeDamageRate() { return m_HeadHunterModeDamageRate; }
	CharacterInfo	GetSuddenDeathModeDamageRate() { return m_SuddenDeathModeDamageRate; }

	// Set damage rate in play mode
	void GetHeadHunterModeDamageRate(const CharacterInfo& HeadHunterModeRate)  
		{ m_HeadHunterModeDamageRate = HeadHunterModeRate; }
	void GetSuddenDeathModeDamageRate(const CharacterInfo& SuddenDeathModeRate) 
		{ m_SuddenDeathModeDamageRate = SuddenDeathModeRate; }

private:
// internal function
	// Insert a character damage information to the map
	bool Insert(UINT32 ui32CharacterIdx, CharacterInfo pInfo);
	
	// To parse specify national data
	void SetNationalColumnName(const UINT8 ui8NationalCode);
};