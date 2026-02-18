#pragma once

#include <baseTsd.h>
#include <string>
#include <map>

// 스킬 획득 조건
enum REQUISTE
{
	REQUISTE_UNKNOWN	= -1,
	REQUISTE_CLASS		= 0,			// 랭크
	REQUISTE_TIME						// 플레이 타임
};

class CSkillInfo
{
public:
	enum
	{
		MAX_SKILL_LEVEL = 10,
	};

	CSkillInfo() { Clear(); }

	void Clear(void)
	{
		m_eSkillType		= SKILLLIST_UNKNOWN;
		m_eRequisteType		= REQUISTE_UNKNOWN;
		m_ui32WeaponSlot	= 0;
		m_ui32WeaponClass	= 0;
		m_ai32SkillIdx		= -1;

		for(INT32 i = 0 ; i < MAX_SKILL_LEVEL; ++i)
		{
			m_ai32RequisiteLV[i] = -1;
			m_ar32Value[i] = -1.0f;
		}
	}

	bool isValid(void)
	{
		return (m_ai32SkillIdx > -1 && m_eSkillType >SKILLLIST_UNKNOWN && m_eRequisteType > REQUISTE_UNKNOWN);
	}

	bool isValidSlot(UINT32 ui32flag)
	{
		return ( (m_ui32WeaponSlot & ui32flag) != 0 );
	}

	bool isValidWeapon(UINT32 ui32flag)
	{
		return ( (m_ui32WeaponClass & ui32flag ) != 0 );
	}

public:
	SKILL_TYPE	m_eSkillType;						//
	REQUISTE	m_eRequisteType;					// 획득 방식 (랭크 / 플레이타임)

	INT32		m_ai32SkillIdx;						// 스킬 인덱스
	UINT32		m_ui32WeaponSlot;					// 적용되는 무기 슬롯 (WEAPON_SLOT_TYPE Bit Flag)
	UINT32		m_ui32WeaponClass;					// 적용되는 무기 계열 (WEAPON_CLASS_TYPE Bit Flag)

	INT32		m_ai32RequisiteLV[MAX_SKILL_LEVEL];
	REAL32		m_ar32Value[MAX_SKILL_LEVEL];
};

class SkillTable
{
public:
	SkillTable(UINT8 iNationalCode = 1);
	~SkillTable(void);

public:
	static SkillTable m_Instance;

	// National Code String name to parse xml column 
	std::string m_strNationalColumnName; 
	UINT8 m_ui8NationalCode;

	std::map<INT32, CSkillInfo>	m_mapSkillTable[SKILL_CARTEGORY_MAX];

	// Get singleton Instance
	static SkillTable&		GetInstance() { return m_Instance; }

	//void	Insert( SKILL_INFO stSkillInfo );
	void	ParseSkillInfo(IDispatch *pNode);
	void	ExportStringToFlag(std::string strVal, UINT32 & flag);

	CSkillInfo*	FindSkillInfo(SKILL_CARTEGORY eCategory, const INT32 i32SkillType);


	void	SetNationalColumnName(UINT8 ui8NationalCode);
	// Set national Code
	void	SetNationalCode(UINT8 ui8NationalCode) { m_ui8NationalCode = ui8NationalCode; 
										SetNationalColumnName(ui8NationalCode); }
};
