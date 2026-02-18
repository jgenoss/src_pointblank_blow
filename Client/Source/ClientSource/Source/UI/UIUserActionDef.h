/**
 * \file	UI\UIUserActionDef.h
 *
 * Declares the user interface user action def class.
 */


#if !defined( __UI_USERACTION_DEF_H__)
#define __UI_USERACTION_DEF_H__

#define	I3_EVT_USER_ACTION	(0x00100000 | I3_EVT_USER)

/** \brief 유저액션 팝업의 아이콘
	\note 순서 중요합니다. Resource 번호와 매치됨. */
enum USER_CONTEXT_MENU
{
	UCM_NONE = -1,
	UCM_PROFILE,					// 상세 정보 보기
	UCM_ADD_FRIEND,					// 친구 추가
	UCM_CLAN_INVITE,				// Clan 초대
	UCM_SEND_MAIL,					// 쪽지 보내기
	UCM_ADD_BLOCK,					// 차단하기
	UCM_USER_REPORT,				// 신고하기
	UCM_FIND_AWAY,					// 찾아가기/같이하기
	UCM_SEND_PRESENT,				// 선물 보내기
	UCM_REMOVE_FRIEND,				// 친구 삭제

	// 이하는 아이콘 Resource가 없음.....응?
	UCM_REMOVE_BLOCK,				// 차단 삭제
	UCM_KICK,						// 강퇴하기 (Only ReadyRoom)
	UCM_WHISPER,					// 귓말하기
	UCM_INVITE,						// 초대하기
	UCM_ACCEPT_FRIEND,				// 친구 수락
	UCM_REFUSE_FRIEND,				// 친구 거절
	UCM_TRANSFER_HOST,				// 방장 넘기기	
	UCM_VIEW_OTHER_USER_ITEM_INFO,	// 상대방 아이템 보기
	UCM_GMOPTION,					// GM옵션 보기

	UCM_MAX
};

#endif
