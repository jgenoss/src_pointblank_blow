#include "pch.h"
#include "QuestDef.h"

static const char * g_StrWeaponClassPostfix_Use[WEAPON_CLASS_COUNT] = 
{
	"NONE",

	"로",	//WEAPON_CLASS_KNIFE
	"로",	//WEAPON_CLASS_HANDGUN
	"으로",	//WEAPON_CLASS_ASSAULT
	"으로",	//WEAPON_CLASS_SMG
	"으로",	//WEAPON_CLASS_SNIPER
	"으로",	//WEAPON_CLASS_SHOTGUN
	"로",	//WEAPON_CLASS_THROWING1
	"로",	//WEAPON_CLASS_THROWING2
	"으로",	//WEAPON_CLASS_MISSION
	"으로",	//WEAPON_CLASS_MG

	"으로",	//WEAPON_CLASS_GRENADESHELL
	"로",	//WEAPON_CLASS_BOMBTRIGGER

	"로",	//WEAPON_CLASS_CIC
	"로",	//WEAPON_CLASS_DUALHANDGUN
	"로",	//WEAPON_CLASS_DUALKNIFE

	"로",	//WEAPON_CLASS_ROCKET_LAUNCHER
	"로",	//WEAPON_CLASS_OBJECT_GUN

	"으로",	//WEAPON_CLASS_DUALSMG

	"",	//WEAPON_CLASS_DINO
	"",	//WEAPON_CLASS_DINO_LEFT_SCRATCH
	"",	//WEAPON_CLASS_TREX
	"",	//WEAPON_CLASS_STING
	"",	//WEAPON_CLASS_KNUCKLE
	"",	//WEAPON_CLASS_DINO_RIGHT_SCRATCH
	"",	//WEAPON_CLASS_DINO_LEFT_SCRATCH
	
};

static const char * g_StrWeaponClassPostfix_Do[WEAPON_CLASS_COUNT] = 
{
	"NONE",

	"를",	//WEAPON_CLASS_KNIFE
	"를",	//WEAPON_CLASS_HANDGUN
	"을",	//WEAPON_CLASS_ASSAULT
	"을",	//WEAPON_CLASS_SMG
	"을",	//WEAPON_CLASS_SNIPER
	"을",	//WEAPON_CLASS_SHOTGUN
	"를",	//WEAPON_CLASS_THROWING1
	"를",	//WEAPON_CLASS_THROWING2
	"을",	//WEAPON_CLASS_MISSION
	"을",	//WEAPON_CLASS_MG

	"을",	//WEAPON_CLASS_GRENADESHELL
	"를",	//WEAPON_CLASS_BOMBTRIGGER

	"를",	//WEAPON_CLASS_CIC
	"를",	//WEAPON_CLASS_DUALHANDGUN
	"를",	//WEAPON_CLASS_DUALKNIFE

	"를",	//WEAPON_CLASS_ROCKET_LAUNCHER
	"를",	//WEAPON_CLASS_OBJECT_GUN

	"을",	//WEAPON_CLASS_DUALSMG

	"",	//WEAPON_CLASS_DINO
	"",	//WEAPON_CLASS_DINO_LEFT_SCRATCH
	"",	//WEAPON_CLASS_TREX
	"",	//WEAPON_CLASS_STING
	"",	//WEAPON_CLASS_KNUCKLE
};

STAGE_ID getQuestStageID(UINT8 QuestStageID)
{
	return QuestStageID;
}

const QUEST_INFO g_pQuestInfoTable[USE_QUEST_COUNT] = 
{
	{QUEST_COLUM_NONE,		QUEST_COLUM_NONE,	QUEST_COLUM_NONE,	QUEST_COLUM_NONE,			QUEST_COLUM_NONE,		QUEST_COLUM_NONE}, 
	{QUEST_COLUM_ACTION,	QUEST_COLUM_STAGE,	QUEST_COLUM_COUNT,	QUEST_COLUM_ACC,			QUEST_COLUM_TEAM,		QUEST_COLUM_SAVECOUNT}, 
	{QUEST_COLUM_ACTION,	QUEST_COLUM_STAGE,	QUEST_COLUM_COUNT,	QUEST_COLUM_WEAPON_CLASS,	QUEST_COLUM_WEAPON_IDX,	QUEST_COLUM_SAVECOUNT}, 
	{QUEST_COLUM_ACTION,	QUEST_COLUM_STAGE,	QUEST_COLUM_COUNT,	QUEST_COLUM_WEAPON_CLASS,	QUEST_COLUM_WEAPON_IDX,	QUEST_COLUM_SAVECOUNT}, 
};

const char * getWeaponClassPostfix_Do( UINT32 WeaponClassIdx)
{
	if(WeaponClassIdx > WEAPON_CLASS_COUNT - 1)I3ASSERT_0;

	return g_StrWeaponClassPostfix_Do[WeaponClassIdx];
}

const char * getWeaponClassPostfix_Use( UINT32 WeaponClassIdx)
{
	if(WeaponClassIdx > WEAPON_CLASS_COUNT - 1)I3ASSERT_0;

	return g_StrWeaponClassPostfix_Use[WeaponClassIdx];
}


INT32 QuestFileLoadEx(const char * strFileName, QUEST_CTX * pCard, QUEST_ITEM_INFO_NEW2 * pGetItem )
{
	// 2011.4.7 작성자 서동권
	// 기존 구버전 구조와 신버전 구조 모두 로드 하도록 하였고 앞으로 신버전만 사용하기 위해서 로드와 동시에 신버전 구조체로 변경하여 작업을 진행하도록 하였습니다.
	// 해당 함수로 파일을 열었을 경우 구버전으로 작성된 퀘스트 파일이라도 따로 서버에서 ReVersion 할 필요가 없습니다. 무조건 신버전으로 읽습니다. QUEST_ITEM_INFO_NEW2 구조체 사용
	// 카드셋 마다 완료시 최종 보상이 추가 되어 최종 보상에 관한 변수도 넘겨주어야 합니다.

	// 2011.4.27 클라이언트 팩파일도 사용할수 있도록 수정하였습니다. - 서동권 -
	
	INT32 Len;
	i3FileStream	kStringStream;
	if( !kStringStream.Open(strFileName, STREAM_READ | STREAM_SHAREREAD) )
		return 0x80000001; //파일 없음

	QUEST_FILE_HEADER FileHeader; 

	//Read FileHeader
	Len = sizeof(QUEST_FILE_HEADER);  
	kStringStream.Read(&FileHeader, Len); 

	//Check File Header 
	if((FileHeader._Signature[0] != 'q')|| (FileHeader._Signature[1] != 'f') )//파일 헤더 에러 
	{
		return 0x80000002;
	}

	if(FileHeader._Version != 1 && FileHeader._Version != 2 ) // 1 = 구버전 , 2 = 신버전
	{
		return 0x80000003;
	}

	//Read Challenge 
	if( FileHeader._Version == 1 ) // 구버전 로드
	{
		
		QUEST_CTX	pQuestOld[MAX_QUEST_PER_CARDSET];	 // 구버전을 로드하기 위해 생성한 임시 변수

		Len = sizeof(QUEST_CTX) * MAX_QUEST_PER_CARDSET;
		kStringStream.Read(pQuestOld,Len);

		for( INT32 i = 0 ; i < MAX_CARD_PER_CARDSET ; i++ )
		{
			for( INT32 j = 0 ; j < 16 ; j += 4 )
			{
				memcpy( pCard, &pQuestOld[(i*16)+j], sizeof( QUEST_CTX ) );
				++pCard;
			}
		}


		QUEST_ITEM_INFO	OldItemInfo[MAX_CARD_PER_CARDSET]; // 구버전을 로드하기 위해 생성한 임시 변수

		Len = sizeof(QUEST_ITEM_INFO) * MAX_CARD_PER_CARDSET;
		kStringStream.Read(OldItemInfo,Len);

		for( int i = 0; i < MAX_CARD_PER_CARDSET; ++i ) // 구버전의 정보를 신버전의 구조체 맴버변수로 입력 합니다.
		{
			pGetItem->m_CardReward[i].m_ui32Point = OldItemInfo[i].m_ColPoint[0] * 10; // 신버전은 구버전과 다르게 포인트와 경험치를 원상태 그대로 가지고 있도록 변경 되었습니다. 그래서 x 10
			pGetItem->m_CardReward[i].m_ui32Exp = OldItemInfo[i].m_ColExp[0] * 10;	 // 구버전의 m_ColPoint 는 배열 개수가 4개지만 0번째만 사용하기 때문에 신버전에서는 배열을 사용하지 않고 0번째의 값만 받아오게 하였다.
			pGetItem->m_CardReward[i].m_ui32Prize = OldItemInfo[i].m_ColPrize[0];

			for( int n = 0; n < MAX_QUEST_ITEM_CARD; ++n)
			{
				pGetItem->m_CardReward[i].m_Item[n].m_ui32ItemID = 0; // 구버전의 경우 무기의 정보가 없으므로 0으로 초기화만 해줍니다.
				pGetItem->m_CardReward[i].m_Item[n].m_i32Editor_index = 0;
				pGetItem->m_CardReward[i].m_Item[n].m_ui32Arg = 0;
			}
		}

		pGetItem->m_CompleteReward.m_ui32Exp = 0;
		pGetItem->m_CompleteReward.m_ui32Point = 0;
		pGetItem->m_CompleteReward.m_ui32Prize = 0;

		for( int i = 0; i < MAX_QUEST_ITEM_CARD; ++i )
		{
			pGetItem->m_CompleteReward.m_Item[i].m_i32Editor_index = 0;
			pGetItem->m_CompleteReward.m_Item[i].m_ui32ItemID = 0;
			pGetItem->m_CompleteReward.m_Item[i].m_ui32Arg = 0;
		}
	}
	else if( FileHeader._Version == 2 ) // 신버전 로드
	{

		Len = sizeof(QUEST_CTX) * (MAX_CARD_PER_CARDSET*MAX_CARDSET_PER_USER);
		kStringStream.Read(pCard,Len);

		Len = sizeof(QUEST_ITEM_INFO_NEW2);
		kStringStream.Read(pGetItem,Len);
	}

	return FileHeader._Version;
}

BOOL QuestFileSaveEx(const char * strFileName, QUEST_CTX * pCard, QUEST_ITEM_INFO_NEW2 * pGetItem )
{
	// 2011.4.7 작성자 서동권
	// 반드시 QuestFileLoadEx() 함수로 파일을 열고서 사용해야 합니다.Ex 가 아닌 QuestFileLoad() 함수로 파일을 열었을 시 해당 함수로 저장하지 말고 기존 QuestFileSave()를 이용하세요.

	HANDLE hFile; 
	INT32 Len; 
	hFile = ::CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );	

	QUEST_FILE_HEADER FileHeader; 
	FileHeader._Version = 2; 
	FileHeader._Signature[0] = 'q'; 
	FileHeader._Signature[1] = 'f'; 

	//Save File Header 
	Len = sizeof(QUEST_FILE_HEADER);  
	::WriteFile(hFile, &FileHeader, Len, (LPDWORD)&Len, NULL); 

	//Save File Data 
	{
		Len = sizeof(QUEST_CTX) * (MAX_CARD_PER_CARDSET*MAX_CARDSET_PER_USER);
		::WriteFile(hFile, pCard, Len, (LPDWORD)&Len, NULL); 		
	}

	{
		//Save Get Last Item
		Len = sizeof(QUEST_ITEM_INFO_NEW2);
		::WriteFile(hFile, pGetItem, Len, (LPDWORD)&Len, NULL);
	}

	::CloseHandle( hFile );
	return TRUE;
}




// 밑에 구현된 함수는 기존에 사용하던 구버전 함수 입니다. ( 기존 카드 한장에 임무 16개가 빙고 형식으로 구현된 퀘스트 파일을 읽어서 Reversion 하여 사용하는 형식 입니다. ) //

INT32 QuestFileLoad(const char * strFileName, QUEST_CTX * pCard, QUEST_ITEM_INFO * pGetItem)
{
	INT32 Len; 
	HANDLE hFile = ::CreateFile(strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if(hFile == INVALID_HANDLE_VALUE)return 0x80000001; //파일 없음

	QUEST_FILE_HEADER FileHeader; 

	//Read FileHeader
	Len = sizeof(QUEST_FILE_HEADER);  
	::ReadFile(hFile, &FileHeader, Len, (LPDWORD)&Len, NULL); 

	//Check File Header 
	if((FileHeader._Signature[0] != 'q')|| (FileHeader._Signature[1] != 'f') )//파일 헤더 에러 
	{
		::CloseHandle( hFile );
		return 0x80000002;
	}

	if(FileHeader._Version != 1)//지금은 버전은 하나만 있다. 
	{
		::CloseHandle( hFile );
		return 0x80000003;
	}

	//Read Challenge 
	//필요한 데이터만 로드합니다. 
	{		
		Len = sizeof(QUEST_CTX) * MAX_QUEST_PER_CARDSET;
		::ReadFile(hFile, pCard, Len, (LPDWORD)&Len, NULL); 		
	}

	{
		//Read Get Item 
		Len = sizeof(QUEST_ITEM_INFO) * MAX_CARD_PER_CARDSET;
		::ReadFile(hFile, pGetItem, Len, (LPDWORD)&Len, NULL); 
	}

	::CloseHandle( hFile );
	return FileHeader._Version; 
}

BOOL QuestFileSave(const char * strFileName, QUEST_CTX * pCard, QUEST_ITEM_INFO * pGetItem)
{
	HANDLE hFile; 
	INT32 Len; 
	hFile = ::CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );	

	QUEST_FILE_HEADER FileHeader; 
	FileHeader._Version = 1; 
	FileHeader._Signature[0] = 'q'; 
	FileHeader._Signature[1] = 'f'; 

	//Save File Header 
	Len = sizeof(QUEST_FILE_HEADER);  
	::WriteFile(hFile, &FileHeader, Len, (LPDWORD)&Len, NULL); 

	//Save File Data 
	{
		Len = sizeof(QUEST_CTX) * MAX_QUEST_PER_CARDSET;
		::WriteFile(hFile, pCard, Len, (LPDWORD)&Len, NULL); 		
	}

	{
		//Read Get Item 
		Len = sizeof(QUEST_ITEM_INFO) * MAX_CARD_PER_CARDSET;
		::WriteFile(hFile, pGetItem, Len, (LPDWORD)&Len, NULL); 
	}

	::CloseHandle( hFile );
	return TRUE;
}