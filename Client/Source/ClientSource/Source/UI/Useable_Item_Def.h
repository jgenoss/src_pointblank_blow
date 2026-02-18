#pragma once

enum ITEM_QUERY_STYLE
{
	IQS_USE = 0, IQS_REMOVE,
};

//	아이템사용 팝업 타입
enum USE_ITEM_POPUP_TYPE
{
	USE_ITEM_UNKNOWN = -1,
	USE_ITEM_QUERY = 0,				//	사용자 사용 문의
	USE_ITEM_COMPLETE,				//	사용 완료

	USE_ITEM_CHANGE_NICK,			//	닉네임 변경
	USE_ITEM_CHANGE_CLAN_NAME,		//	클랜이름 변경
	USE_ITEM_COLOR_NICK,			//	컬러 닉네임
	USE_ITEM_COLOR_CLAN_NAME,		//	컬러 클랜이름
	USE_ITEM_COLOR_CROSSHAIR,		//	컬러 크로스헤어
	USE_ITEM_DISGUISE_NICK,			//	임시 닉네임
	USE_ITEM_DISGUISE_RANK,			//	위장 계급
	USE_ITEM_CHANGE_CLAN_MARK,		// 클랜 마크 변경
	USE_ITEM_COLOR_FIRE_MUZZLE,	// 컬러 총구 화염 변경권

	USE_ITEM_POPUP_MAX
};

enum ITEM_USE_METHOD
{
	USE_METHOD_UNKNOWN = -1,

	USE_METHOD_DEFAULT_AUTH,		//DefaultAuth,
	USE_METHOD_DISGUISE_NICK,		//DisguiseNick,
	USE_METHOD_DISGUISE_RANK,		//DisguiseRank,
	USE_METHOD_COLOR_NICK,			//ColorNick,
	USE_METHOD_COLOR_CLAN,			//ColorClan,
	USE_METHOD_COLOR_CROSS_HAIR,	//ColorCrossHair,
	USE_METHOD_CHANGE_NICK,			//ChangeNick,
	USE_METHOD_CHANGE_CLAN_NAME,	//ChangeClanName,
	USE_METHOD_CHANGE_CLAN_MARK,	//ChangeClanMark,
	USE_METHOD_BATTLE_FIELD_MOVE,	//BattleFieldMove,
	USE_METHOD_COLOR_FIRE_MUZZLE,	//ColorFireMuzzle

	MAX_USE_METHOD
};

enum ITEM_RESULT_METHOD
{
	RESULT_METHOD_UNKNOWN = -1,

	RESULT_METHOD_COLOR_NICK,		//ColorNick,
	RESULT_METHOD_COLOR_CLAN,		//ColorClan,
	RESULT_METHOD_NICK,				//Nick,
	RESULT_METHOD_RANK,				//Rank,
	RESULT_METHOD_COLOR_CROSS_HAIR,	//ColorCrossHair
	RESULT_METHOD_CLAN_NAME,		//ClanName,
	RESULT_METHOD_CLAN_MARK,		//ClanMark,
	RESULT_METHOD_CLAN_PERSON_PLUS,	//ClanPersonPlus
	RESULT_METHOD_GRAB_BAG,			//GrabBag,
	RESULT_METHOD_POINT,			//Point,
	RESULT_METHOD_COLOR_FIRE_MUZZLE, // ColorFireMuzzle

	MAX_RESULT_METHOD
};

namespace use_item
{
	enum UseType
	{
		ut_None= 0,

		ut_OtherView, //상대방 정보 보기

		vut_Duplicate,	//중복 사용할 수 없다.
	};

	ITEM_USE_METHOD UseMethodStringToEnum( T_ItemID item_id);
	ITEM_RESULT_METHOD ResultMethodStringToEnum( T_ItemID item_id);

}

