#ifndef __COMMONDEF_CONTENTS_H__
#define __COMMONDEF_CONTENTS_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum
// 호칭 적용 계열
enum DESIGNATION_APPLY_TYPE
{
	DESIGNATION_APPLY_TYPE_ALL = 0,		// 전체 계열
	DESIGNATION_APPLY_TYPE_ASSULT,		// 어설트라이플 계열
	DESIGNATION_APPLY_TYPE_RIFLE,		// 저격소총 계열
	DESIGNATION_APPLY_TYPE_SMG,			// SMG(기관단총)계열
	DESIGNATION_APPLY_TYPE_MELLE,		// 근접무기 계열
	DESIGNATION_APPLY_TYPE_HANDGUN,		// 핸드건 계열
	DESIGNATION_APPLY_TYPE_SHOTGUN,		// 샷건 계열
	DESIGNATION_APPLY_TYPE_BOMB,		// 폭탄 계열

	DESIGNATION_APPLY_TYPE_COUNT,
};

// 호칭 타입
enum DESIGNATION_TYPE
{
	DESIGNATION_TYPE_NONE = 0,
	DESIGNATION_TYPE_1,			// 최 하위 단계 공통 호칭 A
	DESIGNATION_TYPE_2,			// 최 하위 단계 공통 호칭 B
	DESIGNATION_TYPE_3,			// 최 하위 단계 공통 호칭 C
	DESIGNATION_TYPE_4,			// 최 하위 단계 공통 호칭 D

	DESIGNATION_TYPE_5,			// 1차 선택 단계 호칭 A
	DESIGNATION_TYPE_6,			// 1차 선택 단계 호칭 B
	DESIGNATION_TYPE_7,			// 1차 선택 단계 호칭 C

	DESIGNATION_TYPE_8,			// 2차 선택 단계 호칭 A (Assault Rifle)
	DESIGNATION_TYPE_9,			// 2차 선택 단계 호칭 B (Assault Rifle)
	DESIGNATION_TYPE_10,		// 2차 선택 단계 호칭 C (Assault Rifle)
	DESIGNATION_TYPE_11,		// 2차 선택 단계 호칭 D (Assault Rifle)
	DESIGNATION_TYPE_12,		// 마스터 (Assault Rifle)
	DESIGNATION_TYPE_13,		// 히어로 (Assault Rifle)

	DESIGNATION_TYPE_14,		// 2차 선택 단계 호칭 A (Rifle)
	DESIGNATION_TYPE_15,		// 2차 선택 단계 호칭 B (Rifle)
	DESIGNATION_TYPE_16,		// 2차 선택 단계 호칭 C (Rifle)
	DESIGNATION_TYPE_17,		// 2차 선택 단계 호칭 D (Rifle)
	DESIGNATION_TYPE_18,		// 마스터 (Rifle)
	DESIGNATION_TYPE_19,		// 히어로 (Rifle)

	DESIGNATION_TYPE_20,		// 2차 선택 단계 호칭 A (SMG)
	DESIGNATION_TYPE_21,		// 2차 선택 단계 호칭 B (SMG)
	DESIGNATION_TYPE_22,		// 2차 선택 단계 호칭 C (SMG)
	DESIGNATION_TYPE_23,		// 2차 선택 단계 호칭 D (SMG)
	DESIGNATION_TYPE_24,		// 마스터 (SMG)
	DESIGNATION_TYPE_25,		// 히어로 (SMG)

	DESIGNATION_TYPE_26,		// 2차 선택 단계 호칭 A (Knife)
	DESIGNATION_TYPE_27,		// 2차 선택 단계 호칭 B (Knife)
	DESIGNATION_TYPE_28,		// 2차 선택 단계 호칭 C (Knife)
	DESIGNATION_TYPE_29,		// 마스터 (Knife)

	DESIGNATION_TYPE_30,		// 2차 선택 단계 호칭 A (Hand Gun)
	DESIGNATION_TYPE_31,		// 2차 선택 단계 호칭 B (Hand Gun)
	DESIGNATION_TYPE_32,		// 2차 선택 단계 호칭 C (Hand Gun)
	DESIGNATION_TYPE_33,		// 마스터 (Hand Gun)
	DESIGNATION_TYPE_34,		// 히어로 (Hand Gun)

	DESIGNATION_TYPE_35,		// 2차 선택 단계 호칭 A (Shot Gun)
	DESIGNATION_TYPE_36,		// 2차 선택 단계 호칭 B (Shot Gun)
	DESIGNATION_TYPE_37,		// 2차 선택 단계 호칭 C (Shot Gun)
	DESIGNATION_TYPE_38,		// 마스터 (Shot Gun)
	DESIGNATION_TYPE_39,		// 히어로 (Shot Gun)

	DESIGNATION_TYPE_40,		// 2차 선택 단계 호칭 A (Composition)
	DESIGNATION_TYPE_41,		// 2차 선택 단계 호칭 B (Composition)
	DESIGNATION_TYPE_42,		// 2차 선택 단계 호칭 C (Composition)
	DESIGNATION_TYPE_43,		// 2차 선택 단계 호칭 D (Composition)
	DESIGNATION_TYPE_44,		// 마스터 (Composition)

	DESIGNATION_TYPE_MAX
};

enum GUIDE_EVENT			
{
	//*중요 모든 Guide Event의 갯수가 64개를 초과하면, m_ui64Guide 변수가 하나 더 필요합니다.

	// /*서버*/표시된 항목은 서버에서 해당 이벤트 처리를 위해 별도로 체크 또는 값을 줘야함.

	// 캐릭터 생성후.
	GUIDE_EVENT_VIEW_ROOM		= 0,	//  Event1 - 입장 가능한 방 보기
	GUIDE_EVENT_TOP_ROOM,				//  Event2 - 방목록 최상위 방
	GUIDE_EVENT_ENTER_ROOM,				//  Event3 - 입장

	//  세번째 배틀 정상 종료후.
	GUIDE_EVENT_CREATE_ROOM,			//  Event1 - 방만들기
	GUIDE_EVENT_CREATE_ROOM_OK,			//  Event2 - 확인

/*서버*/	// 3번 로그인후, 주무기가 2개 이상 소유중인 상태. 
	GUIDE_EVENT_SIDE_EQUIP,				//  Event1 - 사이드 메뉴 - EQUIP
	GUIDE_EVENT_1ST_P_WEAPON,			//  Event2 - 주무기
	GUIDE_EVENT_2ND_P_WEAPON,			//  Event3 - 두번째 주무기
	GUIDE_EVENT_SIDE_EQUIP_OK,			//  Event4 - 확인


	//  2번째 캐릭터 구매 완료시.
	GUIDE_EVENT_TOP_MYINFO_NICK_CHANGE,	//  Event1 - 탑메뉴 - MYINFO
	GUIDE_EVENT_1ST_CHARA_NICK_CHANGE,	//  Event2 - 첫번째 캐릭터 닉네임 교체

	// 2번째 캐릭터 구매 후.
	GUIDE_EVENT_CHARA_VIEW_RIGHT,		//  Event1 - 캐릭터 뷰에 오른쪽 방향 화살표
	GUIDE_EVENT_CHARA_VIEW_LEFT,		//  Event2 - 캐릭터 뷰에 왼쪽 방향 화살표

	// 이등병 진급후.
	GUIDE_EVENT_CREATE_ROOM_RANK1,		//  Event1 - 방만들기
	GUIDE_EVENT_MODE_BOMB,				//  Event2 - 모드에 폭파미션.
	GUIDE_EVENT_MODE_BOMB_OK,			//  Event3 - 확인.

	// 캐릭터 생성후 유저리스트에 로비 누른 후.
	GUIDE_EVENT_LOBBY_TOP_USER,			//  Event1 - 로비 접속자 맨 위 유저
	GUIDE_EVENT_ADD_FRIEND,				//  Event2 - 유저 팝업 내 친구 추가
	GUIDE_EVENT_ADD_FRIEND_CLOSE,		//  Event3 - 닫기

	// 하사 진급후.
	GUIDE_EVENT_TOP_CLAN,				//  Event1 - 탑메뉴 - 클랜
	GUIDE_EVENT_SELECT_CLAN,			//  Event2 - 클랜 선택
	GUIDE_EVENT_JOIN_CLAN,				//  Event3 - 가입

	// 캐릭터 생성 후 쪽지함 열경우.
	GUIDE_EVENT_SEND_MESSAGE,			//  Event1 - 쪽지 보내기
	GUIDE_EVENT_SEND_MESSAGE_OK,		//  Event2 - 보내기

	// 쪽지함에 선물이 최초 지급 된 후.
	GUIDE_EVENT_MESSAGE_BOX,			//  Event1 - 쪽지함
	GUIDE_EVENT_GIFT_BOX,				//  Event2 - 선물함
	GUIDE_EVENT_TOP_MESSAGE,			//  Event3 - 맨위쪽지
	GUIDE_EVENT_RECV_GIFT,				//  Event4 - 선물받기

	// 첫 배틀 정상 종료 후.
	GUIDE_EVENT_TOP_SHOP_BUY,			//  Event1 - 탑 메뉴 - 상점
	GUIDE_EVENT_M4A1EXT,				//  Event2 - M4A1 Ext.
	GUIDE_EVENT_BUY_M4A1EXT,			//  Event3 - 구입하기.
	GUIDE_EVENT_BUY_M4A1EXT_OK,			//  Event4 - 확인.

	// 배틀 플레이 시간 1시간 후 첫 스킬이 개방 되었을시.
	GUIDE_EVENT_TOP_MYINFO_SKILL,		//  Event1 - 탑메뉴 - MYINFO
	GUIDE_EVENT_TAP_SKILL,				//  Event2 - 스킬 탭.

	// 병장 진급.
	GUIDE_EVENT_TOP_SHOP_CHARA_BUY,		//  Event1 - 탑 메뉴 - 상점
	GUIDE_EVENT_CATEGORY_CHARA,			//  Event2 - 캐릭터 카테고리 

	// 퀘스트 완료후, 첫 보상 지급 시점.
	GUIDE_EVENT_TOP_MYINFO_QUEST,		//  Event1 - 탑 메뉴  - MYINFO
	GUIDE_EVENT_TAP_QUEST,				//  Event2 - 퀘스트 탭
	GUIDE_EVENT_1ST_MEDAL,				//  Event3 - 1열1행 메달
	GUIDE_EVENT_MEDAL_LV1,				//  Event4 - Lv1


	// 일병 진급 후 방 생성시
	GUIDE_EVENT_CHANGE_STAGE,			//  Event1 - 스테이지 변경 버튼
		
	GUIDE_EVENT_MAX,
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function
// Guide 보상 Point  
const UINT32 g_aui32GuidePoint[GUIDE_EVENT_MAX] = 
{	 
	// 캐릭터 생성후.
	50, 		//  Event1 - 입장 가능한 방 보기
	50,			//  Event2 - 방목록 최상위 방
	100, 		//  Event3 - 입장

	//  세번째 배틀 정상 종료후.
	50,			//  Event1 - 방만들기
	50,			//  Event2 - 확인

	// 3번 로그인후, 주무기가 2개 이상 소유중인 상태. 
	50,			//  Event1 - 사이드 메뉴 - EQUIP
	50,			//  Event2 - 주무기
	100,		//  Event3 - 두번째 주무기
	50,			//  Event4 - 확인


	//  2번째 캐릭터 구매 완료시.
	50,			//  Event1 - 탑메뉴 - MYINFO
	50,			//  Event2 - 첫번째 캐릭터 닉네임 교체
	
	// 2번째 캐릭터 구매 후.
	50,			//  Event1 - 캐릭터 뷰에 오른쪽 방향 화살표
	50,			//  Event2 - 캐릭터 뷰에 왼쪽 방향 화살표

	// 이등병 진급후.
	100,		//  Event1 - 방만들기
	100,			//  Event2 - 모드에 폭파미션.
	100,		//  Event3 - 확인.

	// 캐릭터 생성후 유저리스트에 로비 누른 후.
	100,		//  Event1 - 로비 접속자 맨 위 유저
	100,		//  Event2 - 유저 팝업 내 친구 추가
	300,		//  Event3 - 닫기

	// 하사 진급후.
	50,			//  Event1 - 탑메뉴 - 클랜
	100,		//  Event2 - 클랜 선택
	300,		//  Event3 - 가입

	// 캐릭터 생성 후 쪽지함 열경우.
	50,			//  Event1 - 쪽지 보내기
	50,			//  Event2 - 보내기

	// 쪽지함에 선물이 최초 지급 된 후.
	50,			//  Event1 - 쪽지함
	50,			//  Event2 - 선물함
	50,			//  Event3 - 맨위쪽지
	50,			//  Event4 - 선물받기

	// 첫 배틀 정상 종료 후.
	100,		//  Event1 - 탑 메뉴 - 상점
	100,		//  Event2 - M4A1 Ext.
	100,		//  Event3 - 구입하기.
	100,		//  Event4 - 확인.

	// 배틀 플레이 시간 1시간 후 첫 스킬이 개방 되었을시.
	100,		//  Event1 - 탑메뉴 - MYINFO
	100,		//  Event2 - 스킬 탭.

	// 병장 진급.
	100,		//  Event1 - 탑 메뉴 - 상점~
	100,		//  Event2 - 캐릭터 카테고리 

	// 퀘스트 완료후, 첫 보상 지급 시점.~
	100,		//  Event1 - 탑 메뉴  - MYINFO
	100,		//  Event2 - 퀘스트 탭
	100,		//  Event3 - 1열1행 메달
	100,		//  Event4 - Lv1


	// 일병 진급 후 방 생성시
	50			//  Event1 - 스테이지 변경 버튼
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Struct
#pragma pack( push, 1)


#pragma pack( pop )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif