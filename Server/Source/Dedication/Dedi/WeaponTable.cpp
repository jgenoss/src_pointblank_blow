#include "pch.h"
#include "WeaponTable.h"
#include "XmlWrapper.h"
#include "NationalCodeDef.h"

using namespace std;

WeaponTable	WeaponTable::m_Instance;

// Type-String mapping table for Extensions
struct tagExtType {
	WEAPON::EXT_TYPE		_type;
	const char *		_szExtensionTypeName;
};

static tagExtType sExtensions[ WEAPON::EXT_MAX] = {
	WEAPON::EXT_NONE,				"",						// EXT_NONE
	WEAPON::EXT_SCOPE,				"Scope",				// EXT_SCOPE
	WEAPON::EXT_DUAL_MAG,			"DualMag",				// EXT_DUAL_MAG
	WEAPON::EXT_SILENCER,			"Silencer",				// EXT_SILENCER
	WEAPON::EXT_ZOOM,				"Zoom",					// EXT_ZOOM
	WEAPON::EXT_DOT_SIGHT,			"DotSight",				// EXT_DOT_SIGHT
	WEAPON::EXT_STEADY_AIM,			"SteadyAim",			// EXT_STEADY_AIM
	WEAPON::EXT_THROW_SHELL,		"ThrowShell",			// EXT_RLAUNCHER_SHELL
	WEAPON::EXT_GLAUNCHER,			"GrenadeLauncher",		// EXT_GLAUNCHER
	WEAPON::EXT_SHOTGUN_LAUNCHER,	"ShotGunLauncher",		// EXT_SHOTGUN_LAUNCHER
	WEAPON::EXT_BAYONET,			"Bayonet",				// EXT_BAYONET
	WEAPON::EXT_CROSSFIRE,			"CrossFire",			// EXT_CROSSFIRE
	WEAPON::EXT_DUAL,				"Dual",					// EXT_DUAL				
	WEAPON::EXT_DUAL_LEFT,			"Dual_Left",			// EXT_DUAL_LEFT
	WEAPON::EXT_DIRECT_MELEE,		"DirectMelee",			// EXT_DIRECT_MELEE
	WEAPON::EXT_THROW,				"Throw",				// EXT_THROW
	WEAPON::EXT_DINO,				"Dino",					// EXT_DINO
	WEAPON::EXT_DUAL_LEFT_DIF,		"Dual_Left_Dif",		// EXT_DUAL_LEFT_DIF
};

struct tagDamageType {
	WEAPON::TARGET_TYPE	_target;
	WEAPON::ATTACK_TYPE	_attak;
	const char *		_szDamageTypeName;
};

static tagDamageType sDamageType[] = {
	{ WEAPON::TARGET_HUMAN,	WEAPON::ATTACK_TYPE_L1,			"Damage"},
	{ WEAPON::TARGET_HUMAN,	WEAPON::ATTACK_TYPE_L1, 		"AttackDamageA1"},
	{ WEAPON::TARGET_HUMAN,	WEAPON::ATTACK_TYPE_L2, 		"AttackDamageA2"},
	{ WEAPON::TARGET_HUMAN,	WEAPON::ATTACK_TYPE_L3, 		"AttackDamageA3"},
	{ WEAPON::TARGET_HUMAN,	WEAPON::ATTACK_TYPE_R1, 		"AttackDamageB1"},
	{ WEAPON::TARGET_HUMAN,	WEAPON::ATTACK_TYPE_R2, 		"AttackDamageB2"},
	{ WEAPON::TARGET_HUMAN,	WEAPON::ATTACK_TYPE_R3, 		"AttackDamageB3"},
	{ WEAPON::TARGET_HUMAN,	WEAPON::ATTACK_TYPE_R4, 		"AttackDamageB4" },
	{ WEAPON::TARGET_HUMAN, WEAPON::ATTACK_TYPE_R1,			"ExtensionDamage"},
	{ WEAPON::TARGET_DINO, 	WEAPON::ATTACK_TYPE_L1, 		"AttackDamageA1ForDino"},
	{ WEAPON::TARGET_DINO, 	WEAPON::ATTACK_TYPE_R1, 		"AttackDamageB1ForDino"},
	{ WEAPON::TARGET_NONE,	WEAPON::ATTACK_TYPE_NONE,		""	}
};

WEAPON::EXT_TYPE GetExtensionType( const char * strVal )
{
	for( int i = 0; i < sizeof(sExtensions)/sizeof(tagExtType); i++ )
	{
		if( strcmp(strVal ,sExtensions[i]._szExtensionTypeName) == 0 )
			return sExtensions[i]._type;
	}

	return WEAPON::EXT_NONE;
}

WeaponTable::WeaponTable(UINT8 ui8NationalCode)
{
	m_ui8NationalCode = ui8NationalCode;

	SetNationalColumnName(ui8NationalCode);
	i3mem::FillZero( m_ui32WeaponTableCount, sizeof( m_ui32WeaponTableCount ) );
	i3mem::FillZero( m_pWeaponTable, sizeof( m_pWeaponTable ) );
}

WeaponTable::~WeaponTable(void)
{
	for( UINT32 i = 0; i < WEAPON_CLASS_COUNT; ++i )
	{
		I3_SAFE_DELETE_ARRAY( m_pWeaponTable[i] );
	}
}

bool WeaponTable::Find(const UINT32 ui32Key, WeaponInfo& Info)
{
	map<UINT32,WeaponInfo>::iterator it =m_mapWeaponTable.find(ui32Key);

	if(it == m_mapWeaponTable.end())
	{
		return false;
	}

	Info = it->second;

	return true;
}

bool WeaponTable::Insert(UINT32 ui32Key, WeaponInfo Info)
{
	// find the iterator
	map<UINT32, WeaponInfo>::iterator it;

	it = m_mapWeaponTable.begin();

	// insert a weapon information
	m_mapWeaponTable.insert( it, pair<UINT32, WeaponInfo>(ui32Key, Info) );

	return true;
}

// Parse the weapon xml file to load the Weapon table
void WeaponTable::ParseWeaponTable(IDispatch *pNode)
{
	// Xml node wrapper
	XmlNodeWrapper node(pNode);

	static std::string strKey2Val;
	static std::string strKey3Val;
	static std::string strKey4Val;
	static std::string strKey5Val;
	
	static std::string strNode, strNodeAttr;
	static std::string strThisNode;
	static std::string strDepthNode;

	strNode = node.GetName();

	INT32 i32Attributes = node.NumAttributes();

	// Set a Weapon Information
	static WeaponInfo DamageInfo;

	static bool bLastNode = false;

	for (INT32 i = 0; i < i32Attributes ; i++)
	{ 
		std::string strName = node.GetAttribName(i);  
		std::string strVal = node.GetAttribVal(i);    

		if( (strVal == "Weapon") ) 
		{
			strKey2Val = strVal; 
			strDepthNode = "KEY_2";
		}

		if( (strKey2Val == "Weapon") && (strNode == "KEY_3") ) 
		{
			if( (strDepthNode == "KEY_5") ) bLastNode = true;

			strKey3Val = strVal;
			strDepthNode = "KEY_3";
			continue;
		}

		if((strKey2Val == "Weapon") && (strNode == "KEY_4")) 
		{
			if( strDepthNode == "KEY_5" ) bLastNode = true;

			strKey4Val = strVal;
			strDepthNode = "KEY_4"; // name, basic info
			continue;
		}

		if((strKey2Val == "Weapon") && (strNode == "KEY_5")) 
		{
			strKey5Val = strVal;
			strDepthNode = "KEY_5"; // sub key ( damage, extension... )
			continue;
		}

		if( (strKey2Val == "Weapon") && ((strDepthNode == "KEY_4") || (strDepthNode == "KEY_5") ) )
		{

			// ------------------------------------
			// Basic Info
			// ------------------------------------
			if(  strDepthNode == "KEY_4" )
			{
				// Parse the specified node name
				if	   (strVal == "__Make Time" )				{ strThisNode = strVal; }
				else if(strVal == "ITEMID")						{ strThisNode = strVal; }
				else if(strVal == "_UsageType")					{ strThisNode = strVal; }
				else if(strVal == "Explosion_EffectiveSlope")	{ strThisNode = strVal; }
				else if(strVal == "Explosion_Range")			{ strThisNode = strVal; }
				else if(strVal == "FireBulletCount")			{ strThisNode = strVal;	}
				else if(strVal == "FireDecBulletCount")			{ strThisNode = strVal; }
				else if(strVal == "FireDelay")					{ strThisNode = strVal;	}
				else if(strVal == "AttackDamageA1") 			{ strThisNode = strVal; }
				else if(strVal == "AttackDamageA2") 			{ strThisNode = strVal; }
				else if(strVal == "AttackDamageA3") 			{ strThisNode = strVal; }
				else if(strVal == "AttackDamageB1") 			{ strThisNode = strVal; }
				else if(strVal == "AttackDamageB2") 			{ strThisNode = strVal;	}
				else if(strVal == "AttackDamageB3") 			{ strThisNode = strVal; }
				else if(strVal == "AttackDamageB4")				{ strThisNode = strVal; }

				else if(strVal == "AttackTimeA1") 				{ strThisNode = strVal; }
				else if(strVal == "AttackTimeA2") 				{ strThisNode = strVal; }
				else if(strVal == "AttackTimeA3") 				{ strThisNode = strVal; }
				else if(strVal == "AttackTimeB1") 				{ strThisNode = strVal; }
				else if(strVal == "AttackTimeB2") 				{ strThisNode = strVal; }
				else if(strVal == "AttackTimeB3") 				{ strThisNode = strVal; }
				else if(strVal == "AttackTimeB4")				{ strThisNode = strVal; }
				else if(strVal == "AttackDamageA1ForDino") 		{ strThisNode = strVal; }
				else if(strVal == "AttackDamageB1ForDino") 		{ strThisNode = strVal; }
				// add : because of lemitless bullets hack
				else if(strVal == "LoadBullet")					{ strThisNode = strVal; }
				else if(strVal == "MaxBullet")					{ strThisNode = strVal; }	
				else if(strVal == "IncreaseCount")				{ strThisNode = strVal; }
				else if(strVal == "Range")						{ strThisNode = strVal; }
				// recoil
				else if(strVal == "RecoilHorzAngle")			{ strThisNode = strVal;	}
				else if(strVal == "RecoilHorzMax")				{ strThisNode = strVal;	}
				else if(strVal == "RecoilVertAngle")			{ strThisNode = strVal;	}
				else if(strVal == "RecoilVertMax")				{ strThisNode = strVal;	}
				else if(strVal == "Deviation")					{ strThisNode = strVal;	}
				// ThrowWeapon
				else if(strVal == "IsThrowWeapon")				{ strThisNode = strVal;	}
				else if(strVal == "IndicatorType" )				{ strThisNode = strVal; }
			}

			if(  strDepthNode == "KEY_5" )
			{
				if( strKey5Val == "Damage" )
				{
					if		(strVal == "DPD_Rate" )				{ strThisNode = strVal;	}
					else if	(strVal == "Damage"   )				{ strThisNode = strVal;	}
					else if (strVal == "DamageMax")				{ strThisNode = strVal;	}
					else if (strVal == "CriticalDamageRate")	{ strThisNode = strVal;	}
					else if (strVal == "CriticalDamageChance")	{ strThisNode = strVal;	}
				}
				if( strKey5Val == "Extension1") // RMB
				{
					if		(strVal == "ITEMID")				{ strThisNode = "ExtItemID1";	}
					else if	(strVal == "Type"	)				{ strThisNode = "ExtType1";		}
					else if (strVal == "ExtensionDamage")		{ strThisNode = strVal;			}
				}
				if( strKey5Val == "Extension2" ) // 'B'-Key
				{
					if		(strVal == "ITEMID")				{ strThisNode = "ExtItemID2";	}
					else if	(strVal == "Type"	)		  		{ strThisNode = "ExtType2";		}
					else if (strVal == "ExtensionDamage")		{ strThisNode = strVal;			}

				}
				if( strKey5Val == "Extension3" ) // reserved
				{
					if		(strVal == "ITEMID")				{ strThisNode = "ExtItemID3";	}
					else if	(strVal == "Type"	)		  		{ strThisNode = "ExtType3";		}
					else if (strVal == "ExtensionDamage")		{ strThisNode = strVal;			}

				}
				if( strKey5Val == "Extension4" ) // reserved
				{
					if		(strVal == "ITEMID")				{ strThisNode = "ExtItemID4";	}
					else if	(strVal == "Type"	)		  		{ strThisNode = "ExtType4";		}
					else if (strVal == "ExtensionDamage")		{ strThisNode = strVal;			}

				}
				
				// Speed
				if( strKey5Val == "Movement" )
				{	
					if	 (strVal == "MoveNormal_Fire")		{ strThisNode = strVal;			}
					else if(strVal == "MoveNormal_Scope")	{ strThisNode = strVal;			}
					else if(strVal == "MoveNormal_Sit")		{ strThisNode = strVal;			}
					else if(strVal == "MoveNormal_Stand")	{ strThisNode = strVal;			}
					else if(strVal == "MoveNormal_Zoom")	{ strThisNode = strVal;			}
				}
			}

			if(strName != m_strNationalColumnName)
			{
				continue; 
			}

			// Parse the specified value to create composite key (weapon unique key)
			if( strThisNode == "" )
			{
				//DamageInfo.Clear();
			}
			else if( strDepthNode != "KEY_5" && strThisNode == "__Make Time" )
			{
				//DamageInfo.Clear();
			}
			else if(strThisNode == "ITEMID") 
			{
				DamageInfo.m_TItemID = (T_ItemID)atoi(strVal.c_str()); strThisNode = ""; 
			}
			else if(strThisNode == "_UsageType") 
			{
				DamageInfo.m_ui8Usage = (UINT8)atoi(strVal.c_str()); strThisNode = ""; 	
			}
			else if(strThisNode == "Explosion_EffectiveSlope")
			{
				DamageInfo.m_r32EffectiveSlope = (REAL32)atof(strVal.c_str()); strThisNode = "";
			}
			else if(strThisNode == "Explosion_Range")
			{
				DamageInfo.m_r32ExplosionRange = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
			}
			else if(strThisNode == "FireBulletCount")
			{
				DamageInfo.m_ui8FireBulletCount = (UINT8)atoi(strVal.c_str()); strThisNode = ""; 
			}
			else if(strThisNode == "FireDecBulletcount")
			{
				DamageInfo.m_ui8FireDecBulletCount = (UINT8)atoi(strVal.c_str()); strThisNode = "";
			}
			else if(strThisNode == "FireDelay")
			{
				DamageInfo.m_r32FireDelay = (REAL32)atof(strVal.c_str()); strThisNode = "";
			}
			else if(strThisNode == "AttackTimeA1")
			{
				DamageInfo.m_r32AttackDelay[ WEAPON::ATTACK_TYPE_L1 ] = (REAL32)atof(strVal.c_str()); strThisNode = "";
			}
			else if(strThisNode == "AttackTimeA2")
			{
				DamageInfo.m_r32AttackDelay[ WEAPON::ATTACK_TYPE_L2 ] = (REAL32)atof(strVal.c_str()); strThisNode = "";
			}
			else if(strThisNode == "AttackTimeA3")
			{
				DamageInfo.m_r32AttackDelay[ WEAPON::ATTACK_TYPE_L3 ] = (REAL32)atof(strVal.c_str()); strThisNode = "";
			}
			else if(strThisNode == "AttackTimeB1")
			{
				DamageInfo.m_r32AttackDelay[ WEAPON::ATTACK_TYPE_R1 ] = (REAL32)atof(strVal.c_str()); strThisNode = "";
			}
			else if(strThisNode == "AttackTimeB2")
			{
				DamageInfo.m_r32AttackDelay[ WEAPON::ATTACK_TYPE_R2 ] = (REAL32)atof(strVal.c_str()); strThisNode = "";
			}
			else if(strThisNode == "AttackTimeB3")
			{
				DamageInfo.m_r32AttackDelay[ WEAPON::ATTACK_TYPE_R3 ] = (REAL32)atof(strVal.c_str()); strThisNode = "";
			}
			else if (strThisNode == "AttackTimeB4")
			{
				DamageInfo.m_r32AttackDelay[WEAPON::ATTACK_TYPE_R4] = (REAL32)atof(strVal.c_str()); strThisNode = "";
			}
			
			else if(strThisNode == "LoadBullet")
			{
				DamageInfo.m_ui16LoadBullet = (UINT16)atoi(strVal.c_str()); strThisNode = ""; 
			}
			else if(strThisNode == "MaxBullet")
			{
				DamageInfo.m_ui16MaxBullet = (UINT16)atoi(strVal.c_str()); strThisNode = ""; 
			}
			else if (strThisNode == "IncreaseCount")
			{
				DamageInfo.m_ui8IncreaseCount = (UINT8)atoi(strVal.c_str()); strThisNode = "";
			}
			else if(strThisNode == "Range")
			{
				DamageInfo.m_r32Range = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
			}
			// recoil
			else if(strThisNode == "RecoilHorzAngle")
			{
				DamageInfo.m_r32RecoilHorzAngle = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
			}
			else if(strThisNode == "RecoilHorzMax")
			{
				DamageInfo.m_r32RecoilHorzMax = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
			}
			else if(strThisNode == "RecoilVertAngle")
			{
				DamageInfo.m_r32RecoilVertAngle = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
			}
			else if(strThisNode == "RecoilVertMax")
			{
				DamageInfo.m_r32RecoilVertMax = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
			}
			else if(strThisNode == "Deviation")
			{
				DamageInfo.m_r32Deviation = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
			}
			else if(strThisNode == "CriticalDamageRate")
			{
				DamageInfo.m_r32CriticalDamageRate = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
			}
			else if(strThisNode == "CriticalDamageChance")
			{
				DamageInfo.m_ui8CriticalChanceRange = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
			}			
			else if(strThisNode == "DPD_Rate")
			{
				DamageInfo.m_r32DPDRate = (REAL32)atof(strVal.c_str()); strThisNode = ""; 
			}
			else if(strThisNode == "ExtItemID1")
			{
				DamageInfo.m_ExtInfo[0].m_TItemID = (T_ItemID)atoi(strVal.c_str());	strThisNode = "";
			}
			else if(strThisNode == "ExtType1")
			{
				DamageInfo.m_ExtInfo[0].m_aExtType = GetExtensionType( strVal.c_str() );	strThisNode = "";
			}
			else if(strThisNode == "ExtItemID2")
			{
				DamageInfo.m_ExtInfo[1].m_TItemID	= (T_ItemID)atoi(strVal.c_str());	strThisNode = "";
			}
			else if(strThisNode == "ExtType2")
			{
				DamageInfo.m_ExtInfo[1].m_aExtType	= GetExtensionType( strVal.c_str() );	strThisNode = "";
			}
			else if(strThisNode == "ExtItemID3")
			{
				DamageInfo.m_ExtInfo[2].m_TItemID	= (T_ItemID)atoi(strVal.c_str());	strThisNode = "";
			}
			else if(strThisNode == "ExtType3")
			{
				DamageInfo.m_ExtInfo[2].m_aExtType	= GetExtensionType( strVal.c_str() );	strThisNode = "";
			}
			else if(strThisNode == "ExtItemID4")
			{
				DamageInfo.m_ExtInfo[3].m_TItemID	= (T_ItemID)atoi(strVal.c_str());	strThisNode = "";
			}
			else if(strThisNode == "ExtType4")
			{
				DamageInfo.m_ExtInfo[3].m_aExtType	= GetExtensionType( strVal.c_str() );	strThisNode = "";
			}
			else if(strThisNode == "DamageMax")
			{
				DamageInfo.m_r32MaxDamage = (REAL32)atoi(strVal.c_str());	strThisNode = "";
			}
			else if(strThisNode == "IsThrowWeapon")
			{
				DamageInfo.m_bLauncher = (BOOL)atoi(strVal.c_str());	strThisNode = "";
			}
			else if( strThisNode == "IndicatorType" )
			{
				if( strcmp( strVal.c_str(), "C5" ) == 0 )
				{
					DamageInfo.m_bIndicator = TRUE;
				}
				strThisNode = "";
			}
			else if(strThisNode == "MoveNormal_Fire")
			{
				REAL32 r32Speed = (REAL32)atof(strVal.c_str());
				strThisNode = "";

				if( r32Speed > DamageInfo.m_r32MoveSpeed )
					DamageInfo.m_r32MoveSpeed = r32Speed;
			}
			else if(strThisNode == "MoveNormal_Scope")
			{
				REAL32 r32Speed = (REAL32)atof(strVal.c_str());
				strThisNode = "";

				if( r32Speed > DamageInfo.m_r32MoveSpeed )
					DamageInfo.m_r32MoveSpeed = r32Speed;
			}
			else if(strThisNode == "MoveNormal_Sit")
			{
				REAL32 r32Speed = (REAL32)atof(strVal.c_str());
				strThisNode = "";

				if( r32Speed > DamageInfo.m_r32MoveSpeed )
					DamageInfo.m_r32MoveSpeed = r32Speed;
			}
			else if(strThisNode == "MoveNormal_Stand")
			{
				REAL32 r32Speed = (REAL32)atof(strVal.c_str());
				strThisNode = "";

				if( r32Speed > DamageInfo.m_r32MoveSpeed )
					DamageInfo.m_r32MoveSpeed = r32Speed;
			}
			else if(strThisNode == "MoveNormal_Zoom")
			{
				REAL32 r32Speed = (REAL32)atof(strVal.c_str());
				strThisNode = "";

				if( r32Speed > DamageInfo.m_r32MoveSpeed )
					DamageInfo.m_r32MoveSpeed = r32Speed;
			}
			else
			{
				for(INT32 idx = 0 ; sDamageType[idx]._attak > WEAPON::TARGET_NONE ; ++idx)
				{
					if(strThisNode == sDamageType[idx]._szDamageTypeName)
					{
						WEAPON::TARGET_TYPE target = sDamageType[idx]._target;
						WEAPON::ATTACK_TYPE attack = sDamageType[idx]._attak;

						if( WEAPON::TARGET_TYPE::TARGET_HUMAN != target )	continue;
						if( 0.f < DamageInfo.m_r32Damage[ attack ] )		continue;

						DamageInfo.m_r32Damage[ attack ]		= (REAL32)atoi(strVal.c_str());

						switch( attack )
						{
						case WEAPON::ATTACK_TYPE_L1:	case WEAPON::ATTACK_TYPE_L2:	case WEAPON::ATTACK_TYPE_L3:
							DamageInfo.m_eBulletType[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_LEFT ]		= WEAPON::BULLET_SHOT_DIRECT;
							break;
						case WEAPON::ATTACK_TYPE_R1:	case WEAPON::ATTACK_TYPE_R2:	case WEAPON::ATTACK_TYPE_R3:	case WEAPON::ATTACK_TYPE_R4:
							DamageInfo.m_eBulletType[ WEAPON::EXTENSION_OFF ][ WEAPON::HAND_TYPE_RIGHT ]	= WEAPON::BULLET_SHOT_DIRECT;
							break;
						}

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
		ParseWeaponTable(node.GetNode(i));
	}

	bool bItemParsed = (strDepthNode == "KEY_4" && bLastNode );

	// Insert a Weapon information to the Weapon table
	if(bItemParsed)
	{
		// chk list
		if(DamageInfo.m_TItemID)
		{
			WeaponInfo weaponTmp;

			if( Find( DamageInfo.m_TItemID, weaponTmp ) == FALSE )
			{
				if(DamageInfo.UseRandomDamage() == false)
					DamageInfo.m_r32MaxDamage = 0.0f;
				
				//DamageInfo.SetDinoDamage();

				Insert( DamageInfo.m_TItemID, DamageInfo);

				WEAPON_CLASS_TYPE ui32Class   = (WEAPON_CLASS_TYPE)(GET_ITEM_SUBTYPE(DamageInfo.m_TItemID));
				UINT32 ui32Number  = GET_ITEM_NUMBER( DamageInfo.m_TItemID );
				I3ASSERT( ui32Class < WEAPON_CLASS_COUNT );

				if( (UINT32)(ui32Number+1) > m_ui32WeaponTableCount[ ui32Class ] )
					m_ui32WeaponTableCount[ ui32Class ] = ui32Number+1;
			}
			else
			{
				DSERROR("ERR. Weapon Code duplicated(ItemID = %d\n)",DamageInfo.m_TItemID );
				I3ASSERT( 0 && "Weapon.i3RegXML KEY_4 Load Error / Weapon Code duplicated ");

			}
		}
		else 
		{
			I3ASSERT( 0 && "Weapon.i3RegXML KEY_4 Load Error / DamageInfo.m_TItemID is 0 "); 
		}

		// clear static variable 
		bLastNode = false;
		DamageInfo.Clear();
	}

}

// Parse the weapon xml file to load the Weapon table
void WeaponTable::ParseWeaponTable_New()
{
	if( 0 == m_mapWeaponTable.size() )
	{
		DSERROR("Weapon Count Errror !! ParseWeaponTable_New \n" );
		I3ASSERT( 0 && "Weapon.i3RegXML Load Error / Size "); 
		return;
	}

	for( UINT32 i = 0; i < WEAPON_CLASS_COUNT; ++i )
	{
		if( m_ui32WeaponTableCount[i] == 0 )	continue;

		I3TRACE("m_ui32WeaponTableCount[%d]=%d\n", i, m_ui32WeaponTableCount[i]);
		m_pWeaponTable[i] = new WeaponInfo[ m_ui32WeaponTableCount[i] ];
	}

	map<UINT32,WeaponInfo>::iterator it;

	for( it = m_mapWeaponTable.begin(); it != m_mapWeaponTable.end(); ++it )
	{
		UINT32 ui32Class   = GET_ITEM_SUBTYPE( it->second.m_TItemID );
		UINT32 ui32Number  = GET_ITEM_NUMBER( it->second.m_TItemID );

		if( ui32Class >= WEAPON_CLASS_COUNT || ui32Number >= m_ui32WeaponTableCount[ ui32Class ] )
		{
			DSERROR("Weapon Code Errror ParseWeaponTable_New(itemID=%d\n", it->second.m_TItemID );
		}
		else
		{
			m_pWeaponTable[ ui32Class ][ ui32Number ].Copy( &it->second );
		}
	}

	for( UINT32 i = 0 ; i < WEAPON_CLASS_COUNT ; ++i)
	{
		for(UINT32 j = 0 ; j < m_ui32WeaponTableCount[i] ; ++j)
		{
			if( m_pWeaponTable[i][j].m_TItemID )
			{
				m_pWeaponTable[i][j].BuildSpec();
				m_pWeaponTable[i][j].SetRegisterWeaponInfo(NULL);
			}
		}
	}
}

void WeaponTable::SetNationalColumnName(UINT8 ui8NationalCode)
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
	case MiddleEast: //12
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
