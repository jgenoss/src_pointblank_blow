#include "pch.h"
#include "XmlWrapper.h"
#include "CharacterTable.h"
#include "NationalCodeDef.h"


using namespace std;

CharacterTable	  CharacterTable::m_Instance;

REAL32 CharacterInfo::GetHitPositionRate(CHARA_HIT_PART eCharHitPart)
{
	switch( eCharHitPart )
	{
	// head 
	case CHARA_HIT_HEAD:				// H_Head
	case CHARA_HIT_HEAD_OLD :
	case CHARA_HIT_JAW:					// H_Head
		return m_fHead;
	// Body
	case CHARA_HIT_NECK:				// B_Neck0
	case CHARA_HIT_LCLAVICLE :
	case CHARA_HIT_RCLAVICLE :
	case CHARA_HIT_BODY1:				// B_Spine3
	case CHARA_HIT_BODY2:				// B_Spine2
	case CHARA_HIT_BODY3:				// B_Spine1
	case CHARA_HIT_ROOT	:
	case CHARA_HIT_DEATH_TRAMPLE:
	case CHARA_HIT_DEATH_HOWL:
		return m_fBody;					// human
	// Arm
	case CHARA_HIT_LUARM:				// AU_L UpperArm
	case CHARA_HIT_LDARM:				// AD_L ForeArm
	case CHARA_HIT_RUARM:				// AU_R UpperArm
	case CHARA_HIT_RDARM:				// AD_R ForeArm
		return m_fArm;
	// hand
	case CHARA_HIT_LHAND:				// HN_L Hand
	case CHARA_HIT_RHAND:				// HN_R Hand
		return m_fHand;
	// leg
	case CHARA_HIT_LULEG:				// LU_L Thigh
	case CHARA_HIT_RULEG:				// LU_R Thigh
	case CHARA_HIT_LDLEG:				// LD_L Calf
	case CHARA_HIT_RDLEG:				// LD_R Calf
		return m_fLeg;
	// Foot
	case CHARA_HIT_LFOOT:				// F_L Foot
	case CHARA_HIT_RFOOT:				// F_R Foot
		return m_fFoot;
	// Neck
	case CHARA_HIT_NECK_LOWER:
		return m_fNeck;
	// ±Þ¼Ò
	case CHARA_HIT_VITAL_EYE:				
		return 15.0f;
	// Other - other value only use dinosaurs.. 
	case CHARA_HIT_TAIL1:				// T_Tail1
	case CHARA_HIT_TAIL2:				// T_Tail2
	case CHARA_HIT_TAIL3:				// T_Tail3
	case CHARA_HIT_TAIL4:				// T_Tail4
		return m_fOther;
	}

	return 1.0;
}

CharacterTable::CharacterTable(UINT8 ui8NationalCode)
{
	m_ui8NationalCode = ui8NationalCode;

	SetNationalColumnName(ui8NationalCode);
}

CharacterTable::~CharacterTable(void)
{

}

// Find a character damage rate using key value
bool CharacterTable::Find(const UINT32 ui32CharacterIdx, CharacterInfo& Info)
{
	map<UINT32,CharacterInfo>::iterator it = m_mapCharacterTable.find(ui32CharacterIdx);

	if(it == m_mapCharacterTable.end())
	{
		return false;
	}

	Info = it->second;

	return true;
}

CharacterInfo* CharacterTable::Find(const UINT32 ui32CharacterIdx)
{
	map<UINT32,CharacterInfo>::iterator it = m_mapCharacterTable.find(ui32CharacterIdx);
	if( it == m_mapCharacterTable.end() )
		return NULL;

	return &it->second;
}

// Insert a character damage information to the map
bool CharacterTable::Insert(UINT32 ui32CharacterID, CharacterInfo stCharaInfo)
{
	// find the iterator
	map<UINT32, CharacterInfo>::iterator it;

	it = m_mapCharacterTable.begin();

	// insert a character damage information
	m_mapCharacterTable.insert( it, pair<UINT32, CharacterInfo>(ui32CharacterID, stCharaInfo) );

	return true;
}

void CharacterTable::SetNationalColumnName(UINT8 ui8NationalCode)
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

// Parse the character.xml file to load the Character table
void CharacterTable::ParseCharacterTable(IDispatch *pNode)
{
	// Xml node wrapper
	XmlNodeWrapper node(pNode);

	// static variable to use in recursive call
	static std::string strNode, strNodeAttr;
	static std::string strThisNode;
	static std::string strDepthNode;
	std::string strCurrentDepth;

	static VEC4D CamIKHeight;

	static std::string strKey1Val; 
	static std::string strKey2Val; 
	static std::string strKey3Val; 

	// All of the character damage rate
	static CharacterInfo CharaInfo;

	strNode = node.GetName();

	static bool bLoadDino = false; 
	static bool bLoadHuman = false; 
	static bool bLoadMode = false; 

	// Parse the sub node
	INT32 i32Attributes = node.NumAttributes();
	for (INT32 i = 0; i < i32Attributes ; i++)
	{ 
		std::string strName = node.GetAttribName(i);  
		std::string strVal = node.GetAttribVal(i); 

		/*I3TRACE("%d: strNode = %s, strName = %s,  strVal = %s\n", 
					i, strNode.c_str(), strName.c_str(), strVal.c_str());*/
		
		// parse character type
		if(strNode == "KEY_1")
		{
			strDepthNode = "KEY_1";
			strCurrentDepth = "KEY_1";
			strKey1Val = strVal;
		}
		
		// Parse character name
		if(strNode == "KEY_2") 
		{
			strDepthNode = "KEY_2";
			strCurrentDepth = "KEY_2";
			strKey2Val = strVal;
		}

		if(strNode == "KEY_3")
		{
			strDepthNode = "KEY_3";
			strCurrentDepth = "KEY_3";
			strKey3Val = strVal;
		}

		// To a mode damage rate parsing...
		/*if((strNode == "KEY_2") && (strVal == "HeadHunter_Killer"))
		{
			DamageRate.m_eMode = CharacterDamageRate::HeadHunter;
		}
		else if((strNode == "KEY_2") && (strVal == "SuddenDeath"))
		{
			DamageRate.m_eMode = CharacterDamageRate::SuddenDeath;
		}*/

		// Parse the specify information
		if( strDepthNode == "KEY_2" || strDepthNode == "KEY_3" )
		{
			// Parse the specified node name
			
			if( strThisNode == "CamIKHeight" && strNode == "VEC4D_2")
			{
				if( *strName.c_str() == 'X' ) CamIKHeight.vec[0] = (REAL32)atof( strVal.c_str() );
				if( *strName.c_str() == 'Y' ) CamIKHeight.vec[1] = (REAL32)atof( strVal.c_str() );
				if( *strName.c_str() == 'Z' ) CamIKHeight.vec[2] = (REAL32)atof( strVal.c_str() );
				if( *strName.c_str() == 'W' ) CamIKHeight.vec[3] = (REAL32)atof( strVal.c_str() );

				std::string strTemp = "W_";	
				std::string::size_type offset = strName.find(strTemp.c_str());
				if( offset != std::string::npos )
				{
					strName.replace(offset, strTemp.length(), "Value_");
					break;
				}
			}
			
			if		(strVal == "CamIKHeight" )			{ strThisNode = strVal; }
			else if	(strVal == "ITEMID" )				{ strThisNode = strVal; }
			else if	(strVal == "HP" )					{ strThisNode = strVal; }
			else if (strVal == "DinoWeaponID0")			{ strThisNode = strVal; }
			else if (strVal == "DinoWeaponID1")			{ strThisNode = strVal; }
			else if (strVal == "DinoWeaponID2")			{ strThisNode = strVal; }
			else if	(strVal == "_SexType")  			{ strThisNode = strVal; }
			
			if( strKey3Val == "DamageRate" )
			{
				if	(strVal == "Arm") 					{ strThisNode = strVal; }
				else if	(strVal == "Body")				{ strThisNode = strVal; }
				else if	(strVal == "Foot")				{ strThisNode = strVal; }
				else if	(strVal == "Hand")				{ strThisNode = strVal; }
				else if	(strVal == "Head")				{ strThisNode = strVal; }
				else if	(strVal == "Leg") 				{ strThisNode = strVal; }
				else if	(strVal == "Neck")				{ strThisNode = strVal; }
				else if	(strVal == "Other")				{ strThisNode = strVal; }
			}
			else if(strKey3Val == "Movement")
			{
				if (strVal == "FrontMoveSpeed")			{ strThisNode = strVal; }
			}
			else if (strKey3Val == "Movement_Space")
			{
				if (strVal == "FrontMoveSpeed") { strThisNode = strVal; }
			}

			if(strName != m_strNationalColumnName)
				continue; 
	
			if( strThisNode == "CamIKHeight" )
			{
				CharaInfo.Clear(); // if failed
				
				CharaInfo.m_CamIKHeight = CamIKHeight;

				strThisNode = ""; 
			}
			else if	(strThisNode == "ITEMID" )
			{
				CharaInfo.m_TItemID = static_cast<T_ItemID>(atoi(strVal.c_str())); strThisNode = "";
			}
			else if	(strThisNode == "HP" )
			{
				CharaInfo.m_ui16HP = static_cast<UINT16>(atoi(strVal.c_str())); strThisNode = ""; 
			}
			else if (strThisNode == "DinoWeaponID0")
			{
				CharaInfo.m_TDinoWeaponID[0] = static_cast<T_ItemID>(atoi(strVal.c_str())); strThisNode = "";
			}
			else if (strThisNode == "DinoWeaponID1")
			{
				CharaInfo.m_TDinoWeaponID[1] = static_cast<T_ItemID>(atoi(strVal.c_str())); strThisNode = "";
			}
			else if (strThisNode == "DinoWeaponID2")	
			{
				CharaInfo.m_TDinoWeaponID[2] = static_cast<T_ItemID>(atoi(strVal.c_str())); strThisNode = "";
			}
			else if(strThisNode == "_SexType")
			{
				CharaInfo.m_eGenderType = static_cast<CharacterInfo::Gender>(atoi(strVal.c_str())); strThisNode = ""; 
			}

			if( strKey3Val == "DamageRate" )
			{
				if(strThisNode == "Arm")
				{
					CharaInfo.m_fArm = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
				}
				else if(strThisNode == "Body")
				{
					CharaInfo.m_fBody = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
				}
				else if(strThisNode == "Foot")
				{
					CharaInfo.m_fFoot = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
				}
				else if(strThisNode == "Hand")
				{
					CharaInfo.m_fHand = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
				}
				else if(strThisNode == "Head")
				{
					CharaInfo.m_fHead = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
				}
				else if(strThisNode == "Leg")
				{
					CharaInfo.m_fLeg = (REAL32)atof(strVal.c_str()); strThisNode = ""; 

					if( (CharaInfo.m_eGenderType == CharacterInfo::Male) || 
						(CharaInfo.m_eGenderType == CharacterInfo::Female) ||
						(CharaInfo.m_eGenderType == CharacterInfo::Skinny_Male) )
					{
						bLoadHuman = true;
					}
				}
				else if(strThisNode == "Neck")
				{
					CharaInfo.m_fNeck = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
				}
				else if(strThisNode == "Other")
				{
					CharaInfo.m_fOther = (REAL32)atof(strVal.c_str()); strThisNode = ""; 

					//I3ASSERT( CharaInfo.m_eGenderType == CharacterInfo::Dinosaur );

					if(CharaInfo.m_eGenderType == CharacterInfo::Dinosaur)
					{
						bLoadDino = true;
					}
				}
			}
			else if(strKey3Val == "Movement")
			{
				if(strThisNode == "FrontMoveSpeed" )
				{
					CharaInfo.m_fFrontSpeed = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
				}
			}
			else if (strKey3Val == "Movement_Space")
			{
				if (strThisNode == "FrontMoveSpeed")
				{
					CharaInfo.m_fSpaceFrontSpeed = (REAL32)atof(strVal.c_str()); strThisNode = "";
				}

			}

		}
	} // end of for


	// Parse sub node using recursive call
	INT32 i32Nodes = node.NumNodes();
	for (INT32 i = 0; i < i32Nodes; i++)
	{
		ParseCharacterTable(node.GetNode(i));
	}
	
	if(strCurrentDepth == "KEY_2")
	{
		//if(bLoadHuman || bLoadDino || bLoadMode)
		if(bLoadHuman || bLoadDino)
		{
			//if(DamageRate.m_eMode == CharacterDamageRate::UndefinedMode)
			//{
			//I3ASSERT(CharaInfo.m_TItemID != 0);

			// Insert to the map table
			if( CharaInfo.m_TItemID ) 
			{
				Insert(CharaInfo.m_TItemID, CharaInfo);
			}
			//}
			//else if(DamageRate.m_eMode == CharacterDamageRate::HeadHunter)
			//{
			//	// Set damage rate on HeadHunter mode 
			//	this->m_HeadHunterModeDamageRate = DamageRate;
			//}
			//else if(DamageRate.m_eMode == CharacterDamageRate::SuddenDeath)
			//{
			//	// Set damage rate on SuddenDeath mode 
			//	this->m_SuddenDeathModeDamageRate = DamageRate;
			//}

			// completed Loading a character damage rate
			// clear static variable 
			bLoadHuman = bLoadDino = bLoadMode = false;
			CharaInfo.Clear(); 
		}
	}
}

bool CharacterTable::CheckDinoWeapon(const UINT32 ui32CharaID, const UINT32 ui32WeaponID )
{
	CharacterInfo CharaInfo;

	if( Find(ui32CharaID, CharaInfo) )
	{
		return CharaInfo.CheckDinoWeapon(ui32WeaponID);
	}

	return false;
}