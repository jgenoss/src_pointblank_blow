#ifndef __COMMONDEF_ITEM_H__
#define __COMMONDEF_ITEM_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define

#define	DEFAULT_PARTS_ITEM_ID			0

// ±âº» Áö±Þ ¾ÆÀÌÅÛ DBIDX ±âÁØ°ª. 10¾ï. DB¿¡¼­ ÀÌ°Å ³ÑÀ¸¸é PB 2¸¸µì½Ã´Ù.
#define DEFAULT_ITEM_DBIDX_MIN			0x40000000		 

#define DEFAULT_PARTS_ITEM_ID_MIN		1000000000		 

#define DEFAULT_ITEM_ID_FLAG			0x1FFFFFF
#define DEFAULT_EQUIP_FLAG				0x3F		// ÃÖ´ë°ªÀº 64

// ±âº» ¾ÆÀÌÅÛ Item ID ÃßÃâ
#define GET_DEFAULT_ITEM_ID( DBIDX )		DBIDX & DEFAULT_ITEM_ID_FLAG

// ±âº» ÆÄÃ÷ Equip(ITEM_TYPE) 
#define GET_DEFAULT_EQUIP_TYPE( DBIDX )	DBIDX & DEFAULT_EQUIP_FLAG

// ±âº» ¾ÆÀÌÅÛ DB IDX »ý¼º( ÆÄÃ÷ Á¦¿Ü )
#define MAKE_DEFAULT_ITEM_DBIDX( ItemID )	DEFAULT_ITEM_DBIDX_MIN | ItemID

// ±âº» ÆÄÃ÷ DB IDX »ý¼º
#define MAKE_DEFAULT_EQUIP_DBIDX( Equip )	DEFAULT_ITEM_DBIDX_MIN |  Equip // ( Chara << DEFAULT_CHARA_BIT_ORDER ) |

// ±âº» ÆÄÃ÷ Item ID »ý¼º
#define MAKE_DEFAULT_PARTS_ITEMID( EquipType )	DEFAULT_PARTS_ITEM_ID_MIN + (  (EquipType) * 100000 ) // ( Chara << DEFAULT_CHARA_BIT_ORDER ) |

 
// ¹Ì»ç¿ë
//#define DEFAULT_CHARA_BIT_ORDER			6		// 	7¹øÀç~15 ºñÆ® 
// ±âº» Áö±Þ Ä³¸¯ÅÍ´Â 25~29¹øÂ° 5°³ ºñÆ®¸¦ ½½·Ô ÀÎµ¦½º·Î »ç¿ëÇÕ´Ï´Ù.
//#define DEFAULT_CHARA_BIT_ORDER		25
//#define DEFAULT_CHARA_FLAG			0x1FF		// ÃÖ´ë°ªÀº 512
//#define GET_DEFAULT_CHARA_TYPE( DBIDX )	(DBIDX >> DEFAULT_CHARA_BIT_ORDER ) & (DEFAULT_CHARA_FLAG ) 
//#define MAKE_DEFAULT_CHARA_DBIDX( ItemID, Slot ) MAKE_DEFAULT_ITEM_DBIDX(ItemID) | (Slot << DEFAULT_CHARA_BIT_ORDER)
//#define GET_DEFAULT_CHARA_SLOT( DBIDX )	( DBIDX >> DEFAULT_CHARA_BIT_ORDER ) & (0x1F) 


#define USE_BASKET_COUNT			8 // Àå¹Ù±¸´Ï ÃÖ´ë ¾ÆÀÌÅÛ °³¼ö
#define MAX_INVEN_COUNT				450			// ÀÎº¥Åä¸® Á¦ÇÑ

#define MAX_SHOP_USER_GIFT_COUNT	100					// ¼±¹°¹ÞÀº ¾ÆÀÌÅÛ Ä«¿îÆ®

#define GET_ITEM_EVENT_VALUE(Itemflag)					(((Itemflag) % 100000000) / 10000000)
#define GET_ITEM_TYPE(Itemflag)							(((Itemflag) % 10000000) / 100000)
#define GET_ITEM_SUBTYPE(Itemflag)						(((Itemflag) % 100000) / 1000)
#define GET_ITEM_NUMBER(Itemflag)						( (Itemflag) % 1000 )

#define GET_GOODSID_FROM_ITEMID(ItemID)					( ItemID * 100 )
#define GET_ITEMID_FROM_GOODSID(GoodsID)				( GoodsID / 100 )

#define GET_GOODS_TYPE(GoodsID)							( (GoodsID) / 10000000 )
#define GET_GOODS_SUBTYPE(GoodsID)						(((GoodsID) % 10000000) / 100000)
#define GET_GOODS_NUMBER(GoodsID)						(((GoodsID) % 100000) / 100)
#define GET_GOODS_ARG(GoodsID)							( (GoodsID) % 100 )


// Item Extension State °ü·Ã ÇÔ¼ö 
#define SET_ITEM_EXT_STATE(extflag, Itemid)				(((extflag) * 100000000) + Itemid) 
#define GET_ITEM_EXT_STATE(Itemflag)					((Itemflag) / 100000000)
#define REMOVE_ITEM_EXT_STATE(Itemflag)					((Itemflag) % 100000000)

//case ¹®¿¡ »ç¿ëÇÏ±â À§ÇÑ ItemID »ý¼º Define
//case ¹®¿¡¼­´Â ÇÔ¼ö¸¦ ¹Ù·Î È£ÃâÇÒ¼ö ¾ø±â¶§¹®¿¡ DefineÀ» »ç¿ëÇÕ´Ï´Ù.
//case ¹® ¿Ü¿¡¼­´Â ¹ØÀÇ ÀÎ¶óÀÎ ÇÔ¼ö¸¦ »ç¿ëÇÕ½Ã´Ù.
#define ITEM_ID( type, subtype, num )	( (type) * 100000 + (subtype) * 1000 + (num) )
#define MAKE_GOODS_ID( type, subtype, num, Arg )	( (type) * 10000000 + (subtype) * 100000 + (num) * 100 + (Arg) )

#define INVENTORY_FLAG_CHARACTER		0x00000001
#define INVENTORY_FLAG_WEAPON			0x00000002
#define INVENTORY_FLAG_ITEM				0x00000004

#define GET_POINTITEM(ItemID)			((ItemID % 100000) * 100)

#define CAPSULE_REWARD_ITEM_COUNT			10
////////////////////////////////////////////////////////////////////////////////
// Shop °ü·Ã
////////////////////////////////////////////////////////////////////////////////

// Field Shop
#define MAX_FIELDSHOP_CATEGORYNAMESIZE 		44
#define MAX_FIELDSHOP_CATEGORYCOUNT 		6
#define MAX_FIELDSHOP_GOODSLISTCOUNT		9
#define MAX_FIELDSHOP_GOODSPERIODCOUNT		4

////////////////////////////////////////////////////////////////
// RS. »Ì±â »óÁ¡ °ü·Ã

#define MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT 	6   		// Å¬¶óÀÌ¾ðÆ®¿¡¼­ º¸¿©Áö´Â ÅÇ´ç µî±Þº° ¾ÆÀÌÅÛ °¹¼ö.
#define MAX_RS_TAB_SHOW_ALL_ITEM_COUNT		18			// Å¬¶óÀÌ¾ðÆ®¿¡¼­ º¸¿©Áö´Â ÅÇ´ç ÃÑ ¾ÆÀÌÅÛ °¹¼ö.
#define MIN_RS_TAB_ITEM_COUNT 				10			// À¥Åø¿¡¼­ ¼³Á¤ÇÏ´Â ÇÑ ÅÇÀÇ µî±Þº° ¾ÆÀÌÅÛ ÃÖ¼Ò°¹¼ö
#define MAX_RS_TAB_ITEM_COUNT 				25			// À¥Åø¿¡¼­ ¼³Á¤ÇÏ´Â ÇÑ ÅÇÀÇ µî±Þº° ¾ÆÀÌÅÛ ÃÖ´ë °¹¼ö. 
#define MAX_RS_TAB_COUNT 					3			// ÃÖ´ë ÅÇ °¹¼ö
#define MAX_RS_ROULETTE_ITEM_COUNT			12			// ÇÑ ·ê·¿ÆÇÀÌ °¡Áö´Â ÃÖ´ë ¾ÆÀÌÅÛ ¼ö. ´ë¹Ú ¹ÌÆ÷ÇÔ.
#define MIN_RS_TAB_GRADE_COUNT				3			// ÇÑ ÅÇ¿¡¼­ ±¸¼ºÇÒ¼ö ÀÖ´Â º° °¹¼ö ÇÕÀÇ ÃÖ¼Ò°ª.
#define MAX_RS_TAB_GRADE_COUNT				10			// ÇÑ ÅÇ¿¡¼­ ±¸¼ºÇÒ¼ö ÀÖ´Â º° °¹¼ö ÇÕÀÇ ÃÖ´ë°ª.
#define MAX_RS_AD_MESSAGE_SIZE				101			// ±¤°í ¸Þ½ÃÁö ±æÀÌ.
#define MAX_RS_AD_MESSAGE_COUNT				3			// ±¤°í ¸Þ½ÃÁö °¹¼ö.
#define MAX_RS_HIDDEN_ITEM_COUNT			10			// ÇÏ³ªÀÇ È÷µç ¼¼Æ®°¡ °¡Áö´Â ¾ÆÀÌÅÛ °³¼ö.
#define	MAX_RS_BONUS_COUNT					5			// º¸³Ê½º ÆÇ¼ö.(±âÁØ)
#define RS_ITEM_INFO_UPDATE_TIME			2			// Game ¼­¹ö¿¡ »Ì±â»óÁ¡ »óÇ° ±¸¼ºÀ» ¾÷µ¥ÀÌÆ® ÇØÁÖ´Â ½Ã°£

#define MAKE_RS_TAB_GOODSID(tabidx,playcount)	(((GOODS_TYPE_RSPRICE)*10000000 )+((tabidx)*100)+(playcount))	// TabGoodsID »ý¼º. ÅÇ ¹øÈ£, ÀÌ¿ë È½¼ö

#define GET_GOODS_FLAG_RS_TAB_IDX(Goodsflag)			(((Goodsflag) % 10000000) / 100)	// ±ÂÁî ¾ÆÀÌµð·Î »Ì±â »óÁ¡ ÅÇ¹øÈ£ ÃßÃâ.

#define DEFAULT_WEAPON_ASSAULT							MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, 	WEAPON_CLASS_ASSAULT,			4)			//  103004 : K-2
#define DEFAULT_WEAPON_SNIPER							MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, 	WEAPON_CLASS_SNIPER,			3)			//  105003 : SSG-69	
#define DEFAULT_WEAPON_SMG								MAKE_ITEM_ID(ITEM_TYPE_PRIMARY,		WEAPON_CLASS_SMG,				6)			//  104006 : K-1
#define DEFAULT_WEAPON_SHOTGUN							MAKE_ITEM_ID(ITEM_TYPE_PRIMARY,		WEAPON_CLASS_SHOTGUN,			1)			//  106001 : 870MCS
#define DEFAULT_WEAPON_HANDGUN							MAKE_ITEM_ID(ITEM_TYPE_SECONDARY,	WEAPON_CLASS_HANDGUN, 			3)			//  202003 : K5
#define DEFAULT_WEAPON_KNIFE							MAKE_ITEM_ID(ITEM_TYPE_MELEE,		WEAPON_CLASS_KNIFE,				1)			//  301001 : M7		
#define DEFAULT_WEAPON_ITEM								MAKE_ITEM_ID(ITEM_TYPE_THROWING2,	WEAPON_CLASS_THROWING_CONTAIN,	1)			//  508001 : SMOKE		
#define DEFAULT_WEAPON_MG								MAKE_ITEM_ID(ITEM_TYPE_PRIMARY,		WEAPON_CLASS_MG,				9)			//  110009 : MK.46.Non.Ext
#define DEFAULT_WEAPON_THROWING1						MAKE_ITEM_ID(ITEM_TYPE_THROWING1,	WEAPON_CLASS_THROWING_GRENADE,	1)			//  407001 : K400
#define DEFAULT_WEAPON_THROWING2						MAKE_ITEM_ID(ITEM_TYPE_THROWING2,	WEAPON_CLASS_THROWING_CONTAIN,	1)			//  508001 : SMOKE
#define DEFAULT_WEAPON_MISSON							MAKE_ITEM_ID(ITEM_TYPE_MISSION,		WEAPON_CLASS_MISSION,			1)

#define DEFAULT_CHARA_RED_TEAM							MAKE_ITEM_ID(ITEM_TYPE_CHARA, CHARACTER_TEAM_RED,	CHARA_RES_ID_RED_BULLS)
#define DEFAULT_CHARA_BLUE_TEAM							MAKE_ITEM_ID(ITEM_TYPE_CHARA, CHARACTER_TEAM_BLUE,	CHARA_RES_ID_ACID_POLE)
#define DEFAULT_CHARA_DINO								MAKE_ITEM_ID(ITEM_TYPE_DINO,  CHARACTER_TEAM_NONE,	CHARA_RES_ID_DINO_RAPTOR)

#define DEFAULT_EQUIP_HEAD								MAKE_ITEM_ID(ITEM_TYPE_HEADGEAR, 0, HEADGEAR_RES_ID_DEFAULT)

// ¾ÆÀÌÅÛ ¾ÆÀÌµð ³×¹øÂ°¿¡ µé¾î°©´Ï´Ù. ( 4 )
//
#define MAX_RAND_COUNT			10000 // »Ì±â»óÁ¡ È®·ü ÃÖ´ë°ª

// ÄíÆù¹øÈ£±æÀÌ
#define GIFTCOUPON_LENGTH					16
// ÄíÆù ÇÑÀåÀ¸·Î ¹ÞÀ» ¼ö ÀÖ´Â ÃÖ´ë Goods °¹¼ö
#define MAX_GIFTCOUPON_COUNT				4

#define BERET_ASSAULT_ITEMID		2700014
#define BERET_HANDGUN_ITEMID		2700015
#define BERET_RIFLE_ITEMID			2700016
#define BERET_SMG_ITEMID			2700017
#define BERET_SHOTGUN_ITEMID		2700018

#define BERET_ASSAULT_TITLE			13
#define BERET_RIFLE_TITLE			19
#define BERET_SMG_TITLE				25
#define BERET_HANDGUN_TITLE			34
#define BERET_SHOTGUN_TITLE			39

// Çìµå ±â¾î ÀåÂø Å¸ÀÔ
enum HEAD_PARTS_EQUIP_TYPE
{
	HEAD_PARTS_EQUIP_TYPE_DEFAULT	=	0,	// Çìµå±â¾î¸¸ Âø¿ë°¡´É
	HEAD_PARTS_EQUIP_TYPE_BERET,			// º£·¹¸ð¿Í Áßº¹ ÀåÂø °¡´É
};

// Ä³½¬ ¾ÆÀÌÅÛ ½½·Ô¿¡ ´ã±ä Á¤º¸
enum SLOT_ITEM_FLAG {																	
	SLOT_ITEM_BULLET_PROOF_VEST_GM			=	0x00000001,
	SLOT_ITEM_KETUPAT						=	0x00000002,
	SLOT_ITEM_BULLET_PROOF_VEST_METAL		=	0x00000004,
	SLOT_ITEM_HOLLOW_POINT_AMMO_PLUS		=	0x00000008,

	SLOT_ITEM_BULLET_PROOF_VEST_PLUS		=	0x00000010,
	SLOT_ITEM_MEGA_HP5						=	0x00000020,
	SLOT_ITEM_JACKETED_HELLOW_POINT_AMMO	=	0x00000040,
	SLOT_ITEM_INCREASE_GRENADE_SLOT			=	0x00000080,

	SLOT_ITEM_C4_SPEED_KIT					=	0x00000100,
	SLOT_ITEM_HOLLOW_POINT_AMMO				=	0x00000200,
	SLOT_ITEM_FULLMETALJACKETAMMO			=	0x00000400,
	SLOT_ITEM_BULLET_PROOF_VEST				=	0x00000800,

	SLOT_ITEM_INCREASE_INVINCIBLE_TIME		=	0x00001000,
	SLOT_ITEM_MEGA_HP10						=	0x00002000,
	SLOT_ITEM_QUICK_CHANGE_MAGAZINE			=	0x00004000,
	SLOT_ITEM_QUICK_CHANGE_WEAPON			=	0x00008000,

	SLOT_ITEM_VALENTINE_CHOCOLATE			=	0x00010000,			// ¹ß·»Å¸ÀÎ ÃÊÄÚ·¿ (HP+10)
	SLOT_ITEM_PICKUP_WEAPON					=	0x00020000,
	SLOT_ITEM_AMMO_UP						=	0x00040000,			// Extended Magazines
	SLOT_ITEM_SHORT_RESPAWN_20				=	0x00080000,

	SLOT_ITEM_SHORT_RESPAWN_30				=	0x00100000,
	SLOT_ITEM_SHORT_RESPAWN_50				=	0x00200000,
	SLOT_ITEM_SHORT_RESPAWN_100				=	0x00400000,
	SLOT_ITEM_UPGRADE_REINFORCED			=	0x00800000,			// HP, ÀÌµ¿¼Óµµ, ¹ÝÀÀ¼Óµµ + 20%
	
	SLOT_ITEM_UPGRADE_DAMAGE				=	0x01000000,			// µ¥¹ÌÁö + 5%
	SLOT_ITEM_UPGRADE_DEFENSE				=	0x02000000,			// ¹æ¾î·Â + 10%
	SLOT_ITEM_INCREASE_THR2_SLOT			=	0x04000000,			// Æ¯¼ö¹«±â ½½·Ô Áõ°¡
};

#define HP_RATE_MEGA_HP10						0.1f
#define HP_RATE_MEGA_HP5						0.05f
#define HP_RATE_UPGRADE_REINFORCED				0.2f	// 20%
#define HP_UP_VALENTINE_CHOCOLATE				10
#define HP_UP_KETUPAT							10

#define DEFENCE_RATE_VEST						0.05f
#define DEFENCE_RATE_VEST_PLUS					0.10f
#define DEFENCE_RATE_VEST_METAL					0.20f
#define DEFENCE_RATE_VEST_GM					0.90f
#define DEFENCE_RATE_UPGRADE_DEFENSE			0.10f	// 10%

#define DAMAGE_RATE_ARMOR_PIERCING				0.1f					// Ã¶°©Åº
#define DAMAGE_RATE_HOLLOW_POINT				-0.1f				// ÇÒ·Î¿ì Æ÷ÀÎÆ®Åº
#define DAMAGE_RATE_ENHANCED_HOLLOW_POINT		0.1f
#define DAMAGE_RATE_JACKETED_HOLLOW_POINT		0.05f	
#define DAMAGE_RATE_UPGRADE_DAMAGE				0.05f	// 5%

// ´É·ÂÄ¡ UP ÃÖ´ëÄ¡( µ¥¹ÌÁö, ¹æ¾î·Â, HP µî )
#define MAX_RATE_UPGREADE_STATS					0.2f	// 20%


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum
enum WEAPON_SLOT_TYPE
{
	WEAPON_SLOT_UNKNOWN		= -1,
	WEAPON_SLOT_ALL			= WEAPON_SLOT_UNKNOWN,
	WEAPON_SLOT_PRIMARY		= 0,	//	ÁÖ¹«±â
	WEAPON_SLOT_SECONDARY	= 1,	//	º¸Á¶¹«±â
	WEAPON_SLOT_MELEE		= 2,	//	±ÙÁ¢¹«±â
	WEAPON_SLOT_THROWING1	= 3,	//	ÅõÃ´¹«±â	
	WEAPON_SLOT_THROWING2	= 4,	//	Æ¯¼ö ¾ÆÀÌÅÛ
	WEAPON_SLOT_MISSION		= 5,	//	¹Ì¼Ç ¾ÆÀÌÅÛ

	WEAPON_SLOT_COUNT,

	//Ä³¸¯ÅÍ ºä º¯°æ½Ã¿¡ »ç¿ëµÇ´Â enum ÀÌ´Ù. (WEAPON_SLOT_PRIMARY ~ WEAPON_SLOT_MISSION)
	WEAPON_CHARA_SLOT_MAX = WEAPON_SLOT_MISSION,
}; // ÀÎ°ÔÀÓ¿¡¼­ »ç¿ëÇÒ ¹«±â ½½·Ô ¹è¿­ ÀÎµ¦½º(PLAY_MULTI_SLOT)

enum AUTH_TYPE
{
	AUTH_UNKNWON	= 0,
	AUTH_COUNT		= 1,	// °³¼öÁ¦ ÀÎÁõ
	AUTH_SEC				// ±â°£Á¦ ÀÎÁõ
};

// ¾ÆÀÌÅÛ Áö±Þ Å¸ÀÔ. Client - All Server( Game, Trans, SIA, ZLog ) ¿ë.
enum ITEM_INSERT_REQUEST_TYPE
{
	ITEM_INSERT_REQUEST_UNKNOWN			= -1,
	ITEM_INSERT_REQUEST_BUY				= 0,	// »óÁ¡ ±¸¸Å
	ITEM_INSERT_REQUEST_TS_BONUS		= 1,	// TS º¸³Ê½º
	ITEM_INSERT_REQUEST_CAPSULE			= 2,	// Ä¸½¶ ¾ÆÀÌÅÛ ÀÎÁõ
	ITEM_INSERT_REQUEST_RANKUP			= 3,	// ·¹º§ ¾÷
	ITEM_INSERT_REQUEST_MISSION_CARD	= 4,	// ÀÓ¹«Ä«µå º¸»ó
	ITEM_INSERT_REQUEST_GET_TITLE		= 5,	// È£ÄªÈ¹µæ º¸»ó
	ITEM_INSERT_REQUEST_GIFT_AUTH		= 6,	// ¼±¹° ÀÎÁõ
	ITEM_INSERT_REQUEST_AUTH			= 7,	// ¾ÆÀÌÅÛ ÀÎÁõ.(À¯Àú)
	ITEM_INSERT_REQUEST_ATTENDANCE		= 8,	// Ãâ¼®Ã¼Å©.
	ITEM_INSERT_REQUEST_CREATE_NEWUSER	= 9,	// °èÁ¤ »ý¼º
	ITEM_INSERT_REQUEST_EVENT_CONNECT	= 10,	// ±âÁ¸ À¯Àú ÀÌº¥Æ® ¾ÆÀÌÅÛ.
	ITEM_INSERT_REQUEST_EVENT_NEWUSER	= 11,	// ½Å±Ô À¯Àú ÀÌº¥Æ® ¾ÆÀÌÅÛ
	ITEM_INSERT_REQUEST_GACHA			= 12,	// »Ì±â »óÁ¡(Renewal)
	ITEM_INSERT_REQUEST_MEDAL_REWARD	= 13,	// ¸Þ´Þ º¸»ó
	ITEM_INSERT_REQUEST_DOMI_REWARD		= 14,	// Á¦¾Ð º¸»ó
	ITEM_INSERT_REQUEST_WEB_GIFT		= 15,	// Web Gift
	ITEM_INSERT_REQUEST_GENERAL			= 16,	// Àå¼º±Þ Àü¿ë ¾ÆÀÌÅÛ Áö±Þ
	ITEM_INSERT_REQUEST_USER_GIFT		= 17,	// À¯Àú ¼±¹° ±¸¸Å Áö±Þ
	ITEM_INSERT_REQUEST_WEB_SHOP		= 18,	// À¥»óÁ¡ ±¸¸Å Áö±Þ
	ITEM_INSERT_REQUEST_DORMANT			= 19,	// ÈÞ¸Õ À¯Àú ÃâÃ½ º¸»ó ¾ÆÀÌÅÛ Áö±Þ
};

enum GET_EXP_ROUTE
{
	GET_EXP_ROUTE_GAMEPLAY = 1,
	GET_EXP_ROUTE_MEDAL_REWARD,
	GET_EXP_ROUTE_MAX
};


enum GET_POINT_ROUTE
{
	GET_POINT_ROUTE_GAMEPLAY = 1,
	GET_POINT_ROUTE_QUEST,
	GET_POINT_ROUTE_ITEM,
	GET_POINT_ROUTE_CREATE_CHARA,
	GET_POINT_ROUTE_RANKUP,
	GET_POINT_ROUTE_EVENT,
	GET_POINT_ROUTE_MEDAL_REWARD,
	GET_POINT_ROUTE_RS_JACKPOT,
	GET_POINT_ROUTE_GUIDE,				// Guide
	GET_POINT_ROUTE_DOMI_REWARD,		// Á¦¾Ð ½Ç½Ã°£ º¸»ó.	
	GET_POINT_ROUTE_MAX
};

enum CHARACTER_CLASS_TYPE
{
	CHARACTER_CLASS_UNKNOWN = -1,
	CHARACTER_CLASS_ALL = 0,

	CHARACTER_CLASS_CHARA_TYPE,		//	Ä³¸¯ÅÍ
	CHARACTER_CLASS_HEADGEAR,		//	Çï¸ä
	CHARACTER_CLASS_FACEGEAR,		//	FaceGear
	CHARACTER_CLASS_UPPER,			//	»óÀÇ
	CHARACTER_CLASS_LOWER,			//	ÇÏÀÇ
	CHARACTER_CLASS_GLOVE,			//	Àå°©
	CHARACTER_CLASS_BELT,			//	º§Æ®
	CHARACTER_CLASS_HOLSTER,		//	±ÇÃÑÁý
	CHARACTER_CLASS_SKIN,			//	SKIN
	CHARACTER_CLASS_CHARA_DINO,		//	°ø·æ
	CHARACTER_CLASS_RESERVED,		//	(Á¦¾Ð¼³Ä¡¿ë¹«±â)
	CHARACTER_CLASS_BERET,			//	º£·¹¸ð

	CHARACTER_CLASS_COUNT
};

enum SKIN_CLASS_TYPE
{
	SKIN_CLASS_UNKNOWN = 0,
	SKIN_CLASS_COMMON,
	SKIN_CLASS_MAN,
	SKIN_CLASS_WOMAN,

	SKIN_CLASS_COUNT
};

enum CASHITEM_USAGE_TYPE
{
	CASHITEM_USAGE_UNKNOWN		= -1,
	CASHITEM_USAGE_ALL			= CASHITEM_USAGE_UNKNOWN,
	CASHITEM_USAGE_MAINTENANCE	= 0,	//	Áö¼Ó¼º ¾ÆÀÌÅÛ
	CASHITEM_USAGE_EXPENDABLES	= 1,	//	¼Ò¸ð¼º ¾ÆÀÌÅÛ

	CASHITEM_USAGE_COUNT,
};

enum CASHITEM_CLASS_TYPE
{
	CASHITEM_DAY_NO		= 0,	//	¼Ò¸ð¼º ¾ÆÀÌÅÛ(ITEM_TYPE_WRAP_COUNT ÀÎ°æ¿ì)
	CASHITEM_DAY_1		= 1,	// 
	CASHITEM_DAY_3		= 3,	//
	CASHITEM_DAY_7		= 7,	//
	CASHITEM_DAY_10		= 10,	//
	CASHITEM_DAY_15		= 15,	//
	CASHITEM_DAY_30		= 30,	//
	CASHITEM_DAY_90		= 90,	//
};

//
// Item Event Type
//
// ¾ÆÀÌÅÛ ¾ÆÀÌµð Ã¹¹øÂ°¿¡ µé¾î°©´Ï´Ù. ( 1 )
//
// ÀÌº¥Æ® ÃÑ±â ±¸ºÐÀ» ÇÒ °æ¿ì »ç¿ëÇÕ´Ï´Ù.
//
enum WEAPON_CLASS_TYPE
{
	WEAPON_CLASS_UNKNOWN	= 0,

	WEAPON_CLASS_KNIFE,				//	³ªÀÌÇÁ
	WEAPON_CLASS_HANDGUN,			//	±ÇÃÑ
	WEAPON_CLASS_ASSAULT,			//	¼ÒÃÑ
	WEAPON_CLASS_SMG,				//	¼­ºê¸Ó½Å°Ç
	WEAPON_CLASS_SNIPER,			//	Àú°ÝÃÑ
	WEAPON_CLASS_SHOTGUN,			//	»êÅºÃÑ
	WEAPON_CLASS_THROWING_GRENADE,	//	Æø¹ßÇü ÅõÃ´ ¹«±â (ex. K400, K-413, C5..) 
	WEAPON_CLASS_THROWING_CONTAIN,	//	°ßÁ¦Çü ÅõÃ´ ¹«±â (ex. Smoke, Flash Bang..)
	WEAPON_CLASS_MISSION,			//	¹Ì¼Ç ¾ÆÀÌÅÛ
	WEAPON_CLASS_MG,				//	¸Ó½Å°Ç

	// °ÔÀÓ¿¡¼­¸¸ »ç¿ëÇÏ´Â Å¬·¡½º ( ¼­¹ö¿¡´Â ¾ø½À´Ï´Ù.)
	WEAPON_CLASS_GRENADESHELL,		// À¯Åº
	WEAPON_CLASS_BOMBTRIGGER,		// ÆøÅº ½ºÀ§Ä¡

	// »õ·Î Ãß°¡µÇ´Â ¹«±â 2008.07.09
	WEAPON_CLASS_CIC,
	WEAPON_CLASS_DUALHANDGUN,		// Dual HandGun
	WEAPON_CLASS_DUALKNIFE,			// Dual Knife

	WEAPON_CLASS_ROCKET_LAUNCHER,
	WEAPON_CLASS_OBJECT_GUN,		// Æ¯Á¤ ¿ÀºêÁ§Æ®¿¡ ºÙ¾î ÀÖ´Â ÃÑ ( M197....)

	WEAPON_CLASS_DUALSMG,			// Dual SMG

	WEAPON_CLASS_DINO,		
	WEAPON_CLASS_DINO_LEFT_SCRATCH,	// ¿ÞÂÊ ¼Õ ÇÒÄû±â ÀÌÆåÆ®¸¦ À§ÇØ ¸¸µë.
	WEAPON_CLASS_TREX,				// ¿ÞÂÊ ¼Õ ÇÒÄû±â ÀÌÆåÆ®¸¦ À§ÇØ ¸¸µë.
	WEAPON_CLASS_STING,
	WEAPON_CLASS_KNUCKLE,			// ³ÊÅ¬
	WEAPON_CLASS_ELITE_RIGHT_SCRATCH,
	WEAPON_CLASS_ELITE_LEFT_SCRATCH,
	WEAPON_CLASS_BOW,

	WEAPON_CLASS_THROWING_ATTACK,	// °ø°ÝÇü ÅõÃ´ ¹«±â (ex. WP Smoke..)
	WEAPON_CLASS_THROWING_HEAL,		// È¸º¹Çü ÅõÃ´ ¹«±â (ex. Medical Kit..)

	WEAPON_SUBCLASS_ASSAULT,		// º¸Á¶¹«±âÇü ¾î¼³Æ®
	WEAPON_SUBCLASS_SHOTGUN,		// º¸Á¶¹«±âÇü ¼¦°Ç
	WEAPON_SUBCLASS_SNIPER,			// º¸Á¶¹«±âÇü ½º³ª
	WEAPON_SUBCLASS_SMG,			// º¸Á¶¹«±âÇü SMG
	WEAPON_SUBCLASS_G_LAUNCHER,		// º¸Á¶¹«±âÇü À¯Åº ¹ß»ç±â
	WEAPON_SUBCLASS_BOW,

	WEAPON_CLASS_DUALSHOTGUN,		// Dual ShotGun 

	WEAPON_CLASS_COUNT
};

enum ITEM_EVENT_TYPE
{
	ITEM_EVENT_TYPE_NONE = 0,
	ITEM_EVENT_TYPE_EVENT_ITEM,

	MAX_ITEM_EVENT_TYPE
};

// v1.5 ItemID Ã¹¹øÂ°.
enum ITEM_TYPE
{
	ITEM_TYPE_UNKNOWN	= 0,

	// ¹«±â
	ITEM_TYPE_PRIMARY	= 1,	// ÁÖ¹«±â
	ITEM_TYPE_SECONDARY,		// º¸Á¶¹«±â
	ITEM_TYPE_MELEE,			// ±ÙÁ¢¹«±â
	ITEM_TYPE_THROWING1,		// ¼ö·ùÅº
	ITEM_TYPE_THROWING2,		// Æ¯¼ö¹«±â(¿¬¸·)

	// ÆÄÃ÷
	ITEM_TYPE_CHARA,			// Ä³¸¯ÅÍ ¸öÃ¼
	ITEM_TYPE_HEADGEAR,			// HeadGear
	ITEM_TYPE_FACEGEAR,			// FaceGear
	ITEM_TYPE_UPPER,			// »óÀÇ
	ITEM_TYPE_LOWER,			// ÇÏÀÇ 10
	ITEM_TYPE_GLOVE,			// Àå°©
	ITEM_TYPE_BELT,				// º§Æ®
	ITEM_TYPE_HOLSTER,			// ±ÇÃÑÁý 
	ITEM_TYPE_SKIN,				// ½ºÅ²

	ITEM_TYPE_DINO,				// °ø·æ

	// ITEM_TYPE_WRAP_PERIOD ÀÌ ITEM_TYPE_MAINTENANCEÀ» ´ã°í ÀÖ´Â ÇüÅÂÀÔ´Ï´Ù.
	ITEM_TYPE_MAINTENANCE,		// ±â°£Á¦ ¾ÆÀÌÅÛ - ¼Ò¸ð¿ë(Expendable)
	ITEM_TYPE_WRAP_PERIOD,		// °¹¼öÁ¦ ¾ÆÀÌÅÛ(±â°£Á¦¾ÆÀÌÅÛÀÇ Æ÷ÀåÁö·Î »ç¿ë°¡´É) - Áö¼Ó¿ë(Maintenance)

	// ÀÌ³ðÀº ±×³É °¹¼öÁ¦. µ¶°í´ÙÀÌ.
	ITEM_TYPE_WRAP_COUNT,		// °¹¼öÁ¦ ¾ÆÀÌÅÛ - ¼Ò¸ð¿ë(Expendable)

	ITEM_TYPE_COUPON,			// ¶óÀÌ¼¾½º(?)
	ITEM_TYPE_POINT,			// Æ÷ÀÎÆ® ¾ÆÀÌÅÛ(20)
	
	ITEM_TYPE_SALECOUPON,		// ÇÒÀÎÄíÆù
	ITEM_TYPE_RSCOUPON,			// »Ì±âÀÌ¿ë±Ç

	ITEM_TYPE_INGAMEITEM,		// ¹èÆ²³» È¿°ú ¾ÆÀÌÅÛ (Á¦¾Ð¹Ì¼Ç ¾ßÀü»óÁ¡ ¾ÆÀÌÅÛ)
	ITEM_TYPE_DOMIITEM,			// ¹èÆ²¿Ü È¿°ú ¾ÆÀÌÅÛ (Á¦¾Ð¹Ì¼Ç)

	ITEM_TYPE_GACHA_TICKET,		// »Ì±âÀÌ¿ë±Ç
	ITEM_TYPE_QUESTCARD,		// ÀÓ¹«Ä«µå

	ITEM_TYPE_BERET,			// º£·¹¸ð

	//////////////////////////////////////////////////////////////////////////
	// ¾Æ·¡´Â Å¬¶óÀÌ¾ðÆ®¿¡¼­ »ç¿ë( DB¿¡ ÀÖÀ» °æ¿ì ÀÖÀ½)
	ITEM_TYPE_MISSION	= 50,	// ¼³Ä¡ ¿ÀºêÁ§Æ® ¹«±â (WEAPON_SLOT_MISSION), C4,¼¾Æ®¸®°Ç µî

	//////////////////////////////////////////////////////////////////////////
	// ¾Æ·¡´Â  DB¿¡¼­´Â »ç¿ë¾ÈÇÔ	
	ITEM_TYPE_TEMP,				// RocketLauncher

	//////////////////////////////////////////////////////////////////////////
	ITEM_TYPE_SET		= 99,	// ±ÂÁî ¾ÆÀÌµð·Î 99´Â ¼¼Æ® ¾ÆÀÌÅÛÀÔ´Ï´Ù
	
	//////////////////////////////////////////////////////////////////////////
	MAX_ITEM_TYPE_COUNT			// (DB¿¡¼­´Â »ç¿ë¾ÈÇÔ)
};

//////////////////////////////////////////////////////////////////////////////////////
//
//	À¯·á ¾ÆÀÌÅÛÁß¿¡¼­ °°Àº °è¿­ »óÇ°À» ÇÏ³ª·Î ¹­±â À§ÇÑ ±×·ì idx (ÀÎµ¦½º ¹Ù²î¸é ¾ÈµÊ)
//
// ¾ÆÀÌÅÛ ¾ÆÀÌµð ¼¼¹øÂ°¿¡ µé¾î°©´Ï´Ù. ( 3 )
//
//	Group Index¸¦ ¹ö¸®°í ItemID·Î ÅëÇÕÇÕ´Ï´Ù.
enum CASHITEM_ITEMID_TYPE
{
	CASHITEM_ITEMID_UNKNOWN = 0,

	CASHITEM_ITEMID_XP_PLUS_110								= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 1),
	CASHITEM_ITEMID_XP_PLUS_130								= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 2),
	CASHITEM_ITEMID_XP_PLUS_150								= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 3),
	CASHITEM_ITEMID_PT_PLUS_130								= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 4),
	CASHITEM_ITEMID_COLOR_CLAN								= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 5),

	CASHITEM_ITEMID_COLOR_NICK								= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 6),
	CASHITEM_ITEMID_SHORT_RESPAWN							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 7),
	CASHITEM_ITEMID_AMMO_UP									= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 8),
	CASHITEM_ITEMID_DISGUISE_RANK							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 9),
	CASHITEM_ITEMID_DISGUISE_NICK							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 10),

	CASHITEM_ITEMID_FREE_MOVE								= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 11),
	CASHITEM_ITEMID_CLAN_XP_PLUS_150						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 12),
	CASHITEM_ITEMID_CUSTOM_CROSSHAIR = 13,
	CASHITEM_ITEMID_COLOR_CROSSHAIR							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 14),
	CASHITEM_ITEMID_COLOR_CHATTING							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 15),

	CASHITEM_ITEMID_BATTLEFIELD_MOVE						= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 16),
	CASHITEM_ITEMID_GET_DROPPED_WEAPON						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 17),
	CASHITEM_ITEMID_XP_PLUS_X2_12PM							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 18),
	CASHITEM_ITEMID_XP_PLUS_X2_3PM							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 19),
	CASHITEM_ITEMID_XP_PLUS_X2_6PM							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 20),

	CASHITEM_ITEMID_XP_PLUS_X2_9PM							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 21),
	CASHITEM_ITEMID_PT_PLUS_X2_12PM							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 22),
	CASHITEM_ITEMID_PT_PLUS_X2_3PM							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 23),
	CASHITEM_ITEMID_PT_PLUS_X2_6PM							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 24),
	CASHITEM_ITEMID_PT_PLUS_X2_9PM							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 25),

	CASHITEM_ITEMID_QUICK_CHANGE_WEAPON						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 26),
	CASHITEM_ITEMID_QUICK_CHANGE_MAGAZINE					= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 27),
	CASHITEM_ITEMID_MEGA_HP10								= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 28),
	CASHITEM_ITEMID_INCREASE_INVINCIBLE_TIME				= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 29),
	CASHITEM_ITEMID_BULLET_PROOF_VEST						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 30),

	CASHITEM_ITEMID_FULLMETALJACKETAMMO						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 31),
	CASHITEM_ITEMID_HOLLOW_POINT_AMMO						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 32),
	CASHITEM_ITEMID_ANTI_FLASHBANG_MASK						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 33),
	CASHITEM_ITEMID_C4_SPEED_KIT							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 34),
	CASHITEM_ITEMID_INCREASE_GRENADE_SLOT					= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 35),

	CASHITEM_ITEMID_JACKETED_HELLOW_POINT_AMMO				= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 36),
	CASHITEM_ITEMID_XP_PLUS_200								= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 37),
	CASHITEM_ITEMID_PT_PLUS_200								= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 38),
	CASHITEM_ITEMID_MEGA_HP5								= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 40),

	CASHITEM_ITEMID_BULLET_PROOF_VEST_PLUS					= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 44),
	
	CASHITEM_ITEMID_CHANGE_NICK								= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 47),
	CASHITEM_ITEMID_CLR_HISTORY								= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 48),
	CASHITEM_ITEMID_CLR_SCORE								= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 49),
	CASHITEM_ITEMID_CLR_ESCAPE								= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 50),

	CASHITEM_ITEMID_CHANGE_CLAN_NAME						= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 51),
	CASHITEM_ITEMID_CHANGE_CLAN_MARK						= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 52),
	CASHITEM_ITEMID_CLR_CLAN_HISTORY						= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 53),
	CASHITEM_ITEMID_CLAN_PERSON_PLUS_50						= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 55),

	CASHITEM_ITEMID_CLAN_POINT_RESET						= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 56),
	
	CASHITEM_ITEMID_XP_PLUS_200_0AM_6AM						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 62),
	CASHITEM_ITEMID_XP_PLUS_200_6AM_12PM					= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 63),
	CASHITEM_ITEMID_SHORT_RESPAWN_50						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 64),
	CASHITEM_ITEMID_BULLET_PROOF_VEST_GM					= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 65),

	CASHITEM_ITEMID_SHORT_RESPAWN_20						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 77),
	CASHITEM_ITEMID_HOLLOW_POINT_AMMO_PLUS					= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 78),
	CASHITEM_ITEMID_BULLET_PROOF_VEST_METAL					= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 79),
	CASHITEM_ITEMID_SHORT_RESPAWN_100						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 80),

	CASHITEM_ITEMID_VIEW_OTHER_USER_ITEM_INFO				= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 85),
	
	CASHITEM_ITEMID_PT_PLUS_150								= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 119),	// Point 150%	
	
	// ´ë¸¸ ÀÌº¥Æ® ¾ÆÀÌÅÛ.
	CASHITEM_ITEMID_PT_PLUS_130_EV							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 159),
	CASHITEM_ITEMID_XP_PLUS_130_EV							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 160),
	CASHITEM_ITEMID_CLAN_XP_PLUS_150_EV						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 161),
	CASHITEM_ITEMID_FREE_MOVE_EV							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 162),
	CASHITEM_ITEMID_SHORT_RESPAWN_30_EV						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 163),
	CASHITEM_ITEMID_SHORT_RESPAWN_20_EV						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 164),
	CASHITEM_ITEMID_QUICK_CHANGE_WEAPON_EV					= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 165),
	CASHITEM_ITEMID_QUICK_CHANGE_MAGAZINE_EV				= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 166),
	CASHITEM_ITEMID_INCREASE_INVINCIBLE_TIME_EV				= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 167),
	CASHITEM_ITEMID_GET_DROPPED_WEAPON_EV					= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 168),
	CASHITEM_ITEMID_HOLLOW_POINT_AMMO_EV					= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 169),
	CASHITEM_ITEMID_FULLMETALJACKETAMMO_EV					= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 170),

	CASHITEM_ITEMID_CLAN_PERSON_PLUS_10						= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 171),	//2013.12.04 ½Ì¸» ¿äÃ»¿¡ ÀÇÇÑ Å¬·£¿ø 10¸íÁõ°¡ ¾ÆÀÌÅÛ

	//Reinforced Ä³¸¯ÅÍ È¯±Þ ¾ÆÀÌÅÛ, Mask È¯±Þ ¾ÆÀÌÅÛ - 2014.04.23
	CASHITEM_ITEMID_UPGRADE_REINFORCED						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 172),	//Ä³¸¯ÅÍ°­È­¾ÆÀÌÅÛ(HP,ÀÌ¼Ó,¹ÝÀÀ¼Óµµ Áõ°¡)
	CASHITEM_ITEMID_UPGRADE_DAMAGE							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 177),	//µ¥¹ÌÁö °­È­ ¾ÆÀÌÅÛ
	CASHITEM_ITEMID_UPGRADE_DEFENSE							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 178),	//¹æ¾î·Â °­È­ ¾ÆÀÌÅÛ

	//Ä³¸¯ÅÍ ½ºÅ³ °³¹æ±Ç, ½ºÅ³ °æÇèÄ¡ ºÎ½ºÅÍ ¾ÆÀÌÅÛ - 2014.04.23
	CASHITEM_ITEMID_MAIN_SKILL_ALLOPEN						= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 179),	//¸ÞÀÎ½ºÅ³ °³¹æ ¾ÆÀÌÅÛ
	CASHITEM_ITEMID_MAIN_SKILL_BOOSTER						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 180),	//¸ÞÀÎ½ºÅ³ ºÎ½ºÅÍ ¾ÆÀÌÅÛ

	CASHITEM_ITEMID_CAPTAIN_ARMBAND							= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 181),	//2014¿ùµåÄÅ ÀÌº¥Æ® ¾ÆÀÌÅÛ ÁÖÀå¿ÏÀå(EXP130% + Point130% È¿°ú - ÀåÂø¾ÆÀÌÅÛ¾Æ´Ô!!)
	CASHITEM_ITEMID_MEGAPHONE								= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 183),	//È®¼º±â ¾ÆÀÌÅÛ

	CASHITEM_ITEMID_DISGUISE_RANK_APRIL_FOOL				= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 186),	//¸¸¿ìÀý À§Àå°è±Þ
	CASHITEM_ITEMID_DISGUISE_RANK_APRIL_FOOL_CLIENT			= ITEM_ID(ITEM_TYPE_WRAP_PERIOD, CASHITEM_DAY_1, 186),
	CASHITEM_ITEMID_COLOR_MUZZLE_FLASH						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 187),  // Color Fire Muzzle Effect ±â°£Á¦

	CASHITEM_ITEMID_INCREASE_THR2_SLOT						= ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, 191),	// Æ¯¼ö¹«±â ½½·Ô Áõ°¡ ¾ÆÀÌÅÛ

	// ºê¶óÁú ±º¹øÁÙ ¾ÆÀÌÅÛ - »óÁ¡ ¹ÌÆÇ¸Å, ÀÎº¥ Á÷Á¢Áö±Þ
	CASHITEM_ITEMID_BRAZIL_CHAIN_GOLD						= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 666),
	CASHITEM_ITEMID_BRAZIL_CHAIN_BLACK						= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 667),
	CASHITEM_ITEMID_BRAZIL_CHAIN_RED						= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 668),

	CASHITEM_ITEMID_PBWC_COUPON								= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 896),

	CASHITEM_ITEMID_HP_RECOVERY								= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 1051),
	CASHITEM_ITEMID_GRAVITY_RESOTRE							= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 1052),
	CASHITEM_ITEMID_CHARA_UNBEATABLE						= ITEM_ID(ITEM_TYPE_WRAP_COUNT, CASHITEM_DAY_NO, 1053),
};

// »Ì±â »óÁ¡
enum RS_ITEM_GRADE			// ¾ÆÀÌÅÛ µî±Þ
{
	RS_ITEM_GRADE_NOT_SETTING =-1,
	RS_ITEM_GRADE_LOSE		= 0,		// ²Î »óÇ°
	RS_ITEM_GRADE_1			= 1,		// º° 1°³
	RS_ITEM_GRADE_2			= 2,		// º° 2°³ 	
	RS_ITEM_GRADE_3			= 3,		// º° 3°³ 
	RS_ITEM_GRADE_JACKPOT	= 100,		// ´ë¹Ú 
};

enum RS_TAB			// ÅÇ ¹øÈ£.
{
	RS_TAB_1				= 0,		// 1¹ø ÅÇ
	RS_TAB_2				= 1,		// 2¹ø ÅÇ
	RS_TAB_3				= 2,		// 3¹ø ÅÇ
	RS_TAB_MAX				= RS_TAB_3,		// 3¹ø ÅÇ
	RS_TAB_COUNT	
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline UINT32 MAKE_ITEM_ID( UINT8 ui8Type, UINT8 ui8SubType, UINT16 ui16Num, ITEM_EVENT_TYPE eEventType = ITEM_EVENT_TYPE_NONE )
{
	return eEventType * 10000000 + ui8Type * 100000 + ui8SubType * 1000 + ui16Num;
}

namespace Weapon
{
	ITEM_TYPE			GetItemTypeFromClass(WEAPON_CLASS_TYPE WeaponClass);
	WEAPON_SLOT_TYPE	GetUsageFromItemID(UINT32 ui32ItemID);
	WEAPON_SLOT_TYPE	GetUsageFromItemType(INT32 ItemType);

	inline T_ItemID		GetItemID(const UINT32 ui32SubType, const UINT32 ui32Number)
	{
		UINT8 ui8ItemType  = (UINT8)GetItemTypeFromClass( (WEAPON_CLASS_TYPE)ui32SubType  );
		return  MAKE_ITEM_ID( ui8ItemType, (UINT8)ui32SubType, (UINT16)ui32Number );
	}
};

namespace CashItem
{
	CASHITEM_USAGE_TYPE GetUsageFromItemType(INT32 ItemType);
	CASHITEM_USAGE_TYPE GetUsageFromItemID( T_ItemID ItemID );
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Struct
#pragma pack( push, 1)

struct ST_ITEM
{
	T_ItemID				m_TItemID = 0;
	T_ItemArg				m_TItemArg = 0;
};


// ±¸¸ÅÇÒ »óÇ°ÀÇ »ó¼¼ µ¥ÀÌÅÍ
struct BUY_BASKET
{
	UINT32					m_ui32GoodsID = 0;
	UINT8					m_ui8BuyType = 0;
	UINT32					m_ui32CouponID = 0;
	T_ItemDBIdx				m_TCouponIdx = 0;
};

struct EXTEND_BASKET
{
	T_ItemDBIdx	m_TWareDBIdx = 0;		// ±â°£ ¿¬ÀåÇÒ ¾ÆÀÌÅÛÀÇ DB ÀÎµ¦½º(°íÀ¯¹øÈ£)
	BUY_BASKET	m_Basket;				// ¿¬ÀåÇÒ ±â°£¿¡ ´ëÇÑ »ó¼¼ µ¥ÀÌÅÍ(»óÇ°À» ´øÁö¸é ±¸¸Å¿Í µ¿ÀÏÇÏ°Ô Ã³¸®)
};

// ¾ßÀü »óÁ¡
struct Category
{
	wchar_t	m_strCategoryName[MAX_FIELDSHOP_CATEGORYNAMESIZE] = { 0 };

	UINT32	m_ui32GoodsIDList[MAX_FIELDSHOP_GOODSLISTCOUNT][MAX_FIELDSHOP_GOODSPERIODCOUNT] = { 0 };
};

struct 	FieldShopInfo 
{
	// Field_Shop version information
	UINT32	m_ui32Version = 0;

	// Category List
 	Category  m_CategoryList[MAX_FIELDSHOP_CATEGORYCOUNT];  
};

// »Ì±â »óÁ¡

// Type of a category


struct RS_ITEM_INFO	// »Ì±â »óÁ¡ ¾ÆÀÌÅÛ Á¤º¸. Å¬¶óÀÌ¾ðÆ® ¿ë.
{       
	UINT8			_ui8RsIndex = 0;		// ÇØ´ç ÅÇÀÇ µî±Þ¿¡¼­ °íÀ¯ ÀÎµ¦½º. 0~24
	RS_ITEM_GRADE	_eItemGrade = RS_ITEM_GRADE_LOSE;		// µî±Þ. º° °¹¼ö. 0 : ²Î, 1~3 : °¢ µî±Þ
	UINT32			_ui32GoodsID = 0;		// GoodsID

	UINT32 			_ui32MaxUnit = 0;		// ¾ÆÀÌÅÛ ÃÖ´ë ¼ö·®.
	UINT32 			_ui32CurUnit = 0;		// ¾ÆÀÌÅÛ ÇöÀç ¼ö·® .

	void Reset()
	{
		 _ui8RsIndex		= 0;
		 _eItemGrade 		= RS_ITEM_GRADE_LOSE;
		 _ui32GoodsID		= 0;
		 _ui32MaxUnit		= 0;
		 _ui32CurUnit		= 0;
	}
};

struct  RS_HIDDEN	// È÷µç ±¸¼º
{
	RS_ITEM_INFO	_aRSItemInfo[MAX_RS_HIDDEN_ITEM_COUNT]; // ¾ÆÀÌÅÛ Á¤º¸
};

struct  RS_TAB_INFO	 // »Ì±â »óÁ¡ ÅÇ ¾ÆÀÌÅÛ(´çÃ·&²Î) Á¤º¸. Game¼­¹ö - Å¬¶óÀÌ¾ðÆ® °£ »ç¿ë.
{
	UINT8			_ui8TotalItemGrade = 0;	// ±¸¼ºÇÒ¼ö ÀÖ´Â ÃÖ´ë º° °¹¼ö. 3 ~ 10.
	UINT32			_ui32Price = 0;			// ÅÇ °¡°Ý( cash )
	RS_ITEM_INFO	_aRSItemInfoG1[MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT]; // º°1°³ µî±Þ ¾ÆÀÌÅÛ Á¤º¸.
	RS_ITEM_INFO	_aRSItemInfoG2[MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT]; // º°2°³ µî±Þ ¾ÆÀÌÅÛ Á¤º¸.
	RS_ITEM_INFO	_aRSItemInfoG3[MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT]; // º°3°³ µî±Þ ¾ÆÀÌÅÛ Á¤º¸.	
	RS_HIDDEN 		_aRSHidden; // È÷µç ¼¼Æ®
};

struct	RS_RAND_ITEM_INFO
{
	UINT32			m_ui32Roulette[MAX_RAND_COUNT] = { 0 };
	UINT8			m_ui8Idx[MAX_RAND_COUNT] = { 0 };
	RS_ITEM_GRADE	m_eGrade[ MAX_RAND_COUNT ];
};

struct  RS_TAB_INFO_TRANS	// »Ì±â »óÁ¡ ÅÇ Á¤º¸ - Æ®·£½º¼­¹ö°¡ °¡Áö°í ÀÖ´Â°ÍÀ¸·Î ²Î ¸ñ·Ï±îÁö µé°í ÀÖ½À´Ï´Ù.
{
	UINT8			_ui8TotalItemGrade = { 0 };	// ±¸¼ºÇÒ¼ö ÀÖ´Â ÃÖ´ë º° °¹¼ö. 3 ~ 10.
	UINT32			_ui32Price = { 0 };			// ÅÇ °¡°Ý( cash )
	RS_ITEM_INFO	_aRSItemInfoG1[MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT]; // º°1°³ µî±Þ ¾ÆÀÌÅÛ Á¤º¸.
	RS_ITEM_INFO	_aRSItemInfoG2[MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT]; // º°2°³ µî±Þ ¾ÆÀÌÅÛ Á¤º¸.
	RS_ITEM_INFO	_aRSItemInfoG3[MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT]; // º°3°³ µî±Þ ¾ÆÀÌÅÛ Á¤º¸.
	RS_ITEM_INFO	_aRSItemInfoGLot[ 10 ]; // ²Î µî±Þ ¾ÆÀÌÅÛ Á¤º¸.
	RS_HIDDEN 		_aRSHidden; // È÷µç ¼¼Æ®
};

struct  RS_R_ITEM_INFO	// ·ê·¿ÆÇÀ» ±¸¼ºÇÏ´Â ¾ÆÀÌÅÛ Á¤º¸.
{
	RS_ITEM_GRADE	_eItemGrade = RS_ITEM_GRADE_LOSE;		// µî±Þ. º° °¹¼ö. 0 : ²Î, 1~3 : °¢ µî±Þ
	UINT8			_ui8RsIndex = 0xFF;		// ÇØ´ç ÅÇÀÇ µî±Þ¿¡¼­ °íÀ¯ ÀÎµ¦½º. 0~24

	void Reset()
	{
		 _ui8RsIndex		= 0xFF;
		 _eItemGrade 		= RS_ITEM_GRADE_LOSE;
	}
};


struct  RS_ROULETTE_INFO	// ·ê·¿ÆÇ Á¤º¸
{
	RS_TAB			_eTab = RS_TAB_1;				// ÅÇ ¹øÈ£. 1~3.
	UINT8			_ui8ItemCount = 0;		// ·ê·¿ÆÇ¿¡ µé¾î°¡´Â ¾ÆÀÌÅÛ(²ÎÅÛÁ¦¿Ü) °¹¼ö. 
	RS_R_ITEM_INFO	_RSRitemInfo[MAX_RS_ROULETTE_ITEM_COUNT];	

	void Reset()
	{
		_eTab			= RS_TAB_1;
		_ui8ItemCount 	= 0;
		for(INT32 i=0; i<MAX_RS_ROULETTE_ITEM_COUNT ; i++)	
		{
			_RSRitemInfo[i]._eItemGrade = RS_ITEM_GRADE_LOSE;
			_RSRitemInfo[i]._ui8RsIndex	= 0;
		}

	}
};

struct RS_AD_MESSAGE	// ±¤°í ¸Þ½ÃÁö
{
	wchar_t		_strMessage[MAX_RS_AD_MESSAGE_SIZE] = { 0 };
};

struct RS_BASE_INFO	// »Ì±â »óÁ¡ ±âº» Á¤º¸. 
{
	UINT32			_ui32JackPotGoodsID = 0;	// ÀèÆÌ ¾ÆÀÌÅÛ GoodsID.
	wchar_t			_strJackPotNick[NET_NICK_NAME_SIZE] = { 0 }; // °¡ÀåÃÖ±Ù ´ë¹Ú ´çÃ· À¯Àú ´Ð³×ÀÓ
	RS_AD_MESSAGE	_aRSAdMessage[MAX_RS_AD_MESSAGE_COUNT]; //±¤°í ¸Þ½ÃÁö.
};

struct RS_ALL_ITEM_INFO	// »Ì±â »óÁ¡ ÀüÃ¼ ¾ÆÀÌÅÛ Á¤º¸.
{
	RS_TAB_INFO		_aRSTabInfo[MAX_RS_TAB_COUNT];	// 3°³ °íÁ¤.
};

struct RS_JACKPOT_INFO	// »Ì±â »óÁ¡ ÀèÆÌ ´çÃ· Á¤º¸
{
	BOOL	bIsJackPot = FALSE;						// ÀèÆÌ ´çÃ· ¿©ºÎ. ÆÐÅ¶ Àü¼Û ½ÇÆÐ½Ã FALSE.
	wchar_t	strNick[NET_NICK_NAME_SIZE] = { 0 };	// ÀèÆÌ ´çÃ· À¯Àú ´Ð³×ÀÓ 
	UINT32	ui32GoodsID = 0;					// ÀèÆÌ GoodsID
};


#pragma pack( pop )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif


