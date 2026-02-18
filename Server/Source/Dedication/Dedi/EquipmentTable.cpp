#include "pch.h"
#include "XmlWrapper.h"
#include "EquipmentTable.h"
#include "NationalCodeDef.h"


using namespace std;

EquipmentTable EquipmentTable::m_Instance;

std::string s_strEquipAbilityField[ EQUIP::eABILITY_COUNT ] = {
	"C4SpeedKit",				// eABILITY_C4SPEEDKIT
	"CEBonus",					// eABILITY_CEBONUS
	"CrossHair_Min",			// eABILITY_CROSSHAIRMIN
	"Damage",					// eABILITY_DAMAGE
	"DamageRate",				// eABILITY_DAMAGERATE	
	"Defense_Arm",				// eABILITY_DEFENSE_ARM
	"Defense_Body",				// eABILITY_DEFENSE_BODY
	"Defense_Foot",				// eABILITY_DEFENSE_LEG
	"Defense_Hand",				// eABILITY_DEFENSE_LEG
	"Defense_Leg",				// eABILITY_DEFENSE_LEG
	"DetectC5",					// eABILITY_DETECTC5
	"FallDamage",				// eABILITY_FALLDAMAGE
	"HPIncrease",				// eABILITY_HPINCREASE
	"HelmetProtectionRate",		// eABILITY_PROTECTION_RATE
	"InvincibleTime",			// eABILITY_INVINCIBLE_TIME
	"MoveSpeed_Back",			// eABILITY_MOVESPEED_BACK
	"MoveSpeed_Diagonal",		// eABILITY_MOVESPEED_DIAGONAL
	"MoveSpeed_Front",			// eABILITY_MOVESPEED_FRONT
	"MoveSpeed_Side",			// eABILITY_MOVESPEED_SIDE
	"ProtectionTime",			// eABILITY_PROTECTIONTIME
	"RecoilHorzCount",			// eABILITY_RECOILHORZCOUNT
	"RespawnTime",				// eABILITY_RESPAWNTIME
	"STBonus",					// eABILITY_STBONUS
	"SoundVolume",				// eABILITY_MOVESOUND_VOL
	"WeaponAccuracy",			// eABILITY_ACCURACY
	"WeaponFireDelay",			// eABILITY_FIREDELAY
	"WeaponMaxBullet",			// eABILITY_MAXBULLET
	"WeaponPenetrate",			// eABILITY_PENETRATE
	"WeaponRange",				// eABILITY_RANGE
	"WeaponReloadTime",			// eABILITY_RELOADTIME
	"WeaponSwapTime",			// eABILITY_SWAPTIME
	"WeaponThrowSpeed",			// eABILITY_THROWSPEED
	"WeaponThrowCount",			// eABILITY_THROWCOUNT	
	
};

EquipmentTable::EquipmentTable(UINT8 ui8NationalCode)
{
	m_ui8NationalCode = ui8NationalCode;

	SetNationalColumnName(ui8NationalCode);
}

EquipmentTable::~EquipmentTable(void)
{
}

// Find a equipment info using key value
bool EquipmentTable::Find(const UINT32 ui32ItemID, EquipmentInfo& Info, PB_VER ePBVer )
{
	//기본 파츠인경우 검사하지 않습니다.
	if( DEFAULT_PARTS_ITEM_ID_MIN < ui32ItemID )
	{
		Info.ClearAll();
		return true;
	}

	map<UINT32, EquipmentInfo>::iterator it = m_mapEquipmentTable[ePBVer].find(ui32ItemID);

	if(it == m_mapEquipmentTable[ePBVer].end())
	{
		return false;
	}

	Info = it->second;

	return true;
}

EquipmentInfo* EquipmentTable::Find(const UINT32 ui32ItemID, PB_VER ePBVer )
{
	map<UINT32, EquipmentInfo>::iterator it = m_mapEquipmentTable[ePBVer].find(ui32ItemID);
	if( it == m_mapEquipmentTable[ePBVer].end() )
		return NULL;

	return &it->second;
}

// Insert a character damage information to the map
bool EquipmentTable::Insert(UINT32 ui32ItemID, EquipmentInfo stItem, PB_VER ePBVer )
{
	// find the iterator
	map<UINT32, EquipmentInfo>::iterator it;

	it = m_mapEquipmentTable[ePBVer].begin();

	// insert a equipment item information
	m_mapEquipmentTable[ePBVer].insert( it, pair<UINT32, EquipmentInfo>( ui32ItemID, stItem) );

	return true;
}


void EquipmentTable::SetNationalColumnName(UINT8 ui8NationalCode)
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
		m_strNationalColumnName = "";
	}
}


// Parse the Equipment.xml file to load the Character table
void EquipmentTable::ParseEquipmentTable(IDispatch *pNode)
{
	// Xml node wrapper
	XmlNodeWrapper node(pNode);

	// static variable to use in recursive call
	static std::string strNode, strNodeAttr;
	static std::string strDepthNode;
	std::string strThisNode;

	static std::string strKey1Val; // equipment
	static std::string strKey2Val; // class
	static std::string strKey3Val; // name
	static std::string strKey4Val; // name
	static std::string strKey5Val; // ability


	// All of the character damage rate
	static EquipmentInfo item;

	strNode = node.GetName();

	// Parse the sub node
	INT32 i32Attributes = node.NumAttributes();
	for (INT32 i = 0; i < i32Attributes ; i++)
	{ 
		std::string strName = node.GetAttribName(i);  
		std::string strVal = node.GetAttribVal(i);    

		/*I3TRACE("%d: strNode = %s, strName = %s,  strVal = %s\n", 
					i, strNode.c_str(), strName.c_str(), strVal.c_str());*/
		
		// Parse Equipment name
		if(strNode == "KEY_1") 
		{
			strDepthNode = "KEY_1";
			strKey1Val   = strVal;
		}

		// get equipment class...
		if ( strNode == "KEY_2" )
		{
			strDepthNode = "KEY_2";
			strKey2Val = strVal;
		}

		if (strNode == "KEY_3" )
		{
			strDepthNode = "KEY_3";
			strKey3Val = strVal;
		}

		if (strNode == "KEY_4" )
		{
			strDepthNode = "KEY_4";
			strKey4Val = strVal;
		}

		if (strNode == "KEY_5" )
		{
			strDepthNode = "KEY_5";
			strKey5Val = strVal;
		}

		// Parse the specify information
		if( strDepthNode == "KEY_3" || strDepthNode == "KEY_5")
		{
			// Parse the specified node name

			if( strVal == "ITEMID" || strVal == "_Ability" )
			{
				strThisNode = strVal;
			}
			else if (strKey5Val == "_Ability")
			{
				// find value strings
				for( INT32 i32PartIdx = 0; i32PartIdx < EQUIP::eABILITY_COUNT; i32PartIdx++ )
				{
					if ( strVal == s_strEquipAbilityField[i32PartIdx] ) 
					{
						strThisNode = strVal; 
						continue;
					}
				}
			}

			if(strName != m_strNationalColumnName || strThisNode == "" )
				continue; 

			if( strThisNode == "ITEMID" ) //first property
			{
				item.ClearAll();
				item.m_TItemID = static_cast<UINT32>(atoi(strVal.c_str())); //strThisNode = ""; 
			}
			else // find valid ability 
			{
				for( INT32 i32PartIdx = 0; i32PartIdx < EQUIP::eABILITY_COUNT; i32PartIdx++ )
				{
					if ( strThisNode == s_strEquipAbilityField[i32PartIdx] )
					{
						I3ASSERT( item.m_TItemID != 0 );
						item.m_ar32Ability[i32PartIdx] = (REAL32)atof(strVal.c_str()); //strThisNode = "";
						return;
					} // found
				} // for
			}
		}
	} // end of for


	// Parse sub node using recursive call
	INT32 i32Nodes = node.NumNodes();
	for (INT32 i = 0; i < i32Nodes; i++)
	{
		ParseEquipmentTable(node.GetNode(i));
	}
	
	// Insert a Weapon information to the Weapon table
	BOOL bItemParsed = (BOOL)( strThisNode == "_Ability" && item.m_TItemID != 0 && i32Nodes > 0 );

	if( bItemParsed )
	{
		EquipmentInfo tmp;

		PB_VER ePBVer = PB_VER_V15;

		if( strKey4Val == "V1_Info" ) ePBVer = PB_VER_V10;
	
		if( Find( item.m_TItemID, tmp, ePBVer ) == FALSE )
		{
			//I3TRACE("========= ItemID = %d =========\n", item.m_TItemID);
			//for(INT32 i = 0 ; i < EQUIP::eABILITY_COUNT ; ++i)
			//{
			//	I3TRACE("m_rAbility[%d] = %f\n", i, item.m_ar32Ability[i]);
			//}
			//I3TRACE("=======================================\n");
						
			Insert( item.m_TItemID, item, ePBVer );
			item.ClearAbility();			
			// clear static variables
			
		}
		else
		{
			I3TRACE("Equipment.i3RegXML _Ability Load Error / ItemID : %d\n", item.m_TItemID); 
			//I3ASSERT( 0 && "Equipment.i3RegXML _Ability Load Error");
		}
	}
}
