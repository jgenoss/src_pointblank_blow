/* 
	SkillMgr.h

	2013-3-08 : 1. mode_skill: domination, g2의 mode skill은 구현되지 않은 상태(클라,서버).
				2. 방어 skill: 미구현. NEW_SKILL_SYSTEM에 구현되어 있으나 홀드상태라서 적용안함.
					: pef의 다른 트리 참조.
				3. damage skill 만 사용하고 있음. 
				4. bomb_range, fire_delay, swap_speed 인자를 이용하여 기존 핵탐지 기능 수정 필요.
				5. 테스트: SERVER_DAMAGE define 적용, HMSParser::_ParseCharaWeaponInfo() 패킷 검사하도록 수정.
*/

#include "pch.h"
#include "XmlWrapper.h"
#include "SkillTable.h"
#include "NationalCodeDef.h"

using namespace std;

#define GETBIT64(x, y)	(\
	(\
		((x) >> (y)) & (0x1i64) \
	)\
)

static char* s_strSkillValues[CSkillInfo::MAX_SKILL_LEVEL] = {
	"ValueLv_0",
	"ValueLv_1",
	"ValueLv_2",
	"ValueLv_3",
	"ValueLv_4",
	"ValueLv_5",
	"ValueLv_6",
	"ValueLv_7",
	"ValueLv_8",
	"ValueLv_9",
};

static char* s_strSkillRequisite[CSkillInfo::MAX_SKILL_LEVEL] = {
	"RequisiteLv_0",
	"RequisiteLv_1",
	"RequisiteLv_2",
	"RequisiteLv_3",
	"RequisiteLv_4",
	"RequisiteLv_5",
	"RequisiteLv_6",
	"RequisiteLv_7",
	"RequisiteLv_8",
	"RequisiteLv_9",
};

SkillTable SkillTable::m_Instance;

SkillTable::SkillTable(UINT8 ui8NationalCode)
{
	m_ui8NationalCode = ui8NationalCode;

	SetNationalColumnName(ui8NationalCode);

	for(INT32 idx = 0 ; idx < SKILL_CARTEGORY_MAX ; ++idx)
		m_mapSkillTable[idx].clear();
}

SkillTable::~SkillTable(void)
{
}

void SkillTable::SetNationalColumnName(UINT8 ui8NationalCode)
{
	switch(ui8NationalCode)
	{
	case Dev: // 0
		m_strNationalColumnName = "Value_Dev"; break;
	case Indonesia : //1
		m_strNationalColumnName = "Value_Indonesia"; break;
	case Russia: //2
		m_strNationalColumnName = "Value_Russia"; break;
	case Thai: //3
		m_strNationalColumnName = "Value_Thai"; break;
	case Korea: //4
		m_strNationalColumnName = "Value_Korea"; break;
	case Turkey: //5
		m_strNationalColumnName = "Value_Turkey"; break;
	case Brazil: //6
		m_strNationalColumnName = "Value_Brazil"; break;
	case LatinAmerica: //7
		m_strNationalColumnName = "Value_Latin_America"; break;
	case NorthAmerica: //8
		m_strNationalColumnName = "Value_NorthAmerica"; break;
	case China: //9
		m_strNationalColumnName = "Value_China"; break;
	case Italy: //10
		m_strNationalColumnName = "Value_Italy"; break;
	case Philippines: //11
		m_strNationalColumnName = "Value_Philippines"; break;
	case MiddleEast: // 12
		m_strNationalColumnName = "Value_MiddleEast"; break;
	case Taiwan: //13
		m_strNationalColumnName = "Value_Taiwan"; break;
	case Singmal: //14
		m_strNationalColumnName = "Value_Singmal"; break;
	case Vietnam: //15
		m_strNationalColumnName = "Value_Vietnam"; break;
	default:
		{
			I3FATAL("National Code is NULL\n");
			m_strNationalColumnName = "";
		}
	}
}

void SkillTable::ParseSkillInfo(IDispatch *pNode)
{
	// Xml node wrapper
	XmlNodeWrapper node(pNode);

	// static variable to use in recursive call
	static std::string strNode, strNodeAttr;
	static std::string strThisNode;
	static std::string strDepthNode;

	static std::string strKey1Val; // SkillList
	static std::string strKey2Val; // Skill Nmae (ex:Skill_BombRange
	static std::string strKey3Val; // Level (ex: LV1 )

	static CSkillInfo SkillInfo;

	strNode = node.GetName();

	static bool bLastAttr = false; // last member

	static SKILL_CARTEGORY eCategory = SKILL_NONE;

	// Parse the sub node
	INT32 i32Attributes = node.NumAttributes();
	for (INT32 i = 0; i < i32Attributes ; i++)
	{ 
		std::string strName = node.GetAttribName(i);  
		std::string strVal = node.GetAttribVal(i);    

		// Parse SkillData
		if(strNode == "KEY_1") 
		{
			strDepthNode = "KEY_1";
			strKey1Val   = strVal;
		}

		// get Skill Type List...
		if ( strNode == "KEY_2" )
		{
			strDepthNode = "KEY_2";
			strKey2Val = strVal;

			if( strKey2Val == "0_CommonSkillList" )
				eCategory = SKILL_TYPE_COMMON;
			else if( strKey2Val == "1_MainSkillList" )
				eCategory = SKILL_TYPE_MAIN;
			else if( strKey2Val == "2_AssistSkillList" )
				eCategory = SKILL_TYPE_ASSIST;
			
		}

		if (strNode == "KEY_3" )
		{
			strDepthNode = "KEY_3";
			strKey3Val = strVal;
		}


		// Parse the specify information
		if( strKey1Val == "SkillData" && strDepthNode == "KEY_3" )
		{
			if		(strVal == "SkillType" )		{ strThisNode = strVal; }
			else if (strVal == "RequisiteType" )	{ strThisNode = strVal; }
			else if (strVal == "_Index")			{ strThisNode = strVal; }
			else if (strVal == "__WeaponClass" )	{ strThisNode = strVal; }
			else if (strVal == "__WeaponSlot" )		{ strThisNode = strVal; }
			else
			{
				for( INT32 j = 0; j < CSkillInfo::MAX_SKILL_LEVEL; j++ )
				{
					if( strVal == s_strSkillValues[j] )
					{
						strThisNode = strVal;
						break;
					}
					else if(strVal == s_strSkillRequisite[j])
					{
						strThisNode = strVal;
						break;
					}
				}
			}

			if( strThisNode == "" || strName != m_strNationalColumnName ) continue;

			if( strThisNode == "SkillType")
			{
				SkillInfo.m_eSkillType = (SKILL_TYPE)atoi(strVal.c_str());
				strThisNode = "";
			}
			else if(strThisNode == "RequisiteType" )
			{
				SkillInfo.m_eRequisteType = (REQUISTE)atoi(strVal.c_str());
				strThisNode = "";
			}
			else if (strThisNode == "_Index")
			{ 
				SkillInfo.m_ai32SkillIdx = (INT32)atoi(strVal.c_str());
				strThisNode = "";
			}
			else if(strThisNode == "__WeaponClass") 
			{
				ExportStringToFlag(strVal, SkillInfo.m_ui32WeaponClass);
				strThisNode = "";
			}
			else if(strThisNode == "__WeaponSlot") 
			{
				ExportStringToFlag(strVal, SkillInfo.m_ui32WeaponSlot);
				strThisNode = "";
				bLastAttr = true;
			}
			else
			{
				for( INT32 idx = 0; idx < CSkillInfo::MAX_SKILL_LEVEL; ++idx )
				{
					if( strThisNode == s_strSkillRequisite[idx] )
					{
						SkillInfo.m_ai32RequisiteLV[idx] = (INT32)atof(strVal.c_str()); 
						strThisNode = "";
						break;
					}
					else if(strThisNode == s_strSkillValues[idx])
					{
						SkillInfo.m_ar32Value[idx] = (REAL32)atof(strVal.c_str()); 
						strThisNode = "";
						break;
					}
				}
			}
		}
	} // end of for

	// Parse sub node using recursive call
	INT32 i32Nodes = node.NumNodes();
	for (INT32 i = 0; i < i32Nodes; i++)
	{
		ParseSkillInfo(node.GetNode(i));
	}

	if( SkillInfo.isValid() && bLastAttr == true)
	{
		m_mapSkillTable[eCategory].insert( pair<INT32, CSkillInfo>(SkillInfo.m_ai32SkillIdx, SkillInfo) );
		SkillInfo.Clear();
		bLastAttr = false;
	}
}

void SkillTable::ExportStringToFlag(std::string strVal, UINT32 & flag)
{
	if(strVal.length() == 0)
		return;

	std::string::size_type pos = std::string::npos;
	std::string::size_type idx = std::string::npos;

	do
	{
		INT32 key = -1;

		pos = strVal.find_first_of(",");
		idx = strVal.find_first_not_of(",");

		if( pos == std::string::npos )
		{
			key = (INT32)atoi(strVal.c_str());
		}
		else
		{
			std::string strData = strVal.substr(idx, pos);
			key = (INT32)atoi(strData.c_str());
			strVal = strVal.substr(pos + 1);
		}

		if( key >= 0)
			flag |= ((INT32)0x01) << key;
	}
	while(pos != std::string::npos);
}

CSkillInfo* SkillTable::FindSkillInfo(SKILL_CARTEGORY eCategory, const INT32 i32SkillType)
{
	map<INT32, CSkillInfo>::iterator it = m_mapSkillTable[eCategory].find(i32SkillType);

	if( it == m_mapSkillTable[eCategory].end() )
		return NULL;

	return &it->second;
}